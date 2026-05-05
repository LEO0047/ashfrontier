# macOS 環境報告

## Summary

此報告記錄 Gate 00 當下的本機開發環境。時間為 `2026-05-06 05:41:50 CST`。

## 系統

- macOS：26.4.1 (`25E253`)
- CPU 架構：`arm64`
- CPU：Apple M1 Pro
- GPU：Apple M1 Pro，14 cores
- Metal 支援：Metal 4
- 內建顯示器解析度：3024 x 1964 Retina

## 開發工具

- Git：`git version 2.51.0`
- Python：`Python 3.9.6`
- Command Line Tools：`/Library/Developer/CommandLineTools`
- macOS SDK：`26.4.1`
- Xcode 狀態：`xcodebuild -version` 回報目前 active developer directory 是 Command Line Tools，不是完整 Xcode。

## UE5

- 常見路徑 `/Users/Shared/Epic Games`：不存在。
- `/Applications` 搜尋 `UnrealEditor.app`：未找到。
- Spotlight 搜尋 `UnrealEditor.app` / `UnrealEditor`：未找到。
- Gate 00 結論：目前未找到可用 UE5 Editor。Gate 01 若要編譯 C++ UE 專案，需設定 `UE5_EDITOR` / `UE5_ROOT`，或在 Gate report 中記錄精確 blocker。

## 2026-05-06 安裝嘗試更新

- 已安裝 `mas 7.0.0`，用於 Mac App Store CLI。
- `mas info 497799835` 顯示台灣區 Xcode 版本為 `26.4.1`，最低 macOS 為 `26.2`，本機 macOS `26.4.1` 符合。
- 嘗試執行 `mas get 497799835`，但 macOS 要求互動式 sudo 密碼，非互動 terminal 無法完成。
- 已透過 Homebrew cask 安裝 `/Applications/Epic Games Launcher.app`。
- 已透過 Homebrew cask 安裝 `/Applications/Xcodes.app`。
- 嘗試安裝 `xcodes` CLI 失敗，原因是本機缺完整 Xcode 的 `xcbuild`。
- 已開啟 Mac App Store 的 Xcode 頁面、Epic Games Launcher、Xcodes.app，需使用者在 GUI 中登入 Apple ID / Epic 帳號並輸入密碼。
- 目前 `/Applications/Xcode.app` 仍不存在。
- 目前 `/Users/Shared/Epic Games` 仍不存在，尚未安裝 UE5 Engine。
- 可用空間約 `54GiB`，完整 Xcode 加 UE5 Engine 可能需要先釋放更多磁碟空間。

## Git

- Repo root：`/Users/leo/Library/Mobile Documents/com~apple~CloudDocs/Coding/Projects/Ashfrontier`
- Branch：`main`
- Remote origin：`https://github.com/LEO0047/ashfrontier.git`
- Gate 00 前置檢查：本機 `HEAD` 與 `origin/main` 為 `0 0`，沒有 ahead / behind。
