"""Sanity checks on the generated Citadel database + map."""
import collections
import tmedb

import os as _os
# repo root, two levels up from tools/citadel_build/
ROOT = _os.path.abspath(_os.path.join(_os.path.dirname(_os.path.abspath(__file__)), "..", ".."))

DB = _os.path.join(ROOT, "main", "midnight", "Resources-citadel", "citadel", "18", "database")
MAP = _os.path.join(ROOT, "main", "midnight", "Resources-citadel", "citadel", "18", "map")

db = tmedb.read_db(open(DB, "rb").read())
mp = tmedb.read_map(open(MAP, "rb").read())

TN = {v[0]: k for k, v in
      {n: (n,) for n in []}.items()}  # filled below
NAMES = {i: o["symbol"] for i, o in db["terrains"]}

print("scenario id %d  version %d" % (db["scenarioid"], db["version"]))
print("counts:", db["counts"])
print()

chars = {o["symbol"]: (i, o) for i, o in db["characters"]}
shs = {o["symbol"]: (i, o) for i, o in db["strongholds"]}

# --- the characters the game needs -------------------------------------
must = ["CH_MORKIN", "CH_ARALETH", "CH_RORTHRON", "CH_CORLETH", "CH_ARIN",
        "CH_LUXOR", "CH_ANDERLANE", "CH_BOROTH"]
for m in must:
    assert m in chars, "missing " + m
    i, o = chars[m]
    print("%-14s id=%-4d %-28s race=%-2d loc=%-10s flags=0x%x" % (
        m, i, o["longname"], o["race"], o["location"], o["flags"]))
print()

# --- start characters resolve ------------------------------------------
vars_ = dict((n, v) for n, v, t in db["variables"])
for s in vars_["CHARACTER_DEFAULT"].split("|"):
    assert s in chars, "CHARACTER_DEFAULT names unknown lord " + s
print("CHARACTER_DEFAULT ->", vars_["CHARACTER_DEFAULT"])
print("MAP %sx%s" % (vars_["MAP_WIDTH"], vars_["MAP_HEIGHT"]))
assert int(vars_["MAP_WIDTH"]) == mp["w"] and int(vars_["MAP_HEIGHT"]) == mp["h"]
print()

# --- the Dark Citadel ---------------------------------------------------
assert "ST_MARANOR" in shs, "the Dark Citadel of Maranor is missing"
i, o = shs["ST_MARANOR"]
print("ST_MARANOR id=%d loc=%s race=%d troops=%d terrain=%s" % (
    i, o["location"], o["race"], o["totaltroops"], NAMES[o["terrain"]]))
assert o["location"] == chars["CH_BOROTH"][1]["location"], \
    "Boroth is not in the Dark Citadel"
print()

# --- missions reference real things ------------------------------------
for i, m in db["missions"]:
    refs = [m["ref%d" % k] for k in range(5)]
    print("%-24s prio=%d obj=%d cond=%d refs=%s" % (
        m["symbol"], m["priority"], m["objective"], m["condition"],
        [(r >> 24, r & 0xffffff) for r in refs if r]))
    for r in refs:
        if not r:
            continue
        t, rid = r >> 24, r & 0xffffff
        if t == 10:
            assert any(j == rid for j, _ in db["characters"]), "bad char ref"
        elif t == 14:
            assert any(j == rid for j, _ in db["strongholds"]), "bad stronghold ref"
        elif t == 23:
            assert any(j == rid for j, _ in db["missions"]), "bad mission ref"
for i, v in db["victories"]:
    assert any(j == v["mission"] for j, _ in db["missions"]), "victory -> unknown mission"
    assert any(sid == v["string"] for sid, _, _ in db["strings"]), "victory -> unknown string"
    print("%-24s prio=%d mission=%d string=%d" % (
        v["symbol"], v["priority"], v["mission"], v["string"]))
print()

# --- map ----------------------------------------------------------------
terr = collections.Counter()
areas = set()
objs = collections.Counter()
mist = special = strong = 0
for c in mp["cells"]:
    d = tmedb.uncell(c)
    terr[NAMES.get(d["terrain"], d["terrain"])] += 1
    areas.add(d["area"])
    if d["obj"]:
        objs[d["obj"]] += 1
    if d["flags"] & 1:
        mist += 1
    if d["flags"] & (1 << 7):
        special += 1
    if d["flags"] & (1 << 9):
        strong += 1
print("terrain:", dict(terr.most_common()))
print("areas used: %d (max %d, table has %d)" % (len(areas), max(areas), len(db["areas"])))
assert max(areas) < len(db["areas"]), "map references an area with no info row"
print("map objects:", dict(objs), "total", sum(objs.values()))
print("mist cells %d  special %d  stronghold %d" % (mist, special, strong))
assert strong == len(db["strongholds"]), "stronghold cells != stronghold rows"
maxt = max(tmedb.uncell(c)["terrain"] for c in mp["cells"])
assert maxt < len(db["terrains"]), "map uses terrain %d with no info row" % maxt
for oid in objs:
    assert any(j == oid for j, _ in db["objects"]), "map references unknown object %d" % oid

# --- every stronghold sits on its own cell ------------------------------
for i, o in db["strongholds"]:
    x, y = o["location"]
    d = tmedb.uncell(mp["cells"][y * mp["w"] + x])
    assert d["terrain"] == o["terrain"], \
        "%s terrain %s != map %s" % (o["symbol"], o["terrain"], d["terrain"])

# --- nobody may start stranded on open water ----------------------------
block = {i for i, o in db["terrains"] if o["flags"] & 2}
stranded = []
for i, o in db["characters"]:
    x, y = o["location"]
    if not (0 <= x < mp["w"] and 0 <= y < mp["h"]):
        stranded.append((o["symbol"], "off map"))
        continue
    t = tmedb.uncell(mp["cells"][y * mp["w"] + x])["terrain"]
    if t in block:
        stranded.append((o["symbol"], NAMES[t]))
for s in stranded:
    print("  STRANDED", s)
assert not stranded, "%d lords start on impassable water" % len(stranded)
print("no lord starts on impassable water")

for i, o in db["strongholds"]:
    x, y = o["location"]
    t = tmedb.uncell(mp["cells"][y * mp["w"] + x])["terrain"]
    assert t not in block, "%s stands in water" % o["symbol"]
print("no stronghold stands in water")

# --- plural terrain names ------------------------------------------------
# SS_TERRAIN_PLURAL renders "{terrain:name}s", so a terrain flagged plural
# must be named in the singular or the game says "Treess".
doubled = [o["symbol"] + " -> " + o["name"] + "s"
           for _, o in db["terrains"]
           if (o["flags"] & 1) and o["name"].endswith("s")]
for d in doubled:
    print("  DOUBLED PLURAL", d)
assert not doubled, "%d terrain names double their plural" % len(doubled)
print("no terrain doubles its plural")

print()
print("ALL CHECKS PASSED")
