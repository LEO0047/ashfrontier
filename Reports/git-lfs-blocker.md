# Git LFS Blocker

## 摘要

Gate 10 檢查到本機目前沒有可用的 `git-lfs` 指令。

## 已確認事項

- `.gitattributes` 已建立，會追蹤 UE 與圖片資產型別。
- `git lfs version` 回傳：

```text
git: 'lfs' is not a git command. See 'git --help'.
```

## 影響

- Gate 10 可以先提交文件、manifest、lint 與目錄骨架。
- 後續 Gate 12 起若要 commit `*.png`、`*.uasset` 或其他 LFS 追蹤資產，必須先讓本機 `git-lfs` 可用。
- 在 `git-lfs` 可用前，不應假裝圖片與 UE 二進位資產已正確走 LFS。

## 恢復步驟

1. 安裝或啟用 `git-lfs`。
2. 在 repo 內執行 `git lfs version` 確認指令可用。
3. 執行 `git lfs install`。
4. 重新執行 `./Scripts/validate.sh`。
