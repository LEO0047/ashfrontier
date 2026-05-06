#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
REPORT_DIR="$PROJECT_ROOT/Reports/Art"
UPROJECT="$PROJECT_ROOT/Ashfrontier.uproject"
PY_SCRIPT="$PROJECT_ROOT/Content/Python/import_generated_art.py"

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

smoke_check() {
  python3 - "$PROJECT_ROOT" <<'PY'
from __future__ import annotations

import json
import sys
from pathlib import Path

root = Path(sys.argv[1])
report = root / "Reports" / "Art" / "import-generated-art.json"
manifest = root / "Content" / "Data" / "Art" / "ArtGenManifest.json"
errors: list[str] = []

if not report.exists():
    errors.append("缺少 Reports/Art/import-generated-art.json")
else:
    payload = json.loads(report.read_text(encoding="utf-8"))
    if payload.get("status") != "pass":
        errors.append(f"import report status 不是 pass：{payload.get('status')}")
    if int(payload.get("imported_textures", 0)) < 59:
        errors.append("imported_textures 少於 59")
    if int(payload.get("material_instances", 0)) < 21:
        errors.append("material_instances 少於 21")

for required in [
    "Content/GeneratedArt/Materials/M_Ashfrontier_GeneratedSurface.uasset",
    "Content/GeneratedArt/Materials/M_Ashfrontier_GeneratedDecal.uasset",
    "Content/GeneratedArt/Materials/M_Ashfrontier_GeneratedBanner.uasset",
    "Content/GeneratedArt/Materials/M_Ashfrontier_UIIcon.uasset",
]:
    if not (root / required).exists():
        errors.append(f"缺少 UE material asset：{required}")

if manifest.exists():
    payload = json.loads(manifest.read_text(encoding="utf-8"))
    statuses = {record.get("validation_status") for record in payload.get("records", []) if isinstance(record, dict)}
    if not statuses <= {"imported", "assigned"}:
        errors.append(f"manifest 含未匯入狀態：{sorted(statuses)}")
else:
    errors.append("缺少 ArtGenManifest.json")

if errors:
    print("\n".join(errors), file=sys.stderr)
    sys.exit(1)
print("UE 生成美術匯入 smoke check 通過")
PY
}

if [[ "${1:-}" == "--smoke" ]]; then
  smoke_check
  exit 0
fi

mkdir -p "$REPORT_DIR"

if [[ ! -f "$UPROJECT" ]]; then
  echo "缺少 Ashfrontier.uproject" >&2
  exit 1
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
  cat > "$REPORT_DIR/import-generated-art.md" <<EOF
# 生成美術匯入報告

- 狀態：失敗
- 原因：找不到 UnrealEditor，且 UE5_EDITOR 未設定。
EOF
  exit 1
fi

if [[ "$EDITOR_PATH" == *.app ]]; then
  EDITOR_BIN="$EDITOR_PATH/Contents/MacOS/UnrealEditor"
else
  EDITOR_BIN="$EDITOR_PATH"
fi

if [[ ! -x "$EDITOR_BIN" ]]; then
  cat > "$REPORT_DIR/import-generated-art.md" <<EOF
# 生成美術匯入報告

- 狀態：失敗
- 原因：UnrealEditor 不可執行：$EDITOR_BIN
EOF
  exit 1
fi

if [[ ! -f "$PY_SCRIPT" ]]; then
  echo "缺少 $PY_SCRIPT" >&2
  exit 1
fi

"$EDITOR_BIN" "$UPROJECT" -unattended -nop4 -nosplash -NullRHI -ExecutePythonScript="$PY_SCRIPT"
smoke_check
