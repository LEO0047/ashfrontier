# Ashfrontier 單一主線開發計畫

## Summary

本文件只描述一個主線 goal：在 macOS 工作區推進 Ashfrontier，建立 UE5 3D 單機小隊沙盒 RPG playable prototype，最後產出 macOS `.app` playable build，或清楚記錄阻塞條件。Gate 是同一條主線內的驗收節點，不是多個獨立專案。

## 研究文件轉譯原則

研究文件指出可採用的是系統設計方向，而不是內容照抄。Ashfrontier 的可執行原則如下：

- 先做「一城、一前哨、一野外建造區」，不要一開始追求超大世界。
- 系統核心優先於畫面：小隊控制、部位傷害、倒地搬運、交易、建造、生產、事件記憶與存讀檔必須先活起來。
- 內容資料化：派系、物品、資源、配方、建築、NPC schedule、legal rules 與 dialogue conditions 必須能被 lint 與測試解析。
- AI 採可擴充分層：高階狀態、局部行為、rule / utility decision 與 event memory 分開。
- macOS / Metal 相容性優先，避免 Windows-only plugin、DirectX-only 假設與不可移植建置流程。
- placeholder 可以粗，但互動 loop 必須可玩、可測、可存讀。

## Gate 序列

| Gate | 目標 | 主要驗收 |
|---|---|---|
| Gate 00 | macOS 工作區、文件、Git 與驗證骨架 | 文件、研究副本、Scripts、環境報告與 content lint 通過 |
| Gate 01 | UE5 專案骨架與核心模組 | `.uproject` 可開啟、核心 module 可編譯、空白 playable map 可啟動 |
| Gate 02 | 資料驅動 schema 與內容 lint | 原創 factions、items、resources、recipes、buildings、schedules、legal rules 通過 lint |
| Gate 03 | 小隊控制、鏡頭與移動 | 1 到 4 名隊員可選取、移動、跟隨、待命與切換鏡頭 |
| Gate 04 | 世界 blockout、導航、巡邏與 macOS 基礎效能 | 主城、前哨、野外建造區、NavMesh、巡邏與 perf capture 初版 |
| Gate 05 | 戰鬥、部位傷害、倒地、搬運與醫療 | 近戰、部位傷害、流血、昏迷、搬運與包紮可操作 |
| Gate 06 | 招募、角色資料、庫存與交易 | 4 名可招募角色、庫存、價格表與買賣流程可操作 |
| Gate 07 | 基地建設與生產鏈 | 5 種建築、食物鏈、金屬 / 建材鏈與生產隊列可存讀 |
| Gate 08 | 派系 AI、城市守衛反應與事件記憶 | 偷竊、攻擊、自衛、禁區、警告、追捕、放行與關係變化可測 |
| Gate 09 | 存讀檔、macOS 打包、回歸、soak test 與最終報告 | 完整存讀檔、回歸、soak、perf、macOS `.app` 或 blocker report |

## Gate 執行規則

每個 Gate 必須依序完成：

1. 實作 Gate 內容。
2. 更新 `Docs/PLAN.md`、`Docs/TECH_SPEC.md`、`Docs/TEST_PLAN.md` 或 `Docs/CONTENT_SCHEMA.md`。
3. 執行 `./Scripts/validate.sh`。
4. 若驗證失敗，先修復再重跑，不得 commit 失敗狀態。
5. 驗證通過後執行 `./Scripts/commit_gate.sh <gate-id> "<中文 commit message>"`。
6. push 後確認 `git status --porcelain` 乾淨，且 `HEAD` 與 `origin/main` 一致。
7. 將測試、commit、push、已知問題與下一個 Gate 重點寫入 `Reports/gate-XX-report.md`。

## Gate 00 狀態

- 狀態：完成並已推送。
- 範圍：建立中文文件、研究副本、macOS 環境報告、Scripts 與初始 content lint。
- Gate commit：`9e0a3c5c3730d397c108b2410eb6d683dc05a43a`。
- Gate 00 推送狀態記錄 commit：`c5de7ef072ce8090bb960c8a3f0c57ad4cc6b160`。

## Gate 01 狀態

- 狀態：環境 blocker。
- 原因：目前常見路徑與 Spotlight 尚未找到 UE5 Editor；`/Applications` 也未找到 `Xcode.app`，active developer directory 是 Command Line Tools，不是完整 Xcode。
- 影響：無法誠實建立 UE5 `.umap`、編譯 C++ module、驗證 UE project 可開啟，或產出 macOS `.app`。
- 下一步：安裝或提供 UE5 Editor 路徑，並安裝 / 指定完整 Xcode 後重跑 Gate 01。
