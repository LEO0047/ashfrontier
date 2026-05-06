# ArtSlot 套用報告

- 狀態：通過
- 時間：2026-05-06 23:27:43
- ArtSlot 數：22
- Assignment 數：26
- UE 可載入材質數：22
- 材質缺失數：0

## 套用方式

- `AAshfrontierWorldBlockoutDirector` 於 runtime 讀取 `ArtSlotMapping.json` 與 `EnvironmentArtAssignments.json`。
- block、banner 與 decal actor 依 actor name 查 ArtSlot，再套用對應 material instance。
- 本 UE smoke 確認所有 mapping material instance 可載入。
