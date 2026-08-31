"""Terrain help is looked up as (1 + terrain code) in panel_look.cpp, so the
Citadel help id enum must place every terrain id at exactly that position."""
import re

import os as _os
# repo root, two levels up from tools/citadel_build/
ROOT = _os.path.abspath(_os.path.join(_os.path.dirname(_os.path.abspath(__file__)), "..", ".."))

SRC = _os.path.join(ROOT, "main", "midnight", "Source", "frontend", "help_id.h")

# terrain code -> the help id that must sit at (code + 1)
EXPECT = {
    0: "HELP_TN_PLAINS", 1: "HELP_TN_CITADEL", 2: "HELP_TN_FOREST",
    3: "HELP_TN_HENGE", 4: "HELP_TN_TOWER", 5: "HELP_TN_VILLAGE",
    6: "HELP_TN_DOWNS", 7: "HELP_TN_KEEP", 8: "HELP_TN_SNOWHALL",
    9: "HELP_TN_LAKE", 10: "HELP_TN_FROZENWASTE", 11: "HELP_TN_RUIN",
    12: "HELP_TN_LITH", 13: "HELP_TN_CAVERN", 14: "HELP_TN_MOUNTAIN",
    15: "HELP_TN_UNUSED1",
    16: "HELP_TN_PLAINS2", 17: "HELP_TN_MOUNTAIN2", 18: "HELP_TN_FOREST2",
    19: "HELP_TN_HILLS", 20: "HELP_TN_GATE", 21: "HELP_TN_TEMPLE",
    22: "HELP_TN_PIT", 23: "HELP_TN_PALACE", 24: "HELP_TN_FORTRESS",
    25: "HELP_TN_HALL", 26: "HELP_TN_HUT", 27: "HELP_TN_WATCHTOWER",
    28: "HELP_TN_CITY", 29: "HELP_TN_FOUNTAIN", 30: "HELP_TN_STONES",
    31: "HELP_TN_ICYWASTES", 32: "HELP_TN_UNUSED32",
    33: "HELP_TN_LAND", 34: "HELP_TN_ISLE", 35: "HELP_TN_LAKELAND",
    36: "HELP_TN_PLAIN", 37: "HELP_TN_PLAINS3", 38: "HELP_TN_FOREST3",
    39: "HELP_TN_UNUSED39", 40: "HELP_TN_TREES", 41: "HELP_TN_MOUNTAIN3",
    42: "HELP_TN_ICY_MOUNTAIN", 43: "HELP_TN_DOWNS3", 44: "HELP_TN_HILLS3",
    45: "HELP_TN_FOOTHILLS", 46: "HELP_TN_VALLEY", 47: "HELP_TN_BAY",
    48: "HELP_TN_SEA", 49: "HELP_TN_RIVER", 50: "HELP_TN_MARSH",
    51: "HELP_TN_LAKE3",
}


def enum_for(defines):
    """Evaluate the helpid_t enum under a set of -D defines."""
    body = re.search(r"enum helpid_t\s*\{(.*?)\n\};", open(SRC).read(), re.S).group(1)
    out, stack = [], [True]
    for line in body.splitlines():
        s = line.strip()
        m = re.match(r"#if\s+(!?)defined\((\w+)\)", s)
        if m:
            v = (m.group(2) in defines) != bool(m.group(1))
            stack.append(stack[-1] and v)
            continue
        if s.startswith("#endif"):
            stack.pop()
            continue
        if s.startswith("#else"):
            top = stack.pop()
            stack.append(stack[-1] and not top)
            continue
        if not stack[-1] or s.startswith("//") or not s:
            continue
        for name in re.findall(r"^(\w+)\s*(?:=\s*\d+)?\s*,", s):
            out.append(name)
    return out


names = enum_for({"_CITADEL_", "_LOM_"})
idx = {n: i for i, n in enumerate(names)}
assert idx.get("HELP_NONE") == 0, "HELP_NONE must be 0"

bad = []
for code, want in sorted(EXPECT.items()):
    got = names[code + 1] if code + 1 < len(names) else "<past end>"
    if got != want:
        bad.append((code, want, got))

print("citadel enum has %d ids" % len(names))
for code, want, got in bad:
    print("  MISMATCH terrain %-3d expected %-22s got %s" % (code, want, got))
assert not bad, "%d terrain help ids are misaligned" % len(bad)
print("all 52 terrain help ids line up with (1 + terrain code)")

# LOM must keep its original numbering
lom = enum_for({"_LOM_"})
for code in range(16):
    assert lom[code + 1] == EXPECT[code], "LOM numbering changed at %d" % code
print("LOM numbering unchanged (%d ids)" % len(lom))

ddr = enum_for({"_DDR_"})
print("DDR numbering intact (%d ids)" % len(ddr))
