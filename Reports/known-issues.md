# 已知問題

- macOS app bundle 已成功產出並可用 packaged executable smoke 啟動，但 ad-hoc strict codesign verify 在 iCloud File Provider 路徑下仍可能因 `com.apple.FinderInfo` extended attribute 顯示警告；目前命令列啟動通過，Finder / Gatekeeper 行為需另行驗證。
- v0.2 的世界幾何仍是 prototype blockout，生成美術主要透過 ArtSlot 材質、旗幟、告示、decals 與 UI 套用，不代表最終 3D 美術。
- Gate 17 perf guard 是 texture budget 與 automation smoke，尚未取代 UE Insights 或 Metal frame capture。
- `Builds/macOS/Ashfrontier.app` 是本機打包輸出，依現有 `.gitignore` 不提交到 git；可由 Scripts/package_macos.sh 重新產生。
