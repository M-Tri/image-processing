#!/usr/bin/env python3
"""Generate deterministic equation images used by the solver accuracy test."""

from pathlib import Path

from PIL import Image, ImageDraw, ImageFont


ROOT = Path(__file__).resolve().parents[1]
OUTPUT_DIR = ROOT / "test-fixtures" / "equations"
FONT_CANDIDATES = [
    "/System/Library/Fonts/Supplemental/Arial Bold.ttf",
    "/System/Library/Fonts/Supplemental/Arial.ttf",
    "/System/Library/Fonts/Supplemental/Courier New.ttf",
]

CASES = [
    ("equationsImage3.png", ["x = 7 - y", "2x - y = 5"]),
    ("equationsImage4.png", ["3x = 16 - 2y", "x - y = 2"]),
    ("equationsImage5.png", ["2x = 18 - 3y", "4x - y = 8"]),
    ("equationsImage6.png", ["x = 11 - 2y", "3x - y = 5"]),
    ("equationsImage7.png", ["x - y = 3", "x = 1 - y"]),
    ("equationsImage8.png", ["2x - y - z = 0", "3x - y - 2z = 1", "5x - 2y - z = 3"]),
    ("equationsImage9.png", ["6x - y - z = 1", "5z - x - y = 12", "4y - x - z = 4"]),
    ("equationsImage10.png", ["4x = 10 - 2y", "x = 4 - y"]),
    ("equationsImage11.png", ["5x - y = 14", "2x = 7 - y"]),
    ("equationsImage12.png", ["x = 14 - 3y", "2x = 13 - y"]),
]


def load_font(size):
    for font_path in FONT_CANDIDATES:
        if Path(font_path).exists():
            return ImageFont.truetype(font_path, size)
    return ImageFont.load_default()


def draw_case(filename, lines):
    width = 920
    line_height = 84
    padding_x = 72
    padding_y = 56
    height = padding_y * 2 + line_height * len(lines)

    image = Image.new("RGB", (width, height), "white")
    draw = ImageDraw.Draw(image)
    font = load_font(56)

    for index, line in enumerate(lines):
        y = padding_y + index * line_height
        draw.text((padding_x, y), line, fill="black", font=font)

    image.save(OUTPUT_DIR / filename)


def main():
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)

    for filename, lines in CASES:
        draw_case(filename, lines)

    print(f"Generated {len(CASES)} fixture images in {OUTPUT_DIR}")


if __name__ == "__main__":
    main()
