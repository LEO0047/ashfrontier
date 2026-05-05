# ADR-003：單機限定

## Status

Accepted

## Context

多人連線會讓小隊控制、AI、NavMesh、建造、生產、戰鬥、受傷、事件記憶與存讀檔都增加同步複雜度。此 goal 要求專注於可玩的單機 prototype。

## Decision

Ashfrontier 在本 goal 內只做單機。不接 EOS、Steamworks、主機平台服務或 multiplayer replication。

## Consequences

- 系統設計可優先確保本機資料一致性與可存讀。
- Gate 驗證不包含 multiplayer tests。
- 若未來需要 co-op，必須另行建立獨立技術評估，不併入本 prototype Gate。
