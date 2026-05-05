# ADR-001：引擎與原型範圍

## Status

Accepted

## Context

Ashfrontier 的目標是 UE5 3D 單機小隊沙盒 RPG playable prototype。研究文件指出系統標竿重點在小隊沙盒、反應式世界、資料驅動、AI、基地與經濟，不是單純場景展示。

## Decision

採用 UE5 作為主要引擎。Gate 01 起建立 `Ashfrontier.uproject` 與核心 C++ module。Prototype 範圍固定為一個主城 blockout、一個小型前哨、一個野外建造區、三個原創派系與一到四名玩家小隊。

## Consequences

- 優先投資核心系統、資料驗證與 macOS 可建置流程。
- 不做多人連線、EOS、Steamworks、主機平台或 Windows build。
- placeholder 資產可接受，但 playable loop 必須可驗證。
