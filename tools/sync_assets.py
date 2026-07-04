#!/usr/bin/env python3
"""Import original Egret/Qt art assets and regenerate qrc."""

from __future__ import annotations

import shutil
import subprocess
import sys
from pathlib import Path

PROJECT = Path(__file__).resolve().parents[1]
TARGET = PROJECT / "image"
TOOLS = PROJECT / "tools"

# Egret Check/resource layout -> Qt image/ layout
CHECK_MAPPINGS = (
    ("Check/resource/puker", "puker"),
    ("Check/resource/bgImg", "bgImg"),
    ("Check/resource/button", "button"),
    ("Check/resource/menu", "menu"),
    ("Check/resource/defaultPortrait/man", "defaultPortrait/man"),
    ("Check/resource/defaultPortrait/lady", "defaultPortrait/lady"),
    ("Check/resource/assets", "assets"),
    ("Check/resource/shop", "shop"),
    ("Check/resource/audio", "audio"),
    ("Check/resource/config", "config"),
)

SUPPORTED_EXT = {".png", ".jpg", ".jpeg", ".wav", ".mp3", ".json"}


def copy_files(source: Path, dest_root: Path, keep_relative: bool = True) -> int:
    count = 0
    if not source.exists():
        return 0
    for file in source.rglob("*"):
        if not file.is_file() or file.suffix.lower() not in SUPPORTED_EXT:
            continue
        rel = file.relative_to(source) if keep_relative else Path(file.name)
        dest = dest_root / rel
        dest.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(file, dest)
        count += 1
    return count


def import_original_assets(source: Path) -> int:
    return copy_files(source, TARGET)


def import_check_resources() -> int:
    total = 0
    for src_rel, dst_rel in CHECK_MAPPINGS:
        src = PROJECT / src_rel
        if not src.exists():
            continue
        total += copy_files(src, TARGET / dst_rel, keep_relative=True)
    return total


def run_generate_assets() -> None:
    script = TOOLS / "generate_assets.py"
    if script.exists():
        subprocess.check_call([sys.executable, str(script)], cwd=PROJECT)


def run_generate_qrc() -> None:
    script = TOOLS / "generate_qrc.py"
    if script.exists():
        subprocess.check_call([sys.executable, str(script)], cwd=PROJECT)


def main() -> None:
    args = sys.argv[1:]
    force_generate = "--generate" in args
    args = [a for a in args if a != "--generate"]

    source = Path(args[0]) if args else PROJECT / "original_assets"
    copied = 0

    if source.exists():
        copied += import_original_assets(source)
        print(f"Imported {copied} files from {source}")

    check_copied = import_check_resources()
    if check_copied:
        copied += check_copied
        print(f"Imported {check_copied} files from Check/resource")

    if copied == 0 or force_generate:
        print("Generating fallback/enhanced assets...")
        run_generate_assets()
        copied = sum(1 for _ in TARGET.rglob("*") if _.is_file())
        print(f"Asset tree contains {copied} files under image/")

    run_generate_qrc()
    print("Updated pocker.qrc")


if __name__ == "__main__":
    main()
