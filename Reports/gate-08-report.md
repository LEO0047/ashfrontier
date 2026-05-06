# Gate 08 報告

## 狀態

- Gate：Gate 08 — 派系 AI、城市守衛反應與事件記憶。
- 狀態：本機 C++ build 與 smoke automation 通過，等待完整 `./Scripts/validate.sh` 與 commit gate。
- 日期：2026-05-06。

## 完成內容

- 新增 `EAshfrontierLegalEventType`：
  - 偷竊。
  - 攻擊。
  - 自衛。
  - 禁區闖入。
- 新增 `EAshfrontierGuardReaction`：
  - 放行。
  - 警告。
  - 敵對。
  - 追捕。
- 新增 `EAshfrontierGuardState`：
  - 巡守。
  - 放行。
  - 警告。
  - 敵對。
  - 追捕。
- 新增 `UAshfrontierFactionSystemComponent`：
  - 維護鹽脊守望、玻璃屋商盟、塵路行者與玩家小隊的派系關係。
  - 支援關係查詢、設定、調整與 hostile 判定。
- 新增 `UAshfrontierLegalRuleSystemComponent`：
  - 對齊 `Content/Data/legal_rules.json` 的三組 legal profile。
  - 支援偷竊、攻擊、自衛、禁區闖入的 rule evaluation。
- 新增 `UAshfrontierCrimeEventMemoryComponent`：
  - 保存事件記憶。
  - 支援依違規派系與事件類型查詢。
  - 支援時間推進後移除過期記憶。
- 新增 `UAshfrontierGuardAIComponent`：
  - 將 legal rule 結果轉成守衛狀態。
  - 警告不會讓守衛敵對。
  - 追捕與敵對會讓守衛 hostile。
  - 自衛會放行且不降低派系關係。
- 擴充 `AAshfrontierCharacter`：
  - 新增 `FactionId`。
- 擴充 `AAshfrontierGameMode`：
  - 生成 1 名鹽脊守衛 placeholder。
- 擴充 `AAshfrontierPlayerController`：
  - `V` 觸發偷竊反應。
  - `K` 觸發攻擊反應。
  - `U` 觸發自衛反應。
  - `N` 觸發禁區闖入反應。
- 新增 UE automation test `Ashfrontier.Legal.GuardReactionMemory`。

## 測試結果

- `Build.sh AshfrontierEditor Mac Development -Project=Ashfrontier.uproject`：通過。
- `./Scripts/run_tests.sh --smoke`：通過。
- UE automation：
  - `Ashfrontier.Building.ProductionFlow`：Success。
  - `Ashfrontier.Combat.DamageCarryMedical`：Success。
  - `Ashfrontier.Data.ContentJsonLoads`：Success。
  - `Ashfrontier.Economy.RecruitInventoryTrade`：Success。
  - `Ashfrontier.Legal.GuardReactionMemory`：Success。
  - `Ashfrontier.Smoke.ModuleLoads`：Success。
  - `Ashfrontier.Squad.OrderStateFlow`：Success。
  - `Ashfrontier.World.BlockoutRoutes`：Success。

## Playable Contract 狀態

- Gate 08 已補上城市守衛反應與事件記憶的 Editor / PIE prototype 入口。
- 玩家可在既有 playable flow 外，用游標指向鹽脊守衛 placeholder，透過 `V`、`K`、`U`、`N` 驗證偷竊、攻擊、自衛與禁區闖入反應。
- Gate 08 不宣稱完整 playable prototype；完整存讀檔、5 分鐘 golden path 與 macOS packaged build 仍在 Gate 09。

## macOS build / test 狀態

- macOS packaged build：本 Gate 未執行，屬於 Gate 09。
- Editor automation：已於 macOS / UE5.7.4 / Apple Silicon workflow 通過。

## 已知問題

- 法規 runtime 規則目前由 C++ 對齊 `Content/Data/legal_rules.json`，尚未建立正式 JSON-to-runtime loader。
- 守衛 AI 目前是 rule-driven component，尚未接 StateTree / Behavior Tree。
- 城市反應 UI 仍是 debug HUD 與按鍵入口，尚未有正式警告對話或追捕提示。
- 事件記憶尚未接 SaveGame；此項屬 Gate 09。

## 下一個 Gate 重點

- Gate 09 必須完成存讀檔、回歸、soak、perf capture、macOS packaged build 嘗試與最終中文報告。

## 推送紀錄

- Gate commit：等待 commit gate。
- Push 狀態：等待 commit gate。
