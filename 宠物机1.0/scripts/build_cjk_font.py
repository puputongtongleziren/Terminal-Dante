from pathlib import Path
import re
import sys

from PIL import Image, ImageDraw, ImageFont


ROOT = Path(__file__).resolve().parents[1]
OUTPUT = ROOT / "src" / "zhiyi_cjk_font.h"
FONT_PATH = Path(r"C:\Windows\Fonts\msyh.ttc")
EXTRA_TEXT = [
    "狂气获取提示单抽十连保底抽卡结果星级角色退出不足总抽数饱食度困倦值洁净度",
    "昼夜模式披肩开关背光设置返回白天夜晚背光固定开启卡册已获得剩余未获得翻页服装心情得到吱咿的礼物吱咿的记忆已获得新时装服装尚未解锁硬币颜色金红绿握住我的手吧赤红之钉的洗礼选择技能投掷次数正面反面继续返回",
    "罗佳LCB罪人辛克莱希斯克利夫格里高尔默尔索以实玛利",
    "李箱Seven协会南部科良秀W公司级清扫人员",
]


def main() -> None:
    source_text = "\n".join(
        path.read_text(encoding="utf-8-sig", errors="ignore")
        for path in (ROOT / "src").rglob("*")
        if path.suffix.lower() in {".cpp", ".h"} and path.name != OUTPUT.name
    )
    literals = re.findall(r'"((?:\\.|[^"\\])*)"', source_text)
    chars = {ch for literal in literals + EXTRA_TEXT for ch in literal if "\u4e00" <= ch <= "\u9fff"}
    ordered = sorted(chars, key=ord)

    font = ImageFont.truetype(str(FONT_PATH), 12, index=0)
    rows = []
    for ch in ordered:
        image = Image.new("L", (12, 12), 0)
        draw = ImageDraw.Draw(image)
        bounds = draw.textbbox((0, 0), ch, font=font)
        width, height = bounds[2] - bounds[0], bounds[3] - bounds[1]
        x = (12 - width) // 2 - bounds[0]
        y = (12 - height) // 2 - bounds[1]
        draw.text((x, y), ch, font=font, fill=255)
        bitmap = []
        for y in range(12):
            bits = 0
            for x in range(12):
                if image.getpixel((x, y)) >= 96:
                    bits |= 1 << (11 - x)
            bitmap.extend(((bits >> 8) & 0xFF, bits & 0xFF))
        rows.append((ord(ch), bitmap))

    lines = [
        "#pragma once",
        "#include <Arduino.h>",
        "#include <pgmspace.h>",
        "",
        "struct CjkGlyph12 { uint16_t code; uint8_t bitmap[24]; };",
        f"static const CjkGlyph12 zhiyiCjkGlyphs[{len(rows)}] PROGMEM = {{",
    ]
    for code, bitmap in rows:
        payload = ", ".join(f"0x{byte:02X}" for byte in bitmap)
        lines.append(f"  {{0x{code:04X}, {{{payload}}}}},")
    lines.extend(["};", f"static constexpr size_t ZHIYI_CJK_GLYPH_COUNT = {len(rows)};", ""])
    OUTPUT.write_text("\n".join(lines), encoding="utf-8")
    print(f"Generated {len(rows)} 12x12 glyphs ({len(rows) * 24} bitmap bytes): {OUTPUT}")


if __name__ == "__main__":
    main()
