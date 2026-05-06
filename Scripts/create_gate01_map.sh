#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
UPROJECT="$PROJECT_ROOT/Ashfrontier.uproject"
PYTHON_SCRIPT="$PROJECT_ROOT/BuildScripts/create_gate01_map.py"
MAP_FILE="$PROJECT_ROOT/Content/Maps/L_Ashfrontier_Prototype.umap"
EDITOR_LOG="$HOME/Library/Logs/Unreal Engine/AshfrontierEditor/Ashfrontier.log"

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
  echo "找不到 UnrealEditor。請先完成 UE5 安裝或設定 UE5_EDITOR。" >&2
  exit 1
fi

if [[ "$EDITOR_PATH" == *.app ]]; then
  EDITOR_BIN="$EDITOR_PATH/Contents/MacOS/UnrealEditor"
else
  EDITOR_BIN="$EDITOR_PATH"
fi

if [[ ! -x "$EDITOR_BIN" ]]; then
  echo "UnrealEditor 不可執行：$EDITOR_BIN" >&2
  exit 1
fi

mkdir -p "$(dirname "$EDITOR_LOG")" "$PROJECT_ROOT/Reports"
: > "$EDITOR_LOG"

"$EDITOR_BIN" "$UPROJECT" -unattended -nop4 -nosplash -NullRHI -ExecutePythonScript="$PYTHON_SCRIPT" \
  > "$PROJECT_ROOT/Reports/create-gate01-map.stdout.log" 2>&1 &
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

map_created_successfully() {
  [[ -f "$MAP_FILE" ]] \
    && [[ -f "$EDITOR_LOG" ]] \
    && grep -q "Ashfrontier Gate 01: Gate 01 playable map 已建立並儲存" "$EDITOR_LOG" \
    && grep -q "LogExit: Editor shut down" "$EDITOR_LOG" \
    && ! grep -q "Critical error" "$EDITOR_LOG" \
    && ! grep -q "Assertion failed" "$EDITOR_LOG"
}

for _ in {1..900}; do
  if [[ -f "$EDITOR_LOG" ]] && { grep -q "Critical error" "$EDITOR_LOG" || grep -q "Assertion failed" "$EDITOR_LOG"; }; then
    cleanup_editor
    exit 1
  fi

  if map_created_successfully; then
    cleanup_editor
    exit 0
  fi

  if ! kill -0 "$EDITOR_PID" >/dev/null 2>&1; then
    wait "$EDITOR_PID" || EDITOR_STATUS=$?
    if map_created_successfully; then
      exit 0
    fi
    exit "${EDITOR_STATUS:-1}"
  fi

  sleep 1
done

cleanup_editor
echo "建立 Gate 01 map 逾時。" >&2
exit 1
