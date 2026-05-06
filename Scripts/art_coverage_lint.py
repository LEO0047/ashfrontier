#!/usr/bin/env python3
from __future__ import annotations

import json
import sys
from collections import defaultdict
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
ART_DIR = ROOT / "Content" / "Data" / "Art"
MAPPING = ART_DIR / "ArtSlotMapping.json"
ASSIGNMENTS = ART_DIR / "EnvironmentArtAssignments.json"
MANIFEST = ART_DIR / "ArtGenManifest.json"
REPORT_DIR = ROOT / "Reports" / "Art"
REPORT_MD = REPORT_DIR / "art-coverage.md"
REPORT_JSON = REPORT_DIR / "art-coverage.json"

REQUIRED_SLOTS = {
    "ArtSlot_DesertGround",
    "ArtSlot_RustedMetal",
    "ArtSlot_SaltStoneWall",
    "ArtSlot_SunBleachedCloth",
    "ArtSlot_BoneCeramic",
    "ArtSlot_BlackenedWood",
    "ArtSlot_FactionBanner_01",
    "ArtSlot_FactionBanner_02",
    "ArtSlot_FactionBanner_03",
    "ArtSlot_CitySignage",
    "ArtSlot_OutpostSignage",
}

REQUIRED_COVERAGE = {
    "main_city": 0.70,
    "outpost": 0.60,
    "wilderness": 0.50,
}


def load_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding="utf-8"))


def ue_path_to_uasset(path: str) -> Path:
    if not path.startswith("/Game/"):
        return ROOT / "__invalid__"
    rel = path.removeprefix("/Game/")
    return ROOT / "Content" / f"{rel}.uasset"


def add_issue(issues: list[dict[str, str]], severity: str, file: Path, message: str) -> None:
    issues.append({"severity": severity, "file": str(file.relative_to(ROOT)), "message": message})


def main() -> int:
    issues: list[dict[str, str]] = []
    mapping_payload = load_json(MAPPING) if MAPPING.exists() else {"records": []}
    assignments_payload = load_json(ASSIGNMENTS) if ASSIGNMENTS.exists() else {"records": []}
    manifest_payload = load_json(MANIFEST) if MANIFEST.exists() else {"records": []}

    mapping_records = [record for record in mapping_payload.get("records", []) if isinstance(record, dict)]
    assignment_records = [record for record in assignments_payload.get("records", []) if isinstance(record, dict)]
    manifest_records = [record for record in manifest_payload.get("records", []) if isinstance(record, dict)]

    slots = {str(record.get("slot_id")) for record in mapping_records}
    missing_slots = sorted(REQUIRED_SLOTS - slots)
    if missing_slots:
        add_issue(issues, "error", MAPPING, "缺少必備 ArtSlot：" + ", ".join(missing_slots))

    for record in mapping_records:
        slot = str(record.get("slot_id", ""))
        material_path = str(record.get("material_instance_path", ""))
        if not slot.startswith("ArtSlot_"):
            add_issue(issues, "error", MAPPING, f"slot_id 格式不正確：{slot}")
        if not material_path.startswith("/Game/GeneratedArt/Materials/"):
            add_issue(issues, "error", MAPPING, f"{slot} material_instance_path 不在 GeneratedArt/Materials：{material_path}")
        elif not ue_path_to_uasset(material_path).exists():
            add_issue(issues, "error", MAPPING, f"{slot} material instance 不存在：{material_path}")

    assignment_slots = {str(record.get("art_slot")) for record in assignment_records}
    unknown_assignment_slots = sorted(slot for slot in assignment_slots if slot not in slots)
    if unknown_assignment_slots:
        add_issue(issues, "error", ASSIGNMENTS, "assignment 使用未知 ArtSlot：" + ", ".join(unknown_assignment_slots))

    coverage = defaultdict(float)
    decal_count = 0
    banner_factions: set[str] = set()
    actor_names: set[str] = set()
    for record in assignment_records:
        actor_name = str(record.get("actor_name", ""))
        if not actor_name:
            add_issue(issues, "error", ASSIGNMENTS, "assignment 缺少 actor_name")
        if actor_name in actor_names:
            add_issue(issues, "error", ASSIGNMENTS, f"actor_name 重複：{actor_name}")
        actor_names.add(actor_name)

        area = str(record.get("coverage_area", ""))
        weight = record.get("coverage_weight", 0)
        if isinstance(weight, (int, float)) and area:
            coverage[area] += float(weight)
        else:
            add_issue(issues, "error", ASSIGNMENTS, f"{actor_name} coverage_weight 不正確：{weight}")

        assignment_type = str(record.get("assignment_type", ""))
        if assignment_type in {"decal", "signage"}:
            decal_count += 1
        if assignment_type == "banner" and record.get("faction_id"):
            banner_factions.add(str(record.get("faction_id")))

    for area, minimum in REQUIRED_COVERAGE.items():
        if coverage[area] < minimum:
            add_issue(issues, "error", ASSIGNMENTS, f"{area} 覆蓋率不足：{coverage[area]:.2f} < {minimum:.2f}")

    if decal_count < 10:
        add_issue(issues, "error", ASSIGNMENTS, f"場景 decals/signage 少於 10：{decal_count}")

    expected_factions = {"faction_saltwardens", "faction_glasshouse", "faction_dustrunners"}
    missing_banner_factions = sorted(expected_factions - banner_factions)
    if missing_banner_factions:
        add_issue(issues, "error", ASSIGNMENTS, "缺少派系場景旗幟：" + ", ".join(missing_banner_factions))

    manifest_statuses = {str(record.get("validation_status")) for record in manifest_records}
    if not manifest_statuses <= {"imported", "assigned"}:
        add_issue(issues, "error", MANIFEST, "manifest 含未匯入或未套用狀態：" + ", ".join(sorted(manifest_statuses)))

    missing_imports = [
        str(record.get("asset_id"))
        for record in manifest_records
        if str(record.get("ue_import_path", "")).startswith("/Game/GeneratedArt/")
        and not ue_path_to_uasset(str(record.get("ue_import_path"))).exists()
    ]
    if missing_imports:
        add_issue(issues, "error", MANIFEST, "manifest UE import path 缺失：" + ", ".join(missing_imports[:20]))

    status = "pass" if not any(issue["severity"] == "error" for issue in issues) else "fail"
    REPORT_DIR.mkdir(parents=True, exist_ok=True)
    REPORT_JSON.write_text(
        json.dumps(
            {
                "status": status,
                "coverage": dict(sorted(coverage.items())),
                "decal_count": decal_count,
                "banner_factions": sorted(banner_factions),
                "assignments": len(assignment_records),
                "issues": issues,
            },
            ensure_ascii=False,
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )

    lines = [
        "# Art Coverage Lint 報告",
        "",
        f"- 狀態：{'通過' if status == 'pass' else '失敗'}",
        f"- Assignment 數：{len(assignment_records)}",
        f"- Decal / signage 數：{decal_count}",
        f"- 派系旗幟覆蓋：{', '.join(sorted(banner_factions))}",
        f"- 問題數：{len(issues)}",
        "",
        "## 覆蓋率",
        "",
    ]
    for area, value in sorted(coverage.items()):
        lines.append(f"- `{area}`：{value:.2f}")
    lines.append("")
    if issues:
        lines.append("## 問題")
        lines.extend(f"- [{issue['severity']}] {issue['file']}：{issue['message']}" for issue in issues)
        lines.append("")
    else:
        lines.append("## 結果")
        lines.append("- ArtSlot、場景 assignment、派系旗幟、decals/signage 與 UE import path 目前通過。")
        lines.append("")
    REPORT_MD.write_text("\n".join(lines), encoding="utf-8")
    return 1 if status == "fail" else 0


if __name__ == "__main__":
    sys.exit(main())
