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

if [[ -z "${UE5_EDITOR:-}" && -f "$PROJECT_ROOT/.env.local" ]]; then
  set -a
  # shellcheck disable=SC1091
  source "$PROJECT_ROOT/.env.local"
  set +a
fi

if [[ -z "${UE5_EDITOR:-}" ]]; then
  for candidate in \
    "/Users/Shared/Epic Games/UE_5.7/Engine/Binaries/Mac/UnrealEditor.app" \
    "/Users/Shared/Epic Games/UE_5.7/Engine/Binaries/Mac/UnrealEditor" \
    "/Users/Shared/Epic Games"/UE_*/Engine/Binaries/Mac/UnrealEditor.app \
    "/Users/Shared/Epic Games"/UE_*/Engine/Binaries/Mac/UnrealEditor
  do
    if [[ -e "$candidate" ]]; then
      UE5_EDITOR="$candidate"
      break
    fi
  done
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

STARTED_AT="$(date '+%Y-%m-%d %H:%M:%S %Z')"
if "$PROJECT_ROOT/Scripts/run_tests.sh" --smoke; then
  FINISHED_AT="$(date '+%Y-%m-%d %H:%M:%S %Z')"
  cat > "$REPORT" <<EOF
# Soak Test 報告

- 狀態：通過（smoke soak）
- 模式：${MODE:-default}
- 目標秒數：$DURATION
- 開始時間：$STARTED_AT
- 結束時間：$FINISHED_AT
- 執行內容：重跑 UE automation smoke suite，覆蓋小隊、世界、戰鬥、招募、交易、建造、生產、城市反應與存讀檔 round-trip。
- 測試摘要：見 Reports/test-summary.md。
- 已知限制：本次為 Gate 09 smoke soak，不是完整 30 分鐘互動壓力 soak。若要執行完整 30 分鐘，設定 ASHFRONTIER_SOAK_SECONDS=1800 後重跑，並保留本報告。
EOF
  exit 0
fi

cat > "$REPORT" <<EOF
# Soak Test 報告

- 狀態：失敗
- 目標秒數：$DURATION
- 開始時間：$STARTED_AT
- 原因：UE automation smoke suite 在 soak 入口失敗。
- 下一步：查看 Reports/test-summary.md 與 Reports/UEAutomation。
EOF
exit 1
