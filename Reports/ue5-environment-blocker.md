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

1. 安裝 UE5，或提供現有 UE5 Editor 路徑。
2. 安裝完整 Xcode，或用 `sudo xcode-select -s /Applications/Xcode.app/Contents/Developer` 指定已安裝的 Xcode。
3. 執行：

```bash
source .env.local
./Scripts/validate.sh
```

4. 環境可用後，重新推進 Gate 01。
