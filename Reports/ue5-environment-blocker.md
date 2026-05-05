# UE5 環境 Blocker

## Summary

Gate 01 需要建立 UE5 專案骨架、C++ module、可啟動 playable map 與 smoke tests。目前本機找不到 UE5 Editor，也沒有 active full Xcode，因此無法誠實完成 Gate 01 的 UE build / open / map 驗證。

## 已檢查項目

- `/Users/Shared/Epic Games`：不存在。
- `/Applications` 搜尋 `UnrealEditor.app`：未找到。
- `$HOME/Applications` 搜尋 `UnrealEditor.app`：未找到。
- `$HOME/Library/Application Support/Epic`：不存在。
- Spotlight 搜尋 `UnrealEditor.app` 與 `UnrealEditor`：未找到。
- `/Applications` 搜尋 `Xcode*.app`：未找到。
- `xcode-select -p`：`/Library/Developer/CommandLineTools`。
- `xcodebuild -version`：回報目前 active developer directory 是 Command Line Tools，不是完整 Xcode。

## 2026-05-06 安裝嘗試

- 已安裝 `mas 7.0.0`，並查到 Mac App Store 的 Xcode `26.4.1`。
- 嘗試 `mas get 497799835` 安裝 Xcode，但被互動式 sudo 密碼擋住：`sudo: a terminal is required to read the password`。
- 已安裝並開啟 `/Applications/Xcodes.app`；`xcodes` CLI 因缺完整 Xcode 的 `xcbuild` 無法編譯安裝。
- 已安裝並開啟 `/Applications/Epic Games Launcher.app`。
- Homebrew 沒有可用的 `legendary` formula，因此未建立 Epic CLI 安裝路徑。
- 目前仍需使用者在 GUI 中完成 Apple ID / Epic 帳號登入與下載。
- 目前可用空間約 `54GiB`，可能不足以同時容納完整 Xcode 與 UE5 Engine。

## 影響

- 無法建立可信的 `Content/Maps/L_Ashfrontier_Prototype.umap`。
- 無法編譯 `Source/Ashfrontier/` UE5 C++ module。
- 無法證明 `Ashfrontier.uproject` 可由 UE5 在 macOS 開啟。
- 無法執行 UE automation / functional tests。
- 無法嘗試 macOS package。

## 已完成替代驗證

- Gate 00 文件、Scripts、content lint 與 Git remote 驗證已通過。
- `Scripts/env.example` 已提供 `UE5_EDITOR`、`UE5_ROOT` 與 `PROJECT_ROOT` 設定方式。
- `Scripts/run_tests.sh` 已能在沒有 `.uproject` 時輸出中文略過摘要，避免假裝 UE tests 已執行。

## 下一步

1. 在 Mac App Store 或 Xcodes.app 中完成完整 Xcode 安裝。
2. 在 Epic Games Launcher 中登入 Epic 帳號，進入 Unreal Engine / Library，安裝 UE5。
3. 若磁碟空間不足，先釋放至少足以容納 Xcode 與 UE5 Engine 的空間，或指定外接磁碟作為 UE5 安裝位置。
4. 安裝完成後執行：

```bash
source .env.local
./Scripts/validate.sh
```

5. 環境可用後，重新推進 Gate 01。
