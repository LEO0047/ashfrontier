#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
REPORT_DIR="$PROJECT_ROOT/Reports"
REPORT="$REPORT_DIR/soak-test.md"
UPROJECT="$PROJECT_ROOT/Ashfrontier.uproject"
MODE="${1:-}"

mkdir -p "$REPORT_DIR"

if [[ "$MODE" == "--smoke" ]]; then
  DURATION="${ASHFRONTIER_SMOKE_SECONDS:-60}"
else
  DURATION="${ASHFRONTIER_SOAK_SECONDS:-1800}"
fi

if [[ ! -f "$UPROJECT" ]]; then
  cat > "$REPORT" <<EOF
# Soak Test 報告

- 狀態：略過
- 目標秒數：$DURATION
- 原因：Gate 00 尚未建立 Ashfrontier.uproject。
- 下一步：Gate 09 必須執行完整 30 分鐘 soak test，或記錄明確環境限制。
EOF
  exit 0
fi

if [[ -z "${UE5_EDITOR:-}" ]]; then
  cat > "$REPORT" <<EOF
# Soak Test 報告

- 狀態：失敗
- 目標秒數：$DURATION
- 原因：UE5_EDITOR 未設定，無法執行 UE soak test。
EOF
  exit 1
fi

cat > "$REPORT" <<EOF
# Soak Test 報告

- 狀態：待實作
- 目標秒數：$DURATION
- 備註：Gate 04 起建立巡邏與主城測試場景後，需改為實際 UE soak 指令。
EOF
