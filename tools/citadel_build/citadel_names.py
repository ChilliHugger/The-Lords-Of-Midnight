"""Name tables lifted from tools/exporter/citadel export/main.cpp."""
import re

import os as _os
# repo root, two levels up from tools/citadel_build/
ROOT = _os.path.abspath(_os.path.join(_os.path.dirname(_os.path.abspath(__file__)), "..", ".."))

EXPORTER = _os.path.join(ROOT, "tools", "exporter", "citadel export", "main.cpp")


def table(name, src=None):
    src = src or open(EXPORTER, encoding="latin-1").read()
    m = re.search(r"LPCSTR\s+" + name + r"\s*\[\s*\]\s*=\s*\{(.*?)\};", src, re.S)
    if not m:
        raise KeyError(name)
    return re.findall(r'"([^"]*)"', m.group(1))


def all_tables():
    src = open(EXPORTER, encoding="latin-1").read()
    names = re.findall(r"LPCSTR\s+(\w+)\s*\[\s*\]\s*=", src)
    return {n: table(n, src) for n in names}


if __name__ == "__main__":
    for n, v in all_tables().items():
        print(f"{n}: {len(v)}")
        print("   ", v[:8], "..." if len(v) > 8 else "")
