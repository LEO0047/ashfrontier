# ADR-002：資料驅動內容

## Status

Accepted

## Context

Ashfrontier 需要派系、物品、資源、配方、建築、NPC schedule、dialogue conditions 與 legal rules。若這些內容散落在 Blueprint 或一次性腳本裡，後續 Gate 會難以測試與維護。

## Decision

所有核心內容必須資料驅動，並由 `Scripts/content_lint.py` 驗證。Gate 02 起建立初始原創資料，使用穩定 ASCII `id`，使用者可見文字使用繁體中文。

## Consequences

- Content lint 成為每個 Gate 的基本驗證。
- 資料錯誤必須在進入 UE runtime 前被攔下。
- Blueprint 可用於組裝，但不得成為唯一資料來源。
