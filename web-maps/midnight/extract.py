#!/usr/bin/env python3
"""Pull a game's detailed-map data + graphics out of the repo into JSON for template.html.

usage: extract.py <repo>/main/midnight <lom|ddr> <outdir>   (normally run by build.py)
"""
import base64, csv, io, json, plistlib, re, sys, xml.etree.ElementTree as ET
from pathlib import Path
from PIL import Image

ROOT = Path(sys.argv[1])            # .../main/midnight
GAME = sys.argv[2]
OUT = Path(sys.argv[3])
GAMES = {
    # res dir, scenario folder, config, page title, the land's name, whether a lord without a face uses the race face
    "lom": dict(res="Resources-midnight/lom", scn="16", tme="lom.tme", title="Lords of Midnight Map",
                land="the Land of Midnight", race_faces=False),
    "ddr": dict(res="Resources-revenge/ddr", scn="17", tme="ddr.tme", title="Doomdark's Revenge Map",
                land="the Icemark", race_faces=True,      # GetCharacterFace(): character face, else race face
                # mxloc::Serialize() turns DDR's frozen waste into icy waste as the map loads, so the border
                # (painted as frozen waste in the TMX) joins up with the icy wastes inside the land
                terrain_remap={"TN_FROZENWASTE": "TN_ICYWASTE"},
                # DDR has no domains: every square is named "the <terrain> of <area>", never
                # "... in the Domain of ...", even though the TMX still carries a Domain layer
                domains=False),
}
G = GAMES[GAME]
LOM = ROOT / G["res"]
SCN = LOM / G["scn"]

def tsv(name):
    with open(SCN / "tsv" / name, newline="", encoding="utf-8") as f:
        return list(csv.DictReader(f, delimiter="\t"))

def b64png(img):
    buf = io.BytesIO(); img.save(buf, "PNG", optimize=True)
    return "data:image/png;base64," + base64.b64encode(buf.getvalue()).decode()

# ---------------------------------------------------------------- TMX map
tmx = ET.parse(SCN / "tmx/map.tmx").getroot()
W, H = int(tmx.get("width")), int(tmx.get("height"))
gids = {ts.get("name"): int(ts.get("firstgid")) for ts in tmx.findall("tileset")}
def layer_data(el):
    d = el.find("data")
    if d.get("encoding") == "base64":
        import gzip, zlib, struct
        raw = base64.b64decode(d.text.strip())
        comp = d.get("compression")
        if comp == "gzip": raw = gzip.decompress(raw)
        elif comp == "zlib": raw = zlib.decompress(raw)
        return [v & 0x1FFFFFFF for v in struct.unpack("<%dI" % (len(raw) // 4), raw)]
    return [int(v) & 0x1FFFFFFF for v in re.split(r"[,\s]+", d.text.strip()) if v]

terrain = [0] * (W * H); area = [0] * (W * H); things = [0] * (W * H); flags = [0] * (W * H)
FLAG_BITS = {0: "domain", 1: "tunnel", 2: "mist", 3: "creature", 8: "impassable",
             10: "tunnel_entrance", 11: "tunnel_exit", 12: "tunnel_entrance_exit", 13: "tunnel_small"}
FLAG_MASK = {"domain": 1, "tunnel": 2, "mist": 4, "creature": 8, "impassable": 16,
             "tunnel_entrance": 32 | 2, "tunnel_exit": 64 | 2, "tunnel_entrance_exit": 96 | 2,
             "tunnel_small": 128 | 2}
for el in tmx.iter("layer"):
    cls, data = el.get("class"), layer_data(el)
    for i, t in enumerate(data):
        if not t: continue
        if cls == "Terrain": terrain[i] = t
        elif cls == "Area": area[i] = t - gids["areas"] + 1
        elif cls == "Things": things[i] = t - gids["things"] + 1
        elif cls == "Flags":
            f = FLAG_BITS.get(t - gids["flags"])
            if f: flags[i] |= FLAG_MASK[f]

if not G.get("domains", True):
    flags = [f & ~FLAG_MASK["domain"] for f in flags]

objgroups = {}
for g in tmx.iter("objectgroup"):
    objgroups[g.get("name")] = {o.get("name"): (int(float(o.get("x"))) // 64, int(float(o.get("y"))) // 64)
                                for o in g.iter("object")}

# ---------------------------------------------------------------- tables
terraininfo = {int(r["Id"]): r for r in tsv("terraininfo.tsv")}
areas = {int(r["Id"]): {"name": r["Name"], "prefix": r.get("Prefix", "")} for r in tsv("areainfo.tsv")}
strings = {r["Symbol"]: r["Text"] for r in tsv("strings.tsv")}
strings_by_id = {int(r["Id"]): r["Text"] for r in tsv("strings.tsv")}
objects = {int(r["Id"]): r for r in tsv("objects.tsv")}

_cfgtext = (LOM / G["tme"]).read_text()
class _El(dict):
    def get(self, k, d=None): return dict.get(self, k, d)
class _Cfg:
    def iter(self, tag):
        for m in re.finditer(r"<%s\s([^>]*?)/?>" % tag, _cfgtext):
            yield _El(re.findall(r'([\w]+)\s*=\s*"([^"]*)"', m.group(1)))
cfg = _Cfg()
term_sym = {r["Symbol"]: tid for tid, r in terraininfo.items()}
for src, dst in G.get("terrain_remap", {}).items():
    terrain = [term_sym[dst] if t == term_sym[src] else t for t in terrain]
obj_sym = {r["Symbol"]: oid for oid, r in objects.items()}

terrain_cells = {}
for t in cfg.iter("terrain"):
    tid = term_sym.get(t.get("id"))
    if tid is None: continue
    terrain_cells[tid] = {"cell": int(t.get("mapcell", "-1")) + 1,
                          "density": t.get("mapdensity", "false") == "true"}
object_cells = {}
for o in cfg.iter("object"):
    oid = obj_sym.get(o.get("id"))
    if oid is not None and o.get("mapcell"):
        object_cells[oid] = int(o.get("mapcell"))

# strongholds flag the map
strongholds = []
for r in tsv("strongholds.tsv"):
    loc = objgroups.get("strongholds", {}).get(r["Symbol"])
    if loc is None:
        loc = tuple(int(v) for v in r["Location"].split(","))
    strongholds.append({"symbol": r["Symbol"], "x": loc[0], "y": loc[1]})
    flags[loc[1] * W + loc[0]] |= 256          # lf_stronghold

faces = {c.get("id"): c.get("face") for c in cfg.iter("character")}
race_faces = {r.get("id"): r.get("face", "") for r in cfg.iter("race")} if G["race_faces"] else {}
chars = []
for r in tsv("characters.tsv"):
    if "ALIVE" not in r["Flags"] or "HIDDEN" in r["Flags"]: continue
    loc = objgroups.get("characters", {}).get(r["Symbol"])
    if loc is None:
        loc = tuple(int(v) for v in r["Location"].split(","))
    chars.append({"id": int(r["Id"]), "symbol": r["Symbol"], "name": r["Long Name"], "short": r["Short Name"],
                  "x": loc[0], "y": loc[1], "recruited": "RECRUITED" in r["Flags"],
                  "face": faces.get(r["Symbol"], "") or race_faces.get(r.get("Race", ""), "")})

# ---------------------------------------------------------------- graphics
images = {}
tiles = Image.open(LOM / "gfx_h/terrain/map_tiles.png").convert("RGBA")
images["tiles"] = b64png(tiles)

def load_frames(plist_path):
    with open(plist_path, "rb") as f:
        p = plistlib.load(f)
    sheet = Image.open(plist_path.with_name(p["metadata"]["textureFileName"])).convert("RGBA")
    return p["frames"], sheet

def nums(s): return [float(v) for v in re.findall(r"-?\d+(?:\.\d+)?", s)]

def cut(frames, sheet, name):
    fr = frames[name]
    x, y, w, h = nums(fr["textureRect"])
    img = sheet.crop((int(x), int(y), int(x + (h if fr["textureRotated"] else w)), int(y + (w if fr["textureRotated"] else h))))
    if fr["textureRotated"]:
        img = img.rotate(90, expand=True)
    sw, sh = nums(fr["spriteSourceSize"]); ox, oy = nums(fr["spriteOffset"])
    full = Image.new("RGBA", (int(sw), int(sh)))
    px = int(round((sw - w) / 2 + ox)); py = int(round((sh - h) / 2 - oy))
    full.paste(img, (px, py))
    return full

ui_names = ["i_center", "i_critters", "i_filter_lords", "i_filter_tunnel", "i_visible", "i_not_visible",
            "map_scale_up_button", "map_scale_down_button", "i_group_disband", "circle_selector",
            "map_lords_many", "f_dead!"]
fr_h, sh_h = load_frames(LOM / "gfx_h/rest-0.plist")
# TexturePacker tucks small icons into the empty corners of round ones, so the
# rectangle of a round frame also catches pieces of its neighbours: keep only the disc
ROUND = {"i_center", "i_critters", "i_filter_lords", "i_filter_tunnel", "map_scale_up_button",
         "map_scale_down_button", "i_group_disband", "circle_selector"}
from PIL import ImageDraw, ImageChops
for n in ui_names:
    im = cut(fr_h, sh_h, n)
    if n in ROUND:
        x, y, w, h = nums(fr_h[n]["textureRect"])
        ox, oy = nums(fr_h[n]["spriteOffset"])
        cx, cy = im.width / 2 + ox, im.height / 2 - oy
        r = min(w, h) / 2 + 0.5
        mask = Image.new("L", im.size, 0)
        ImageDraw.Draw(mask).ellipse((cx - r, cy - r, cx + r, cy + r), fill=255)
        im.putalpha(ImageChops.multiply(im.getchannel("A"), mask))
    images[n] = b64png(im)

# faces only exist at medium resolution
for c in chars:
    f = c["face"]
    if f and (LOM / "gfx_m" / f).exists():
        key = Path(f).stem
        if key not in images:
            images[key] = b64png(Image.open(LOM / "gfx_m" / f).convert("RGBA"))
        c["face"] = key
    else:
        c["face"] = ""      # the game draws nothing for a lord with no face

font = "data:font/ttf;base64," + base64.b64encode((ROOT / "Resources/fonts/Midnight.ttf").read_bytes()).decode()

terrain_text = {tid: {"name": r["Name"], "flags": r["Flags"], "desc": r["Description"]} for tid, r in terraininfo.items()}

data = {
    "w": W, "h": H, "terrain": terrain, "area": area, "things": things, "flags": flags,
    "terrainCells": terrain_cells, "objectCells": object_cells,
    "objects": {oid: r["Name"] for oid, r in objects.items()},
    "terrainInfo": terrain_text, "areas": areas,
    "strings": {k: strings[k] for k in ("SS_DEFAULT_TERRAIN", "SS_LOCATION_DOMAIN", "SS_LOCATION_NOTDOMAIN",
                                         "SS_TERRAIN_PLURAL", "SS_TERRAIN_SINGLE", "SS_AREA_PREFIXED")},
    "stringsById": {5: strings_by_id[5]},
    "characters": chars,
    "game": GAME, "land": G["land"],
    # place labels go on strongholds outside a domain and on these terrains (mapbuilder::updateTerrain)
    "placeTerrains": [term_sym[s] for s in ("TN_TOWER", "TN_WATCHTOWER", "TN_GATE") if s in term_sym],
    "critterMax": obj_sym["OB_WILDHORSES"],
    "current": "CH_LUXOR",
}
OUT.mkdir(parents=True, exist_ok=True)
(OUT / "data.json").write_text(json.dumps(data, separators=(",", ":")))
(OUT / "images.json").write_text(json.dumps(images, separators=(",", ":")))
(OUT / "font.txt").write_text(font)
(OUT / "title.txt").write_text(G["title"])
print("map", W, H, "chars", len(chars), "strongholds", len(strongholds),
      "things", sum(1 for t in things if t), "tunnels", sum(1 for f in flags if f & 2),
      "terrain ids", sorted(set(terrain)), "images", len(images))
