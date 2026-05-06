#!/usr/bin/env python3
from __future__ import annotations

import json
import sys
from pathlib import Path
from typing import Any

from PIL import Image, ImageStat


ROOT = Path(__file__).resolve().parents[1]
SCREENSHOT_DIR = ROOT / "Reports" / "Art" / "Screenshots"
REPORT_DIR = ROOT / "Reports" / "Art"
REPORT_MD = REPORT_DIR / "screenshot-smoke.md"
REPORT_JSON = REPORT_DIR / "screenshot-smoke.json"

EXPECTED_SCREENSHOTS = {
    "01_main_city_entrance.png": "主城入口",
    "02_main_city_market.png": "主城商店區",
    "03_guard_patrol.png": "守衛巡邏區",
    "04_outpost_exterior.png": "前哨外觀",
    "05_wilderness_build_zone.png": "野外建造區",
    "06_recruit_ui.png": "角色招募 UI",
    "07_trade_ui.png": "交易 UI",
    "08_inventory_ui.png": "庫存 UI",
}


def add_issue(issues: list[dict[str, Any]], severity: str, file: Path, message: str) -> None:
    issues.append({"severity": severity, "file": str(file.relative_to(ROOT)), "message": message})


def inspect_image(path: Path, issues: list[dict[str, Any]]) -> dict[str, Any]:
    try:
        with Image.open(path) as image:
            rgb = image.convert("RGB")
            width, height = rgb.size
            stat = ImageStat.Stat(rgb)
            mean = stat.mean
            extrema = stat.extrema
            pixels = list(rgb.resize((160, 90)).getdata())
    except Exception as exc:
        add_issue(issues, "error", path, f"圖片無法讀取：{exc}")
        return {"path": str(path.relative_to(ROOT)), "readable": False}

    brightness = sum(mean) / 3.0
    dynamic_range = max(high for _, high in extrema) - min(low for low, _ in extrema)
    gray_like = 0
    very_dark = 0
    low_saturation = 0
    for red, green, blue in pixels:
        local_brightness = (red + green + blue) / 3.0
        local_range = max(red, green, blue) - min(red, green, blue)
        if local_brightness < 8:
            very_dark += 1
        if local_range < 8 and 70 <= local_brightness <= 190:
            gray_like += 1
        if local_range < 14:
            low_saturation += 1

    total = max(1, len(pixels))
    gray_ratio = gray_like / total
    dark_ratio = very_dark / total
    low_saturation_ratio = low_saturation / total

    if width < 640 or height < 360:
        add_issue(issues, "error", path, f"解析度過低：{width}x{height}")
    if brightness < 12 or dark_ratio > 0.92:
        add_issue(issues, "error", path, f"畫面疑似全黑：brightness={brightness:.1f}, dark_ratio={dark_ratio:.2f}")
    if dynamic_range < 30:
        add_issue(issues, "error", path, f"畫面動態範圍過低：{dynamic_range}")
    if gray_ratio > 0.88 and low_saturation_ratio > 0.94:
        add_issue(issues, "error", path, f"畫面疑似大面積未貼材質灰色：gray_ratio={gray_ratio:.2f}")

    return {
        "path": str(path.relative_to(ROOT)),
        "readable": True,
        "width": width,
        "height": height,
        "brightness": round(brightness, 2),
        "dynamic_range": dynamic_range,
        "dark_ratio": round(dark_ratio, 4),
        "gray_ratio": round(gray_ratio, 4),
        "low_saturation_ratio": round(low_saturation_ratio, 4),
    }


def write_report(status: str, images: list[dict[str, Any]], issues: list[dict[str, Any]]) -> None:
    REPORT_DIR.mkdir(parents=True, exist_ok=True)
    REPORT_JSON.write_text(
        json.dumps({"status": status, "images": images, "issues": issues}, ensure_ascii=False, indent=2) + "\n",
        encoding="utf-8",
    )

    lines = [
        "# Screenshot Smoke 報告",
        "",
        f"- 狀態：{'通過' if status == 'pass' else '失敗'}",
        f"- 截圖數：{len(images)}",
        f"- 問題數：{len(issues)}",
        "",
        "## 截圖檢查",
        "",
    ]
    for image in images:
        lines.append(
            f"- `{image.get('path')}`：{image.get('width', '?')}x{image.get('height', '?')}，"
            f"brightness={image.get('brightness', '?')}，gray_ratio={image.get('gray_ratio', '?')}"
        )
    lines.append("")
    if issues:
        lines.append("## 問題")
        lines.extend(f"- [{issue['severity']}] {issue['file']}：{issue['message']}" for issue in issues)
        lines.append("")
    else:
        lines.append("## 結果")
        lines.append("- 固定相機截圖皆存在，且未偵測到全黑、解析度過低或大面積純灰畫面。")
        lines.append("")
    REPORT_MD.write_text("\n".join(lines), encoding="utf-8")


def main() -> int:
    issues: list[dict[str, Any]] = []
    images: list[dict[str, Any]] = []

    for filename, label in EXPECTED_SCREENSHOTS.items():
        path = SCREENSHOT_DIR / filename
        if not path.exists():
            add_issue(issues, "error", path, f"缺少固定相機截圖：{label}")
            continue
        images.append(inspect_image(path, issues))

    status = "pass" if not any(issue["severity"] == "error" for issue in issues) else "fail"
    write_report(status, images, issues)
    return 1 if status == "fail" else 0


if __name__ == "__main__":
    sys.exit(main())
