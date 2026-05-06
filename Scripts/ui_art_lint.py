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
REPORT_MD = REPORT_DIR / "ui-art-lint.md"
REPORT_JSON = REPORT_DIR / "ui-art-lint.json"

UI_ASSIGNMENTS = ART_DATA / "UIArtAssignments.json"
FACTION_ASSIGNMENTS = ART_DATA / "FactionArtAssignments.json"
PORTRAIT_ASSIGNMENTS = ART_DATA / "CharacterPortraitAssignments.json"
ITEMS = CONTENT_DATA / "items.json"
FACTIONS = CONTENT_DATA / "factions.json"
MANIFEST = ART_DATA / "ArtGenManifest.json"
MATERIAL_INSTANCES = ART_DATA / "GeneratedMaterialInstances.json"

REQUIRED_RECRUIT_IDS = {
    "recruit_mender_luhe",
    "recruit_scout_jianan",
    "recruit_trader_moze",
    "recruit_guard_luochui",
}
REQUIRED_UI_TARGETS = {
    "inventory_ui",
    "trade_ui",
}
EXPECTED_FACTIONS = {
    "faction_saltwardens",
    "faction_glasshouse",
    "faction_dustrunners",
}


def load_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding="utf-8"))


def records(path: Path) -> list[dict[str, Any]]:
    payload = load_json(path)
    if not isinstance(payload, dict):
        return []
    value = payload.get("records")
    return [item for item in value if isinstance(item, dict)] if isinstance(value, list) else []


def ue_path_to_uasset(path_value: str) -> Path:
    if not path_value.startswith("/Game/"):
        return ROOT / "__invalid__"
    rel = path_value.removeprefix("/Game/")
    return ROOT / "Content" / f"{rel}.uasset"


def add_issue(issues: list[dict[str, str]], severity: str, path: Path, message: str) -> None:
    issues.append({"severity": severity, "file": str(path.relative_to(ROOT)), "message": message})


def check_texture_path(
    issues: list[dict[str, str]],
    path: Path,
    record_id: str,
    key: str,
    value: Any,
    expected_prefix: str,
    known_primary_textures: set[str],
) -> None:
    if not isinstance(value, str) or not value.strip():
        add_issue(issues, "error", path, f"{record_id} 缺少 {key}")
        return
    if not value.startswith(expected_prefix):
        add_issue(issues, "error", path, f"{record_id} 的 {key} 不在 {expected_prefix}：{value}")
    if value not in known_primary_textures:
        add_issue(issues, "error", path, f"{record_id} 的 {key} 未登錄於 GeneratedMaterialInstances.primary_texture：{value}")
    if not ue_path_to_uasset(value).exists():
        add_issue(issues, "error", path, f"{record_id} 的 {key} 缺少 UE texture asset：{value}")


def check_material_path(
    issues: list[dict[str, str]],
    path: Path,
    record_id: str,
    key: str,
    value: Any,
    known_materials: set[str],
) -> None:
    if not isinstance(value, str) or not value.strip():
        add_issue(issues, "error", path, f"{record_id} 缺少 {key}")
        return
    if not value.startswith("/Game/GeneratedArt/Materials/"):
        add_issue(issues, "error", path, f"{record_id} 的 {key} 不在 /Game/GeneratedArt/Materials/：{value}")
    if value not in known_materials:
        add_issue(issues, "error", path, f"{record_id} 的 {key} 未登錄於 GeneratedMaterialInstances.material_instance：{value}")
    if not ue_path_to_uasset(value).exists():
        add_issue(issues, "error", path, f"{record_id} 的 {key} 缺少 UE material asset：{value}")


def main() -> int:
    issues: list[dict[str, str]] = []
    required_files = [UI_ASSIGNMENTS, FACTION_ASSIGNMENTS, PORTRAIT_ASSIGNMENTS, ITEMS, FACTIONS, MANIFEST, MATERIAL_INSTANCES]
    for path in required_files:
        if not path.exists():
            add_issue(issues, "error", path, "必要檔案不存在")

    if issues:
        write_report(issues, {}, {}, {})
        return 1

    item_records = records(ITEMS)
    faction_records = records(FACTIONS)
    ui_records = records(UI_ASSIGNMENTS)
    faction_art_records = records(FACTION_ASSIGNMENTS)
    portrait_records = records(PORTRAIT_ASSIGNMENTS)
    manifest_records = records(MANIFEST)
    material_records = records(MATERIAL_INSTANCES)

    item_ids = {str(record.get("id")) for record in item_records}
    faction_ids = {str(record.get("id")) for record in faction_records}
    manifest_by_asset = {str(record.get("asset_id")): record for record in manifest_records}
    known_primary_textures = {str(record.get("primary_texture")) for record in material_records if record.get("primary_texture")}
    known_materials = {str(record.get("material_instance")) for record in material_records if record.get("material_instance")}

    ui_by_item = {str(record.get("item_id")): record for record in ui_records}
    missing_item_icons = sorted(item_ids - set(ui_by_item.keys()))
    if missing_item_icons:
        add_issue(issues, "error", UI_ASSIGNMENTS, "缺少物品 icon assignment：" + ", ".join(missing_item_icons))
    extra_item_icons = sorted(set(ui_by_item.keys()) - item_ids)
    if extra_item_icons:
        add_issue(issues, "error", UI_ASSIGNMENTS, "assignment 指向不存在物品：" + ", ".join(extra_item_icons))

    for item_id, record in sorted(ui_by_item.items()):
        asset_id = str(record.get("asset_id", ""))
        manifest_record = manifest_by_asset.get(asset_id)
        if not manifest_record or manifest_record.get("category") != "ui_icon":
            add_issue(issues, "error", UI_ASSIGNMENTS, f"{item_id} asset_id 不是 ui_icon manifest record：{asset_id}")
        targets = record.get("ui_targets")
        target_set = set(targets) if isinstance(targets, list) else set()
        missing_targets = REQUIRED_UI_TARGETS - target_set
        if missing_targets:
            add_issue(issues, "error", UI_ASSIGNMENTS, f"{item_id} 缺少 UI targets：{', '.join(sorted(missing_targets))}")
        if record.get("validation_status") != "assigned":
            add_issue(issues, "error", UI_ASSIGNMENTS, f"{item_id} validation_status 必須是 assigned")
        check_texture_path(issues, UI_ASSIGNMENTS, item_id, "icon_texture_path", record.get("icon_texture_path"), "/Game/GeneratedArt/UI/", known_primary_textures)
        check_material_path(issues, UI_ASSIGNMENTS, item_id, "icon_material_path", record.get("icon_material_path"), known_materials)

    faction_art_by_id = {str(record.get("faction_id")): record for record in faction_art_records}
    missing_faction_art = sorted((faction_ids & EXPECTED_FACTIONS) - set(faction_art_by_id.keys()))
    if missing_faction_art:
        add_issue(issues, "error", FACTION_ASSIGNMENTS, "缺少派系 emblem assignment：" + ", ".join(missing_faction_art))
    for faction_id, record in sorted(faction_art_by_id.items()):
        if faction_id not in faction_ids:
            add_issue(issues, "error", FACTION_ASSIGNMENTS, f"assignment 指向不存在派系：{faction_id}")
        asset_id = str(record.get("asset_id", ""))
        banner_asset_id = str(record.get("banner_asset_id", ""))
        if manifest_by_asset.get(asset_id, {}).get("category") != "faction_emblem":
            add_issue(issues, "error", FACTION_ASSIGNMENTS, f"{faction_id} asset_id 不是 faction_emblem：{asset_id}")
        if manifest_by_asset.get(banner_asset_id, {}).get("category") != "banner":
            add_issue(issues, "error", FACTION_ASSIGNMENTS, f"{faction_id} banner_asset_id 不是 banner：{banner_asset_id}")
        if record.get("validation_status") != "assigned":
            add_issue(issues, "error", FACTION_ASSIGNMENTS, f"{faction_id} validation_status 必須是 assigned")
        check_texture_path(issues, FACTION_ASSIGNMENTS, faction_id, "emblem_texture_path", record.get("emblem_texture_path"), "/Game/GeneratedArt/Factions/", known_primary_textures)
        check_material_path(issues, FACTION_ASSIGNMENTS, faction_id, "emblem_material_path", record.get("emblem_material_path"), known_materials)
        check_texture_path(issues, FACTION_ASSIGNMENTS, faction_id, "banner_texture_path", record.get("banner_texture_path"), "/Game/GeneratedArt/Banners/", known_primary_textures)

    portrait_by_character = {str(record.get("character_id")): record for record in portrait_records}
    missing_portraits = sorted(REQUIRED_RECRUIT_IDS - set(portrait_by_character.keys()))
    if missing_portraits:
        add_issue(issues, "error", PORTRAIT_ASSIGNMENTS, "缺少 recruit portrait assignment：" + ", ".join(missing_portraits))
    for character_id, record in sorted(portrait_by_character.items()):
        asset_id = str(record.get("asset_id", ""))
        if manifest_by_asset.get(asset_id, {}).get("category") != "portrait":
            add_issue(issues, "error", PORTRAIT_ASSIGNMENTS, f"{character_id} asset_id 不是 portrait：{asset_id}")
        faction_id = str(record.get("faction_id", ""))
        if faction_id not in faction_ids:
            add_issue(issues, "error", PORTRAIT_ASSIGNMENTS, f"{character_id} faction_id 不存在：{faction_id}")
        targets = record.get("ui_targets")
        if not isinstance(targets, list) or "recruit_ui" not in targets:
            add_issue(issues, "error", PORTRAIT_ASSIGNMENTS, f"{character_id} 缺少 recruit_ui target")
        if record.get("validation_status") != "assigned":
            add_issue(issues, "error", PORTRAIT_ASSIGNMENTS, f"{character_id} validation_status 必須是 assigned")
        check_texture_path(issues, PORTRAIT_ASSIGNMENTS, character_id, "portrait_texture_path", record.get("portrait_texture_path"), "/Game/GeneratedArt/Characters/", known_primary_textures)
        check_material_path(issues, PORTRAIT_ASSIGNMENTS, character_id, "portrait_material_path", record.get("portrait_material_path"), known_materials)

    stats = {
        "item_icon_count": len(ui_by_item),
        "faction_emblem_count": len(faction_art_by_id),
        "portrait_count": len(portrait_by_character),
        "missing_icon_count": len(missing_item_icons),
        "missing_faction_emblem_count": len(missing_faction_art),
        "missing_portrait_count": len(missing_portraits),
    }
    write_report(issues, stats, ui_by_item, portrait_by_character)
    return 1 if any(issue["severity"] == "error" for issue in issues) else 0


def write_report(
    issues: list[dict[str, str]],
    stats: dict[str, int],
    ui_by_item: dict[str, dict[str, Any]],
    portrait_by_character: dict[str, dict[str, Any]],
) -> None:
    status = "pass" if not any(issue["severity"] == "error" for issue in issues) else "fail"
    REPORT_DIR.mkdir(parents=True, exist_ok=True)
    REPORT_JSON.write_text(
        json.dumps(
            {
                "status": status,
                "stats": stats,
                "issues": issues,
            },
            ensure_ascii=False,
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )

    lines = [
        "# UI Art Lint 報告",
        "",
        f"- 狀態：{'通過' if status == 'pass' else '失敗'}",
        f"- 物品 icon 數：{stats.get('item_icon_count', 0)}",
        f"- 派系 emblem 數：{stats.get('faction_emblem_count', 0)}",
        f"- 角色 portrait 數：{stats.get('portrait_count', 0)}",
        f"- missing icon count：{stats.get('missing_icon_count', 0)}",
        f"- missing faction emblem count：{stats.get('missing_faction_emblem_count', 0)}",
        f"- missing portrait count：{stats.get('missing_portrait_count', 0)}",
        f"- 問題數：{len(issues)}",
        "",
    ]
    if ui_by_item:
        lines.append("## 物品 UI icon")
        for item_id in sorted(ui_by_item):
            record = ui_by_item[item_id]
            lines.append(f"- `{item_id}`：`{record.get('icon_texture_path', '')}`")
        lines.append("")
    if portrait_by_character:
        lines.append("## 招募角色 portrait")
        for character_id in sorted(portrait_by_character):
            record = portrait_by_character[character_id]
            lines.append(f"- `{character_id}`：`{record.get('portrait_texture_path', '')}`")
        lines.append("")
    if issues:
        lines.append("## 問題")
        lines.extend(f"- [{issue['severity']}] {issue['file']}：{issue['message']}" for issue in issues)
        lines.append("")
    else:
        lines.append("## 結果")
        lines.append("- 12 種物品、3 個派系與 4 名可招募角色皆已接到生成美術 texture / material。")
        lines.append("")
    REPORT_MD.write_text("\n".join(lines), encoding="utf-8")


if __name__ == "__main__":
    sys.exit(main())
