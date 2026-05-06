# Gate 14 報告：ArtSlot 世界場景套用

## 摘要

Gate 14 建立資料驅動 ArtSlot mapping，並把主城、前哨與野外建造區的主要 placeholder block、旗幟與場景 signage / decal 接到生成材質。世界 blockout director 會在 runtime 讀取 `ArtSlotMapping.json` 與 `EnvironmentArtAssignments.json`，依 actor name 套用 material instance。

## 完成內容

- 建立 `Content/Data/Art/ArtSlotMapping.json`。
- 建立 `Content/Data/Art/EnvironmentArtAssignments.json`。
- 建立 `Scripts/apply_art_slots.sh`。
- 建立 `Content/Python/apply_art_slots.py`。
- 建立 `Scripts/art_coverage_lint.py`。
- 更新 `AAshfrontierWorldBlockoutDirector`：
  - 讀取 ArtSlot mapping 與 assignment JSON。
  - 將 block placeholder 套用 surface / banner material instance。
  - 生成 3 個派系場景旗幟。
  - 生成 10 個場景 decal / signage actor。
  - 保留 debug material fallback，避免缺失時直接崩潰。

## 覆蓋目標

- 主城：主要地面、城牆、城門、商店區、守衛區、旗幟與 5 個 signage / decal。
- 前哨：地面、塔樓、倉庫、旗幟與 2 個 signage / decal。
- 野外建造區：地面、工坊、採集點與 3 個 signage / decal。
- 三個派系皆有場景可見旗幟。

## 驗證

- `python3 Scripts/art_coverage_lint.py`：通過，主城覆蓋 1.05、前哨覆蓋 1.00、野外建造區覆蓋 1.00，10 個 signage / decal 與 3 個派系旗幟皆已登錄。
- `./Scripts/apply_art_slots.sh`：通過，UE Python smoke 成功載入 22 個 ArtSlot material 與 26 筆場景 assignment。
- `./Scripts/validate.sh`：通過，警告數 0、失敗數 0。

## 風險

- 本 Gate 使用 cube block、banner block 與 decal actor 完成 v0.2 prototype 套皮，不代表最終關卡美術。
- Decal 實際視覺需 Gate 17 固定相機截圖再確認可讀距離與投射角度。
