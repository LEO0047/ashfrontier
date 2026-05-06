#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
REPORT_DIR="$PROJECT_ROOT/Reports"
SUMMARY="$REPORT_DIR/test-summary.md"
XML="$REPORT_DIR/test-results.xml"
UPROJECT="$PROJECT_ROOT/Ashfrontier.uproject"
GATE01_MAP="$PROJECT_ROOT/Content/Maps/L_Ashfrontier_Prototype.umap"
MODE="${1:-}"

detect_unreal_editor() {
  local candidate
  for candidate in \
    "/Users/Shared/Epic Games/UE_5.7/Engine/Binaries/Mac/UnrealEditor.app" \
    "/Users/Shared/Epic Games/UE_5.7/Engine/Binaries/Mac/UnrealEditor" \
    "/Users/Shared/Epic Games"/UE_*/Engine/Binaries/Mac/UnrealEditor.app \
    "/Users/Shared/Epic Games"/UE_*/Engine/Binaries/Mac/UnrealEditor
  do
    if [[ -e "$candidate" ]]; then
      printf '%s\n' "$candidate"
      return 0
    fi
  done
  return 1
}

mkdir -p "$REPORT_DIR"

write_skipped() {
  local reason="$1"
  cat > "$SUMMARY" <<EOF
# 測試摘要

- 狀態：略過
- 模式：${MODE:-default}
- 原因：$reason
- 備註：Gate 00 尚未建立 UE5 專案；Gate 01 起必須改為實際 UE automation / functional tests。
EOF
  cat > "$XML" <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<testsuite name="Ashfrontier.Gate" tests="1" failures="0" skipped="1">
  <testcase classname="Gate00" name="UEProjectAvailability">
    <skipped message="$reason"/>
  </testcase>
</testsuite>
EOF
}

if [[ ! -f "$UPROJECT" ]]; then
  write_skipped "尚未建立 Ashfrontier.uproject"
  exit 0
fi

if [[ -f "$PROJECT_ROOT/.env.local" ]]; then
  set -a
  # shellcheck disable=SC1091
  source "$PROJECT_ROOT/.env.local"
  set +a
fi

EDITOR_PATH="${UE5_EDITOR:-}"
if [[ -z "$EDITOR_PATH" ]]; then
  EDITOR_PATH="$(detect_unreal_editor || true)"
fi
if [[ -z "$EDITOR_PATH" ]]; then
  cat > "$SUMMARY" <<EOF
# 測試摘要

- 狀態：失敗
- 原因：已存在 Ashfrontier.uproject，但 UE5_EDITOR 未設定，且常見路徑尚未找到 UnrealEditor。
- 下一步：設定 Scripts/env.example 內的 UE5_EDITOR，或等 Epic Games Launcher 完成 UE5 安裝。
EOF
  exit 1
fi

if [[ "$EDITOR_PATH" == *.app ]]; then
  EDITOR_BIN="$EDITOR_PATH/Contents/MacOS/UnrealEditor"
else
  EDITOR_BIN="$EDITOR_PATH"
fi

if [[ ! -x "$EDITOR_BIN" ]]; then
  cat > "$SUMMARY" <<EOF
# 測試摘要

- 狀態：失敗
- 原因：UE5_EDITOR 不可執行：$EDITOR_BIN
EOF
  exit 1
fi

if [[ ! -f "$GATE01_MAP" ]]; then
  cat > "$SUMMARY" <<EOF
# 測試摘要

- 狀態：失敗
- 原因：缺少 Gate 01 playable map：Content/Maps/L_Ashfrontier_Prototype.umap。
- 下一步：安裝 UE5 Editor 後執行 Scripts/create_gate01_map.sh，以 UE Editor 建立真正的 .umap asset。
EOF
  exit 1
fi

"$EDITOR_BIN" "$UPROJECT" -unattended -nop4 -nosplash -NullRHI -ExecCmds="Automation RunTests Ashfrontier; Quit" -ReportExportPath="$REPORT_DIR/UEAutomation" || {
  cat > "$SUMMARY" <<EOF
# 測試摘要

- 狀態：失敗
- 原因：UE automation tests 失敗。
- 報告目錄：Reports/UEAutomation
EOF
  exit 1
}

python3 - "$REPORT_DIR/UEAutomation/index.json" "$SUMMARY" "$XML" "${MODE:-default}" <<'PY'
from __future__ import annotations

import html
import json
import sys
from pathlib import Path

index_path = Path(sys.argv[1])
summary_path = Path(sys.argv[2])
xml_path = Path(sys.argv[3])
mode = sys.argv[4]

tests = []
if index_path.exists():
    payload = json.loads(index_path.read_text(encoding="utf-8-sig"))
    tests = payload.get("tests", [])

failures = [test for test in tests if test.get("state") != "Success"]
summary_lines = [
    "# 測試摘要",
    "",
    f"- 狀態：{'通過' if not failures else '失敗'}",
    f"- 模式：{mode}",
    "- 報告目錄：Reports/UEAutomation",
    f"- UE automation tests：{len(tests)}",
    f"- 失敗數：{len(failures)}",
    "",
]
if tests:
    summary_lines.append("## 測試項目")
    for test in tests:
        summary_lines.append(f"- {test.get('fullTestPath', '<unknown>')}：{test.get('state', '<unknown>')}")
    summary_lines.append("")
summary_path.write_text("\n".join(summary_lines), encoding="utf-8")

xml_lines = [
    '<?xml version="1.0" encoding="UTF-8"?>',
    f'<testsuite name="Ashfrontier.Gate" tests="{len(tests)}" failures="{len(failures)}" skipped="0">',
]
for test in tests:
    name = html.escape(str(test.get("fullTestPath", "Ashfrontier.Unknown")))
    state = str(test.get("state", "Unknown"))
    xml_lines.append(f'  <testcase classname="UEAutomation" name="{name}">')
    if state != "Success":
        xml_lines.append(f'    <failure message="{html.escape(state)}"/>')
    xml_lines.append("  </testcase>")
xml_lines.append("</testsuite>")
xml_path.write_text("\n".join(xml_lines) + "\n", encoding="utf-8")

sys.exit(1 if failures else 0)
PY
