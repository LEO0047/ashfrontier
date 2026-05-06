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
- Gate 02：資料驅動初始內容通過 content lint，UE 可載入資料。
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
