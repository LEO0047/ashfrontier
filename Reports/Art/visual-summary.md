# Gate 17 視覺回歸摘要

- 狀態：通過
- 截圖來源：Scripts/capture_art_screenshots.sh
- 截圖目錄：Reports/Art/Screenshots
- 截圖數：8
- smoke 檢查：Reports/Art/screenshot-smoke.md

## 截圖重點

- `01_main_city_entrance.png`：主城入口以鹽蝕牆面、深色金屬門面、派系旗幟面片與生成地表材質建立入口辨識；HUD 已隱藏，鏡頭聚焦城門與可讀色塊。
- `02_main_city_market.png`：商店區保留可走動 blockout，同時顯示工業地板、玻璃屋商盟旗幟與店區標記，避免只剩灰盒平台。
- `03_guard_patrol.png`：守衛巡邏區使用鏽蝕金屬與警示標記對巡邏動線作視覺區隔，對應城市反應與守衛測試情境。
- `04_outpost_exterior.png`：前哨外觀顯示焦黑木材、風化牆體與塵路行者場景旗幟，與主城的秩序感形成差異。
- `05_wilderness_build_zone.png`：野外建造區使用乾裂地表、骨陶與工坊 / 危險標記，支援建造、生產與採集流程的視覺辨識。
- `06_recruit_ui.png`：招募 UI 顯示 4 名可招募角色頭像、派系徽章與物品圖示，沒有 missing texture。
- `07_trade_ui.png`：交易 UI 顯示商人徽章、交易物品 icon 與庫存數量，低噪音圖示在小尺寸仍可辨識。
- `08_inventory_ui.png`：庫存 UI 顯示 12 種物品 icon、角色列與資源欄位，確認生成 UI 美術已進入可玩流程。

## Gate 17 視覺結論

- 主城、前哨與野外建造區仍是 prototype 幾何，但已透過 ArtSlot、生成材質、派系旗幟、場景告示與 UI 美術套用出一致的低科技高磨損荒漠邊境語言。
- 固定相機截圖未偵測到全黑、解析度過低或大面積純灰畫面。
- macOS / Metal 方向已關閉不適合本 prototype 的 Lumen 預設，改用較穩定的非 Lumen 白晝照明與天空，以維持可玩性與截圖可讀性。
