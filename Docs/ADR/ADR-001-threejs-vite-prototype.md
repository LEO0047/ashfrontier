# ADR-001：使用 Three.js 與 Vite 建立 Ashfrontier 原型

## 狀態

Accepted

## 背景

Ashfrontier 第一階段目標是讓 Codex 能在單一 repo 內自動產生、驗證、commit 與 push 可玩的 3D prototype。大型二進位遊戲引擎會引入編輯器狀態、專案檔與資產匯入流程，降低自動化驗證的穩定性。

## 決策

使用 TypeScript、Vite 與 Three.js 建立瀏覽器原生 3D prototype。核心邏輯保持資料導向，renderer 只負責顯示與輸入。

## 後果

- 優點：可用一般 CI、unit test、e2e 與靜態建置驗證。
- 優點：placeholder 3D 資產可由程式產生，不依賴未授權素材。
- 代價：不具備 UE5 等大型引擎的美術工具鏈、物理系統與動畫工作流。
