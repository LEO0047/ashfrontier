# Gate 16 報告：角色、裝備與建築視覺差異化

## 摘要

Gate 16 在不導入最終 3D 模型的前提下，使用生成材質、派系標記、裝備 icon mapping 與建築 variant，讓角色、派系與建築在可玩 prototype 中具備更清楚的視覺辨識。

## 完成內容

- 建立 `Content/Data/Art/CharacterVisualVariants.json`。
- 建立 `Content/Data/Art/BuildingVisualVariants.json`。
- 建立 `Content/Data/Art/EquipmentVisualAssignments.json`。
- 建立 `Scripts/visual_variant_lint.py`。
- 更新 `AAshfrontierCharacter`：
  - 支援 art character id。
  - 支援 portrait texture path。
  - 支援 prototype body material path 並套用到 placeholder body。
- 更新 `AAshfrontierGameMode`：
  - 4 名可招募角色使用不同 body material 與 portrait。
  - 商人、守衛與敵對角色使用不同派系材質。
- 更新 `UAshfrontierSquadManagerComponent`：
  - 玩家小隊套用鹽蝕砂岩材質，避免仍是灰盒 cube。
- 更新 `AAshfrontierPlacedBuilding`：
  - 初始化建築時讀取 `BuildingVisualVariants.json`，套用對應 generated material。
- 更新 save/load：
  - 保留角色 body material path。
- 更新 UE automation save test：
  - 確認角色 art id、portrait path 與 body material path 可保存與還原。
- 更新 `validate.sh`，加入 Visual Variant Lint。

## 覆蓋結果

- 3 個派系皆有角色材質與徽章辨識。
- 4 名可招募角色皆有不同 portrait 與角色 variant。
- 5 種建築皆有不同材質或標誌規則。
- 交易商、守衛、招募 NPC 具備不同 UI 或場景標記。

## 驗證

- `python3 Scripts/visual_variant_lint.py`：通過。
- `python3 Scripts/art_manifest_lint.py`：通過。
- `./Scripts/validate.sh`：通過，警告數 0、失敗數 0。

## 風險

- 本 Gate 沒有導入最終 character mesh；角色差異主要由材質、portrait、派系徽章與 UI panel 支撐。
- 建築材質 variant 已套到可放置建築 placeholder，最終美術仍需後續 mesh pass。
