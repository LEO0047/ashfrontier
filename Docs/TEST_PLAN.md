# Ashfrontier 測試計畫

## Summary

測試目標是證明 prototype 可操作、可戰鬥、可受傷、可搬運、可招募、可交易、可建造、可生產、可觸發城市反應、可存讀檔，並能在 macOS 上驗證或清楚記錄 blocker。

## 測試層級

| 類型 | 入口 | 目的 |
|---|---|---|
| 文件與環境檢查 | `Scripts/validate.sh` | 確認必要文件、繁體中文、Git remote、macOS 環境與腳本狀態 |
| Content lint | `python3 Scripts/content_lint.py` | 檢查資料缺欄、重複 ID、無效引用、價格錯誤、recipe 循環與非法建築成本 |
| Unit tests | `Scripts/run_tests.sh` | 驗證 C++ 純邏輯，例如傷害、庫存、配方、派系關係與 SaveGame schema |
| Functional tests | `Scripts/run_tests.sh` | 驗證 UE gameplay flow，例如選取、移動、戰鬥、招募、交易與建造 |
| Play-in-Editor tests | `Scripts/run_tests.sh` | 驗證 playable map、NavMesh、AI 巡邏與 UI placeholder |
| Soak test | `Scripts/soak_test.sh` | 長時間循環城市巡邏、商人移動、玩家建造、戰鬥與存讀檔 |
| Performance capture | `Scripts/perf_capture.sh` | 記錄主城壓力場景的 macOS / Metal 效能與設定 |
| macOS package smoke test | `Scripts/package_macos.sh` | 嘗試產出並啟動 `Builds/macOS/Ashfrontier.app`，或輸出 blocker report |

## Gate 00 驗證

- `git remote get-url origin` 必須是 `https://github.com/LEO0047/ashfrontier.git`。
- `Reports/environment.md` 必須存在並記錄 macOS、CPU 架構、Xcode / CLT、UE5 路徑、Git、Python。
- `Docs/Research/Kenshi2_DeepResearch_zh-TW.md` 必須存在。
- 必備中文文件與 ADR 必須存在。
- Shell scripts 必須可由 bash / zsh 載入。
- `Scripts/content_lint.py` 必須可執行並輸出 `Reports/content-lint.md` 與 `Reports/content-lint.json`。

## Gate 01 到 Gate 09 驗收重點

- Gate 01：UE5 專案可開啟、編譯、啟動空白 playable map，smoke tests 通過。
- Gate 02：資料驅動初始內容通過 content lint，且 `Ashfrontier.Data.ContentJsonLoads` 以 UE automation 證明 `Content/Data/*.json` 可由 Editor 載入與解析。
- Gate 03：小隊選取、移動、跟隨、待命、鏡頭切換與 UI placeholder 通過 functional tests；必須能在 UE Editor 內控制至少 2 名小隊成員移動。
- Gate 04：主城、前哨、野外建造區與巡邏路線可導航，perf capture 有輸出。
- Gate 05：近戰、部位傷害、流血、倒地、搬運與醫療通過 combat tests；必須能在 UE Editor 內完成「移動 → 戰鬥 → 受傷 → 倒地 → 搬運 → 醫療」流程。
- Gate 06：招募、庫存、交易與價格資料通過流程測試。
- Gate 07：建築放置、成本扣除、採集、recipe、生產隊列與庫存流向通過測試；必須能在 UE Editor 內完成「招募 → 交易 → 建造 → 生產」流程。
- Gate 08：偷竊、攻擊、自衛、禁區、警告、追捕、放行、派系關係與事件記憶通過測試。
- Gate 09：存讀檔、回歸、soak、perf、macOS package 或 blocker report 完成；若 packaged build 成功，必須不依賴 Unreal Editor 完成 5 分鐘 golden path。

## Golden Path 驗證

Gate 09 必須提供 5 分鐘 golden path，從新遊戲開始驗證：

1. 啟動 packaged build 或 Editor playable map。
2. 進入 prototype map。
3. 控制至少 2 名小隊成員。
4. 選取隊員並下達移動或跟隨命令。
5. 與至少 1 名敵人近戰。
6. 觀察部位傷害、倒地或昏迷。
7. 搬運倒地隊友並執行醫療。
8. 與至少 1 名 NPC 交易。
9. 招募至少 1 名 NPC。
10. 在野外建造區放置至少 1 種建築。
11. 取得或產出至少 1 種資源。
12. 存檔並讀檔。
13. 確認讀檔後角色位置、血量、庫存、派系關係與建築狀態沒有全部重置。

## 失敗處理

若驗證失敗，不得 commit 或 push。必須先修復功能或測試；若是本機環境限制，建立繁體中文 blocker report，記錄精確原因、已完成替代驗證與下一步。

不得把「測試腳本存在」當成「測試通過」，也不得把「UE project 能開」或「map 能載入」當成 prototype 完成。placeholder asset 不得阻止 gameplay loop；缺資產時使用 cube、capsule、primitive mesh 或 debug UI 也必須把流程做出來。

## Gate 01 Smoke Test 策略

- `Source/Ashfrontier/Private/Tests/AshfrontierSmokeTests.cpp` 提供 `Ashfrontier.Smoke.ModuleLoads` automation test，確認 runtime module 已載入。
- `Scripts/create_gate01_map.sh` 會呼叫 UE Editor 與 `BuildScripts/create_gate01_map.py`，建立真正的 `/Game/Maps/L_Ashfrontier_Prototype` map asset。
- `Scripts/run_tests.sh --smoke` 在 `Ashfrontier.uproject` 存在後必須使用 `UE5_EDITOR` 執行 UE automation；若 `UE5_EDITOR` 未設定或不可執行，驗證必須失敗，不得宣稱 Gate 01 通過。
- `Content/Maps/L_Ashfrontier_Prototype.umap` 必須由 UE Editor / commandlet 建立並可啟動；不得以 placeholder 文字檔替代。

## Gate 02 Content Data Test 策略

- `Scripts/content_lint.py` 必須檢查 Gate 02 的最低內容量：3 個派系、12 種物品、5 種資源、5 種建築、2 條以上 recipe、3 組 NPC schedule、legal rules 與 dialogue conditions。
- Content lint 必須阻擋缺欄、ID 格式錯誤、重複 ID、無效 faction / item / resource / building reference、價格或成本非法、recipe 循環、非法 legal event / reaction 與 dialogue condition。
- UE automation test `Ashfrontier.Data.ContentJsonLoads` 必須確認每個 `Content/Data/*.json` 檔案存在、可解析、`type` 正確、`records` 非空且基本筆數符合 Gate 02。
- Gate 02 的通過只代表資料基礎可用，不代表 gameplay loop 已完成；Gate 03 起必須開始交付 Editor playable 行為。

## Gate 03 Squad Control Test 策略

- UE automation test `Ashfrontier.Squad.OrderStateFlow` 必須建立 2 名小隊成員，驗證預設選取、全選、移動命令、跟隨命令、待命命令與戰術鏡頭切換。
- `Scripts/run_tests.sh --smoke` 必須同時通過 `Ashfrontier.Smoke.ModuleLoads`、`Ashfrontier.Data.ContentJsonLoads` 與 `Ashfrontier.Squad.OrderStateFlow`。
- Gate 03 的人工 playable smoke 重點是在 UE Editor / PIE 內進入 `L_Ashfrontier_Prototype`，確認玩家看得到 runtime prototype floor 與 2 名小隊員，且能用滑鼠選取與右鍵移動。此項若無法以 headless automation 完整證明，必須在 Gate report 中標明已完成自動化替代驗證與仍需人工確認的範圍。

## Gate 04 World Blockout Test 策略

- UE automation test `Ashfrontier.World.BlockoutRoutes` 必須建立 world blockout director，驗證 3 個 zone、2 條 route、至少 7 個 route waypoint 與 2 個 route agent。
- `Scripts/run_tests.sh --smoke` 必須同時通過 `Ashfrontier.Smoke.ModuleLoads`、`Ashfrontier.Data.ContentJsonLoads`、`Ashfrontier.Squad.OrderStateFlow` 與 `Ashfrontier.World.BlockoutRoutes`。
- `Scripts/perf_capture.sh --gate04` 必須輸出 `Reports/perf-summary.md`，記錄 macOS 版本、CPU 架構、Apple Silicon / 顯示晶片、解析度與主城壓力區設定。
- Gate 04 的 NavMesh 驗證目前以路線資料與可移動 route agent 作替代；正式 NavMesh build 與 15 分鐘巡邏 soak 需要在 geometry 轉成穩定 map asset 後強化。

## Gate 05 Combat Flow Test 策略

- UE automation test `Ashfrontier.Combat.DamageCarryMedical` 必須驗證：
  - 近戰攻擊會造成指定部位傷害。
  - 傷害會造成流血。
  - 重複攻擊可讓角色倒地或昏迷。
  - 倒地 / 昏迷角色可被搬運。
  - 醫療包紮可停止流血並穩定患者。
  - 搬運者可放下患者。
- `Scripts/run_tests.sh --smoke` 必須同時通過 `Ashfrontier.Combat.DamageCarryMedical`、`Ashfrontier.World.BlockoutRoutes`、`Ashfrontier.Squad.OrderStateFlow`、`Ashfrontier.Data.ContentJsonLoads` 與 `Ashfrontier.Smoke.ModuleLoads`。
- Gate 05 的人工 playable smoke 重點是在 UE Editor / PIE 中完成「移動 → 攻擊敵人 → 造成部位傷害 → 倒地 / 昏迷 → 搬運 → 包紮」流程。

## Gate 06 Recruit / Inventory / Trade Test 策略

- UE automation test `Ashfrontier.Economy.RecruitInventoryTrade` 必須驗證：
  - 玩家角色可持有灰印幣與物品。
  - 可招募 NPC 會在付款後加入小隊。
  - 招募費用會從玩家庫存扣除。
  - 商人可賣出物品並收取灰印幣。
  - 玩家可賣出物品並取得灰印幣。
  - 交易後物品不會重複生成。
- `Scripts/run_tests.sh --smoke` 必須同時通過 economy、combat、world、squad、data 與 smoke tests。
- Gate 06 的人工 playable smoke 重點是在 UE Editor / PIE 中用 `R` 招募 NPC，並用 `T` 與商人購買野外繃帶。

## Gate 07 Building / Production Test 策略

- UE automation test `Ashfrontier.Building.ProductionFlow` 必須驗證：
  - 招募與交易可在基地流程前成立。
  - 野外建造區外不可放置建築，且失敗不會扣除材料。
  - 野外建造區內可放置營火灶、儲物箱、採集點與工坊。
  - 建築成本會從角色 inventory 扣除。
  - 資源採集會把對應 item 放入 inventory。
  - 食物鏈 recipe 會消耗乾穀袋與鹹水壺，產出烤行糧。
  - 金屬 / 建材鏈 recipe 會消耗碎鐵礦袋、粗鐵錠與回收木板，產出粗鐵錠與拼裝建材板。
  - 生產隊列會新增、完成並清空。
  - 儲物箱可存入與取出物品。
- `Scripts/run_tests.sh --smoke` 必須同時通過 building / production、economy、combat、world、squad、data 與 smoke tests。
- Gate 07 的人工 playable smoke 重點是在 UE Editor / PIE 中完成「招募 → 交易 → 採集 → 建造 → 生產」。目前操作入口為 `R`、`T`、`G`、`B`、`P`，正式 UI 仍留待後續 Gate。

## Gate 08 Legal / Guard Reaction Test 策略

- UE automation test `Ashfrontier.Legal.GuardReactionMemory` 必須驗證：
  - 玩家偷竊會觸發鹽脊守衛警告。
  - 玩家攻擊會觸發追捕，並讓守衛進入 hostile 狀態。
  - 玩家自衛會被放行，且不降低派系關係。
  - 玩家禁區闖入會觸發警告與關係變化。
  - 不同 legal profile 會影響不同 authority faction 的關係。
  - 事件記憶可查詢，並會在時間推進後過期。
- `Scripts/run_tests.sh --smoke` 必須同時通過 legal、building / production、economy、combat、world、squad、data 與 smoke tests。
- Gate 08 的人工 playable smoke 重點是在 UE Editor / PIE 中用游標指向鹽脊守衛 placeholder，再透過 `V`、`K`、`U`、`N` 觸發偷竊、攻擊、自衛與禁區闖入反應。
