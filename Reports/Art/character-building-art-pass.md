# 角色、裝備與建築視覺差異化報告

## 摘要

Gate 16 將 Gate 12-15 的生成美術從 UI 與場景 signage 延伸到角色 body material、可放置建築 material variant 與裝備 icon mapping。這仍是 v0.2 prototype 套皮，不等待最終 3D 模型，但角色、派系、建築與裝備在遠距離與 UI 中已有更清楚的辨識點。

## 角色與派系

- 玩家小隊：鹽蝕砂岩材質，保留灰白鹽風基調。
- 鹽脊守望：鏽金屬材質與垂直徽章，偏軍事與秩序。
- 玻璃屋商盟：粗糙工業地板材質與商貿徽章，偏工業與交易。
- 塵路行者：日曬布料與骨質陶片材質，偏荒野、游牧與拾荒。
- 4 名可招募角色皆有不同 portrait，且 runtime 角色保留 art id、portrait path 與 body material path。

## 建築

- `building_camp_kitchen`：焦黑木材。
- `building_storage_crate`：日曬舊布。
- `building_scrap_mine`：生鏽金屬。
- `building_field_workshop`：粗糙工業地板。
- `building_palisade_wall`：骨質陶片。

## 裝備與物品

- `item_rustblade`：武器 icon mapping。
- `item_quilted_vest`：護具 icon mapping。
- `item_field_bandage` 與 `item_medic_wrap`：醫療用品 icon mapping。
- `item_building_plate`：建造材料 icon mapping。

## 驗證

- `python3 Scripts/visual_variant_lint.py`：通過。
- 派系角色 variant 覆蓋數：3。
- 可招募角色 variant 覆蓋數：4。
- 建築 variant 覆蓋數：5。
- 裝備 mapping 數：5。

## 風險

- 角色仍使用 cube placeholder body，材質差異化只作為 v0.2 可玩原型辨識手段。
- 建築 signage 的實際可讀距離需 Gate 17 固定截圖確認。
