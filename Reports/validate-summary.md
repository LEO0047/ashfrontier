# Gate 驗證摘要

- 時間：2026-05-06 20:16:28 CST
- 專案根目錄：/Users/leo/Library/Mobile Documents/com~apple~CloudDocs/Coding/Projects/Ashfrontier

## Git
- 通過：目前在 git repo 內
- 通過：目前 branch 是 main
- 通過：origin remote 正確
- 通過：git diff --check 通過
- 通過：沒有 unresolved merge conflicts

## 必備文件
- 通過：README.md 存在
- 通過：Docs/PLAN.md 存在
- 通過：Docs/TECH_SPEC.md 存在
- 通過：Docs/CONTENT_SCHEMA.md 存在
- 通過：Docs/TEST_PLAN.md 存在
- 通過：Docs/ADR/ADR-001-engine-and-scope.md 存在
- 通過：Docs/ADR/ADR-002-data-driven-content.md 存在
- 通過：Docs/ADR/ADR-003-single-player-only.md 存在
- 通過：Docs/ADR/ADR-004-git-validation-gates.md 存在
- 通過：Docs/ADR/ADR-005-macos-first-target.md 存在
- 通過：Docs/Research/Kenshi2_DeepResearch_zh-TW.md 存在
- 通過：Scripts/env.example 存在
- 通過：Scripts/validate.sh 存在
- 通過：Scripts/content_lint.py 存在
- 通過：Scripts/commit_gate.sh 存在
- 通過：Reports/environment.md 存在
- 通過：Reports/gate-00-report.md 存在

## Gate 01 專案骨架
- 通過：Ashfrontier.uproject 存在
- 通過：Source/Ashfrontier/Ashfrontier.Build.cs 存在
- 通過：Source/Ashfrontier.Target.cs 存在
- 通過：Source/AshfrontierEditor.Target.cs 存在
- 通過：Source/Ashfrontier/Private/Ashfrontier.cpp 存在
- 通過：Source/Ashfrontier/Private/AshfrontierGameMode.cpp 存在
- 通過：Source/Ashfrontier/Private/AshfrontierPlayerController.cpp 存在
- 通過：Source/Ashfrontier/Private/AshfrontierCharacter.cpp 存在
- 通過：Source/Ashfrontier/Private/AshfrontierHUD.cpp 存在
- 通過：Source/Ashfrontier/Private/Tests/AshfrontierSmokeTests.cpp 存在
- 通過：Config/DefaultEngine.ini 存在
- 通過：Config/DefaultGame.ini 存在
- 通過：Config/DefaultInput.ini 存在
- 通過：Scripts/create_gate01_map.sh 存在
- 通過：BuildScripts/create_gate01_map.py 存在
- 通過：Ashfrontier.uproject JSON 語法通過

## 繁體中文文件檢查
- 通過：中文文件基本檢查通過

## 腳本檢查
- 通過：Scripts/validate.sh 可執行
- 通過：Scripts/validate.sh bash 語法通過
- 通過：Scripts/run_tests.sh 可執行
- 通過：Scripts/run_tests.sh bash 語法通過
- 通過：Scripts/soak_test.sh 可執行
- 通過：Scripts/soak_test.sh bash 語法通過
- 通過：Scripts/perf_capture.sh 可執行
- 通過：Scripts/perf_capture.sh bash 語法通過
- 通過：Scripts/package_macos.sh 可執行
- 通過：Scripts/package_macos.sh bash 語法通過
- 通過：Scripts/commit_gate.sh 可執行
- 通過：Scripts/commit_gate.sh bash 語法通過
- 通過：Scripts/create_gate01_map.sh 可執行
- 通過：Scripts/create_gate01_map.sh bash 語法通過
- 通過：Scripts/content_lint.py Python 語法通過

## Content Lint
- 通過：content_lint.py 通過

## UE 測試入口
Opening shared memory
Shared memory doesn't exist, checking legacy lock file
Checking for a '/tmp/UnrealTraceServer.pid' lock file
Forking process
Wait until we know the daemon has started.
Parent received signal 30
Daemon signalled successful start
Forked complete (ret=0)
- 通過：run_tests.sh smoke 通過

## 結果
- 警告數：0
- 失敗數：0
Opening shared memory
Shared memory doesn't exist, checking legacy lock file
Checking for a '/tmp/UnrealTraceServer.pid' lock file
Forking process
Opening shared memory
Starting the store
Reading settings from '/Users/leo/UnrealEngine/UnrealTrace/Settings.ini'

Unable to open settings file /Users/leo/UnrealEngine/UnrealTrace/Settings.ini.

Store settings (/Users/leo/UnrealEngine/UnrealTrace/Settings.ini):
 - Store port: 1989
 - Recorder port: 1981
 - Thread count: 0
 - Sponsored mode: 1
 - Directory: '/Users/leo/UnrealEngine/UnrealTrace/Store/001'
Watcher enabled on /Users/leo/UnrealEngine/UnrealTrace/Store/001
Signalling parent 88510
Terminating server, no sponsors or connections active.
Received signal 15
Daemon is exiting without errors.
Listening cancelled, closing port...
Listening cancelled, closing port...
