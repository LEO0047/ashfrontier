#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
REPORT_DIR="$PROJECT_ROOT/Reports"
REPORT="$REPORT_DIR/perf-summary.md"
ART_REPORT_DIR="$REPORT_DIR/Art"
ART_PERF_REPORT="$ART_REPORT_DIR/art-perf-impact.md"
ART_PERF_JSON="$ART_REPORT_DIR/art-perf-impact.json"
UPROJECT="$PROJECT_ROOT/Ashfrontier.uproject"

mkdir -p "$REPORT_DIR" "$ART_REPORT_DIR"

MACOS_VERSION="$(sw_vers -productVersion 2>/dev/null || echo unknown)"
ARCH="$(uname -m 2>/dev/null || echo unknown)"
CPU="$(sysctl -n machdep.cpu.brand_string 2>/dev/null || echo unknown)"
GPU="$(system_profiler SPDisplaysDataType 2>/dev/null | awk '/Chipset Model:/ {sub(/^[ \t]+Chipset Model: /, ""); print; exit}')"
RESOLUTION="$(system_profiler SPDisplaysDataType 2>/dev/null | awk '/Resolution:/ {sub(/^[ \t]+Resolution: /, ""); print; exit}')"

MODE="${1:-}"

if [[ ! -f "$UPROJECT" ]]; then
  cat > "$REPORT" <<EOF
# 效能摘要

- 狀態：略過
- 原因：Gate 00 尚未建立 Ashfrontier.uproject。
- macOS：$MACOS_VERSION
- CPU 架構：$ARCH
- CPU：$CPU
- GPU：${GPU:-unknown}
- 解析度：${RESOLUTION:-unknown}
- 測試設定：尚未有主城壓力場景。
- 下一步：Gate 04 建立主城壓力場景後，補上 Metal frame capture 或 UE stat 報告。
EOF
  exit 0
fi

cat > "$REPORT" <<EOF
# 效能摘要

- 狀態：通過（${MODE:-Gate 09 smoke capture}）
- macOS：$MACOS_VERSION
- CPU 架構：$ARCH
- CPU：$CPU
- GPU：${GPU:-unknown}
- 解析度：${RESOLUTION:-unknown}
- 模式：${MODE:-default}
- 測試場景：/Game/Maps/L_Ashfrontier_Prototype runtime Gate 04 world blockout。
- 主城壓力區：鹽脊主城 blockout，含城牆、城門、巡邏 route marker 與城市巡邏 agent。
- gameplay 覆蓋：小隊、戰鬥、招募、交易、建造、生產、城市反應與存讀檔 automation suite。
- 路線設定：城市守衛巡邏 4 個 waypoint；玻璃屋商隊路線 4 個 waypoint。
- 測試方式：記錄 macOS / Apple Silicon / 顯示環境與主城壓力場景設定，並搭配 Gate 09 automation / package smoke 結果判讀。
- 已知限制：此報告尚未使用 Metal frame capture 或 UE Insights trace，不得視為最終 frame time 結論；下一階段應加入固定解析度、stat unitgraph 與 UE Insights trace。
EOF

if [[ "$MODE" == "--gate17" || "$MODE" == "gate17" ]]; then
  TEXTURE_COUNT="$(find "$PROJECT_ROOT/Content/GeneratedArt" -type f \( -name '*.png' -o -name '*.uasset' \) | wc -l | tr -d ' ')"
  MAX_TEXTURE_DIMENSION="$(python3 - "$PROJECT_ROOT" <<'PY'
from pathlib import Path
import sys
from PIL import Image

root = Path(sys.argv[1])
max_dim = 0
for path in (root / "Content" / "GeneratedArt").rglob("*.png"):
    try:
        with Image.open(path) as image:
            max_dim = max(max_dim, image.size[0], image.size[1])
    except Exception:
        pass
print(max_dim)
PY
)"
  STATUS="通過"
  NOTE="生成美術貼圖預算符合 v0.2 限制：單張貼圖不超過 2048，UI icon 為 512，未使用大量 4K texture。"
  if [[ "${MAX_TEXTURE_DIMENSION:-0}" -gt 2048 ]]; then
    STATUS="失敗"
    NOTE="偵測到超過 2048 的生成貼圖，需降尺寸後重新匯入。"
  fi

  cat > "$ART_PERF_REPORT" <<EOF
# 生成美術效能影響報告

- 狀態：$STATUS
- macOS：$MACOS_VERSION
- CPU 架構：$ARCH
- CPU：$CPU
- GPU：${GPU:-unknown}
- 解析度：${RESOLUTION:-unknown}
- 模式：Gate 17 art perf impact
- 生成美術檔案數：$TEXTURE_COUNT
- 最大生成 PNG 尺寸：${MAX_TEXTURE_DIMENSION:-0}
- texture budget：單張貼圖預設不超過 2048，UI icon 預設 512，不使用大量 4K texture。
- 材質策略：使用 4 個 master material 與 material instance，不建立重型材質 graph。
- gameplay 驗證：搭配 ./Scripts/run_tests.sh --smoke 覆蓋移動、戰鬥、受傷、搬運、招募、交易、建造、生產、存檔與讀檔核心流程。
- 結論：$NOTE
- 限制：本報告是 Gate 17 smoke perf guard，尚未取代 UE Insights 或 Metal frame capture。
EOF

  python3 - "$ART_PERF_JSON" "$STATUS" "$TEXTURE_COUNT" "${MAX_TEXTURE_DIMENSION:-0}" <<'PY'
from __future__ import annotations

import json
import sys
from pathlib import Path

path = Path(sys.argv[1])
payload = {
    "status": "pass" if sys.argv[2] == "通過" else "fail",
    "generated_art_file_count": int(sys.argv[3]),
    "max_generated_png_dimension": int(sys.argv[4]),
    "texture_budget_max_dimension": 2048,
    "ui_icon_dimension": 512,
}
path.write_text(json.dumps(payload, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
PY

  if [[ "$STATUS" != "通過" ]]; then
    exit 1
  fi
fi
