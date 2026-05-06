# Ashfrontier

Ashfrontier 是一個原創世界觀的 UE5 3D 單機小隊沙盒 RPG playable prototype。專案以「Kenshi 2 公開資訊與設計方向」作為系統設計標竿，但所有派系、角色、地點、文本與資產都必須使用 Ashfrontier 原創內容或 placeholder / 自有資產，不得複製 Kenshi / Kenshi 2 的名稱、角色、派系、美術、地圖、文本或商標。

目前階段是 Gate 03：Gate 02 已完成並推送，`Unreal Engine 5.7.4` 位於 `/Users/Shared/Epic Games/UE_5.7`，Xcode 已完成 Apple SDK agreement。Gate 03 目標是在 UE Editor 內可控制至少 2 名小隊成員，支援選取、移動、跟隨、待命、第三人稱 / 戰術鏡頭與角色狀態 HUD placeholder。此階段仍不是完整 playable prototype；戰鬥、交易、建造、生產與存讀檔會在後續 Gate 逐步完成。

## macOS 開啟方式

1. 確認工作目錄：

```bash
cd "/Users/leo/Library/Mobile Documents/com~apple~CloudDocs/Coding/Projects/Ashfrontier"
```

2. UE5 安裝完成後，可用 UE5 Editor 開啟 `Ashfrontier.uproject`。UE5 路徑不要硬寫死，請先複製並調整環境範例：

```bash
cp Scripts/env.example .env.local
```

3. 在 `.env.local` 設定 `UE5_EDITOR`、`UE5_ROOT` 與 `PROJECT_ROOT`。Apple Silicon Mac 優先使用 native UE5 workflow；若只能使用 Intel 或 Universal workflow，必須記錄在 `Reports/environment.md`。

## 驗證方式

主要入口是：

```bash
./Scripts/validate.sh
```

Gate 01 起，`Ashfrontier.uproject` 已存在，因此 `Scripts/run_tests.sh` 會要求 `UE5_EDITOR` 可執行，並要求 `Content/Maps/L_Ashfrontier_Prototype.umap` 是由 UE Editor 建立的真實 map asset。Gate 02 起，`Scripts/content_lint.py` 會把缺少 `Content/Data/`、資料筆數不足、無效引用或 recipe 循環視為失敗；UE automation test 也會載入 JSON 資料。

可單獨執行：

```bash
python3 Scripts/content_lint.py
./Scripts/create_gate01_map.sh
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
