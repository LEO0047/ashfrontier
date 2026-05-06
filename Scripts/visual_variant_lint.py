#!/usr/bin/env python3
from __future__ import annotations

import json
import sys
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
CONTENT_DATA = ROOT / "Content" / "Data"
ART_DATA = CONTENT_DATA / "Art"
REPORT_DIR = ROOT / "Reports" / "Art"
REPORT_MD = REPORT_DIR / "visual-variant-lint.md"
REPORT_JSON = REPORT_DIR / "visual-variant-lint.json"

CHARACTER_VARIANTS = ART_DATA / "CharacterVisualVariants.json"
BUILDING_VARIANTS = ART_DATA / "BuildingVisualVariants.json"
EQUIPMENT_ASSIGNMENTS = ART_DATA / "EquipmentVisualAssignments.json"
BUILDINGS = CONTENT_DATA / "buildings.json"
ITEMS = CONTENT_DATA / "items.json"
FACTIONS = CONTENT_DATA / "factions.json"
MATERIAL_INSTANCES = ART_DATA / "GeneratedMaterialInstances.json"
UI_ASSIGNMENTS = ART_DATA / "UIArtAssignments.json"
ART_SLOT_MAPPING = ART_DATA / "ArtSlotMapping.json"

EXPECTED_FACTIONS = {
    "faction_saltwardens",
    "faction_glasshouse",
    "faction_dustrunners",
}
EXPECTED_RECRUITS = {
    "recruit_mender_luhe",
    "recruit_scout_jianan",
    "recruit_trader_moze",
    "recruit_guard_luochui",
}
EXPECTED_EQUIPMENT_ITEMS = {
    "item_rustblade",
    "item_quilted_vest",
    "item_field_bandage",
    "item_medic_wrap",
    "item_building_plate",
}


def load_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding="utf-8"))


def records(path: Path) -> list[dict[str, Any]]:
    payload = load_json(path)
    if not isinstance(payload, dict):
        return []
    value = payload.get("records")
    return [item for item in value if isinstance(item, dict)] if isinstance(value, list) else []


def add_issue(issues: list[dict[str, str]], severity: str, path: Path, message: str) -> None:
    issues.append({"severity": severity, "file": str(path.relative_to(ROOT)), "message": message})


def ue_path_to_uasset(path_value: str) -> Path:
    if not path_value.startswith("/Game/"):
        return ROOT / "__invalid__"
    rel = path_value.removeprefix("/Game/")
    return ROOT / "Content" / f"{rel}.uasset"


def check_material(
    issues: list[dict[str, str]],
    path: Path,
    record_id: str,
    value: Any,
    known_materials: set[str],
) -> None:
    if not isinstance(value, str) or not value.strip():
        add_issue(issues, "error", path, f"{record_id} 缺少 body_material_path")
        return
    if value not in known_materials:
        add_issue(issues, "error", path, f"{record_id} body_material_path 未登錄於 GeneratedMaterialInstances：{value}")
    if not ue_path_to_uasset(value).exists():
        add_issue(issues, "error", path, f"{record_id} body_material_path 缺少 UE material asset：{value}")


def check_texture(
    issues: list[dict[str, str]],
    path: Path,
    record_id: str,
    key: str,
    value: Any,
    known_textures: set[str],
) -> None:
    if not isinstance(value, str) or not value.strip():
        add_issue(issues, "error", path, f"{record_id} 缺少 {key}")
        return
    if value not in known_textures:
        add_issue(issues, "error", path, f"{record_id} {key} 未登錄於 GeneratedMaterialInstances.primary_texture：{value}")
    if not ue_path_to_uasset(value).exists():
        add_issue(issues, "error", path, f"{record_id} {key} 缺少 UE texture asset：{value}")


def main() -> int:
    issues: list[dict[str, str]] = []
    required_files = [
        CHARACTER_VARIANTS,
        BUILDING_VARIANTS,
        EQUIPMENT_ASSIGNMENTS,
        BUILDINGS,
        ITEMS,
        FACTIONS,
        MATERIAL_INSTANCES,
        UI_ASSIGNMENTS,
        ART_SLOT_MAPPING,
    ]
    for path in required_files:
        if not path.exists():
            add_issue(issues, "error", path, "必要檔案不存在")

    if issues:
        write_report(issues, {})
        return 1

    character_records = records(CHARACTER_VARIANTS)
    building_records = records(BUILDING_VARIANTS)
    equipment_records = records(EQUIPMENT_ASSIGNMENTS)
    building_ids = {str(record.get("id")) for record in records(BUILDINGS)}
    item_ids = {str(record.get("id")) for record in records(ITEMS)}
    faction_ids = {str(record.get("id")) for record in records(FACTIONS)}
    material_records = records(MATERIAL_INSTANCES)
    known_materials = {str(record.get("material_instance")) for record in material_records if record.get("material_instance")}
    known_textures = {str(record.get("primary_texture")) for record in material_records if record.get("primary_texture")}
    ui_icon_by_item = {str(record.get("item_id")): record.get("icon_texture_path") for record in records(UI_ASSIGNMENTS)}
    art_slots = {str(record.get("slot_id")) for record in records(ART_SLOT_MAPPING)}

    faction_variant_coverage = {str(record.get("faction_id")) for record in character_records if str(record.get("faction_id")) in EXPECTED_FACTIONS}
    missing_factions = sorted(EXPECTED_FACTIONS - faction_variant_coverage)
    if missing_factions:
        add_issue(issues, "error", CHARACTER_VARIANTS, "缺少派系角色視覺變體：" + ", ".join(missing_factions))

    recruit_variant_coverage = {str(record.get("target_id")) for record in character_records if record.get("target_type") == "recruit"}
    missing_recruits = sorted(EXPECTED_RECRUITS - recruit_variant_coverage)
    if missing_recruits:
        add_issue(issues, "error", CHARACTER_VARIANTS, "缺少可招募角色視覺變體：" + ", ".join(missing_recruits))

    faction_materials: dict[str, set[str]] = {}
    for record in character_records:
        variant_id = str(record.get("variant_id", ""))
        faction_id = str(record.get("faction_id", ""))
        if faction_id not in faction_ids and faction_id != "faction_player_squad":
            add_issue(issues, "error", CHARACTER_VARIANTS, f"{variant_id} faction_id 不存在：{faction_id}")
        if record.get("validation_status") != "assigned":
            add_issue(issues, "error", CHARACTER_VARIANTS, f"{variant_id} validation_status 必須是 assigned")
        material_path = record.get("body_material_path")
        check_material(issues, CHARACTER_VARIANTS, variant_id, material_path, known_materials)
        if faction_id:
            faction_materials.setdefault(faction_id, set()).add(str(material_path))
        marker_path = record.get("marker_texture_path")
        if marker_path:
            check_texture(issues, CHARACTER_VARIANTS, variant_id, "marker_texture_path", marker_path, known_textures)

    faction_material_signatures = {next(iter(paths)) for faction, paths in faction_materials.items() if faction in EXPECTED_FACTIONS and paths}
    if len(faction_material_signatures) < 3:
        add_issue(issues, "error", CHARACTER_VARIANTS, "三個派系需要不同角色材質語言")

    building_by_id = {str(record.get("building_id")): record for record in building_records}
    missing_buildings = sorted(building_ids - set(building_by_id.keys()))
    if missing_buildings:
        add_issue(issues, "error", BUILDING_VARIANTS, "缺少建築視覺變體：" + ", ".join(missing_buildings))
    for building_id, record in sorted(building_by_id.items()):
        if building_id not in building_ids:
            add_issue(issues, "error", BUILDING_VARIANTS, f"變體指向不存在建築：{building_id}")
        if record.get("validation_status") != "assigned":
            add_issue(issues, "error", BUILDING_VARIANTS, f"{building_id} validation_status 必須是 assigned")
        check_material(issues, BUILDING_VARIANTS, building_id, record.get("body_material_path"), known_materials)
        art_slot = str(record.get("art_slot", ""))
        signage_art_slot = str(record.get("signage_art_slot", ""))
        if art_slot not in art_slots:
            add_issue(issues, "error", BUILDING_VARIANTS, f"{building_id} art_slot 不存在：{art_slot}")
        if signage_art_slot not in art_slots:
            add_issue(issues, "error", BUILDING_VARIANTS, f"{building_id} signage_art_slot 不存在：{signage_art_slot}")

    equipment_by_item = {str(record.get("item_id")): record for record in equipment_records}
    missing_equipment = sorted(EXPECTED_EQUIPMENT_ITEMS - set(equipment_by_item.keys()))
    if missing_equipment:
        add_issue(issues, "error", EQUIPMENT_ASSIGNMENTS, "缺少裝備 icon mapping：" + ", ".join(missing_equipment))
    for item_id, record in sorted(equipment_by_item.items()):
        if item_id not in item_ids:
            add_issue(issues, "error", EQUIPMENT_ASSIGNMENTS, f"裝備 mapping 指向不存在物品：{item_id}")
        if record.get("validation_status") != "assigned":
            add_issue(issues, "error", EQUIPMENT_ASSIGNMENTS, f"{item_id} validation_status 必須是 assigned")
        icon_path = record.get("icon_texture_path")
        if ui_icon_by_item.get(item_id) != icon_path:
            add_issue(issues, "error", EQUIPMENT_ASSIGNMENTS, f"{item_id} icon_texture_path 未對齊 UIArtAssignments")
        check_texture(issues, EQUIPMENT_ASSIGNMENTS, item_id, "icon_texture_path", icon_path, known_textures)

    stats = {
        "character_variant_count": len(character_records),
        "covered_faction_count": len(faction_variant_coverage & EXPECTED_FACTIONS),
        "covered_recruit_count": len(recruit_variant_coverage & EXPECTED_RECRUITS),
        "building_variant_count": len(building_by_id),
        "equipment_mapping_count": len(equipment_by_item),
        "missing_building_count": len(missing_buildings),
        "missing_recruit_count": len(missing_recruits),
    }
    write_report(issues, stats)
    return 1 if any(issue["severity"] == "error" for issue in issues) else 0


def write_report(issues: list[dict[str, str]], stats: dict[str, int]) -> None:
    status = "pass" if not any(issue["severity"] == "error" for issue in issues) else "fail"
    REPORT_DIR.mkdir(parents=True, exist_ok=True)
    REPORT_JSON.write_text(
        json.dumps({"status": status, "stats": stats, "issues": issues}, ensure_ascii=False, indent=2) + "\n",
        encoding="utf-8",
    )

    lines = [
        "# Visual Variant Lint 報告",
        "",
        f"- 狀態：{'通過' if status == 'pass' else '失敗'}",
        f"- 角色 variant 數：{stats.get('character_variant_count', 0)}",
        f"- 派系角色覆蓋數：{stats.get('covered_faction_count', 0)}",
        f"- 可招募角色覆蓋數：{stats.get('covered_recruit_count', 0)}",
        f"- 建築 variant 數：{stats.get('building_variant_count', 0)}",
        f"- 裝備 mapping 數：{stats.get('equipment_mapping_count', 0)}",
        f"- missing building count：{stats.get('missing_building_count', 0)}",
        f"- missing recruit count：{stats.get('missing_recruit_count', 0)}",
        f"- 問題數：{len(issues)}",
        "",
    ]
    if issues:
        lines.append("## 問題")
        lines.extend(f"- [{issue['severity']}] {issue['file']}：{issue['message']}" for issue in issues)
        lines.append("")
    else:
        lines.append("## 結果")
        lines.append("- 角色、三派系、四名可招募角色、五種建築與裝備 icon mapping 目前通過。")
        lines.append("")
    REPORT_MD.write_text("\n".join(lines), encoding="utf-8")


if __name__ == "__main__":
    sys.exit(main())
