# Ashfrontier 技術規格

## Summary

Ashfrontier 使用 UE5 建立 3D 單機小隊沙盒 RPG prototype。核心系統優先使用 C++，Blueprint 用於組裝、展示、資料配置與快速驗證。macOS 與 Metal 相容性是優先目標；若本機 UE5、Xcode 或簽章限制導致某項驗證不可執行，必須輸出中文 blocker report。

## UE5 架構

- 專案名稱：`Ashfrontier`。
- 目標平台：macOS 優先，不要求 Windows build。
- Runtime module：Gate 01 建立 `Source/Ashfrontier/`。
- 核心 C++ 系統：GameMode、PlayerController、Character / Pawn、SquadManager、OrderSystem、CameraController、CombatResolver、DamageModel、InventorySystem、TradingSystem、BuildingPlacementSystem、ProductionSystem、FactionSystem、LegalRuleSystem、SaveGame schema。
- Blueprint 用途：map 組裝、UI placeholder、資料資產配置、測試場景、blockout actor。
- Content root：`Content/`。
- C++ root：`Source/`。

## macOS / Metal 相容性

- Renderer 以 Metal 相容為準。
- 避免 Windows-only API、DirectX-only shader 假設與 Windows-only plugin。
- Nanite / Lumen 若在本機 macOS 或 Apple Silicon 上有限制，必須記錄替代設定。
- UE5 路徑由 `UE5_EDITOR` 與 `UE5_ROOT` 指定，不硬寫死。
- 打包目標是 `Builds/macOS/Ashfrontier.app`。
- 原型階段不強制 Apple notarization；若需要簽章，優先採 ad-hoc signing 或 development signing。

## Build Truth Rule

若最終聲稱 macOS playable build 完成，必須同時滿足：

1. `Builds/macOS/Ashfrontier.app` 存在。
2. 該 `.app` 可從 Finder 或命令列啟動。
3. 啟動後不需要 Unreal Editor。
4. 啟動後可以進入 playable map。
5. 可以完成 5 分鐘 golden path。
6. `Reports/final-prototype-report.md` 必須包含 build 路徑、build 時間、commit hash、測試摘要、已知問題與 5 分鐘 golden path 驗證結果。

Blocker report 只能代表目前環境阻塞，不能代表 goal 完成。若 packaged build 失敗，仍必須保留 Editor playable prototype；若 Editor playable prototype 也無法完成，最終狀態必須明確標示 prototype 未完成。

## 資料驅動設計

以下內容必須資料化，並由 `Scripts/content_lint.py` 驗證：

- `factions`
- `items`
- `resources`
- `recipes`
- `buildings`
- `npc_schedules`
- `dialogue_conditions`
- `legal_rules`

Gate 02 起資料可放在 `Content/Data/` 或 `Source/Data/`，格式需符合 `Docs/CONTENT_SCHEMA.md`。資料 ID 使用穩定 ASCII slug，使用者可見名稱與描述使用繁體中文。

Gate 02 初始內容已採 `Content/Data/*.json`：

- 派系：鹽脊守望、玻璃屋商盟、塵路行者。
- 物品：12 種基礎交易、食物、材料、醫療、武器與護具項目。
- 資源：5 種野外可取得資源，並可映射到 item。
- 建築：營火灶、粗木儲物箱、碎鐵採集點、野地工坊、釘板防線。
- 配方：食物鏈與金屬 / 建材鏈。
- NPC schedule、dialogue conditions 與 legal rules：作為 Gate 04、Gate 06 與 Gate 08 的可載入資料基礎。

`Scripts/content_lint.py` 會檢查最低筆數、必填欄位、ID 格式、重複 ID、價格與成本、recipe 循環、faction reference、building / item / resource reference、legal event / reaction 與 dialogue condition。UE automation test `Ashfrontier.Data.ContentJsonLoads` 會確認 UE Editor 可以從 `Content/Data/` 載入並解析這些 JSON。

## AI 架構

- 高階狀態：StateTree 或自訂狀態機。
- 局部行為：Behavior Tree 或等效實作。
- 決策：utility scoring 或 rule priority。
- 記憶：event memory、crime memory、faction reaction memory。
- 城市守衛必須能判斷偷竊、攻擊、自衛與禁區闖入，並觸發警告、敵對、追捕或放行。

## 戰鬥與健康

部位傷害 schema 必須包含：

- 頭
- 胸
- 腹
- 左臂
- 右臂
- 左腿
- 右腿

健康狀態至少包含正常、流血、倒地、昏迷、包紮中、已穩定。肢體損傷或義肢槽位可先簡化，但必須能存讀。

## 存讀檔版本策略

SaveGame schema 必須包含 `schema_version`。Gate 09 前至少保存：

- 玩家小隊位置
- 角色健康狀態
- 庫存
- 裝備
- 派系關係
- 建築狀態
- 生產隊列
- NPC 重要狀態

版本升級策略：

1. 新欄位必須有預設值。
2. 舊版本載入時先轉換到最新 runtime 結構。
3. `content_lint.py` 與 UE automation tests 必須覆蓋常見 schema 缺漏。
4. 不得用一次性 Blueprint 變數取代正式 SaveGame schema。

## 驗證方式

- `Scripts/validate.sh`：主要 Gate 驗證入口。
- `Scripts/content_lint.py`：資料表與 schema lint。
- `Scripts/run_tests.sh`：UE automation / functional tests。
- `Scripts/soak_test.sh`：30 分鐘 soak 或 smoke mode。
- `Scripts/perf_capture.sh`：主城壓力場景效能擷取。
- `Scripts/package_macos.sh`：macOS `.app` 打包與 blocker report。
- `Scripts/commit_gate.sh`：驗證通過後 commit / push / 報告狀態。

## Gate 01 專案骨架

- `Ashfrontier.uproject` 宣告 `Ashfrontier` runtime module，啟用 `EnhancedInput`。
- `Source/Ashfrontier/` 目前包含 Game / Editor target、module rules、GameMode、PlayerController、Character、HUD、C++ automation smoke test 與內容資料載入測試。
- `Config/DefaultEngine.ini` 指向 `/Game/Maps/L_Ashfrontier_Prototype` 作為 startup / default map；此 `.umap` 必須由 UE Editor 建立，不得用文字檔或空檔偽造。
- 完整 Xcode 已安裝並完成 agreement；active developer directory 是 `/Applications/Xcode.app/Contents/Developer`。
- UE5.7.4 已安裝於 `/Users/Shared/Epic Games/UE_5.7`，Gate 01 的 C++ build、map 建立與 UE automation smoke test 已通過。

## Gate 02 資料載入

- C++ module 新增 `Json` private dependency，讓 automation test 可用 UE 原生 JSON parser 載入 `Content/Data/*.json`。
- Gate 02 不把 Python lint 當成唯一證據；`Scripts/run_tests.sh` 會執行 `Automation RunTests Ashfrontier`，包含 `Ashfrontier.Data.ContentJsonLoads`。
- 這一階段仍不代表 playable prototype；它只建立後續小隊、世界、戰鬥、交易、建造與法規系統可共用的資料基底。

## Gate 03 小隊控制

- `AAshfrontierCharacter` 現在包含 placeholder body、selection marker、squad index、display name、order state、move destination、follow target 與第三人稱 camera boom。
- `UAshfrontierSquadManagerComponent` 負責建立 1 到 4 名隊員、維護選取集合、更新 selection marker，並在 Gate 03 預設建立 2 名隊員。
- `UAshfrontierOrderSystemComponent` 負責把移動、跟隨與待命命令套用到目前選取隊員；多角色移動會套用簡易橫向隊形 offset。
- `UAshfrontierCameraControllerComponent` 負責把視角聚焦到主要選取隊員，並在第三人稱與戰術 arm length 之間切換。
- `AAshfrontierGameMode` 會在 BeginPlay 產生 runtime prototype floor，讓右鍵 trace 與角色移動在空白 map 中也能成立。Gate 04 會改以正式 blockout / NavMesh 取代這個臨時地面。
- `AAshfrontierHUD` 目前提供小隊數、選取數、鏡頭狀態、隊員名稱與命令狀態 placeholder。此 HUD 是 debug / prototype UI，不是最終 UX。

## Gate 04 世界 Blockout 與路線

- `AAshfrontierWorldBlockoutDirector` 是 Gate 04 的 runtime world builder，會建立：
  - `zone_salt_spine_city`：鹽脊主城。
  - `zone_cinder_outpost`：燼沙前哨。
  - `zone_wilderness_yard`：裂土建造區。
- Director 會生成主城地面、城牆、城門、前哨塔、野外工坊區與 route markers，全部使用 UE basic cube mesh 作為 placeholder。
- `AAshfrontierRouteAgent` 繼承 `AAshfrontierCharacter`，使用 route waypoints 進行循環移動。Gate 04 先建立兩條路線：
  - `route_city_guard_patrol`：城市守衛巡邏，4 個 waypoint。
  - `route_glasshouse_caravan`：商隊 / NPC 移動路線，4 個 waypoint。
- Gate 04 仍使用直接 movement input 執行 route movement；正式 NavMesh 與路徑尋路會在世界 geometry 穩定後接上。
- `Scripts/perf_capture.sh` Gate 04 會輸出 macOS、Apple Silicon、顯示環境與主城壓力區設定。此階段尚不是 UE Insights 或 Metal frame capture 的最終效能報告。
