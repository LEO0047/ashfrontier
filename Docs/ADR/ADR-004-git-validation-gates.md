# ADR-004：Git 驗證 Gate

## Status

Accepted

## Context

使用者要求每個 Gate 完成後先驗證與測試，通過後才 commit，然後 push 到 GitHub `main`。失敗狀態不得 commit。

## Decision

每個 Gate 使用 `Scripts/validate.sh` 作為主要驗證入口，並以 `Scripts/commit_gate.sh <gate-id> "<中文 commit message>"` 執行 commit / push。

推送狀態與 Gate commit hash 由 `Reports/git-push-status.md` 與對應 `Reports/gate-XX-report.md` 記錄。因為 commit hash 無法穩定寫入同一個 commit 自身，commit gate 允許在功能 commit push 後建立一個同 Gate 的報告 commit，用來記錄已推送的 Gate commit。

## Consequences

- 驗證失敗時不得 commit / push。
- 每個 Gate 至少有一個主要 commit。
- 報告 commit 只記錄推送狀態，不應混入功能變更。
