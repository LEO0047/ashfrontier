#!/usr/bin/env python3
from __future__ import annotations

import json
import re
import sys
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
PROMPT_DIR = ROOT / "Docs" / "Art" / "Prompts"
MANIFEST = ROOT / "Content" / "Data" / "Art" / "ArtGenManifest.json"
METADATA_DIR = ROOT / "SourceArt" / "Generated" / "Metadata"
REPORT_DIR = ROOT / "Reports" / "Art"
REPORT_MD = REPORT_DIR / "art-prompt-lint.md"
REPORT_JSON = REPORT_DIR / "art-prompt-lint.json"
REQUIRED_PROMPT_FILES = [
    "ashfrontier_style_bible.md",
    "texture_prompts.md",
    "ui_icon_prompts.md",
    "faction_emblem_prompts.md",
    "portrait_prompts.md",
    "decal_and_signage_prompts.md",
]

BANNED_TERMS = [
    "Kenshi",
    "Kenshi 2",
    "Lo-Fi Games",
    "Hiver",
    "Shek",
    "Holy Nation",
    "United Cities",
    "Old Empire",
    "Beak Thing",
    "Skin Bandit",
    "Skeleton",
]

TRADITIONAL_ONLY_CHECK = set("简汉语开发测试验证构项")


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def load_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding="utf-8"))


def banned_hits(text: str) -> list[str]:
    hits: list[str] = []
    for term in BANNED_TERMS:
        if re.search(re.escape(term), text, re.IGNORECASE):
            hits.append(term)
    return sorted(set(hits), key=str.lower)


def check_text(path: Path, label: str, text: str, issues: list[dict[str, str]]) -> None:
    hits = banned_hits(text)
    if hits:
        issues.append(
            {
                "severity": "error",
                "file": str(path.relative_to(ROOT)),
                "message": f"{label} 含禁止詞：{', '.join(hits)}",
            }
        )
    simplified = sorted({ch for ch in text if ch in TRADITIONAL_ONLY_CHECK})
    if simplified:
        issues.append(
            {
                "severity": "error",
                "file": str(path.relative_to(ROOT)),
                "message": f"{label} 疑似包含簡體字：{''.join(simplified)}",
            }
        )


def collect_manifest_prompt_text(payload: Any) -> list[tuple[str, str]]:
    records = payload.get("records", []) if isinstance(payload, dict) else []
    results: list[tuple[str, str]] = []
    for index, record in enumerate(records):
        if not isinstance(record, dict):
            continue
        asset_id = str(record.get("asset_id", f"record_{index}"))
        for key in ("prompt", "negative_prompt"):
            value = record.get(key)
            if isinstance(value, str) and value.strip():
                results.append((f"{asset_id}.{key}", value))
    return results


def collect_manifest_asset_ids(payload: Any) -> list[str]:
    records = payload.get("records", []) if isinstance(payload, dict) else []
    asset_ids: list[str] = []
    for record in records:
        if isinstance(record, dict) and isinstance(record.get("asset_id"), str):
            asset_ids.append(record["asset_id"])
    return asset_ids


def write_report(issues: list[dict[str, str]], checked_files: list[str]) -> None:
    REPORT_DIR.mkdir(parents=True, exist_ok=True)
    status = "pass" if not any(issue["severity"] == "error" for issue in issues) else "fail"
    REPORT_JSON.write_text(
        json.dumps({"status": status, "checked_files": checked_files, "issues": issues}, ensure_ascii=False, indent=2) + "\n",
        encoding="utf-8",
    )
    lines = [
        "# Art Prompt Lint 報告",
        "",
        f"- 狀態：{'通過' if status == 'pass' else '失敗'}",
        f"- 檢查檔案數：{len(checked_files)}",
        f"- 問題數：{len(issues)}",
        "",
    ]
    if issues:
        lines.append("## 問題")
        lines.extend(f"- [{issue['severity']}] {issue['file']}：{issue['message']}" for issue in issues)
        lines.append("")
    else:
        lines.append("## 結果")
        lines.append("- Prompt、manifest prompt 與 metadata prompt 目前未發現禁止詞或簡體字。")
        lines.append("")
    REPORT_MD.write_text("\n".join(lines), encoding="utf-8")


def main() -> int:
    issues: list[dict[str, str]] = []
    checked_files: list[str] = []
    prompt_docs_text: list[str] = []

    for filename in REQUIRED_PROMPT_FILES:
        path = PROMPT_DIR / filename
        if not path.exists():
            issues.append({"severity": "error", "file": str(path.relative_to(ROOT)), "message": "缺少必備 prompt 文件"})

    if PROMPT_DIR.exists():
        for path in sorted(PROMPT_DIR.rglob("*.md")):
            text = read_text(path)
            prompt_docs_text.append(text)
            checked_files.append(str(path.relative_to(ROOT)))
            check_text(path, "prompt 文件", text, issues)

    manifest_asset_ids: list[str] = []
    if MANIFEST.exists():
        checked_files.append(str(MANIFEST.relative_to(ROOT)))
        try:
            payload = load_json(MANIFEST)
        except Exception as exc:
            issues.append({"severity": "error", "file": str(MANIFEST.relative_to(ROOT)), "message": f"JSON 解析失敗：{exc}"})
        else:
            manifest_asset_ids = collect_manifest_asset_ids(payload)
            for label, text in collect_manifest_prompt_text(payload):
                check_text(MANIFEST, label, text, issues)
    else:
        issues.append({"severity": "error", "file": str(MANIFEST.relative_to(ROOT)), "message": "缺少 ArtGenManifest.json"})

    prompt_docs_combined = "\n".join(prompt_docs_text)
    missing_asset_ids = [asset_id for asset_id in manifest_asset_ids if asset_id not in prompt_docs_combined]
    if missing_asset_ids:
        issues.append(
            {
                "severity": "error",
                "file": str(PROMPT_DIR.relative_to(ROOT)),
                "message": "prompt pack 未覆蓋 manifest asset_id："
                + ", ".join(missing_asset_ids[:20])
                + ("..." if len(missing_asset_ids) > 20 else ""),
            }
        )

    if METADATA_DIR.exists():
        for path in sorted(METADATA_DIR.glob("*.json")):
            checked_files.append(str(path.relative_to(ROOT)))
            try:
                payload = load_json(path)
            except Exception as exc:
                issues.append({"severity": "error", "file": str(path.relative_to(ROOT)), "message": f"JSON 解析失敗：{exc}"})
                continue
            if isinstance(payload, dict):
                for key in ("prompt", "negative_prompt"):
                    value = payload.get(key)
                    if isinstance(value, str) and value.strip():
                        check_text(path, key, value, issues)

    write_report(issues, checked_files)
    return 1 if any(issue["severity"] == "error" for issue in issues) else 0


if __name__ == "__main__":
    sys.exit(main())
