# Citadel data build

Generates `main/midnight/Resources-citadel/citadel/18/database` and `map`
from the source data in the repository. Needs Python 3 only.

```
python build_citadel.py     # writes the database and the map
python verify_citadel.py    # checks what was written
python check_helpids.py     # checks the terrain help ids still line up
```

`tmedb.py` is a reader/writer for the engine's binary format, taken from
`Source/tme/base/mxengine.cpp` (`LoadDatabase`), `Source/library/src/carchive.cpp`
(the primitives) and each entity's `Serialize`. It round-trips the shipped
Lords of Midnight and Doomdark's Revenge files byte for byte, which is what
makes it safe to author a new database with:

```
python -c "import tmedb,os; \
 p=os.path.join('..','..','main','midnight','Resources-midnight','lom','16','database'); \
 d=open(p,'rb').read(); print(tmedb.write_db(tmedb.read_db(d))==d)"
```

Inputs, all in the repository:

| Source | Used for |
|---|---|
| `citadel/tiled/citadel_wip.tmx` | terrain, areas, mist, place and city cells |
| `citadel/data/citadel_character.txt` | 155 lords |
| `citadel/data/citadel_buildings.txt` | 131 lands, 104 strongholds on the map |
| `citadel/data/citadel_beasts.txt` | 244 creatures |
| `citadel/data/citadel_objects.txt` | the 7 weapons |
| `tools/exporter/citadel export/main.cpp` | race, realm and land names |
| `main/midnight/Resources-midnight/lom/16/database` | the scenario-neutral message templates and engine variables |

See `docs/build_citadel.md` for how the 1995 data maps onto the engine.
