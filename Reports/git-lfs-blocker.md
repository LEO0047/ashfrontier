# Git LFS 狀態

## 摘要

Gate 10 檢查到本機當時沒有可用的 `git-lfs` 指令。Gate 12 前已用 Homebrew 安裝 `git-lfs`，並在本 repo 執行 `git lfs install --local`。

## 已確認事項

- `.gitattributes` 已建立，會追蹤 UE 與圖片資產型別。
- Gate 10 當時 `git lfs version` 回傳：

```text
git: 'lfs' is not a git command. See 'git --help'.
```

- Gate 12 目前 `git lfs version` 回傳：

```text
git-lfs/3.7.1 (GitHub; darwin arm64; go 1.25.3)
```

## 影響

- Gate 10 與 Gate 11 已先提交文件、manifest、lint 與目錄骨架。
- Gate 12 起可以提交 `.gitattributes` 追蹤的圖片資產。
- 仍需在 commit 前用 `git lfs status` 或 `git check-attr filter -- <file>` 確認圖片資產會走 LFS。

## 已執行恢復步驟

1. 執行 `brew install git-lfs`。
2. 執行 `git lfs install --local`。
3. 執行 `git lfs version` 驗證。
