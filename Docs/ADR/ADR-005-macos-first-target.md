# ADR-005：macOS 優先目標

## Status

Accepted

## Context

主要開發與驗證平台是 macOS，Apple Silicon Mac 優先。最終打包目標是 macOS `.app` playable build，不要求 Windows build。

## Decision

所有驗證腳本以 zsh / bash 與 Python 為主。UE5 路徑由 `UE5_EDITOR`、`UE5_ROOT` 與 `PROJECT_ROOT` 指定，不硬寫死。Renderer 與 plugin 選擇必須考慮 Metal 相容性。

## Consequences

- PowerShell 不是必要交付物。
- 若本機缺 UE5、完整 Xcode、SDK 或簽章能力，必須輸出中文 blocker report。
- CI 與本機驗證優先使用 `Scripts/validate.sh`。
