# Gate 17 報告：視覺回歸、效能與 macOS 可玩性驗證

- Gate：17
- 狀態：待 commit gate 最終推送紀錄
- 目標：完成 v0.2 生成美術套皮後的固定相機回歸、art coverage、screenshot smoke、效能預算與 gameplay automation 驗證。
- 工作日期：2026-05-07

## 已完成內容

- 新增固定相機擷取流程：Scripts/capture_art_screenshots.sh 與 Scripts/screenshot_smoke_lint.py。
- 在 AAshfrontierGameMode 加入 Gate 17 screenshot mode，可用 -AshfrontierCaptureArtScreenshots 自動擷取 8 張固定鏡頭。
- 固定相機輸出 Reports/Art/Screenshots/，並產生 Reports/Art/capture-art-screenshots.md、Reports/Art/screenshot-smoke.md 與 Reports/Art/visual-summary.md。
- 將 macOS / Metal prototype renderer 改為非 Lumen，關閉 motion blur，加入白晝 sky atmosphere 與 dust haze，降低截圖雜訊與 Metal 警告。
- 增加 ArtSlot 驅動的 banner / signage plane，並補出 mesh 可用的 MI_AfV02_Signage_* material instance，讓場景旗幟與告示可在驗收鏡頭中辨識。
- 更新 Scripts/perf_capture.sh --gate17，產生 Reports/Art/art-perf-impact.md 與 JSON。
- 更新 Scripts/validate.sh，納入 screenshot smoke 與 Gate 17 art perf guard。

## 驗證結果

- 固定相機截圖：通過，8 張 1280x720 PNG 已輸出。
- Screenshot smoke：通過，問題數 0。
- Art coverage lint：通過，主城、前哨、野外建造區覆蓋率與 decals/signage / 旗幟要求維持通過。
- UI art lint：通過，12 種物品 icon、3 個派系徽章與 4 名角色 portrait 維持完整。
- Visual variant lint：通過，角色、建築與裝備視覺差異化資料維持完整。
- Art perf guard：通過，最大生成 PNG 尺寸 1024，未超過 2048 texture budget。
- UE automation：通過，涵蓋移動、戰鬥、受傷、搬運、招募、交易、建造、生產、存檔與讀檔相關流程。

## 環境處理紀錄

- 截圖初次卡住的主因是本機 /tmp 保留多個舊 Ashfrontier 打包暫存工作區，導致 UE5 低磁碟空間視窗阻塞。已清除由 Scripts/package_macos.sh 產生的舊 /tmp/ashfrontier-package-* 暫存目錄，保留目前 repo 與 Builds/macOS/Ashfrontier.app。
- 本 Gate 不建立 v0.2 tag，tag 會留到 Gate 18 完成打包與最終報告後建立。

## 待 commit gate 補寫

- Gate commit：待 Scripts/commit_gate.sh 補寫。
- Remote SHA：待 Scripts/commit_gate.sh 補寫。
