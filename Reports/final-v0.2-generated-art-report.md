# Ashfrontier v0.2 生成美術套皮可玩版最終報告

- 狀態：Gate 18 打包、驗證與 commit gate 已完成；v0.2 annotated tag 於本報告提交後建立
- Build：Builds/macOS/Ashfrontier.app
- Launch smoke：Reports/mac-launch-smoke.md
- Build summary：Reports/mac-build-summary.md
- 最終美術清冊：Reports/Art/final-art-assets-inventory.md
- 最終視覺摘要：Reports/Art/final-visual-summary.md
- 已知問題：Reports/known-issues.md

## 完成範圍

- Gate 10 到 Gate 18 已完成並推送到 origin/main。
- Gate 18 已成功執行 macOS package，產出 Builds/macOS/Ashfrontier.app。
- Packaged executable 以 -NullRHI -unattended smoke 乾淨啟動並退出。
- v0.2 生成美術資產、metadata、ArtGenManifest、ArtSlot mapping、UI assignments、faction assignments、character portrait assignments 與 visual variant assignments 維持完整。
- 固定相機截圖、visual summary、art coverage、art perf impact 與 UE automation 報告皆已產出。

## 驗證摘要

- validate.sh：Gate 18 通過。
- package_macos.sh：通過。
- mac-launch-smoke：通過。
- screenshot smoke：通過。
- art_manifest_lint.py：通過。
- art_prompt_lint.py：通過。
- art_coverage_lint.py：通過。
- ui_art_lint.py：通過。
- visual_variant_lint.py：通過。
- UE automation：通過。

## 最終 Git 狀態

- Gate 18 commit gate：已完成並推送到 origin/main。
- v0.2 tag：本報告提交後建立 annotated tag，最終 commit 以 Git tag dereference 驗證。
- v0.1-gate09-editor-playable：已確認維持 5fdb11433459742b7f6a9e10e2e33f6ed523826b。

## 注意

Builds/macOS/Ashfrontier.app 依現有 .gitignore 作為本機打包輸出保留，不提交至 git。若需要重建，使用 Scripts/package_macos.sh。
