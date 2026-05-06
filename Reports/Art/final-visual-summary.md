# v0.2 最終視覺摘要

- 狀態：通過
- 主要截圖報告：Reports/Art/visual-summary.md
- 固定相機截圖：Reports/Art/Screenshots/
- Screenshot smoke：Reports/Art/screenshot-smoke.md

## 最終視覺狀態

Ashfrontier v0.2 仍保留 playable prototype 的低幾何成本，但已不再是純灰盒展示。世界場景透過生成地表材質、城牆 / 金屬 / 木材 / 骨陶材質變體、派系旗幟、場景告示、decals、UI icon、派系徽章與角色頭像建立一致的荒漠邊境美術語言。

## 區域摘要

- 主城：鹽蝕牆面、鏽蝕金屬、工業地板與秩序派系旗幟已套用，入口與商店區有固定鏡頭驗收。
- 前哨：焦黑木材、風化牆體、荒野派系旗幟與倉庫標記已套用，與主城視覺語言不同。
- 野外建造區：乾裂地表、骨陶材質、工坊標記與危險標記已套用，支援建造與生產流程辨識。
- UI：12 種物品 icon、3 個派系 emblem、4 張角色 portrait 已在招募、交易與庫存 UI 中顯示。

## 可讀性與效能

- 8 張固定相機截圖皆為 1280x720，未偵測到全黑、解析度過低或大面積純灰畫面。
- macOS / Metal prototype 已改為非 Lumen 白晝照明並關閉 motion blur，降低截圖與 runtime 噪音。
- 生成貼圖最大尺寸 1024，未超出 v0.2 texture budget。
