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

## Gate 00 環境限制

目前本機常見路徑與 Spotlight 尚未找到 UE5 Editor；active developer directory 是 `/Library/Developer/CommandLineTools`，`xcodebuild -version` 回報需要完整 Xcode。Gate 01 若要編譯 C++ UE 專案，需要先提供可用的 UE5 Editor 與完整 Xcode 或記錄 blocker。
