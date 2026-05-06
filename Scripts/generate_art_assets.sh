#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

usage() {
  cat >&2 <<'EOF'
用法：
  Scripts/generate_art_assets.sh \
    --concept-sheet <path> \
    --surface-sheet <path> \
    --ui-icon-sheet <path> \
    --emblem-sheet <path> \
    --banner-sheet <path> \
    --portrait-sheet <path> \
    --decal-sheet <path>
EOF
}

CONCEPT_SHEET=""
SURFACE_SHEET=""
UI_ICON_SHEET=""
EMBLEM_SHEET=""
BANNER_SHEET=""
PORTRAIT_SHEET=""
DECAL_SHEET=""

while [[ $# -gt 0 ]]; do
  case "$1" in
    --concept-sheet)
      CONCEPT_SHEET="$2"
      shift 2
      ;;
    --surface-sheet)
      SURFACE_SHEET="$2"
      shift 2
      ;;
    --ui-icon-sheet)
      UI_ICON_SHEET="$2"
      shift 2
      ;;
    --emblem-sheet)
      EMBLEM_SHEET="$2"
      shift 2
      ;;
    --banner-sheet)
      BANNER_SHEET="$2"
      shift 2
      ;;
    --portrait-sheet)
      PORTRAIT_SHEET="$2"
      shift 2
      ;;
    --decal-sheet)
      DECAL_SHEET="$2"
      shift 2
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      echo "未知參數：$1" >&2
      usage
      exit 2
      ;;
  esac
done

for required in \
  "$CONCEPT_SHEET" \
  "$SURFACE_SHEET" \
  "$UI_ICON_SHEET" \
  "$EMBLEM_SHEET" \
  "$BANNER_SHEET" \
  "$PORTRAIT_SHEET" \
  "$DECAL_SHEET"; do
  if [[ ! -s "$required" ]]; then
    echo "缺少或空白的生成 sheet：$required" >&2
    usage
    exit 1
  fi
done

cd "$PROJECT_ROOT"
python3 "$SCRIPT_DIR/slice_generated_art_sheets.py" \
  --concept-sheet "$CONCEPT_SHEET" \
  --surface-sheet "$SURFACE_SHEET" \
  --ui-icon-sheet "$UI_ICON_SHEET" \
  --emblem-sheet "$EMBLEM_SHEET" \
  --banner-sheet "$BANNER_SHEET" \
  --portrait-sheet "$PORTRAIT_SHEET" \
  --decal-sheet "$DECAL_SHEET"
