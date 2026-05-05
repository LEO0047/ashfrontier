#!/usr/bin/env python3
from __future__ import annotations

import json
import re
import sys
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
REPORTS = ROOT / "Reports"
REPORT_MD = REPORTS / "content-lint.md"
REPORT_JSON = REPORTS / "content-lint.json"
DATA_DIRS = [ROOT / "Content" / "Data", ROOT / "Source" / "Data", ROOT / "Data"]
ID_RE = re.compile(r"^[a-z0-9_]+$")
TRADITIONAL_ONLY_CHECK = set("简汉语开发测试验证构项")


def load_json(path: Path) -> Any:
    with path.open("r", encoding="utf-8") as fh:
        return json.load(fh)


def as_records(payload: Any) -> list[dict[str, Any]]:
    if isinstance(payload, list):
        return [item for item in payload if isinstance(item, dict)]
    if isinstance(payload, dict):
        for key in ("records", "items", "data", "entries", "factions", "resources", "recipes", "buildings", "npc_schedules", "legal_rules", "dialogue_conditions"):
            value = payload.get(key)
            if isinstance(value, list):
                return [item for item in value if isinstance(item, dict)]
        return [payload]
    return []


def dataset_name(path: Path, payload: Any) -> str:
    if isinstance(payload, dict) and isinstance(payload.get("type"), str):
        return payload["type"]
    return path.stem.lower().replace("-", "_")


def add_issue(issues: list[dict[str, str]], severity: str, path: Path, message: str) -> None:
    issues.append({"severity": severity, "file": str(path.relative_to(ROOT)), "message": message})


def check_common(records: list[dict[str, Any]], path: Path, issues: list[dict[str, str]]) -> set[str]:
    seen: set[str] = set()
    ids: set[str] = set()
    for index, record in enumerate(records):
        rid = record.get("id")
        label = f"{path.relative_to(ROOT)}[{index}]"
        if not isinstance(rid, str) or not rid:
            add_issue(issues, "error", path, f"{label} 缺少 id")
            continue
        if not ID_RE.match(rid):
            add_issue(issues, "error", path, f"{label} 的 id 必須是小寫 ASCII、數字或底線：{rid}")
        if rid in seen:
            add_issue(issues, "error", path, f"{label} 重複 id：{rid}")
        seen.add(rid)
        ids.add(rid)

        name = record.get("name_zh")
        if not isinstance(name, str) or not name.strip():
            add_issue(issues, "error", path, f"{label} 缺少 name_zh")
        for key in ("name_zh", "description_zh"):
            text = record.get(key)
            if isinstance(text, str) and any(ch in text for ch in TRADITIONAL_ONLY_CHECK):
                add_issue(issues, "error", path, f"{label} 的 {key} 疑似包含簡體中文：{text}")
    return ids


def number_at_least(record: dict[str, Any], key: str, minimum: float, path: Path, issues: list[dict[str, str]]) -> None:
    value = record.get(key)
    if value is None:
        return
    if not isinstance(value, (int, float)) or value < minimum:
        add_issue(issues, "error", path, f"{record.get('id', '<unknown>')} 的 {key} 必須大於等於 {minimum}")


def check_count_list(record: dict[str, Any], key: str, path: Path, issues: list[dict[str, str]]) -> None:
    value = record.get(key)
    if value is None:
        return
    if not isinstance(value, list) or not value:
        add_issue(issues, "error", path, f"{record.get('id', '<unknown>')} 的 {key} 必須是非空陣列")
        return
    for entry in value:
        if not isinstance(entry, dict):
            add_issue(issues, "error", path, f"{record.get('id', '<unknown>')} 的 {key} 包含非物件項目")
            continue
        if not isinstance(entry.get("id"), str):
            add_issue(issues, "error", path, f"{record.get('id', '<unknown>')} 的 {key} 項目缺少 id")
        count = entry.get("count")
        if not isinstance(count, (int, float)) or count <= 0:
            add_issue(issues, "error", path, f"{record.get('id', '<unknown>')} 的 {key} count 必須大於 0")


def detect_recipe_cycles(recipes: list[dict[str, Any]], path: Path, issues: list[dict[str, str]]) -> None:
    producer_by_output: dict[str, str] = {}
    for recipe in recipes:
        rid = recipe.get("id")
        if not isinstance(rid, str):
            continue
        for output in recipe.get("outputs", []):
            if isinstance(output, dict) and isinstance(output.get("id"), str):
                producer_by_output[output["id"]] = rid

    graph: dict[str, set[str]] = {}
    for recipe in recipes:
        rid = recipe.get("id")
        if not isinstance(rid, str):
            continue
        graph.setdefault(rid, set())
        for input_item in recipe.get("inputs", []):
            if isinstance(input_item, dict):
                producer = producer_by_output.get(input_item.get("id"))
                if producer and producer != rid:
                    graph[rid].add(producer)

    visiting: set[str] = set()
    visited: set[str] = set()

    def dfs(node: str, trail: list[str]) -> None:
        if node in visiting:
            cycle = " -> ".join(trail + [node])
            add_issue(issues, "error", path, f"recipe 形成循環：{cycle}")
            return
        if node in visited:
            return
        visiting.add(node)
        for next_node in graph.get(node, set()):
            dfs(next_node, trail + [node])
        visiting.remove(node)
        visited.add(node)

    for node in graph:
        dfs(node, [])


def write_reports(files: list[str], issues: list[dict[str, str]], warnings: list[str]) -> None:
    REPORTS.mkdir(parents=True, exist_ok=True)
    status = "pass" if not any(issue["severity"] == "error" for issue in issues) else "fail"
    payload = {"status": status, "files": files, "issues": issues, "warnings": warnings}
    REPORT_JSON.write_text(json.dumps(payload, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")

    lines = [
        "# Content Lint 報告",
        "",
        f"- 狀態：{'通過' if status == 'pass' else '失敗'}",
        f"- 檢查檔案數：{len(files)}",
        f"- 問題數：{len(issues)}",
        "",
    ]
    if warnings:
        lines.append("## Warnings")
        lines.extend(f"- {warning}" for warning in warnings)
        lines.append("")
    if issues:
        lines.append("## Issues")
        lines.extend(f"- [{issue['severity']}] {issue['file']}：{issue['message']}" for issue in issues)
        lines.append("")
    else:
        lines.append("## 結果")
        lines.append("- 目前沒有發現 content schema 錯誤。")
        lines.append("")
    REPORT_MD.write_text("\n".join(lines), encoding="utf-8")


def main() -> int:
    issues: list[dict[str, str]] = []
    warnings: list[str] = []
    files: list[str] = []
    datasets: dict[str, list[dict[str, Any]]] = {}
    ids_by_dataset: dict[str, set[str]] = {}
    path_by_dataset: dict[str, Path] = {}

    json_files: list[Path] = []
    for data_dir in DATA_DIRS:
        if data_dir.exists():
            json_files.extend(sorted(data_dir.rglob("*.json")))

    if not json_files:
        warnings.append("Gate 00 尚未建立 Content/Data；已跳過深度內容檢查，Gate 02 必須加入實際資料。")

    for path in json_files:
        files.append(str(path.relative_to(ROOT)))
        try:
            payload = load_json(path)
        except Exception as exc:
            add_issue(issues, "error", path, f"JSON 解析失敗：{exc}")
            continue
        name = dataset_name(path, payload)
        records = as_records(payload)
        datasets[name] = records
        path_by_dataset[name] = path
        ids_by_dataset[name] = check_common(records, path, issues)

    factions = ids_by_dataset.get("factions", set())

    for name, records in datasets.items():
        path = path_by_dataset[name]
        for record in records:
            if name == "items":
                number_at_least(record, "base_price", 0, path, issues)
                number_at_least(record, "stack_size", 0, path, issues)
                number_at_least(record, "weight", 0, path, issues)
            elif name == "resources":
                number_at_least(record, "gather_time_seconds", 0, path, issues)
            elif name == "recipes":
                check_count_list(record, "inputs", path, issues)
                check_count_list(record, "outputs", path, issues)
                number_at_least(record, "work_seconds", 0, path, issues)
            elif name == "buildings":
                check_count_list(record, "cost", path, issues)
            elif name == "npc_schedules":
                if record.get("faction_id") not in factions:
                    add_issue(issues, "error", path, f"{record.get('id', '<unknown>')} 引用不存在的 faction_id")
                if not isinstance(record.get("entries"), list) or not record.get("entries"):
                    add_issue(issues, "error", path, f"{record.get('id', '<unknown>')} 的 entries 不可為空")
            elif name == "legal_rules":
                if record.get("faction_id") not in factions:
                    add_issue(issues, "error", path, f"{record.get('id', '<unknown>')} 引用不存在的 faction_id")
                for rule in record.get("rules", []):
                    if isinstance(rule, dict):
                        value = rule.get("memory_seconds")
                        if value is not None and (not isinstance(value, (int, float)) or value < 0):
                            add_issue(issues, "error", path, f"{record.get('id', '<unknown>')} 的 memory_seconds 不可小於 0")

        if name == "recipes":
            detect_recipe_cycles(records, path, issues)

    write_reports(files, issues, warnings)
    return 1 if any(issue["severity"] == "error" for issue in issues) else 0


if __name__ == "__main__":
    sys.exit(main())
