#!/usr/bin/env python3
"""Copy original art assets into image/ if an originals folder is provided."""

from __future__ import annotations

import shutil
from pathlib import Path

PROJECT = Path(__file__).resolve().parents[1]
TARGET = PROJECT / "image"
DEFAULT_SOURCE = PROJECT / "original_assets"


def copy_tree(source: Path) -> int:
    count = 0
    for file in source.rglob("*"):
        if not file.is_file():
            continue
        rel = file.relative_to(source)
        dest = TARGET / rel
        dest.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(file, dest)
        count += 1
    return count


def main() -> None:
    import sys

    source = Path(sys.argv[1]) if len(sys.argv) > 1 else DEFAULT_SOURCE
    if not source.exists():
        print(f"Original asset folder not found: {source}")
        print("Place PNG/JPG files under original_assets/ using the same layout as image/.")
        return

    copied = copy_tree(source)
    print(f"Imported {copied} files from {source} into {TARGET}")


if __name__ == "__main__":
    main()
