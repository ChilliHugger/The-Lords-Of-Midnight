# Midnight web maps

Web versions of the detailed map from **The Lords of Midnight** and **Doomdark's Revenge**, built straight from the game's own data and graphics: the same TMX maps, tile sheets, sprite atlases, faces and Midnight font, and the same rules as `panel_map_detailed` and `mapbuilder`.

## What's here

| Path | What it is |
| --- | --- |
| `site/lords-of-midnight/` | The Lords of Midnight map (scenario 16), a complete site on its own. |
| `site/doomdarks-revenge/` | Doomdark's Revenge map (scenario 17), a complete site on its own. |
| `build.py` | Rebuilds the sites from the repo. |
| `extract.py` | Reads a game's map, tables, sprites and font (run by `build.py`). |
| `template.html` | The map page. `build.py` fills in the title and the links to its assets. |

Each game's folder holds everything that game needs and nothing else:

```
lords-of-midnight/
  index.html          the map page
  assets/
    data.js           the map, terrain, areas, lords and strings
    tiles.png         the map tile sheet
    Midnight.woff     the game font
    f_*.png, p_*.png  lord faces (Doomdark's Revenge uses race faces)
    i_*.png, map_*.png, circle_selector.png
                      buttons and icons
```

To publish, upload the *contents* of a game's folder to the root of its server, or to any sub-folder. All links are relative, so it works wherever it's placed. It also works opened straight from disk. Any static host will do.

## Rebuilding

Needs Python 3.8 or later and Pillow (`pip install pillow`). fontTools is optional (`pip install fonttools`): with it the font is packed as WOFF at about half the size, and without it the TTF is used as it is.

If this folder lives inside the repo, `build.py` finds `main/midnight` by itself:

```sh
python3 build.py              # both games into ./site
python3 build.py ddr          # just Doomdark's Revenge
```

Anywhere else, point it at the repo:

```sh
python3 build.py --root ~/Projects/The-Lords-Of-Midnight/main/midnight --out site
```

Run it again whenever the map, terrain, characters, strings or graphics change in the repo. Each game's folder is cleared before it's rebuilt, so nothing from an earlier build is left behind. Building one game doesn't touch the other.

To get a single `index.html` per game with everything embedded (about 1.7 MB each), for somewhere that only takes one file, use `--inline`:

```sh
python3 build.py --inline --out single
```

### Where the data comes from

| | The Lords of Midnight | Doomdark's Revenge |
| --- | --- | --- |
| Map | `Resources-midnight/lom/16/tmx/map.tmx` | `Resources-revenge/ddr/17/tmx/map.tmx` |
| Tables | `lom/16/tsv/*.tsv` | `ddr/17/tsv/*.tsv` |
| Map cells, faces | `lom/lom.tme` | `ddr/ddr.tme` |
| Tiles | `lom/gfx_h/terrain/map_tiles.png` | `ddr/gfx_h/terrain/map_tiles.png` |
| Icons | `lom/gfx_h/rest-0.plist` | `ddr/gfx_h/rest-0.plist` |
| Faces | `lom/gfx_m/faces` | `ddr/gfx_m/faces` |
| Font | `Resources/fonts/Midnight.ttf` | same |

### Game differences the build handles

- **Doomdark's Revenge frozen waste** becomes icy waste, as `mxloc::Serialize()` does when the game loads the map. This is what makes the border draw.
- **Doomdark's Revenge has no domains.** The TMX Domain layer is ignored, so places read "the Tower of Thiril" and never "… in the Domain of …".
- **Lord faces in Doomdark's Revenge** fall back to the race face, as `GetCharacterFace()` does.
- **Filter buttons** only appear for layers the scenario has. Doomdark's Revenge has tunnels but no creatures in its map file, because the game places those at random when a game starts. The Lords of Midnight has creatures but no tunnels.

## Using the maps

- **Drag** to scroll. Use the **mouse wheel or pinch** to zoom, or the **– / reset / +** buttons. **Up/Down** zoom from the keyboard.
- **Hover** over a square, or touch it, to see its name in the hint at the top.
- **Shift-click** a square, or **press and hold** it on a touch screen, to highlight its whole area. Shift-click again, click anywhere, or press **Esc** to clear it.
- **?** (or **/** or **Ctrl/⌘+F**) opens search. A lord is scrolled to and ringed, a place is highlighted, and an area is shown in full.
- **Filters**, top right, all start off: centre on Luxor (**F1**), creatures (**F2**), lords (**F3**), tunnels (**F4**).
- Click a **group shield** to open the circle of lords standing there.
- A place's name is hidden while a lord is shown standing on it, as in the game, and comes back when the lords filter is off.

The maps behave like the game's debug map: every square is revealed and every lord is at their starting position.
