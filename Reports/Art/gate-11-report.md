# Gate 11 報告：原創風格聖經、生成提示與 IP 防呆

## 摘要

Gate 11 補齊 Ashfrontier v0.2 生成美術的完整風格聖經、prompt pack、來源規則與禁用詞規則。所有 prompt 使用繁體中文描述，必要技術詞保留英文，且不要求模仿外部作品或使用未授權素材。

## 完成內容

- 建立 `Docs/Art/STYLE_BIBLE.md`。
- 建立 `Docs/Art/PROVENANCE_RULES.md`。
- 建立 `Docs/Art/BANNED_TERMS.md`。
- 建立 `Docs/Art/Prompts/texture_prompts.md`。
- 建立 `Docs/Art/Prompts/ui_icon_prompts.md`。
- 建立 `Docs/Art/Prompts/faction_emblem_prompts.md`。
- 建立 `Docs/Art/Prompts/portrait_prompts.md`。
- 建立 `Docs/Art/Prompts/decal_and_signage_prompts.md`。
- 更新 `Docs/Art/Prompts/ashfrontier_style_bible.md` 以覆蓋概念圖 asset id。
- 更新 `Scripts/art_prompt_lint.py`，要求必備 prompt files 存在，並檢查 manifest 內每個 `asset_id` 都出現在 prompt pack。

## 驗證

- `Scripts/art_prompt_lint.py`：通過，43 個 manifest `asset_id` 全部有 prompt pack 覆蓋。
- `Scripts/art_manifest_lint.py`：通過。
- `./Scripts/validate.sh`：通過。
- 驗證警告：`git-lfs` 不可用，延續 Gate 10 blocker。

## 風險

- Prompt pack 已覆蓋 manifest，但 Gate 12 實際圖片生成後仍需人工挑選最終入選圖，並檢查文字 signage 是否可讀。
- 本機仍缺少 `git-lfs`，圖片提交前需要解除 blocker。

## 推送紀錄

- Gate commit：d70bad89d924399084abbe692ea1877a4e294c72
- Push 狀態：已推送到 origin/main
- Remote SHA：d70bad89d924399084abbe692ea1877a4e294c72
- 記錄時間：2026-05-06 22:45:05 CST
