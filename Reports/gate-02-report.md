# Gate 02 報告

## 狀態

- Gate：Gate 02 — 資料驅動 schema 與內容驗證。
- 狀態：本機 `./Scripts/validate.sh` 通過，等待 commit gate 執行最終 validate / commit / push。
- 日期：2026-05-06。

## 完成內容

- 新增 `Content/Data/` 初始原創資料集：
  - 3 個原創派系：鹽脊守望、玻璃屋商盟、塵路行者。
  - 12 種物品，涵蓋貨幣、食物、材料、醫療、武器與護具。
  - 5 種資源，作為採集與生產鏈輸入。
  - 5 種建築，涵蓋營火灶、儲物箱、採集點、工坊與防禦物。
  - 4 筆 recipe，涵蓋食物鏈與金屬 / 建材鏈。
  - 3 組 NPC schedule，涵蓋城市巡邏、商隊路線與野外巡撿。
  - 3 組 legal rules，涵蓋偷竊、攻擊、自衛與禁區闖入。
  - 3 組 dialogue conditions，供後續交易與招募流程使用。
- 強化 `Scripts/content_lint.py`：
  - 檢查最低資料筆數、ID 格式、重複 ID、必填欄位、價格、重量、成本與數量。
  - 檢查 faction、item、resource、building、recipe、legal profile 與 dialogue condition reference。
  - 檢查 recipe 循環、legal event / reaction 合法性與 schedule entry 完整性。
- 新增 UE automation test `Ashfrontier.Data.ContentJsonLoads`，確認 UE Editor 可載入並解析 `Content/Data/*.json`。
- 更新 `README.md`、`Docs/PLAN.md`、`Docs/TECH_SPEC.md`、`Docs/CONTENT_SCHEMA.md` 與 `Docs/TEST_PLAN.md`。

## 測試結果

- `python3 -m py_compile Scripts/content_lint.py`：通過。
- `python3 Scripts/content_lint.py`：通過，檢查 8 個資料檔，0 個 issue。
- `Build.sh AshfrontierEditor Mac Development -Project=Ashfrontier.uproject`：通過。
- `./Scripts/run_tests.sh --smoke`：通過。
- `./Scripts/validate.sh`：通過，0 個失敗。
- UE automation：
  - `Ashfrontier.Data.ContentJsonLoads`：Success。
  - `Ashfrontier.Smoke.ModuleLoads`：Success。

## Playable Contract 狀態

- Gate 02 不宣稱 prototype 已可玩。
- Gate 02 只完成 gameplay loop 的資料基底。
- 下一個不可協商 playable milestone 是 Gate 03：必須在 UE Editor 內控制至少 2 名小隊成員移動，並支援選取、移動、跟隨或移動命令的初版流程。

## macOS build / test 狀態

- macOS build：本 Gate 未執行 packaged build；packaged build 屬於 Gate 09。
- Editor automation：已於 macOS / UE5.7.4 / Apple Silicon workflow 通過。

## 已知問題

- 目前磁碟可用空間偏低，後續 Gate 的 cook、DerivedDataCache 或 macOS package 可能需要先釋放空間。
- Gate 02 尚未實作 gameplay runtime 載入後的 UI 或互動；這會在 Gate 03 起逐步落地。

## 下一個 Gate 重點

- Gate 03 必須開始交付 Editor playable：至少 2 名小隊成員、選取、移動、跟隨、待命、鏡頭與狀態 UI placeholder。
- Gate 03 的驗證不能只靠資料或文件，必須證明 UE Editor 內可以控制小隊移動。
