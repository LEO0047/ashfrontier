# Gate 06 報告

## 狀態

- Gate：Gate 06 — 招募、角色資料、庫存與交易。
- 狀態：本機 `./Scripts/validate.sh` 通過，等待 commit gate 執行最終 validate / commit / push。
- 日期：2026-05-06。

## 完成內容

- 新增 `UAshfrontierInventoryComponent`：
  - 支援 item stack。
  - 支援新增、移除、查詢物品。
- 新增 `UAshfrontierRecruitSystemComponent`：
  - 檢查 NPC 是否可招募。
  - 檢查玩家是否有足夠 `item_ash_credit`。
  - 扣款後將 NPC 加入小隊。
- 新增 `UAshfrontierTradingSystemComponent`：
  - 內建 Gate 02 item 價格表。
  - 支援買入與賣出。
  - 轉移灰印幣與物品庫存。
- 擴充 `AAshfrontierCharacter`：
  - 角色 inventory。
  - 可招募狀態。
  - 招募費用。
  - 商人狀態。
- 擴充 `AAshfrontierGameMode`：
  - 生成 4 名可招募 NPC。
  - 生成 1 名玻璃屋商人 NPC。
  - 商人持有繃帶、行糧、醫療壓布與灰印幣庫存。
- 擴充 `AAshfrontierPlayerController`：
  - `R` 嘗試招募游標下 NPC。
  - `T` 嘗試向游標下商人購買野外繃帶。
- 新增 UE automation test `Ashfrontier.Economy.RecruitInventoryTrade`。

## 測試結果

- `Build.sh AshfrontierEditor Mac Development -Project=Ashfrontier.uproject`：通過。
- `./Scripts/run_tests.sh --smoke`：通過。
- `./Scripts/validate.sh`：通過，0 個失敗。
- UE automation：
  - `Ashfrontier.Combat.DamageCarryMedical`：Success。
  - `Ashfrontier.Data.ContentJsonLoads`：Success。
  - `Ashfrontier.Economy.RecruitInventoryTrade`：Success。
  - `Ashfrontier.Smoke.ModuleLoads`：Success。
  - `Ashfrontier.Squad.OrderStateFlow`：Success。
  - `Ashfrontier.World.BlockoutRoutes`：Success。

## Playable Contract 狀態

- Gate 06 已補上招募、庫存與交易的核心資料流。
- 玩家在 Editor / PIE 中可用 `R` 招募游標下可招募 NPC，並用 `T` 與商人購買野外繃帶。
- Gate 06 不宣稱完整 playable prototype；基地建設、生產鏈、城市法規反應與存讀檔仍在後續 Gate。

## macOS build / test 狀態

- macOS packaged build：本 Gate 未執行，屬於 Gate 09。
- Editor automation：已於 macOS / UE5.7.4 / Apple Silicon workflow 通過。

## 已知問題

- 商店 UI 仍是按鍵測試流程，尚未有正式商品清單。
- 招募對話仍是系統觸發，尚未有正式 dialogue UI。
- 醫療包紮尚未消耗 inventory 中的醫療物品；Gate 07 或 Gate 09 回歸前需要接上。
- 招募角色資料仍是 runtime seed，尚未完全轉成資料表驅動。

## 下一個 Gate 重點

- Gate 07 必須完成野外建造區建築放置、成本扣除、儲物、採集、生產與至少兩條生產鏈。
- Gate 07 需要把建築與 recipe 資料實際接到 runtime 系統。
