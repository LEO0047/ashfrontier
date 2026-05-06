# Gate 07 報告

## 狀態

- Gate：Gate 07 — 基地建設與生產鏈。
- 狀態：完成並已推送。
- 日期：2026-05-06。

## 完成內容

- 新增 `FAshfrontierBuildingDefinition`、`FAshfrontierResourceDefinition`、`FAshfrontierRecipeDefinition` 與 `FAshfrontierProductionQueueEntry`。
- 新增 `AAshfrontierPlacedBuilding`：
  - 保存建築 ID、名稱、類別、resource id。
  - 內含儲物 inventory。
  - 內含 production queue。
- 新增 `UAshfrontierConstructionSystemComponent`：
  - 驗證建築成本。
  - 扣除角色 inventory 中的材料。
- 新增 `UAshfrontierBuildingPlacementSystemComponent`：
  - 建築只能放在 `wilderness_build_zone`。
  - 支援營火灶、儲物箱、碎鐵採集點、野地工坊與釘板防線定義。
- 新增 `UAshfrontierResourceNodeSystemComponent`：
  - 支援採集旱地穀穗、鹹井水、碎鐵礦、舊棚木料與苦根藥草。
  - 支援從採集型建築取得資源。
- 新增 `UAshfrontierStorageSystemComponent`：
  - 支援角色與建築儲物 inventory 之間存入與取出物品。
- 新增 `UAshfrontierProductionSystemComponent`：
  - 檢查 recipe station。
  - 消耗 recipe inputs。
  - 新增 production queue。
  - 推進與完成生產。
  - 將 outputs 寫回角色或建築 inventory。
- 擴充 `AAshfrontierPlayerController`：
  - `G` 採集建造與生產所需資源。
  - `B` 在野外建造區放置營火灶。
  - `P` 執行烤行糧 recipe。
- 新增 UE automation test `Ashfrontier.Building.ProductionFlow`。

## 測試結果

- `Build.sh AshfrontierEditor Mac Development -Project=Ashfrontier.uproject`：通過。
- `./Scripts/run_tests.sh --smoke`：通過。
- UE automation：
  - `Ashfrontier.Building.ProductionFlow`：Success。
  - `Ashfrontier.Combat.DamageCarryMedical`：Success。
  - `Ashfrontier.Data.ContentJsonLoads`：Success。
  - `Ashfrontier.Economy.RecruitInventoryTrade`：Success。
  - `Ashfrontier.Smoke.ModuleLoads`：Success。
  - `Ashfrontier.Squad.OrderStateFlow`：Success。
  - `Ashfrontier.World.BlockoutRoutes`：Success。

## Playable Contract 狀態

- Gate 07 已補上 Editor / PIE 內的「招募 → 交易 → 採集 → 建造 → 生產」流程。
- 玩家可用既有小隊操作進入 prototype map，使用 `R` 招募、`T` 交易、`G` 採集、`B` 建造營火灶、`P` 生產烤行糧。
- Gate 07 不宣稱完整 playable prototype；城市守衛反應、存讀檔與 macOS packaged build 仍在後續 Gate。

## macOS build / test 狀態

- macOS packaged build：本 Gate 未執行，屬於 Gate 09。
- Editor automation：已於 macOS / UE5.7.4 / Apple Silicon workflow 通過。

## 已知問題

- 建築與 recipe runtime 定義目前由 C++ 對齊 Gate 02 JSON ID，尚未建立正式 JSON-to-runtime loader。
- 建築 placement 目前使用 Gate 04 裂土建造區座標作為簡化 volume，尚未接正式 NavMesh / collision placement preview。
- 生產流程可用 automation 驗證，正式建築 UI、recipe 清單 UI 與工作者派工 UI 仍未完成。
- 生產隊列已存在於建築 actor，但 SaveGame schema 尚未接上；此項屬 Gate 09。

## 下一個 Gate 重點

- Gate 08 必須完成派系關係、城市守衛反應、偷竊 / 攻擊 / 自衛 / 禁區判定、警告 / 追捕 / 放行與事件記憶。

## 推送紀錄

- Gate commit：等待 commit gate。
- Push 狀態：等待 commit gate。

## 推送紀錄

- Gate commit：3491bbf1901678314c20539edf82098756d6d855
- Push 狀態：已推送到 origin/main
- Remote SHA：3491bbf1901678314c20539edf82098756d6d855
- 記錄時間：2026-05-06 20:07:58 CST
