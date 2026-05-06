#!/usr/bin/env python3
from __future__ import annotations

import json
import re
import sys
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
MANIFEST = ROOT / "Content" / "Data" / "Art" / "ArtGenManifest.json"
METADATA_DIR = ROOT / "SourceArt" / "Generated" / "Metadata"
REPORT_DIR = ROOT / "Reports" / "Art"
REPORT_MD = REPORT_DIR / "art-manifest-lint.md"
REPORT_JSON = REPORT_DIR / "art-manifest-lint.json"
ASSET_ID_RE = re.compile(r"^af_v02_[a-z0-9_]+$")
ALLOWED_CATEGORIES = {
    "concept",
    "surface_texture",
    "ui_icon",
    "faction_emblem",
    "portrait",
    "decal_signage",
    "banner",
}
ALLOWED_STATUS = {"planned", "generated", "processed", "imported", "assigned", "blocked"}
REQUIRED_FIELDS = {
    "asset_id",
    "category",
    "prompt",
    "source_image_path",
    "generated_image_path",
    "ue_import_path",
    "assigned_targets",
    "style_tags",
    "validation_status",
}
GENERATED_STATUSES = {"generated", "processed", "imported", "assigned"}
METADATA_REQUIRED_FIELDS = {
    "asset_id",
    "category",
    "prompt",
    "negative_prompt",
    "generation_tool",
    "timestamp",
    "selected_reason",
    "output_path",
    "ue_import_path",
    "usage_target",
}


def add_issue(issues: list[dict[str, str]], severity: str, path: Path, message: str) -> None:
    issues.append({"severity": severity, "file": str(path.relative_to(ROOT)), "message": message})


def load_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding="utf-8"))


def project_path_exists(value: Any) -> bool:
    if not isinstance(value, str) or not value.strip():
        return False
    return (ROOT / value).exists()


def check_record(record: Any, index: int, issues: list[dict[str, str]]) -> str | None:
    label = f"records[{index}]"
    if not isinstance(record, dict):
        add_issue(issues, "error", MANIFEST, f"{label} 必須是物件")
        return None

    missing = sorted(REQUIRED_FIELDS - set(record.keys()))
    if missing:
        add_issue(issues, "error", MANIFEST, f"{label} 缺少欄位：{', '.join(missing)}")

    asset_id = record.get("asset_id")
    if not isinstance(asset_id, str) or not ASSET_ID_RE.match(asset_id):
        add_issue(issues, "error", MANIFEST, f"{label} asset_id 必須符合 af_v02_* 小寫命名：{asset_id}")
        return None

    category = record.get("category")
    if category not in ALLOWED_CATEGORIES:
        add_issue(issues, "error", MANIFEST, f"{asset_id} category 不在允許清單：{category}")

    prompt = record.get("prompt")
    if not isinstance(prompt, str) or len(prompt.strip()) < 20:
        add_issue(issues, "error", MANIFEST, f"{asset_id} prompt 太短或不存在")

    ue_import_path = record.get("ue_import_path")
    if not isinstance(ue_import_path, str) or not ue_import_path.startswith("/Game/GeneratedArt/"):
        add_issue(issues, "error", MANIFEST, f"{asset_id} ue_import_path 必須位於 /Game/GeneratedArt/：{ue_import_path}")

    for list_key in ("assigned_targets", "style_tags"):
        value = record.get(list_key)
        if not isinstance(value, list) or not value:
            add_issue(issues, "error", MANIFEST, f"{asset_id} {list_key} 必須是非空陣列")
        elif not all(isinstance(item, str) and item.strip() for item in value):
            add_issue(issues, "error", MANIFEST, f"{asset_id} {list_key} 只能包含非空字串")

    faction_id = record.get("faction_id")
    if faction_id is not None and (not isinstance(faction_id, str) or not faction_id.startswith("faction_")):
        add_issue(issues, "error", MANIFEST, f"{asset_id} faction_id 格式不正確：{faction_id}")

    status = record.get("validation_status")
    if status not in ALLOWED_STATUS:
        add_issue(issues, "error", MANIFEST, f"{asset_id} validation_status 不在允許清單：{status}")
    elif status in GENERATED_STATUSES:
        for key in ("generated_image_path",):
            if not project_path_exists(record.get(key)):
                add_issue(issues, "error", MANIFEST, f"{asset_id} {key} 指向不存在檔案：{record.get(key)}")

    return asset_id


def check_metadata(asset_ids: set[str], issues: list[dict[str, str]]) -> None:
    if not METADATA_DIR.exists():
        return
    for path in sorted(METADATA_DIR.glob("*.json")):
        try:
            payload = load_json(path)
        except Exception as exc:
            add_issue(issues, "error", path, f"metadata JSON 解析失敗：{exc}")
            continue
        if not isinstance(payload, dict):
            add_issue(issues, "error", path, "metadata 必須是物件")
            continue
        missing = sorted(METADATA_REQUIRED_FIELDS - set(payload.keys()))
        if missing:
            add_issue(issues, "error", path, f"metadata 缺少欄位：{', '.join(missing)}")
        asset_id = payload.get("asset_id")
        if asset_id not in asset_ids:
            add_issue(issues, "error", path, f"metadata asset_id 未登錄於 manifest：{asset_id}")
        output_path = payload.get("output_path")
        if output_path and not project_path_exists(output_path):
            add_issue(issues, "error", path, f"metadata output_path 指向不存在檔案：{output_path}")


def write_report(records_count: int, issues: list[dict[str, str]]) -> None:
    REPORT_DIR.mkdir(parents=True, exist_ok=True)
    status = "pass" if not any(issue["severity"] == "error" for issue in issues) else "fail"
    REPORT_JSON.write_text(
        json.dumps({"status": status, "records": records_count, "issues": issues}, ensure_ascii=False, indent=2) + "\n",
        encoding="utf-8",
    )
    lines = [
        "# Art Manifest Lint 報告",
        "",
        f"- 狀態：{'通過' if status == 'pass' else '失敗'}",
        f"- Manifest records：{records_count}",
        f"- 問題數：{len(issues)}",
        "",
    ]
    if issues:
        lines.append("## 問題")
        lines.extend(f"- [{issue['severity']}] {issue['file']}：{issue['message']}" for issue in issues)
        lines.append("")
    else:
        lines.append("## 結果")
        lines.append("- Manifest schema、metadata 對應與已生成檔案引用目前通過。")
        lines.append("")
    REPORT_MD.write_text("\n".join(lines), encoding="utf-8")


def main() -> int:
    issues: list[dict[str, str]] = []
    records_count = 0

    if not MANIFEST.exists():
        add_issue(issues, "error", MANIFEST, "缺少 ArtGenManifest.json")
        write_report(records_count, issues)
        return 1

    try:
        payload = load_json(MANIFEST)
    except Exception as exc:
        add_issue(issues, "error", MANIFEST, f"JSON 解析失敗：{exc}")
        write_report(records_count, issues)
        return 1

    if not isinstance(payload, dict):
        add_issue(issues, "error", MANIFEST, "manifest 根節點必須是物件")
        write_report(records_count, issues)
        return 1

    records = payload.get("records")
    if not isinstance(records, list):
        add_issue(issues, "error", MANIFEST, "manifest 缺少 records 陣列")
        records = []

    records_count = len(records)
    asset_ids: set[str] = set()
    for index, record in enumerate(records):
        asset_id = check_record(record, index, issues)
        if not asset_id:
            continue
        if asset_id in asset_ids:
            add_issue(issues, "error", MANIFEST, f"asset_id 重複：{asset_id}")
        asset_ids.add(asset_id)

    check_metadata(asset_ids, issues)
    write_report(records_count, issues)
    return 1 if any(issue["severity"] == "error" for issue in issues) else 0


if __name__ == "__main__":
    sys.exit(main())
