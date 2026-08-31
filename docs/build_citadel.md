# The Lords of Midnight III : The Citadel

The Citadel is the third scenario for the Midnight Engine, built from the
1995 design by David Ollman and James Shaw for a game that was never
released. It runs on the same engine as Lords of Midnight and Doomdark's
Revenge; only the scenario, the data and the artwork differ.

## Building

The Citadel builds exactly like the other two, with `-DTME=CITADEL`:

```
cd main\midnight\build.win32\citadel
mkdir vs_project && cd vs_project
cmake ..\..\.. -G"Visual Studio 17 2022" -A Win32 -DTME:string=CITADEL
```

Then build the `citadel` target. The executable appears under
`vs_project\bin\citadel\Release`.

`TME=CITADEL` defines both `_CITADEL_` and `_LOM_`. The Citadel is a
Lords-of-Midnight-family scenario: it shares the moon-ring-era code paths,
the LOM battle resolution, the stronghold model and the look/think panels.
The handful of places where it must differ are guarded with
`&& !defined(_CITADEL_)`:

| File | What differs |
|---|---|
| `Source/tme/tme.cpp` | scenario name, resource directory, `citadel::Create` |
| `Source/tme/tme_interface.h` | includes `scenario_citadel.h` |
| `Source/system/moonring.cpp` | loads `citadel/citadel.tme`; no novel scenario |
| `Source/system/settingsmanager.h` | no scenario selector, no pre-rendered lord names |
| `Source/frontend/version_check.h` | no update check |
| `Source/frontend/language.h` | title and exit strings |
| `Source/frontend/help_id.h` | help ids for the Citadel terrain range |
| `Source/system/help_lom.cpp` | LOM's help table is excluded |

## Where the game data comes from

`main/midnight/Resources-citadel/citadel/18/database` and `.../18/map` are
generated, not hand-authored. Their sources are all in the repository:

- `citadel/tiled/citadel_wip.tmx` — the 256x256 map painted in Tiled. Each
  terrain layer already holds the engine's own `mxterrain_t` code, the
  `regions` layer holds the area index (`gid - 177`) and `border.txt` marks
  the mist at the edge of the world.
- `citadel/data/citadel_*.txt` — the 1995 game data dumped by
  `tools/exporter`: 155 characters, 131 buildings (one per land), 244
  beasts and the 7 weapons.
- `tools/exporter/citadel export/main.cpp` — the race, realm and land name
  tables.
- `The Citadel - Design.pdf` — the lore, and the win and lose conditions.

The generator scripts are not part of the game build; regenerate the
database only when the source data changes.

### How the 1995 data maps onto the engine

**Races.** The engine hard-codes race 0 as "none", race 2 as the free
(stronghold change-of-sides, memory) and race 3 as the fey with their
forest affinity. The Citadel's sixteen peoples are seated to match: the
Dark Fey take slot 1 (the engine's Doomguard slot, so captured Dark Fey
strongholds correctly fall to the free), the Free take slot 2, the High Fey
slot 3, and the remaining thirteen follow.

**Strongholds.** The building list is indexed by land, so each of the 131
lands has one castle or citadel; 104 of them are on the map. Troops are the
owning race's warrior count, which is the only non-zero entry in every
building.

**Objects.** Slots 1-7 keep their `mxthing_t` meaning, so the engine's
creature and seek handling still works: wolves, dragons, trolls, wild cats,
wild horses, shelter, guidance. Slots 8-14 are the seven weapons. On the
map, the beasts with portrait `head_27` become wolves (they roam the rivers
and plains) and those with `head_28` become wild cats (they sit in the icy
mountains) — which is what the design document says of each.

**Victory.** From the design document: you win when Boroth the Wolfheart is
dead *and* the Dark Citadel of Maranor has fallen, because killing him
without taking the citadel leaves him a wraith there. You lose when all
four of the House of Moon — Luxor, Morkin, Corleth and Anderlane — are
dead. This is expressed as four missions and two victories in the database.

**Recruiting.** A lord may be rallied by a lord of his own race, or by any
lord of the Free. Freeing a realm's hostage from Maranor is meant to bring
that realm in; the hostages are in the data and at the right place, but the
"free the hostage, gain the realm" rule itself is not yet implemented.

## Artwork

The Citadel's own artwork is the 1995 VGA set in `citadel/graphics`, which
has not been converted into the engine's atlases. What is in place:

- `screens/0/splash.png` — the original `LOM3.bmp` cover art
- `misc/logo.png` — the Lords of Midnight wordmark with "The Citadel" set
  beneath it in the game's own Midnight face
- `screens/misc/overview_map.png` — `citadel/maps/drawn map.jpg`

Everything else — terrain, people, faces, shields, panel furniture — is
still Lords of Midnight artwork, mapped onto the Citadel's terrain and race
symbols by `Resources-citadel/citadel/citadel.tme`.

## Known gaps

- **No roaming regiments.** Every one of the 312 army records in the 1995
  data is an empty runtime slot; LOM3 spawned armies from its own AI, which
  the Midnight Engine does not have. The world therefore has stronghold
  garrisons and lord armies but no wandering enemy hosts.
- **The hostages do nothing yet.** All thirteen are present in the Dark
  Citadel with the right allegiances, but freeing one does not yet bring
  its realm over.
- **The weapons carry no powers.** Stormblade, Bloodbringer, Widowmaker,
  Aranath, Persuader, Skullcrusher and Swiftwing can be found and carried,
  but their design-document effects (loyalty, tirelessness, invincibility,
  recruitment) are not implemented.
- **Artwork** is largely Lords of Midnight's, as above.
- **Faces and shields** are only assigned for the eight named lords; the
  other 146 fall back to blank entries.
