# v0.2 最終生成美術資產清冊

- 狀態：通過
- Manifest：Content/Data/Art/ArtGenManifest.json
- Material instance map：Content/Data/Art/GeneratedMaterialInstances.json
- SourceArt 最終選中圖片：SourceArt/Generated/
- UE 匯入資產：Content/GeneratedArt/

## Manifest 統計

- 總資產數：43
- 概念圖：3
- 表面材質：8
- UI icon：12
- 派系徽章：3
- 派系旗幟 / banner：3
- 角色 portrait：4
- Decal / signage：10

## UE 材質與貼圖

- Material instance 總數：50
- Surface material instance：8
- UI / emblem / portrait material instance：19
- Banner material instance：3
- Decal material instance：10
- Signage plane material instance：10
- 最大生成 PNG 尺寸：1024
- Texture budget：符合單張不超過 2048、UI icon 512 的 v0.2 限制。

## 場景與 UI 套用

- ArtSlotMapping.json 已覆蓋主城、前哨與野外建造區。
- EnvironmentArtAssignments.json 維持 10 個 decals/signage 與 3 個派系旗幟。
- UIArtAssignments.json 維持 12 種物品 icon。
- FactionArtAssignments.json 維持 3 個派系 emblem。
- CharacterPortraitAssignments.json 維持 4 名可招募角色 portrait。

## Provenance

- 每個 manifest asset 皆有 SourceArt/Generated/Metadata/*.json provenance。
- Prompt pack 與 banned terms lint 維持通過。
- 未使用網路抓圖或未授權素材。
