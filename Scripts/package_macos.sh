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
  cat > "$BLOCKER" <<EOF
# macOS Build Blocker

- 狀態：打包失敗
- 原因：UE5_EDITOR 未設定。
- 輸出目標：$BUILD_DIR/Ashfrontier.app
- 下一步：依 Scripts/env.example 設定 UE5_EDITOR 與 UE5_ROOT。
EOF
  exit 1
fi

cat > "$BLOCKER" <<EOF
# macOS Build Blocker

- 狀態：待實作
- 原因：Gate 09 前需補上 RunUAT BuildCookRun 指令與簽章策略。
- 輸出目標：$BUILD_DIR/Ashfrontier.app
EOF
