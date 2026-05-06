# macOS Build Summary

- 狀態：通過
- Build 路徑：/Users/leo/Library/Mobile Documents/com~apple~CloudDocs/Coding/Projects/Ashfrontier/Builds/macOS/Ashfrontier.app
- 開始時間：2026-05-06 21:32:55 CST
- 完成時間：2026-05-06 21:41:56 CST
- UE5_ROOT：/Users/Shared/Epic Games/UE_5.7/Engine
- Log：Reports/package-macos.log
- 簽章狀態：警告
- 簽章備註：ad-hoc re-sign 命令完成，但 strict verify 未通過；目前 iCloud File Provider 會重新附加 Finder metadata。若命令列啟動通過，仍可作為本機 playable smoke build，但 Finder / Gatekeeper 行為需另行驗證。
- 正常 Metal 啟動 smoke：通過，2026-05-06 21:43 CST 以 `Builds/macOS/Ashfrontier.app/Contents/MacOS/Ashfrontier -nosplash -stdout -FullStdOutLogOutput` 啟動，log 顯示 `LoadMap Load map complete /Game/Maps/L_Ashfrontier_Prototype` 與 `Engine is initialized`。
- 黑畫面修補：本次 build 已加入開場 DirectionalLight、SkyLight、固定俯視相機、可見 HUD 大標題與操作提示。
- 備註：已產出 app bundle；真人 5 分鐘 golden path 驗證需搭配 final prototype report 記錄。
