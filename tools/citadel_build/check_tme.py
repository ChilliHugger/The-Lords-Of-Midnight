"""Every symbol citadel.tme names must exist in the generated database.

A typo here is silent at runtime - the artwork simply never binds - so it is
worth checking rather than noticing a missing sprite three panels later.
"""
import os
import re
import tmedb

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
RES = os.path.join(ROOT, "main", "midnight", "Resources-citadel", "citadel")

db = tmedb.read_db(open(os.path.join(RES, "18", "database"), "rb").read())
tme = open(os.path.join(RES, "citadel.tme"), encoding="latin-1").read()

have = {
    "character": {o["symbol"] for _, o in db["characters"]},
    "race": {o["symbol"] for _, o in db["races"]},
    "object": {o["symbol"] for _, o in db["objects"]},
    "terrain": {o["symbol"] for _, o in db["terrains"]},
}
# the terrain block also carries two frontend-only pseudo terrains
have["terrain"] |= {"ARMY", "MIST"}

bad = []
counts = {}
for kind in ("character", "race", "object", "terrain"):
    ids = re.findall(r'<%s\s+id\s*=\s*"([^"]+)"' % kind, tme)
    counts[kind] = len(ids)
    for i in ids:
        if i not in have[kind]:
            bad.append((kind, i))

for kind, i in bad:
    print("  UNKNOWN %-10s %s" % (kind, i))
print("citadel.tme references: " +
      ", ".join("%d %ss" % (n, k) for k, n in counts.items()))
assert not bad, "%d symbols in citadel.tme are not in the database" % len(bad)

# every race a lord actually uses should have a body sprite, or he draws blank
used = {o["race"] for _, o in db["characters"]}
race_sym = {i: o["symbol"] for i, o in db["races"]}
declared = set(re.findall(r'<race\s+id\s*=\s*"([^"]+)"', tme))
missing = sorted(race_sym[r] for r in used if race_sym[r] not in declared)
assert not missing, "lords use races with no artwork: %s" % missing
print("all %d races in use have artwork" % len(used))

# and every terrain the map actually uses should have a sprite entry
mp = tmedb.read_map(open(os.path.join(RES, "18", "map"), "rb").read())
used_t = {tmedb.uncell(c)["terrain"] for c in mp["cells"]}
terr_sym = {i: o["symbol"] for i, o in db["terrains"]}
decl_t = set(re.findall(r'<terrain\s+id\s*=\s*"([^"]+)"', tme))
missing_t = sorted(terr_sym[t] for t in used_t if terr_sym[t] not in decl_t)
assert not missing_t, "map uses terrain with no artwork: %s" % missing_t
print("all %d terrains on the map have artwork" % len(used_t))

print("citadel.tme and the database agree")
