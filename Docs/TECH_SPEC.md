# Ashfrontier 技術規格

## 技術選型

- TypeScript：保護遊戲核心資料結構與系統界面。
- Vite：快速本機開發與靜態建置。
- Three.js：瀏覽器原生 3D 場景與互動。
- Vitest：核心系統 deterministic unit tests。
- Playwright：瀏覽器 smoke 與互動測試。
- Zod：資料內容 schema 驗證。
- ESLint / Prettier：維持可 review 的程式品質。

## 架構方向

`src/game/core` 保存純資料型別與 deterministic 邏輯；`src/game/systems` 保存命令、戰鬥、交易、生產、派系與存讀檔狀態轉換；`src/game/render` 專注 Three.js 呈現與 input picking；`src/game/ui` 專注 DOM HUD。

Renderer 不應成為權威狀態來源。測試優先驗證核心系統，再用 Playwright 驗證玩家可操作路徑。
