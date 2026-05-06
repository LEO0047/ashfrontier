# UE5 環境 Blocker

## Summary

Gate 01 需要建立 UE5 專案骨架、C++ module、可啟動 playable map 與 smoke tests。目前完整 Xcode 已可用，但本機仍找不到已完成安裝的 UE5 Editor，因此無法誠實完成 Gate 01 的 UE build / open / map 驗證。

## 已檢查項目

- `/Users/Shared/Epic Games/UE_5.7`：已存在，約 `9.0G`。
- `/Applications` 搜尋 `UnrealEditor.app`：未找到。
- `$HOME/Applications` 搜尋 `UnrealEditor.app`：未找到。
- `$HOME/Library/Application Support/Epic`：不存在。
- Spotlight 搜尋 `UnrealEditor.app` 與 `UnrealEditor`：未找到。
- `/Applications/Xcode.app`：已存在。
- `xcode-select -p`：`/Applications/Xcode.app/Contents/Developer`。
- `xcodebuild -version`：`Xcode 26.4.1` / `Build version 17E202`。

## 2026-05-06 安裝嘗試

- 已安裝 `mas 7.0.0`，並查到 Mac App Store 的 Xcode `26.4.1`。
- 嘗試 `mas get 497799835` 安裝 Xcode，但被互動式 sudo 密碼擋住：`sudo: a terminal is required to read the password`。
- 已安裝並開啟 `/Applications/Xcodes.app`；`xcodes` CLI 因缺完整 Xcode 的 `xcbuild` 無法編譯安裝。
- 已安裝並開啟 `/Applications/Epic Games Launcher.app`。
- Homebrew 沒有可用的 `legendary` formula，因此未建立 Epic CLI 安裝路徑。
- 目前仍需使用者在 GUI 中完成 Apple ID / Epic 帳號登入與下載。
- 目前可用空間約 `54GiB`，可能不足以同時容納完整 Xcode 與 UE5 Engine。

## 2026-05-06 07:50 CST 更新

- 使用者已在 Xcode agreement 視窗按下 `Agree`，完整 Xcode 驗證通過。
- `xcodebuild -runFirstLaunch` 已執行完成，macOS SDK 與 Xcode `clang` 可用。
- Epic Games Launcher 下載管理器顯示 `Unreal Engine 5.7` 正在安裝，約 `1004.97 MB of 11.78 GB`，速度約 `1.67 Mbps`。
- `UnrealEditor.app` 尚未出現在 `/Users/Shared/Epic Games/UE_5.7`，因此 UE5 仍不可用。
- 目前可用空間約 `38GiB`，需要持續監控安裝是否因磁碟空間不足而失敗。

## 2026-05-06 08:46 CST 更新

- `/Users/Shared/Epic Games/UE_5.7` 約 `13G`，仍停留在 `.egstore` 暫存目錄階段。
- 尚未出現 `Engine/`、`Engine/Binaries/Mac/UnrealEditor.app` 或等效可執行檔。
- 可用空間降至約 `32GiB`。若 Epic Launcher 接下來報磁碟不足，Gate 01 需保持 blocked，並先釋放空間或改用外接磁碟安裝 UE5。

## 2026-05-06 UE5 安裝完成觀察

- 已找到 `/Users/Shared/Epic Games/UE_5.7/Engine/Binaries/Mac/UnrealEditor.app`。
- UE5 Editor 環境 blocker 解除，Gate 01 可進入 map 建立、C++ build 與 UE automation 驗證。
- 仍需注意磁碟可用空間約 `16GiB`，後續若 build 或 package 因空間不足失敗，需另建 `Reports/mac-build-blocker.md` 或更新 Gate report。

## Blocker 結論

- UE5 Editor 缺失 blocker：已解除。
- 完整 Xcode / SDK blocker：已解除。
- 剩餘風險：磁碟可用空間偏低，後續 cook / package 可能需要釋放空間。

## 影響

- 無法建立可信的 `Content/Maps/L_Ashfrontier_Prototype.umap`。
- 在 UE5 Editor 完成安裝前，無法編譯 `Source/Ashfrontier/` UE5 C++ module。
- 無法證明 `Ashfrontier.uproject` 可由 UE5 在 macOS 開啟。
- 無法執行 UE automation / functional tests。
- 無法嘗試 macOS package。

## 已完成替代驗證

- Gate 00 文件、Scripts、content lint 與 Git remote 驗證已通過。
- `Scripts/env.example` 已提供 `UE5_EDITOR`、`UE5_ROOT` 與 `PROJECT_ROOT` 設定方式。
- `Scripts/run_tests.sh` 已能在沒有 `.uproject` 時輸出中文略過摘要，避免假裝 UE tests 已執行。

## 下一步

1. 等 Epic Games Launcher 完成 `Unreal Engine 5.7` 安裝。
2. 安裝完成後確認 `/Users/Shared/Epic Games/UE_5.7/Engine/Binaries/Mac/UnrealEditor.app` 或等效路徑存在。
3. 若磁碟空間不足，先釋放至少足以容納 Xcode 與 UE5 Engine 的空間，或指定外接磁碟作為 UE5 安裝位置。
4. 安裝完成後執行：

```bash
source .env.local
./Scripts/validate.sh
```

5. 環境可用後，重新推進 Gate 01。
