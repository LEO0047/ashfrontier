#!/usr/bin/env python3
from __future__ import annotations

import argparse
import json
from datetime import datetime
from pathlib import Path
from typing import Any

from PIL import Image, ImageOps

try:
    import numpy as np
except Exception:  # pragma: no cover
    np = None


ROOT = Path(__file__).resolve().parents[1]
MANIFEST = ROOT / "Content" / "Data" / "Art" / "ArtGenManifest.json"
REPORT_DIR = ROOT / "Reports" / "Art"
REPORT_MD = REPORT_DIR / "texture-processing.md"
REPORT_JSON = REPORT_DIR / "texture-processing.json"


CATEGORY_DIRS = {
    "concept": "Concepts",
    "surface_texture": "Textures",
    "ui_icon": "UI",
    "faction_emblem": "Factions",
    "portrait": "Characters",
    "decal_signage": "Decals",
    "banner": "Banners",
}


def load_manifest() -> dict[str, Any]:
    return json.loads(MANIFEST.read_text(encoding="utf-8"))


def save_manifest(payload: dict[str, Any]) -> None:
    MANIFEST.write_text(json.dumps(payload, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def asset_name_from_ue_path(ue_path: str) -> str:
    return ue_path.rsplit("/", 1)[-1]


def png_path_from_ue_path(ue_path: str) -> Path:
    if not ue_path.startswith("/Game/GeneratedArt/"):
        raise ValueError(f"ue_import_path 不在 /Game/GeneratedArt：{ue_path}")
    relative = ue_path.removeprefix("/Game/")
    return ROOT / "Content" / relative / ".." / f"{asset_name_from_ue_path(ue_path)}.png"


def content_dir_for(category: str) -> Path:
    if category not in CATEGORY_DIRS:
        raise ValueError(f"未知 category：{category}")
    return ROOT / "Content" / "GeneratedArt" / CATEGORY_DIRS[category]


def write_png(image: Image.Image, path: Path) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    image.save(path, format="PNG", optimize=True)


def make_roughness(base: Image.Image) -> Image.Image:
    gray = ImageOps.grayscale(base)
    gray = ImageOps.autocontrast(gray)
    if np is None:
        return ImageOps.colorize(gray, black=(172, 172, 172), white=(235, 235, 235)).convert("L")
    arr = np.asarray(gray, dtype=np.float32)
    rough = 176.0 + (arr / 255.0) * 58.0
    return Image.fromarray(np.clip(rough, 0, 255).astype("uint8"))


def make_normal(base: Image.Image) -> Image.Image:
    gray = ImageOps.grayscale(base.resize((1024, 1024), Image.Resampling.LANCZOS))
    if np is None:
        return Image.new("RGB", gray.size, (128, 128, 255))
    height = np.asarray(gray, dtype=np.float32) / 255.0
    gy, gx = np.gradient(height)
    strength = 3.0
    nx = -gx * strength
    ny = -gy * strength
    nz = np.ones_like(height)
    length = np.sqrt(nx * nx + ny * ny + nz * nz)
    nx = nx / length
    ny = ny / length
    nz = nz / length
    normal = np.stack(((nx * 0.5 + 0.5) * 255.0, (ny * 0.5 + 0.5) * 255.0, (nz * 0.5 + 0.5) * 255.0), axis=-1)
    return Image.fromarray(np.clip(normal, 0, 255).astype("uint8"))


def process_surface(record: dict[str, Any]) -> list[dict[str, Any]]:
    source = ROOT / record["generated_image_path"]
    asset_name = asset_name_from_ue_path(record["ue_import_path"])
    stem = asset_name.removesuffix("_BaseColor")
    dest_dir = content_dir_for("surface_texture")
    base_path = dest_dir / f"{stem}_BaseColor.png"
    roughness_path = dest_dir / f"{stem}_Roughness.png"
    normal_path = dest_dir / f"{stem}_Normal.png"
    with Image.open(source) as image:
        base = image.convert("RGBA").resize((1024, 1024), Image.Resampling.LANCZOS)
        write_png(base, base_path)
        roughness = make_roughness(base.convert("RGB"))
        write_png(roughness, roughness_path)
        normal = make_normal(base.convert("RGB"))
        write_png(normal, normal_path)
    return [
        {"kind": "basecolor", "path": str(base_path.relative_to(ROOT)), "size": "1024x1024"},
        {"kind": "roughness", "path": str(roughness_path.relative_to(ROOT)), "size": "1024x1024"},
        {"kind": "normal", "path": str(normal_path.relative_to(ROOT)), "size": "1024x1024"},
    ]


def copy_non_surface(record: dict[str, Any]) -> list[dict[str, Any]]:
    source = ROOT / record["generated_image_path"]
    category = record["category"]
    asset_name = asset_name_from_ue_path(record["ue_import_path"])
    dest = content_dir_for(category) / f"{asset_name}.png"
    with Image.open(source) as image:
        rgba = image.convert("RGBA")
        write_png(rgba, dest)
        size = f"{rgba.size[0]}x{rgba.size[1]}"
    return [{"kind": "texture", "path": str(dest.relative_to(ROOT)), "size": size}]


def expected_outputs(record: dict[str, Any]) -> list[Path]:
    category = record["category"]
    asset_name = asset_name_from_ue_path(record["ue_import_path"])
    if category == "surface_texture":
        stem = asset_name.removesuffix("_BaseColor")
        return [
            content_dir_for(category) / f"{stem}_BaseColor.png",
            content_dir_for(category) / f"{stem}_Roughness.png",
            content_dir_for(category) / f"{stem}_Normal.png",
        ]
    return [content_dir_for(category) / f"{asset_name}.png"]


def check_outputs(payload: dict[str, Any]) -> list[str]:
    errors: list[str] = []
    for record in payload.get("records", []):
        if not isinstance(record, dict):
            continue
        for path in expected_outputs(record):
            if not path.exists():
                errors.append(f"缺少輸出：{path.relative_to(ROOT)}")
                continue
            try:
                with Image.open(path) as image:
                    width, height = image.size
                    mode = image.mode
            except Exception as exc:
                errors.append(f"無法讀取：{path.relative_to(ROOT)}：{exc}")
                continue
            if width not in {512, 1024} or height not in {512, 1024}:
                errors.append(f"尺寸不符：{path.relative_to(ROOT)} {width}x{height}")
            if mode not in {"L", "RGB", "RGBA"}:
                errors.append(f"色彩模式不符：{path.relative_to(ROOT)} {mode}")
    return errors


def write_report(rows: list[dict[str, Any]], errors: list[str], check_only: bool) -> None:
    REPORT_DIR.mkdir(parents=True, exist_ok=True)
    status = "pass" if not errors else "fail"
    REPORT_JSON.write_text(
        json.dumps({"status": status, "check_only": check_only, "outputs": rows, "errors": errors}, ensure_ascii=False, indent=2) + "\n",
        encoding="utf-8",
    )
    lines = [
        "# 貼圖處理報告",
        "",
        f"- 狀態：{'通過' if status == 'pass' else '失敗'}",
        f"- 模式：{'檢查' if check_only else '處理'}",
        f"- 時間：{datetime.now().strftime('%Y-%m-%d %H:%M:%S %Z')}",
        f"- 輸出數：{len(rows)}",
        f"- 問題數：{len(errors)}",
        "",
    ]
    if errors:
        lines.append("## 問題")
        lines.extend(f"- {error}" for error in errors)
        lines.append("")
    if rows:
        lines.extend(["## 輸出", "", "| asset_id | kind | path | size |", "|---|---|---|---:|"])
        for row in rows:
            lines.append(f"| `{row['asset_id']}` | `{row['kind']}` | `{row['path']}` | {row['size']} |")
        lines.append("")
    REPORT_MD.write_text("\n".join(lines), encoding="utf-8")


def process() -> int:
    payload = load_manifest()
    rows: list[dict[str, Any]] = []
    for record in payload.get("records", []):
        if not isinstance(record, dict):
            continue
        if record.get("validation_status") not in {"generated", "processed", "imported", "assigned"}:
            continue
        if record.get("category") == "surface_texture":
            outputs = process_surface(record)
        else:
            outputs = copy_non_surface(record)
        for output in outputs:
            rows.append({"asset_id": record["asset_id"], **output})
        if record.get("validation_status") == "generated":
            record["validation_status"] = "processed"
    save_manifest(payload)
    errors = check_outputs(payload)
    write_report(rows, errors, check_only=False)
    return 1 if errors else 0


def check() -> int:
    payload = load_manifest()
    rows: list[dict[str, Any]] = []
    for record in payload.get("records", []):
        if not isinstance(record, dict):
            continue
        for path in expected_outputs(record):
            if path.exists():
                with Image.open(path) as image:
                    size = f"{image.size[0]}x{image.size[1]}"
                rows.append({"asset_id": record["asset_id"], "kind": "existing", "path": str(path.relative_to(ROOT)), "size": size})
    errors = check_outputs(payload)
    write_report(rows, errors, check_only=True)
    return 1 if errors else 0


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    return check() if args.check else process()


if __name__ == "__main__":
    raise SystemExit(main())
