#!/usr/bin/env python3
"""Build the web versions of the games' detailed maps.

usage: build.py [--root <repo>/main/midnight] [--out <dir>] [--inline] [game ...]

games: lom, ddr (default: both)

Each game becomes its own complete site, ready to upload to its own server:

    <out>/lords-of-midnight/index.html + assets/
    <out>/doomdarks-revenge/index.html + assets/

assets/ holds the tile sheet, icons, faces, font (Midnight.woff) and the map's data (data.js).
A game's assets folder is cleared and rewritten each time that game is built.

--inline writes each map as a single index.html with everything embedded instead.

Needs Python 3.8+ and Pillow. With fontTools installed the font is packed as WOFF, else TTF.
"""
import argparse, base64, html, io, json, re, shutil, subprocess, sys, tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
GAMES = {
    "lom": dict(slug="lords-of-midnight",
                blurb="Explore the Land of Midnight on the game's own detailed map."),
    "ddr": dict(slug="doomdarks-revenge",
                blurb="Explore the Icemark, its tunnels and its lords on the game's own detailed map."),
}
ICON = "map_lords_many"                  # browser-tab icon: the game's many-lords shield
PRELOAD = ["tiles", "circle_selector"]   # images the map needs before it can draw
EXT = {"image/png": "png", "font/woff": "woff", "font/ttf": "ttf"}


def find_root():
    """Walk up from this script to the repo's main/midnight folder."""
    for d in [HERE, *HERE.parents]:
        if (d / "main/midnight/Resources-midnight").is_dir():
            return d / "main/midnight"
    sys.exit("Can't find main/midnight above this script: pass --root <repo>/main/midnight")


def from_data_uri(uri):
    head, b64 = uri.split(",", 1)
    return head[5:].split(";")[0], base64.b64decode(b64)


def to_data_uri(mime, raw):
    return f"data:{mime};base64," + base64.b64encode(raw).decode()


def js_assign(obj):
    # "</" can't appear inside an inline <script>
    return "window.MIDNIGHT_MAP = " + json.dumps(obj, separators=(",", ":")).replace("</", "<\\/") + ";\n"


def file_name(name, mime):
    return re.sub(r"[^A-Za-z0-9_-]+", "-", name).strip("-") + "." + EXT[mime]


def pack_font(ttf):
    """WOFF when fontTools is available (about half the size of the TTF), else the TTF."""
    try:
        from fontTools.ttLib import TTFont
    except ImportError:
        return "font/ttf", "truetype", ttf
    f = TTFont(io.BytesIO(ttf), recalcTimestamp=False)   # same bytes every build
    f.flavor = "woff"
    buf = io.BytesIO()
    f.save(buf)
    return "font/woff", "woff", buf.getvalue()


def extract(root, game, work):
    subprocess.run([sys.executable, "-I", str(HERE / "extract.py"), str(root), game, str(work)], check=True)
    data = json.loads((work / "data.json").read_text(encoding="utf-8"))
    images = json.loads((work / "images.json").read_text(encoding="utf-8"))
    _, ttf = from_data_uri((work / "font.txt").read_text(encoding="utf-8"))
    title = (work / "title.txt").read_text(encoding="utf-8")
    return data, images, ttf, title


def fill(page, **values):
    for k, v in values.items():
        page = page.replace(f"__{k}__", v)
    left = re.findall(r"__[A-Z_]+__", page)
    if left:
        sys.exit(f"template placeholders not filled: {sorted(set(left))}")
    return page


def build_game(root, game, out, inline):
    g = GAMES[game]
    site = out / g["slug"]
    with tempfile.TemporaryDirectory() as tmp:
        data, images, ttf, title = extract(root, game, Path(tmp))
    font_mime, font_format, font = pack_font(ttf)

    # start the site clean, so nothing from an earlier build is left behind
    if site.exists():
        shutil.rmtree(site)
    site.mkdir(parents=True)

    if inline:
        urls = images
        font_url = to_data_uri(font_mime, font)
        data_script = "<script>" + js_assign({"data": data, "images": urls}) + "</script>"
        head_extra = ""
    else:
        adir = site / "assets"
        adir.mkdir()
        urls = {}
        for name, uri in images.items():
            mime, raw = from_data_uri(uri)
            fname = file_name(name, mime)
            (adir / fname).write_bytes(raw)
            urls[name] = "assets/" + fname
        font_url = "assets/" + file_name("Midnight", font_mime)
        (site / font_url).write_bytes(font)
        (adir / "data.js").write_text(js_assign({"data": data, "images": urls}), encoding="utf-8")
        data_script = '<script src="assets/data.js"></script>'
        head_extra = "\n".join(
            # no font preload: a crossorigin preload fails when the page is opened from disk, and the
            # @font-face in the head is found just as early
            ['<link rel="preload" href="assets/data.js" as="script">']
            + [f'<link rel="preload" href="{urls[k]}" as="image">' for k in PRELOAD])

    page = fill((HERE / "template.html").read_text(encoding="utf-8"),
                TITLE=html.escape(title), DESCRIPTION=html.escape(g["blurb"]), ICON=urls[ICON],
                HEAD_EXTRA=head_extra, FONT=font_url, FONT_FORMAT=font_format, DATA_SCRIPT=data_script)
    (site / "index.html").write_text(page, encoding="utf-8")
    files = [p for p in site.rglob("*") if p.is_file()]
    print(f"{site}/  {len(files)} files, {sum(p.stat().st_size for p in files) // 1024} KB")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--root", type=Path, help="the repo's main/midnight folder (found automatically when this "
                                              "script lives inside the repo)")
    ap.add_argument("--out", type=Path, default=HERE / "site", help="where to write the sites (default: ./site)")
    ap.add_argument("--inline", action="store_true", help="embed everything in each index.html instead of assets/")
    ap.add_argument("games", nargs="*", help="lom, ddr (default: both)")
    a = ap.parse_args()
    root = a.root or find_root()
    games = a.games or list(GAMES)
    for g in games:
        if g not in GAMES:
            ap.error(f"unknown game '{g}': choose from {', '.join(GAMES)}")
    a.out.mkdir(parents=True, exist_ok=True)
    for g in games:
        build_game(root, g, a.out, a.inline)


if __name__ == "__main__":
    main()
