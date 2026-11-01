from pathlib import Path

from PIL import Image, ImageDraw


ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "assets" / "menu_icons"
HEADER = ROOT / "src" / "menu_icons_hd.h"
SIZE = 54
SCALE = 3

PALETTE = {
    "ink": (21, 31, 47),
    "white": (231, 244, 250),
    "cyan": (49, 205, 232),
    "blue": (45, 116, 202),
    "navy": (31, 67, 117),
    "gold": (255, 194, 58),
    "orange": (237, 105, 57),
    "coral": (237, 94, 123),
    "mint": (82, 211, 157),
    "green": (39, 137, 107),
    "lavender": (171, 150, 225),
    "black": (0, 0, 0),
}


def canvas():
    image = Image.new("RGB", (SIZE * SCALE, SIZE * SCALE), (0, 0, 0))
    return image, ImageDraw.Draw(image)


def xy(points):
    return [(int(x * SCALE), int(y * SCALE)) for x, y in points]


def rect(draw, box, fill, outline=None, width=1):
    x0, y0, x1, y1 = box
    draw.rectangle((x0 * SCALE, y0 * SCALE, x1 * SCALE - 1, y1 * SCALE - 1),
                   fill=PALETTE[fill], outline=PALETTE[outline] if outline else None,
                   width=width * SCALE)


def polygon(draw, points, fill, outline="ink", width=2):
    draw.polygon(xy(points), fill=PALETTE[fill])
    if outline:
        draw.line(xy(points + [points[0]]), fill=PALETTE[outline], width=width * SCALE, joint="curve")


def line(draw, points, color, width=2):
    draw.line(xy(points), fill=PALETTE[color], width=width * SCALE, joint="curve")


def ellipse(draw, box, fill, outline="ink", width=2):
    b = tuple(v * SCALE for v in box)
    draw.ellipse(b, fill=PALETTE[fill], outline=PALETTE[outline] if outline else None,
                 width=width * SCALE)


def status():
    im, d = canvas()
    ellipse(d, (7, 7, 47, 47), "gold", "ink", 2)
    ellipse(d, (12, 12, 42, 42), "white", "orange", 1)
    line(d, [(27, 15), (27, 27), (37, 21)], "coral", 3)
    ellipse(d, (24, 24, 30, 30), "blue", None, 0)
    line(d, [(17, 35), (37, 35)], "mint", 2)
    return im


def status_warn():
    im, d = canvas()
    ellipse(d, (7, 7, 47, 47), "coral", "ink", 2)
    ellipse(d, (12, 12, 42, 42), "white", "orange", 1)
    polygon(d, [(27, 14), (40, 38), (14, 38)], "gold", "ink", 2)
    line(d, [(27, 21), (27, 30)], "ink", 3)
    ellipse(d, (25, 33, 29, 37), "ink", None, 0)
    return im


def food():
    im, d = canvas()
    line(d, [(16, 12), (16, 20), (38, 20), (38, 12)], "cyan", 2)
    ellipse(d, (14, 9, 20, 15), "coral", None, 0)
    ellipse(d, (24, 7, 30, 14), "gold", None, 0)
    ellipse(d, (34, 10, 40, 16), "mint", None, 0)
    polygon(d, [(10, 25), (44, 25), (40, 39), (14, 39)], "coral", "ink", 2)
    line(d, [(13, 29), (41, 29)], "white", 2)
    line(d, [(17, 40), (14, 45)], "gold", 3)
    line(d, [(37, 40), (40, 45)], "gold", 3)
    return im


def sleep():
    im, d = canvas()
    ellipse(d, (9, 8, 35, 34), "gold", "ink", 2)
    ellipse(d, (18, 3, 42, 27), "black", None, 0)
    polygon(d, [(38, 10), (40, 15), (45, 16), (41, 19), (42, 24), (38, 21), (34, 24), (35, 19), (31, 16), (36, 15)], "cyan", None, 0)
    line(d, [(12, 39), (42, 39)], "white", 3)
    line(d, [(16, 34), (16, 44)], "coral", 3)
    line(d, [(38, 34), (38, 44)], "coral", 3)
    return im


def shower():
    im, d = canvas()
    line(d, [(14, 8), (14, 17), (29, 17)], "white", 4)
    line(d, [(29, 17), (38, 17), (42, 22)], "cyan", 4)
    line(d, [(38, 23), (35, 29)], "blue", 2)
    line(d, [(43, 23), (42, 30)], "blue", 2)
    line(d, [(31, 23), (28, 29)], "blue", 2)
    polygon(d, [(11, 32), (43, 32), (39, 43), (15, 43)], "cyan", "ink", 2)
    line(d, [(15, 36), (39, 36)], "white", 2)
    return im


def gacha():
    im, d = canvas()
    rect(d, (12, 10, 42, 45), "white", "ink", 2)
    rect(d, (16, 14, 38, 41), "blue", "gold", 1)
    polygon(d, [(27, 17), (30, 23), (37, 24), (32, 29), (34, 36), (27, 32), (20, 36), (22, 29), (17, 24), (24, 23)], "gold", "white", 1)
    line(d, [(16, 46), (38, 46)], "coral", 3)
    return im


def rooms():
    im, d = canvas()
    polygon(d, [(7, 25), (27, 8), (47, 25)], "coral", "ink", 2)
    rect(d, (11, 24, 43, 45), "gold", "ink", 2)
    rect(d, (23, 30, 33, 45), "blue", "ink", 1)
    ellipse(d, (29, 36, 31, 38), "white", None, 0)
    rect(d, (14, 28, 20, 35), "cyan", "ink", 1)
    return im


def collection():
    im, d = canvas()
    rect(d, (11, 17, 43, 43), "gold", "ink", 2)
    rect(d, (8, 12, 39, 20), "coral", "ink", 2)
    line(d, [(19, 26), (35, 26)], "white", 2)
    line(d, [(19, 32), (35, 32)], "white", 2)
    line(d, [(19, 38), (30, 38)], "white", 2)
    return im


def settings():
    im, d = canvas()
    ellipse(d, (14, 14, 40, 40), "cyan", "ink", 2)
    ellipse(d, (21, 21, 33, 33), "navy", "white", 1)
    for x, y in [(25, 7), (25, 41)]: rect(d, (x, y, x + 4, y + 7), "gold", "ink", 1)
    for x, y in [(7, 25), (41, 25)]: rect(d, (x, y, x + 7, y + 4), "gold", "ink", 1)
    for x, y in [(12, 12), (36, 12), (12, 36), (36, 36)]:
        ellipse(d, (x, y, x + 6, y + 6), "coral", "ink", 1)
    return im


def rgb565(image: Image.Image) -> list[int]:
    data = []
    for r, g, b in image.getdata():
        data.append(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3))
    return data


def main() -> None:
    OUT.mkdir(parents=True, exist_ok=True)
    icons = {
        "menu_status_hd": status(),
        "menu_status_warn_hd": status_warn(),
        "menu_food_hd": food(),
        "menu_sleep_hd": sleep(),
        "menu_shower_hd": shower(),
        "menu_gacha_hd": gacha(),
        "menu_rooms_hd": rooms(),
        "menu_collection_hd": collection(),
        "menu_settings_hd": settings(),
    }
    sheet = Image.new("RGB", (SIZE * 4, SIZE * 2), PALETTE["ink"])
    lines = ["#pragma once", "#include <Arduino.h>", "#include <pgmspace.h>", ""]
    for index, (name, image) in enumerate(icons.items()):
        preview = image.resize((SIZE, SIZE), Image.Resampling.NEAREST)
        preview.save(OUT / f"{name}.png")
        sheet.paste(preview, ((index % 4) * SIZE, (index // 4) * SIZE))
        values = rgb565(preview)
        lines.append(f"static const uint16_t {name}[{SIZE * SIZE}] PROGMEM = {{")
        for start in range(0, len(values), 12):
            lines.append("  " + ", ".join(f"0x{v:04X}" for v in values[start:start + 12]) + ",")
        lines.extend(["};", ""])
    sheet.save(OUT / "menu-icons-preview.png")
    HEADER.write_text("\n".join(lines), encoding="utf-8")
    print(f"Wrote {len(icons)} 54x54 menu icons to {HEADER}")


if __name__ == "__main__":
    main()
