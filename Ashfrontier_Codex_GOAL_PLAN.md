# Ashfrontier：Codex 單一 /goal 最終開發計畫

> 目標 repo：<https://github.com/LEO0047/ashfrontier>  
> 本機專案路徑：`/Users/leo/Library/Mobile Documents/com~apple~CloudDocs/Coding/Projects/Ashfrontier`  
> 研究參考檔：`Kenshi 2 深度研究與 Codex 單一目標開發藍圖.md`

---

## 最終可貼進 Codex 的 `/goal`

```text
/goal

你是資深遊戲工程師、技術製作人、測試自動化工程師與嚴格 code reviewer 的合體。請在單一主目標內完成 Ashfrontier 的第一個 3D 可玩原型，不要拆成多個人工管理的小計畫。你可以在同一 goal 內自行使用 subagents 或內部分工，但所有工作必須回到同一套驗收標準、同一個 repo、同一條 main branch，並在每個階段完成驗證、commit、push 到 GitHub。

本任務必須全程使用繁體中文撰寫文件、階段報告、UI 文字、commit message 與最終回報。程式識別字可以使用英文，因為這樣比較符合工程慣例；但註解、文件、測試說明、遊戲內 UI、錯誤訊息與報告必須使用自然繁體中文。

============================================================
一、最高目標
============================================================

在 GitHub repo `https://github.com/LEO0047/ashfrontier` 內，建立一個可在瀏覽器執行的 3D 單機小隊沙盒 RPG 原型，名稱為 Ashfrontier。

這不是 Kenshi 同人重製，也不是複製 Kenshi 2 的名稱、角色、派系、地點或資產。請只吸收「小隊沙盒、荒原求生、派系反應、失敗也能產生故事、基地建設、生產鏈、資料驅動內容」這些系統精神，所有內容必須使用 Ashfrontier 的原創 placeholder 或自有命名。

最終結果必須是一個可玩的 3D prototype，玩家可以：
1. 在荒原場景中控制 1 到 4 人小隊。
2. 使用第三人稱 / 戰術縮放鏡頭觀察世界。
3. 點選角色、下達移動、攻擊、跟隨、待命、搬運倒地隊友等命令。
4. 與 NPC 發生即時戰鬥，並看到部位傷害、流血、倒地與簡化治療。
5. 在小型城鎮中看到守衛巡邏、商人、招募 NPC、禁區與偷竊反應。
6. 買賣物品，管理小隊庫存。
7. 在野外建造簡化基地，至少包含 5 種建築與 2 條生產鏈。
8. 儲存與讀取世界狀態，包含角色、血量、庫存、建築、生產隊列與派系關係。
9. 通過自動化測試、建置、瀏覽器 smoke test 與資料內容檢查。
10. 每一個階段完成後都必須驗證、commit、push 到 GitHub。

============================================================
二、前置脈絡
============================================================

本機工作目錄預設為：

`/Users/leo/Library/Mobile Documents/com~apple~CloudDocs/Coding/Projects/Ashfrontier`

研究參考檔預設位於：

`/Users/leo/Library/Mobile Documents/com~apple~CloudDocs/Coding/Projects/Ashfrontier/Kenshi 2 深度研究與 Codex 單一目標開發藍圖.md`

如果該路徑不存在，請不要中止整個任務。請改用本 goal 內的產品規格作為權威來源，並在 `/Docs/PHASE_REPORTS/phase-0.md` 記錄「未讀取到研究檔，已使用 goal 內建摘要繼續」。

GitHub repo：

`https://github.com/LEO0047/ashfrontier`

如果本機目錄尚未 clone repo，請 clone；如果目錄已存在，請確認 git remote origin 指向上述 repo。若 repo 為空，請從零建立專案。若 repo 已有內容，請先讀 README、package.json 與現有結構，再保留可用內容並補齊缺口。

============================================================
三、技術選型
============================================================

請使用以下技術路線，因為本目標重點是讓 Codex 能自己產生、驗證、測試、commit、push，而不是依賴大型二進位引擎編輯器。

主技術：
- TypeScript
- Vite
- Three.js
- Vitest
- Playwright
- ESLint
- Prettier
- GitHub Actions

狀態與資料：
- 遊戲核心邏輯必須盡量與 Three.js renderer 解耦。
- 使用 deterministic seed，讓測試可以穩定重跑。
- 使用 JSON / TypeScript data schema 管理 factions、items、resources、recipes、buildings、characters、npc schedules。
- 使用 Zod 或等效 schema 驗證資料內容。
- 儲存檔使用 JSON 格式，可存到 localStorage，也能 export/import。

渲染與互動：
- 使用 Three.js 建立低多邊形 3D 荒原。
- 所有角色、建築、資源點都可以先用 cube、capsule、cylinder、billboard label 或簡化模型表示。
- 不等待終版美術，不引入未授權資產。
- 必須有可玩的攝影機、角色選取、地面點擊移動、戰鬥與建造互動。

============================================================
四、強制限制
============================================================

1. 不要建立多個人工管理的小計畫。
2. 不要停在「我需要使用者確認」；除非遇到無法操作 GitHub 的權限錯誤，否則請自行做合理假設並前進。
3. 不要使用 Kenshi、Lo-Fi Games、Hiver、Old Empire、Holy Nation、United Cities、Shek、Beep 等受保護或高相似度名稱作為遊戲內容。
4. 不要使用未授權圖片、音樂、模型、商標、logo 或商店素材。
5. 不要只做靜態展示頁。最後必須是可互動、可操作、可測試的 3D playable prototype。
6. 不要只 commit 文件。每個功能階段都必須有可執行增量與驗證報告。
7. 每個階段完成後必須跑驗證，驗證未過不可 commit 作為階段完成。
8. 每個階段完成後必須 push 到 `origin main`。
9. 所有重要決策必須記錄在 `/Docs/ADR/`。
10. 若自動化測試因環境限制無法跑，必須建立替代驗證與明確報告，但不能默默跳過。

============================================================
五、專案結構
============================================================

請建立或整理成以下結構：

/src
  /app
  /game
    /core
    /systems
    /render
    /data
    /ui
    /save
    /tests
/public
/Docs
  /ADR
  /PHASE_REPORTS
  PLAN.md
  TECH_SPEC.md
  CONTENT_SCHEMA.md
  TEST_PLAN.md
/Reports
  test-results.xml
  perf-summary.md
  content-lint.md
/scripts
  verify.mjs
  content-lint.mjs
  smoke-report.mjs
/tests
  /unit
  /e2e
.github
  /workflows
    ci.yml

必要 npm scripts：

- `pnpm dev`
- `pnpm lint`
- `pnpm format:check`
- `pnpm typecheck`
- `pnpm test`
- `pnpm test:e2e`
- `pnpm build`
- `pnpm verify`
- `pnpm content:lint`
- `pnpm report:smoke`

`pnpm verify` 必須串起至少：
1. format check
2. lint
3. typecheck
4. unit tests
5. content lint
6. build

Playwright e2e 可以在階段 1 之後逐步補強，但最終必須包含可啟動遊戲、移動角色、觸發戰鬥、建造建築、存讀檔的 smoke test。

============================================================
六、階段執行協議
============================================================

每個階段都要照以下流程執行：

1. 讀取目前 repo 狀態：
   - `git status --short`
   - `git branch --show-current`
   - `git remote -v`

2. 若不是 main branch，請切回 main 或建立可安全合併的策略，但最終必須 push 到 main。

3. 開始階段前，在階段報告中記錄：
   - 階段目標
   - 預計修改檔案
   - 風險
   - 驗證方式

4. 實作本階段功能。

5. 執行本階段驗證：
   - `pnpm verify`
   - 如果 e2e 已建立，執行 `pnpm test:e2e`
   - 如果本階段涉及畫面或互動，至少執行一次本機 smoke 或 Playwright smoke

6. 如果驗證失敗：
   - 先修復。
   - 再重跑驗證。
   - 不得把失敗狀態當作階段完成。
   - 若環境限制導致無法執行，必須在 `/Reports/` 與階段報告中記錄原因、替代驗證、殘留風險。

7. 更新文件：
   - `/Docs/PLAN.md`
   - `/Docs/TECH_SPEC.md`
   - `/Docs/TEST_PLAN.md`
   - `/Docs/PHASE_REPORTS/phase-N.md`
   - 必要時新增 `/Docs/ADR/ADR-NNN-*.md`

8. commit：
   - 使用繁體中文 commit message。
   - 格式：`階段 N：<一句話說明>`
   - commit 前必須 `git diff --check`
   - commit 後必須記錄 commit SHA 到階段報告。

9. push：
   - `git push origin main`
   - push 完成後執行 `git status --short`
   - 在最終回報中列出每一階段 commit SHA 與驗證結果。

============================================================
七、階段門禁
============================================================

請依序完成以下階段。這些是同一個 /goal 的 gate，不是分開的小計畫。

------------------------------------------------------------
階段 0：專案啟動、repo 骨架、文件與 CI
------------------------------------------------------------

目標：
建立可維護的 TypeScript / Vite / Three.js 專案骨架，並讓 repo 從空白變成可建置、可測試、可 push 的工程專案。

必做：
- 初始化 pnpm 專案。
- 建立 Vite + TypeScript。
- 安裝 Three.js、Vitest、Playwright、ESLint、Prettier、Zod。
- 建立 GitHub Actions CI。
- 建立 Docs 與 Reports 結構。
- 建立 `pnpm verify`。
- 建立初版 `PLAN.md`、`TECH_SPEC.md`、`CONTENT_SCHEMA.md`、`TEST_PLAN.md`。
- 建立 ADR：為什麼 Codex prototype 選 Three.js 而不是 UE5。

驗證：
- `pnpm install`
- `pnpm verify`
- `pnpm build`
- GitHub Actions workflow 檔案存在且語法合理。

commit：
`階段 0：建立 Ashfrontier 專案骨架與驗證管線`

push：
必須 push 到 `origin main`。

------------------------------------------------------------
階段 1：3D 場景、攝影機與可操作角色
------------------------------------------------------------

目標：
建立可啟動的 3D 荒原場景，玩家可以控制一名角色移動。

必做：
- 建立 Three.js renderer、scene、camera、lights。
- 建立地形平面、荒原色塊、簡化障礙物、資源點 placeholder。
- 建立玩家角色 capsule/cylinder placeholder。
- 支援 WASD 或點擊地面移動。
- 支援戰術縮放與旋轉鏡頭。
- 建立基礎 HUD：角色狀態、操作提示、事件 log。
- 建立 Playwright smoke test：頁面可載入，canvas 存在，角色可移動。

驗證：
- `pnpm verify`
- `pnpm test:e2e`
- 手動或自動 smoke report 寫入 `/Reports/perf-summary.md` 初版。

commit：
`階段 1：建立 3D 荒原場景與角色控制`

push：
必須 push 到 `origin main`。

------------------------------------------------------------
階段 2：小隊選取、命令系統與路徑移動
------------------------------------------------------------

目標：
從單一角色擴展到 1 到 4 人小隊，支援選取、編隊與命令。

必做：
- 建立 squad manager。
- 支援多角色選取與目前選取高亮。
- 支援點擊地面下達移動命令。
- 建立簡化 A* grid pathfinding 或 waypoint pathfinding。
- 支援 follow、hold、move、attack-move 的命令資料結構。
- UI 顯示小隊名單、生命狀態、目前命令。
- 加入 deterministic test，測試命令狀態轉換。

驗證：
- `pnpm verify`
- `pnpm test:e2e`
- e2e 驗證選取角色、下達移動、角色位置改變。

commit：
`階段 2：完成小隊選取與命令系統`

push：
必須 push 到 `origin main`。

------------------------------------------------------------
階段 3：NPC、巡邏、城鎮與資料驅動內容
------------------------------------------------------------

目標：
建立第一個可巡邏的城鎮，以及 NPC schedule / faction / item 的資料驅動基礎。

必做：
- 建立原創城鎮：例如「鹽脊鎮」。
- 建立 3 個原創派系，例如：
  - 灰燼契會
  - 鹽冠商盟
  - 鐵路遊民
- 建立 NPC 資料 schema。
- 建立 patrol route。
- 建立商人、守衛、流民三種 NPC archetype。
- 建立資料內容 lint，檢查派系、NPC、物品、路線引用是否有效。
- 建立事件 log，顯示 NPC 巡邏、警戒、交易提示。

驗證：
- `pnpm verify`
- `pnpm content:lint`
- unit test 驗證資料 schema。
- e2e 驗證 NPC 出現在城鎮且會巡邏。

commit：
`階段 3：加入城鎮 NPC 巡邏與資料驅動內容`

push：
必須 push 到 `origin main`。

------------------------------------------------------------
階段 4：即時戰鬥、部位傷害、倒地與搬運
------------------------------------------------------------

目標：
建立 Ashfrontier 的核心味道：打輸不是結束，而是系統事件的開始。

必做：
- 建立 combat resolver。
- 建立 body parts：
  - 頭
  - 胸
  - 腹
  - 左臂
  - 右臂
  - 左腿
  - 右腿
- 每個部位有 HP、狀態與受傷效果。
- 建立攻擊命中、傷害、流血、倒地、昏迷狀態。
- 建立簡化治療與包紮。
- 建立搬運倒地隊友命令。
- UI 顯示部位傷害與流血狀態。
- NPC 可攻擊玩家，玩家也可攻擊 NPC。

驗證：
- `pnpm verify`
- unit test 驗證傷害計算、倒地、流血與治療。
- e2e 驗證玩家可攻擊訓練敵人，敵人可倒地，隊友可搬運倒地角色。
- 儲存一份 combat smoke report 到 `/Reports/test-results.xml` 或 `/Reports/combat-summary.md`。

commit：
`階段 4：完成即時戰鬥與部位傷害系統`

push：
必須 push 到 `origin main`。

------------------------------------------------------------
階段 5：招募、交易、庫存與物品系統
------------------------------------------------------------

目標：
讓小隊開始像沙盒 RPG，而不是只有戰鬥 demo。

必做：
- 建立 item schema。
- 建立 inventory system。
- 建立商店交易 UI。
- 建立貨幣。
- 玩家可買、賣、轉移物品。
- 建立招募 NPC flow。
- 招募後 NPC 加入小隊，上限 4 人。
- 建立裝備或簡化武器欄位。
- 交易與招募要寫入事件 log。

驗證：
- `pnpm verify`
- unit test 驗證庫存增減、交易價格、招募上限。
- e2e 驗證玩家可與商人交易、招募一名 NPC。

commit：
`階段 5：加入招募交易與小隊庫存`

push：
必須 push 到 `origin main`。

------------------------------------------------------------
階段 6：基地建設、資源採集與生產鏈
------------------------------------------------------------

目標：
建立可玩的據點 loop，玩家能從荒原資源轉成生產與生存能力。

必做：
- 建立 build mode。
- 玩家可放置至少 5 種建築：
  - 營火
  - 儲物箱
  - 簡易採礦點
  - 食物加工台
  - 金屬加工台
- 建立至少 2 條生產鏈：
  - 食物鏈：原料 → 加工 → 口糧
  - 金屬鏈：礦石 → 金屬板 → 建材
- 建立資源節點。
- 建立生產隊列。
- 建築與生產狀態需顯示在 UI。
- 生產與採集必須消耗時間，不可瞬間完成。

驗證：
- `pnpm verify`
- unit test 驗證 recipe、資源消耗、生產輸出。
- e2e 驗證玩家可建造、採集、開始生產、取得產出。

commit：
`階段 6：完成基地建設與生產鏈原型`

push：
必須 push 到 `origin main`。

------------------------------------------------------------
階段 7：派系關係、犯罪、禁區與守衛反應
------------------------------------------------------------

目標：
建立城市與派系反應，讓世界不是布景板。

必做：
- 建立 faction relation system。
- 建立 crime event：
  - 偷竊
  - 攻擊
  - 禁區闖入
  - 自衛
- 守衛能依事件做出反應：
  - 警告
  - 追捕
  - 攻擊
  - 放行
- 建立 suspicion / hostility 狀態。
- UI 顯示派系關係與通緝狀態。
- 事件記憶至少能保存最近 N 個事件。
- 儲存檔需包含派系關係與犯罪狀態。

驗證：
- `pnpm verify`
- unit test 驗證犯罪判定與派系關係變化。
- e2e 驗證玩家闖入禁區後守衛反應。
- e2e 驗證玩家自衛不被錯判為主動攻擊，若太難可先寫明簡化規則。

commit：
`階段 7：加入派系犯罪與守衛反應`

push：
必須 push 到 `origin main`。

------------------------------------------------------------
階段 8：存讀檔、整合打磨、效能報告與最終可玩版
------------------------------------------------------------

目標：
把所有系統收斂成一個可玩、可測、可交付的 prototype。

必做：
- 完成 save/load manager。
- 儲存內容至少包含：
  - 小隊位置
  - 角色生命與部位傷害
  - 庫存
  - 建築
  - 生產隊列
  - 派系關係
  - 犯罪事件
  - 世界時間
- 建立 export/import save JSON。
- 建立首頁或 start screen。
- 建立「新遊戲」、「繼續遊戲」、「匯出存檔」、「匯入存檔」。
- 整理 UI，讓玩家知道下一步能做什麼。
- 建立 README，包含玩法、操作、開發指令、測試指令。
- 建立 `/Docs/FINAL_REPORT.md`，列出已完成、未完成、已知問題與下一步。
- 建立最終 smoke test。
- 產出 `/Reports/perf-summary.md`。
- 產出 `/Reports/content-lint.md`。
- 產出 `/Reports/test-results.xml` 或等效測試摘要。

驗證：
- `pnpm verify`
- `pnpm test:e2e`
- `pnpm build`
- 手動或自動確認 dist 可啟動。
- e2e 驗證新遊戲、移動、戰鬥、建造、存檔、讀檔流程。

commit：
`階段 8：完成 Ashfrontier 可玩原型與最終驗證`

push：
必須 push 到 `origin main`。

============================================================
八、最終驗收標準
============================================================

整個 /goal 完成時，必須滿足：

1. GitHub repo `LEO0047/ashfrontier` 有完整可執行專案。
2. main branch 已 push 最新成果。
3. README 使用繁體中文，且能讓新工程師在 10 分鐘內跑起來。
4. `pnpm install && pnpm verify && pnpm build` 可以通過。
5. 若 Playwright 環境可用，`pnpm test:e2e` 可以通過。
6. 遊戲可在瀏覽器中操作，不是靜態頁。
7. 至少完成：
   - 3D 場景
   - 角色控制
   - 小隊命令
   - NPC 巡邏
   - 即時戰鬥
   - 部位傷害
   - 搬運倒地隊友
   - 交易
   - 招募
   - 基地建設
   - 生產鏈
   - 派系犯罪反應
   - 存讀檔
8. `/Docs/PHASE_REPORTS/` 內有每階段報告。
9. 每階段都有對應 commit，且已 push。
10. 最終回報必須列出：
    - 每階段 commit SHA
    - 每階段驗證指令與結果
    - 最終可玩功能清單
    - 已知限制
    - 下一步建議

============================================================
九、失敗處理規則
============================================================

如果遇到問題，請依序處理：

1. 如果是測試失敗，先修測試或修功能，不要跳過。
2. 如果是環境缺套件，請補安裝設定或改寫 scripts，使 repo 更容易重現。
3. 如果是 Playwright 在目前環境不能跑，請保留 Playwright 測試，並用 unit/build/content lint 先驗證，同時在報告記錄原因。
4. 如果是 GitHub push 權限問題，請停止並回報明確錯誤。
5. 如果某功能超出單次可完成範圍，請做最小可玩版本，但不得刪掉該功能的測試與文件追蹤。
6. 不要因為做不出高品質美術而卡住。用 placeholder 完成功能。
7. 不要因為想做完美架構而卡住。先讓系統活起來，再重構。

============================================================
十、最終回報格式
============================================================

完成後，請用繁體中文回報：

# Ashfrontier /goal 完成報告

## 總結
用 5 到 8 句話說明完成狀態。

## Git 狀態
- repo:
- branch:
- latest commit:
- pushed to origin/main: yes/no

## 階段結果
| 階段 | commit SHA | 驗證 | 備註 |
|---|---|---|---|

## 可玩功能
列出玩家現在可以實際操作的功能。

## 驗證結果
列出實際跑過的 commands 與結果。

## 產物
列出 README、Docs、Reports、dist build 等位置。

## 已知限制
誠實列出尚未完成或簡化的地方。

## 下一步
只列 5 個最高 ROI 的下一步，不要寫成發散願望清單。
```

---

## 給 LEO 的一句話使用方式

在本機先確認目錄存在，然後把上面整段 `/goal` 貼進 Codex：

```bash
cd "/Users/leo/Library/Mobile Documents/com~apple~CloudDocs/Coding/Projects/Ashfrontier"
```

如果資料夾還沒 clone repo，先跑：

```bash
git clone https://github.com/LEO0047/ashfrontier.git "/Users/leo/Library/Mobile Documents/com~apple~CloudDocs/Coding/Projects/Ashfrontier"
cd "/Users/leo/Library/Mobile Documents/com~apple~CloudDocs/Coding/Projects/Ashfrontier"
```

然後貼上 `/goal`。這份設計的重點是：**不是叫 Codex 做一張漂亮展示頁，而是逼它每個階段都交出可玩增量、驗證結果、commit 與 push。**
