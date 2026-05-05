# Ashfrontier 測試計畫

## 必跑驗證

- `pnpm format:check`
- `pnpm lint`
- `pnpm typecheck`
- `pnpm test`
- `pnpm content:lint`
- `pnpm build`
- `pnpm verify`

## 瀏覽器測試

Playwright e2e 會逐步覆蓋：

- 啟動遊戲並確認 canvas 存在。
- 移動角色。
- 選取小隊並下達命令。
- 觸發 NPC 巡邏與戰鬥。
- 建造、採集、生產。
- 存檔與讀檔。

若本機瀏覽器環境缺少 Playwright browser，需保留測試並在報告記錄限制。
