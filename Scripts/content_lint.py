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
REQUIRED_DATASET_COUNTS = {
    "factions": 3,
    "items": 12,
    "resources": 5,
    "recipes": 2,
    "buildings": 5,
    "npc_schedules": 3,
    "legal_rules": 1,
    "dialogue_conditions": 1,
}
ALLOWED_ITEM_CATEGORIES = {"currency", "weapon", "armor", "consumable", "material", "trade_good", "tool"}
ALLOWED_BUILDING_CATEGORIES = {"production", "storage", "gathering", "defense"}
ALLOWED_LEGAL_EVENTS = {"theft", "attack", "self_defense", "restricted_area"}
ALLOWED_LEGAL_REACTIONS = {"allow", "warning", "hostile", "pursuit"}
ALLOWED_DIALOGUE_CONDITIONS = {"relation_at_least", "has_item_count", "no_recent_crime"}


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


def require_string(record: dict[str, Any], key: str, path: Path, issues: list[dict[str, str]]) -> str | None:
    value = record.get(key)
    if not isinstance(value, str) or not value.strip():
        add_issue(issues, "error", path, f"{record.get('id', '<unknown>')} 缺少 {key}")
        return None
    return value


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


def check_count_references(
    record: dict[str, Any],
    key: str,
    valid_ids: set[str],
    path: Path,
    issues: list[dict[str, str]],
) -> None:
    value = record.get(key)
    if not isinstance(value, list):
        return
    for entry in value:
        if not isinstance(entry, dict):
            continue
        ref_id = entry.get("id")
        if isinstance(ref_id, str) and ref_id not in valid_ids:
            add_issue(issues, "error", path, f"{record.get('id', '<unknown>')} 的 {key} 引用不存在的 id：{ref_id}")


def check_schedule_entries(record: dict[str, Any], path: Path, issues: list[dict[str, str]]) -> None:
    entries = record.get("entries")
    if not isinstance(entries, list) or not entries:
        add_issue(issues, "error", path, f"{record.get('id', '<unknown>')} 的 entries 不可為空")
        return
    for index, entry in enumerate(entries):
        if not isinstance(entry, dict):
            add_issue(issues, "error", path, f"{record.get('id', '<unknown>')} 的 entries[{index}] 必須是物件")
            continue
        for key in ("time", "action", "target"):
            if not isinstance(entry.get(key), str) or not entry.get(key).strip():
                add_issue(issues, "error", path, f"{record.get('id', '<unknown>')} 的 entries[{index}] 缺少 {key}")


def add_minimum_dataset_issues(
    datasets: dict[str, list[dict[str, Any]]],
    path_by_dataset: dict[str, Path],
    issues: list[dict[str, str]],
) -> None:
    for name, minimum in REQUIRED_DATASET_COUNTS.items():
        count = len(datasets.get(name, []))
        if count < minimum:
            path = path_by_dataset.get(name, ROOT / "Content" / "Data" / f"{name}.json")
            add_issue(issues, "error", path, f"{name} 至少需要 {minimum} 筆資料，目前只有 {count} 筆")


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
    id_locations: dict[str, list[str]] = {}

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
        datasets.setdefault(name, []).extend(records)
        path_by_dataset[name] = path
        record_ids = check_common(records, path, issues)
        ids_by_dataset.setdefault(name, set()).update(record_ids)
        for record_id in record_ids:
            id_locations.setdefault(record_id, []).append(str(path.relative_to(ROOT)))

    add_minimum_dataset_issues(datasets, path_by_dataset, issues)
    for record_id, locations in sorted(id_locations.items()):
        if len(set(locations)) > 1:
            add_issue(issues, "error", ROOT / "Content" / "Data", f"id 跨資料集重複：{record_id} 出現在 {', '.join(sorted(set(locations)))}")

    factions = ids_by_dataset.get("factions", set())
    items = ids_by_dataset.get("items", set())
    resources = ids_by_dataset.get("resources", set())
    recipes = ids_by_dataset.get("recipes", set())
    buildings = ids_by_dataset.get("buildings", set())
    legal_rules = ids_by_dataset.get("legal_rules", set())
    item_or_resource_ids = items | resources
    all_content_refs = item_or_resource_ids | buildings | factions | recipes | legal_rules

    recipe_chains = {record.get("chain") for record in datasets.get("recipes", []) if isinstance(record.get("chain"), str)}
    for required_chain in ("food", "metal"):
        if required_chain not in recipe_chains:
            add_issue(issues, "error", path_by_dataset.get("recipes", ROOT / "Content" / "Data" / "recipes.json"), f"recipes 缺少 {required_chain} 生產鏈")

    for name, records in datasets.items():
        path = path_by_dataset[name]
        for record in records:
            if name == "factions":
                legal_profile_id = require_string(record, "legal_profile_id", path, issues)
                if legal_profile_id and legal_profile_id not in legal_rules:
                    add_issue(issues, "error", path, f"{record.get('id', '<unknown>')} 的 legal_profile_id 不存在：{legal_profile_id}")
                relations = record.get("starting_relation")
                if not isinstance(relations, dict):
                    add_issue(issues, "error", path, f"{record.get('id', '<unknown>')} 的 starting_relation 必須是物件")
                else:
                    for faction_id, value in relations.items():
                        if faction_id not in factions:
                            add_issue(issues, "error", path, f"{record.get('id', '<unknown>')} 的 starting_relation 引用不存在的 faction：{faction_id}")
                        if not isinstance(value, (int, float)):
                            add_issue(issues, "error", path, f"{record.get('id', '<unknown>')} 的 starting_relation 值必須是數字：{faction_id}")
            elif name == "items":
                category = require_string(record, "category", path, issues)
                if category and category not in ALLOWED_ITEM_CATEGORIES:
                    add_issue(issues, "error", path, f"{record.get('id', '<unknown>')} 的 category 不在允許清單：{category}")
                number_at_least(record, "base_price", 0, path, issues)
                number_at_least(record, "stack_size", 0, path, issues)
                number_at_least(record, "weight", 0, path, issues)
            elif name == "resources":
                number_at_least(record, "gather_time_seconds", 0, path, issues)
                produces_item_id = record.get("produces_item_id")
                if produces_item_id is not None and produces_item_id not in items:
                    add_issue(issues, "error", path, f"{record.get('id', '<unknown>')} 的 produces_item_id 不存在：{produces_item_id}")
            elif name == "recipes":
                check_count_list(record, "inputs", path, issues)
                check_count_list(record, "outputs", path, issues)
                check_count_references(record, "inputs", item_or_resource_ids, path, issues)
                check_count_references(record, "outputs", item_or_resource_ids, path, issues)
                number_at_least(record, "work_seconds", 0, path, issues)
                station_id = require_string(record, "station_id", path, issues)
                if station_id and station_id not in buildings:
                    add_issue(issues, "error", path, f"{record.get('id', '<unknown>')} 的 station_id 不存在：{station_id}")
                require_string(record, "chain", path, issues)
            elif name == "buildings":
                category = require_string(record, "category", path, issues)
                if category and category not in ALLOWED_BUILDING_CATEGORIES:
                    add_issue(issues, "error", path, f"{record.get('id', '<unknown>')} 的 category 不在允許清單：{category}")
                require_string(record, "allowed_zone", path, issues)
                check_count_list(record, "cost", path, issues)
                check_count_references(record, "cost", item_or_resource_ids, path, issues)
                resource_id = record.get("resource_id")
                if resource_id is not None and resource_id not in resources:
                    add_issue(issues, "error", path, f"{record.get('id', '<unknown>')} 的 resource_id 不存在：{resource_id}")
            elif name == "npc_schedules":
                if record.get("faction_id") not in factions:
                    add_issue(issues, "error", path, f"{record.get('id', '<unknown>')} 引用不存在的 faction_id")
                check_schedule_entries(record, path, issues)
            elif name == "legal_rules":
                if record.get("faction_id") not in factions:
                    add_issue(issues, "error", path, f"{record.get('id', '<unknown>')} 引用不存在的 faction_id")
                rules = record.get("rules")
                if not isinstance(rules, list) or not rules:
                    add_issue(issues, "error", path, f"{record.get('id', '<unknown>')} 的 rules 不可為空")
                    continue
                for rule in rules:
                    if isinstance(rule, dict):
                        event = rule.get("event")
                        reaction = rule.get("reaction")
                        if event not in ALLOWED_LEGAL_EVENTS:
                            add_issue(issues, "error", path, f"{record.get('id', '<unknown>')} 的 event 不在允許清單：{event}")
                        if reaction not in ALLOWED_LEGAL_REACTIONS:
                            add_issue(issues, "error", path, f"{record.get('id', '<unknown>')} 的 reaction 不在允許清單：{reaction}")
                        value = rule.get("memory_seconds")
                        if value is not None and (not isinstance(value, (int, float)) or value < 0):
                            add_issue(issues, "error", path, f"{record.get('id', '<unknown>')} 的 memory_seconds 不可小於 0")
                        relation_delta = rule.get("relation_delta")
                        if relation_delta is not None and not isinstance(relation_delta, (int, float)):
                            add_issue(issues, "error", path, f"{record.get('id', '<unknown>')} 的 relation_delta 必須是數字")
                    else:
                        add_issue(issues, "error", path, f"{record.get('id', '<unknown>')} 的 rules 包含非物件項目")
            elif name == "dialogue_conditions":
                conditions = record.get("conditions")
                if not isinstance(conditions, list) or not conditions:
                    add_issue(issues, "error", path, f"{record.get('id', '<unknown>')} 的 conditions 不可為空")
                    continue
                for condition in conditions:
                    if not isinstance(condition, dict):
                        add_issue(issues, "error", path, f"{record.get('id', '<unknown>')} 的 conditions 包含非物件項目")
                        continue
                    ctype = condition.get("type")
                    if ctype not in ALLOWED_DIALOGUE_CONDITIONS:
                        add_issue(issues, "error", path, f"{record.get('id', '<unknown>')} 的 condition type 不在允許清單：{ctype}")
                    faction_id = condition.get("faction_id")
                    if faction_id is not None and faction_id not in factions:
                        add_issue(issues, "error", path, f"{record.get('id', '<unknown>')} 的 condition faction_id 不存在：{faction_id}")
                    item_id = condition.get("item_id")
                    if item_id is not None and item_id not in items:
                        add_issue(issues, "error", path, f"{record.get('id', '<unknown>')} 的 condition item_id 不存在：{item_id}")
                    if "value" in condition and not isinstance(condition.get("value"), (int, float)):
                        add_issue(issues, "error", path, f"{record.get('id', '<unknown>')} 的 condition value 必須是數字")
            else:
                for key, value in record.items():
                    if key.endswith("_id") and isinstance(value, str) and value not in all_content_refs:
                        add_issue(issues, "warning", path, f"{record.get('id', '<unknown>')} 的 {key} 尚未能驗證引用：{value}")

        if name == "recipes":
            detect_recipe_cycles(records, path, issues)

    write_reports(files, issues, warnings)
    return 1 if any(issue["severity"] == "error" for issue in issues) else 0


if __name__ == "__main__":
    sys.exit(main())
