#!/usr/bin/env bash
set -euo pipefail

if [[ $# -lt 2 ]]; then
  echo "用法：$0 <gate-id> \"<中文 commit message>\"" >&2
  exit 2
fi

GATE_ID="$1"
COMMIT_MESSAGE="$2"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
REPORT_DIR="$PROJECT_ROOT/Reports"
STATUS_REPORT="$REPORT_DIR/git-push-status.md"
FAILURE_REPORT="$REPORT_DIR/gate-failure.md"
ROOT_GATE_REPORT="$REPORT_DIR/${GATE_ID}-report.md"
ART_GATE_REPORT="$REPORT_DIR/Art/${GATE_ID}-report.md"
GATE_REPORT="$ROOT_GATE_REPORT"
EXPECTED_REMOTE="https://github.com/LEO0047/ashfrontier.git"

mkdir -p "$REPORT_DIR"
cd "$PROJECT_ROOT"

if [[ -f "$ART_GATE_REPORT" ]]; then
  GATE_REPORT="$ART_GATE_REPORT"
fi

if [[ "$(git branch --show-current)" != "main" ]]; then
  cat > "$FAILURE_REPORT" <<EOF
# Gate Failure

- Gate：$GATE_ID
- 狀態：失敗
- 原因：目前 branch 不是 main。
- 時間：$(date '+%Y-%m-%d %H:%M:%S %Z')
EOF
  exit 1
fi

REMOTE_URL="$(git remote get-url origin 2>/dev/null || true)"
if [[ "$REMOTE_URL" != "$EXPECTED_REMOTE" ]]; then
  cat > "$FAILURE_REPORT" <<EOF
# Gate Failure

- Gate：$GATE_ID
- 狀態：失敗
- 原因：origin remote 不正確：$REMOTE_URL
- 時間：$(date '+%Y-%m-%d %H:%M:%S %Z')
EOF
  exit 1
fi

git fetch origin main
read -r AHEAD BEHIND < <(git rev-list --left-right --count HEAD...origin/main)
if [[ "$BEHIND" != "0" ]]; then
  cat > "$FAILURE_REPORT" <<EOF
# Gate Failure

- Gate：$GATE_ID
- 狀態：失敗
- 原因：本機 main 落後 origin/main $BEHIND 個 commit，需先 rebase 或 merge。
- 時間：$(date '+%Y-%m-%d %H:%M:%S %Z')
EOF
  exit 1
fi

if ! ./Scripts/validate.sh; then
  cat > "$FAILURE_REPORT" <<EOF
# Gate Failure

- Gate：$GATE_ID
- 狀態：失敗
- 原因：Scripts/validate.sh 未通過。
- 時間：$(date '+%Y-%m-%d %H:%M:%S %Z')
- 下一步：修復驗證失敗項目後重跑 commit gate。
EOF
  exit 1
fi

git add -A
if git diff --cached --quiet; then
  cat > "$FAILURE_REPORT" <<EOF
# Gate Failure

- Gate：$GATE_ID
- 狀態：失敗
- 原因：沒有可提交的變更。
- 時間：$(date '+%Y-%m-%d %H:%M:%S %Z')
EOF
  exit 1
fi

git commit -m "$COMMIT_MESSAGE"
GATE_COMMIT="$(git rev-parse HEAD)"
git push origin main
REMOTE_SHA="$(git ls-remote origin refs/heads/main | awk '{print $1}')"

cat > "$STATUS_REPORT" <<EOF
# Git Push Status

- Gate：$GATE_ID
- Gate commit：$GATE_COMMIT
- Push 狀態：已推送
- Remote branch：origin/main
- Remote SHA：$REMOTE_SHA
- 時間：$(date '+%Y-%m-%d %H:%M:%S %Z')
EOF

if [[ -f "$GATE_REPORT" ]]; then
  cat >> "$GATE_REPORT" <<EOF

## 推送紀錄

- Gate commit：$GATE_COMMIT
- Push 狀態：已推送到 origin/main
- Remote SHA：$REMOTE_SHA
- 記錄時間：$(date '+%Y-%m-%d %H:%M:%S %Z')
EOF
fi

git add "$STATUS_REPORT"
if [[ -f "$GATE_REPORT" ]]; then
  git add "$GATE_REPORT"
fi

if ! git diff --cached --quiet; then
  git commit -m "$GATE_ID: 記錄推送狀態"
  git push origin main
fi

git fetch origin main
LOCAL_SHA="$(git rev-parse HEAD)"
REMOTE_SHA="$(git rev-parse origin/main)"
if [[ "$LOCAL_SHA" != "$REMOTE_SHA" ]]; then
  echo "本機 HEAD 與 origin/main 不一致：$LOCAL_SHA / $REMOTE_SHA" >&2
  exit 1
fi

if [[ -n "$(git status --porcelain)" ]]; then
  echo "commit gate 完成後工作樹仍不乾淨：" >&2
  git status --short >&2
  exit 1
fi

echo "Gate commit 已推送：$GATE_COMMIT"
echo "目前 HEAD：$LOCAL_SHA"
