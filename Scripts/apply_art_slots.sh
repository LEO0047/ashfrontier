#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
UPROJECT="$PROJECT_ROOT/Ashfrontier.uproject"
PY_SCRIPT="$PROJECT_ROOT/Content/Python/apply_art_slots.py"

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

if [[ "${1:-}" == "--smoke" ]]; then
  python3 "$SCRIPT_DIR/art_coverage_lint.py"
  exit 0
fi

python3 "$SCRIPT_DIR/art_coverage_lint.py"

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
  echo "找不到 UnrealEditor，已完成 art coverage lint，但無法執行 UE apply smoke。" >&2
  exit 1
fi

if [[ "$EDITOR_PATH" == *.app ]]; then
  EDITOR_BIN="$EDITOR_PATH/Contents/MacOS/UnrealEditor"
else
  EDITOR_BIN="$EDITOR_PATH"
fi

"$EDITOR_BIN" "$UPROJECT" -unattended -nop4 -nosplash -NullRHI -ExecutePythonScript="$PY_SCRIPT"
