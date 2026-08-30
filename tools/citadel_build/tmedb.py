"""Reader/writer for the Midnight Engine binary "database" and "map" files.

Format taken from Source/tme/base/mxengine.cpp (LoadDatabase),
Source/library/src/carchive.cpp (primitives) and each entity's Serialize().
Round-trip is verified byte-exact against the shipped LOM/DDR data.
"""
import struct

MAGIC = b"TME!"
DB_HEADER = "MidnightEngineDatabase"
MAP_HEADER = "MidnightEngineMap"


class R:
    def __init__(s, d):
        s.d, s.o = d, 0

    def u8(s):
        v, = struct.unpack_from("<B", s.d, s.o); s.o += 1; return v

    def u16(s):
        v, = struct.unpack_from("<H", s.d, s.o); s.o += 2; return v

    def u32(s):
        v, = struct.unpack_from("<I", s.d, s.o); s.o += 4; return v

    def s32(s):
        v, = struct.unpack_from("<i", s.d, s.o); s.o += 4; return v

    def u64(s):
        v, = struct.unpack_from("<Q", s.d, s.o); s.o += 8; return v

    def st(s):
        n = s.u16(); v = s.d[s.o:s.o + n]; s.o += n
        return v.decode("latin-1")

    def loc(s):
        return (s.s32(), s.s32())


class W:
    def __init__(s):
        s.b = bytearray()

    def u8(s, v):
        s.b += struct.pack("<B", v & 0xff)

    def u16(s, v):
        s.b += struct.pack("<H", v & 0xffff)

    def u32(s, v):
        s.b += struct.pack("<I", v & 0xffffffff)

    def s32(s, v):
        s.b += struct.pack("<i", v)

    def u64(s, v):
        s.b += struct.pack("<Q", v)

    def st(s, v):
        e = v.encode("latin-1"); s.u16(len(e)); s.b += e

    def loc(s, v):
        s.u32(v[0] & 0xffffffff); s.u32(v[1] & 0xffffffff)


ENTITY = "entity"   # symbol:str flags:u32
ITEM = "item"       # entity + location:loc
INFO = "info"       # entity + name:str

CHARACTER = [
    ("longname", "str"), ("shortname", "str"), ("looking", "u32"), ("time", "u32"),
    ("riders", "unit"), ("warriors", "unit"), ("battleloc", "loc"), ("battleslew", "u32"),
    ("reckless", "u32"), ("energy", "u32"), ("strength", "u32"), ("cowardess", "u32"),
    ("courage", "u32"), ("fear", "u32"), ("RecruitingKey", "u32"), ("RecruitedBy", "u32"),
    ("race", "u32"), ("carrying", "u32"), ("killedbyobject", "u32"), ("gender", "u32"),
    ("loyalty", "u32"), ("liege", "u32"), ("foe", "u32"), ("wait", "u32"),
    ("orders", "u32"), ("despondency", "u32"), ("traits", "u32"),
    ("following", "u32"), ("followers", "u32"),
]

REGIMENT = [
    ("race", "u32"), ("type", "u32"), ("total", "u32"), ("targetid", "u32"),
    ("orders", "u32"), ("success", "u32"), ("loyalty", "u32"), ("killed", "u32"),
    ("lastlocation", "loc"), ("delay", "u32"),
]

ROUTENODE = [("path0", "u32"), ("path1", "u32")]

STRONGHOLD = [
    ("occupyingrace", "u32"), ("race", "u32"), ("type", "u32"), ("totaltroops", "u32"),
    ("mintroops", "u32"), ("maxtroops", "u32"), ("strategical_success", "u32"),
    ("owner_success", "u32"), ("enemy_success", "u32"), ("influence", "u32"),
    ("respawn", "u32"), ("occupier", "u32"), ("owner", "u32"), ("terrain", "u32"),
    ("killed", "u32"),
]

PLACE = []

OBJECT = [
    ("name", "str"), ("carriedby", "u32"), ("description", "str"),
    ("kills", "u32"), ("usedescription", "u32"),
]

MISSION = [
    ("priority", "s32"), ("objective", "u32"), ("condition", "u32"),
    ("ref0", "u32"), ("ref1", "u32"), ("ref2", "u32"), ("ref3", "u32"), ("ref4", "u32"),
    ("points", "s32"), ("scorer", "u32"), ("action", "u32"), ("actionid", "u32"),
]

VICTORY = [("priority", "s32"), ("mission", "u32"), ("string", "u32")]

DIRECTION = []
UNITINFO = [("success", "u32"), ("baserestmodifier", "u32")]
RACE = [
    ("soldiersname", "str"), ("success", "u32"), ("initialmovement", "u32"),
    ("diagonalmodifier", "s32"), ("ridingmultiplier", "s32"), ("movementmax", "u32"),
    ("baserestamount", "u32"), ("strongholdstartups", "u32"), ("misttimeaffect", "s32"),
    ("mistdespondecyaffect", "s32"), ("baseenergycost", "s32"), ("baseenergycosthorse", "s32"),
]
GENDER = [("pronoun1", "str"), ("pronoun2", "str"), ("pronoun3", "str")]
TERRAIN = [
    ("preposition", "str"), ("description", "str"), ("success", "u32"),
    ("visibility", "u32"), ("obstruction", "u32"), ("movementcost", "s32"),
]
AREA = [("prefix", "str")]
COMMAND = [("successtime", "s32"), ("failuretime", "s32")]


def _rd(r, base, fields):
    o = {"symbol": r.st(), "flags": r.u32()}
    if base == ITEM:
        o["location"] = r.loc()
    if base == INFO:
        o["name"] = r.st()
    for n, k in fields:
        if k == "u32":
            o[n] = r.u32()
        elif k == "s32":
            o[n] = r.s32()
        elif k == "str":
            o[n] = r.st()
        elif k == "loc":
            o[n] = r.loc()
        elif k == "unit":
            o[n] = [r.u32() for _ in range(4)]
    return o


def _wr(w, o, base, fields):
    w.st(o["symbol"]); w.u32(o["flags"])
    if base == ITEM:
        w.loc(o["location"])
    if base == INFO:
        w.st(o["name"])
    for n, k in fields:
        v = o[n]
        if k == "u32":
            w.u32(v)
        elif k == "s32":
            w.s32(v)
        elif k == "str":
            w.st(v)
        elif k == "loc":
            w.loc(v)
        elif k == "unit":
            for x in v:
                w.u32(x)


# order matters, matches mxengine::LoadDatabase
TABLES = [
    ("characters", ITEM, CHARACTER),
    ("regiments", ITEM, REGIMENT),
    ("routenodes", ITEM, ROUTENODE),
    ("strongholds", ITEM, STRONGHOLD),
    ("places", ITEM, PLACE),
    ("objects", ITEM, OBJECT),
    ("missions", ENTITY, MISSION),
    ("victories", ENTITY, VICTORY),
    ("directions", INFO, DIRECTION),
    ("units", INFO, UNITINFO),
    ("races", INFO, RACE),
    ("genders", INFO, GENDER),
    ("terrains", INFO, TERRAIN),
    ("areas", INFO, AREA),
    ("commands", INFO, COMMAND),
]


def read_db(data):
    r = R(data)
    assert r.d[:4] == MAGIC
    r.o = 4
    db = {"scenarioid": r.u32(), "version": r.u32()}
    db["header"] = r.st()
    assert db["header"] == DB_HEADER, db["header"]
    counts = [r.u32() for _ in range(len(TABLES) + 1)]   # +1 = variables
    db["counts"] = counts
    nvars = counts[-1]
    for (key, base, fields), n in zip(TABLES, counts):
        rows = []
        for _ in range(n):
            rows.append((r.u32(), _rd(r, base, fields)))
        db[key] = rows
    nstr = r.u32()
    db["strings"] = [(r.u32(), r.st(), r.st()) for _ in range(nstr)]
    db["variables"] = [(r.st(), r.st(), r.u32()) for _ in range(nvars)]
    db["tail"] = data[r.o:]
    return db


def write_db(db):
    w = W()
    w.b += MAGIC
    w.u32(db["scenarioid"]); w.u32(db["version"]); w.st(db["header"])
    for (key, _, _) in TABLES:
        w.u32(len(db[key]))
    w.u32(len(db["variables"]))
    for (key, base, fields) in TABLES:
        for i, o in db[key]:
            w.u32(i)
            _wr(w, o, base, fields)
    w.u32(len(db["strings"]))
    for i, c, s in db["strings"]:
        w.u32(i); w.st(c); w.st(s)
    for n, v, t in db["variables"]:
        w.st(n); w.st(v); w.u32(t)
    w.b += db.get("tail", b"")
    return bytes(w.b)


def read_map(data):
    r = R(data)
    assert r.d[:4] == MAGIC
    r.o = 4
    m = {"version": r.u32()}
    m["header"] = r.st()
    assert m["header"] == MAP_HEADER
    m["w"] = r.u32(); m["h"] = r.u32()
    m["flags"] = r.u32() if m["version"] >= 3 else None
    m["cells"] = [r.u64() for _ in range(m["w"] * m["h"])]
    m["tail"] = data[r.o:]
    return m


def write_map(m):
    w = W()
    w.b += MAGIC
    w.u32(m["version"]); w.st(m["header"])
    w.u32(m["w"]); w.u32(m["h"])
    if m["version"] >= 3:
        w.u32(m["flags"])
    for c in m["cells"]:
        w.u64(c)
    w.b += m.get("tail", b"")
    return bytes(w.b)


def cell(terrain=0, variant=0, climate=0, obj=0, area=0, flags=0):
    return ((terrain & 0x7f) | ((variant & 7) << 7) | ((climate & 0x7f) << 10)
            | ((obj & 0x1ff) << 17) | ((area & 0x3ff) << 26)
            | ((flags & 0xfffffff) << 36))


def uncell(v):
    return dict(terrain=v & 0x7f, variant=(v >> 7) & 7, climate=(v >> 10) & 0x7f,
                obj=(v >> 17) & 0x1ff, area=(v >> 26) & 0x3ff,
                flags=(v >> 36) & 0xfffffff)
