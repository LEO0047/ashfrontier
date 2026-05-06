# Gate 09 報告

## 狀態

- Gate：Gate 09 — 存讀檔、macOS 打包、回歸、soak test 與最終報告。
- 狀態：本機實作、package smoke、soak smoke 與 perf smoke 完成，等待最終 `./Scripts/validate.sh` 與 commit gate。
- 日期：2026-05-06。

## 完成內容

- 新增 `UAshfrontierSaveGame`：
  - `schema_version` 目前為 `1`。
  - 保存角色、部位健康、庫存、派系關係、建築、儲物、生產隊列與事件記憶。
- 新增 `UAshfrontierSaveGameSystemComponent`：
  - 支援 snapshot / restore。
  - 支援存到 slot `AshfrontierPrototype`。
  - 支援從 slot 載入。
- 擴充 gameplay components：
  - `UAshfrontierInventoryComponent` 支援 SaveGame 回寫。
  - `UAshfrontierDamageModelComponent` 支援 7 個部位健康狀態保存與回復。
  - `UAshfrontierFactionSystemComponent` 支援派系關係保存與回復。
  - `UAshfrontierCrimeEventMemoryComponent` 支援事件記憶保存與回復。
  - `AAshfrontierPlacedBuilding` 支援建築儲物與生產隊列保存與回復。
- 擴充 `AAshfrontierPlayerController`：
  - `F5` 執行存檔。
  - `F9` 執行讀檔。
  - 追蹤玩家放置建築，供 SaveGame snapshot 使用。
- 更新 Gate 09 腳本：
  - `Scripts/soak_test.sh --smoke` 重新執行 UE automation smoke suite。
  - `Scripts/perf_capture.sh --gate09` 輸出 Gate 09 macOS 效能摘要。
  - `Scripts/package_macos.sh` 使用 `/tmp` 乾淨工作區打包，並優先複製完整 staged `.app`。
- 新增 UE automation test `Ashfrontier.SaveGame.RoundTripCoreState`。

## 測試結果

- `Build.sh AshfrontierEditor Mac Development -Project=Ashfrontier.uproject`：通過。
- `./Scripts/run_tests.sh --smoke`：通過。
- UE automation：
  - `Ashfrontier.Building.ProductionFlow`：Success。
  - `Ashfrontier.Combat.DamageCarryMedical`：Success。
  - `Ashfrontier.Data.ContentJsonLoads`：Success。
  - `Ashfrontier.Economy.RecruitInventoryTrade`：Success。
  - `Ashfrontier.Legal.GuardReactionMemory`：Success。
  - `Ashfrontier.SaveGame.RoundTripCoreState`：Success。
  - `Ashfrontier.Smoke.ModuleLoads`：Success。
  - `Ashfrontier.Squad.OrderStateFlow`：Success。
  - `Ashfrontier.World.BlockoutRoutes`：Success。
- `./Scripts/soak_test.sh --smoke`：通過；此為 smoke soak，不是完整 30 分鐘 soak。
- `./Scripts/perf_capture.sh --gate09`：通過；已輸出 `Reports/perf-summary.md`。

## Playable Contract 狀態

- Editor playable prototype 已完成：
  - 小隊控制與移動。
  - 近戰、部位傷害、流血、倒地 / 昏迷、搬運與醫療。
  - 招募、交易、採集、建造與生產。
  - 城市守衛反應、派系關係變化與事件記憶。
  - 存檔與讀檔 round trip。
- macOS packaged smoke build 已嘗試並產出 `Builds/macOS/Ashfrontier.app`。
- packaged build 已完成命令列 smoke：不依賴 Unreal Editor，能掛載 pak、初始化 UE runtime 並載入 `/Game/Maps/L_Ashfrontier_Prototype`。
- 尚未宣稱真人手動 5 分鐘 packaged golden path 已完成；目前 golden path 由 Editor automation 與 packaged map-load smoke 共同覆蓋。

## macOS build / test 狀態

- Build 路徑：`Builds/macOS/Ashfrontier.app`。
- Build 時間：2026-05-06 21:06:22 CST 到 21:10:22 CST。
- 打包腳本：`./Scripts/package_macos.sh`。
- Package log：`Reports/package-macos.log`。
- Build summary：`Reports/mac-build-summary.md`。
- 命令列啟動 smoke：2026-05-06 21:11 CST 通過，已掛載 `Ashfrontier-Mac.pak` 並載入 `/Game/Maps/L_Ashfrontier_Prototype`。
- 簽章限制：iCloud File Provider metadata 會造成 strict ad-hoc `codesign` 警告；詳見 `Reports/known-issues.md`。

## 已知問題

- 真人手動 5 分鐘 packaged golden path 尚待完成。
- Finder / Gatekeeper 啟動需在非 iCloud 路徑或清乾淨 metadata 後另行驗證。
- 正式 UI、正式 AI 行為樹、完整 NavMesh bake、完整 30 分鐘 soak、UE Insights / Metal capture 尚未完成。

## 下一階段建議

- 將 debug key flow 收斂成正式互動 UI。
- 將 runtime blockout 逐步轉成 editor-time map asset 與 NavMesh build。
- 補 headless golden path driver，讓 packaged build 可以自動跑 5 分鐘 golden path。
- 把 packaged `.app` 複製到非 iCloud 本機路徑後重新測試 ad-hoc signing 與 Finder 啟動。

## 推送紀錄

- Gate commit：等待 commit gate。
- Push 狀態：等待 commit gate。

## 推送紀錄

- Gate commit：7f55ed6125bb57b56b011908f86946918a80ef6a
- Push 狀態：已推送到 origin/main
- Remote SHA：7f55ed6125bb57b56b011908f86946918a80ef6a
- 記錄時間：2026-05-06 21:14:40 CST
