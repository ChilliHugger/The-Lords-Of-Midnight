# The Citadel: Analysis of James Shaw's Original Assembly

Source: `citadel/James Shaw Code/` (`QUEST.ASM`, `QUEST.EQU`, `QCONTROL.ASM`, `REGION.ASM`, `REGION.EQU`, `CHARPROP.EQU`, `STRUCTUR.ASM`, `QUEST.EQU.txt`). About 10,000 lines of 32-bit x86 (Borland TASM, IDEAL mode, `P386`, USE32), dated March to August 1995 by the in-source comments. The files are plain text with CRLF line endings.

Companion document: [citadel_scenario_design.md](citadel_scenario_design.md) describes the current C++ port. Section 9 below compares the two.

**This is a partial drop.** The files reference routines and data that are not included: `dark_fey_rout`, `TR_check_recruit/join/attack/take`, `calc_combat_energy`, `fight_sim_battle`, `restock_citadels`, `goodie_takes_dark_citadel`, `modrandn`, `EQUATES.ASM`, the character/citadel data files (`charac0001`…`citad0025`), and the map. Statements about those are inferred from how they are called.

---

## 1. What the code is

It is the **autonomous character AI** for the 1995 game: how the roughly 154 lords act when the player is not driving them. It has three layers.

| Layer | File | Job |
|---|---|---|
| Strategic AI ("JIM" rules) | `QUEST.ASM`, `QUEST.EQU` | Once a day, each idle lord's *purpose* runs a chain of *reactions* that picks a new *quest* |
| Quest executor | `QCONTROL.ASM` | Per-frame state machines that carry out quests (walk, recruit, kill, siege, guard, …) |
| World data | `REGION.ASM/.EQU`, `STRUCTUR.ASM`, `CHARPROP.EQU` | Kingdom and realm graphs, the object record layouts, the personality bits |

The player's own lords use the same quest executor. The player or the AI picks a quest, and `quest_control` runs it. That is why the design calls the AI "JIM": the scripted rules are written as if they were player commands.

---

## 2. World model

### 2.1 Geography (`REGION.EQU`, `REGION.ASM`)

Three nested levels:

| Level | Count | Names | Notes |
|---|---|---|---|
| **Kingdom** | 17 (0–16) | Witherlands, Lee, Eldmark, LongMountains, LastNorthing, Arungor, Weirdwood, Dawnwood, Fallows, Gelm, Deeping, Delve, Marish, Glimormir, GreatOcean, Midnight, MistsOblivion | The peoples. In the C++ port these became `RA_*` races. |
| **Realm** | 131 (0–130) | Imilvir, Eodral, Arelon, … Varorn | Named regions, 4 per row in `REGION.EQU` |
| **Land** | the "citadel list" | Each citadel or castle | Indexed 0–129. The first 25 are special and the AI skips them (see 2.3). |

Tables in `REGION.ASM`:
- `kingdom_adj`: for each kingdom, a count byte followed by neighbouring kingdoms. For example Witherlands → Midnight, Lee, Glimormir, Eldmark. This is the source of `citadel_kingdoms[]` in `citadel_scenario.cpp`. The port maps Lee→Atheling, Eldmark→Eldrin and Glimormir→Golden Fey. I spot-checked Witherlands and it matches.
- `kingdom_list`: for each kingdom, the realms it contains (for example Witherlands = Imilvir, Sareon, Eomir, Erifel; Lee = 9 realms).
- `place_list`: for each realm, the lands it neighbours. Water and a few specials are filtered by "id < 25, or Corlon, or Elormane".
- `region_capitals[]`: each realm's capital citadel.
- `REGION.ASM` also holds helpers: `regions_neighbours`, `regions_neighbours_not_water`, `kingdoms_neighbours`, `kingdom_neighbour`, `in_same_kingdom`, `get_region_in_kingdom`, `get_neighbour_land`, `do_water_test_*`.

### 2.2 Records (`STRUCTUR.ASM`)

All objects share a header (links, id, type, flags, 16:16 fixed-point x/y/z, sprite, scale, collision sizes). Then per-type extensions:

- **Castle / citadel**: shield, portrait/sprite, `obj_race_size[16]` (a warrior count per race), `obj_owner`, and the war-fighting fields:
  - `obj_land_ruler`: the character currently ruling it.
  - `obj_land_war_state`: one of 8 states (see 3.1).
  - `obj_land_realm`, `obj_land_owner` (the original ruler), `obj_land_enemies` (a 16-bit enemy-race mask), and `obj_cit_link` (a linked list of the characters inside).
- **Character**: movement, journey, energy, combat (`obj_strength`, `obj_opponent`), **quest state** (`obj_quest`, `obj_qstate`, `obj_qdat1..4`, `obj_qtype1/2`), party links, army (race, size, lost, killed), `obj_start_realm` and `obj_start_land` (the lord's home), sleep and wake times, weapon, plus the AI fields `obj_purpose`, `obj_reaction`, `obj_curr_land`, `obj_curr_realm`, `obj_allegiance`, and two 32-bit quality masks.

`obj_char_flags` bits include recruited, knowledge, dead, friendly, rescue (a prisoner), interest, major, control, location, watch. This corresponds to `cf_*` in the port. `rescue_bit` is "prisoner". `obj_flags2` holds `no_process_bit`, `autoq_bit` and `really_inside_bit`.

### 2.3 Personality (`CHARPROP.EQU`)

64 qualities as two masks of 32 bits: `qualities1` (positive: brave, far_sighted, selfish, bold, kind, … mighty_warrior, superb_leader) and `qualities2` (the opposites: cowardly, reckless, cautious, … feeble_warrior, pitiful_leader). `q_selfish` is defined in both. The positive and negative bit positions line up row for row (brave↔cowardly, kind↔cruel, …), which is the structure the port's `std::rotl(theirs, 32)` trick reuses for recruit scoring.

The AI reads only six: `BRAVE`, `CAUTIOUS`, `COWARDLY`, `FAR_SIGHTED`, `RECKLESS`, `SELFISH`.

### 2.4 Realm records (`QUEST.EQU`)

```
realm_ruler (2)  realm_war_state (1)  realm_state (1)
realm_owner (2)  realm_enemies (2)    realm_hostage (2)   → 10 bytes (region_mul = ×10)
```

- `realm_enemies` is a **16-bit mask of enemy realms/allegiances**. Enmity is dynamic (see 6.3).
- `realm_hostage` is the character number of the realm's hostage.

---

## 3. The AI vocabulary

### 3.1 Enumerations (`QUEST.EQU`)

| Group | Values |
|---|---|
| Purposes | `NO_PURPOSE`(-1), `DEFEND_HOMELAND`, `DARK_FEY`, `RANDOMLY_WANDER`, `BE_A_HOSTAGE` |
| Reactions | `RETURN_HOME`, `STAND_FIRM`, `TAKE_BACK_STRONGHOLD`, `ATTACK_ENEMY`, `HELP_NEIGHBOUR`, `RETREAT`, `GATHER_STRENGTH`, `COUNTER_THREAT`, `LEND_SERVICE` |
| Land war states | `AT_PEACE` < `TROUBLED` < `THREATENED` < `MENACED` < `IN_DANGER` < `IN_PERIL` < `UNDER_ATTACK` < `SUBJUGATED` |
| Realm war states | `AT_PEACE`, `THREATENED`, `UNDER_ATTACK`, `SUBJUGATED` |
| Realm state | `UNFETTERED`, `AT_RANSOM`, `BLOOD_FEUD`, `INTERREGNUM` |
| Quests | `RECRUIT, JOIN, KILL, RESCUE, FOLLOW, GOTO, GUARD, SEIZE, FIND, TAKE, DESTROY, REST` (+`UNDER`) |
| Who (for conditions) | `ENEMY, SELF, NEIGHBOUR, LEADER, QUEST_SUBJECT, LIST` |

Purposes, reactions and quest names map one-to-one onto the port's `PU_*`, `RE_*` and `QS_*` and the strings in `characters.tsv`.

### 3.2 Macro DSL (`QUEST.EQU`)

The rules are not hand-written assembly. `QUEST.EQU` defines a small **rule language out of macros**:

- **Truth is the carry flag.** `STC` is true and `CLC` is false.
- Control flow: `BIFF label` (branch if false → `jnc`), `BIFT` (if true → `jc`), `BRA`, `LAB`, `NOTT` (`cmc`).
- `ZtoC` copies the zero flag to carry (`lahf; bt ax,14`).
- Every reaction is a `DEFINE_REACTION name … END_REACTION` procedure.
- **Conditions** (set carry): `HOMELAND state`, `AT_STRONGHOLD`, `BATTLE_ODDS a,b`, `DEAD who`, `IN_HOMELAND/IN_NEARLAND/IN_SAMELAND/IN_ADJACENT_LAND who`, `ANY_NEARLAND state`, `BRAVE/COWARDLY/CAUTIOUS/RECKLESS/SELFISH/FAR_SIGHTED`, `ARE_QUESTING`, `IS_EMPTY_LIST`, `SOMEONE_TO_RECRUIT`, `FELLOW_LORD_RECRUITING`.
- **List builders** (`GEN_LIST cond`) fill a shared buffer. **Filters** (`SUB_LIST cond`) drop entries. `CONV_LIST` converts between lands and their ruling characters. `PICK_FROM_LIST` chooses one entry at random into `last_result`.
- **Actions**: `GO_STATE reaction` writes the new reaction and returns true. `SET_QUEST type, where` writes the quest and returns false.
- An unfinished `FOR/NEXT/LIST_ENTRY` is present and marked "not used (yet) / they don't work anyway".

A convention to remember: **the filter names say what is removed**, not what is kept. `sc_not_at_peace_rout` removes the not-at-peace entries, so only entries at peace remain. `sc_gte_under_attack` removes entries at or above UNDER_ATTACK.

---

## 4. The daily strategic loop (`QUEST.ASM`)

### 4.1 `reevaluate_quests`

Runs **at most once per game day** (`last_game_day` guard, "JS 5/4/95").

1. `restock_citadels` (external): garrisons are replenished.
2. `setup_character_positions`: recomputes each lord's `obj_curr_land` and `obj_curr_realm` from his x/y via the `map_regions` table.
3. `evaluate_realm_war_states` (see 4.2).
4. For every character in the list, skip him if any of these is true:
   - not `typ_character`, or `no_process_bit`, or the special `seize_character`
   - he is the player's controlled lord while the `watch_flag` is clear
   - he is not currently resting (`obj_quest != _qrest`), so **an active quest is never interrupted**
   - he is a prisoner (`rescue_bit`), or Boroth
   - `Boroth_dead` is set and he starts in the Marish realm (the war is over for them)
   - his purpose is `NO_PURPOSE` (negative)
5. Otherwise call his purpose routine through `purpose_list`.

### 4.2 War-state evaluation

Two passes, using a **pseudo-character**: the code copies the first character into a scratch buffer, fakes its `obj_start_realm` and `obj_start_land`, then reuses the normal `GEN_LIST`/`IN_*` conditions. This lets the same rule code evaluate a realm or land instead of a lord.

**Realms** (all except the Great Ocean):
- `SUBJUGATED` if **every** land in the realm is subjugated (ruler differs from original owner and is an enemy of it).
- Otherwise `AT_PEACE`. If any enemy lord is scanned inside the realm's lands, it becomes `UNDER_ATTACK` (if the realm has any enemy flags) or `THREATENED` (if not).
- `realm_state`: `INTERREGNUM` if the realm's owner (the King) is dead, else `BLOOD_FEUD` if the hostage is dead, else `AT_RANSOM` if the hostage is still a prisoner, else `UNFETTERED`.

**Lands** (citadel list 25–129):
- `SUBJUGATED` if the land's ruler differs from its original owner and is hostile.
- Realm `UNDER_ATTACK` → land is `UNDER_ATTACK` (enemy within the land), `IN_PERIL` (enemy in a neighbouring land), else `IN_DANGER`.
- Realm `THREATENED` → `MENACED` (enemy in the land), `THREATENED` (enemy near), else `TROUBLED`.
- Otherwise `AT_PEACE`.

So a keep's danger level is a **graded, realm-wide-then-local** value. It depends on where enemy lords *are* relative to the land map, not on a distance in squares.

### 4.3 Purposes

`purpose_list`:

| Purpose | Routine |
|---|---|
| `DEFEND_HOMELAND` | `defend_homeland_rout`: the reaction state machine (4.4) |
| `DARK_FEY` | `dark_fey_rout` (**external, not provided**) |
| `RANDOMLY_WANDER` | `random_wander_nothing`: **an empty stub (`ret`)** |
| `BE_A_HOSTAGE` | `be_hostage_nothing`: **an empty stub (`ret`)** |

### 4.4 The reaction state machine

`defend_homeland_rout` runs the lord's current reaction repeatedly. Each reaction returns carry = "I changed state, run again" (`GO_STATE`) or clear = "I have settled" (`SET_QUEST` or fall-through). It is bounded to 13 transitions per lord per day (`cmp ebx,14`).

The state graph, each reaction written as an ordered list of tests where the first match wins:

```
RETURN_HOME
  home land SUBJUGATED                         → TAKE_BACK_STRONGHOLD
  already at own stronghold                    → STAND_FIRM
  else: mark unavailable; if idle → QUEST GOTO own stronghold

STAND_FIRM
  1 home IN_PERIL, far-sighted, not cowardly, someone to recruit  → GATHER_STRENGTH
  2 home IN_PERIL and (brave and not cautious) or (reckless and not cowardly) → ATTACK_ENEMY
  3 home IN_DANGER, not cowardly, not selfish, a neighbouring land that is
      at most IN_PERIL (including calm places) whose original owner is at home, defending and not retreating  → QUEST JOIN him, → HELP_NEIGHBOUR
  4 home TROUBLED, any near land THREATENED, not cowardly, not selfish → COUNTER_THREAT
  5 home UNDER_ATTACK and (cowardly and odds 3:4 against) or (not brave and odds 3:1 against) → RETREAT
  6 mark available iff not under attack; QUEST REST

TAKE_BACK_STRONGHOLD
  home no longer subjugated → STAND_FIRM
  in homeland and (cowardly and odds 1:2) or (not brave and odds 2:1) vs own citadel's ruler → RETREAT
  else if idle → QUEST SEIZE own stronghold

ATTACK_ENEMY
  no longer in homeland while home is UNDER_ATTACK, or enemy not near, or I'm not near, or quest subject dead → RETURN_HOME
  else if idle: QUEST KILL the nearest enemy in order: same land, home land, adjacent land, near land; else REST

HELP_NEIGHBOUR
  home UNDER_ATTACK, or subject dead, no longer defending, or retreating,
  (or neighbour no longer IN_DANGER while I'm in peril)  → RETURN_HOME
  else if idle: QUEST JOIN subject (or REST)

RETREAT
  home state < UNDER_ATTACK → RETURN_HOME
  else if idle: QUEST GOTO a safe near land, else safe land in home realm,
      else a land in a neighbouring realm not at peace, else any land not at peace; else REST

GATHER_STRENGTH
  home UNDER_ATTACK, or nobody to recruit → RETURN_HOME
  enemy in same land → ATTACK_ENEMY
  else if idle: QUEST RECRUIT a random non-coward neighbour lord who is available and near; else REST

COUNTER_THREAT
  home worse than TROUBLED → RETURN_HOME
  else if idle: STAY (REST) if already at a threatened land's stronghold, else GOTO a threatened near land

LEND_SERVICE
  home UNDER_ATTACK, or leader dead → RETURN_HOME
  else mark unavailable and wait
```

Observations:
- **It is a real state machine with memory.** `obj_reaction` persists between days, so a lord that is `RETREAT`ing stays so until `RETURN_HOME` is triggered.
- Quests are only assigned when the lord is idle (`ARE_QUESTING` followed by `NOTT`).
- Personality drives almost every branch. Cowardly lords retreat sooner and never counter-threaten. Selfish lords will not help neighbours. Reckless lords attack. Brave lords fight longer.
- `BATTLE_ODDS a,b` compares `obj_strength` after `calc_combat_energy`: it is true when my strength × b ≥ his × a (that is, I am no worse than a:b against him).
- The "available" flag (`available_bit`) tells other lords whether this one can be recruited or asked to help.
- Several rules have disabled blocks (`IF 0 … ENDIF`) for `LEND_SERVICE`. In the shipped code `LEND_SERVICE` is **never entered**. It is only a state the lord could be in.

---

## 5. The quest executor (`QCONTROL.ASM`)

### 5.1 Dispatch

`quest_control` runs per object per frame:

```
quest_type[quest] → a table of state routines
state routine = table[obj_qstate]
call it; if carry set → run again (next qstate or a new quest); else stop for this frame
```

Each quest is a list of function pointers, one per `obj_qstate`, so quest progress is an index that routines increment (`inc obj_qstate`). Common states:

| Routine | Role |
|---|---|
| `quest_wake_up_rout` | Wake a sleeping player lord when given a new quest |
| `quest_store_charac_quest_rout` / `restore_…` | Save and restore the quest around a detour |
| `quest_find_rout` | Resolve the target to a location and set up a journey |
| `quest_goto_rout` | Per-frame travel along the journey |
| `quest_reset_maybe_rout` | Re-plan if the target has moved |
| `quest_check_not_dead_rout` | Cancel if the target died |
| `quest_move_inside/outside_maybe_rout` | Enter or leave a building as needed |
| `quest_is_about_to_X_rout` + two `quest_delay_1_frame_rout` | Set the "about to" flag so the UI can show it, then wait two frames |
| `quest_X_rout` | The action itself |
| `quest_cancel` | Final state: clear the quest |
| `quest_panic` (×8) | Any state past the end is a bug; it hangs deliberately |

Quest tables:

| Quest | Steps |
|---|---|
| RECRUIT | wake → store → find → goto → reset → check-dead → move-inside → about-to → delay×2 → **recruit** → cancel |
| JOIN | the same shape, ending in `quest_join_rout` (and the AI-used `join_quest_no` = 11 is the "now in a party" state) |
| KILL | wake → store → find → goto → reset → check-dead → about-to → delay×2 → **kill** → cancel |
| FOLLOW | … goto → … → `quest_follow_rout` (loops until the target dies) |
| GOTO | init → move-outside → goto → move-inside → reset-meet → cancel |
| GUARD | wake → init-guard → store-target → goto → **guard** → goto → check-dead-guard → kill → restore-goto → restart |
| SEIZE | wake → goto → about-to → delay×2 → **seize** → cancel |
| FIND | … → pickup → cancel |
| TAKE | … → decide → kill-holder → pickup → cancel |
| DESTROY | … → decide → kill-holder → pickup → destroy-object → cancel |
| RESCUE, REST | essentially do nothing |

### 5.2 Key actions

- **`quest_recruit_rout`**: refuses if either lord is on a boat or dragon (`obj_link_type != -1`). Calls `TR_check_recruit` (the "will he join?" test, external), which is the 1995 equivalent of `RecruitScore`/`CheckRecruitChar`. On success it is the **inverse of a join**: swaps esi/edi, adds the target to the recruiter's party, rewrites the target's quest into `QJOIN` with `qstate = join_quest_no` pointing at the recruiter, and records "recruited". Handles the cases where the target is already in a party (leaves it first) and where two lords try to recruit each other (the higher `obj_rank` becomes leader).
- **`quest_kill_rout`**: calls `TR_check_attack` (should a fight start?). **The act of attacking makes the attacker's allegiance an enemy of the victim's** (`bts realm_enemies`). If visible, it starts a real fight on screen (`start_fighting`). If not visible, it resolves instantly with `fight_sim_battle`. Next state when the opponent is dead.
- **`quest_seize_rout`**: moves the attacker *inside* the citadel, links him into it, makes the ruler his opponent, and fights (visible fight or instant sim). A global `seize_semaphore` and `combat_flag` serialise sieges (one at a time). On victory: `kill_citadel_armies` zeroes all 16 race garrison counts, `obj_owner` becomes the victor's allegiance, `obj_land_ruler` becomes the victor, and the citadel is registered as taken. If it is the Dark Citadel (`citad0027`) it calls `goodie_takes_dark_citadel`. If the attacker dies, the siege is cleaned up and the quest ends.
- **`quest_guard_rout`**: stops the lord, scans the 3×3 squares around him in the link map for an **enemy, walking, outside, not already in combat, not on a boat** character. If found, it rewrites the quest data into a KILL on that character and advances state. Otherwise it stays put.
- **`quest_init_guard_rout`**: converts any other target type into a `qt_goto` of the target's coordinates.
- **`quest_follow_rout`**: cancels itself if the target is dead.

### 5.3 Quest data encoding

`obj_qdat1..4` plus `qtype1/2` hold up to two targets: `(list type, object number)` pairs, or raw x/y for `qt_goto`. `setup_quest_data` (`QUEST.ASM`) is how the AI writes them. `bung_general` stores the target's `obj_num` and sets `autoq_bit` ("automatic quest", so the UI can tell AI quests from player ones). Quest numbers are registered in several other files ("change the list on line 49 of action.asm", "bodges in party.asm and savedata.asm"), so the quest set is hard-wired across the 1995 code base.

---

## 6. Rules worth knowing

### 6.1 Only idle lords are re-planned
`ARE_QUESTING` followed by `NOTT` guards every `SET_QUEST`. A lord mid-quest is never redirected. He only reconsiders when the quest ends in `_qrest`.

### 6.2 Hostage and realm state
`realm_hostage` and the `realm_state` enumeration encode the hostage mechanic in the realm record: `UNFETTERED` (hostage free or no hostage), `AT_RANSOM` (hostage prisoner), `BLOOD_FEUD` (hostage dead), `INTERREGNUM` (King dead). In the shown code only the war states and `realm_state` are *computed*. Where `AT_RANSOM` prevents a King from marching is not in these files.

### 6.3 Enmity is a bitmask that grows
`realm_enemies` is set when someone attacks (`kill` or `seize` both do `bts … realm_enemies`). `is_enemy`, `are_enemies` and `are_friends` compare `obj_allegiance` bytes against that mask. There is no static diplomacy table. **Who is at war with whom is created by attacks during play.** (An older version using `obj_start_realm` is preserved inside an `IF 0` block.)

### 6.4 Boroth
`Boroth = charac0015`. He is excluded from the daily loop, and the `Boroth_dead` flag turns off the Marish realm's lords.

### 6.5 Random choice
`PICK_FROM_LIST` calls `modrandn` for a uniform pick, so equal candidates are chosen at random. The C++ port uses nearest-first instead.

---

## 7. Code-quality notes

These are things I noticed while reading. They are not necessarily bugs in the shipped game.

1. **Assertions are infinite loops.** Labels like `@@err: jmp @@err`, `bastard1..4`, and `quest_panic` deliberately hang on an impossible state.
2. **`STATE` macro** contains `@@p: jne @@p`. It hangs if the argument is anything but `MY_HOMELAND`.
3. **`check_at_stronghold SELF`** returns true immediately (`stc; ret`). The position-compare code after it is unreachable. A lord is treated as "at his stronghold" whenever the reaction asks, so `RETURN_HOME → STAND_FIRM` triggers without a position check.
4. **`lc_this_kingdom_rout`** compares `ebp` (the list write pointer) with 25, Corlon and Elormane instead of `ebx`. This looks like a typo. The routine's only user is `find_a_region_citadel`.
5. **`check_recruitables`** always returns true (`stc; ret`), so `SOMEONE_TO_RECRUIT` never filters anything.
6. **`check_fellow_lord_quest`** always returns false. Together with the `IF 0` blocks, the `LEND_SERVICE` path is dead.
7. **Hard-coded exclusions**: `cmp ebx,25 / Corlon / Elormane` is repeated in every list builder.
8. **Hard-coded magic numbers**: `fudge_people` forces every character's flags to `0E12Bh`. `set_beddies` sets everyone's sleep/wake to 18:00/06:00 (apparently debug helpers).
9. **Commented history** is frank ("fuck me! it actually worked first time", "DAFT CLOT! This is a bloke!"). The dates show the AI was still being changed in August 1995.

---

## 8. Dependencies not in this folder

`TR_check_recruit/join/attack/take` (the willingness tests), `calc_combat_energy`, `fight_sim_battle`, `start_fighting`, `restock_citadels`, `dark_fey_rout`, `goodie_takes_dark_citadel`, `link_map_scan`, journey and route code (`init_journey`, `cancel_journey`), `make_quest_register` and `make_info_register` (the text), `modrandn`, `EQUATES.ASM`.

The first four are the most worth hunting for if more source exists, because they hold the original numbers for recruiting and combat odds.

---

## 9. Comparison with the C++ port

| Topic | 1995 assembly | Current C++ (`citadel_processor_quest.cpp`, `citadel_scenario.cpp`) |
|---|---|---|
| Cadence | Once per game day | Every `LordsTurn` |
| Decision structure | **Persistent state machine** of 9 reactions with up to 13 transitions per day | **Stateless ladder** (`Defend`): the whole decision is recomputed each night and the reaction is overwritten |
| Interruption | Only idle lords are re-planned | `Order()` overwrites the quest each night, even mid-march |
| Danger model | 8-level land war state and 4-level realm war state, derived from where enemy lords are in the land and realm graph | Straight-line distance `THREAT = 4` from a keep to an enemy regiment |
| Enemies | Enemy *lords* in lands (the Dark Fey are `DARK_FEY` purpose characters) | Boroth's *regiments* (`regiments.tsv`) |
| Personality | Six qualities decide most branches: brave, cautious, cowardly, reckless, selfish, far-sighted | Only cowardly (`HasQuality(qf_cowardly)`) and impatient (impatience) are used |
| Battle odds | Strength ratio tests (3:4, 3:1, 1:2, 2:1) that depend on personality | Men count vs regiment count (`men >= invader->Total()`, `upon->Total() > men`) |
| Hostages | In the realm record (`realm_hostage`, `realm_state` incl. `AT_RANSOM`) | `HostageOfRace()` gates `Marches()` and the recruit threshold |
| Enmity | Dynamic `realm_enemies` bitmask that grows with every attack | Fixed: `RA_ENEMY` vs everyone, plus `feuds` for posting |
| Reactions in use | `LEND_SERVICE` is dead code | `RE_LEND_SERVICE` is live (`service` in `Defend`) and `TAKE_BACK_STRONGHOLD` is a high priority |
| Purposes | `RANDOMLY_WANDER` and `BE_A_HOSTAGE` are empty stubs | `Wander()` is implemented, and a freed hostage defends its home |
| `GATHER_STRENGTH` | Goes to *recruit a neighbour lord* | Pulls *troops from a keep's spare garrison* into the lord's army |
| Recruiting | `TR_check_recruit` (not provided) | Quality-overlap score with a hostage-dependent threshold |
| Seize | Walk in, fight the ruler, zero all garrisons, set owner and ruler; semaphore allows one siege at a time | Uses the LoM battle (`citadel_battle`), with ×3/×4 wall multipliers |
| Guard | Scan 3×3 squares for an enemy lord, convert to KILL | `citadel_battle::Guard`: duels adjacent enemy lords or attacks an adjacent regiment the lord can outnumber |
| Pathing | Journey planner (not provided) | Dijkstra over the land map (`StepsFrom`/`MarchStep`) |
| Random vs nearest | Random pick from a list | Nearest-first (`Nearer`, `FirstReachable`) |
| Kingdom adjacency | `kingdom_adj` | `citadel_kingdoms[]` (same graph, renamed) |
| Boroth's wraith | `Boroth_dead` flag stops Marish lords | `RaiseWraith`, and victory requires Maranor to fall as well |

### Where the port is faithful
- The vocabulary: purposes, reactions, quests and war-state names are carried over.
- The kingdom graph.
- The shape of "react → set a quest → execute the quest".
- Hostages tied to a realm.
- Seizing a keep by fighting its ruler.

### Where the port diverges (candidates for a closer port)
1. **Persistent reaction state.** The original lets a lord *stay* in `RETREAT` or `HELP_NEIGHBOUR` across days and only re-evaluates when idle. The port flips every night.
2. **The 8-level war-state model.** It is a richer, realm-aware danger signal than a 4-square radius, and it drives `TROUBLED`/`THREATENED` → `COUNTER_THREAT`, which the port approximates with `danger`.
3. **Personality.** Brave, cautious, reckless, selfish and far-sighted are all unused in the port, which is where much of the original's variety came from.
4. **Battle-odds retreats.** The original compared strength using `calc_combat_energy`.
5. **Dynamic enmity.** Attacking a realm turns it hostile to the attacker.
6. **Randomness.** The original chose among equals at random.
7. **Realm state (`INTERREGNUM`, `BLOOD_FEUD`).** Computed in the original but not modelled in the port.

None of these is a defect in the port. It is a deliberate reimplementation. The list shows where the two behave differently and where the original source can settle design questions.
