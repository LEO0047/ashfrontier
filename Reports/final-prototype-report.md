# Ashfrontier 最終原型報告

## Summary

- 最終狀態：Editor playable prototype completed; packaged build command-line smoke passed; manual packaged 5-minute golden path pending.
- 日期：2026-05-06。
- 目標平台：macOS，Apple Silicon 優先。
- Build 路徑：`Builds/macOS/Ashfrontier.app`。
- Build 時間：2026-05-06 21:06:22 CST 到 21:10:22 CST。
- 最新 commit hash：等待 Gate 09 commit gate。

## 完成項目

- 小隊控制：至少 2 名隊員可選取、移動、跟隨、待命與切換鏡頭。
- 世界：主城、前哨、野外建造區、巡邏路線與商隊路線 placeholder。
- 戰鬥：即時近戰、7 個部位傷害、流血、倒地 / 昏迷、搬運與醫療。
- 招募與交易：4 名可招募角色、商人、灰印幣、買賣與庫存。
- 基地建設：野外建造區、營火灶、儲物箱、採集點、工坊、防線與 2 條生產鏈。
- 派系與法規：3 個原創派系、派系關係、偷竊、攻擊、自衛、禁區、警告、追捕、放行與事件記憶。
- 存讀檔：角色位置、健康、庫存、派系關係、建築、儲物、生產隊列與事件記憶 round trip。
- macOS package：已產出 `Builds/macOS/Ashfrontier.app`，並完成命令列啟動與 map load smoke。

## 測試摘要

- UE C++ build：通過。
- UE automation smoke：9 項通過，0 項失敗。
- Content lint：通過。
- Soak smoke：通過；未執行完整 30 分鐘 soak。
- Perf capture：已輸出 Gate 09 macOS summary。
- Packaged smoke：命令列啟動通過，可掛載 pak 並載入 prototype map。

## 5 分鐘 Golden Path 驗證

- Editor playable golden path：由 Gate 03 / 05 / 07 / 08 / 09 automation 與 PIE 操作入口覆蓋。
- Packaged build golden path：尚未完成真人手動 5 分鐘流程。
- 已驗證 packaged build 最低啟動條件：
  - `.app` 存在。
  - executable 可命令列啟動。
  - 不依賴 Unreal Editor。
  - 可掛載 pak。
  - 可載入 `/Game/Maps/L_Ashfrontier_Prototype`。
- 不宣稱事項：尚未宣稱 Finder 啟動、Gatekeeper 完整通過或真人手動 packaged golden path 已完成。

## macOS 效能結果

- macOS：26.4.1。
- CPU / GPU：Apple M1 Pro。
- 架構：arm64。
- 解析度：3024 x 1964 Retina。
- 測試模式：`Scripts/perf_capture.sh --gate09`。
- 限制：尚未接 UE Insights trace 或 Metal frame capture。

## macOS build 狀態

- Build 路徑：`Builds/macOS/Ashfrontier.app`。
- 打包工具：`Scripts/package_macos.sh`。
- 打包環境：UE 5.7.4、Xcode 26.4.1、Metal Toolchain 17E188。
- 啟動驗證：2026-05-06 21:11 CST 命令列 smoke 通過，已掛載 `Ashfrontier-Mac.pak` 並載入 `/Game/Maps/L_Ashfrontier_Prototype`。
- 簽章狀態：iCloud File Provider metadata 會導致 strict ad-hoc codesign 警告；Finder / Gatekeeper 尚待非 iCloud 路徑驗證。

## Known Issues

- 真人手動 packaged 5 分鐘 golden path 尚未完成。
- `Builds/macOS/Ashfrontier.app` 因體積龐大由 `.gitignore` 排除，不直接 push 到 GitHub。
- 正式 UI、完整 NavMesh bake、完整 30 分鐘 soak、UE Insights / Metal capture 尚未完成。
- iCloud File Provider metadata 會影響 strict codesign。

## 下一階段建議

- 將 golden path 自動化成 packaged build 可執行的 headless driver。
- 將 build output 移到非 iCloud 本機路徑後重新做 ad-hoc signing 與 Finder launch smoke。
- 補正式 UI、互動提示、商店 / 建築 / 狀態面板。
- 將 runtime blockout 升級為 editor-time map asset，接正式 NavMesh bake。
