# macOS Build Summary

- 狀態：通過
- Build 路徑：/Users/leo/Library/Mobile Documents/com~apple~CloudDocs/Coding/Projects/Ashfrontier/Builds/macOS/Ashfrontier.app
- 開始時間：2026-05-06 21:06:22 CST
- 完成時間：2026-05-06 21:10:22 CST
- UE5_ROOT：/Users/Shared/Epic Games/UE_5.7/Engine
- Log：Reports/package-macos.log
- 簽章狀態：警告
- 簽章備註：ad-hoc re-sign 命令完成，但 `codesign --verify --deep --strict --verbose=2 Builds/macOS/Ashfrontier.app` 受 iCloud File Provider 重新附加的 `com.apple.FinderInfo` metadata 影響而未通過；命令列啟動 smoke 仍需另行記錄。
- 啟動 smoke：通過，2026-05-06 21:11 CST 以命令列 executable 啟動，掛載 `Ashfrontier-Mac.pak`，並載入 `/Game/Maps/L_Ashfrontier_Prototype`。
- 備註：已產出 app bundle；5 分鐘 golden path 驗證需搭配 final prototype report 記錄。
