# Ashfrontier 單一主線開發計畫

## Summary

本文件只描述一個主線 goal：在 macOS 工作區推進 Ashfrontier，建立 UE5 3D 單機小隊沙盒 RPG playable prototype，最後產出 macOS `.app` playable build，或清楚記錄阻塞條件。Gate 是同一條主線內的驗收節點，不是多個獨立專案。

## 研究文件轉譯原則

研究文件指出可採用的是系統設計方向，而不是內容照抄。Ashfrontier 的可執行原則如下：

- 先做「一城、一前哨、一野外建造區」，不要一開始追求超大世界。
- 系統核心優先於畫面：小隊控制、部位傷害、倒地搬運、交易、建造、生產、事件記憶與存讀檔必須先活起來。
- 內容資料化：派系、物品、資源、配方、建築、NPC schedule、legal rules 與 dialogue conditions 必須能被 lint 與測試解析。
- AI 採可擴充分層：高階狀態、局部行為、rule / utility decision 與 event memory 分開。
- macOS / Metal 相容性優先，避免 Windows-only plugin、DirectX-only 假設與不可移植建置流程。
- placeholder 可以粗，但互動 loop 必須可玩、可測、可存讀。

## Gate 序列

| Gate | 目標 | 主要驗收 |
|---|---|---|
| Gate 00 | macOS 工作區、文件、Git 與驗證骨架 | 文件、研究副本、Scripts、環境報告與 content lint 通過 |
| Gate 01 | UE5 專案骨架與核心模組 | `.uproject` 可開啟、核心 module 可編譯、空白 playable map 可啟動 |
| Gate 02 | 資料驅動 schema 與內容 lint | 原創 factions、items、resources、recipes、buildings、schedules、legal rules 通過 lint |
| Gate 03 | 小隊控制、鏡頭與移動 | 1 到 4 名隊員可選取、移動、跟隨、待命與切換鏡頭 |
| Gate 04 | 世界 blockout、導航、巡邏與 macOS 基礎效能 | 主城、前哨、野外建造區、NavMesh、巡邏與 perf capture 初版 |
| Gate 05 | 戰鬥、部位傷害、倒地、搬運與醫療 | 近戰、部位傷害、流血、昏迷、搬運與包紮可操作 |
| Gate 06 | 招募、角色資料、庫存與交易 | 4 名可招募角色、庫存、價格表與買賣流程可操作 |
| Gate 07 | 基地建設與生產鏈 | 5 種建築、食物鏈、金屬 / 建材鏈與生產隊列可存讀 |
| Gate 08 | 派系 AI、城市守衛反應與事件記憶 | 偷竊、攻擊、自衛、禁區、警告、追捕、放行與關係變化可測 |
| Gate 09 | 存讀檔、macOS 打包、回歸、soak test 與最終報告 | 完整存讀檔、回歸、soak、perf、macOS `.app` 或 blocker report |

## Playable Contract

本 goal 的最終目的不是產出文件，而是在 macOS 上交付可啟動、可操作、可驗證的 playable prototype。文件、報告、ADR、測試腳本或 blocker report 都不能替代 playable prototype。

最低可玩定義如下：

1. 玩家可以啟動遊戲，不依賴 Unreal Editor。
2. 玩家可以進入主選單或直接進入 prototype map。
3. 玩家可以控制至少 2 名小隊成員。
4. 玩家可以移動、選取隊員、下達跟隨或移動命令。
5. 玩家可以與至少 1 名敵人發生近戰。
6. 角色會受到部位傷害。
7. 角色可以倒地或昏迷。
8. 玩家可以搬運倒地隊友。
9. 玩家可以與至少 1 名 NPC 交易。
10. 玩家可以招募至少 1 名 NPC。
11. 玩家可以在野外建造區放置至少 1 種建築。
12. 玩家可以產出或取得至少 1 種資源。
13. 玩家可以存檔。
14. 玩家可以讀檔。
15. 讀檔後，角色位置、血量、庫存、派系關係與建築狀態不得全部重置。
16. 遊戲必須提供一條 5 分鐘 golden path，可以從新遊戲開始一路驗證上述行為。

不可協商 playable milestone：

- Gate 03 結束時，必須已經可以在 UE Editor 內控制小隊移動。
- Gate 05 結束時，必須已經可以在 UE Editor 內完成「移動 → 戰鬥 → 受傷 → 倒地 → 搬運 → 醫療」流程。
- Gate 07 結束時，必須已經可以在 UE Editor 內完成「招募 → 交易 → 建造 → 生產」流程。
- Gate 09 結束時，必須嘗試產出 macOS packaged build；若 packaged build 成功，必須可完成 5 分鐘 golden path。

若 macOS packaged build 失敗，但 Editor playable prototype 已完成，最終狀態只能標示為 `Editor playable prototype completed; macOS packaged build blocked.`。若連 Editor playable prototype 都無法完成，最終狀態必須標示為 `Playable prototype not completed.`。

## Gate 執行規則

每個 Gate 必須依序完成：

1. 實作 Gate 內容。
2. 更新 `Docs/PLAN.md`、`Docs/TECH_SPEC.md`、`Docs/TEST_PLAN.md` 或 `Docs/CONTENT_SCHEMA.md`。
3. 執行 `./Scripts/validate.sh`。
4. 若驗證失敗，先修復再重跑，不得 commit 失敗狀態。
5. 驗證通過後執行 `./Scripts/commit_gate.sh <gate-id> "<中文 commit message>"`。
6. push 後確認 `git status --porcelain` 乾淨，且 `HEAD` 與 `origin/main` 一致。
7. 將測試、commit、push、已知問題與下一個 Gate 重點寫入 `Reports/gate-XX-report.md`。

## Gate 00 狀態

- 狀態：完成並已推送。
- 範圍：建立中文文件、研究副本、macOS 環境報告、Scripts 與初始 content lint。
- Gate commit：`9e0a3c5c3730d397c108b2410eb6d683dc05a43a`。
- Gate 00 推送狀態記錄 commit：`c5de7ef072ce8090bb960c8a3f0c57ad4cc6b160`。

## Gate 01 狀態

- 狀態：完成並已推送。
- 已完成：完整 Xcode 已安裝並完成 Apple SDK agreement；UE5.7 Editor 已安裝於 `/Users/Shared/Epic Games/UE_5.7`；本機已建立 `Ashfrontier.uproject`、`Source/Ashfrontier/` C++ module、初始 Config、HUD / Controller / Character placeholder、C++ automation smoke test 與真正的 `Content/Maps/L_Ashfrontier_Prototype.umap`。
- 已驗證：`AshfrontierEditor` C++ build 成功；Gate 01 map 由 UE Editor 建立並 MapCheck 0 errors / 0 warnings；`Ashfrontier.Smoke.ModuleLoads` automation test 通過。
- Gate commit：`7b68327b2975a13746c88921508d5e8738e24a90`。
- Gate 01 推送狀態記錄 commit：`15add1786bedf258e93a9c0c696c5f580b611bc2`。
- 風險：磁碟可用空間約 `12-16GiB`，後續 Gate 的 DerivedDataCache、cook 或 package 可能需要釋放空間。

## Gate 02 狀態

- 狀態：完成並已推送。
- 目標：建立 `Content/Data/` 初始原創資料，強化 `Scripts/content_lint.py`，並用 UE automation test 確認資料可由 Editor 載入。
- 已驗證：`python3 Scripts/content_lint.py` 通過；`AshfrontierEditor` C++ build 通過；`Ashfrontier.Data.ContentJsonLoads` 與 `Ashfrontier.Smoke.ModuleLoads` automation tests 通過；`./Scripts/validate.sh` 通過。
- Gate commit：`0260776b81f3a9aac40952d708e1ca4e604a2e54`。
- Gate 02 推送狀態記錄 commit：`0b149fdc3d71242370073bee54197a67e4d87931`。
- 已納入 playable contract：Gate 02 不宣稱可玩；它只建立後續小隊、戰鬥、招募、交易、建造、生產、法規與存讀檔可共用的資料基底。Gate 03 必須交付 UE Editor 內可控制小隊移動。

## Gate 03 狀態

- 狀態：完成並已推送。
- 目標：在 UE Editor 內交付第一個可操作小隊 loop，至少 2 名隊員可被選取、移動、跟隨、待命，並可切換第三人稱與戰術鏡頭。
- 已完成：`SquadManager`、`OrderSystem`、`CameraController`、可見 placeholder character、runtime prototype floor、PlayerController input flow、HUD 狀態 placeholder 與 `Ashfrontier.Squad.OrderStateFlow` automation test。
- 已驗證：`AshfrontierEditor` C++ build 通過；`Ashfrontier.Squad.OrderStateFlow`、`Ashfrontier.Data.ContentJsonLoads` 與 `Ashfrontier.Smoke.ModuleLoads` automation tests 通過；`./Scripts/validate.sh` 通過。
- Gate commit：`7a941ef054a2b5880e7836a54b3a6d1733f03846`。
- Gate 03 推送狀態記錄 commit：`700c1dafff6835402ced528867f64f08de2f5e5d`。
- Playable milestone：Gate 03 結束後可在 UE Editor / PIE 內進入 `L_Ashfrontier_Prototype`，看見 2 名小隊員與 prototype floor，透過滑鼠選取、右鍵下達移動命令，並使用跟隨、待命與鏡頭切換流程。

## Gate 04 狀態

- 狀態：完成並已推送。
- 目標：建立主城 blockout、小型前哨、野外建造區、城市巡邏路線、商隊路線與 macOS perf capture 初版。
- 已完成：`AAshfrontierWorldBlockoutDirector` runtime world builder、`AAshfrontierRouteAgent` route follower、主城 / 前哨 / 野外建造區 zone records、城市守衛巡邏與玻璃屋商隊 route records、route markers、route agents 與 `Ashfrontier.World.BlockoutRoutes` automation test。
- 已驗證：`AshfrontierEditor` C++ build 通過；`Ashfrontier.World.BlockoutRoutes`、`Ashfrontier.Squad.OrderStateFlow`、`Ashfrontier.Data.ContentJsonLoads` 與 `Ashfrontier.Smoke.ModuleLoads` automation tests 通過；`Reports/perf-summary.md` 已輸出 Gate 04 初版效能摘要；`./Scripts/validate.sh` 通過。
- Gate commit：`532981c723b37d7e90b31ad6c161aa9cf6ea1a7b`。
- Gate 04 推送狀態記錄 commit：`7513977b10752ca1760b465364f74b7d4059e0f4`。
- 已知限制：Gate 04 world blockout 目前以 runtime actors 產生，NavMesh 尚未烘焙成正式 map asset；Gate 05 前可繼續使用直接 movement，Gate 04 後續若要嚴格驗證 NavMesh，需建立正式 level geometry 或 editor-time navmesh build。

## Gate 05 狀態

- 狀態：完成並已推送。
- 目標：完成「移動 → 戰鬥 → 受傷 → 倒地 → 搬運 → 醫療」Editor playable flow。
- 已完成：`UAshfrontierDamageModelComponent`、`UAshfrontierCombatResolverComponent`、`UAshfrontierCarrySystemComponent`、`UAshfrontierMedicalSystemComponent`、部位血量、流血、倒地 / 昏迷 / 穩定狀態、敵對角色 spawn、右鍵攻擊、`C` 搬運、`M` 包紮與 `Ashfrontier.Combat.DamageCarryMedical` automation test。
- 已驗證：`AshfrontierEditor` C++ build 通過；`Ashfrontier.Combat.DamageCarryMedical`、`Ashfrontier.World.BlockoutRoutes`、`Ashfrontier.Squad.OrderStateFlow`、`Ashfrontier.Data.ContentJsonLoads` 與 `Ashfrontier.Smoke.ModuleLoads` automation tests 通過；`./Scripts/validate.sh` 通過。
- Gate commit：`5fec17cbf2b96fab27a6a2d6ebd09984426da5c4`。
- Gate 05 推送狀態記錄 commit：`be4f90f7b586d594d7514d268d2cf3a6d043287c`。
- Playable milestone：Gate 05 結束後，Editor / PIE 內應可在現有 world blockout 中移動小隊、攻擊敵人、造成部位傷害與流血，讓角色倒地 / 昏迷，並用搬運與包紮流程穩定患者。

## Gate 06 狀態

- 狀態：完成並已推送。
- 目標：完成至少 4 名可招募角色、招募流程、角色庫存、價格資料與買賣流程。
- 已完成：`UAshfrontierInventoryComponent`、`UAshfrontierRecruitSystemComponent`、`UAshfrontierTradingSystemComponent`、4 名可招募 NPC、1 名商人 NPC、灰印幣付款、物品買賣、`R` 招募、`T` 買繃帶與 `Ashfrontier.Economy.RecruitInventoryTrade` automation test。
- 已驗證：`AshfrontierEditor` C++ build 通過；`Ashfrontier.Economy.RecruitInventoryTrade`、`Ashfrontier.Combat.DamageCarryMedical`、`Ashfrontier.World.BlockoutRoutes`、`Ashfrontier.Squad.OrderStateFlow`、`Ashfrontier.Data.ContentJsonLoads` 與 `Ashfrontier.Smoke.ModuleLoads` automation tests 通過；`./Scripts/validate.sh` 通過。
- Gate commit：`38affe04c59eb5e407b73d65fa26eb4c24a132b2`。
- Gate 06 推送狀態記錄 commit：`e4355aaaf92b5b1f64b0a1a1f6c5b8d9dccbd9b5`。
- 已知限制：交易 UI 仍是按鍵 / 測試流程，尚未有正式商店面板；醫療消耗品與包紮成本會在後續 Gate 更嚴格整合。

## Gate 07 狀態

- 狀態：完成並已推送。
- 目標：完成野外建造區建築放置、成本扣除、儲物、採集、食物鏈、金屬 / 建材鏈與生產隊列。
- 已完成：`AAshfrontierPlacedBuilding`、`UAshfrontierBuildingPlacementSystemComponent`、`UAshfrontierConstructionSystemComponent`、`UAshfrontierResourceNodeSystemComponent`、`UAshfrontierStorageSystemComponent`、`UAshfrontierProductionSystemComponent`、`B` 建造、`G` 採集、`P` 生產與 `Ashfrontier.Building.ProductionFlow` automation test。
- 已驗證：`AshfrontierEditor` C++ build 通過；smoke automation 7 項測試通過，包含 building / production、economy、combat、world、squad、data 與 smoke。
- Gate commit：`3491bbf1901678314c20539edf82098756d6d855`。
- Gate 07 推送狀態記錄 commit：`16fe61e539f1767c733100564606de927e0da562`。
- Playable milestone：Gate 07 結束後，Editor / PIE 內可完成「招募 → 交易 → 採集 → 建造 → 生產」流程。此 Gate 不宣稱 macOS packaged playable build 已完成。
- 已知限制：建築與 recipe runtime 定義目前由 C++ 對齊 Gate 02 JSON ID；正式資料載入器與 SaveGame 版本化會在 Gate 09 前補強。

## Gate 08 狀態

- 狀態：完成並已推送。
- 目標：完成 3 個派系的基本關係、城市守衛狀態、偷竊 / 攻擊 / 自衛 / 禁區判定、警告 / 敵對 / 追捕 / 放行與事件記憶。
- 已完成：`UAshfrontierFactionSystemComponent`、`UAshfrontierLegalRuleSystemComponent`、`UAshfrontierCrimeEventMemoryComponent`、`UAshfrontierGuardAIComponent`、角色 faction id、鹽脊守衛 placeholder、城市反應 debug input 與 `Ashfrontier.Legal.GuardReactionMemory` automation test。
- 已驗證：`AshfrontierEditor` C++ build 通過；`./Scripts/validate.sh` 通過；smoke automation 8 項測試通過，包含 legal、building / production、economy、combat、world、squad、data 與 smoke。
- Gate commit：`fb129892cc2d78eb8457123e583dbe426cbad67a`。
- Gate 08 推送狀態記錄 commit：`7df15b6e9e5fb90540bbebf1b1512f9e589b02ba`。
- 已知限制：法規 runtime 規則目前由 C++ 對齊 `Content/Data/legal_rules.json`；正式 JSON-to-runtime loader、AI 行為樹與 UI 提示仍待後續強化。

## Gate 09 狀態

- 狀態：本機實作與驗證中，等待最終 `./Scripts/validate.sh` 與 commit gate。
- 目標：完成 SaveGame round trip、全功能回歸、soak smoke、perf summary、macOS packaged build smoke 與最終中文報告。
- 已完成：`UAshfrontierSaveGameSystemComponent`、`UAshfrontierSaveGame` schema、角色位置 / 健康 / 庫存 / 派系關係 / 建築 / 生產隊列 / 事件記憶 snapshot 與 restore、`F5` 存檔、`F9` 讀檔、`Ashfrontier.SaveGame.RoundTripCoreState` automation test。
- 已驗證：`AshfrontierEditor` C++ build 通過；smoke automation 9 項測試通過，包含 SaveGame、legal、building / production、economy、combat、world、squad、data 與 smoke。
- macOS packaged build：已嘗試產出 `Builds/macOS/Ashfrontier.app`，命令列 smoke 啟動可掛載 pak、初始化 UE runtime 並載入 `/Game/Maps/L_Ashfrontier_Prototype`。Finder / Gatekeeper 簽章仍受 iCloud File Provider metadata 影響，需在 final report 與 known issues 中保留限制。
- Golden path：Editor playable flow 已由 automation 與 Gate 03 / 05 / 07 / 08 flow 覆蓋；packaged build 已完成啟動與 map load smoke，尚未宣稱完成真人手動 5 分鐘 packaged golden path。
