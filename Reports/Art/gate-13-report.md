# Gate 13 報告：貼圖處理、材質建立與 UE 匯入

## 摘要

Gate 13 將 Gate 12 的生成圖片處理為 UE 可用來源貼圖，並透過 UE Python 匯入為 `UTexture2D`、master material 與 material instance。Manifest 內 43 個資產狀態已推進為 `imported`。

## 完成內容

- 建立 `Scripts/build_texture_maps.py`。
- 建立 `Scripts/import_generated_art.sh`。
- 建立 `Content/Python/import_generated_art.py`。
- 產出 `Content/GeneratedArt/Textures/*_BaseColor.png`。
- 產出 `Content/GeneratedArt/Textures/*_Roughness.png`。
- 產出 `Content/GeneratedArt/Textures/*_Normal.png`。
- 複製 UI、徽章、portrait、decal、banner、concept 來源圖到 `Content/GeneratedArt/` 對應目錄。
- 透過 UnrealEditor 匯入 59 個 texture assets。
- 建立 4 個 master material：
  - `/Game/GeneratedArt/Materials/M_Ashfrontier_GeneratedSurface`
  - `/Game/GeneratedArt/Materials/M_Ashfrontier_GeneratedDecal`
  - `/Game/GeneratedArt/Materials/M_Ashfrontier_GeneratedBanner`
  - `/Game/GeneratedArt/Materials/M_Ashfrontier_UIIcon`
- 建立 40 個 material instances。
- 產出 `Content/Data/Art/GeneratedMaterialInstances.json`。
- 產出 `Reports/Art/texture-processing.md`。
- 產出 `Reports/Art/import-generated-art.md`。

## 驗證

- `python3 Scripts/build_texture_maps.py --check`：通過。
- `./Scripts/import_generated_art.sh --smoke`：通過。
- `Scripts/art_manifest_lint.py`：通過。
- `Scripts/art_prompt_lint.py`：通過。
- `./Scripts/validate.sh`：通過，警告數 0，失敗數 0。

## 風險

- Master material graph 目前是 v0.2 prototype 用的輕量材質網路，後續 Gate 14 套用到場景後需要用固定相機確認實際視覺效果。
- Decal 與 banner 材質已建立，但場景套用會在 Gate 14 透過 ArtSlot mapping 完成。
