# Soak Test 報告

- 狀態：通過（smoke soak）
- 模式：--smoke
- 目標秒數：60
- 開始時間：2026-05-06 21:13:16 CST
- 結束時間：2026-05-06 21:13:37 CST
- 執行內容：重跑 UE automation smoke suite，覆蓋小隊、世界、戰鬥、招募、交易、建造、生產、城市反應與存讀檔 round-trip。
- 測試摘要：見 Reports/test-summary.md。
- 已知限制：本次為 Gate 09 smoke soak，不是完整 30 分鐘互動壓力 soak。若要執行完整 30 分鐘，設定 ASHFRONTIER_SOAK_SECONDS=1800 後重跑，並保留本報告。
