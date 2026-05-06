# 生成美術效能影響報告

- 狀態：通過
- macOS：26.4.1
- CPU 架構：arm64
- CPU：Apple M1 Pro
- GPU：Apple M1 Pro
- 解析度：3024 x 1964 Retina
- 模式：Gate 17 art perf impact
- 生成美術檔案數：172
- 最大生成 PNG 尺寸：1024
- texture budget：單張貼圖預設不超過 2048，UI icon 預設 512，不使用大量 4K texture。
- 材質策略：使用 4 個 master material 與 material instance，不建立重型材質 graph。
- gameplay 驗證：搭配 ./Scripts/run_tests.sh --smoke 覆蓋移動、戰鬥、受傷、搬運、招募、交易、建造、生產、存檔與讀檔核心流程。
- 結論：生成美術貼圖預算符合 v0.2 限制：單張貼圖不超過 2048，UI icon 為 512，未使用大量 4K texture。
- 限制：本報告是 Gate 17 smoke perf guard，尚未取代 UE Insights 或 Metal frame capture。
