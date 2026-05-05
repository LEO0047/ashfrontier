#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
REPORT_DIR="$PROJECT_ROOT/Reports"
REPORT="$REPORT_DIR/perf-summary.md"
UPROJECT="$PROJECT_ROOT/Ashfrontier.uproject"

mkdir -p "$REPORT_DIR"

MACOS_VERSION="$(sw_vers -productVersion 2>/dev/null || echo unknown)"
ARCH="$(uname -m 2>/dev/null || echo unknown)"
CPU="$(sysctl -n machdep.cpu.brand_string 2>/dev/null || echo unknown)"
GPU="$(system_profiler SPDisplaysDataType 2>/dev/null | awk '/Chipset Model:/ {sub(/^[ \t]+Chipset Model: /, ""); print; exit}')"
RESOLUTION="$(system_profiler SPDisplaysDataType 2>/dev/null | awk '/Resolution:/ {sub(/^[ \t]+Resolution: /, ""); print; exit}')"

if [[ ! -f "$UPROJECT" ]]; then
  cat > "$REPORT" <<EOF
# 效能摘要

- 狀態：略過
- 原因：Gate 00 尚未建立 Ashfrontier.uproject。
- macOS：$MACOS_VERSION
- CPU 架構：$ARCH
- CPU：$CPU
- GPU：${GPU:-unknown}
- 解析度：${RESOLUTION:-unknown}
- 測試設定：尚未有主城壓力場景。
- 下一步：Gate 04 建立主城壓力場景後，補上 Metal frame capture 或 UE stat 報告。
EOF
  exit 0
fi

cat > "$REPORT" <<EOF
# 效能摘要

- 狀態：待實作
- macOS：$MACOS_VERSION
- CPU 架構：$ARCH
- CPU：$CPU
- GPU：${GPU:-unknown}
- 解析度：${RESOLUTION:-unknown}
- 測試設定：Gate 04 起指定主城壓力場景。
EOF
