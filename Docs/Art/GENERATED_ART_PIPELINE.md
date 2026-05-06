# Ashfrontier v0.2 生成美術管線

## 摘要

本管線把內建圖片生成、來源記錄、貼圖處理、UE 匯入、ArtSlot 套用與驗證串成同一條路徑。Gate 10 先建立資料結構與 lint；後續 Gate 會逐步生成圖片、處理 maps、匯入 UE、套用到世界與 UI，最後完成 macOS playable build。

## 目錄規則

| 目錄 | 用途 |
|---|---|
| `SourceArt/Generated/Concepts/` | 概念圖與 paintover 成果 |
| `SourceArt/Generated/Textures/` | 最終選中的表面貼圖來源圖 |
| `SourceArt/Generated/UIIcons/` | 物品與 UI icon 來源圖 |
| `SourceArt/Generated/FactionEmblems/` | 派系徽章來源圖 |
| `SourceArt/Generated/Portraits/` | 角色頭像來源圖 |
| `SourceArt/Generated/Decals/` | 場景 decal 與 signage 來源圖 |
| `SourceArt/Generated/Banners/` | 旗幟與 banner 來源圖 |
| `SourceArt/Generated/Metadata/` | 每張生成圖的 provenance JSON |
| `Content/GeneratedArt/` | UE 匯入後的可用資產 |
| `Content/Data/Art/` | Manifest 與 ArtSlot 對應資料 |
| `Docs/Art/Prompts/` | Prompt pack 與風格說明 |
| `Reports/Art/` | 每個美術 Gate 的驗證與摘要 |

## Gate 10 狀態

- 已建立 `.gitattributes` 追蹤 UE 與圖片資產型別。
- 本機尚未偵測到 `git-lfs` 指令，已建立 `Reports/git-lfs-blocker.md`。
- `Content/Data/Art/ArtGenManifest.json` 先登錄 v0.2 預計生成資產，狀態維持 `planned`。
- `Scripts/art_prompt_lint.py` 檢查 prompt、manifest prompt 與 metadata prompt 的禁用詞與簡體字。
- `Scripts/art_manifest_lint.py` 檢查 manifest schema、asset id、路徑、狀態與 metadata 對應。

## 後續管線

1. 以 `Docs/Art/Prompts/` 的 prompt pack 產出原創圖片。
2. 每張入選圖片寫入 `SourceArt/Generated/Metadata/*.json`。
3. `Scripts/build_texture_maps.py` 產出 basecolor、roughness approximation、normal approximation 與必要 mask。
4. `Scripts/import_generated_art.sh` 呼叫 UE Python 匯入貼圖並建立材質。
5. `Content/Data/Art/ArtSlotMapping.json` 把材質、旗幟、徽章、告示與 UI icon 對應到場景或資料。
6. `Scripts/art_coverage_lint.py` 檢查灰盒覆蓋率、缺失 icon、缺失 portrait 與缺失 UE asset。
7. `Scripts/capture_art_screenshots.sh` 擷取固定相機畫面並產出視覺摘要。

## 防呆原則

- 沒有圖片檔就不是已生成資產。
- 沒有 UE 匯入與引用就不是已套用美術。
- 沒有 metadata 就不得進入最終資產清單。
- 未選中的候選圖預設不 commit。
- 如果工具或環境阻塞，寫 blocker report，保留 prompt manifest 與恢復步驟。
