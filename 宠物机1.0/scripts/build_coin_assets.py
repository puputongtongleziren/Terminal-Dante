from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT.parent / "Prescript_Terminal" / "fat_data" / "Resources" / "coin" / "images"
OUTPUT = ROOT / "src" / "coin_assets.h"
SIZE = 96


def main() -> None:
    arrays = []
    for name in ("heads.bin", "tails.bin", "rheads.bin", "rtails.bin", "gheads.bin", "gtails.bin"):
        data = (SOURCE / name).read_bytes()
        if len(data) != SIZE * SIZE * 2:
            raise SystemExit(f"Unexpected asset size: {name} ({len(data)})")
        values = [data[i] | (data[i + 1] << 8) for i in range(0, len(data), 2)]
        arrays.append((name.split(".")[0], values))

    lines = ["#pragma once", "#include <Arduino.h>", "#include <pgmspace.h>", ""]
    for name, values in arrays:
        lines.append(f"static const uint16_t coin_{name}[{SIZE * SIZE}] PROGMEM = {{")
        for start in range(0, len(values), 16):
            lines.append("  " + ", ".join(f"0x{value:04X}" for value in values[start:start + 16]) + ",")
        lines.extend(["};", ""])
    lines.append(f"static constexpr uint16_t COIN_ASSET_SIZE = {SIZE};")
    OUTPUT.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(f"Generated coin assets: {OUTPUT}")


if __name__ == "__main__":
    main()
