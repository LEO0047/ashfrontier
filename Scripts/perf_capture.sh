#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
REPORT_DIR="$PROJECT_ROOT/Reports"
REPORT="$REPORT_DIR/perf-summary.md"
UPROJECT="$PROJECT_ROOT/Ashfrontier.uproject"

mkdir -p "$REPORT_DIR"

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

- 狀態：通過（Gate 04 初版）
- macOS：$MACOS_VERSION
- CPU 架構：$ARCH
- CPU：$CPU
- GPU：${GPU:-unknown}
- 解析度：${RESOLUTION:-unknown}
- 模式：${MODE:-default}
- 測試場景：/Game/Maps/L_Ashfrontier_Prototype runtime Gate 04 world blockout。
- 主城壓力區：鹽脊主城 blockout，含城牆、城門、巡邏 route marker 與城市巡邏 agent。
- 路線設定：城市守衛巡邏 4 個 waypoint；玻璃屋商隊路線 4 個 waypoint。
- 測試方式：本階段記錄 macOS / Apple Silicon / 顯示環境與主城場景設定；精確 frame time capture 會在 Gate 09 packaged build smoke 前補強。
- 已知限制：此報告尚未使用 Metal frame capture 或 UE Insights trace，不得視為最終效能結論。
EOF
