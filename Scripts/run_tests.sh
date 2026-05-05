#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
REPORT_DIR="$PROJECT_ROOT/Reports"
SUMMARY="$REPORT_DIR/test-summary.md"
XML="$REPORT_DIR/test-results.xml"
UPROJECT="$PROJECT_ROOT/Ashfrontier.uproject"
MODE="${1:-}"

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

EDITOR_PATH="${UE5_EDITOR:-}"
if [[ -z "$EDITOR_PATH" ]]; then
  cat > "$SUMMARY" <<EOF
# 測試摘要

- 狀態：失敗
- 原因：已存在 Ashfrontier.uproject，但 UE5_EDITOR 未設定。
- 下一步：設定 Scripts/env.example 內的 UE5_EDITOR，或在 Reports/mac-build-blocker.md 記錄本機 UE5 blocker。
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

"$EDITOR_BIN" "$UPROJECT" -unattended -nop4 -nosplash -NullRHI -ExecCmds="Automation RunTests Ashfrontier; Quit" -ReportOutputPath="$REPORT_DIR/UEAutomation" || {
  cat > "$SUMMARY" <<EOF
# 測試摘要

- 狀態：失敗
- 原因：UE automation tests 失敗。
- 報告目錄：Reports/UEAutomation
EOF
  exit 1
}

cat > "$SUMMARY" <<EOF
# 測試摘要

- 狀態：通過
- 模式：${MODE:-default}
- 報告目錄：Reports/UEAutomation
EOF

cat > "$XML" <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<testsuite name="Ashfrontier.Gate" tests="1" failures="0" skipped="0">
  <testcase classname="UEAutomation" name="Ashfrontier"/>
</testsuite>
EOF
