from __future__ import annotations

import json
from pathlib import Path
import sys


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_SOURCE = Path(r"C:\Users\18308\Documents\ChatGPT\bb\Prescript_Terminal\rawids.json")
OUTPUT = ROOT / "src" / "gacha_pool.h"


def main() -> None:
    source = Path(sys.argv[1]) if len(sys.argv) > 1 else DEFAULT_SOURCE
    records = json.loads(source.read_text(encoding="utf-8"))["_embedded"]
    pools = {1: [], 2: [], 3: []}
    seen = set()
    for record in records:
        name = record.get("name")
        rarity = int(record.get("rarity", 0))
        if name and rarity in pools and name not in seen:
            pools[rarity].append(name)
            seen.add(name)

    lines = ["#pragma once", "", "// Embedded from the original personality pool; no network is required."]
    for rarity in (1, 2, 3):
        items = ",\n".join("  " + json.dumps(name, ensure_ascii=False) for name in pools[rarity])
        lines += [
            f"static const char* const gacha{rarity}Star[] = {{",
            items,
            "};",
            f"static constexpr size_t gacha{rarity}StarCount = sizeof(gacha{rarity}Star) / sizeof(gacha{rarity}Star[0]);",
            "",
        ]
    OUTPUT.write_text("\n".join(lines), encoding="utf-8")
    print(f"Generated {sum(len(v) for v in pools.values())} entries: {len(pools[1])}/{len(pools[2])}/{len(pools[3])}")


if __name__ == "__main__":
    main()
