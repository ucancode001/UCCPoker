#!/usr/bin/env python3
"""Generate poker game PNG assets matching the original UCCPocker resource layout."""

from __future__ import annotations

import os
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont

ROOT = Path(__file__).resolve().parents[1] / "image"

RANKS = ["A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"]
SUITS = [
    ("♠", (20, 20, 20)),
    ("♥", (200, 30, 30)),
    ("♣", (20, 20, 20)),
    ("♦", (200, 30, 30)),
]

CARD_W, CARD_H = 140, 190


def ensure_dirs() -> None:
    for sub in [
        "puker",
        "bgImg",
        "button",
        "menu",
        "defaultPortrait/man",
        "defaultPortrait/lady",
        "assets",
        "shop",
        "config",
        "audio",
    ]:
        (ROOT / sub).mkdir(parents=True, exist_ok=True)


def load_font(size: int, bold: bool = False) -> ImageFont.FreeTypeFont | ImageFont.ImageFont:
    candidates = [
        "C:/Windows/Fonts/msyhbd.ttc" if bold else "C:/Windows/Fonts/msyh.ttc",
        "C:/Windows/Fonts/segoeui.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf" if bold else "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
    ]
    for path in candidates:
        if os.path.exists(path):
            try:
                return ImageFont.truetype(path, size=size)
            except OSError:
                continue
    return ImageFont.load_default()


def draw_card(card_id: int) -> Image.Image:
    img = Image.new("RGBA", (CARD_W, CARD_H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)

    shadow = (20, 20, 20, 70)
    draw.rounded_rectangle((10, 10, CARD_W - 2, CARD_H - 2), radius=14, fill=shadow)
    draw.rounded_rectangle((4, 4, CARD_W - 8, CARD_H - 8), radius=14, fill=(252, 252, 255), outline=(180, 150, 60), width=3)
    draw.rounded_rectangle((8, 8, CARD_W - 12, CARD_H - 12), radius=12, outline=(230, 230, 235), width=1)

    if card_id >= 53:
        label = "小王" if card_id == 53 else "大王"
        color = (30, 120, 200) if card_id == 53 else (200, 40, 40)
        draw.ellipse((28, 36, CARD_W - 28, CARD_H - 36), outline=color, width=3)
        draw.text((CARD_W // 2, CARD_H // 2), label, fill=color, font=load_font(34, True), anchor="mm")
        return img

    rank_index = (card_id - 1) // 4
    suit_index = (card_id - 1) % 4
    rank = RANKS[rank_index]
    suit, color = SUITS[suit_index]

    rank_font = load_font(30, True)
    suit_font = load_font(26)
    center_font = load_font(58, True)

    draw.text((18, 14), rank, fill=color, font=rank_font)
    draw.text((18, 48), suit, fill=color, font=suit_font)
    draw.text((CARD_W // 2, CARD_H // 2 + 4), suit, fill=color, font=center_font, anchor="mm")
    draw.text((CARD_W - 18, CARD_H - 14), rank, fill=color, font=rank_font, anchor="rs")
    draw.text((CARD_W - 18, CARD_H - 48), suit, fill=color, font=suit_font, anchor="rs")
    return img


def draw_card_back() -> Image.Image:
    img = Image.new("RGBA", (CARD_W, CARD_H), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    draw.rounded_rectangle((10, 10, CARD_W - 2, CARD_H - 2), radius=14, fill=(20, 20, 20, 70))
    draw.rounded_rectangle((4, 4, CARD_W - 8, CARD_H - 8), radius=14, fill=(18, 58, 112), outline=(212, 175, 55), width=3)
    for y in range(14, CARD_H - 14, 16):
        for x in range(14, CARD_W - 14, 16):
            draw.rectangle((x, y, x + 10, y + 10), fill=(255, 255, 255, 28))
    draw.ellipse((34, 52, CARD_W - 34, CARD_H - 52), outline=(255, 255, 255, 90), width=2)
    draw.text((CARD_W // 2, CARD_H // 2), "UCC", fill=(255, 255, 255, 210), font=load_font(30, True), anchor="mm")
    return img


def save_felt_background(path: Path, size: tuple[int, int], base: tuple[int, int, int]) -> None:
    img = Image.new("RGB", size, base)
    draw = ImageDraw.Draw(img)
    for y in range(0, size[1], 4):
        shade = 8 if (y // 4) % 2 == 0 else -8
        c = tuple(max(0, min(255, base[i] + shade)) for i in range(3))
        draw.line((0, y, size[0], y), fill=c)
    for x in range(-size[1], size[0], 80):
        draw.line((x, 0, x + size[1], size[1]), fill=(0, 0, 0), width=1)
    img.save(path, "PNG" if path.suffix.lower() == ".png" else "JPEG", quality=92)


def save_button(path: Path, text: str, bg: tuple[int, int, int]) -> None:
    img = Image.new("RGBA", (300, 200), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    draw.rounded_rectangle((8, 8, 292, 192), radius=24, fill=bg + (255,), outline=(255, 255, 255, 120), width=2)
    draw.text((150, 100), text, fill=(255, 255, 255), font=load_font(28, True), anchor="mm")
    img.save(path, "PNG")


def save_portrait(path: Path, hue: int) -> None:
    img = Image.new("RGBA", (120, 120), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    draw.ellipse((4, 4, 116, 116), fill=(hue, 120, 180, 255), outline=(255, 255, 255, 200), width=3)
    draw.text((60, 60), "U", fill=(255, 255, 255), font=load_font(48, True), anchor="mm")
    img.save(path, "PNG")


def save_menu_icon(path: Path, label: str, color: tuple[int, int, int]) -> None:
    img = Image.new("RGBA", (160, 160), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    draw.rounded_rectangle((8, 8, 152, 152), radius=20, fill=color + (230,))
    draw.text((80, 80), label, fill=(255, 255, 255), font=load_font(22, True), anchor="mm")
    img.save(path, "PNG")


def save_wav(path: Path, freq: int, duration_ms: int = 120) -> None:
    import math
    import struct
    import wave

    sample_rate = 22050
    frames = int(sample_rate * duration_ms / 1000)
    with wave.open(str(path), "w") as wav:
        wav.setnchannels(1)
        wav.setsampwidth(2)
        wav.setframerate(sample_rate)
        for i in range(frames):
            value = int(12000 * math.sin(2 * math.pi * freq * (i / sample_rate)))
            wav.writeframes(struct.pack("<h", value))


def save_bomb_wav(path: Path) -> None:
    import math
    import random
    import struct
    import wave

    sample_rate = 44100
    duration_ms = 480
    frames = int(sample_rate * duration_ms / 1000)
    with wave.open(str(path), "w") as wav:
        wav.setnchannels(1)
        wav.setsampwidth(2)
        wav.setframerate(sample_rate)
        for i in range(frames):
            t = i / sample_rate
            env = math.exp(-t * 7.5)
            freq = 120 + 520 * math.exp(-t * 14)
            boom = math.sin(2 * math.pi * freq * t)
            noise = (random.random() * 2 - 1) * 0.35 * math.exp(-t * 18)
            value = int(18000 * env * (boom * 0.65 + noise))
            value = max(-32767, min(32767, value))
            wav.writeframes(struct.pack("<h", value))


def save_king_boom_wav(path: Path) -> None:
    import math
    import random
    import struct
    import wave

    sample_rate = 44100
    duration_ms = 720
    frames = int(sample_rate * duration_ms / 1000)
    with wave.open(str(path), "w") as wav:
        wav.setnchannels(1)
        wav.setsampwidth(2)
        wav.setframerate(sample_rate)
        for i in range(frames):
            t = i / sample_rate
            env = math.exp(-t * 5.2)
            low = math.sin(2 * math.pi * (90 + 80 * math.exp(-t * 8)) * t)
            mid = math.sin(2 * math.pi * (280 + 420 * math.exp(-t * 10)) * t) * 0.55
            sparkle = math.sin(2 * math.pi * (880 + t * 1200) * t) * 0.25 * math.exp(-t * 12)
            noise = (random.random() * 2 - 1) * 0.28 * math.exp(-t * 16)
            value = int(19000 * env * (low * 0.45 + mid + sparkle + noise))
            value = max(-32767, min(32767, value))
            wav.writeframes(struct.pack("<h", value))


def main() -> None:
    ensure_dirs()

    for i in range(1, 55):
        draw_card(i).save(ROOT / "puker" / f"{i}.png", "PNG")
    draw_card_back().save(ROOT / "puker" / "back.png", "PNG")

    save_felt_background(ROOT / "bgImg" / "startBgImg.png", (1920, 1080), (26, 71, 42))
    save_felt_background(ROOT / "bgImg" / "gameBgImg.png", (1920, 1080), (28, 96, 58))
    save_felt_background(ROOT / "assets" / "bg.jpg", (1920, 1080), (223, 221, 205))

    buttons = {
        "button_begin.png": ("开始游戏", (255, 143, 0)),
        "button_call.png": ("叫地主", (239, 108, 0)),
        "button_nocall.png": ("不  叫", (96, 125, 139)),
        "button_ok.png": ("出  牌", (46, 125, 50)),
        "button_pass.png": ("不  要", (84, 110, 122)),
        "button_tip.png": ("提  示", (2, 136, 209)),
        "button_leaveRoom.png": ("离开房间", (96, 125, 139)),
        "button_newGame.png": ("再来一局", (239, 108, 0)),
        "button_none.png": ("", (180, 180, 180)),
    }
    for name, (text, color) in buttons.items():
        save_button(ROOT / "button" / name, text, color)

    save_menu_icon(ROOT / "menu" / "menu_home.png", "首页", (76, 175, 80))
    save_menu_icon(ROOT / "menu" / "menu_setting.png", "设置", (96, 125, 139))
    save_menu_icon(ROOT / "menu" / "menu_shop.png", "商城", (255, 152, 0))
    save_menu_icon(ROOT / "menu" / "menu_friendMode.png", "好友", (63, 81, 181))
    save_menu_icon(ROOT / "menu" / "a.png", "单机", (67, 160, 71))
    save_menu_icon(ROOT / "menu" / "b.png", "经典", (25, 118, 210))

    for i in range(1, 11):
        save_portrait(ROOT / "defaultPortrait" / "man" / f"man_{i}.png", 80 + i * 10)
        save_portrait(ROOT / "defaultPortrait" / "lady" / f"lady_{i}.png", 180 + i * 8)

    Image.new("RGB", (64, 64), (255, 193, 7)).save(ROOT / "assets" / "egret_icon.png", "PNG")

    save_wav(ROOT / "audio" / "click.wav", 880, 80)
    save_wav(ROOT / "audio" / "win.wav", 660, 220)
    save_wav(ROOT / "audio" / "lose.wav", 220, 260)
    save_wav(ROOT / "audio" / "puker_deal.wav", 520, 60)
    save_bomb_wav(ROOT / "audio" / "boom_bomb.wav")
    save_king_boom_wav(ROOT / "audio" / "boom_king.wav")

    print(f"Generated assets under {ROOT}")


if __name__ == "__main__":
    main()
