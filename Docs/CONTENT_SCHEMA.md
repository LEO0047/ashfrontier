# Ashfrontier 內容 Schema

內容資料會以 TypeScript 物件搭配 Zod schema 驗證，涵蓋：

- factions：派系、初始關係與敵意狀態。
- items：物品、重量、價格與用途。
- resources：資源節點與採集輸出。
- recipes：生產輸入、輸出與時間。
- buildings：建築成本、功能與生產槽位。
- characters：角色原型、身體部位、技能與庫存。
- npc schedules：NPC 巡邏、商店、守衛與禁區行為。

階段 3 起，`pnpm content:lint` 會檢查引用完整性與命名限制。
