#!/usr/bin/env python3
from __future__ import annotations

import argparse
import json
import shutil
from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from PIL import Image, ImageDraw, ImageFont, ImageOps


ROOT = Path(__file__).resolve().parents[1]
MANIFEST = ROOT / "Content" / "Data" / "Art" / "ArtGenManifest.json"
METADATA_DIR = ROOT / "SourceArt" / "Generated" / "Metadata"
REPORT_DIR = ROOT / "Reports" / "Art"
INVENTORY_MD = REPORT_DIR / "generated-assets-inventory.md"
INVENTORY_JSON = REPORT_DIR / "generated-assets-inventory.json"


@dataclass(frozen=True)
class SheetSpec:
    key: str
    workspace_path: Path
    columns: int
    rows: int


@dataclass(frozen=True)
class AssetSlice:
    asset_id: str
    sheet_key: str
    index: int
    output_path: Path
    target_size: int
    selected_reason: str
    overlay_text: str | None = None


SPECS: list[AssetSlice] = [
    AssetSlice("af_v02_concept_city_gate", "concept", 0, Path("SourceArt/Generated/Concepts/af_v02_concept_city_gate.png"), 1024, "主城入口概念圖有清楚城門、旗幟、砂岩與告示套用位置。"),
    AssetSlice("af_v02_concept_outpost", "concept", 1, Path("SourceArt/Generated/Concepts/af_v02_concept_outpost.png"), 1024, "前哨概念圖有粗木、金屬修補與旗幟告示可用位置。"),
    AssetSlice("af_v02_concept_wild_build_zone", "concept", 2, Path("SourceArt/Generated/Concepts/af_v02_concept_wild_build_zone.png"), 1024, "野外建造區概念圖有地表、工坊與危險標記方向。"),
    AssetSlice("af_v02_surface_cracked_sand", "surface", 0, Path("SourceArt/Generated/Textures/af_v02_surface_cracked_sand.png"), 1024, "乾裂沙地細節均勻，適合作為 pseudo-tileable 地表來源。"),
    AssetSlice("af_v02_surface_salt_stone", "surface", 1, Path("SourceArt/Generated/Textures/af_v02_surface_salt_stone.png"), 1024, "鹽蝕砂岩有灰白鹽痕與粗糙孔洞，可支援牆體材質。"),
    AssetSlice("af_v02_surface_rusted_metal", "surface", 2, Path("SourceArt/Generated/Textures/af_v02_surface_rusted_metal.png"), 1024, "生鏽金屬有暗鐵與氧化刮痕，符合低科技工業語言。"),
    AssetSlice("af_v02_surface_sun_bleached_cloth", "surface", 3, Path("SourceArt/Generated/Textures/af_v02_surface_sun_bleached_cloth.png"), 1024, "日曬舊布有粗織與砂塵，可用於旗幟與棚布。"),
    AssetSlice("af_v02_surface_blackened_wood", "surface", 4, Path("SourceArt/Generated/Textures/af_v02_surface_blackened_wood.png"), 1024, "焦黑木材有裂紋與炭化邊，適合前哨與建築變體。"),
    AssetSlice("af_v02_surface_bone_ceramic", "surface", 5, Path("SourceArt/Generated/Textures/af_v02_surface_bone_ceramic.png"), 1024, "骨質陶片保留手工拼接感，可做荒野裝飾材質。"),
    AssetSlice("af_v02_surface_industrial_floor", "surface", 6, Path("SourceArt/Generated/Textures/af_v02_surface_industrial_floor.png"), 1024, "粗糙工業地板有拼裝鐵板與砂土卡縫，適合商店區。"),
    AssetSlice("af_v02_surface_weathered_wall", "surface", 7, Path("SourceArt/Generated/Textures/af_v02_surface_weathered_wall.png"), 1024, "風化城牆有灰泥、補片與鹽霜垂痕，可替換灰盒牆面。"),
    AssetSlice("af_v02_icon_ash_credit", "ui_icon", 0, Path("SourceArt/Generated/UIIcons/af_v02_icon_ash_credit.png"), 512, "灰印幣輪廓清楚，小尺寸可讀。"),
    AssetSlice("af_v02_icon_cooked_ration", "ui_icon", 1, Path("SourceArt/Generated/UIIcons/af_v02_icon_cooked_ration.png"), 512, "烤行糧剪影清楚，能辨識為食物。"),
    AssetSlice("af_v02_icon_raw_grain", "ui_icon", 2, Path("SourceArt/Generated/UIIcons/af_v02_icon_raw_grain.png"), 512, "乾穀袋有粗布與顆粒辨識。"),
    AssetSlice("af_v02_icon_water_flask", "ui_icon", 3, Path("SourceArt/Generated/UIIcons/af_v02_icon_water_flask.png"), 512, "鹹水壺有金屬與鹽痕，可讀性佳。"),
    AssetSlice("af_v02_icon_scrap_ore", "ui_icon", 4, Path("SourceArt/Generated/UIIcons/af_v02_icon_scrap_ore.png"), 512, "碎鐵礦袋可辨識為礦石材料。"),
    AssetSlice("af_v02_icon_crude_ingot", "ui_icon", 5, Path("SourceArt/Generated/UIIcons/af_v02_icon_crude_ingot.png"), 512, "粗鐵錠形狀單純，適合 inventory。"),
    AssetSlice("af_v02_icon_salvaged_plank", "ui_icon", 6, Path("SourceArt/Generated/UIIcons/af_v02_icon_salvaged_plank.png"), 512, "回收木板有焦黑裂紋，與金屬材料區分。"),
    AssetSlice("af_v02_icon_building_plate", "ui_icon", 7, Path("SourceArt/Generated/UIIcons/af_v02_icon_building_plate.png"), 512, "拼裝建材板有鉚釘與木金屬混合特徵。"),
    AssetSlice("af_v02_icon_field_bandage", "ui_icon", 8, Path("SourceArt/Generated/UIIcons/af_v02_icon_field_bandage.png"), 512, "野外繃帶剪影明確，能區分醫療用品。"),
    AssetSlice("af_v02_icon_medic_wrap", "ui_icon", 9, Path("SourceArt/Generated/UIIcons/af_v02_icon_medic_wrap.png"), 512, "草藥壓布有布包與草藥色點綴。"),
    AssetSlice("af_v02_icon_rustblade", "ui_icon", 10, Path("SourceArt/Generated/UIIcons/af_v02_icon_rustblade.png"), 512, "鏽刃短刀斜向剪影清楚。"),
    AssetSlice("af_v02_icon_quilted_vest", "ui_icon", 11, Path("SourceArt/Generated/UIIcons/af_v02_icon_quilted_vest.png"), 512, "絎縫護背正面輪廓可讀。"),
    AssetSlice("af_v02_emblem_saltwardens", "emblem", 0, Path("SourceArt/Generated/FactionEmblems/af_v02_emblem_saltwardens.png"), 1024, "鹽脊守望徽章有直線與矛尖形狀，遠距離可讀。"),
    AssetSlice("af_v02_emblem_glasshouse", "emblem", 1, Path("SourceArt/Generated/FactionEmblems/af_v02_emblem_glasshouse.png"), 1024, "玻璃屋商盟徽章有圓弧與貨牌語言，與守望區分。"),
    AssetSlice("af_v02_emblem_dustrunners", "emblem", 2, Path("SourceArt/Generated/FactionEmblems/af_v02_emblem_dustrunners.png"), 1024, "塵路行者徽章有斜線裂片與繩結感，荒野辨識清楚。"),
    AssetSlice("af_v02_banner_saltwardens", "banner", 0, Path("SourceArt/Generated/Banners/af_v02_banner_saltwardens.png"), 1024, "鹽脊守望旗幟有日曬布與秩序色塊。"),
    AssetSlice("af_v02_banner_glasshouse", "banner", 1, Path("SourceArt/Generated/Banners/af_v02_banner_glasshouse.png"), 1024, "玻璃屋商盟旗幟有商貿色塊與粗布質感。"),
    AssetSlice("af_v02_banner_dustrunners", "banner", 2, Path("SourceArt/Generated/Banners/af_v02_banner_dustrunners.png"), 1024, "塵路行者旗幟有日曬紅布與荒野裂片輪廓。"),
    AssetSlice("af_v02_portrait_recruit_guard", "portrait", 0, Path("SourceArt/Generated/Portraits/af_v02_portrait_recruit_guard.png"), 1024, "守衛 portrait 有暗鐵肩片與冷靜表情。"),
    AssetSlice("af_v02_portrait_recruit_trader", "portrait", 1, Path("SourceArt/Generated/Portraits/af_v02_portrait_recruit_trader.png"), 1024, "修補商 portrait 有護目鏡與氧化銅飾件。"),
    AssetSlice("af_v02_portrait_recruit_scout", "portrait", 2, Path("SourceArt/Generated/Portraits/af_v02_portrait_recruit_scout.png"), 1024, "荒野斥候 portrait 有頭巾、砂塵與警覺感。"),
    AssetSlice("af_v02_portrait_recruit_mender", "portrait", 3, Path("SourceArt/Generated/Portraits/af_v02_portrait_recruit_mender.png"), 1024, "醫療修補者 portrait 有醫療布帶與草藥染色布。"),
    AssetSlice("af_v02_decal_shop_sign", "decal", 0, Path("SourceArt/Generated/Decals/af_v02_decal_shop_sign.png"), 1024, "商店招牌底圖乾燥磨損，已覆蓋可讀繁體中文。", "補給"),
    AssetSlice("af_v02_decal_restricted_warning", "decal", 1, Path("SourceArt/Generated/Decals/af_v02_decal_restricted_warning.png"), 1024, "禁區警告有暗鐵與鹽痕，已覆蓋可讀繁體中文。", "禁區"),
    AssetSlice("af_v02_decal_guard_notice", "decal", 2, Path("SourceArt/Generated/Decals/af_v02_decal_guard_notice.png"), 1024, "守衛告示有布底與護板感，已覆蓋可讀繁體中文。", "停步"),
    AssetSlice("af_v02_decal_broken_direction", "decal", 3, Path("SourceArt/Generated/Decals/af_v02_decal_broken_direction.png"), 1024, "破損方向牌有裂片木與繩結，已覆蓋可讀繁體中文。", "北門"),
    AssetSlice("af_v02_decal_workshop_mark", "decal", 4, Path("SourceArt/Generated/Decals/af_v02_decal_workshop_mark.png"), 1024, "工坊標誌有氧化金屬底，已覆蓋可讀繁體中文。", "工坊"),
    AssetSlice("af_v02_decal_warehouse_mark", "decal", 5, Path("SourceArt/Generated/Decals/af_v02_decal_warehouse_mark.png"), 1024, "倉庫標誌有粗布與貨牌感，已覆蓋可讀繁體中文。", "倉庫"),
    AssetSlice("af_v02_decal_water_warning", "decal", 6, Path("SourceArt/Generated/Decals/af_v02_decal_water_warning.png"), 1024, "水源警告有鹽痕與水壺語意，已覆蓋可讀繁體中文。", "鹹水"),
    AssetSlice("af_v02_decal_trade_notice", "decal", 7, Path("SourceArt/Generated/Decals/af_v02_decal_trade_notice.png"), 1024, "交易告示有秤盤與氧化銅語言，已覆蓋可讀繁體中文。", "交易"),
    AssetSlice("af_v02_decal_gate_mark", "decal", 8, Path("SourceArt/Generated/Decals/af_v02_decal_gate_mark.png"), 1024, "城門標記有砂岩與護板語言，已覆蓋可讀繁體中文。", "城門"),
    AssetSlice("af_v02_decal_wild_danger", "decal", 9, Path("SourceArt/Generated/Decals/af_v02_decal_wild_danger.png"), 1024, "荒野危險標記有裂片與焦黑邊緣，已覆蓋可讀繁體中文。", "危險"),
]


def load_manifest() -> dict[str, Any]:
    return json.loads(MANIFEST.read_text(encoding="utf-8"))


def save_manifest(payload: dict[str, Any]) -> None:
    MANIFEST.write_text(json.dumps(payload, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def copy_sheet(source: Path, dest: Path) -> None:
    dest.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(source, dest)


def crop_cell(image: Image.Image, columns: int, rows: int, index: int) -> Image.Image:
    width, height = image.size
    cell_width = width / columns
    cell_height = height / rows
    col = index % columns
    row = index // columns
    left = int(round(col * cell_width))
    upper = int(round(row * cell_height))
    right = int(round((col + 1) * cell_width))
    lower = int(round((row + 1) * cell_height))
    crop = image.crop((left, upper, right, lower))
    return ImageOps.fit(crop, (min(crop.size), min(crop.size)), method=Image.Resampling.LANCZOS, centering=(0.5, 0.5))


def font_for(size: int) -> ImageFont.FreeTypeFont | ImageFont.ImageFont:
    candidates = [
        Path("/System/Library/Fonts/PingFang.ttc"),
        Path("/System/Library/Fonts/STHeiti Light.ttc"),
        Path("/System/Library/Fonts/Supplemental/Arial Unicode.ttf"),
    ]
    for candidate in candidates:
        if candidate.exists():
            try:
                return ImageFont.truetype(str(candidate), size=size, index=0)
            except Exception:
                continue
    return ImageFont.load_default()


def overlay_label(image: Image.Image, text: str) -> Image.Image:
    rgba = image.convert("RGBA")
    draw = ImageDraw.Draw(rgba)
    font = font_for(max(96, rgba.size[0] // 6))
    box = draw.textbbox((0, 0), text, font=font, stroke_width=6)
    text_width = box[2] - box[0]
    text_height = box[3] - box[1]
    x = (rgba.size[0] - text_width) // 2
    y = (rgba.size[1] - text_height) // 2
    draw.rounded_rectangle(
        (x - 42, y - 32, x + text_width + 42, y + text_height + 42),
        radius=28,
        fill=(35, 29, 23, 165),
        outline=(229, 218, 188, 180),
        width=4,
    )
    draw.text((x, y), text, font=font, fill=(238, 225, 188, 255), stroke_width=6, stroke_fill=(28, 23, 18, 230))
    return rgba


def metadata_path(asset_id: str) -> Path:
    return METADATA_DIR / f"{asset_id}.json"


def write_metadata(record: dict[str, Any], spec: AssetSlice, source_sheet: Path) -> None:
    METADATA_DIR.mkdir(parents=True, exist_ok=True)
    payload = {
        "asset_id": spec.asset_id,
        "category": record["category"],
        "prompt": record["prompt"],
        "negative_prompt": record.get("negative_prompt", ""),
        "generation_tool": "built-in image_gen",
        "timestamp": datetime.now(timezone.utc).isoformat(),
        "selected_reason": spec.selected_reason,
        "output_path": str(spec.output_path),
        "ue_import_path": record["ue_import_path"],
        "usage_target": record["assigned_targets"],
        "source_sheet_path": str(source_sheet),
        "postprocess": "grid crop, resize, and optional Traditional Chinese signage overlay",
    }
    metadata_path(spec.asset_id).write_text(json.dumps(payload, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def validate_outputs(records_by_id: dict[str, dict[str, Any]]) -> list[dict[str, Any]]:
    rows: list[dict[str, Any]] = []
    for spec in SPECS:
        output = ROOT / spec.output_path
        with Image.open(output) as image:
            width, height = image.size
            mode = image.mode
        record = records_by_id[spec.asset_id]
        rows.append(
            {
                "asset_id": spec.asset_id,
                "category": record["category"],
                "output_path": str(spec.output_path),
                "ue_import_path": record["ue_import_path"],
                "width": width,
                "height": height,
                "mode": mode,
                "metadata_path": str(metadata_path(spec.asset_id).relative_to(ROOT)),
            }
        )
    return rows


def write_inventory(rows: list[dict[str, Any]]) -> None:
    REPORT_DIR.mkdir(parents=True, exist_ok=True)
    INVENTORY_JSON.write_text(json.dumps({"assets": rows}, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    lines = [
        "# Gate 12 生成美術資產清單",
        "",
        f"- 資產數：{len(rows)}",
        "- 生成工具：built-in image_gen",
        "- 後處理：grid crop、resize、必要時覆蓋繁體中文 signage 文字",
        "",
        "| asset_id | category | size | output | metadata |",
        "|---|---|---:|---|---|",
    ]
    for row in rows:
        lines.append(
            f"| `{row['asset_id']}` | `{row['category']}` | {row['width']}x{row['height']} | `{row['output_path']}` | `{row['metadata_path']}` |"
        )
    lines.append("")
    INVENTORY_MD.write_text("\n".join(lines), encoding="utf-8")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument("--concept-sheet", required=True)
    parser.add_argument("--surface-sheet", required=True)
    parser.add_argument("--ui-icon-sheet", required=True)
    parser.add_argument("--emblem-sheet", required=True)
    parser.add_argument("--banner-sheet", required=True)
    parser.add_argument("--portrait-sheet", required=True)
    parser.add_argument("--decal-sheet", required=True)
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    sheet_specs = {
        "concept": SheetSpec("concept", Path("SourceArt/Generated/Concepts/af_v02_sheet_concepts.png"), 3, 1),
        "surface": SheetSpec("surface", Path("SourceArt/Generated/Textures/af_v02_sheet_surfaces.png"), 4, 2),
        "ui_icon": SheetSpec("ui_icon", Path("SourceArt/Generated/UIIcons/af_v02_sheet_ui_icons.png"), 4, 3),
        "emblem": SheetSpec("emblem", Path("SourceArt/Generated/FactionEmblems/af_v02_sheet_faction_emblems.png"), 3, 1),
        "banner": SheetSpec("banner", Path("SourceArt/Generated/Banners/af_v02_sheet_banners.png"), 3, 1),
        "portrait": SheetSpec("portrait", Path("SourceArt/Generated/Portraits/af_v02_sheet_portraits.png"), 2, 2),
        "decal": SheetSpec("decal", Path("SourceArt/Generated/Decals/af_v02_sheet_decals.png"), 5, 2),
    }
    source_paths = {
        "concept": Path(args.concept_sheet),
        "surface": Path(args.surface_sheet),
        "ui_icon": Path(args.ui_icon_sheet),
        "emblem": Path(args.emblem_sheet),
        "banner": Path(args.banner_sheet),
        "portrait": Path(args.portrait_sheet),
        "decal": Path(args.decal_sheet),
    }

    for key, sheet_spec in sheet_specs.items():
        copy_sheet(source_paths[key], ROOT / sheet_spec.workspace_path)

    payload = load_manifest()
    records = payload.get("records", [])
    records_by_id = {record["asset_id"]: record for record in records if isinstance(record, dict) and "asset_id" in record}

    open_sheets: dict[str, Image.Image] = {}
    try:
        for key, sheet_spec in sheet_specs.items():
            open_sheets[key] = Image.open(ROOT / sheet_spec.workspace_path).convert("RGB")

        for spec in SPECS:
            if spec.asset_id not in records_by_id:
                raise RuntimeError(f"manifest 缺少 {spec.asset_id}")
            sheet_spec = sheet_specs[spec.sheet_key]
            image = crop_cell(open_sheets[spec.sheet_key], sheet_spec.columns, sheet_spec.rows, spec.index)
            image = image.resize((spec.target_size, spec.target_size), Image.Resampling.LANCZOS)
            if spec.overlay_text:
                image = overlay_label(image, spec.overlay_text)
            else:
                image = image.convert("RGBA")
            output = ROOT / spec.output_path
            output.parent.mkdir(parents=True, exist_ok=True)
            image.save(output, format="PNG", optimize=True)

            record = records_by_id[spec.asset_id]
            record["source_image_path"] = str(sheet_spec.workspace_path)
            record["generated_image_path"] = str(spec.output_path)
            record["validation_status"] = "generated"
            write_metadata(record, spec, sheet_spec.workspace_path)
    finally:
        for image in open_sheets.values():
            image.close()

    save_manifest(payload)
    rows = validate_outputs(records_by_id)
    write_inventory(rows)
    print(f"生成資產完成：{len(rows)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
