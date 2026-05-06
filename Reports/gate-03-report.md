# Gate 03 報告

## 狀態

- Gate：Gate 03 — 小隊控制、鏡頭與移動。
- 狀態：本機 `./Scripts/validate.sh` 通過，等待 commit gate 執行最終 validate / commit / push。
- 日期：2026-05-06。

## 完成內容

- 新增 `EAshfrontierSquadOrder`，定義 `Idle`、`Moving`、`Following`、`Holding`。
- 擴充 `AAshfrontierCharacter`：
  - 可見 placeholder body。
  - selection marker。
  - 小隊編號、顯示名稱、選取狀態。
  - 移動、跟隨、待命命令狀態。
  - 第三人稱 camera boom 與戰術 zoom arm length 控制。
- 新增 `UAshfrontierSquadManagerComponent`：
  - Gate 03 預設建立 2 名隊員。
  - 支援選取單一隊員、追加選取、全選、切換下一名隊員。
- 新增 `UAshfrontierOrderSystemComponent`：
  - 支援對目前選取隊員下達移動、跟隨與待命命令。
  - 多角色移動時套用簡易隊形 offset。
- 新增 `UAshfrontierCameraControllerComponent`：
  - 支援主要選取隊員 view target。
  - 支援第三人稱與戰術鏡頭切換。
- 擴充 `AAshfrontierPlayerController`：
  - 左鍵選取、右鍵移動。
  - `F` 跟隨、`H` 待命、`Q` 全選、`E` 選下一名、`Tab` 切換鏡頭、`1-4` 選取指定隊員。
- 擴充 `AAshfrontierGameMode`：
  - BeginPlay 產生 runtime prototype floor，讓空白 map 也能進行滑鼠 trace 與移動命令。
- 擴充 `AAshfrontierHUD`：
  - 顯示小隊數、選取數、鏡頭狀態、隊員名稱與命令狀態 placeholder。
- 新增 UE automation test `Ashfrontier.Squad.OrderStateFlow`。
- 更新 `Scripts/run_tests.sh`，由 UE automation `index.json` 產出較準確的 `Reports/test-summary.md` 與 `Reports/test-results.xml`。

## 測試結果

- `Build.sh AshfrontierEditor Mac Development -Project=Ashfrontier.uproject`：通過。
- `./Scripts/run_tests.sh --smoke`：通過。
- `./Scripts/validate.sh`：通過，0 個失敗。
- UE automation：
  - `Ashfrontier.Data.ContentJsonLoads`：Success。
  - `Ashfrontier.Smoke.ModuleLoads`：Success。
  - `Ashfrontier.Squad.OrderStateFlow`：Success。

## Playable Contract 狀態

- Gate 03 的不可協商 milestone 已進入可驗證狀態：UE Editor / PIE 內會建立 2 名小隊成員，並支援選取、移動、跟隨、待命與鏡頭切換。
- 目前自動化驗證覆蓋小隊建立與命令狀態流；滑鼠點擊實際手感屬於 Editor / PIE 人工 smoke 範圍。
- Gate 03 不宣稱完整 playable prototype；戰鬥、受傷、搬運、醫療、交易、招募、建造、生產與存讀檔仍在後續 Gate。

## macOS build / test 狀態

- macOS packaged build：本 Gate 未執行，屬於 Gate 09。
- Editor automation：已於 macOS / UE5.7.4 / Apple Silicon workflow 通過。

## 已知問題

- Runtime prototype floor 是 Gate 03 臨時方案；Gate 04 必須用正式世界 blockout 與 NavMesh 取代。
- 目前小隊移動使用直接 movement input，不依賴 NavMesh；Gate 04 後需銜接導航與路徑。
- 目前 UI 是 debug HUD placeholder，不是最終 UI。

## 下一個 Gate 重點

- Gate 04 要建立主城 blockout、小型前哨、野外建造區、巡邏與商隊路線。
- Gate 04 需要開始驗證 NavMesh 與 NPC 路線，不再只依賴 Gate 03 的 runtime floor。

## 推送紀錄

- Gate commit：7a941ef054a2b5880e7836a54b3a6d1733f03846
- Push 狀態：已推送到 origin/main
- Remote SHA：7a941ef054a2b5880e7836a54b3a6d1733f03846
- 記錄時間：2026-05-06 19:30:09 CST
