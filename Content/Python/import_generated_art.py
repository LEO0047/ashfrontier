from __future__ import annotations

import json
from datetime import datetime
from pathlib import Path
from typing import Any

import unreal


PROJECT_ROOT = Path(unreal.Paths.project_dir()).resolve()
MANIFEST = PROJECT_ROOT / "Content" / "Data" / "Art" / "ArtGenManifest.json"
REPORT_DIR = PROJECT_ROOT / "Reports" / "Art"
REPORT_MD = REPORT_DIR / "import-generated-art.md"
REPORT_JSON = REPORT_DIR / "import-generated-art.json"
MATERIAL_MAP_JSON = PROJECT_ROOT / "Content" / "Data" / "Art" / "GeneratedMaterialInstances.json"


MASTER_MATERIALS = {
    "surface": ("M_Ashfrontier_GeneratedSurface", "/Game/GeneratedArt/Materials"),
    "decal": ("M_Ashfrontier_GeneratedDecal", "/Game/GeneratedArt/Materials"),
    "banner": ("M_Ashfrontier_GeneratedBanner", "/Game/GeneratedArt/Materials"),
    "ui": ("M_Ashfrontier_UIIcon", "/Game/GeneratedArt/Materials"),
}


def load_manifest() -> dict[str, Any]:
    return json.loads(MANIFEST.read_text(encoding="utf-8"))


def save_manifest(payload: dict[str, Any]) -> None:
    MANIFEST.write_text(json.dumps(payload, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def game_path_for_png(path: Path) -> str:
    rel = path.relative_to(PROJECT_ROOT / "Content").with_suffix("")
    return "/Game/" + "/".join(rel.parts)


def import_pngs() -> list[str]:
    source_root = PROJECT_ROOT / "Content" / "GeneratedArt"
    pngs = sorted(source_root.rglob("*.png"))
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    tasks = []
    for path in pngs:
        destination = "/Game/" + "/".join(path.parent.relative_to(PROJECT_ROOT / "Content").parts)
        task = unreal.AssetImportTask()
        task.set_editor_property("filename", str(path))
        task.set_editor_property("destination_path", destination)
        task.set_editor_property("destination_name", path.stem)
        task.set_editor_property("automated", True)
        task.set_editor_property("replace_existing", True)
        task.set_editor_property("save", True)
        tasks.append(task)
    asset_tools.import_asset_tasks(tasks)
    imported = []
    for path in pngs:
        asset_path = game_path_for_png(path)
        asset = unreal.load_asset(asset_path)
        if asset:
            configure_texture(asset, asset_path)
            unreal.EditorAssetLibrary.save_loaded_asset(asset)
            imported.append(asset_path)
    return imported


def set_if_possible(asset: unreal.Object, prop: str, value: Any) -> None:
    try:
        asset.set_editor_property(prop, value)
    except Exception:
        pass


def configure_texture(texture: unreal.Object, asset_path: str) -> None:
    lower = asset_path.lower()
    if "_normal" in lower:
        set_if_possible(texture, "compression_settings", unreal.TextureCompressionSettings.TC_NORMALMAP)
        set_if_possible(texture, "srgb", False)
    elif "_roughness" in lower:
        set_if_possible(texture, "compression_settings", unreal.TextureCompressionSettings.TC_GRAYSCALE)
        set_if_possible(texture, "srgb", False)
    elif "/ui/" in lower or "/factions/" in lower or "/characters/" in lower:
        set_if_possible(texture, "texture_group", unreal.TextureGroup.TEXTUREGROUP_UI)
    set_if_possible(texture, "mip_gen_settings", unreal.TextureMipGenSettings.TMGS_FROM_TEXTURE_GROUP)


def create_asset(name: str, package_path: str, asset_class: Any, factory: Any) -> unreal.Object:
    full_path = f"{package_path}/{name}"
    existing = unreal.load_asset(full_path)
    if existing:
        return existing
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    return asset_tools.create_asset(name, package_path, asset_class, factory)


def create_master_materials() -> dict[str, unreal.Material]:
    materials: dict[str, unreal.Material] = {}
    for key, (name, package_path) in MASTER_MATERIALS.items():
        material = create_asset(name, package_path, unreal.Material, unreal.MaterialFactoryNew())
        if key == "decal":
            set_if_possible(material, "material_domain", unreal.MaterialDomain.MD_DEFERRED_DECAL)
            set_if_possible(material, "blend_mode", unreal.BlendMode.BLEND_TRANSLUCENT)
        elif key == "banner":
            set_if_possible(material, "two_sided", True)
        elif key == "ui":
            set_if_possible(material, "material_domain", unreal.MaterialDomain.MD_UI)
            set_if_possible(material, "blend_mode", unreal.BlendMode.BLEND_TRANSLUCENT)
        try:
            add_texture_parameter_graph(material, key)
        except Exception as exc:
            unreal.log_warning(f"Material graph setup skipped for {name}: {exc}")
        unreal.EditorAssetLibrary.save_loaded_asset(material)
        materials[key] = material
    return materials


def add_texture_parameter_graph(material: unreal.Material, key: str) -> None:
    expressions = unreal.MaterialEditingLibrary.get_material_expressions(material)
    if expressions:
        if key == "banner":
            for expression in expressions:
                if getattr(expression, "parameter_name", None) == "BaseColorTexture":
                    unreal.MaterialEditingLibrary.connect_material_property(expression, "RGB", unreal.MaterialProperty.MP_EMISSIVE_COLOR)
            unreal.MaterialEditingLibrary.recompile_material(material)
        return
    base = unreal.MaterialEditingLibrary.create_material_expression(
        material, unreal.MaterialExpressionTextureSampleParameter2D, -420, -80
    )
    base.set_editor_property("parameter_name", "BaseColorTexture")
    unreal.MaterialEditingLibrary.connect_material_property(base, "RGB", unreal.MaterialProperty.MP_BASE_COLOR)
    if key == "banner":
        unreal.MaterialEditingLibrary.connect_material_property(base, "RGB", unreal.MaterialProperty.MP_EMISSIVE_COLOR)
    if key == "surface":
        rough = unreal.MaterialEditingLibrary.create_material_expression(
            material, unreal.MaterialExpressionTextureSampleParameter2D, -420, 120
        )
        rough.set_editor_property("parameter_name", "RoughnessTexture")
        unreal.MaterialEditingLibrary.connect_material_property(rough, "R", unreal.MaterialProperty.MP_ROUGHNESS)
        normal = unreal.MaterialEditingLibrary.create_material_expression(
            material, unreal.MaterialExpressionTextureSampleParameter2D, -420, 320
        )
        normal.set_editor_property("parameter_name", "NormalTexture")
        try:
            normal.set_editor_property("sampler_type", unreal.MaterialSamplerType.SAMPLERTYPE_NORMAL)
        except Exception:
            pass
        unreal.MaterialEditingLibrary.connect_material_property(normal, "RGB", unreal.MaterialProperty.MP_NORMAL)
    unreal.MaterialEditingLibrary.recompile_material(material)


def create_material_instance(name: str, parent: unreal.Material, texture_paths: dict[str, str]) -> unreal.Object:
    instance = create_asset(name, "/Game/GeneratedArt/Materials", unreal.MaterialInstanceConstant, unreal.MaterialInstanceConstantFactoryNew())
    instance.set_editor_property("parent", parent)
    for parameter, asset_path in texture_paths.items():
        texture = unreal.load_asset(asset_path)
        if texture:
            try:
                unreal.MaterialEditingLibrary.set_material_instance_texture_parameter_value(instance, parameter, texture)
            except Exception as exc:
                unreal.log_warning(f"Cannot set {parameter} on {name}: {exc}")
    unreal.EditorAssetLibrary.save_loaded_asset(instance)
    return instance


def surface_stem(record: dict[str, Any]) -> str:
    name = record["ue_import_path"].rsplit("/", 1)[-1]
    return name.removesuffix("_BaseColor")


def create_instances(records: list[dict[str, Any]], masters: dict[str, unreal.Material]) -> list[dict[str, str]]:
    rows: list[dict[str, str]] = []
    for record in records:
        category = record.get("category")
        if category == "surface_texture":
            stem = surface_stem(record)
            name = "MI_" + stem.removeprefix("T_")
            paths = {
                "BaseColorTexture": f"/Game/GeneratedArt/Textures/{stem}_BaseColor",
                "RoughnessTexture": f"/Game/GeneratedArt/Textures/{stem}_Roughness",
                "NormalTexture": f"/Game/GeneratedArt/Textures/{stem}_Normal",
            }
            parent = masters["surface"]
        elif category == "decal_signage":
            tex_name = record["ue_import_path"].rsplit("/", 1)[-1]
            name = "MI_" + tex_name.removeprefix("T_")
            paths = {"BaseColorTexture": record["ue_import_path"]}
            decal_instance = create_material_instance(name, masters["decal"], paths)
            rows.append(
                {
                    "asset_id": record["asset_id"],
                    "category": category,
                    "material_instance": decal_instance.get_path_name().split(".", 1)[0],
                    "primary_texture": next(iter(paths.values())),
                }
            )
            name = name.replace("MI_AfV02_Decal_", "MI_AfV02_Signage_", 1)
            parent = masters["banner"]
        elif category == "banner":
            tex_name = record["ue_import_path"].rsplit("/", 1)[-1]
            name = "MI_" + tex_name.removeprefix("T_")
            paths = {"BaseColorTexture": record["ue_import_path"]}
            parent = masters["banner"]
        elif category in {"ui_icon", "faction_emblem", "portrait"}:
            tex_name = record["ue_import_path"].rsplit("/", 1)[-1]
            name = "MI_" + tex_name.removeprefix("T_")
            paths = {"BaseColorTexture": record["ue_import_path"]}
            parent = masters["ui"]
        else:
            continue
        instance = create_material_instance(name, parent, paths)
        rows.append(
            {
                "asset_id": record["asset_id"],
                "category": category,
                "material_instance": instance.get_path_name().split(".", 1)[0],
                "primary_texture": next(iter(paths.values())),
            }
        )
    return rows


def validate_assets(imported_textures: list[str], material_rows: list[dict[str, str]]) -> list[str]:
    errors: list[str] = []
    for path in imported_textures:
        if not unreal.load_asset(path):
            errors.append(f"缺少 texture asset：{path}")
    for key, (name, package_path) in MASTER_MATERIALS.items():
        if not unreal.load_asset(f"{package_path}/{name}"):
            errors.append(f"缺少 master material：{name}")
    for row in material_rows:
        if not unreal.load_asset(row["material_instance"]):
            errors.append(f"缺少 material instance：{row['material_instance']}")
        if not unreal.load_asset(row["primary_texture"]):
            errors.append(f"material instance primary texture 無效：{row['primary_texture']}")
    return errors


def write_reports(imported_textures: list[str], material_rows: list[dict[str, str]], errors: list[str]) -> None:
    REPORT_DIR.mkdir(parents=True, exist_ok=True)
    status = "pass" if not errors else "fail"
    payload = {
        "status": status,
        "imported_textures": len(imported_textures),
        "material_instances": len(material_rows),
        "errors": errors,
        "textures": imported_textures,
        "material_rows": material_rows,
    }
    REPORT_JSON.write_text(json.dumps(payload, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    MATERIAL_MAP_JSON.write_text(json.dumps({"records": material_rows}, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    lines = [
        "# 生成美術匯入報告",
        "",
        f"- 狀態：{'通過' if status == 'pass' else '失敗'}",
        f"- 時間：{datetime.now().strftime('%Y-%m-%d %H:%M:%S')}",
        f"- 匯入貼圖數：{len(imported_textures)}",
        f"- Material Instance 數：{len(material_rows)}",
        f"- 問題數：{len(errors)}",
        "",
    ]
    if errors:
        lines.append("## 問題")
        lines.extend(f"- {error}" for error in errors)
        lines.append("")
    lines.extend(["## Master Materials", "", "- `/Game/GeneratedArt/Materials/M_Ashfrontier_GeneratedSurface`", "- `/Game/GeneratedArt/Materials/M_Ashfrontier_GeneratedDecal`", "- `/Game/GeneratedArt/Materials/M_Ashfrontier_GeneratedBanner`", "- `/Game/GeneratedArt/Materials/M_Ashfrontier_UIIcon`", ""])
    lines.extend(["## Material Instances", "", "| asset_id | category | material_instance | primary_texture |", "|---|---|---|---|"])
    for row in material_rows:
        lines.append(
            f"| `{row['asset_id']}` | `{row['category']}` | `{row['material_instance']}` | `{row['primary_texture']}` |"
        )
    lines.append("")
    REPORT_MD.write_text("\n".join(lines), encoding="utf-8")


def main() -> int:
    payload = load_manifest()
    records = [record for record in payload.get("records", []) if isinstance(record, dict)]
    imported_textures = import_pngs()
    masters = create_master_materials()
    material_rows = create_instances(records, masters)
    for record in records:
        if record.get("validation_status") in {"processed", "generated"}:
            record["validation_status"] = "imported"
    save_manifest(payload)
    errors = validate_assets(imported_textures, material_rows)
    write_reports(imported_textures, material_rows, errors)
    if errors:
        for error in errors:
            unreal.log_error(error)
        return 1
    unreal.log("Ashfrontier generated art import complete")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
