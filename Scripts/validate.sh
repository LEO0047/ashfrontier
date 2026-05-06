#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
REPORT_DIR="$PROJECT_ROOT/Reports"
SUMMARY="$REPORT_DIR/validate-summary.md"
EXPECTED_REMOTE="https://github.com/LEO0047/ashfrontier.git"

mkdir -p "$REPORT_DIR"

FAILURES=0
WARNINGS=0

log() {
  printf '%s\n' "$1"
}

fail() {
  FAILURES=$((FAILURES + 1))
  log "- 失敗：$1"
}

warn() {
  WARNINGS=$((WARNINGS + 1))
  log "- 警告：$1"
}

pass() {
  log "- 通過：$1"
}

{
  log "# Gate 驗證摘要"
  log ""
  log "- 時間：$(date '+%Y-%m-%d %H:%M:%S %Z')"
  log "- 專案根目錄：$PROJECT_ROOT"
  log ""

  cd "$PROJECT_ROOT"

  log "## Git"
  if git rev-parse --is-inside-work-tree >/dev/null 2>&1; then
    pass "目前在 git repo 內"
  else
    fail "目前不是 git repo"
  fi

  CURRENT_BRANCH="$(git branch --show-current || true)"
  if [[ "$CURRENT_BRANCH" == "main" ]]; then
    pass "目前 branch 是 main"
  else
    fail "目前 branch 不是 main：$CURRENT_BRANCH"
  fi

  REMOTE_URL="$(git remote get-url origin 2>/dev/null || true)"
  if [[ "$REMOTE_URL" == "$EXPECTED_REMOTE" ]]; then
    pass "origin remote 正確"
  else
    fail "origin remote 不正確：$REMOTE_URL"
  fi

  if git diff --check >/tmp/ashfrontier-diff-check.txt 2>&1; then
    pass "git diff --check 通過"
  else
    fail "git diff --check 失敗：$(cat /tmp/ashfrontier-diff-check.txt)"
  fi

  if git diff --name-only --diff-filter=U | grep -q .; then
    fail "存在 unresolved merge conflicts"
  else
    pass "沒有 unresolved merge conflicts"
  fi

  log ""
  log "## 必備文件"
  REQUIRED_FILES=(
    "README.md"
    "Docs/PLAN.md"
    "Docs/TECH_SPEC.md"
    "Docs/CONTENT_SCHEMA.md"
    "Docs/TEST_PLAN.md"
    "Docs/ADR/ADR-001-engine-and-scope.md"
    "Docs/ADR/ADR-002-data-driven-content.md"
    "Docs/ADR/ADR-003-single-player-only.md"
    "Docs/ADR/ADR-004-git-validation-gates.md"
    "Docs/ADR/ADR-005-macos-first-target.md"
    "Docs/Research/Kenshi2_DeepResearch_zh-TW.md"
    "Scripts/env.example"
    "Scripts/validate.sh"
    "Scripts/content_lint.py"
    "Scripts/commit_gate.sh"
    "Reports/environment.md"
    "Reports/gate-00-report.md"
  )

  for file in "${REQUIRED_FILES[@]}"; do
    if [[ -s "$file" ]]; then
      pass "$file 存在"
    else
      fail "$file 不存在或是空檔"
    fi
  done

  if [[ -f "Ashfrontier.uproject" ]]; then
    log ""
    log "## Gate 01 專案骨架"
    GATE01_FILES=(
      "Ashfrontier.uproject"
      "Source/Ashfrontier/Ashfrontier.Build.cs"
      "Source/Ashfrontier.Target.cs"
      "Source/AshfrontierEditor.Target.cs"
      "Source/Ashfrontier/Private/Ashfrontier.cpp"
      "Source/Ashfrontier/Private/AshfrontierGameMode.cpp"
      "Source/Ashfrontier/Private/AshfrontierPlayerController.cpp"
      "Source/Ashfrontier/Private/AshfrontierCharacter.cpp"
      "Source/Ashfrontier/Private/AshfrontierHUD.cpp"
      "Source/Ashfrontier/Private/Tests/AshfrontierSmokeTests.cpp"
      "Config/DefaultEngine.ini"
      "Config/DefaultGame.ini"
      "Config/DefaultInput.ini"
      "Scripts/create_gate01_map.sh"
      "BuildScripts/create_gate01_map.py"
    )

    for file in "${GATE01_FILES[@]}"; do
      if [[ -s "$file" ]]; then
        pass "$file 存在"
      else
        fail "$file 不存在或是空檔"
      fi
    done

    if python3 -m json.tool Ashfrontier.uproject >/dev/null; then
      pass "Ashfrontier.uproject JSON 語法通過"
    else
      fail "Ashfrontier.uproject JSON 語法失敗"
    fi
  fi

  log ""
  log "## 繁體中文文件檢查"
  if python3 - "$PROJECT_ROOT" <<'PY'
from pathlib import Path
import sys

root = Path(sys.argv[1])
targets = [root / "README.md", root / "Docs", root / "Reports"]
simplified = set("简汉语开发测试验证构项")
errors = []
for target in targets:
    files = [target] if target.is_file() else list(target.rglob("*.md"))
    for path in files:
        if not path.exists():
            continue
        text = path.read_text(encoding="utf-8")
        if not any("\u4e00" <= ch <= "\u9fff" for ch in text):
            errors.append(f"{path.relative_to(root)} 沒有中文內容")
        bad = sorted({ch for ch in text if ch in simplified})
        if bad:
            errors.append(f"{path.relative_to(root)} 疑似包含簡體字：{''.join(bad)}")
if errors:
    print("\n".join(errors))
    sys.exit(1)
PY
  then
    pass "中文文件基本檢查通過"
  else
    fail "中文文件基本檢查失敗"
  fi

  log ""
  log "## 腳本檢查"
  SCRIPT_FILES=(
    "Scripts/validate.sh"
    "Scripts/run_tests.sh"
    "Scripts/soak_test.sh"
    "Scripts/perf_capture.sh"
    "Scripts/package_macos.sh"
    "Scripts/commit_gate.sh"
    "Scripts/create_gate01_map.sh"
  )
  for script in "${SCRIPT_FILES[@]}"; do
    if [[ -x "$script" ]]; then
      pass "$script 可執行"
    else
      fail "$script 不可執行"
    fi
    if bash -n "$script"; then
      pass "$script bash 語法通過"
    else
      fail "$script bash 語法失敗"
    fi
  done

  if python3 -m py_compile Scripts/content_lint.py; then
    pass "Scripts/content_lint.py Python 語法通過"
  else
    fail "Scripts/content_lint.py Python 語法失敗"
  fi

  log ""
  log "## Content Lint"
  if python3 Scripts/content_lint.py; then
    pass "content_lint.py 通過"
  else
    fail "content_lint.py 失敗"
  fi

  log ""
  log "## UE 測試入口"
  if ./Scripts/run_tests.sh --smoke; then
    pass "run_tests.sh smoke 通過"
  else
    fail "run_tests.sh smoke 失敗"
  fi

  log ""
  log "## 結果"
  log "- 警告數：$WARNINGS"
  log "- 失敗數：$FAILURES"

  if [[ "$FAILURES" -gt 0 ]]; then
    exit 1
  fi
} | tee "$SUMMARY"

if [[ "$FAILURES" -gt 0 ]]; then
  exit 1
fi

exit 0
