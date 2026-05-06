# Gate 12 報告：第一批原創生成美術資產

## 摘要

Gate 12 使用內建圖片生成功能建立 7 張原創 source sheet，並以本機腳本裁切、縮放與必要文字覆蓋，產出 43 個最終入選資產。所有 final asset 都已寫入 `SourceArt/Generated/`，每個 asset 都有 metadata，並更新 `Content/Data/Art/ArtGenManifest.json` 的 `validation_status` 為 `generated`。

## 生成來源

- 概念圖 sheet：3 張概念圖。
- 表面貼圖 sheet：8 張 surface texture。
- UI icon sheet：12 個物品 icon。
- 派系徽章 sheet：3 個 emblem。
- 派系旗幟 sheet：3 張 banner。
- 角色頭像 sheet：4 張 portrait。
- Decal / signage sheet：10 張告示與場景 decal，後處理覆蓋繁體中文短字。

## 完成內容

- 建立 `Scripts/generate_art_assets.sh`。
- 建立 `Scripts/slice_generated_art_sheets.py`。
- 生成並保存 `SourceArt/Generated/Textures/*`。
- 生成並保存 `SourceArt/Generated/UIIcons/*`。
- 生成並保存 `SourceArt/Generated/FactionEmblems/*`。
- 生成並保存 `SourceArt/Generated/Portraits/*`。
- 生成並保存 `SourceArt/Generated/Decals/*`。
- 生成並保存 `SourceArt/Generated/Banners/*`。
- 生成並保存 `SourceArt/Generated/Concepts/*`。
- 產出 `SourceArt/Generated/Metadata/*.json`。
- 產出 `Reports/Art/generated-assets-inventory.md`。
- 更新 `Reports/git-lfs-blocker.md`，記錄 `git-lfs` 已安裝並在 repo local 初始化。

## 驗證

- `Scripts/generate_art_assets.sh`：通過，產出 43 個 final assets。
- `Scripts/art_prompt_lint.py`：通過，prompt、manifest prompt 與 metadata prompt 無禁用詞。
- `Scripts/art_manifest_lint.py`：通過，43 個 generated assets 均有 metadata、有效路徑、power-of-two 尺寸與支援的色彩模式。
- `git check-attr filter -- SourceArt/Generated/Textures/af_v02_surface_cracked_sand.png`：確認 PNG 會走 `filter: lfs`。
- `./Scripts/validate.sh`：通過，警告數 0，失敗數 0。

## 風險

- Signage 文字由後處理覆蓋，視覺上可讀，但 Gate 14 實際套用場景後仍需截圖確認遠距離可讀性。
- 表面貼圖由 source sheet 裁切後放大到規格尺寸，屬 v0.2 prototype 品質，不代表最終量產材質。
