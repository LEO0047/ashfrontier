# 已知問題

## Summary

此清單記錄 Gate 09 截止時仍存在的限制。這些限制不會被包裝成已完成項目；若影響 packaged build 或 5 分鐘 golden path，會同步反映在 `Reports/final-prototype-report.md`。

## Gameplay / UX

- 操作介面仍是 prototype debug flow：`R` 招募、`T` 交易、`G` 採集、`B` 建造、`P` 生產、`V` / `K` / `U` / `N` 城市反應、`F5` / `F9` 存讀檔。正式選單、商店面板、建築面板與教學提示尚未完成。
- 世界 blockout 以 runtime cube / capsule / primitive placeholder 建立；尚未有正式美術、材質、動畫或最終地形。
- NavMesh 尚未轉成完整烘焙流程；目前以 runtime route agent、direct movement 與 automation flow 驗證可走動路線。
- 守衛 AI 是 rule-driven component，尚未接 StateTree 或 Behavior Tree。

## 驗證限制

- `Scripts/soak_test.sh --smoke` 已執行完整 UE automation smoke suite，但尚未執行 30 分鐘真人或 headless gameplay soak。
- `Scripts/perf_capture.sh --gate09` 已輸出 macOS / Apple Silicon / 解析度與測試模式，但尚未接 UE Insights trace 或 Metal frame capture。
- packaged build 已完成命令列啟動與 prototype map load smoke；真人手動 5 分鐘 packaged golden path 尚未完成，因此最終報告不能宣稱此項已人工通過。

## macOS Package / Signing

- repo 位於 iCloud Drive File Provider 工作區，`Builds/macOS/Ashfrontier.app` 根目錄會被重新附加 `com.apple.FinderInfo`、`com.apple.fileprovider.fpfs#P` 與 `com.apple.provenance` extended attributes。
- 這些 metadata 會讓 `codesign --force --sign - --timestamp=none --deep Builds/macOS/Ashfrontier.app` 在本工作區出現 `resource fork, Finder information, or similar detritus not allowed`。
- 命令列 executable smoke 已可啟動並載入 map；Finder / Gatekeeper 啟動仍需在非 iCloud 路徑或清乾淨 metadata 後另行驗證。
- `Builds/macOS/Ashfrontier.app` 是大型本機打包產物，已由 `.gitignore` 排除，不會直接 commit 到 GitHub。GitHub 保存的是專案 source、資料、腳本與報告；本機 build 產物留在工作區。
