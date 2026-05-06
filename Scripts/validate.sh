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

  if git fetch origin main >/tmp/ashfrontier-fetch-origin-main.txt 2>&1; then
    read -r AHEAD BEHIND < <(git rev-list --left-right --count HEAD...origin/main)
    if [[ "$AHEAD" == "0" && "$BEHIND" == "0" ]]; then
      pass "本機 HEAD 與 origin/main 同步"
    else
      fail "本機 HEAD 與 origin/main 不同步：ahead=$AHEAD behind=$BEHIND"
    fi
  else
    fail "無法 fetch origin/main：$(cat /tmp/ashfrontier-fetch-origin-main.txt)"
  fi

  V01_TAG_SHA="$(git rev-parse v0.1-gate09-editor-playable^{} 2>/dev/null || true)"
  if [[ "$V01_TAG_SHA" == "5fdb11433459742b7f6a9e10e2e33f6ed523826b" ]]; then
    pass "v0.1-gate09-editor-playable 仍指向 5fdb114"
  else
    fail "v0.1-gate09-editor-playable 指向不正確：$V01_TAG_SHA"
  fi

  log ""
  log "## Git LFS"
  if git lfs version >/tmp/ashfrontier-git-lfs-version.txt 2>&1; then
    pass "git-lfs 可用：$(cat /tmp/ashfrontier-git-lfs-version.txt)"
  else
    warn "git-lfs 不可用，後續圖片與 UE 二進位資產提交前必須處理"
    if [[ -s "Reports/git-lfs-blocker.md" ]]; then
      pass "Reports/git-lfs-blocker.md 已記錄 git-lfs blocker"
    else
      fail "缺少 Reports/git-lfs-blocker.md"
    fi
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
    "Docs/Art/ART_DIRECTION.md"
    "Docs/Art/STYLE_BIBLE.md"
    "Docs/Art/GENERATED_ART_PIPELINE.md"
    "Docs/Art/PROVENANCE_RULES.md"
    "Docs/Art/BANNED_TERMS.md"
    "Docs/Art/Prompts/ashfrontier_style_bible.md"
    "Docs/Art/Prompts/texture_prompts.md"
    "Docs/Art/Prompts/ui_icon_prompts.md"
    "Docs/Art/Prompts/faction_emblem_prompts.md"
    "Docs/Art/Prompts/portrait_prompts.md"
    "Docs/Art/Prompts/decal_and_signage_prompts.md"
    "Scripts/env.example"
    "Scripts/validate.sh"
    "Scripts/content_lint.py"
    "Scripts/art_prompt_lint.py"
    "Scripts/art_manifest_lint.py"
    "Scripts/art_coverage_lint.py"
    "Scripts/ui_art_lint.py"
    "Scripts/visual_variant_lint.py"
    "Scripts/build_texture_maps.py"
    "Scripts/generate_art_assets.sh"
    "Scripts/slice_generated_art_sheets.py"
    "Scripts/import_generated_art.sh"
    "Scripts/apply_art_slots.sh"
    "Scripts/capture_art_screenshots.sh"
    "Scripts/screenshot_smoke_lint.py"
    "Scripts/commit_gate.sh"
    ".gitattributes"
    "Content/Data/Art/ArtGenManifest.json"
    "Content/Data/Art/ArtSlotMapping.json"
    "Content/Data/Art/EnvironmentArtAssignments.json"
    "Content/Data/Art/UIArtAssignments.json"
    "Content/Data/Art/FactionArtAssignments.json"
    "Content/Data/Art/CharacterPortraitAssignments.json"
    "Content/Data/Art/CharacterVisualVariants.json"
    "Content/Data/Art/BuildingVisualVariants.json"
    "Content/Data/Art/EquipmentVisualAssignments.json"
    "Reports/environment.md"
    "Reports/gate-00-report.md"
    "Reports/Art/gate-10-report.md"
    "Reports/Art/gate-11-report.md"
    "Reports/Art/gate-12-report.md"
    "Reports/Art/gate-13-report.md"
    "Reports/Art/gate-14-report.md"
    "Reports/Art/gate-15-report.md"
    "Reports/Art/gate-16-report.md"
    "Reports/Art/generated-assets-inventory.md"
    "Reports/Art/texture-processing.md"
    "Reports/Art/import-generated-art.md"
    "Reports/Art/art-coverage.md"
    "Reports/Art/artslot-application.md"
    "Reports/Art/ui-art-lint.md"
    "Reports/Art/ui-art-application.md"
    "Reports/Art/visual-variant-lint.md"
    "Reports/Art/character-building-art-pass.md"
    "Reports/Art/capture-art-screenshots.md"
    "Reports/Art/screenshot-smoke.md"
    "Reports/Art/visual-summary.md"
    "Reports/Art/art-perf-impact.md"
    "Reports/Art/gate-17-report.md"
    "Reports/mac-build-summary.md"
    "Reports/mac-launch-smoke.md"
    "Reports/final-v0.2-generated-art-report.md"
    "Reports/Art/final-art-assets-inventory.md"
    "Reports/Art/final-visual-summary.md"
    "Reports/known-issues.md"
    "Reports/Art/gate-18-report.md"
    "Content/Python/import_generated_art.py"
    "Content/Python/apply_art_slots.py"
    "Content/Data/Art/GeneratedMaterialInstances.json"
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
  log "## v0.2 生成美術目錄"
  ART_DIRS=(
    "SourceArt/Generated/Concepts"
    "SourceArt/Generated/Textures"
    "SourceArt/Generated/UIIcons"
    "SourceArt/Generated/FactionEmblems"
    "SourceArt/Generated/Portraits"
    "SourceArt/Generated/Decals"
    "SourceArt/Generated/Banners"
    "SourceArt/Generated/Metadata"
    "Content/GeneratedArt/Textures"
    "Content/GeneratedArt/Materials"
    "Content/GeneratedArt/UI"
    "Content/GeneratedArt/Factions"
    "Content/GeneratedArt/Characters"
    "Content/GeneratedArt/Decals"
    "Content/GeneratedArt/Banners"
    "Content/Data/Art"
    "Docs/Art/Prompts"
    "Reports/Art"
    "Reports/Art/Screenshots"
  )

  for dir in "${ART_DIRS[@]}"; do
    if [[ -d "$dir" ]]; then
      pass "$dir 目錄存在"
    else
      fail "$dir 目錄不存在"
    fi
  done

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
    "Scripts/generate_art_assets.sh"
    "Scripts/import_generated_art.sh"
    "Scripts/apply_art_slots.sh"
    "Scripts/capture_art_screenshots.sh"
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

  for py_script in Scripts/art_prompt_lint.py Scripts/art_manifest_lint.py Scripts/art_coverage_lint.py Scripts/ui_art_lint.py Scripts/visual_variant_lint.py Scripts/build_texture_maps.py Scripts/slice_generated_art_sheets.py Scripts/screenshot_smoke_lint.py Content/Python/import_generated_art.py Content/Python/apply_art_slots.py; do
    if python3 -m py_compile "$py_script"; then
      pass "$py_script Python 語法通過"
    else
      fail "$py_script Python 語法失敗"
    fi
  done

  log ""
  log "## Art Prompt Lint"
  if python3 Scripts/art_prompt_lint.py; then
    pass "art_prompt_lint.py 通過"
  else
    fail "art_prompt_lint.py 失敗"
  fi

  log ""
  log "## Art Manifest Lint"
  if python3 Scripts/art_manifest_lint.py; then
    pass "art_manifest_lint.py 通過"
  else
    fail "art_manifest_lint.py 失敗"
  fi

  log ""
  log "## Texture Processing Smoke"
  if python3 Scripts/build_texture_maps.py --check; then
    pass "build_texture_maps.py --check 通過"
  else
    fail "build_texture_maps.py --check 失敗"
  fi

  log ""
  log "## UE Import Smoke"
  if ./Scripts/import_generated_art.sh --smoke; then
    pass "import_generated_art.sh --smoke 通過"
  else
    fail "import_generated_art.sh --smoke 失敗"
  fi

  log ""
  log "## Art Coverage Lint"
  if python3 Scripts/art_coverage_lint.py; then
    pass "art_coverage_lint.py 通過"
  else
    fail "art_coverage_lint.py 失敗"
  fi

  log ""
  log "## UI Art Lint"
  if python3 Scripts/ui_art_lint.py; then
    pass "ui_art_lint.py 通過"
  else
    fail "ui_art_lint.py 失敗"
  fi

  log ""
  log "## Visual Variant Lint"
  if python3 Scripts/visual_variant_lint.py; then
    pass "visual_variant_lint.py 通過"
  else
    fail "visual_variant_lint.py 失敗"
  fi

  log ""
  log "## ArtSlot Apply Smoke"
  if ./Scripts/apply_art_slots.sh --smoke; then
    pass "apply_art_slots.sh --smoke 通過"
  else
    fail "apply_art_slots.sh --smoke 失敗"
  fi

  log ""
  log "## Screenshot Smoke"
  if ./Scripts/capture_art_screenshots.sh --smoke; then
    pass "capture_art_screenshots.sh --smoke 通過"
  else
    fail "capture_art_screenshots.sh --smoke 失敗"
  fi

  log ""
  log "## Art Perf Guard"
  if ./Scripts/perf_capture.sh --gate17; then
    pass "perf_capture.sh --gate17 通過"
  else
    fail "perf_capture.sh --gate17 失敗"
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
  log "## macOS Build Output"
  if [[ -d "Builds/macOS/Ashfrontier.app" ]]; then
    pass "Builds/macOS/Ashfrontier.app 存在"
  else
    fail "Builds/macOS/Ashfrontier.app 不存在"
  fi
  if [[ -x "Builds/macOS/Ashfrontier.app/Contents/MacOS/Ashfrontier" ]]; then
    pass "packaged executable 可執行"
  else
    fail "packaged executable 不存在或不可執行"
  fi
  if [[ -s "Reports/mac-launch-smoke.json" ]] && python3 - "$PROJECT_ROOT" <<'PY'
from __future__ import annotations

import json
import sys
from pathlib import Path

payload = json.loads((Path(sys.argv[1]) / "Reports" / "mac-launch-smoke.json").read_text(encoding="utf-8"))
sys.exit(0 if payload.get("status") == "pass" else 1)
PY
  then
    pass "mac-launch-smoke 通過"
  else
    fail "mac-launch-smoke 未通過"
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
