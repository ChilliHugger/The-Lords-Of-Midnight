"""Build the Midnight Engine database + map for The Lords of Midnight III: The Citadel.

Inputs (all shipped in the repo):
  citadel/tiled/citadel_wip.tmx     - the 256x256 map, painted in Tiled
  citadel/data/citadel_*.txt        - the 1995 LOM3 game data, dumped by tools/exporter
  tools/exporter/.../main.cpp       - the race / realm / land name tables
  The Citadel - Design.pdf          - the lore, and the win/lose conditions

Outputs:
  Resources-citadel/citadel/18/map
  Resources-citadel/citadel/18/database
"""
import os
import tmedb
import citadel_data as cd
import citadel_names

import os as _os
# repo root, two levels up from tools/citadel_build/
ROOT = _os.path.abspath(_os.path.join(_os.path.dirname(_os.path.abspath(__file__)), "..", ".."))

W = H = 256
SCENARIO_ID = 18          # mxscenarioid::CITADEL
DB_VERSION = 7            # DATABASEVERSION the engine accepts
MAP_VERSION = 2

OUT = _os.path.join(ROOT, "main", "midnight", "Resources-citadel", "citadel", "18")

# --------------------------------------------------------------------------
# terrain codes (tme::mxterrain_t)
TN_PLAINS, TN_CITADEL, TN_FOREST, TN_HENGE, TN_TOWER = 0, 1, 2, 3, 4
TN_VILLAGE, TN_DOWNS, TN_KEEP, TN_SNOWHALL, TN_LAKE = 5, 6, 7, 8, 9
TN_FROZENWASTE, TN_RUIN, TN_LITH, TN_CAVERN, TN_MOUNTAIN = 10, 11, 12, 13, 14
TN_ARMY = 15
TN_GATE, TN_TEMPLE, TN_PIT, TN_PALACE = 20, 21, 22, 23
TN_FORTRESS, TN_HALL, TN_HUT, TN_WATCHTOWER = 24, 25, 26, 27
TN_CITY, TN_FOUNTAIN, TN_STONES, TN_ICYWASTE = 28, 29, 30, 31
TN_LAND, TN_ISLE, TN_LAKELAND, TN_PLAIN, TN_PLAINS3 = 33, 34, 35, 36, 37
TN_FOREST3, TN_TREES, TN_MOUNTAIN3, TN_ICY_MOUNTAIN = 38, 40, 41, 42
TN_DOWNS3, TN_HILLS3, TN_FOOTHILLS, TN_VALLEY = 43, 44, 45, 46
TN_BAY, TN_SEA, TN_RIVER, TN_MARSH, TN_LAKE3 = 47, 48, 49, 50, 51
MAX_TERRAIN = 52

# location flags
lf_mist, lf_special, lf_impassable, lf_stronghold = 1 << 0, 1 << 7, 1 << 8, 1 << 9

# terrain info flags
tif_plural, tif_block, tif_interesting, tif_army = 1, 2, 4, 8

# character flags
cf_hidden, cf_riding, cf_alive, cf_recruited = 1, 2, 4, 8
cf_army, cf_hide, cf_horse = 16, 32, 64
cf_allowedwarriors, cf_allowedriders = 1 << 10, 1 << 11
cf_ai = 1 << 12

# object flags
of_fight, of_pickup, of_drop, of_weapon = 1, 2, 4, 8
of_see, of_remove, of_unique = 16, 32, 64

# id types
IDT_OBJECT, IDT_RACEINFO, IDT_CHARACTER = 5, 6, 10
IDT_STRONGHOLD, IDT_PLACE, IDT_MISSION = 14, 22, 23


def idt(t, i):
    return (t << 24) | i


# --------------------------------------------------------------------------
# races: index 0 must stay "none", 2 is used as "the free" by the engine
# (memory / stronghold change-sides), and 3 carries the engine's fey forest
# affinity, so the Citadel races are seated to match.
RACES = [
    # (symbol,          display name,     citadel race index, soldiers name)
    ("RA_NONE",         "None",           None, ""),
    ("RA_DARKFEY",      "Dark Fey",       12,   "Dark Fey"),
    ("RA_FREE",         "Free",           15,   "Free"),
    ("RA_HIGHFEY",      "High Fey",       6,    "High Fey"),
    ("RA_KITH",         "Kith",           0,    "Kith"),
    ("RA_ATHELING",     "Atheling",       1,    "Athelings"),
    ("RA_ELDRIN",       "Eldrin",         2,    "Eldrin"),
    ("RA_LONGDWARF",    "Long Dwarf",     3,    "Long Dwarves"),
    ("RA_ARAKAI",       "Arakai",         4,    "Arakai"),
    ("RA_DRAGONLORD",   "Dragonlord",     5,    "Dragonlords"),
    ("RA_DAWNFEY",      "Dawn Fey",       7,    "Dawn Fey"),
    ("RA_USKARG",       "Uskarg",         8,    "Uskarg"),
    ("RA_GELMING",      "Gelming",        9,    "Gelmings"),
    ("RA_DEEPINGDWARF", "Deeping Dwarf",  10,   "Deeping Dwarves"),
    ("RA_GIANT",        "Giant",          11,   "Giants"),
    ("RA_GOLDENFEY",    "Golden Fey",     13,   "Golden Fey"),
    ("RA_CORSAIR",      "Corsair",        14,   "Corsairs"),
]
RA_NONE, RA_DARKFEY, RA_FREE = 0, 1, 2

CIT_RACE_NAMES = citadel_names.table("text_races")
# citadel race name -> tme race id
RACE_BY_NAME = {CIT_RACE_NAMES[c]: i for i, (_, _, c, _) in enumerate(RACES) if c is not None}

# races that cannot cross mountains are handled by the engine via
# RA_DWARF/RA_GIANT/RA_DRAGON, which do not exist here; impassable mountains
# is a rule the player opts into, so every race gets ordinary movement.
RACE_STATS = {
    # symbol: (success, initialmovement, diagonal, ridingx10000, movemax, baserest)
    "default": (0, 2, 0, 10000, 255, 8),
}


def race_row(i):
    sym, name, cit, soldiers = RACES[i]
    s, im, dg, rm, mm, br = RACE_STATS["default"]
    return {
        "symbol": sym, "flags": 0, "name": name, "soldiersname": soldiers,
        "success": s, "initialmovement": im, "diagonalmodifier": dg,
        "ridingmultiplier": rm, "movementmax": mm, "baserestamount": br,
        "strongholdstartups": 100,
        "misttimeaffect": 0, "mistdespondecyaffect": 0,
        "baseenergycost": 2, "baseenergycosthorse": 1,
    }


# --------------------------------------------------------------------------
# terrain table. success/visibility/obstruction/movementcost follow the LOM
# values for the equivalent terrain; the Citadel-only terrains are seated
# between the nearest LOM neighbours.
#   sym, name, prep, description, success, vis, obs, cost, flags
T_DEF = "{str:5}"
TERRAINS = {
    TN_PLAINS:      ("TN_PLAINS",      "Plain",        "on",     T_DEF, 0, 3, 1, 0, tif_plural | tif_army),
    TN_CITADEL:     ("TN_CITADEL",     "Citadel",      "at",     T_DEF, 32, 14, 14, 0, tif_interesting),
    TN_FOREST:      ("TN_FOREST",      "Forest",       "in",     T_DEF, 0, 12, 13, 3, tif_plural),
    TN_HENGE:       ("TN_HENGE",       "Henge",        "at",     "{area:name}henge", 0, 12, 12, 0, tif_interesting),
    TN_TOWER:       ("TN_TOWER",       "Tower",        "at",     T_DEF, 0, 15, 7, 0, tif_interesting),
    TN_VILLAGE:     ("TN_VILLAGE",     "Village",      "in",     T_DEF, 0, 10, 8, 0, tif_interesting),
    TN_DOWNS:       ("TN_DOWNS",       "Down",         "on",     T_DEF, 0, 5, 4, 1, tif_plural | tif_army),
    TN_KEEP:        ("TN_KEEP",        "Keep",         "at",     T_DEF, 16, 10, 10, 0, tif_interesting),
    TN_SNOWHALL:    ("TN_SNOWHALL",    "Snowhall",     "at",     T_DEF, 0, 6, 4, 0, tif_interesting),
    TN_LAKE:        ("TN_LAKE",        "Lake",         "beside", "Lake {area:name}", 0, 4, 6, 0, tif_interesting),
    TN_FROZENWASTE: ("TN_FROZENWASTE", "Frozen Waste", "in",     "the Frozen Wastes", 0, 12, 12, 0, 0),
    TN_RUIN:        ("TN_RUIN",        "Ruin",         "in",     T_DEF, 0, 8, 4, 0, tif_interesting),
    TN_LITH:        ("TN_LITH",        "Lith",         "at",     T_DEF, 0, 10, 6, 0, tif_interesting),
    TN_CAVERN:      ("TN_CAVERN",      "Cavern",       "at",     T_DEF, 0, 2, 6, 0, tif_interesting),
    TN_MOUNTAIN:    ("TN_MOUNTAIN",    "Mountain",     "in",     T_DEF, 0, 16, 16, 4, tif_plural),
    TN_ARMY:        ("TN_ARMY",        "Army",         "",       "", 0, 8, 8, 0, 0),
    TN_GATE:        ("TN_GATE",        "Gate",         "at",     T_DEF, 0, 10, 6, 0, tif_interesting),
    TN_TEMPLE:      ("TN_TEMPLE",      "Temple",       "at",     T_DEF, 0, 10, 6, 0, tif_interesting),
    TN_PIT:         ("TN_PIT",         "Pit",          "at",     T_DEF, 0, 8, 4, 0, tif_interesting),
    TN_PALACE:      ("TN_PALACE",      "Palace",       "at",     T_DEF, 8, 12, 10, 0, tif_interesting),
    TN_FORTRESS:    ("TN_FORTRESS",    "Fortress",     "at",     T_DEF, 24, 12, 12, 0, tif_interesting),
    TN_HALL:        ("TN_HALL",        "Hall",         "at",     T_DEF, 0, 8, 6, 0, tif_interesting),
    TN_HUT:         ("TN_HUT",         "Hut",          "at",     T_DEF, 0, 6, 4, 0, tif_interesting),
    TN_WATCHTOWER:  ("TN_WATCHTOWER",  "Watchtower",   "at",     T_DEF, 0, 15, 7, 0, tif_interesting),
    TN_CITY:        ("TN_CITY",        "City",         "in",     T_DEF, 16, 12, 12, 0, tif_interesting),
    TN_FOUNTAIN:    ("TN_FOUNTAIN",    "Fountain",     "at",     T_DEF, 0, 8, 4, 0, tif_interesting),
    TN_STONES:      ("TN_STONES",      "Stone",        "at",     T_DEF, 0, 10, 6, 0, tif_interesting),
    TN_ICYWASTE:    ("TN_ICYWASTE",    "Frozen Waste", "in",     "the Frozen Wastes", 0, 12, 12, 0, 0),
    # ---- the Citadel terrains ----
    TN_LAND:        ("TN_LAND",        "Land",         "on",     T_DEF, 0, 3, 1, 0, tif_plural | tif_army),
    TN_ISLE:        ("TN_ISLE",        "Isle",         "on",     T_DEF, 0, 4, 2, 0, tif_army),
    TN_LAKELAND:    ("TN_LAKELAND",    "Lakeland",     "in",     T_DEF, 0, 4, 3, 1, tif_plural | tif_army),
    TN_PLAIN:       ("TN_PLAIN",       "Plain",        "on",     T_DEF, 0, 3, 1, 0, tif_plural | tif_army),
    TN_PLAINS3:     ("TN_PLAINS3",     "Plain",        "on",     T_DEF, 0, 3, 1, 0, tif_plural | tif_army),
    TN_FOREST3:     ("TN_FOREST3",     "Forest",       "in",     T_DEF, 0, 12, 13, 3, tif_plural),
    TN_TREES:       ("TN_TREES",       "Tree",        "among",  T_DEF, 0, 9, 9, 2, tif_plural),
    TN_MOUNTAIN3:   ("TN_MOUNTAIN3",   "Mountain",     "in",     T_DEF, 0, 16, 16, 4, tif_plural),
    TN_ICY_MOUNTAIN: ("TN_ICY_MOUNTAIN", "Icy Mountain", "in",   T_DEF, 0, 16, 16, 5, tif_plural),
    TN_DOWNS3:      ("TN_DOWNS3",      "Down",         "on",     T_DEF, 0, 5, 4, 1, tif_plural | tif_army),
    TN_HILLS3:      ("TN_HILLS3",      "Hill",         "in",     T_DEF, 0, 8, 6, 2, tif_plural),
    TN_FOOTHILLS:   ("TN_FOOTHILLS",   "Foothill",     "in",     T_DEF, 0, 6, 5, 2, tif_plural),
    TN_VALLEY:      ("TN_VALLEY",      "Valley",       "in",     T_DEF, 0, 6, 5, 1, tif_plural),
    TN_BAY:         ("TN_BAY",         "Bay",          "beside", "the Bay of {area:name}", 0, 4, 6, 0, tif_block),
    TN_SEA:         ("TN_SEA",         "Sea",          "beside", "the Sea of {area:name}", 0, 3, 3, 0, tif_block),
    TN_RIVER:       ("TN_RIVER",       "River",        "beside", "the River {area:name}", 0, 4, 4, 2, 0),
    TN_MARSH:       ("TN_MARSH",       "Marsh",        "in",     T_DEF, 0, 6, 8, 3, 0),
    TN_LAKE3:       ("TN_LAKE3",       "Lake",         "beside", "Lake {area:name}", 0, 4, 6, 0, tif_block),
}


def terrain_rows():
    rows = []
    for t in range(MAX_TERRAIN):
        if t in TERRAINS:
            sym, name, prep, desc, succ, vis, obs, cost, fl = TERRAINS[t]
        else:
            sym, name, prep, desc = "TN_UNUSED_%d" % t, "", "", ""
            succ = vis = obs = cost = fl = 0
        rows.append((t, {
            "symbol": sym, "flags": fl, "name": name, "preposition": prep,
            "description": desc, "success": succ, "visibility": vis,
            "obstruction": obs, "movementcost": cost,
        }))
    return rows


# --------------------------------------------------------------------------
# The map: terrain layers are painted bottom-up in the order Tiled stores
# them, and each layer already holds the engine's own terrain code.
TERRAIN_LAYERS = [
    "land (32)", "sea (47)", "bay (46)", "isle (33)", "lake (50)",
    "lakeland (34)", "plains (36)", "river (48)", "mountains (40,41)",
    "downs (42)", "swamp (49)", "valley (45)", "hills (43)",
    "foothills (44)", "forest (37)", "Trees (39)", "plain (35)",
]
AREA_GID = 177      # firstgid of the "areas" tileset
REALM_GID = 1201


def build_map(strongholds, places, objects_on_map):
    terrain = [TN_SEA] * (W * H)
    for name in TERRAIN_LAYERS:
        lay = cd.layer(name)
        for i, v in enumerate(lay):
            if v:
                terrain[i] = v

    area = [(g - AREA_GID) & 0x3ff for g in cd.layer("regions")]

    flags = [0] * (W * H)
    for i, v in enumerate(cd.layer("border.txt")):
        if v:
            flags[i] |= lf_mist

    # decorative places (henges, liths, ruins ...) sit on top of the terrain
    for (x, y, t) in places:
        terrain[y * W + x] = t
        flags[y * W + x] |= lf_special

    # strongholds last: a castle or citadel replaces whatever it stands on
    for (x, y, t) in strongholds:
        terrain[y * W + x] = t
        flags[y * W + x] |= lf_stronghold | lf_special

    obj = [0] * (W * H)
    for (x, y, oid) in objects_on_map:
        obj[y * W + x] = oid

    cells = [
        tmedb.cell(terrain=terrain[i], area=area[i], obj=obj[i], flags=flags[i])
        for i in range(W * H)
    ]
    return {"version": MAP_VERSION, "header": tmedb.MAP_HEADER,
            "w": W, "h": H, "flags": None, "cells": cells}, terrain


# --------------------------------------------------------------------------
def symbolise(text, prefix, used):
    s = "".join(ch.upper() if ch.isalnum() else "_" for ch in text).strip("_")
    while "__" in s:
        s = s.replace("__", "_")
    base = prefix + s
    out, n = base, 2
    while out in used:
        out, n = "%s_%d" % (base, n), n + 1
    used.add(out)
    return out


def main():
    areas_names = citadel_names.table("text_areas")
    chars = cd.records("citadel_character.txt", "character")
    builds = cd.records("citadel_buildings.txt", "building")
    beasts = cd.records("citadel_beasts.txt", "beast")
    objs = cd.records("citadel_objects.txt", "object")

    onmap = lambda r: 0 <= int(r["x"]) < W and 0 <= int(r["y"]) < H

    # ---------------- characters ----------------
    char_rows = []
    sym_to_id = {}
    live = []
    for c in chars:
        if "+dead" in c.get("char_flags_txt", ""):
            continue
        sym = c["symbol"] or ("CH_" + c["Name"].upper())
        if sym in sym_to_id:
            continue
        sym_to_id[sym] = len(live) + 1
        live.append((sym, c))

    for sym, c in live:
        cflags = int(c["char_flags_txt_value"] or 0)
        oflags = int(c["flags_txt_value"] or 0)
        race = RACE_BY_NAME.get(c["allegiance"] or c["army_race"], RA_FREE)
        female = bool(oflags & 32768)
        recruited = bool(cflags & 1)      # LOM3 recruit_bit
        army = int(c["army_size"] or 0)
        f = cf_alive | cf_army | cf_hide | cf_horse | cf_allowedwarriors | cf_allowedriders
        if recruited:
            f |= cf_recruited
        else:
            f |= cf_ai
        title = (c["Title"] or "").strip()
        char_rows.append((len(char_rows) + 1, {
            "symbol": sym, "flags": f,
            "location": (int(c["x"]), int(c["y"])),
            "longname": (c["Name"] + " " + title).strip(),
            "shortname": c["Name"],
            "looking": {"NORTH": 0, "NORTH_EAST": 1, "EAST": 2, "SOUTH_EAST": 3,
                        "SOUTH": 4, "SOUTH_WEST": 5, "WEST": 6,
                        "NORTH_WEST": 7}.get(c["direction"], 0),
            "time": 16,
            "riders": [0, 127, 0, 0],
            "warriors": [army, 127, 0, 0],
            "battleloc": (-1, -1), "battleslew": 0,
            "reckless": 180, "energy": 127,
            "strength": max(4, min(30, int(c["combat_power"] or 128) // 8)),
            "cowardess": 96, "courage": 0, "fear": 0,
            # a lord may be rallied by his own race, or by the House of Moon
            "RecruitingKey": 1 << race,
            "RecruitedBy": (1 << race) | (1 << RA_FREE),
            "race": race, "carrying": 0, "killedbyobject": 0,
            "gender": 2 if female else 1,
            "loyalty": race, "liege": 0, "foe": 0,
            "wait": 0, "orders": 0, "despondency": 0, "traits": 0,
            "following": 0, "followers": 0,
        }))

    # every lord's foe is Boroth
    boroth = sym_to_id.get("CH_BOROTH", 0)
    for _, o in char_rows:
        if o["symbol"] != "CH_BOROTH":
            o["foe"] = idt(IDT_CHARACTER, boroth)

    # ---------------- strongholds ----------------
    sh_rows, sh_cells, used_sh = [], [], set()
    for b in builds:
        if not onmap(b):
            continue
        land = areas_names[int(b["id"])] if int(b["id"]) < len(areas_names) else "Unknown"
        is_citadel = b["type"] == "citadel"
        terr = TN_CITADEL if is_citadel else TN_KEEP
        race = RACE_BY_NAME.get(b["owner"], RA_NONE)
        troops = sum(int(b["warrior_%d" % i]) for i in range(16))
        ruler = sym_to_id.get(b["land_ruler"], 0)
        sym = symbolise(land, "ST_", used_sh)
        x, y = int(b["x"]), int(b["y"])
        sh_cells.append((x, y, terr))
        sh_rows.append((len(sh_rows) + 1, {
            "symbol": sym, "flags": 0, "location": (x, y),
            "occupyingrace": race, "race": race, "type": 1,   # UT_WARRIORS
            "totaltroops": troops,
            "mintroops": 100, "maxtroops": max(1200, troops),
            "strategical_success": 0, "owner_success": 0, "enemy_success": 0,
            "influence": 5 if is_citadel else 2,
            "respawn": 0xFFFFFFFF,
            "occupier": idt(IDT_CHARACTER, ruler) if ruler else 0,
            "owner": idt(IDT_CHARACTER, ruler) if ruler else 0,
            "terrain": terr, "killed": 0,
        }))

    # ---------------- places (map landmarks from the Places layer) ----------
    place_rows, place_cells, used_pl = [], [], set()
    areas_layer = cd.layer("regions")
    place_names = {TN_HENGE: "Henge", TN_VILLAGE: "Village", TN_RUIN: "Ruin",
                   TN_LITH: "Lith", TN_PIT: "Pit", TN_PALACE: "Palace",
                   TN_HALL: "Hall", TN_WATCHTOWER: "Watchtower",
                   TN_FOUNTAIN: "Fountain", TN_STONES: "Stones"}
    for i, g in enumerate(cd.layer("Places")):
        if not g:
            continue
        x, y = i % W, i // W
        a = (areas_layer[i] - AREA_GID) & 0x3ff
        land = areas_names[a] if a < len(areas_names) else "Unknown"
        sym = symbolise(land + "_" + place_names.get(g, "Place"), "PL_", used_pl)
        place_cells.append((x, y, g))
        place_rows.append((len(place_rows) + 1,
                           {"symbol": sym, "flags": 0, "location": (x, y)}))

    # ---------------- objects ----------------
    # slots 1..7 keep the engine's mxthing_t meaning (creatures, then the two
    # seek results); 8..14 are the seven weapons of the Citadel.
    obj_rows = []

    def add_obj(sym, name, desc, kills, flags):
        obj_rows.append((len(obj_rows) + 1, {
            "symbol": sym, "flags": flags, "location": (0, 0), "name": name,
            "carriedby": 0, "description": desc, "kills": kills,
            "usedescription": 0,
        }))

    add_obj("OB_WOLVES", "Wolves", "wolves", 0, of_fight | of_see)
    add_obj("OB_DRAGONS", "Dragons", "dragons", 0, of_fight | of_see)
    add_obj("OB_TROLLS", "Trolls", "trolls", 0, of_fight | of_see)
    add_obj("OB_WILDCATS", "Wild Cats", "wild cats", 0, of_fight | of_see)
    add_obj("OB_WILDHORSES", "Wild Horses", "wild horses", 0, of_see)
    add_obj("OB_SHELTER", "Shelter", "shelter and is refreshed", 0, of_remove)
    add_obj("OB_GUIDANCE", "Guidance", "guidance", 0, of_remove)

    OB_WOLVES, OB_DRAGONS, OB_TROLLS, OB_WILDCATS = 1, 2, 3, 4
    weapon_flags = of_pickup | of_drop | of_weapon | of_see | of_remove | of_unique
    weapon_cells = []
    for o in objs:
        name = o["name"].replace("the sword ", "").replace("the axe ", "").title()
        kind = "Sword" if "sword" in o["name"] else "Axe"
        sym = "OB_" + name.upper()
        add_obj(sym, "%s %s" % (kind, name), "the {obj:name}", 0, weapon_flags)
        if onmap(o):
            weapon_cells.append((int(o["x"]), int(o["y"]), len(obj_rows)))

    # creatures on the map, from the beast list: head_27 roams the rivers and
    # plains (wolves), head_28 keeps to the high mountains (wild cats).
    obj_cells = []
    for b in beasts:
        if not onmap(b):
            continue
        obj_cells.append((int(b["x"]), int(b["y"]),
                          OB_WILDCATS if b["portrait"] == "head_28" else OB_WOLVES))

    # trolls guard the Dark Citadel of Maranor (design document)
    dark = next((s for i, s in sh_rows if s["symbol"] == "ST_MARANOR"), None)
    if dark:
        obj_cells.append((dark["location"][0], dark["location"][1], OB_TROLLS))

    # the seven weapons are unique, so they go down last and win the cell
    obj_cells.extend(weapon_cells)

    # ---------------- areas ----------------
    area_rows = []
    for i, n in enumerate(areas_names):
        area_rows.append((i, {"symbol": symbolise(n, "AR_", set()), "flags": 0,
                              "name": n, "prefix": ""}))

    # ---------------- missions / victories ----------------
    def ch(sym):
        return idt(IDT_CHARACTER, sym_to_id.get(sym, 0))

    maranor_id = next((i for i, s in sh_rows if s["symbol"] == "ST_MARANOR"), 0)
    house_of_moon = ["CH_LUXOR", "CH_MORKIN", "CH_CORLETH", "CH_ANDERLANE"]

    missions = [
        ("MI_BOROTH_DEAD", 1, 2, 3,
         [ch("CH_BOROTH"), 0, 0, 0, 0], 16, idt(IDT_RACEINFO, RA_FREE)),
        ("MI_MARANOR_TAKEN", 1, 2, 1,
         [idt(IDT_STRONGHOLD, maranor_id), idt(IDT_RACEINFO, RA_FREE), 0, 0, 0],
         16, idt(IDT_RACEINFO, RA_FREE)),
        ("MI_HOUSE_OF_MOON_DEAD", 1, 2, 3,
         [ch(s) for s in house_of_moon] + [0], 16, idt(IDT_RACEINFO, RA_DARKFEY)),
        ("MI_BOROTH_DESTROYED", 2, 0, 4,
         [idt(IDT_MISSION, 1), idt(IDT_MISSION, 2), 0, 0, 0], 0, 0),
    ]
    mission_rows = []
    for n, (sym, prio, obj_t, cond, refs, pts, scorer) in enumerate(missions, 1):
        mission_rows.append((n, {
            "symbol": sym, "flags": 2, "priority": prio,
            "objective": obj_t, "condition": cond,
            "ref0": refs[0], "ref1": refs[1], "ref2": refs[2],
            "ref3": refs[3], "ref4": refs[4],
            "points": pts, "scorer": scorer, "action": 0, "actionid": 0,
        }))

    SS_VICTORY_CITADEL, SS_DEFEAT_MOON = 101, 102
    victory_rows = [
        (1, {"symbol": "VI_BOROTH_DESTROYED", "flags": 3, "priority": 1,
             "mission": 4, "string": SS_VICTORY_CITADEL}),
        (2, {"symbol": "VI_HOUSE_OF_MOON_DEAD", "flags": 3, "priority": 3,
             "mission": 3, "string": SS_DEFEAT_MOON}),
    ]

    # ---------------- text + variables ----------------
    lom = tmedb.read_db(open(_os.path.join(ROOT, "main", "midnight",
        "Resources-midnight", "lom", "16", "database"), "rb").read())
    # The engine's message templates are scenario-neutral; only the handful
    # that name Lords of Midnight things need rewording for the Citadel.
    REWORD = {
        # there is no ice fear in the Citadel
        "SS_CHARACTER_FEAR": "",
        # ... and no Ice Crown to destroy
        "SS_GUIDANCE2": "may know something of the Dark Citadel",
        "SS_SEEK_MSG1": "", "SS_SEEK_MSG2": "",
        "SS_SEEK_MSG3": "", "SS_SEEK_MSG4": "",
    }
    strings = []
    for i, code, s in lom["strings"]:
        if code in REWORD:
            s = REWORD[code]
        elif "War of the Solstice" in s:
            s = s.replace("War of the Solstice", "War of the Bloodmarch")
        strings.append((i, code, s))
    strings.append((SS_VICTORY_CITADEL, "SS_VICTORY_CITADEL",
                    "\r\n\r\nBoroth the Wolfheart is destroyed and the Dark Citadel "
                    "of Maranor has fallen.\r\n\r\nVictory to the Bloodmarch! "))
    strings.append((SS_DEFEAT_MOON, "SS_DEFEAT_MOON",
                    "\r\n\r\nThe House of Moon is no more.\r\n\r\n"
                    "Victory to Boroth the Wolfheart! "))

    variables = []
    for name, value, t in lom["variables"]:
        if name == "MAP_WIDTH":
            value = str(W)
        elif name == "MAP_HEIGHT":
            value = str(H)
        elif name == "CHARACTER_DEFAULT":
            value = "CH_MORKIN|CH_ARALETH|CH_RORTHRON|CH_CORLETH|CH_ARIN"
        elif name == "CHARACTER_FRIEND":
            value = "CH_MORKIN"
        elif name == "CHARACTER_FOE":
            value = "CH_BOROTH"
        elif name == "SEEK_MESSAGES":
            continue
        variables.append((name, value, t))

    # ---------------- assemble ----------------
    gamemap, terrain = build_map(sh_cells, place_cells, obj_cells)

    # A handful of the 1995 coordinates land on a cell the Tiled map paints as
    # open water, which would strand that lord for the whole game. Walk each
    # one out to the nearest cell he could actually stand on.
    BLOCKED = {TN_SEA, TN_BAY, TN_LAKE3}

    def nearest_open(x, y, limit=12):
        for r in range(1, limit):
            for dy in range(-r, r + 1):
                for dx in range(-r, r + 1):
                    if max(abs(dx), abs(dy)) != r:
                        continue
                    nx, ny = x + dx, y + dy
                    if 0 <= nx < W and 0 <= ny < H and terrain[ny * W + nx] not in BLOCKED:
                        return nx, ny
        return None

    moved = 0
    for _, o in char_rows:
        x, y = o["location"]
        if not (0 <= x < W and 0 <= y < H) or terrain[y * W + x] in BLOCKED:
            n = nearest_open(min(max(x, 0), W - 1), min(max(y, 0), H - 1))
            if n:
                o["location"] = n
                moved += 1
    if moved:
        print("moved %d lord(s) off open water" % moved)

    db = {
        "scenarioid": SCENARIO_ID, "version": DB_VERSION,
        "header": tmedb.DB_HEADER,
        "characters": char_rows,
        "regiments": [],
        "routenodes": [],
        "strongholds": sh_rows,
        "places": place_rows,
        "objects": obj_rows,
        "missions": mission_rows,
        "victories": victory_rows,
        "directions": [(i, {"symbol": s, "flags": 0, "name": n}) for i, (s, n) in enumerate([
            ("DR_NORTH", "North"), ("DR_NORTHEAST", "Northeast"),
            ("DR_EAST", "East"), ("DR_SOUTHEAST", "Southeast"),
            ("DR_SOUTH", "South"), ("DR_SOUTHWEST", "Southwest"),
            ("DR_WEST", "West"), ("DR_NORTHWEST", "Northwest")])],
        "units": [(i, {"symbol": s, "flags": 0, "name": n,
                       "success": su, "baserestmodifier": br})
                  for i, (s, n, su, br) in enumerate([
                      ("UT_NONE", "none", 0, 0),
                      ("UT_WARRIORS", "warriors", 96, 4),
                      ("UT_RIDERS", "riders", 64, 6)])],
        "races": [(i, race_row(i)) for i in range(len(RACES))],
        "genders": [(i, {"symbol": s, "flags": 0, "name": n,
                         "pronoun1": p1, "pronoun2": p2, "pronoun3": p3})
                    for i, (s, n, p1, p2, p3) in enumerate([
                        ("GD_NONE", "none", "none", "none", "none"),
                        ("GD_MALE", "Male", "he", "his", "him"),
                        ("GD_FEMALE", "Female", "she", "her", "her")])],
        "terrains": terrain_rows(),
        "areas": area_rows,
        "commands": [(i, {"symbol": s, "flags": 0, "name": n,
                          "successtime": 0, "failuretime": 0})
                     for i, (s, n) in enumerate([
                         ("CMD_NONE", "None"), ("CMD_MOVE", "Move"),
                         ("CMD_APPROACH", "Approach"), ("CMD_RECRUITMEN", "RecruitMen"),
                         ("CMD_POSTMEN", "PostMen"), ("CMD_ATTACK", "Attack"),
                         ("CMD_FIGHT", "Fight"), ("CMD_REST", "Rest"),
                         ("CMD_HIDE", "Hide"), ("CMD_UNHIDE", "UnHide"),
                         ("CMD_SEEK", "Seek"), ("CMD_TAKEOBJECT", "TakeObject"),
                         ("CMD_DROPOBJECT", "DropObject"), ("CMD_WAIT", "Wait")])],
        "strings": strings,
        "variables": variables,
        "tail": b"",
    }

    os.makedirs(OUT, exist_ok=True)
    open(_os.path.join(OUT, "database"), "wb").write(tmedb.write_db(db))
    open(_os.path.join(OUT, "map"), "wb").write(tmedb.write_map(gamemap))

    # a written database must read back identically
    back = tmedb.read_db(open(_os.path.join(OUT, "database"), "rb").read())
    assert tmedb.write_db(back) == tmedb.write_db(db), "database does not round-trip"
    assert back["counts"][0] == len(char_rows)
    assert len(back["tail"]) == 0, "trailing bytes in database"
    bm = tmedb.read_map(open(_os.path.join(OUT, "map"), "rb").read())
    assert bm["w"] == W and bm["h"] == H and len(bm["tail"]) == 0

    print("characters  %4d" % len(char_rows))
    print("strongholds %4d" % len(sh_rows))
    print("places      %4d" % len(place_rows))
    print("objects     %4d  (%d placed on the map)" % (len(obj_rows), len(obj_cells)))
    print("areas       %4d" % len(area_rows))
    print("terrains    %4d" % len(terrain_rows()))
    print("races       %4d" % len(RACES))
    print("missions    %4d   victories %d" % (len(mission_rows), len(victory_rows)))
    print("strings     %4d   variables %d" % (len(strings), len(variables)))
    print("database    %d bytes" % os.path.getsize(_os.path.join(OUT, "database")))
    print("map         %d bytes  (%dx%d)" % (os.path.getsize(_os.path.join(OUT, "map")), W, H))


if __name__ == "__main__":
    main()
