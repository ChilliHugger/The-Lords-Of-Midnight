"""Parse the exported Citadel data dumps and the Tiled map."""
import re

import os as _os
# repo root, two levels up from tools/citadel_build/
ROOT = _os.path.abspath(_os.path.join(_os.path.dirname(_os.path.abspath(__file__)), "..", ".."))

DATA = _os.path.join(ROOT, "citadel", "data") + _os.sep
TMX = _os.path.join(ROOT, "citadel", "tiled", "citadel_wip.tmx")

W = H = 256


def records(filename, tag):
    s = open(DATA + filename, encoding="latin-1").read()
    out = []
    for m in re.finditer(r"<%s\b([^>]*)>(.*?)</%s>" % (tag, tag), s, re.S):
        d = dict(re.findall(r"(\w+)='([^']*)'", m.group(1)))
        body = m.group(2)
        for f in re.finditer(r"<(\w+)>([^<]*)</\1>", body):
            d[f.group(1)] = f.group(2)
        # <tag value='N'>text</tag> and <tag value='N'/>
        for f in re.finditer(r"<(\w+)\s+value='([^']*)'\s*(?:/>|>([^<]*)</\1>)", body):
            d[f.group(1) + "_value"] = f.group(2)
            d[f.group(1)] = f.group(3) or ""
        out.append(d)
    return out


_tmx_cache = {}


def layer(name):
    if not _tmx_cache:
        s = open(TMX, encoding="utf-8").read()
        for m in re.finditer(
            r'<layer name="([^"]+)"[^>]*>\s*<data encoding="csv">\s*(.*?)\s*</data>',
            s, re.S):
            _tmx_cache[m.group(1)] = [
                int(v) for v in m.group(2).replace("\n", "").split(",") if v.strip()
            ]
    return _tmx_cache[name]


def layers():
    layer("realms")
    return dict(_tmx_cache)


def at(lay, x, y):
    return lay[y * W + x]


if __name__ == "__main__":
    import collections
    b = records("citadel_buildings.txt", "building")
    cities = layer("cities.txt")
    print("buildings:", len(b))
    onmap = [r for r in b if 0 <= int(r["x"]) < W and 0 <= int(r["y"]) < H]
    print("on-map:", len(onmap))
    hits = collections.Counter()
    for r in onmap:
        x, y = int(r["x"]), int(r["y"])
        hits[(r["type"], at(cities, x, y))] += 1
    print("(building type, cities-layer gid at that cell) ->", dict(hits))
