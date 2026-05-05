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
- Gate 03：小隊選取、移動、跟隨、待命、鏡頭切換與 UI placeholder 通過 functional tests。
- Gate 04：主城、前哨、野外建造區與巡邏路線可導航，perf capture 有輸出。
- Gate 05：近戰、部位傷害、流血、倒地、搬運與醫療通過 combat tests。
- Gate 06：招募、庫存、交易與價格資料通過流程測試。
- Gate 07：建築放置、成本扣除、採集、recipe、生產隊列與庫存流向通過測試。
- Gate 08：偷竊、攻擊、自衛、禁區、警告、追捕、放行、派系關係與事件記憶通過測試。
- Gate 09：存讀檔、回歸、soak、perf、macOS package 或 blocker report 完成。

## 失敗處理

若驗證失敗，不得 commit 或 push。必須先修復功能或測試；若是本機環境限制，建立繁體中文 blocker report，記錄精確原因、已完成替代驗證與下一步。
