# 階段 0 報告：專案啟動、repo 骨架、文件與 CI

## 目標

建立可維護的 TypeScript / Vite / Three.js 專案骨架，並讓 repo 從空白變成可建置、可測試、可 push 的工程專案。

## 預計修改檔案

- `package.json`
- `index.html`
- `src/app/main.ts`
- `src/app/styles.css`
- `scripts/*`
- `Docs/*`
- `Reports/*`
- `.github/workflows/ci.yml`
- TypeScript、ESLint、Prettier、Vitest、Playwright 設定檔

## 風險

- 本機原本沒有 `pnpm`，需要先安裝才能跑標準驗證。
- Playwright browser 可能需要另行下載。

## 驗證方式

- `pnpm install`
- `pnpm verify`
- `pnpm build`
- `git diff --check`

## 結果

- `pnpm install`：通過，已產出 `pnpm-lock.yaml`。
- `pnpm verify`：通過，包含 format、lint、typecheck、unit test、content lint 與 build。
- `pnpm test:e2e`：通過，啟動畫面可載入。
- `git diff --check`：通過。
- commit SHA：於最終報告彙整。
