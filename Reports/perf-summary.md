# 效能摘要

- 狀態：通過（--gate17）
- macOS：26.4.1
- CPU 架構：arm64
- CPU：Apple M1 Pro
- GPU：Apple M1 Pro
- 解析度：3024 x 1964 Retina
- 模式：--gate17
- 測試場景：/Game/Maps/L_Ashfrontier_Prototype runtime Gate 04 world blockout。
- 主城壓力區：鹽脊主城 blockout，含城牆、城門、巡邏 route marker 與城市巡邏 agent。
- gameplay 覆蓋：小隊、戰鬥、招募、交易、建造、生產、城市反應與存讀檔 automation suite。
- 路線設定：城市守衛巡邏 4 個 waypoint；玻璃屋商隊路線 4 個 waypoint。
- 測試方式：記錄 macOS / Apple Silicon / 顯示環境與主城壓力場景設定，並搭配 Gate 09 automation / package smoke 結果判讀。
- 已知限制：此報告尚未使用 Metal frame capture 或 UE Insights trace，不得視為最終 frame time 結論；下一階段應加入固定解析度、stat unitgraph 與 UE Insights trace。
