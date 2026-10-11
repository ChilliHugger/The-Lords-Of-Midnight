#!/usr/bin/env python3
"""Convert the web build's saves export (<app>-saves-*.json, e.g. midnight-saves-*) to and from real files.

  wasm_saves.py unpack midnight-saves-2026-10-10T20-00-00-000Z.json [outdir]
      Writes each saved file under outdir (default: the export's name without .json).

  wasm_saves.py pack folder [bundle.json]
      Builds a bundle from a folder, importable with Ctrl+Shift+S > Import in the web build.
      The folder layout is the layout in the export, e.g. "The Lords of Midnight/settings.cfg".
"""
import base64
import json
import os
import sys

FORMAT = "axmol-persist-v1"


def check_name(name):
    parts = name.split("/")
    if name.startswith("/") or ".." in parts or "\\" in name or name == "":
        raise SystemExit(f"refusing unsafe path in bundle: {name!r}")


def unpack(bundle_path, out_dir=None):
    with open(bundle_path, encoding="utf-8") as f:
        bundle = json.load(f)
    if bundle.get("format") != FORMAT or not isinstance(bundle.get("files"), dict):
        raise SystemExit("not a saves export")
    out_dir = out_dir or os.path.splitext(bundle_path)[0]
    for name in bundle["files"]:
        check_name(name)  # validate everything before writing anything
    for name, data in bundle["files"].items():
        dest = os.path.join(out_dir, *name.split("/"))
        os.makedirs(os.path.dirname(dest), exist_ok=True)
        with open(dest, "wb") as f:
            f.write(base64.b64decode(data))
        print(f"{dest}")
    print(f"{len(bundle['files'])} files written to {out_dir}"
          f" (exported {bundle.get('created', '?')} from {bundle.get('page', '?')})")


def pack(folder, bundle_path=None):
    files = {}
    for root, _, names in os.walk(folder):
        for n in names:
            full = os.path.join(root, n)
            rel = os.path.relpath(full, folder).replace(os.sep, "/")
            if n == ".DS_Store":
                continue
            with open(full, "rb") as f:
                files[rel] = base64.b64encode(f.read()).decode("ascii")
    bundle = {"format": FORMAT, "created": None, "page": "", "files": files}
    from datetime import datetime, timezone
    bundle["created"] = datetime.now(timezone.utc).isoformat()
    bundle_path = bundle_path or folder.rstrip("/\\") + ".json"
    with open(bundle_path, "w", encoding="utf-8") as f:
        json.dump(bundle, f)
    print(f"{len(files)} files packed into {bundle_path}")


def main(argv):
    if len(argv) >= 3 and argv[1] == "unpack":
        unpack(argv[2], argv[3] if len(argv) > 3 else None)
    elif len(argv) >= 3 and argv[1] == "pack":
        pack(argv[2], argv[3] if len(argv) > 3 else None)
    else:
        print(__doc__)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
