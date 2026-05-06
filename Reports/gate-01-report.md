# Gate 01 報告

## Summary

Gate 01 目標是建立 Ashfrontier UE5 專案骨架與核心模組。目前完整 Xcode 與 UE5.7 Editor 已可用，已建立 `.uproject`、C++ module、初始 Config、可載入的 Gate 01 map 與 automation smoke test。

## 狀態

- Gate 狀態：本機驗證中。
- Blocker 類型：UE5 環境 blocker 已解除；目前剩餘風險是磁碟空間偏低。
- Blocker 報告：`Reports/ue5-environment-blocker.md` 保留作為歷史紀錄。

## 歷史 blocker 與解除狀態

- 初始 blocker：曾找不到 UE5 Editor，無法由 UE 產生或驗證 `Ashfrontier.uproject`、C++ module 與 `.umap`。
- 完整 Xcode 已可用，`xcodebuild -version` 回報 `Xcode 26.4.1` / `Build version 17E202`，`xcode-select -p` 為 `/Applications/Xcode.app/Contents/Developer`。
- UE5 Editor blocker 已解除：`/Users/Shared/Epic Games/UE_5.7/Engine/Binaries/Mac/UnrealEditor.app` 已存在且可執行。
- `Content/Maps/L_Ashfrontier_Prototype.umap` 已由 UE Editor / Python 建立，不是文字檔或空檔 placeholder。

## 環境驗證

- Gate 00 的 `./Scripts/validate.sh` 通過。
- Git remote 已確認為 `https://github.com/LEO0047/ashfrontier.git`。
- 環境變數範例已建立於 `Scripts/env.example`。
- 已安裝完整 `/Applications/Xcode.app`，使用者已在 GUI 按下 `Agree`。
- 已安裝 `Unreal Engine 5.7.4`，`UnrealEditor.app` 已可執行。
- `mas get 497799835` 曾因互動式 sudo 密碼需求無法在非互動 terminal 完成，後續已透過 GUI 完成 Xcode。

## 本機進行中內容

- 已建立 `Ashfrontier.uproject`，宣告 `Ashfrontier` runtime module 與 `EnhancedInput` plugin。
- 已建立 `Source/Ashfrontier/` C++ module skeleton，包含 Game / Editor target、GameMode、PlayerController、Character、HUD 與 automation smoke test。
- 已建立 `Config/DefaultEngine.ini`、`Config/DefaultGame.ini`、`Config/DefaultInput.ini` 與 `Config/DefaultEditor.ini`。
- 已建立 `Content/Maps/L_Ashfrontier_Prototype.umap`，MapCheck 0 errors / 0 warnings。
- 已建立 `Content/UI/` 目錄 placeholder。
- `AshfrontierEditor` C++ build 已通過。
- `Ashfrontier.Smoke.ModuleLoads` automation test 已通過。

## 2026-05-06 08:46 CST 安裝狀態

- 完整 Xcode、macOS SDK 與 Xcode `clang` 已可用。
- Epic Games Launcher 仍在處理 `Unreal Engine 5.7.4` 安裝，`/Users/Shared/Epic Games/UE_5.7` 約 `13G`。
- `UE_5.7` 尚未出現 `Engine/` 與 `UnrealEditor.app`。
- 目前磁碟剩餘約 `32GiB`，UE 解壓可能因空間不足變成 blocker；尚未看到明確錯誤。

後續觀察：已找到 `/Users/Shared/Epic Games/UE_5.7/Engine/Binaries/Mac/UnrealEditor.app`，UE5 Editor 安裝完成。安裝後磁碟可用空間降至約 `12-16GiB`。

## Gate 01 驗證結果

- `Build.sh AshfrontierEditor Mac Development`：通過。
- `Scripts/create_gate01_map.sh`：通過；首次使用 `spawn_actor_from_class` 在 `-NullRHI` 下觸發 UE5.7 viewport hit proxy crash，已修正為只建立 / 儲存真 `.umap`，blockout actor 延後到 Gate 04。
- `./Scripts/run_tests.sh --smoke`：通過。
- UE automation：找到並執行 `Ashfrontier.Smoke.ModuleLoads`，結果 `Success`。
- UE map check：`Content/Maps/L_Ashfrontier_Prototype.umap` 為 0 errors / 0 warnings。

## Playable Contract 接收狀態

- Gate 03 必須完成 Editor 內小隊移動 playable milestone。
- Gate 05 必須完成 Editor 內「移動 → 戰鬥 → 受傷 → 倒地 → 搬運 → 醫療」playable milestone。
- Gate 07 必須完成 Editor 內「招募 → 交易 → 建造 → 生產」playable milestone。
- Gate 09 必須嘗試 macOS packaged build；若 build 成功，必須完成 5 分鐘 golden path。
- 文件、報告、測試腳本、UE project 可開啟或 map 可載入，都不得替代 playable prototype。

## 下一步

- 執行完整 `./Scripts/validate.sh`。
- 驗證通過後執行 `./Scripts/commit_gate.sh gate-01 "gate-01: 建立 UE5 專案骨架與核心模組"`。
- Gate 02 開始建立資料驅動初始內容。

## 推送紀錄

- Gate commit：db6478a902040faf04bdc6222421cb35ae754160
- Push 狀態：已推送到 origin/main
- Remote SHA：db6478a902040faf04bdc6222421cb35ae754160
- 記錄時間：2026-05-06 05:55:14 CST

## 推送紀錄

- Gate commit：a937a5a09e915115cf24594c7631d5aa5e8d1ac0
- Push 狀態：已推送到 origin/main
- Remote SHA：a937a5a09e915115cf24594c7631d5aa5e8d1ac0
- 記錄時間：2026-05-06 06:03:47 CST

## 推送紀錄

- Gate commit：f34d2f1e008618c3982b20acba7e46b8d6ba2fe8
- Push 狀態：已推送到 origin/main
- Remote SHA：f34d2f1e008618c3982b20acba7e46b8d6ba2fe8
- 記錄時間：2026-05-06 07:51:52 CST
