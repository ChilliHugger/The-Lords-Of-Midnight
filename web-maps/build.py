#!/usr/bin/env python3
"""Build the web versions of the games' detailed maps.

usage: build.py [--root <repo>/main/midnight] [--out <site dir>] [--inline] [game ...]

games: lom, ddr (default: both)

By default every graphic, the font and each map's data go into <out>/assets/ as separate files
whose names carry a hash of their contents (map_tiles.3f9a1c2e.png). A browser can then keep them
for good: the font and any graphics the maps share download once, and changing the page code only
re-downloads the small HTML. Each map is <out>/<slug>/index.html; <out>/index.html links them.

--inline writes each page as one self-contained HTML file instead, with everything embedded.

Needs Python 3.8+ and Pillow. With fontTools installed the font is also packed as WOFF.
"""
import argparse, base64, hashlib, html, io, json, re, shutil, subprocess, sys, tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
GAMES = {
    "lom": dict(slug="lords-of-midnight", name="The Lords of Midnight",
                blurb="Explore the Land of Midnight on the game's own detailed map."),
    "ddr": dict(slug="doomdarks-revenge", name="Doomdark's Revenge",
                blurb="Explore the Icemark, its tunnels and its lords on the game's own detailed map."),
}
ICON = "map_lords_many"           # browser-tab icon: the game's many-lords shield
PRELOAD = ["tiles", "circle_selector"]   # images the map needs before it can draw


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


class Assets:
    """Writes files into <out>/assets/ under content-hashed names, once each."""
    EXT = {"image/png": "png", "font/woff": "woff", "font/ttf": "ttf", "text/javascript": "js"}

    def __init__(self, out):
        self.dir = out / "assets"
        self.dir.mkdir(parents=True, exist_ok=True)
        self.written = set()

    def add(self, name, mime, raw):
        stem = re.sub(r"[^A-Za-z0-9_-]+", "-", name).strip("-")
        fname = f"{stem}.{hashlib.sha256(raw).hexdigest()[:8]}.{self.EXT[mime]}"
        path = self.dir / fname
        if not path.exists():
            path.write_bytes(raw)
        self.written.add(fname)
        return "assets/" + fname          # relative to <out>


def pack_font(ttf):
    """WOFF when fontTools is available (about half the size of the TTF), else the TTF."""
    try:
        from fontTools.ttLib import TTFont
    except ImportError:
        return "font/ttf", "truetype", ttf
    f = TTFont(io.BytesIO(ttf))
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


def build_game(root, game, out, assets, inline):
    g = GAMES[game]
    with tempfile.TemporaryDirectory() as tmp:
        data, images, ttf, title = extract(root, game, Path(tmp))
    font_mime, font_format, font = pack_font(ttf)

    if inline:
        urls = images
        font_url = to_data_uri(font_mime, font)
        data_script = "<script>" + js_assign({"data": data, "images": urls}) + "</script>"
        head_extra = ""
    else:
        # pages sit one folder down (<slug>/index.html), so asset URLs start with ../
        urls = {k: "../" + assets.add(k, *from_data_uri(v)) for k, v in images.items()}
        font_url = "../" + assets.add("Midnight", font_mime, font)
        js = js_assign({"data": data, "images": urls}).encode()
        data_script = f'<script src="../{assets.add(g["slug"] + "-data", "text/javascript", js)}"></script>'
        head_extra = "\n".join(
            [f'<link rel="preload" href="{font_url}" as="font" type="{font_mime}" crossorigin>',
             f'<link rel="preload" href="{data_script.split(chr(34))[1]}" as="script">']
            + [f'<link rel="preload" href="{urls[k]}" as="image">' for k in PRELOAD])

    page = fill((HERE / "template.html").read_text(encoding="utf-8"),
                TITLE=html.escape(title), DESCRIPTION=html.escape(g["blurb"]), ICON=urls[ICON],
                HEAD_EXTRA=head_extra, FONT=font_url, FONT_FORMAT=font_format, DATA_SCRIPT=data_script)
    dest = out / g["slug"] / "index.html"
    dest.parent.mkdir(parents=True, exist_ok=True)
    dest.write_text(page, encoding="utf-8")
    print(f"{dest}  {len(page.encode()) // 1024} KB")
    # the landing page reuses this game's font and icon
    return dict(mime=font_mime, format=font_format, font=font, icon=images[ICON])


def build_index(out, games, assets, inline, shared):
    links = "\n".join(
        f'    <li><a href="{GAMES[g]["slug"]}/"><span class="name">{html.escape(GAMES[g]["name"])}</span>'
        f'<span class="blurb">{html.escape(GAMES[g]["blurb"])}</span></a></li>'
        for g in games)
    if inline:
        font_url, icon_url, head_extra = to_data_uri(shared["mime"], shared["font"]), shared["icon"], ""
    else:
        font_url = assets.add("Midnight", shared["mime"], shared["font"])
        icon_url = assets.add(ICON, *from_data_uri(shared["icon"]))
        head_extra = f'<link rel="preload" href="{font_url}" as="font" type="{shared["mime"]}" crossorigin>'
    page = fill((HERE / "index-template.html").read_text(encoding="utf-8"),
                FONT=font_url, FONT_FORMAT=shared["format"], ICON=icon_url, HEAD_EXTRA=head_extra, LINKS=links)
    (out / "index.html").write_text(page, encoding="utf-8")
    print(out / "index.html")


def prune(out):
    """Remove hashed files nothing in the site refers to any more (left over from earlier builds)."""
    adir = out / "assets"
    if not adir.is_dir():
        return
    used = set()
    for f in [*out.glob("*.html"), *out.glob("*/index.html"), *adir.glob("*.js")]:
        used.update(re.findall(r"assets/([A-Za-z0-9_.-]+)", f.read_text(encoding="utf-8")))
    for f in adir.iterdir():
        if f.name not in used:
            f.unlink()
            print("removed", f)
    if not any(adir.iterdir()):
        adir.rmdir()


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--root", type=Path, help="the repo's main/midnight folder (found automatically when this "
                                              "script lives inside the repo)")
    ap.add_argument("--out", type=Path, default=HERE / "site", help="where to write the site (default: ./site)")
    ap.add_argument("--inline", action="store_true", help="embed everything in each page instead of assets/")
    ap.add_argument("games", nargs="*", help="lom, ddr (default: both)")
    a = ap.parse_args()
    root = a.root or find_root()
    games = a.games or list(GAMES)
    for g in games:
        if g not in GAMES:
            ap.error(f"unknown game '{g}': choose from {', '.join(GAMES)}")
    a.out.mkdir(parents=True, exist_ok=True)
    assets = None if a.inline else Assets(a.out)
    shared = None
    for g in games:
        shared = build_game(root, g, a.out, assets, a.inline)
    build_index(a.out, [g for g in GAMES if (a.out / GAMES[g]["slug"] / "index.html").exists()],
                assets, a.inline, shared)
    prune(a.out)


if __name__ == "__main__":
    main()
