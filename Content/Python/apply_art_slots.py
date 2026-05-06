from __future__ import annotations

import json
from datetime import datetime
from pathlib import Path
from typing import Any

import unreal


PROJECT_ROOT = Path(unreal.Paths.project_dir()).resolve()
ART_DIR = PROJECT_ROOT / "Content" / "Data" / "Art"
MAPPING = ART_DIR / "ArtSlotMapping.json"
ASSIGNMENTS = ART_DIR / "EnvironmentArtAssignments.json"
REPORT_DIR = PROJECT_ROOT / "Reports" / "Art"
REPORT_MD = REPORT_DIR / "artslot-application.md"
REPORT_JSON = REPORT_DIR / "artslot-application.json"


def load_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def main() -> int:
    mapping = load_json(MAPPING)
    assignments = load_json(ASSIGNMENTS)
    mapping_records = [record for record in mapping.get("records", []) if isinstance(record, dict)]
    assignment_records = [record for record in assignments.get("records", []) if isinstance(record, dict)]
    material_failures: list[str] = []
    loaded_materials = 0
    for record in mapping_records:
        path = str(record.get("material_instance_path", ""))
        if unreal.load_asset(path):
            loaded_materials += 1
        else:
            material_failures.append(path)

    status = "pass" if not material_failures else "fail"
    REPORT_DIR.mkdir(parents=True, exist_ok=True)
    REPORT_JSON.write_text(
        json.dumps(
            {
                "status": status,
                "slots": len(mapping_records),
                "assignments": len(assignment_records),
                "loaded_materials": loaded_materials,
                "material_failures": material_failures,
            },
            ensure_ascii=False,
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )
    lines = [
        "# ArtSlot 套用報告",
        "",
        f"- 狀態：{'通過' if status == 'pass' else '失敗'}",
        f"- 時間：{datetime.now().strftime('%Y-%m-%d %H:%M:%S')}",
        f"- ArtSlot 數：{len(mapping_records)}",
        f"- Assignment 數：{len(assignment_records)}",
        f"- UE 可載入材質數：{loaded_materials}",
        f"- 材質缺失數：{len(material_failures)}",
        "",
        "## 套用方式",
        "",
        "- `AAshfrontierWorldBlockoutDirector` 於 runtime 讀取 `ArtSlotMapping.json` 與 `EnvironmentArtAssignments.json`。",
        "- block、banner 與 decal actor 依 actor name 查 ArtSlot，再套用對應 material instance。",
        "- 本 UE smoke 確認所有 mapping material instance 可載入。",
        "",
    ]
    if material_failures:
        lines.append("## 問題")
        lines.extend(f"- 缺少材質：`{path}`" for path in material_failures)
        lines.append("")
    REPORT_MD.write_text("\n".join(lines), encoding="utf-8")
    if material_failures:
        for path in material_failures:
            unreal.log_error(f"缺少材質：{path}")
        return 1
    unreal.log("Ashfrontier ArtSlot apply smoke complete")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
