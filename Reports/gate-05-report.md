# Gate 05 報告

## 狀態

- Gate：Gate 05 — 戰鬥、部位傷害、倒地、搬運與醫療。
- 狀態：本機 `./Scripts/validate.sh` 通過，等待 commit gate 執行最終 validate / commit / push。
- 日期：2026-05-06。

## 完成內容

- 新增 `EAshfrontierBodyPart`，包含頭、胸、腹、左臂、右臂、左腿、右腿。
- 新增 `FAshfrontierBodyPartHealth` 與 `UAshfrontierDamageModelComponent`：
  - 各部位獨立血量。
  - 流血速度。
  - 倒地、昏迷與已穩定狀態。
- 新增 `UAshfrontierCombatResolverComponent`：
  - 簡化近戰 attack resolve。
  - 可指定攻擊部位與傷害量。
- 新增 `UAshfrontierCarrySystemComponent`：
  - 可搬運倒地、昏迷或已穩定角色。
  - 被搬運角色會跟隨搬運者位置並暫停碰撞。
- 新增 `UAshfrontierMedicalSystemComponent`：
  - 包紮可停止流血。
  - 倒地 / 昏迷患者包紮後轉為已穩定。
- 擴充 `AAshfrontierCharacter`：
  - 內建 DamageModel。
  - 支援 team：玩家小隊、中立、敵對。
  - 支援 carried target / carrier 狀態。
- 擴充 `AAshfrontierPlayerController`：
  - 右鍵敵對角色會讓選取隊員近戰攻擊。
  - `C` 嘗試搬運游標下的倒地角色。
  - `M` 嘗試包紮被搬運角色或游標下角色。
- 擴充 `AAshfrontierGameMode`：
  - 在 runtime world blockout 內產生 1 名敵對 placeholder 角色「塵路劫手」。
- 新增 UE automation test `Ashfrontier.Combat.DamageCarryMedical`。

## 測試結果

- `Build.sh AshfrontierEditor Mac Development -Project=Ashfrontier.uproject`：通過。
- `./Scripts/run_tests.sh --smoke`：通過。
- `./Scripts/validate.sh`：通過，0 個失敗。
- UE automation：
  - `Ashfrontier.Combat.DamageCarryMedical`：Success。
  - `Ashfrontier.Data.ContentJsonLoads`：Success。
  - `Ashfrontier.Smoke.ModuleLoads`：Success。
  - `Ashfrontier.Squad.OrderStateFlow`：Success。
  - `Ashfrontier.World.BlockoutRoutes`：Success。

## Playable Contract 狀態

- Gate 05 的不可協商 milestone 已進入可驗證狀態：系統支援移動後攻擊、部位傷害、流血、倒地 / 昏迷、搬運與包紮。
- 目前自動化驗證覆蓋核心 combat/carry/medical 狀態流；滑鼠右鍵攻擊、`C` 搬運與 `M` 包紮的實際手感屬於 Editor / PIE 人工 smoke 範圍。
- Gate 05 不宣稱完整 playable prototype；招募、交易、建造、生產、城市法規反應與存讀檔仍在後續 Gate。

## macOS build / test 狀態

- macOS packaged build：本 Gate 未執行，屬於 Gate 09。
- Editor automation：已於 macOS / UE5.7.4 / Apple Silicon workflow 通過。

## 已知問題

- 近戰尚未包含動畫、攻擊冷卻、命中距離檢查或武器差異。
- 部位傷害尚未連動速度、攻擊能力或義肢槽位。
- 搬運目前以位置跟隨方式呈現，尚未使用正式 attachment socket 或動畫姿勢。
- 包紮目前只停止流血並穩定患者，尚未消耗庫存中的醫療物品；這會在 Gate 06 庫存 / 物品系統整合時補上。

## 下一個 Gate 重點

- Gate 06 必須完成至少 4 名可招募角色、招募流程、庫存、價格資料與交易流程。
- Gate 06 需要把醫療包紮與物品 / 庫存開始接起來，避免醫療無成本。
