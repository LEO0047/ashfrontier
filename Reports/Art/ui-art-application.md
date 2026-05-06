# UI 美術套用報告

## 摘要

Gate 15 將已匯入的生成美術接到 prototype UI 資料層與 runtime HUD。現階段 Ashfrontier 仍使用 `AAshfrontierHUD` 的 Canvas HUD，而不是正式 UMG；本 Gate 因此以資料驅動 assignment JSON 與 Canvas texture drawing 完成可玩原型的 UI 美術套用。

## 套用內容

- `Content/Data/Art/UIArtAssignments.json`：12 種物品皆已對應生成 icon，目標覆蓋 inventory、trade、building、production、medical 與 equipment UI。
- `Content/Data/Art/FactionArtAssignments.json`：3 個派系皆已對應 emblem 與 banner，目標覆蓋 map marker、character panel、trade UI、recruit UI 與 warning notification。
- `Content/Data/Art/CharacterPortraitAssignments.json`：4 名可招募角色皆已對應 portrait，並寫入 runtime character art id 與 portrait path。
- `AAshfrontierHUD` 會載入 assignment JSON，繪製：
  - 物品 icon strip 與數量。
  - 小隊列中的派系徽章。
  - 招募 UI portrait 面板。
  - 交易 UI 商人派系與商品 icon 面板。

## 驗證

- `python3 Scripts/ui_art_lint.py`：通過。
- missing icon count：0。
- missing faction emblem count：0。
- missing portrait count：0。

## 風險

- 本 Gate 仍是 Canvas HUD prototype，不是最終 UMG 互動介面。
- 小尺寸 icon 的最終視覺讀性需要 Gate 17 固定截圖與 screenshot smoke 再確認。
