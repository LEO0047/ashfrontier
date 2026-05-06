# Gate 04 報告

## 狀態

- Gate：Gate 04 — 世界 blockout、導航、巡邏與 macOS 基礎效能。
- 狀態：本機 `./Scripts/validate.sh` 通過，等待 commit gate 執行最終 validate / commit / push。
- 日期：2026-05-06。

## 完成內容

- 新增 `AAshfrontierWorldBlockoutDirector` runtime world builder：
  - `zone_salt_spine_city`：鹽脊主城。
  - `zone_cinder_outpost`：燼沙前哨。
  - `zone_wilderness_yard`：裂土建造區。
- 新增 runtime blockout geometry：
  - 世界地面。
  - 主城地面、城牆與城門。
  - 前哨塔。
  - 野外工坊 / 建造區 placeholder。
  - route markers。
- 新增 `AAshfrontierRouteAgent`：
  - 使用 waypoint route 循環移動。
  - 城市守衛巡邏 route agent。
  - 玻璃屋商隊 route agent。
- 新增兩條路線：
  - `route_city_guard_patrol`：4 個 waypoint。
  - `route_glasshouse_caravan`：4 個 waypoint。
- 新增 UE automation test `Ashfrontier.World.BlockoutRoutes`。
- 更新 `Scripts/perf_capture.sh`，輸出 Gate 04 主城壓力區與 macOS / Apple Silicon 環境摘要。
- 更新 `README.md`、`Docs/PLAN.md`、`Docs/TECH_SPEC.md` 與 `Docs/TEST_PLAN.md`。

## 測試結果

- `Build.sh AshfrontierEditor Mac Development -Project=Ashfrontier.uproject`：通過。
- `./Scripts/run_tests.sh --smoke`：通過。
- `./Scripts/perf_capture.sh --gate04`：通過，輸出 `Reports/perf-summary.md`。
- `./Scripts/validate.sh`：通過，0 個失敗。
- UE automation：
  - `Ashfrontier.Data.ContentJsonLoads`：Success。
  - `Ashfrontier.Smoke.ModuleLoads`：Success。
  - `Ashfrontier.Squad.OrderStateFlow`：Success。
  - `Ashfrontier.World.BlockoutRoutes`：Success。

## Playable Contract 狀態

- Gate 04 擴大 Gate 03 的可移動環境，讓小隊與 NPC route agent 有主城、前哨與野外建造區的 runtime blockout 可用。
- Gate 04 不宣稱完整 playable prototype；戰鬥、部位傷害、搬運、醫療、招募、交易、建造、生產、城市法規反應與存讀檔仍在後續 Gate。

## macOS build / test 狀態

- macOS packaged build：本 Gate 未執行，屬於 Gate 09。
- Editor automation：已於 macOS / UE5.7.4 / Apple Silicon workflow 通過。
- Perf capture 初版：已記錄 macOS 26.4.1、arm64、Apple M1 Pro、Retina 解析度與主城壓力區設定。

## 已知問題

- Gate 04 目前以 runtime actors 生成 blockout，尚未把 geometry 寫回 `.umap`。
- NavMesh 尚未烘焙；目前 route agent 使用直接 movement input 沿 waypoint 移動。這是 Gate 04 的可接受替代驗證，但不是最終 NavMesh 驗證。
- 尚未執行 15 分鐘巡邏 soak；目前以 `Ashfrontier.World.BlockoutRoutes` smoke test 驗證 zone、route 與 route agent。長時間 soak 會在 Gate 09 或正式 NavMesh 前補強。
- `Reports/perf-summary.md` 尚未包含 UE Insights trace 或 Metal frame capture，不得視為最終效能結論。

## 下一個 Gate 重點

- Gate 05 必須完成「移動 → 戰鬥 → 受傷 → 倒地 → 搬運 → 醫療」Editor playable flow。
- Gate 05 需要在現有 route / blockout 環境中放入至少 1 名敵人與部位傷害測試流程。

## 推送紀錄

- Gate commit：532981c723b37d7e90b31ad6c161aa9cf6ea1a7b
- Push 狀態：已推送到 origin/main
- Remote SHA：532981c723b37d7e90b31ad6c161aa9cf6ea1a7b
- 記錄時間：2026-05-06 19:38:04 CST
