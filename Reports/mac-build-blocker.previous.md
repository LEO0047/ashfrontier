# macOS Build Blocker

- 狀態：打包失敗
- 原因：macOS cook shader 編譯失敗：Xcode 缺少 Metal Toolchain。`Reports/package-macos.log` 與 cook log 內出現 `cannot execute tool 'metal' due to missing Metal Toolchain; use: xcodebuild -downloadComponent MetalToolchain`，並伴隨 UE shader compile SIGSEGV。
- 開始時間：2026-05-06 20:29:10 CST
- 輸出目標：/Users/leo/Library/Mobile Documents/com~apple~CloudDocs/Coding/Projects/Ashfrontier/Builds/macOS/Ashfrontier.app
- Log：Reports/package-macos.log
- 已完成替代驗證：Editor C++ build 與 UE automation 可作為替代驗證；不得宣稱 packaged build 完成。
- 下一步：執行 `xcodebuild -downloadComponent MetalToolchain` 安裝 Metal Toolchain 後，重新執行 `Scripts/package_macos.sh`。若仍在 iCloud 工作區遇到 codesign metadata 問題，保留目前 `/tmp` 乾淨打包工作區策略。
