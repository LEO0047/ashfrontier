# Ashfrontier

Ashfrontier 是一個原創世界觀的 UE5 3D 單機小隊沙盒 RPG playable prototype。專案以「Kenshi 2 公開資訊與設計方向」作為系統設計標竿，但所有派系、角色、地點、文本與資產都必須使用 Ashfrontier 原創內容或 placeholder / 自有資產，不得複製 Kenshi / Kenshi 2 的名稱、角色、派系、美術、地圖、文本或商標。

目前階段是 Gate 00：macOS 工作區、中文文件、研究文件副本與驗證骨架。尚未建立 UE5 `.uproject`，因此 UE Editor、automation tests 與 macOS package 會先以環境檢查與 blocker 報告方式處理。

## macOS 開啟方式

1. 確認工作目錄：

```bash
cd "/Users/leo/Library/Mobile Documents/com~apple~CloudDocs/Coding/Projects/Ashfrontier"
```

2. Gate 01 建立 `Ashfrontier.uproject` 後，可用 UE5 Editor 開啟。UE5 路徑不要硬寫死，請先複製並調整環境範例：

```bash
cp Scripts/env.example .env.local
```

3. 在 `.env.local` 設定 `UE5_EDITOR`、`UE5_ROOT` 與 `PROJECT_ROOT`。Apple Silicon Mac 優先使用 native UE5 workflow；若只能使用 Intel 或 Universal workflow，必須記錄在 `Reports/environment.md`。

## 驗證方式

主要入口是：

```bash
./Scripts/validate.sh
```

Gate 00 的驗證會檢查 Git remote、必要文件、繁體中文文件、macOS 環境紀錄、腳本語法與初始 content lint。若尚未建立 UE5 專案，`Scripts/run_tests.sh` 會輸出中文測試摘要並標示 UE 測試尚未啟用。

可單獨執行：

```bash
python3 Scripts/content_lint.py
./Scripts/run_tests.sh --smoke
./Scripts/soak_test.sh --smoke
./Scripts/perf_capture.sh
./Scripts/package_macos.sh
```

## 建置與打包

Gate 09 的目標輸出是：

```text
Builds/macOS/Ashfrontier.app
```

若本機 UE5、Xcode、macOS SDK、簽章或權限限制導致無法完成 macOS `.app`，不得偽造產物；必須輸出 `Reports/mac-build-blocker.md`，清楚記錄阻塞條件、替代驗證與下一步。

## 文件與報告

- `Docs/PLAN.md`：單一主線 Gate 計畫。
- `Docs/TECH_SPEC.md`：UE5、macOS / Metal、核心系統與資料驅動架構。
- `Docs/CONTENT_SCHEMA.md`：factions、items、resources、recipes、buildings、npc schedules、dialogue conditions、legal rules 的資料格式。
- `Docs/TEST_PLAN.md`：unit、functional、PIE、content lint、soak、performance、macOS package smoke test。
- `Docs/Research/Kenshi2_DeepResearch_zh-TW.md`：本機研究文件副本，只作系統設計與技術方向依據。
- `Reports/`：每個 Gate 的中文狀態、測試、commit、push、macOS build 與已知問題報告。

## Gate 提交規則

每個 Gate 必須先實作，再更新文件，接著執行：

```bash
./Scripts/validate.sh
```

驗證通過後才可執行：

```bash
./Scripts/commit_gate.sh gate-00 "gate-00: 建立 macOS 工作區、中文文件與自動驗證骨架"
```

若驗證失敗，`commit_gate.sh` 會停止 commit / push，並輸出 `Reports/gate-failure.md`。
