from pathlib import Path
import sys

from PIL import Image


ROOT = Path(__file__).resolve().parents[1]
FRAME_DIR = ROOT / "assets" / "boot_frames"
OUTPUT = ROOT / "src" / "zhiyi_boot.h"
WIDTH = 128
HEIGHT = 128


def rgb565(red: int, green: int, blue: int) -> int:
    return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3)


def main() -> None:
    paths = sorted(FRAME_DIR.glob("frame_*.png"))
    if not paths:
        raise SystemExit("No boot frames found")
    lines = [
        "#pragma once",
        "#include <Arduino.h>",
        "#include <pgmspace.h>",
        "",
        f"static constexpr uint8_t ZHIYI_BOOT_FRAME_COUNT = {len(paths)};",
        f"static const uint16_t zhiyiBootFrames[{len(paths)}][{WIDTH * HEIGHT}] PROGMEM = {{",
    ]
    for path in paths:
        image = Image.open(path).convert("RGB")
        if image.size != (WIDTH, HEIGHT):
            raise SystemExit(f"Unexpected frame size: {path} -> {image.size}")
        values = [rgb565(*pixel) for pixel in image.getdata()]
        lines.append("  {")
        for start in range(0, len(values), 16):
            lines.append("    " + ", ".join(f"0x{value:04X}" for value in values[start:start + 16]) + ",")
        lines.append("  },")
    lines.append("};")
    OUTPUT.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(f"Generated {len(paths)} boot frames: {OUTPUT}")


if __name__ == "__main__":
    main()
