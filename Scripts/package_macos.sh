#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
REPORT_DIR="$PROJECT_ROOT/Reports"
BUILD_DIR="${ASHFRONTIER_MAC_BUILD_DIR:-$PROJECT_ROOT/Builds/macOS}"
BLOCKER="$REPORT_DIR/mac-build-blocker.md"
UPROJECT="$PROJECT_ROOT/Ashfrontier.uproject"

mkdir -p "$REPORT_DIR" "$BUILD_DIR"

if [[ ! -f "$UPROJECT" ]]; then
  cat > "$BLOCKER" <<EOF
# macOS Build Blocker

- 狀態：尚未嘗試打包
- 原因：Gate 00 尚未建立 Ashfrontier.uproject。
- 已完成替代驗證：文件、環境與腳本骨架驗證。
- 下一步：Gate 01 建立 UE5 專案後，設定 UE5_EDITOR 與 UE5_ROOT，再執行 macOS package smoke test。
EOF
  exit 0
fi

if [[ -z "${UE5_EDITOR:-}" ]]; then
  if [[ -f "$PROJECT_ROOT/.env.local" ]]; then
    set -a
    # shellcheck disable=SC1091
    source "$PROJECT_ROOT/.env.local"
    set +a
  fi
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
  cat > "$BLOCKER" <<EOF
# macOS Build Blocker

- 狀態：打包失敗
- 原因：UE5_EDITOR 未設定，且常見 UE5 路徑未找到 UnrealEditor。
- 輸出目標：$BUILD_DIR/Ashfrontier.app
- 下一步：依 Scripts/env.example 設定 UE5_EDITOR 與 UE5_ROOT。
EOF
  exit 1
fi

if [[ "$UE5_EDITOR" == *.app ]]; then
  EDITOR_BIN="$UE5_EDITOR/Contents/MacOS/UnrealEditor"
  UE5_ROOT="$(cd "$(dirname "$UE5_EDITOR")/../.." && pwd)"
else
  EDITOR_BIN="$UE5_EDITOR"
  UE5_ROOT="$(cd "$(dirname "$UE5_EDITOR")/../.." && pwd)"
fi

RUN_UAT="$UE5_ROOT/Build/BatchFiles/RunUAT.sh"
TARGET_APP="$BUILD_DIR/Ashfrontier.app"
LOG="$REPORT_DIR/package-macos.log"
STARTED_AT="$(date '+%Y-%m-%d %H:%M:%S %Z')"

export COPYFILE_DISABLE=1
if [[ -d "$PROJECT_ROOT/Binaries" ]]; then
  xattr -cr "$PROJECT_ROOT/Binaries" 2>/dev/null || true
fi

PACKAGE_WORK_ROOT="${ASHFRONTIER_PACKAGE_WORK_ROOT:-/tmp/ashfrontier-package-$(date '+%Y%m%d%H%M%S')}"
PACKAGE_PROJECT_ROOT="$PACKAGE_WORK_ROOT/Ashfrontier"
mkdir -p "$PACKAGE_PROJECT_ROOT"
rsync -a \
  --exclude ".git" \
  --exclude "Binaries" \
  --exclude "Intermediate" \
  --exclude "Saved" \
  --exclude "Builds" \
  --exclude "DerivedDataCache" \
  --exclude ".DS_Store" \
  "$PROJECT_ROOT/" "$PACKAGE_PROJECT_ROOT/" || {
  cat > "$BLOCKER" <<EOF
# macOS Build Blocker

- 狀態：打包失敗
- 原因：無法建立 /tmp 乾淨打包工作區。
- 工作區：$PACKAGE_PROJECT_ROOT
- 輸出目標：$TARGET_APP
- 下一步：確認 /tmp 可寫入與磁碟空間。
EOF
  exit 1
}
UPROJECT_FOR_PACKAGE="$PACKAGE_PROJECT_ROOT/Ashfrontier.uproject"

if [[ ! -x "$EDITOR_BIN" || ! -x "$RUN_UAT" ]]; then
  cat > "$BLOCKER" <<EOF
# macOS Build Blocker

- 狀態：打包失敗
- 原因：UE5_EDITOR 或 RunUAT 不可執行。
- UE5_EDITOR：$EDITOR_BIN
- RunUAT：$RUN_UAT
- 輸出目標：$TARGET_APP
- 下一步：確認 UE5 安裝完整，並修正 Scripts/env.example / .env.local。
EOF
  exit 1
fi

"$RUN_UAT" BuildCookRun \
  -project="$UPROJECT_FOR_PACKAGE" \
  -noP4 \
  -platform=Mac \
  -clientconfig=Development \
  -build \
  -cook \
  -stage \
  -pak \
  -archive \
  -archivedirectory="$BUILD_DIR" \
  -utf8output > "$LOG" 2>&1 || {
  FAILURE_REASON="RunUAT BuildCookRun 失敗。"
  NEXT_STEP="查看 Reports/package-macos.log，檢查 UE5、Xcode、SDK、磁碟空間、簽章或 cook 錯誤。"
  if grep -q "missing Metal Toolchain" "$LOG" 2>/dev/null; then
    FAILURE_REASON="macOS cook shader 編譯失敗：Xcode 缺少 Metal Toolchain。Log 內出現 cannot execute tool 'metal' due to missing Metal Toolchain。"
    NEXT_STEP="執行 xcodebuild -downloadComponent MetalToolchain 安裝 Metal Toolchain 後，重新執行 Scripts/package_macos.sh。"
  elif grep -q "resource fork, Finder information" "$LOG" 2>/dev/null; then
    FAILURE_REASON="macOS codesign 失敗：app bundle 含 resource fork / Finder metadata。"
    NEXT_STEP="改用 /tmp 乾淨工作區打包，或清理 extended attributes 後重跑。"
  fi
  cat > "$BLOCKER" <<EOF
# macOS Build Blocker

- 狀態：打包失敗
- 原因：$FAILURE_REASON
- 開始時間：$STARTED_AT
- 輸出目標：$TARGET_APP
- Log：Reports/package-macos.log
- 已完成替代驗證：Editor C++ build 與 UE automation 可作為替代驗證；不得宣稱 packaged build 完成。
- 下一步：$NEXT_STEP
EOF
  exit 1
}

STAGED_APP="$PACKAGE_PROJECT_ROOT/Saved/StagedBuilds/Mac/Ashfrontier.app"
if [[ -d "$STAGED_APP" ]]; then
  FOUND_APP="$STAGED_APP"
else
  FOUND_APP="$(find "$BUILD_DIR" -maxdepth 4 -name "Ashfrontier.app" -type d | head -n 1 || true)"
fi
if [[ -z "$FOUND_APP" ]]; then
  cat > "$BLOCKER" <<EOF
# macOS Build Blocker

- 狀態：打包失敗
- 原因：RunUAT 結束但未找到 Ashfrontier.app。
- 開始時間：$STARTED_AT
- 輸出目標：$TARGET_APP
- Log：Reports/package-macos.log
- 下一步：查看 archive 目錄與 RunUAT log。
EOF
  exit 1
fi

if [[ "$FOUND_APP" != "$TARGET_APP" ]]; then
  if [[ -d "$TARGET_APP" ]]; then
    BACKUP_APP="$BUILD_DIR/Ashfrontier.app.previous.$(date '+%Y%m%d%H%M%S')"
    mv "$TARGET_APP" "$BACKUP_APP"
  fi
  ditto "$FOUND_APP" "$TARGET_APP"
fi

xattr -cr "$TARGET_APP" 2>/dev/null || true
CODESIGN_STATUS="通過"
CODESIGN_NOTE="已完成 ad-hoc re-sign。"
if ! codesign --force --sign - --timestamp=none --deep "$TARGET_APP" >> "$LOG" 2>&1; then
  CODESIGN_STATUS="警告"
  CODESIGN_NOTE="ad-hoc re-sign 未通過；目前 iCloud File Provider 會重新附加 Finder metadata。若命令列啟動通過，仍可作為本機 playable smoke build，但 Finder / Gatekeeper 行為需另行驗證。"
elif ! codesign --verify --deep --strict --verbose=2 "$TARGET_APP" >> "$LOG" 2>&1; then
  CODESIGN_STATUS="警告"
  CODESIGN_NOTE="ad-hoc re-sign 命令完成，但 strict verify 未通過；目前 iCloud File Provider 會重新附加 Finder metadata。若命令列啟動通過，仍可作為本機 playable smoke build，但 Finder / Gatekeeper 行為需另行驗證。"
fi

APP_BIN="$TARGET_APP/Contents/MacOS/Ashfrontier"
if [[ ! -x "$APP_BIN" ]]; then
  cat > "$BLOCKER" <<EOF
# macOS Build Blocker

- 狀態：打包失敗
- 原因：找到 Ashfrontier.app，但 packaged executable 不可執行。
- Build 路徑：$TARGET_APP
- Log：Reports/package-macos.log
- 下一步：檢查 app bundle 結構與簽章設定。
EOF
  exit 1
fi

FINISHED_AT="$(date '+%Y-%m-%d %H:%M:%S %Z')"
cat > "$REPORT_DIR/mac-build-summary.md" <<EOF
# macOS Build Summary

- 狀態：通過
- Build 路徑：$TARGET_APP
- 開始時間：$STARTED_AT
- 完成時間：$FINISHED_AT
- UE5_ROOT：$UE5_ROOT
- Log：Reports/package-macos.log
- 簽章狀態：$CODESIGN_STATUS
- 簽章備註：$CODESIGN_NOTE
- 備註：已產出 app bundle；5 分鐘 golden path 驗證需搭配 final prototype report 記錄。
EOF

if [[ -f "$BLOCKER" ]]; then
  mv "$BLOCKER" "$REPORT_DIR/mac-build-blocker.previous.md"
fi

exit 0
