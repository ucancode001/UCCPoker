from pathlib import Path

root = Path(__file__).resolve().parents[1] / "image"
files = sorted(p for p in root.rglob("*") if p.is_file())
lines = ["<RCC>", '    <qresource prefix="/">']
for f in files:
    rel = f.relative_to(root.parent).as_posix()
    lines.append(f"        <file>{rel}</file>")
lines += ["    </qresource>", "</RCC>", ""]
Path(__file__).resolve().parents[1].joinpath("pocker.qrc").write_text("\n".join(lines), encoding="utf-8")
print(f"Wrote {len(files)} files to pocker.qrc")
