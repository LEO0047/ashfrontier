#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
UPROJECT="$PROJECT_ROOT/Ashfrontier.uproject"
REPORT_DIR="$PROJECT_ROOT/Reports/Art"
SCREENSHOT_DIR="$REPORT_DIR/Screenshots"
CAPTURE_REPORT="$REPORT_DIR/capture-art-screenshots.md"
CAPTURE_JSON="$REPORT_DIR/capture-art-screenshots.json"
STDOUT_LOG="$REPORT_DIR/capture-art-screenshots.stdout.log"
UNREAL_LOG="$REPORT_DIR/capture-art-screenshots.unreal.log"
CAPTURE_TIMEOUT_SECONDS="${ASHFRONTIER_CAPTURE_TIMEOUT_SECONDS:-480}"
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

mkdir -p "$SCREENSHOT_DIR"

if [[ "$MODE" == "--smoke" ]]; then
  python3 "$SCRIPT_DIR/screenshot_smoke_lint.py"
  exit $?
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
  cat > "$CAPTURE_REPORT" <<EOF
# Art Screenshot Capture Blocker

- 狀態：失敗
- 原因：找不到 UnrealEditor，無法擷取固定相機畫面。
- 下一步：設定 UE5_EDITOR 後重新執行 Scripts/capture_art_screenshots.sh。
EOF
  exit 1
fi

if [[ "$EDITOR_PATH" == *.app ]]; then
  EDITOR_BIN="$EDITOR_PATH/Contents/MacOS/UnrealEditor"
else
  EDITOR_BIN="$EDITOR_PATH"
fi

if [[ ! -x "$EDITOR_BIN" ]]; then
  cat > "$CAPTURE_REPORT" <<EOF
# Art Screenshot Capture Blocker

- 狀態：失敗
- 原因：UnrealEditor 不可執行：$EDITOR_BIN。
- 下一步：確認 UE5 安裝與 Scripts/env.example 設定。
EOF
  exit 1
fi

rm -f "$SCREENSHOT_DIR"/*.png
STARTED_AT="$(date '+%Y-%m-%d %H:%M:%S %Z')"
: > "$STDOUT_LOG"
: > "$UNREAL_LOG"

"$EDITOR_BIN" "$UPROJECT" /Game/Maps/L_Ashfrontier_Prototype \
  -game \
  -windowed \
  -ResX=1280 \
  -ResY=720 \
  -unattended \
  -nop4 \
  -nosplash \
  -NoSound \
  -NoTrace \
  -stdout \
  -FullStdOutLogOutput \
  -log \
  -Abslog="$UNREAL_LOG" \
  -AshfrontierCaptureArtScreenshots \
  > "$STDOUT_LOG" 2>&1 &
EDITOR_PID=$!

cleanup_editor() {
  if kill -0 "$EDITOR_PID" >/dev/null 2>&1; then
    kill "$EDITOR_PID" >/dev/null 2>&1 || true
    sleep 2
  fi
  if kill -0 "$EDITOR_PID" >/dev/null 2>&1; then
    kill -9 "$EDITOR_PID" >/dev/null 2>&1 || true
  fi
}

CAPTURE_STATUS=0
for ((elapsed=0; elapsed<CAPTURE_TIMEOUT_SECONDS; elapsed++)); do
  if ! kill -0 "$EDITOR_PID" >/dev/null 2>&1; then
    wait "$EDITOR_PID" || CAPTURE_STATUS=$?
    break
  fi
  sleep 1
done

if kill -0 "$EDITOR_PID" >/dev/null 2>&1; then
  cleanup_editor
  CAPTURE_STATUS=124
fi

FINISHED_AT="$(date '+%Y-%m-%d %H:%M:%S %Z')"
SCREENSHOT_COUNT="$(find "$SCREENSHOT_DIR" -maxdepth 1 -name '*.png' -type f | wc -l | tr -d ' ')"

if [[ "$CAPTURE_STATUS" -ne 0 ]]; then
  cat > "$CAPTURE_REPORT" <<EOF
# Art Screenshot Capture Blocker

- 狀態：失敗
- 開始時間：$STARTED_AT
- 完成時間：$FINISHED_AT
- 原因：UnrealEditor 截圖流程失敗或逾時，exit=${CAPTURE_STATUS}。
- stdout log：Reports/Art/capture-art-screenshots.stdout.log
- Unreal log：Reports/Art/capture-art-screenshots.unreal.log
- 下一步：查看 log 內的 crash、Metal、視窗或 map load 問題。
EOF
  exit "$CAPTURE_STATUS"
fi

python3 "$SCRIPT_DIR/screenshot_smoke_lint.py"

cat > "$CAPTURE_REPORT" <<EOF
# 固定相機截圖擷取報告

- 狀態：通過
- 開始時間：$STARTED_AT
- 完成時間：$FINISHED_AT
- 截圖目錄：Reports/Art/Screenshots
- 截圖數：$SCREENSHOT_COUNT
- stdout log：Reports/Art/capture-art-screenshots.stdout.log
- Unreal log：Reports/Art/capture-art-screenshots.unreal.log
- smoke report：Reports/Art/screenshot-smoke.md
EOF

python3 - "$CAPTURE_JSON" "$STARTED_AT" "$FINISHED_AT" "$SCREENSHOT_COUNT" <<'PY'
from __future__ import annotations

import json
import sys
from pathlib import Path

path = Path(sys.argv[1])
payload = {
    "status": "pass",
    "started_at": sys.argv[2],
    "finished_at": sys.argv[3],
    "screenshot_count": int(sys.argv[4]),
    "screenshot_dir": "Reports/Art/Screenshots",
}
path.write_text(json.dumps(payload, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
PY
