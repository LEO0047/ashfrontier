# Ashfrontier 生成美術來源與 IP 防呆規則

## 摘要

所有 v0.2 生成美術都必須是 Ashfrontier 原創資產。研究文件只作系統與風格方向參考，不得把任何外部作品的專有名稱、角色、派系、美術、地點、標誌或文本放入 prompt、metadata、資產名稱或使用者可見內容。

## 禁用詞清單

下列字串不得出現在美術 prompt、圖片 metadata、生成資產描述或使用者可見文案中：

- `Kenshi`
- `Kenshi 2`
- `Lo-Fi Games`
- `Hiver`
- `Shek`
- `Holy Nation`
- `United Cities`
- `Old Empire`
- `Beak Thing`
- `Skin Bandit`
- `Skeleton`

`Skeleton` 若作為技術文件中的一般程式或骨架概念可以存在，但不得出現在美術 prompt 中作為角色種族、造型或派系描述。

## 每張圖片必備 metadata

每張入選生成圖都必須有 `SourceArt/Generated/Metadata/*.json`，欄位如下：

- `asset_id`
- `category`
- `prompt`
- `negative_prompt`
- `generation_tool`
- `timestamp`
- `selected_reason`
- `output_path`
- `ue_import_path`
- `usage_target`

## 入選規則

- 只 commit 最終選中的圖片與 metadata。
- 未選中的候選圖預設放在 `SourceArt/Generated/_rejected/`，不 commit。
- 若需要保留 rejected 圖，必須在報告中說明原因。
- 圖片不得來自網路抓圖、未授權素材、截圖翻畫或外部 IP 近似仿作。
- 生成工具若無法輸出圖檔，必須建立 `Reports/image-generation-blocker.md`。

## Prompt 規則

- Prompt 使用繁體中文描述，可保留 PBR、basecolor、roughness、normal map、icon sheet 等必要技術詞。
- Prompt 可以描述乾燥、風沙、鹽蝕、生鏽金屬、低科技高磨損等抽象方向。
- Prompt 不得要求模仿特定外部遊戲、工作室、角色、派系、地點或商標。
- Signage 文字必須使用繁體中文，且短、粗、遠距離可讀。

## 恢復與追蹤

- Manifest 是真實狀態來源，metadata 是單張圖片來源記錄。
- `validation_status` 必須隨 Gate 推進：`planned`、`generated`、`processed`、`imported`、`assigned` 或 `blocked`。
- 若某資產被替換，保留同一 `asset_id`，更新 metadata 與報告，除非資產用途已改變。
- 若圖片生成或 LFS 阻塞，保留 prompt pack 與 manifest，待工具恢復後從該 Gate 續跑。
