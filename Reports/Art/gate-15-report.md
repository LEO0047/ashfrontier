# Gate 15 報告：UI 圖示、派系徽章與角色頭像套用

## 摘要

Gate 15 將第一批生成 UI 美術接到資料層與 runtime prototype HUD。物品、派系與可招募角色不再只有文字資料；HUD 會以生成 icon、emblem 與 portrait 顯示庫存、交易與招募資訊。

## 完成內容

- 建立 `Content/Data/Art/UIArtAssignments.json`。
- 建立 `Content/Data/Art/FactionArtAssignments.json`。
- 建立 `Content/Data/Art/CharacterPortraitAssignments.json`。
- 建立 `Scripts/ui_art_lint.py`。
- 更新 `AAshfrontierCharacter`，加入 art character id 與 portrait texture path。
- 更新 `AAshfrontierGameMode`，替 4 名可招募角色、商人與守衛設定 portrait path 與派系差異。
- 更新 `AAshfrontierHUD`，從 assignment JSON 載入並繪製：
  - 12 種物品 icon。
  - 3 個派系 emblem。
  - 4 名可招募角色 portrait。
  - 商人交易 icon 面板。
- 更新 save/load，保留角色 art id 與 portrait path。
- 更新 UE automation data test，檢查 UI art assignment 數量與 UE texture asset 存在。
- 更新 `validate.sh`，加入 UI Art Lint。

## 覆蓋結果

- 12 種物品全部有 icon。
- 3 個派系全部有 emblem。
- 4 名可招募角色全部有 portrait。
- `missing icon count = 0`。
- `missing faction emblem count = 0`。
- `missing portrait count = 0`。

## 驗證

- `python3 Scripts/ui_art_lint.py`：通過。
- `python3 Scripts/art_manifest_lint.py`：通過。
- `git diff --check`：通過。
- `./Scripts/validate.sh`：通過，警告數 0、失敗數 0。

## 風險

- 本 Gate 的 UI 仍是 prototype Canvas HUD；正式 UMG 視覺層可在後續 UI pass 再拆分。
- 小尺寸 icon 最終可讀性需在 Gate 17 透過固定相機截圖與 screenshot smoke 確認。
