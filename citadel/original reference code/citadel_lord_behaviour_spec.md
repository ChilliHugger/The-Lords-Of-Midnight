# The Citadel: Behaviour of the Non-Player Lords
## A specification for rebuilding the original game's lord behaviour

This document describes how the lords of the Blood March behaved in the 1995 game *The Lords of Midnight III: The Citadel*, when the player was not controlling them. It is written so that a programmer who has never seen the original program can rebuild the behaviour. It describes what happens and why, in ordinary words, and gives every table of data the behaviour depends on.

**How it was produced.** It was reconstructed from about 10,000 lines of the original program source (the behaviour rules, the quest handling, the map and kingdom tables, the personality tables) plus the game's design document and the game data files. Where the source was missing a piece, this document says so (section 14) rather than guessing silently. Where the source clearly contained a mistake or an odd result, the literal behaviour is described and the oddity is flagged with the words **Odd behaviour**, so you can decide whether to copy it or correct it.

**What is covered.** The daily decision-making of the lords; the quests they carry out; recruiting, fighting and sieges as the quests use them; the world they move through; the data tables. **What is not covered:** drawing, sound, the dungeon maze, the 3D landscape, the player interface, saving and loading.

---

## 1. The big picture

The game world is the Blood March, a land east of Midnight. It holds **fourteen kingdoms**, each home to one people, plus Midnight (the player's homeland), the Great Ocean and the Mists of Oblivion. Each kingdom contains named **places**. Most places hold a **keep**, a castle or a citadel, which stores soldiers and is ruled by a lord.

About **154 lords** exist. A few belong to the player's side (the House of Moon and their friends, called the Alliance). The rest are native lords of the Blood March. The enemy is **Boroth the Wolfheart** and his Dark Fey, who hold the Dark Citadel of Maranor and are conquering the kingdoms one at a time.

The native lords are not idle. Every game day each lord who is not busy looks at the military situation around his home and decides what to do next: stay put, march to help a neighbour, gather men, retake a lost keep, fight an invader, or run away. He turns that decision into a **quest** (go there, attack him, seize that keep, and so on). A separate piece of machinery then carries the quest out moment by moment. The player's own lords use exactly the same quest machinery; the player simply picks the quests by hand.

So the behaviour has three parts:

1. **The world**: kingdoms, places, keeps, who rules what, who is at war with whom (sections 2 to 4).
2. **The daily decision**: the chain of behaviours that turns "what is going on near my home?" into a quest (sections 5 to 8).
3. **Quest carrying-out**: what each kind of quest actually does (sections 9 to 11).

Section 12 covers hostages and the special realm statuses, section 13 the victory rules, section 14 what is missing from the source, and the appendices hold the data.

---

## 2. The world

### 2.1 Kingdoms, places and keeps

- A **kingdom** is a large area ruled by one people (Witherlands, Lee, Eldmark, Long Mountains, Last Northing, Arungor, Weirdwood, Dawnwood, Fallows, Gelm, Deeping, Delve, Marish, Glimormir) plus three special areas: **Midnight** (the player's homeland, three places), the **Great Ocean**, and the **Mists of Oblivion** (no places; things sent there are sent "to the middle of the map").
- A **place** is a named region of the map. There are 131 places in all.
  - The **first 25 places** in the master list are water (rivers, lakes and seas). They hold no keep. Appendix E lists them.
  - The other 106 places each hold one keep: a **castle**, or a **citadel** for the principal stronghold of a kingdom. There are fourteen citadels, one per kingdom (Midnight, the Great Ocean and the Mists have none).
  - Two places, **Corlon** and **Elormane**, are special and are skipped by every list the behaviour builds (see 14).
- A place may belong to more than one kingdom's list. For example the river Imilvir is listed under the Witherlands, the Eldmark, the Weirdwood and the Dawnwood.
- **Neighbouring places.** Every place has a list of places it touches. Appendix C gives the full list. Neighbourhood is stored per place; two entries are one-way (Tarefyr lists Tharn but not the reverse, and Galorwain lists Fire but not the reverse), so use the table as given rather than assuming symmetry.
- **Neighbouring kingdoms.** Kingdoms touch each other according to Appendix A. It is almost symmetrical; the one-way entries are that Glimormir lists the Eldmark (the Eldmark does not list it back) and the Great Ocean lists six kingdoms that do not list it back. Use the table as given.

### 2.2 Terms used in the rules below

| Term | Meaning |
|---|---|
| **Home kingdom** | The kingdom a lord starts in and belongs to. |
| **Home place** | The place that holds the lord's own keep. The lord's home keep is where he returns to and defends. |
| **Current place** | The place the lord is standing in now (worked out from his map position; it is refreshed for every lord once a day). |
| **Near places** (also "near lands") | All places that neighbour the lord's **home place** (Appendix C), leaving out water places, Corlon and Elormane. |
| **Adjacent places** | The near places that also neighbour the lord's **current place**. If the lord has wandered far from home, there are none. |
| **Home kingdom's places** | All places listed under the lord's home kingdom (Appendix B), leaving out water places, Corlon and Elormane. |
| **Places in neighbouring kingdoms** | All places listed under every kingdom that touches the lord's current kingdom (Appendix A, B), again leaving out the exclusions. The sea and the mists are not counted. |
| **All places** | Every place from number 26 onward in the master list (see 14 about the last one), leaving out Corlon and Elormane. |
| **Original owner** of a place | The lord who first held its keep. A keep that has been captured still counts as "belonging" to this lord. |
| **Current ruler** of a place | The lord who rules its keep now. |

---

## 3. Lords and their properties

Every lord has the following. The full roster is in Appendix F.

| Property | Use |
|---|---|
| Name, people (kingdom of origin), start position | Identity |
| **Purpose** | One of five long-term roles (section 5). Never changes during play, except that a freed hostage keeps the purpose "be a hostage". |
| **Current behaviour** | One of nine behaviours (section 6). Starts as **Return Home** for every lord. Persists from day to day. |
| **Current quest** | One of the quest kinds (section 9). Starts as **Rest**, except Corleth and Arin who start on **Rescue**. |
| **Quest target** | Who or what the quest is about, or a map position. |
| **Home place / home keep** | The keep the lord defends. |
| **Allegiance** | Which kingdom's side he fights for. Used to decide who is an enemy. |
| **Available** marker | Yes or no. "Yes" means he could be asked to help someone or be recruited by a neighbour. Set and cleared by the behaviours. |
| **Personality** | A set of traits (section 4). |
| **Strength** | How hard he fights, in fighting men (section 10). |
| **Army** | Warriors and riders he leads. |
| Rank | A number used only to decide who becomes leader when two groups merge. |
| Status markers | Dead, prisoner (needing rescue), recruited into the Alliance, "not to be processed" (some special characters), and so on. |

---

## 4. Personality

There are **64 traits**: 32 good and 32 bad, arranged as 32 opposite pairs (the trait in the good list and the trait in the same position of the bad list oppose each other).

| # | Good trait | Opposite (bad) trait |
|---|---|---|
| 1 | Brave | Cowardly |
| 2 | Far-sighted | Reckless |
| 3 | Selfless | Selfish |
| 4 | Bold | Cautious |
| 5 | Kind | Cruel |
| 6 | Warm-hearted | Cold-hearted |
| 7 | Gentle | Vicious |
| 8 | Generous | Greedy |
| 9 | Quick-witted | Slow-witted |
| 10 | Talkative | Tight-lipped |
| 11 | Home-loving | Restless |
| 12 | Eager | Apathetic |
| 13 | Energetic | Languid |
| 14 | Tireless | Easily tired |
| 15 | Modest | Arrogant |
| 16 | Passionate | Passionless |
| 17 | Reliable | Headstrong |
| 18 | Gallant | Malicious |
| 19 | Charming | Repulsive |
| 20 | Loyal | Treacherous |
| 21 | Gregarious | Solitary |
| 22 | Peaceable | Bloodthirsty |
| 23 | Polite | Provocative |
| 24 | Submissive | Domineering |
| 25 | Patient | Impatient |
| 26 | Level-headed | Mad |
| 27 | Persuasive | Unconvincing |
| 28 | Knowledgeable | Ignorant |
| 29 | Naive | Sceptical |
| 30 | Hot-tempered | Thick-skinned |
| 31 | Mighty warrior | Feeble warrior |
| 32 | Superb leader | Pitiful leader |

(The pairing is by position in the original tables. The original gave the third good trait the same name as the third bad one, "selfish"; the game's character data calls the good one **Selfless**.)

**Traits the daily decisions look at** (only six): **Brave**, **Far-sighted**, **Cowardly**, **Cautious**, **Reckless**, **Selfish**. Every other trait matters only for recruiting (section 10.1) and for fighting strength (section 10.4).

---

## 5. The daily cycle

Once per **game day** (the check is made against the calendar day, so it cannot run twice in one day) the following happens, in this order.

1. **Replenish keeps.** Keeps receive their routine replenishment of soldiers. (The rules for this are not in the source; see 14.)
2. **Locate everyone.** Every lord's current place and current kingdom are worked out from his map position.
3. **Work out danger levels** for every kingdom and every place (section 7).
4. **Let each eligible lord think.** Go through every lord. **Skip** a lord if any of the following is true:
   - he is not an ordinary lord (some special figures are marked "not to be processed", and one special "siege" figure is always skipped);
   - he is the lord the player is directly controlling and the player has asked to watch (the player's own current lord is not moved by the computer);
   - **he is already doing something**: his current quest is anything other than Rest. A lord in the middle of a quest is never redirected;
   - he is a prisoner (still awaiting rescue);
   - he is Boroth;
   - Boroth is dead and the lord started in the Marish kingdom (the war is over for them);
   - his purpose is "none".

   Everyone else runs the routine for his purpose (section 6).

Between these daily thinks, quests run continuously (section 9). When a lord finishes or abandons a quest he becomes idle and is eligible at the next daily think.

---

## 6. Purposes

A purpose is a lifelong role. There are five.

| Purpose | What the lord does |
|---|---|
| **Defend homeland** | Runs the chain of behaviours described in section 8. The vast majority of lords (138 of 154) have this purpose. |
| **Dark Fey** | Boroth's own campaign of conquest. **The routine for this is not in the source** (see 14 and 8.10 for what the design says it should do). Only Boroth has it. |
| **Randomly wander** | Does nothing in the original: the routine was an empty placeholder. Two lords (Corleth and Arin) have it, but they start on a rescue quest instead. A rebuild may add a wandering behaviour (pick a reachable place a few squares away and go there). |
| **Be a hostage** | Does nothing in the original (an empty placeholder). In the supplied lord data 13 lords carry this purpose: Prince Morkin, Amarin Starchaser (Prince of the Gelm), and eleven of the twelve realm hostages (Wythran the Weaver is recorded with Defend homeland; Luxor with Defend homeland). Hostages are prisoners and are skipped by the daily think anyway; after rescue the player directs them. The data's assignments look partly unreliable, so treat the purpose column in Appendix F as a starting point. |
| **None** | Never processed. |

---

## 7. Danger levels

The behaviours are driven by how threatened a kingdom and each of its places are. The levels are worked out afresh every day (section 5, step 3).

### 7.1 Who is an enemy

Every lord has an **allegiance** (the kingdom whose side he is on). Each kingdom keeps a list of the other allegiances that it is **at war with**. A lord is an **enemy of a place** if his allegiance differs from the allegiance of the place's current ruler and the ruler's allegiance has his allegiance on its war list. Two lords are enemies of each other by the same test.

**War lists grow during play.** Whenever a lord attacks someone (a kill quest) or besieges a keep, the attacker's allegiance is added to the victim's (or keep ruler's) war list. A lord attacking his own allegiance adds nothing. Nothing in the source ever removes anyone from a war list. The starting war lists are not in the source (see 14); at the start the Dark Fey must be at war with everyone else, and everyone else at peace with each other.

### 7.2 Danger of a kingdom

For each kingdom except the Great Ocean:

1. If **every** place in the kingdom is **subjugated**, the kingdom is **Subjugated**. A place is subjugated when its current ruler is not its original owner **and** the ruler is an enemy of that owner.
2. Otherwise the kingdom starts at **At peace**.
3. If any enemy lord is standing in any of the kingdom's places, the kingdom is **Under attack** if its war list is not empty, otherwise **Threatened**.

### 7.3 Danger of a place

Only places from number 26 onward (those that hold keeps), skipping Corlon and Elormane.

1. If the place is subjugated (same test as above): **Subjugated**.
2. Otherwise, if its kingdom is **Under attack**:
   - an enemy lord stands in the place: **Under attack**;
   - else an enemy lord stands in any near place (section 2.2): **In peril**;
   - else **In danger**.
3. Otherwise, if its kingdom is **Threatened**:
   - an enemy lord stands in the place: **Menaced**;
   - else an enemy in a near place: **Threatened**;
   - else **Troubled**.
4. Otherwise **At peace**.

### 7.4 The eight place levels, in order from calm to worst

| Level | Rank |
|---|---|
| At peace | 0 |
| Troubled | 1 |
| Threatened | 2 |
| Menaced | 3 |
| In danger | 4 |
| In peril | 5 |
| Under attack | 6 |
| Subjugated | 7 |

Kingdom levels, in order: At peace, Threatened, Under attack, Subjugated.

Note that "In danger" and "In peril" are **calm** levels in a kingdom under attack (the enemy is not in or next to the place); "Under attack" means the enemy is in the place itself.

---

## 8. Behaviours: the "defend homeland" chain

A lord with the purpose Defend Homeland is always in one of **nine behaviours**. The behaviour is remembered from day to day. Each day, the lord's current behaviour is examined; it either **settles** (picks a quest or decides to wait) or **hands over** to another behaviour, which is then examined straight away, and so on. At most **13 hand-overs** are allowed per lord per day, after which the lord simply waits.

Inside a behaviour, the rules are tried **in the order listed; the first rule whose conditions are all true wins**. A rule either:
- **hands over** to another behaviour, or
- **sets a quest** (and the lord settles).

"**If idle**" means: only if the lord's current quest is Rest. Since only resting lords are examined at all (section 5), a lord is normally idle here; the test matters mainly after a hand-over that has just set a quest in the same day.

### 8.0 Conditions used in the rules

| Condition | Meaning |
|---|---|
| **Home danger is X** | The danger level of the lord's home place **is exactly** X (not "at least"). |
| **Home danger above/below X** | By the ranks in 7.4. |
| **Enemy in the same place** | At least one enemy lord is in the lord's current place. One such enemy is picked at random and remembered. |
| **Enemy in the homeland** | At least one enemy lord is in the lord's home place. One is picked at random and remembered as "the attacker". |
| **Enemy in a near place** | At least one enemy lord is in any near place. One is chosen at random. |
| **Enemy in an adjacent place** | Same, but only adjacent places. |
| **I am in my homeland / in a near place** | The lord's current place is his home place / one of his near places. |
| **At his stronghold** | The lord is at his home keep. **Odd behaviour:** in the original this test always answered "yes", whatever the position, so the "walk home" step of Return Home is never taken. See 8.1. |
| **Odds a:b against the attacker** | Compare fighting strength (section 10.4) of this lord and the opponent after allowing for tiredness. True when `opponent strength x a` is **no more than** `my strength x b`, so "3:1" is true when I am at least three times as strong. See the note under 8.2: the rules appear to use it the wrong way round. |
| **Subject is dead** | The target of the lord's current quest is a dead character. |
| **Leader is dead** | The lord's party leader is dead. |
| Brave, Cowardly, Cautious, Reckless, Selfish, Far-sighted | The lord has that trait. |
| **Someone to recruit** | Always true in the original (the real test was never written). |
| **Neighbour lord** | A lord who originally owns a place neighbouring the lord's home place (the list of places comes from Appendix C, excluding water, Corlon and Elormane). |

**Lists and filters.** Several rules build a list and then remove entries. A list is built, then each filter removes entries, then "if the list is empty" is tested, then one entry is picked **at random**. The filters used:

| Filter (what it **removes**) | Leaves behind |
|---|---|
| Places with danger at or above Under attack | Places below Under attack |
| Cowards | Lords without the Cowardly trait |
| Lords not in a near place | Lords whose current place is a near place |
| Places not Threatened | Only Threatened places |
| Places not At peace | Only At peace places |
| Lords not marked Available | Only Available lords |
| Lords not in their homeland | Only lords currently in their own home place |
| Lords not defending their homeland | Only lords whose purpose is Defend Homeland |
| Retreating lords | Only Defend Homeland lords whose behaviour is not Retreat |
| Places not In danger | Only places whose danger is In danger |

"Lords of places" means converting a list of places into a list of their original owners.

### 8.1 Return Home

The default resting behaviour.

| Order | Conditions | Result |
|---|---|---|
| 1 | Home danger is Subjugated | Hand over to **Take Back Stronghold** |
| 2 | The lord is at his stronghold (see **Odd behaviour** above: always true) | Hand over to **Stand Firm** |
| 3 | (never reached as written) mark himself Not available; if idle, set quest **Go to** his own stronghold | Settle |

Because of the odd behaviour in rule 2, a lord whose home is not subjugated moves straight to Stand Firm wherever he is standing. If you want lords who have strayed to walk home, rule 3 must be enabled by making rule 2 test the lord's real position.

### 8.2 Stand Firm

Reached by hand-over from Return Home. Rules in order:

| Order | Conditions | Result |
|---|---|---|
| 1 Gather strength | Home danger is **In peril** AND the lord is Far-sighted AND not Cowardly AND (someone to recruit: always true) | Hand over to **Gather Strength** |
| 2 Attack | Home danger is **In peril** AND ((Brave AND not Cautious) OR (Reckless AND not Cowardly)) | Hand over to **Attack Enemy** |
| 3 Help a neighbour | Home danger is **In danger** AND not Cowardly AND not Selfish AND: build the list of near places whose danger is **below Under attack**; convert to their original owners; keep only those currently at home; keep only Defend Homeland lords; remove retreating lords; the list is not empty | Pick one at random; set quest **Join** him; hand over to **Help Neighbour** |
| 4 Counter threat | Home danger is **Troubled** AND at least one near place is **Threatened** AND not Cowardly AND not Selfish | Hand over to **Counter Threat** |
| 5 Retreat | Home danger is **Under attack** AND either (Cowardly AND odds 3:4 against the attacker) OR (not Brave AND odds 3:1 against the attacker) | Hand over to **Retreat** |
| 6 Wait | Otherwise: mark **Available** unless home danger is Under attack (then mark Not available); set quest **Rest** | Settle |

**Odd behaviour (rule 3).** The list of places is "below Under attack", which includes places at peace and so can include lords who are in no trouble at all. The program's own comment says the test was meant as "at most In peril".

**Odd behaviour (rule 5).** The odds test "a:b" is true when `attacker's strength x a` is no more than `my strength x b`. For the cowardly test (3:4) that means "I am at least three-quarters as strong as the attacker"; for the other test (3:1) it means "I am at least three times as strong". Used as written, a lord would retreat when he is *not* outmatched, the opposite of what a retreat rule should do. The most plausible intent is the reverse: **a Brave lord never retreats; every other lord retreats unless he is at least 3 times as strong as the attacker; a Cowardly lord also retreats if he is less than three-quarters as strong** (which the 3:1 test already covers). This is an inference, not something the source states. Decide whether to copy the literal behaviour or the intent; the intent is the safer choice. The same inversion appears in the (unreachable) retreat rule in 8.3.

The "attacker" used in rule 5 is the enemy most recently found in the lord's home place by an earlier scan, so in the original it could be stale. A rebuild should look for a fresh enemy in the home place.

### 8.3 Take Back Stronghold

| Order | Conditions | Result |
|---|---|---|
| 1 | Home danger is **not** Subjugated | Hand over to **Stand Firm** |
| 2 | Home still Subjugated; mark Not available; if idle | Set quest **Seize** the lord's own home keep |

The original contains a retreat rule here (retreat if Cowardly and the odds against the ruler of his keep are 1:2, or if not Brave and the odds are 2:1) but it sits behind an unconditional jump and **can never run**. Ignore it unless you want to add it.

### 8.4 Attack Enemy

| Order | Conditions | Result |
|---|---|---|
| 1 | The lord is **not** in his homeland AND home danger is **Under attack** | Hand over to **Return Home** |
| 2 | **No** enemy in the homeland AND no enemy in any near place | Return Home |
| 3 | The lord is **not** in his homeland AND **is** in a near place | Return Home |
| 4 | The target of his current quest is dead | Return Home |
| 5 | Mark himself **Available**; if idle: look for an enemy in this order and set quest **Kill** against the first found: (a) in the lord's own place, (b) in the homeland, (c) in an adjacent place, (d) in a near place | Settle |
| 6 | No enemy found | Set quest **Rest** |

(Rule 3 sends a lord home if he is outside home but still in a neighbouring place; a lord far away is left alone. This reads oddly but is what the original did.)

### 8.5 Help Neighbour

Return Home if **any** of the following: home danger is Under attack; the subject (the lord being helped) is dead; the subject is no longer defending his homeland; the subject is retreating; **or** home danger is In peril and the subject's home place is In danger (**odd behaviour**: probably meant the other way round).
Otherwise: mark himself **Not available**; if idle, set quest **Join** the subject (or **Rest** if the subject has gone).

### 8.6 Retreat

| Order | Conditions | Result |
|---|---|---|
| 1 | Home danger is below Under attack | Hand over to **Return Home** |
| 2 | Mark himself **Available**; if idle, pick a safe destination, trying the following lists in order and taking the first non-empty list, choosing one place at random: (a) near places below Under attack, (b) places in the home kingdom below Under attack, (c) places in neighbouring kingdoms that are At peace, (d) any place that is At peace. Set quest **Go to** it. If none, **Rest**. | Settle |

### 8.7 Gather Strength

| Order | Conditions | Result |
|---|---|---|
| 1 | Home danger is Under attack | Return Home |
| 2 | There is an enemy in the lord's own place | Attack Enemy |
| 3 | Mark Not available; if idle: build the list of neighbour lords, remove cowards, keep only those marked Available, keep only those currently in a near place. If empty, **Rest**; else pick one at random and set quest **Recruit** him. | Settle |

### 8.8 Counter Threat

| Order | Conditions | Result |
|---|---|---|
| 1 | Home danger is **above Troubled** | Return Home |
| 2 | Mark Not available. Build the list of near places that are **Threatened**. If the list is empty, do nothing. Otherwise, if the lord is already in one of them: set quest **Rest**; else if idle, pick one at random and set quest **Go to** it. | Settle |

### 8.9 Lend Service

Return Home if home danger is Under attack or the lord's party leader is dead; otherwise mark Not available and wait. **No behaviour in the original ever hands over to Lend Service** (the rules that did so were switched off), so a rebuild may omit it. The player's design notes list it as a reaction, so it is kept here for completeness.

### 8.10 Boroth's campaign (Dark Fey purpose)

The routine for this is not in the source. The design document and the strategy text describe it:

- Boroth conquers **one kingdom at a time**, seizing every stronghold in it before turning to a **neighbouring** kingdom, then another, creeping towards Midnight and the Castle of Corelay.
- While he is attacking a kingdom, its king is unwilling to do anything but defend his own citadel, because Boroth holds the king's relative hostage. Its other lords fight bravely for their own and neighbouring castles and even counter-attack, but there is "little concerted resistance".
- Where the Alliance holds a stronghold, soldiers slowly gather there; where Boroth does, they gather for him.
- Wolves scout ahead of Boroth's armies.
- Boroth leaves **Toraneth the Cruel** in charge of the Dark Citadel when he marches.
- If Boroth is killed he rises again as a wraith in the Dark Citadel (section 13).

---

## 9. Quests

A quest is what a lord is doing right now. There are twelve kinds. The daily think (section 8) sets or leaves them; the **player** may also set them directly for his own lords. Once set, a quest runs to completion on its own without further thought until it is finished or cancelled.

| Quest | Target | The lord... |
|---|---|---|
| **Recruit** | Another lord | goes to him and tries to persuade him to join the Alliance |
| **Join** | Another lord | goes to him and joins his fellowship (the target becomes the leader) |
| **Kill** | Another lord | goes to him and fights him to the death |
| **Rescue** | (the dungeons) | searches the Dark Citadel's dungeons for hostages (only Corleth and Arin can) |
| **Follow** | Another lord | follows him around at a distance (shadowing) |
| **Go to** | A place | travels there and then rests |
| **Guard** | A place | goes there, then stands still and attacks any enemy who comes close |
| **Seize** | A keep | goes there and attacks it |
| **Find** | An artefact | goes to where it lies, or to its holder, and picks it up |
| **Take** | An artefact held by someone | goes to its holder, usually after talking to him, and takes it |
| **Destroy** | An artefact | finds it and destroys it |
| **Rest** | (none) | waits where he is |

### 9.1 Stages every moving quest shares

Most quests are a fixed sequence of stages, advanced one at a time:

1. **Wake up** (player lords only): a sleeping lord given a new quest is woken.
2. **Remember the quest** so that it can be restored after a detour (for example to find shelter when tired).
3. **Find the target**: work out where it is now and plan a journey.
4. **Travel**, step by step. If the target has moved, re-plan the journey (only when it has moved to a new map square; not every pixel).
5. **Check the target is not dead.** If it died, the quest is cancelled.
6. **Go inside** a building if the target is inside one.
7. **"About to"** pause: the lord is marked as "about to recruit/kill/..." for two short steps so the display can show it.
8. **Do the action** (below).
9. **Cancel** (quest ends, lord becomes idle).

A lord on a boat or on a dragon **cannot** recruit, join, kill or seize; the quest is cancelled with a "could not" message (and the target cannot be recruited or attacked while on one either).

**Travel when tired:** a lord who runs out of energy looks for shelter and rests; his quest is temporarily replaced by "seeking shelter" and is restored afterwards. If no shelter can be found he rests where he stands.

**Go to** has its own, shorter sequence: plan the journey, leave any building he is in, travel, go inside if the destination is a building, reach it, finish. **Rest** and **Rescue** do nothing (Rescue is performed by the dungeon exploration, not by this machinery).

### 9.2 Quest data

The quest stores up to two targets: a character, a place or object, or raw map coordinates. When an automatic quest is set by the daily think the lord is also marked "automatic" so the player can tell computer-chosen quests from his own.

### 9.3 Setting quests from the daily think

The daily think can set only these quest kinds: Recruit, Join, Kill, Go to, Seize and Rest. A Go to or Seize aimed at "his own stronghold" targets his home keep; a Kill or Recruit or Join targets the chosen lord.

---

## 10. How quest actions work

### 10.1 Recruiting

A lord on a Recruit quest reaches the target, then:

1. The program asks whether the target is willing. **The exact rule is not in the source**; the strategy guide gives it (and the replica should use it):
   - For every trait the two lords **share**, add 1 point.
   - For every trait where one has the **opposite** of the other's, subtract 1.
   - Other traits count nothing.
   - The recruiter needs **2 points** to win the target over. If the target's own kingdom's hostage has already been freed from the Dark Citadel, **1 point** is enough.
   - A total of **0** means the target simply refuses.
   - A **negative** total means the target is offended and **attacks** the recruiter.
2. If willing, it works as the **reverse of a Join**: the target is added to the recruiter's fellowship, the target's own quest is replaced by "Join the recruiter", and a "recruited" message is shown.
3. If the target already belongs to another fellowship, he is first removed from it. If two lords try to recruit **each other**, the one of higher **rank** becomes the leader.

### 10.2 Killing

1. If neither lord is already fighting, ask whether the attack should proceed (**rule not in the source**).
2. **The very act of attacking makes the attacker's allegiance an enemy of the victim's allegiance** (section 7.1).
3. If the fight can be seen by the player, a real on-screen fight begins. If not, the outcome is worked out instantly by a battle simulation (**rules not in the source**).
4. If the attacker dies his quest ends with a "has been killed" message. If the target dies the attacker's quest advances and a "has killed" message is shown.

### 10.3 Seizing a keep

A seize quest is a siege, and only **one siege may run at a time** (and none while another combat is in progress; the lord simply waits).

1. The attacker travels to the keep. If he is already inside some other building, the quest is abandoned.
2. The attackers become enemies of the keep ruler's allegiance (as in 10.2).
3. The attacker **moves inside the keep** with his whole fellowship and is linked into its occupants.
4. If he already rules the keep, nothing more happens.
5. Otherwise the keep's ruler (or, if absent, the captain of the guard) becomes his **opponent**, and the two fight, on screen or by instant simulation.
6. If the attacker dies, the siege ends and the quest is cancelled.
7. If the defender dies: **all garrisons of every race in the keep are destroyed (set to zero)**; the keep's owner (the allegiance) becomes the attacker's allegiance; the keep's current ruler becomes the attacker; a "has successfully sieged" message is shown. **If the keep is the Dark Citadel of Maranor**, a special rule fires: the player's side takes the Dark Citadel (section 13).
8. After a successful siege the lord stays in the keep awaiting orders. The conqueror's banner (his king's, or Luxor's for the Alliance) is raised.

### 10.4 Fighting strength

From the strategy guide (these numbers are not in the program text provided):

| Fighter | Strength counted as |
|---|---|
| A lord with the **Mighty warrior** trait | 100 fighting men |
| A lord with the **Feeble warrior** trait | 25 fighting men |
| Any other lord | 50 fighting men |

Plus the soldiers he leads. A **castle** counts its garrison **three times** over when defending; a **citadel** counts it **four times**. For example 500 men in a citadel fight as 2,000. Artefacts add or subtract bonuses. Tiredness (energy) reduces a lord's strength; the odds test in section 8.0 first adjusts both sides for it.

### 10.5 Guarding

A lord on a Guard quest first travels to the spot (a Guard aimed at anything but a map position is turned into a map position). Then he stands still (stops walking, speed zero) and each moment looks at the **nine squares** (his own and the eight around him) of the map grid for a character who is:

- an **enemy**, and
- **walking** (not flying or sailing), and
- **outside** a building, and
- **not already in combat**, and
- **not on a boat or dragon**, and
- **alive**.

If one is found, the Guard quest is turned into a **Kill** quest against him (his Guard position is remembered), and the lord fights him. When the kill is done the lord returns to his guard post and carries on.

### 10.6 Following, joining, finding, taking, destroying

- **Follow**: travel towards the target and keep following; if the target dies the quest is cancelled.
- **Join**: travel to the target; on arrival, merge into his fellowship (if the target is already in one, join that fellowship). When two fellowships merge, the higher-ranked leader leads.
- **Find**: travel to the artefact's position (or its holder's) and pick it up.
- **Take**: if the artefact is held by someone, travel to him; the holder may need persuading or killing first (the quest's stage list includes "kill the holder", though the original later moved that into the pick-up step), then pick it up.
- **Destroy**: as Take, then destroy the object instead of keeping it.
- A Take or Destroy first **swaps** the quest's target from the artefact to its current holder and **keeps the artefact as the subject** of the quest.

### 10.7 Messages to the player

Quests report their outcome ("recruited", "has been killed", "has killed", "could not recruit", "has successfully sieged", and so on) to the player's news. The messages for the player's directly controlled lord are shown differently from those for lords the computer is running.

---

## 11. Enmity and diplomacy summary

- There is no fixed table of who hates whom. Wars come into being by **attacks** (10.2, 10.3) and are never ended.
- Separate from war, certain **peoples will not share a garrison** (section 12.3).
- A lord's **allegiance** is the side he fights for. Recruiting a lord into the Alliance changes whose side he is on (his enemies become the Alliance's enemies).

---

## 12. Hostages and realm status

### 12.1 The hostages

Boroth holds one noble from each of twelve kingdoms in the dungeons of Maranor, plus Luxor.


| Hostage | People | Home keep |
| --- | --- | --- |
| Aloroth the Fey | Dawn Fey | Castle of Carafel |
| Djalina Snowheart | Uskarg | Castle of Theodel |
| Emedrel of the Fire | High Fey | Shimmering Citadel |
| Kiranda the Wild | Arakai | Castle of Erefar |
| Luxor the Moonprince | Free | Castle of Corelay |
| Melinissa the Sweet | Giants | Castle of Athruk |
| Mogrik the Witless | Kith | Citadel of Erifel |
| Olthruda the Bountiful | Long Dwarves | Castle of Farwain |
| Princess Aremela | Eldrin | Castle of Arelban |
| Sagrana Goldenwing | Dragonlords | Castle of Faragor |
| Thalgrima the Betrothed | Deeping Dwarves | Castle of Zaragorn |
| Wythran the Weaver | Gelmings | Citadel of Samarand |
| Zenethor the Strong | Athelings | Citadel of Varorn |

The Golden Fey have no hostage; the Dark Fey are the enemy. A Dark Fey lord has no realm to protect.

### 12.2 Status of each kingdom

Worked out every day, from the realm's king (original owner) and its hostage:

| Status | When |
|---|---|
| **Interregnum** | The kingdom's king (original owner) is dead. |
| **Blood feud** | The king lives but the hostage is dead. |
| **At ransom** | The king and the hostage live and the hostage is still a prisoner. |
| **Unfettered** | King and hostage live and the hostage has been freed. |

The checks are made in that order (the first that applies wins). The original program **computes** this status but the shown source never reads it. The design text says that while a kingdom's hostage is held, its king **will not march** and "will do nothing but defend his own citadel", and that after the hostage is freed, recruiting from that kingdom needs 1 point instead of 2.

### 12.3 Peoples that will not share a garrison

| People | Will not share a garrison with |
|---|---|
| Dragonlords (Arungor) | Giants (Delve) |
| Deeping Dwarves | Long Dwarves |
| Dawn Fey (Dawnwood) | Gelmings |
| Eldrin (Eldmark) | High Fey (Weirdwood) |
| Uskarg (Fallows) | Kith (Witherlands) |
| Arakai (Last Northing) | Athelings (Lee) |

These are mutual. (From the design letter and game data; the behaviour source itself does not use them.)

### 12.4 Dungeon rescue (design rules)

- Only **Corleth** and **Arin** can enter the Dark Citadel's dungeons; others die to its guards. A lord defeated by a dungeon troll is moved to a random place in the dungeon, except that Luxor is killed.
- Freed hostages are given a dragon and fly to their home keep.
- If the player controls Corleth or Arin directly, rescue goes about three times faster than if the computer does. Left to the computer it can take up to ten months.
- When Luxor is freed or killed, about 5,000 warriors of the Free gather at the Castle of Corelay.
- **After 12 months** of game time all hostages are killed. The game ends at day 366 if not won.

---

## 13. Victory and defeat

| Outcome | Condition |
|---|---|
| **Win** | Boroth is dead **and** the Dark Citadel of Maranor has been taken. |
| **Lose** | Luxor, Morkin, Corleth and Anderlane are all dead. |
| **Lose** | The Castle of Corelay falls to the Dark Fey. |
| **Lose** | Day 366 arrives without a win. |

Boroth, when killed, **rises once more as a wraith in the Dark Citadel**. So there are two ways to win: kill him, kill him again as a wraith in the citadel; or **take the Dark Citadel first and kill Boroth when he returns to retake it** (then he fights only once). When the Dark Citadel is seized, Boroth's followers' strength there is broken and the citadel is destroyed on his death.

Advice from the design text for a stronger replica: do not kill Boroth while Luxor is still a prisoner (the Dark Fey will execute him), and Boroth's lieutenants will pour out of the Marish when he falls.

---

## 14. What the source did not contain

A programmer rebuilding this must **choose** values for the following. They are listed with the most useful hint.

| Missing piece | Hint |
|---|---|
| Willingness to be recruited | Use the trait-points rule in 10.1. |
| When an attack goes ahead | Not recoverable; fight when the quest says so. |
| Fight resolution and strength | Use section 10.4; resolve by comparing total strength with some luck. |
| Journey planning and travel speed | Use ordinary land pathfinding; avoid water. |
| Shelter seeking and tiredness | A lord who is exhausted rests until his energy returns. |
| Boroth's campaign | Section 8.10. |
| Daily replenishment of keep garrisons | Keeps slowly gain soldiers; citadels at double the rate of castles. |
| Starting war lists | The Dark Fey are at war with everyone; no one else is at war. |
| Lords with no keep (52 of the roster) | The roster shows the keep each lord holds. A lord with no keep needs a home place; use the nearest keep of his own people. |
| Dark Fey purpose routine | Section 8.10. |
| The last place (Varorn, the Lee's citadel) | The original's loops stopped one place short, so it was skipped by every list and danger calculation. A rebuild should include it. |
| Corlon and Elormane | Two places the original excluded from all lists. Their nature is not stated; a rebuild should treat them as ordinary unless they are known to be special. |
| Where the original's war checks used the kingdom's starting allegiance rather than the current one | Not recoverable; use current allegiance. |

**Other oddities in the original** (all described above in context): the "at his stronghold" test is always true (8.1), the retreat odds read as inverted (8.2), the help-neighbour selection list includes calm places (8.2), a retreat rule in Take Back Stronghold is unreachable (8.3), the Help Neighbour peril test looks inverted (8.5), and Lend Service is unused (8.9).

---

## Appendix A. Neighbouring kingdoms

| Kingdom | Touches |
| --- | --- |
| Witherlands | Midnight, Lee, Glimormir, Eldmark |
| Lee | Witherlands, Long Mountains, Glimormir, Eldmark |
| Eldmark | Witherlands, Lee, Long Mountains, Wierdwood |
| Long Mountains | Lee, Last Northing, Dawnwood, Deeping, Wierdwood, Eldmark |
| Last Northing | Long Mountains, Dawnwood, Arungor |
| Arungor | Last Northing |
| Wierdwood | Eldmark, Long Mountains, Deeping, Gelm |
| Dawnwood | Long Mountains, Last Northing, Fallows, Marish, Deeping |
| Fallows | Dawnwood, Delve, Marish |
| Gelm | Wierdwood, Deeping, Marish |
| Deeping | Long Mountains, Dawnwood, Marish, Gelm, Wierdwood |
| Delve | Fallows, Marish |
| Marish | Gelm, Deeping, Dawnwood, Fallows, Delve |
| Glimormir | Witherlands, Lee, Eldmark |
| Great Ocean | Arungor, Last Northing, Fallows, Delve, Marish, Dawnwood |
| Midnight | Witherlands |

## Appendix B. Places in each kingdom

| Kingdom | Places |
| --- | --- |
| The Witherlands | Imilvir Sareon Eomir Erifel |
| The Lee | Eodral Thumar Sharvik Imildral Melthor Varorn Rilnor Cerevere Ceremelth |
| The Eldmark | Arelban Arunvere Meranor Erilan Imilvir |
| The Long Mountains | Erenim Isilfrey Thordroth Arelon Farwain Beomir Saradwin Scaradir Corlon |
| The Last Northing | Sharmark Sapphire Elormane Melibor Rilleon Elmir Ravenfrey Mermaids Erefar Tharran Arelon Galorbard Crimson Orcar Caramane |
| Arungor | Angelf Dragon Rildroth Erivik Sarnoth Shrygal Faragor Serpents Cormir Ashnar |
| Weirdwood | Elinvar Arelmar Merikith Imilvir Shimmering Forgotten Whispering Golden Forever Aravik Aradel |
| Dawnwood | Maralan Romiel Songs Emerthen Elorthord Imilvir Corithel Carafel |
| The Fallows | Alathor Elinbrand Arabar Theodel Cerrelm Skordroth Roreon Erilmark Athrudan |
| The Gelm | Finfyr Geremiel Emergelm Samarand Gilgrath |
| The Deeping, | Thunder Ararak Dawn Tarefyr Sharenor Tharn Ildemar Zaragorn |
| The Delve | Grorn Finrod Corth Gogrun Iron Tark Othrym Big Storms Skirol Athruk Thordrin |
| The Marish | Malgor Caradwin Dwardor Barathor Death Valgrorn Maranor Falthrang Ravens Burning |
| Glimormir | Northern Eastern Western Southern Middle Immiel |
| The Great Ocean | Galorwain Fire Weeping Eregoth Grey Silfar Skulls Ulmor Shining |
| Midnight | Corelay Jade Last |

(The Mists of Oblivion has no places. The Great Ocean's places are listed in the table above; water places are those marked in Appendix D.)

## Appendix C. Neighbouring places

Every place and the places it touches. Rows 1 to 25 are water places.

| # | Place | Touches (count) | Neighbouring places |
| --- | --- | --- | --- |
| 1 | Imilvir | 25 | Corelay, Last, Jade, Erifel, Eomir, Northern, Meranor, Erilan, Saradwin, Elinvar, Merikith, Shimmering, Forgotten, Whispering, Aradel, Scaradir, Emerthen, Carafel, Songs, Corithel, Elorthord, Southern, Aravik, Eregoth, Elinmor |
| 2 | Eodral | 6 | Rilnor, Varorn, Thumar, Sharvik, Northern, Elinmor |
| 3 | Arelon | 8 | Thordroth, Isilfrey, Rilleon, Galorbard, Erefar, Orcar, Tharran, Corlon |
| 4 | Aravik | 6 | Forever, Forgotten, Whispering, Finfyr, Geremiel, Imilvir |
| 5 | Caradwin | 11 | Tarefyr, Silfar, Dawn, Malgor, Barathor, Grorn, Thunder, Corithel, Gliwain, Athrudan, Erilmark |
| 6 | Falthrang | 9 | Gilgrath, Zaragorn, Valgrorn, Maranor, Death, Gilgrath, Burning, Ravens, Ulmor |
| 7 | Elinmor | 8 | Thumar, Sharvik, Imildral, Melthor, Varorn, Imilvir, Eodral, Rilnor |
| 8 | Tarefyr | 8 | Caradwin, Thunder, Ararak, Sharenor, Ildemar, Zaragorn, Tharn, Dawn |
| 9 | Northern | 8 | Imilvir, Middle, Eomir, Rilnor, Varorn, Eastern, Eodral, Western |
| 10 | Eastern | 6 | Northern, Southern, Middle, Varorn, Cerevere, Ceremelth |
| 11 | Southern | 6 | Western, Middle, Eastern, Imilvir, Arunvere, Meranor |
| 12 | Western | 6 | Eomir, Northern, Southern, Middle, Arelban, Arunvere |
| 13 | Middle | 5 | Northern, Southern, Eastern, Western, Immiel |
| 14 | Ulmor | 9 | Falthrang, Maranor, Ravens, Burning, Death, Tark, Athruk, Skulls, Shining |
| 15 | Skulls | 9 | Athruk, Thordrin, Ulmor, Shining, Storms, Big, Corth, Silfar, Grey |
| 16 | Shining | 4 | Burning, Ulmor, Skulls, Grey |
| 17 | Silfar | 7 | Caradwin, Erilmark, Roreon, Grey, Finrod, Corth, Skulls |
| 18 | Grey | 5 | Roreon, Weeping, Silfar, Skulls, Shining |
| 19 | Eregoth | 11 | Fire, Weeping, Roreon, Cerrelm, Arabar, Elinbrand, Alathor, Imilvir, Songs, Romiel, Caramane |
| 20 | Weeping | 8 | Fire, Ashnar, Shrygal, Eregoth, Roreon, Grey, Galorwain, Sarnoth |
| 21 | Fire | 14 | Eregoth, Weeping, Ashnar, Cormir, Melibor, Mermaids, Ravenfrey, Tharran, Caramane, Serpents, Faragor, Rildroth, Angelf, Fire |
| 22 | Galorwain | 8 | Sharmark, Melibor, Elormane, Fire, Angelf, Weeping, Erivik, Sarnoth |
| 23 | Tharran | 6 | Fire, Caramane, Arelon, Erefar, Elmir, Ravenfrey |
| 24 | Mermaids | 3 | Ravenfrey, Melibor, Fire |
| 25 | Serpents | 3 | Fire, Cormir, Faragor |
| 26 | Immiel | 1 | Middle |
| 27 | Rilleon | 9 | Arelon, Thordroth, Galorbard, Orcar, Erefar, Elmir, Sapphire, Isilfrey, Corlon |
| 28 | Maranor | 5 | Falthrang, Valgrorn, Death, Ravens, Ulmor |
| 29 | Corelay | 2 | Imilvir, Last |
| 30 | Jade | 5 | Imilvir, Last, Erifel, Eomir, Sareon |
| 31 | Last | 3 | Imilvir, Corelay, Jade |
| 32 | Erifel | 6 | Jade, Sareon, Rilnor, Eomir, Imilvir, Eomir |
| 33 | Sareon | 4 | Jade, Erifel, Rilnor, Thumar |
| 34 | Eomir | 7 | Imilvir, Northern, Western, Erifel, Rilnor, Jade, Arelban |
| 35 | Rilnor | 8 | Sareon, Erifel, Eomir, Northern, Varorn, Elinmor, Thumar, Eodral |
| 36 | Thumar | 5 | Elinmor, Sharvik, Rilnor, Sareon, Eodral |
| 37 | Sharvik | 6 | Thumar, Erenim, Imildral, Elinmor, Thumar, Eodral |
| 38 | Imildral | 6 | Sharvik, Erenim, Isilfrey, Thordroth, Melthor, Elinmor |
| 39 | Erenim | 5 | Sharmark, Sapphire, Isilfrey, Imildral, Sharvik |
| 40 | Isilfrey | 7 | Erenim, Sapphire, Rilleon, Arelon, Thordroth, Imildral, Corlon |
| 41 | Thordroth | 8 | Arelon, Isilfrey, Rilleon, Galorbard, Farwain, Melthor, Imildral, Corlon |
| 42 | Galorbard | 7 | Thordroth, Arelon, Rilleon, Orcar, Crimson, Maralan, Farwain |
| 43 | Corlon | 4 | Arelon, Thordroth, Rilleon, Isilfrey |
| 44 | Farwain | 6 | Thordroth, Galorbard, Maralan, Beomir, Ceremelth, Melthor |
| 45 | Maralan | 5 | Farwain, Galorbard, Crimson, Carafel, Beomir |
| 46 | Beomir | 7 | Ceremelth, Farwain, Maralan, Carafel, Scaradir, Saradwin, Meranor |
| 47 | Ceremelth | 6 | Cerevere, Melthor, Farwain, Beomir, Meranor, Eastern |
| 48 | Carafel | 8 | Maralan, Crimson, Songs, Elorthord, Imilvir, Emerthen, Scaradir, Beomir |
| 49 | Saradwin | 5 | Imilvir, Beomir, Scaradir, Merikith, Erilan |
| 50 | Emerthen | 5 | Carafel, Imilvir, Elorthord, Corithel, Scaradir |
| 51 | Scaradir | 9 | Imilvir, Beomir, Carafel, Emerthen, Corithel, Thunder, Ararak, Merikith, Saradwin |
| 52 | Meranor | 6 | Southern, Imilvir, Ceremelth, Erilan, Arunvere, Beomir |
| 53 | Arelban | 3 | Eomir, Western, Arunvere |
| 54 | Arunvere | 5 | Western, Southern, Arelban, Meranor, Erilan |
| 55 | Erilan | 5 | Arunvere, Meranor, Imilvir, Saradwin, Elinvar |
| 56 | Elinvar | 6 | Erilan, Imilvir, Merikith, Shimmering, Forgotten, Arelmar |
| 57 | Arelmar | 2 | Elinvar, Forgotten |
| 58 | Shimmering | 6 | Imilvir, Elinvar, Merikith, Aradel, Whispering, Forgotten |
| 59 | Forgotten | 7 | Arelmar, Elinvar, Imilvir, Shimmering, Whispering, Aravik, Forever |
| 60 | Whispering | 8 | Shimmering, Imilvir, Aradel, Golden, Finfyr, Aravik, Forever, Forgotten |
| 61 | Forever | 5 | Forgotten, Aravik, Whispering, Finfyr, Geremiel |
| 62 | Golden | 5 | Whispering, Aradel, Ararak, Sharenor, Finfyr |
| 63 | Finfyr | 6 | Whispering, Golden, Sharenor, Geremiel, Aravik, Forever |
| 64 | Geremiel | 7 | Forever, Aravik, Finfyr, Sharenor, Ildemar, Samarand, Emergelm |
| 65 | Samarand | 5 | Geremiel, Ildemar, Zaragorn, Gilgrath, Emergelm |
| 66 | Emergelm | 2 | Geremiel, Samarand |
| 67 | Ildemar | 5 | Sharenor, Tarefyr, Zaragorn, Samarand, Geremiel |
| 68 | Zaragorn | 7 | Tarefyr, Tharn, Valgrorn, Falthrang, Gilgrath, Samarand, Ildemar |
| 69 | Gilgrath | 5 | Falthrang, Valgrorn, Burning, Samarand, Zaragorn |
| 70 | Tharn | 4 | Dawn, Valgrorn, Zaragorn, tarefyr |
| 71 | Sharenor | 6 | Golden, Ararak, Tarefyr, Ildemar, Geremiel, Finfyr |
| 72 | Ararak | 6 | Aradel, Scaradir, Thunder, Tarefyr, Sharenor, Golden |
| 73 | Thunder | 6 | Scaradir, Corithel, Caradwin, Dawn, Tarefyr, Ararak |
| 74 | Dawn | 7 | Caradwin, Thunder, Malgor, Dwardor, Valgrorn, Tharn, Tarefyr |
| 75 | Merikith | 6 | Imilvir, Saradwin, Scaradir, Aradel, Shimmering, Elinvar |
| 76 | Aradel | 6 | Imilvir, Merikith, Ararak, Golden, Whispering, Shimmering |
| 77 | Valgrorn | 10 | Dwardor, Death, Maranor, Falthrang, Ravens, Gilgrath, Zaragorn, Tharn, Dawn, Burning |
| 78 | Malgor | 7 | Caradwin, Gliwain, Athrudan, Barathor, Dwardor, Dawn, Corithel |
| 79 | Dwardor | 5 | Malgor, Barathor, Death, Valgrorn, Dawn |
| 80 | Death | 8 | Barathor, Maranor, Ulmor, Falthrang, Valgrorn, Dwardor, Iron, Tark |
| 81 | Barathor | 8 | Athrudan, Caradwin, Erilmark, Grorn, Iron, Death, Dwardor, Malgor |
| 82 | Burning | 6 | Gilgrath, Falthrang, Ravens, Valgrorn, Ulmor, Shining |
| 83 | Ravens | 5 | Falthrang, Valgrorn, Maranor, Ulmor, Burning |
| 84 | Iron | 5 | Grorn, Gogrun, Tark, Death, Barathor |
| 85 | Tark | 6 | Gogrun, Othrym, Athruk, Ulmor, Death, Iron |
| 86 | Athruk | 5 | Othrym, Thordrin, Skulls, Ulmor, Tark |
| 87 | Thordrin | 5 | Othrym, Skirol, Storms, Skulls, Athruk |
| 88 | Storms | 4 | Skulls, Thordrin, Skirol, Big |
| 89 | Big | 4 | Skulls, Storms, Skirol, Corth |
| 90 | Corth | 5 | Silfar, Skulls, Big, Skirol, Finrod |
| 91 | Skirol | 7 | Corth, Big, Storms, Thordrin, Othrym, Gogrun, Finrod |
| 92 | Othrym | 6 | Gogrun, Skirol, Thordrin, Athruk, Tark, Gogrun |
| 93 | Grorn | 6 | Erilmark, Caradwin, Finrod, Gogrun, Iron, Barathor |
| 94 | Gogrun | 6 | Grorn, Finrod, Skirol, Othrym, Tark, Iron |
| 95 | Finrod | 6 | Silfar, Corth, Skirol, Gogrun, Grorn, Erilmark |
| 96 | Songs | 7 | Imilvir, Eregoth, Carafel, Crimson, Romiel, Elorthord, Alathor |
| 97 | Crimson | 6 | Songs, Romiel, Orcar, Galorbard, Maralan, Carafel |
| 98 | Elorthord | 7 | Imilvir, Elinbrand, Gliwain, Corithel, Emerthen, Carafel, Songs |
| 99 | Corithel | 8 | Imilvir, Elorthord, Gliwain, Caradwin, Malgor, Thunder, Scaradir, Emerthen |
| 100 | Alathor | 5 | Eregoth, Arabar, Elinbrand, Songs, Romiel |
| 101 | Elinbrand | 7 | Eregoth, Arabar, Alathor, Theodel, Skordroth, Gliwain, Elorthord |
| 102 | Gliwain | 7 | Elinbrand, Skordroth, Athrudan, Caradwin, Malgor, Corithel, Elorthord |
| 103 | Athrudan | 6 | Skordroth, Erilmark, Caradwin, Barathor, Malgor, Gliwain |
| 104 | Skordroth | 5 | Theodel, Erilmark, Athrudan, Gliwain, Elinbrand |
| 105 | Erilmark | 10 | Cerrelm, Roreon, Silfar, Finrod, Grorn, Caradwin, Barathor, Athrudan, Skordroth, Theodel |
| 106 | Arabar | 5 | Eregoth, Alathor, Cerrelm, Theodel, Elinbrand |
| 107 | Theodel | 5 | Arabar, Cerrelm, Erilmark, Skordroth, Elinbrand |
| 108 | Cerrelm | 5 | Eregoth, Roreon, Erilmark, Theodel, Arabar |
| 109 | Roreon | 6 | Eregoth, Weeping, Grey, Silfar, Erilmark, Cerrelm |
| 110 | Caramane | 5 | Tharran, Fire, Eregoth, Romiel, Orcar |
| 111 | Romiel | 6 | Orcar, Caramane, Eregoth, Alathor, Songs, Crimson |
| 112 | Orcar | 7 | Erefar, Arelon, Caramane, Romiel, Crimson, Galorbard, Rilleon |
| 113 | Ravenfrey | 5 | Melibor, Mermaids, Fire, Tharran, Elmir |
| 114 | Erefar | 5 | Elmir, Tharran, Orcar, Arelon, Rilleon |
| 115 | Elmir | 6 | Sapphire, Melibor, Ravenfrey, Tharran, Erefar, Rilleon |
| 116 | Melibor | 10 | Elormane, Galorwain, Mermaids, Angelf, Rildroth, Fire, Ravenfrey, Elmir, Sapphire, Sharmark |
| 117 | Sharmark | 5 | Galorwain, Elormane, Melibor, Sapphire, Erenim |
| 118 | Sapphire | 6 | Sharmark, Melibor, Elmir, Rilleon, Isilfrey, Erenim |
| 119 | Elormane | 3 | Galorwain, Melibor, Sharmark |
| 120 | Dragon | 6 | Angelf, Erivik, Sarnoth, Shrygal, Rildroth, Faragor |
| 121 | Rildroth | 5 | Fire, Melibor, Angelf, Dragon, Faragor |
| 122 | Angelf | 6 | Galorwain, Dragon, Fire, Melibor, Erivik, Rildroth |
| 123 | Erivik | 4 | Angelf, Galorwain, Sarnoth, Dragon |
| 124 | Sarnoth | 5 | Erivik, Galorwain, Shrygal, Weeping, Dragon |
| 125 | Shrygal | 5 | Weeping, Ashnar, Faragor, Dragon, Sarnoth |
| 126 | Faragor | 7 | Fire, Serpents, Rildroth, Dragon, Shrygal, Ashnar, Cormir |
| 127 | Ashnar | 5 | Weeping, Shrygal, Faragor, Cormir, Fire |
| 128 | Cormir | 4 | Serpents, Fire, Faragor, Ashnar |
| 129 | Cerevere | 4 | Varorn, Melthor, Ceremelth, Eastern |
| 130 | Melthor | 7 | Imildral, Thordroth, Farwain, Ceremelth, Cerevere, Varorn, Elinmor |
| 131 | Varorn | 7 | Elinmor, Rilnor, Eodral, Northern, Eastern, Cerevere, Melthor |
| 132 | Mists Oblivion | 0 |  |

## Appendix D. Keeps (castles and citadels)

Only places with keeps are listed. The 25 water places (numbers 1 to 25 above) have no keep. "Garrison" is the starting number of soldiers. The "starting holder" is the lord who holds it at the beginning of the game.

| No. | Keep | Kind | Kingdom | People | Map square (x,y) | Starting holder | Garrison |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 26 | Citadel of Immiel | Citadel | Glimormir | Golden Fey | 47,71 | Galahar the Calm | 106 |
| 27 | Citadel of Rilleon | Citadel | The Last Northing | Arakai | 149,52 | Arak the Avenger | 100 |
| 28 | Citadel of Maranor | Citadel | The Marish | Dark Fey | 145,220 | Toraneth the Cruel | 10016 |
| 29 | Castle of Corelay | Castle | Midnight | Free (Midnight) | 7,12 | Prince Morkin | 55 |
| 30 | Castle of Jade | Castle | Midnight | Free (Midnight) | 11,23 | Prince Morkin | 46 |
| 31 | Castle of Last | Castle | Midnight | Free (Midnight) | 14,10 | Prince Morkin | 43 |
| 32 | Citadel of Erifel | Citadel | The Witherlands | Kith | 33,24 | Sharila Suresword | 95 |
| 33 | Castle of Sareon | Castle | The Witherlands | Kith | 54,11 | Talmar the Quiet | 46 |
| 34 | Castle of Eomir | Castle | The Witherlands | Kith | 19,50 | Orabrin Lionsblood | 51 |
| 35 | Castle of Rilnor | Castle | The Lee | Atheling | 62,39 | Garamor the Fat | 58 |
| 36 | Castle of Thumar | Castle | The Lee | Atheling | 105,14 | Eothor Sparehand | 59 |
| 37 | Castle of Sharvik | Castle | The Lee | Atheling | 120,14 | Forthar the Hunter | 41 |
| 38 | Castle of Imildral | Castle | The Lee | Atheling | 129,37 | Aranor Boldsword | 53 |
| 39 | Castle of Erenim | Castle | The Long Mountains | Long Dwarf | 151,15 | Sadlak the Merry | 40 |
| 40 | Castle of Isilfrey | Castle | The Long Mountains | Long Dwarf | 147,31 | Rugrak Firmaxe | 51 |
| 41 | Castle of Thordroth | Castle | The Long Mountains | Long Dwarf | 126,71 | Alagrim Ironaxe | 51 |
| 42 | Castle of Galorbard | Castle | The Last Northing | Arakai | 128,96 | Corlane the Bear | 55 |
| 44 | Castle of Farwain | Castle | The Long Mountains | Long Dwarf | 106,94 | Olthruda the Bountiful | 41 |
| 45 | Castle of Maralan | Castle | Dawnwood | Dawn Fey | 112,106 | Ildrar the Stubborn | 41 |
| 46 | Citadel of Beomir | Citadel | The Long Mountains | Long Dwarf | 73,115 | Oglim the Lonely | 93 |
| 47 | Castle of Ceremelth | Castle | The Lee | Atheling | 77,93 | Ilvar the Penitent | 59 |
| 48 | Castle of Carafel | Castle | Dawnwood | Dawn Fey | 91,121 | Aloroth the Fey | 42 |
| 49 | Castle of Saradwin | Castle | The Long Mountains | Long Dwarf | 60,129 | Grumrud Slowaxe | 40 |
| 50 | Castle of Emerthen | Castle | Dawnwood | Dawn Fey | 93,137 | Itar the Green | 45 |
| 51 | Castle of Scaradir | Castle | The Long Mountains | Long Dwarf | 74,142 | Ulgrud the Treacherous | 54 |
| 52 | Castle of Meranor | Castle | Eldmark | Eldrin | 59,99 | Sherinar of Shadows | 54 |
| 53 | Castle of Arelban | Castle | Eldmark | Eldrin | 8,73 | Princess Aremela | 59 |
| 54 | Citadel of Arunvere | Citadel | Eldmark | Eldrin | 21,105 | Queen Carithila | 104 |
| 55 | Castle of Erilan | Castle | Eldmark | Eldrin | 28,123 | Avila the Cold | 45 |
| 56 | Castle of Elinvar | Castle | Wierdwood | High Fey | 18,128 | Arithel the Joybringer | 43 |
| 57 | Castle of Arelmar | Castle | Wierdwood | High Fey | 8,142 | Melkrith Nightshade | 47 |
| 58 | Citadel of Shimmering | Citadel | Wierdwood | High Fey | 41,153 | Jarleth the Shining | 103 |
| 59 | Castle of Forgotten | Castle | Wierdwood | High Fey | 19,158 | Asholeth the Fey | 58 |
| 60 | Castle of Whispering | Castle | Wierdwood | High Fey | 19,169 | Ilareth the Chosen | 46 |
| 61 | Castle of Forever | Castle | Wierdwood | High Fey | 13,181 | Andremar the Starborn | 47 |
| 62 | Castle of Golden | Castle | Wierdwood | High Fey | 48,175 | Galdreth the Fair | 48 |
| 63 | Castle of Finfyr | Castle | The Gelm | Gelming | 31,190 | Wythran the Weaver | 56 |
| 64 | Castle of Geremiel | Castle | The Gelm | Gelming | 14,219 | Hoon the Warrior | 40 |
| 65 | Citadel of Samarand | Citadel | The Gelm | Gelming | 36,246 | Amarin Starchaser | 92 |
| 66 | Castle of Emergelm | Castle | The Gelm | Gelming | 7,237 | Elessa of the Mists | 58 |
| 67 | Castle of Ildemar | Castle | The Deeping | Deeping Dwarf | 55,220 | Boragrim Sharpaxe | 43 |
| 68 | Castle of Zaragorn | Castle | The Deeping | Deeping Dwarf | 79,241 | Thalgrima the Betrothed | 57 |
| 69 | Castle of Gilgrath | Castle | The Gelm | Gelming | 91,244 | Marik Silktongue | 56 |
| 70 | Castle of Tharn | Castle | The Deeping | Deeping Dwarf | 90,204 | Ulgrim the Weary | 50 |
| 71 | Castle of Sharenor | Castle | The Deeping | Deeping Dwarf | 40,204 | Mograk Rustaxe | 47 |
| 72 | Castle of Ararak | Castle | The Deeping | Deeping Dwarf | 71,174 | Golgud the Reluctant | 45 |
| 73 | Citadel of Thunder | Citadel | The Deeping | Deeping Dwarf | 88,162 | Norgrim the Rock | 96 |
| 74 | Castle of Dawn | Castle | The Deeping | Deeping Dwarf | 98,187 | Aluthrim the Bold | 42 |
| 75 | Castle of Merikith | Castle | Wierdwood | High Fey | 52,140 | Emedrel of the Fire | 43 |
| 76 | Castle of Aradel | Castle | Wierdwood | High Fey | 60,159 | Alargrith Warhelm | 43 |
| 77 | Castle of Valgrorn | Castle | The Marish | Dark Fey | 120,206 | Skyrdreth Iceheart | 59 |
| 78 | Castle of Malgor | Castle | The Marish | Dark Fey | 134,182 | Urskreth the Vile | 50 |
| 79 | Castle of Dwardor | Castle | The Marish | Dark Fey | 118,188 | Mirgrath the Black | 53 |
| 80 | Castle of Death | Castle | The Marish | Dark Fey | 133,199 | Dargrith the Butcher | 54 |
| 81 | Castle of Barathor | Castle | The Marish | Dark Fey | 171,190 | Akrith Bloodhand | 59 |
| 82 | Castle of Burning | Castle | The Marish | Dark Fey | 150,247 | Urgoreth the Despiser | 52 |
| 83 | Castle of Ravens | Castle | The Marish | Dark Fey | 131,234 | Graleth the Bitter | 48 |
| 84 | Castle of Iron | Castle | The Delve | Giant | 174,205 | Muglum the Handsome | 45 |
| 85 | Castle of Tark | Castle | The Delve | Giant | 185,214 | Slorum the Smug | 46 |
| 86 | Castle of Athruk | Castle | The Delve | Giant | 190,227 | Melinissa the Sweet | 53 |
| 87 | Castle of Thordrin | Castle | The Delve | Giant | 205,227 | Morgrissa Hammertongue | 48 |
| 88 | Castle of Storms | Castle | The Delve | Giant | 232,202 | Snorglum Bighammer | 50 |
| 89 | Castle of Big | Castle | The Delve | Giant | 222,201 | Borlum the Happy | 53 |
| 90 | Castle of Corth | Castle | The Delve | Giant | 208,198 | Olagrum the Steady | 41 |
| 91 | Citadel of Skirol | Citadel | The Delve | Giant | 210,211 | Borgalug Bonecrusher | 105 |
| 92 | Castle of Othrym | Castle | The Delve | Giant | 196,214 | Oglissa the Rose | 52 |
| 93 | Castle of Grorn | Castle | The Delve | Giant | 186,190 | Stublog Ironskull | 46 |
| 94 | Castle of Gogrun | Castle | The Delve | Giant | 187,203 | Gothrum the Glum | 56 |
| 95 | Castle of Finrod | Castle | The Delve | Giant | 199,187 | Olog the Friendly | 51 |
| 96 | Castle of Songs | Castle | Dawnwood | Dawn Fey | 130,121 | Melinoth Larkstongue | 54 |
| 97 | Castle of Crimson | Castle | The Last Northing | Arakai | 149,96 | Anderlane of the Arakai | 45 |
| 98 | Citadel of Elorthord | Citadel | Dawnwood | Dawn Fey | 116,133 | Miranar Fairhand | 94 |
| 99 | Castle of Corithel | Castle | Dawnwood | Dawn Fey | 98,150 | Taroleth the Jester | 56 |
| 100 | Castle of Alathor | Castle | The Fallows | Uskarg | 164,123 | Elina the Enchantress | 56 |
| 101 | Castle of Elinbrand | Castle | The Fallows | Uskarg | 140,144 | Darath the Lion | 50 |
| 102 | Castle of Gliwain | Castle | The Marish | Dark Fey | 128,155 | Yrgreth Deathbringer | 57 |
| 103 | Castle of Athrudan | Castle | The Fallows | Uskarg | 149,187 | Skarai the Dreamer | 46 |
| 104 | Castle of Skordroth | Castle | The Fallows | Uskarg | 147,157 | Haraglai Stormgut | 53 |
| 105 | Castle of Erilmark | Castle | The Fallows | Uskarg | 179,172 | Moglai Firewolf | 59 |
| 106 | Castle of Arabar | Castle | The Fallows | Uskarg | 163,132 | Khalak the Blue | 54 |
| 107 | Castle of Theodel | Castle | The Fallows | Uskarg | 162,153 | Djalina Snowheart | 45 |
| 108 | Castle of Cerrelm | Castle | The Fallows | Uskarg | 187,147 | Thorgran Wildsword | 58 |
| 109 | Citadel of Roreon | Citadel | The Fallows | Uskarg | 225,132 | Asunai Ironwing | 91 |
| 110 | Castle of Caramane | Castle | The Last Northing | Arakai | 205,79 | Rainar the Besotted | 42 |
| 111 | Castle of Romiel | Castle | Dawnwood | Dawn Fey | 170,100 | Morgreth the Unsure | 43 |
| 112 | Castle of Orcar | Castle | The Last Northing | Arakai | 171,87 | Arbethor Greenhand | 43 |
| 113 | Castle of Ravenfrey | Castle | The Last Northing | Arakai | 190,60 | Morathron the Sorceror | 51 |
| 114 | Castle of Erefar | Castle | The Last Northing | Arakai | 160,74 | Kiranda the Wild | 45 |
| 115 | Castle of Elmir | Castle | The Last Northing | Arakai | 178,45 | Merithel of the Lake | 53 |
| 116 | Castle of Melibor | Castle | The Last Northing | Arakai | 207,35 | Storbold the Scribbler | 43 |
| 117 | Castle of Sharmark | Castle | The Last Northing | Arakai | 174,13 | Brunak the Wanderer | 53 |
| 118 | Castle of Sapphire | Castle | The Last Northing | Arakai | 158,20 | Guthrane Oakfist | 59 |
| 120 | Castle of Dragon | Castle | Arungor | Dragonlord | 230,40 | Torgrim Arrowhand | 43 |
| 121 | Castle of Rildroth | Castle | Arungor | Dragonlord | 222,45 | Moongrim Longneck | 44 |
| 122 | Castle of Angelf | Castle | Arungor | Dragonlord | 236,16 | Sparthor the Patient | 47 |
| 123 | Castle of Erivik | Castle | Arungor | Dragonlord | 238,41 | Kargrim the Cautious | 45 |
| 124 | Castle of Sarnoth | Castle | Arungor | Dragonlord | 241,63 | Oragrane the Fearless | 51 |
| 125 | Castle of Shrygal | Castle | Arungor | Dragonlord | 240,74 | Jaranor the Hasty | 42 |
| 126 | Castle of Faragor | Castle | Arungor | Dragonlord | 229,81 | Sagrana Goldenwing | 45 |
| 127 | Citadel of Ashnar | Citadel | Arungor | Dragonlord | 236,106 | Galagrim of the Flame | 109 |
| 128 | Castle of Cormir | Castle | Arungor | Dragonlord | 219,91 | Scirane the Swift | 43 |
| 129 | Castle of Cerevere | Castle | The Lee | Atheling | 94,71 | Dorok the Dour | 54 |
| 130 | Castle of Melthor | Castle | The Lee | Atheling | 116,64 | Marathor the Splendid | 45 |
| 131 | Citadel of Varorn | Citadel | The Lee | Atheling | 70,56 | Zenethor the Strong | 97 |

## Appendix E. Water places (no keep)


Imilvir, Eodral, Arelon, Aravik, Caradwin, Falthrang, Elinmor, Tarefyr, Northern, Eastern, Southern, Western, Middle, Ulmor, Skulls, Shining, Silfar, Grey, Eregoth, Weeping, Fire, Galorwain, Tharran, Mermaids, Serpents.

## Appendix F. The lords

Personalities use the 64 traits of section 4. "Start" is the starting map square (x,y); lords at 145,220 begin in the dungeons of the Dark Citadel. "Keep" is the keep the lord is recorded as holding in the game data (blank if none; see section 14). "Soldiers" is the number of warriors he starts with. Abbreviated purposes are as in section 6.

| No. | Lord | People | Start | Keep | Purpose | Starting quest | Soldiers | Title | Personality |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | Prince Morkin | House of Moon | 8,12 | Castle of Corelay, Castle of Jade, Castle of Last | Be a hostage | Rest | 195 |  | Bold, Tireless, Reliable, Gallant |
| 2 | Corleth of Corelay | Fey (Midnight) | 145,220 |  | Randomly wander | Rescue | 0 |  | Selfless, Talkative, Reliable |
| 3 | Anderlane of the Arakai | Arakai | 149,97 | Castle of Crimson | Defend homeland | Rest | 80 |  | Brave, Bold, Eager, Passionate, Loyal, Hot-tempered, Mighty warrior, Headstrong |
| 4 | Rorthron the Wise | Wise | 71,57 |  | Defend homeland | Rest | 0 |  | Brave, Far-sighted, Quick-witted, Loyal, Peaceable, Patient, Persuasive, Knowledgeable |
| 5 | Itar the Green | Dawn Fey | 93,137 | Castle of Emerthen | Defend homeland | Rest | 65 |  | Far-sighted, Selfless, Gentle |
| 6 | Arin Lord Blood | Free (Midnight) | 145,220 |  | Randomly wander | Rescue | 0 |  | Eager, Energetic, Charming, Hot-tempered |
| 7 | Galagrim of the Flame | Dragonlord | 236,107 | Citadel of Ashnar | Defend homeland | Rest | 185 | Dragon King of Arungor | Persuasive, Superb leader, Headstrong, Thick-skinned |
| 8 | Luxor the Moonprince | Moonprince | 145,220 |  | Defend homeland | Rest | 0 |  | Brave, Bold, Kind, Superb leader |
| 9 | Kargrim the Cautious | Dragonlord | 239,41 | Castle of Erivik | Defend homeland | Rest | 85 |  | Quick-witted, Modest, Cautious |
| 10 | Morathron the Sorceror | Arakai | 191,61 | Castle of Ravenfrey | Defend homeland | Rest | 75 |  | Far-sighted, Cold-hearted, Treacherous, Impatient |
| 11 | Guthrane Oakfist | Arakai | 157,20 | Castle of Sapphire | Defend homeland | Rest | 65 |  | Hot-tempered, Headstrong, Impatient |
| 12 | Djalina Snowheart | Uskarg | 145,220 | Castle of Theodel | Be a hostage | Rest | 0 |  | Bold, Energetic, Gallant, Reckless |
| 13 | Grumrud Slowaxe | Long Dwarf | 59,130 | Castle of Saradwin | Defend homeland | Rest | 75 |  | Bold, Kind, Charming |
| 14 | Asholeth the Fey | High Fey | 18,159 | Castle of Forgotten | Defend homeland | Rest | 70 |  | Selfless, Eager, Persuasive |
| 15 | Boroth the Wolfheart | Dark Fey | 145,220 |  | Dark Fey | Rest | 10050 | High King of the Marish | Superb leader, Cruel, Vicious, Bloodthirsty |
| 16 | Ildrar the Stubborn | Dawn Fey | 112,107 | Castle of Maralan | Defend homeland | Rest | 70 |  | Charming, Reckless, Headstrong |
| 17 | Mogrik the Witless | Kith | 145,220 |  | Be a hostage | Rest | 0 | Prince of the Witherlands | Warm-hearted, Home-loving, Slow-witted |
| 18 | Torgrim Arrowhand | Dragonlord | 231,40 | Castle of Dragon | Defend homeland | Rest | 75 |  | Selfless, Cold-hearted, Restless |
| 19 | Uthran the Meddler | Dragonlord | 230,40 |  | Defend homeland | Rest | 0 |  | Talkative, Persuasive |
| 20 | Marik Silktongue | Gelming | 91,245 | Castle of Gilgrath | Defend homeland | Rest | 75 |  | Modest, Persuasive, Naive |
| 21 | Haraglai Stormgut | Uskarg | 147,158 | Castle of Skordroth | Defend homeland | Rest | 80 |  | Talkative, Hot-tempered, Reckless |
| 22 | Oglim the Lonely | Long Dwarf | 73,115 | Citadel of Beomir | Defend homeland | Rest | 170 | King of the Long Mountains | Brave, Mighty warrior, Headstrong, Solitary |
| 23 | Aloroth the Fey | Dawn Fey | 145,220 | Castle of Carafel | Be a hostage | Rest | 0 |  | Generous, Eager, Impatient |
| 24 | Yrgreth Deathbringer | Dark Fey | 129,155 | Castle of Gliwain | Defend homeland | Rest | 450 |  | Quick-witted, Vicious, Greedy |
| 25 | Stublog Ironskull | Giant | 186,191 | Castle of Grorn | Defend homeland | Rest | 58 |  | Talkative, Naive, Reckless |
| 26 | Orabrin Lionsblood | Kith | 19,50 | Castle of Eomir | Defend homeland | Rest | 95 |  | Brave, Gallant, Knowledgeable |
| 27 | Sparthor the Patient | Dragonlord | 237,16 | Castle of Angelf | Defend homeland | Rest | 70 |  | Loyal, Polite, Patient |
| 28 | Cadron the Bemused | Kith | 19,50 |  | Defend homeland | Rest | 0 |  | Peaceable, Slow-witted |
| 29 | Brunak the Wanderer | Arakai | 174,13 | Castle of Sharmark | Defend homeland | Rest | 60 |  | Bold, Eager, Tireless, Restless |
| 30 | Skarai the Dreamer | Uskarg | 149,187 | Castle of Athrudan | Defend homeland | Rest | 80 |  | Generous, Passionate, Peaceable |
| 31 | Rugrak Firmaxe | Long Dwarf | 147,31 | Castle of Isilfrey | Defend homeland | Rest | 80 |  | Loyal, Reckless |
| 32 | Morgreth the Unsure | Dawn Fey | 171,100 | Castle of Romiel | Defend homeland | Rest | 60 |  | Level-headed, Cautious, Tight-lipped |
| 33 | Graleth the Bitter | Dark Fey | 131,234 | Castle of Ravens | Defend homeland | Rest | 500 |  | Cruel, Cold-hearted, Ignorant |
| 34 | Muglum the Handsome | Giant | 174,205 | Castle of Iron | Defend homeland | Rest | 65 |  | Far-sighted, Arrogant, Headstrong |
| 35 | Dorok the Dour | Atheling | 94,72 | Castle of Cerevere | Defend homeland | Rest | 80 |  | Brave, Passionate, Arrogant |
| 36 | Scirane the Swift | Dragonlord | 219,92 | Castle of Cormir | Defend homeland | Rest | 75 |  | Energetic, Gallant, Hot-tempered |
| 37 | Sadlak the Merry | Long Dwarf | 151,15 | Castle of Erenim | Defend homeland | Rest | 55 |  | Talkative, Home-loving, Charming |
| 38 | Corlane the Bear | Arakai | 128,96 | Castle of Galorbard | Defend homeland | Rest | 70 |  | Brave, Generous, Tireless, Gallant, Loyal, Mighty warrior, Reckless |
| 39 | Moglai Firewolf | Uskarg | 180,172 | Castle of Erilmark | Defend homeland | Rest | 70 |  | Brave, Energetic, Arrogant |
| 40 | Norgrim the Rock | Deeping Dwarf | 88,162 | Citadel of Thunder | Defend homeland | Rest | 185 | King of the Deeping | Brave, Bold, Hot-tempered, Mighty warrior |
| 41 | Jarleth the Shining | High Fey | 41,153 | Citadel of Shimmering | Defend homeland | Rest | 180 | High King of the Weirdwood | Quick-witted, Knowledgeable, Superb leader, Cautious |
| 42 | Urskreth the Vile | Dark Fey | 134,182 | Castle of Malgor | Defend homeland | Rest | 455 |  | Arrogant, Repulsive, Provocative |
| 43 | Borgalug Bonecrusher | Giant | 210,212 | Citadel of Skirol | Defend homeland | Rest | 190 | Great King of the Delve | Greedy, Restless, Ignorant |
| 44 | Aranor Boldsword | Atheling | 130,37 | Castle of Imildral | Defend homeland | Rest | 90 |  | Bold, Quick-witted, Reckless |
| 45 | Moongrim Longneck | Dragonlord | 221,46 | Castle of Rildroth | Defend homeland | Rest | 75 |  | Gallant, Provocative, Impatient |
| 46 | Leonik Leatherhand | Deeping Dwarf | 88,162 |  | Defend homeland | Rest | 0 |  | Arrogant, Sceptical |
| 47 | Storbold the Scribbler | Arakai | 207,36 | Castle of Melibor | Defend homeland | Rest | 55 |  | Quick-witted, Reliable, Loyal |
| 48 | Arbethor Greenhand | Arakai | 170,88 | Castle of Orcar | Defend homeland | Rest | 70 |  | Home-loving, Reliable, Loyal |
| 49 | Amarin Starchaser | Gelming | 36,246 | Citadel of Samarand | Be a hostage | Rest | 195 | Prince of the Gelm | Tireless, Charming, Persuasive, Mighty warrior |
| 50 | Avila the Cold | Eldrin | 28,123 | Castle of Erilan | Defend homeland | Rest | 85 |  | Home-loving, Naive, Cold-hearted |
| 51 | Hoon the Warrior | Gelming | 15,219 | Castle of Geremiel | Defend homeland | Rest | 65 |  | Brave, Modest, Loyal |
| 52 | Eothor Sparehand | Atheling | 105,15 | Castle of Thumar | Defend homeland | Rest | 70 |  | Bold, Kind, Reckless |
| 53 | Snorglum Bighammer | Giant | 233,203 | Castle of Storms | Defend homeland | Rest | 60 |  | Brave, Hot-tempered, Greedy |
| 54 | Orgrotha the Persuader | Dark Fey | 145,220 |  | Defend homeland | Rest | 510 |  | Quick-witted, Persuasive, Bloodthirsty |
| 55 | Forthar the Hunter | Atheling | 119,15 | Castle of Sharvik | Defend homeland | Rest | 85 |  | Generous, Eager, Modest |
| 56 | Joruk Redfist | Atheling | 120,14 |  | Defend homeland | Rest | 0 |  | Tireless, Naive |
| 57 | Var the Swordsman | Gelming | 14,219 |  | Defend homeland | Rest | 0 |  | Hot-tempered, Mighty warrior |
| 58 | Wythran the Weaver | Gelming | 145,220 | Castle of Finfyr | Defend homeland | Rest | 0 |  | Reliable, Cautious, Tight-lipped |
| 59 | Queen Carithila | Eldrin | 21,106 | Citadel of Arunvere | Defend homeland | Rest | 190 | Queen of the Eldmark | Kind, Knowledgeable, Cautious |
| 60 | Garfin Quicklip | Eldrin | 21,105 |  | Defend homeland | Rest | 0 |  | Quick-witted, Cowardly |
| 61 | Zenethor the Strong | Atheling | 145,220 | Citadel of Varorn | Be a hostage | Rest | 0 | King of the Athelings | Brave, Passionate, Mighty warrior, Superb leader |
| 62 | Gothrum the Glum | Giant | 187,204 | Castle of Gogrun | Defend homeland | Rest | 65 |  | Selfless, Slow-witted, Solitary |
| 63 | Skyrdreth Iceheart | Dark Fey | 119,207 | Castle of Valgrorn | Defend homeland | Rest | 550 |  | Cold-hearted, Repulsive, Bloodthirsty |
| 64 | Lanklin the Boaster | Giant | 187,203 |  | Defend homeland | Rest | 0 |  | Arrogant, Passionless |
| 65 | Holdar Longeye | Uskarg | 140,144 |  | Defend homeland | Rest | 0 |  | Far-sighted, Patient |
| 66 | Aramila the Seer | Arakai | 178,45 |  | Defend homeland | Rest | 0 |  | Talkative, Knowledgeable |
| 67 | Melinoth Larkstongue | Dawn Fey | 130,122 | Castle of Songs | Defend homeland | Rest | 70 |  | Far-sighted, Home-loving, Tireless |
| 68 | Mana the Huntress | Eldrin | 8,73 |  | Defend homeland | Rest | 0 |  | Warm-hearted, Energetic |
| 69 | Rorlbar the Poet | Gelming | 31,190 |  | Defend homeland | Rest | 0 |  | Charming, Sceptical |
| 70 | Garamor the Fat | Atheling | 63,40 | Castle of Rilnor | Defend homeland | Rest | 70 |  | Selfless, Easily tired, Impatient |
| 71 | Borlum the Happy | Giant | 222,202 | Castle of Big | Defend homeland | Rest | 75 |  | Kind, Eager, Cautious |
| 72 | Akrith Bloodhand | Dark Fey | 170,191 | Castle of Barathor | Defend homeland | Rest | 545 |  | Reckless, Cruel, Passionless |
| 73 | Godrold Heavyhand | Arakai | 149,52 |  | Defend homeland | Rest | 0 |  | Kind, Gallant |
| 74 | Darath the Lion | Uskarg | 140,144 | Castle of Elinbrand | Defend homeland | Rest | 85 |  | Far-sighted, Bold, Generous |
| 75 | Parik the Miser | Uskarg | 225,132 |  | Defend homeland | Rest | 0 |  | Cruel, Ignorant |
| 76 | Merithel of the Lake | Arakai | 178,46 | Castle of Elmir | Defend homeland | Rest | 75 |  | Warm-hearted, Eager, Passionate, Reckless, Greedy, Headstrong, Impatient |
| 77 | Princess Aremela | Eldrin | 145,220 | Castle of Arelban | Be a hostage | Rest | 0 |  | Far-sighted, Patient, Tight-lipped |
| 78 | Farlik Whiteknife | Long Dwarf | 151,15 |  | Defend homeland | Rest | 0 |  | Brave, Reliable |
| 79 | Arak the Avenger | Arakai | 150,53 | Citadel of Rilleon | Defend homeland | Rest | 180 | King of the Last Northing | Brave, Quick-witted, Energetic, Superb leader, Restless, Impatient |
| 80 | Toraneth the Cruel | Dark Fey | 145,220 | Citadel of Maranor | Defend homeland | Rest | 530 |  | Selfish, Cruel, Bloodthirsty |
| 81 | Dargrith the Butcher | Dark Fey | 134,200 | Castle of Death | Defend homeland | Rest | 495 |  | Malicious, Bloodthirsty, Feeble warrior |
| 82 | Ogrin Woodenblade | Dawn Fey | 93,137 |  | Defend homeland | Rest | 0 |  | Naive, Selfish |
| 83 | Arag Drythroat | Dawn Fey | 130,121 |  | Defend homeland | Rest | 0 |  | Talkative, Unconvincing |
| 84 | Justrik the Hawk | Arakai | 205,79 |  | Defend homeland | Rest | 0 |  | Quick-witted, Cautious |
| 85 | Sharila Suresword | Kith | 33,25 | Citadel of Erifel | Defend homeland | Rest | 190 | Princess Regent of the Witherlands | Eager, Loyal, Hot-tempered |
| 86 | Rainar the Besotted | Arakai | 205,80 | Castle of Caramane | Defend homeland | Rest | 70 |  | Eager, Energetic, Passionate |
| 87 | Jaranor the Hasty | Dragonlord | 240,75 | Castle of Shrygal | Defend homeland | Rest | 80 |  | Brave, Hot-tempered, Impatient |
| 88 | Asunai Ironwing | Uskarg | 225,132 | Citadel of Roreon | Defend homeland | Rest | 200 | King of the Fallows | Brave, Home-loving, Passionate, Mighty warrior |
| 89 | Urgoreth the Despiser | Dark Fey | 150,247 | Castle of Burning | Defend homeland | Rest | 515 |  | Cautious, Greedy, Bloodthirsty |
| 90 | Mirgrath the Black | Dark Fey | 117,188 | Castle of Dwardor | Defend homeland | Rest | 485 |  | Easily tired, Repulsive, Solitary |
| 91 | Mograk Rustaxe | Deeping Dwarf | 40,204 | Castle of Sharenor | Defend homeland | Rest | 80 |  | Bold, Gallant, Restless |
| 92 | Taroleth the Jester | Dawn Fey | 98,150 | Castle of Corithel | Defend homeland | Rest | 55 |  | Quick-witted, Cowardly, Mad |
| 93 | Arfold Longtooth | Gelming | 36,246 |  | Defend homeland | Rest | 0 |  | Knowledgeable, Solitary |
| 94 | Dorgrun Roughhand | Dragonlord | 238,41 |  | Defend homeland | Rest | 0 |  | Reckless, Impatient |
| 95 | Molicor the Kind | Arakai | 190,60 |  | Defend homeland | Rest | 0 |  | Kind, Level-headed |
| 96 | Igral Mouseheart | Arakai | 158,20 |  | Defend homeland | Rest | 0 |  | Generous, Cowardly |
| 97 | Orimund the Resplendent | Uskarg | 162,153 |  | Defend homeland | Rest | 0 |  | Modest, Arrogant |
| 98 | Crun the Weasel | Long Dwarf | 60,129 |  | Defend homeland | Rest | 0 |  | Cowardly, Treacherous |
| 99 | Faramoth the Solemn | High Fey | 19,158 |  | Defend homeland | Rest | 0 |  | Solitary, Thick-skinned |
| 100 | Ulgrim the Weary | Deeping Dwarf | 89,205 | Castle of Tharn | Defend homeland | Rest | 75 |  | Selfless, Cautious, Slow-witted |
| 101 | Miranar Fairhand | Dawn Fey | 116,133 | Citadel of Elorthord | Defend homeland | Rest | 190 | King of the Dawnwood | Modest, Loyal, Patient |
| 102 | Ulgrud the Treacherous | Long Dwarf | 74,142 | Castle of Scaradir | Defend homeland | Rest | 60 |  | Selfish, Cold-hearted, Treacherous |
| 103 | Elgrena the Gracious | Dawn Fey | 112,106 |  | Defend homeland | Rest | 0 |  | Selfless, Kind |
| 104 | Alagrim Ironaxe | Long Dwarf | 127,72 | Castle of Thordroth | Defend homeland | Rest | 85 |  | Brave, Selfless, Apathetic |
| 105 | Karahar the Wistful | Gelming | 91,244 |  | Defend homeland | Rest | 0 |  | Talkative, Patient |
| 106 | Oragrane the Fearless | Dragonlord | 241,63 | Castle of Sarnoth | Defend homeland | Rest | 90 |  | Mighty warrior, Cold-hearted, Impatient |
| 107 | Marathor the Splendid | Atheling | 115,65 | Castle of Melthor | Defend homeland | Rest | 95 |  | Kind, Warm-hearted, Generous |
| 108 | Barag the Fierce | Uskarg | 147,157 |  | Defend homeland | Rest | 0 |  | Hot-tempered, Vicious |
| 109 | Golgud the Reluctant | Deeping Dwarf | 71,174 | Castle of Ararak | Defend homeland | Rest | 60 |  | Generous, Patient, Cowardly |
| 110 | Ilareth the Chosen | High Fey | 18,170 | Castle of Whispering | Defend homeland | Rest | 70 |  | Submissive, Knowledgeable, Arrogant |
| 111 | Torbrith Swiftfoot | Long Dwarf | 73,115 |  | Defend homeland | Rest | 0 |  | Energetic, Reliable |
| 112 | Slorum the Smug | Giant | 186,215 | Castle of Tark | Defend homeland | Rest | 75 |  | Quick-witted, Talkative, Ignorant |
| 113 | Samara Wildheart | Dawn Fey | 91,121 |  | Defend homeland | Rest | 0 |  | Brave, Passionate |
| 114 | Hilgor the Meek | Giant | 186,190 |  | Defend homeland | Rest | 0 |  | Languid, Unconvincing |
| 115 | Zalnor Sourspleen | Kith | 19,50 |  | Defend homeland | Rest | 0 |  | Apathetic, Easily tired |
| 116 | Melgran Dragonsword | Arakai | 174,13 |  | Defend homeland | Rest | 0 |  | Bold, Modest |
| 117 | Khalak the Blue | Uskarg | 163,133 | Castle of Arabar | Defend homeland | Rest | 65 |  | Gallant, Tight-lipped, Restless |
| 118 | Boragrim Sharpaxe | Deeping Dwarf | 55,220 | Castle of Ildemar | Defend homeland | Rest | 85 |  | Generous, Home-loving, Gallant |
| 119 | Melkrith Nightshade | High Fey | 7,143 | Castle of Arelmar | Defend homeland | Rest | 60 |  | Treacherous, Provocative, Domineering |
| 120 | Oscruth Loosehead | Uskarg | 149,187 |  | Defend homeland | Rest | 0 |  | Cowardly, Mad |
| 121 | Volgor the Sure | Long Dwarf | 147,31 |  | Defend homeland | Rest | 0 |  | Brave, Kind |
| 122 | Talmar the Quiet | Kith | 55,12 | Castle of Sareon | Defend homeland | Rest | 80 |  | Quick-witted, Peaceable, Patient |
| 123 | Olog the Friendly | Giant | 199,187 | Castle of Finrod | Defend homeland | Rest | 70 |  | Warm-hearted, Gentle, Eager |
| 124 | Olagrum the Steady | Giant | 208,199 | Castle of Corth | Defend homeland | Rest | 60 |  | Patient, Level-headed, Passionless |
| 125 | Harumbar the Unhappy | Dawn Fey | 170,100 |  | Defend homeland | Rest | 0 |  | Polite, Solitary |
| 126 | Thorgran Wildsword | Uskarg | 187,147 | Castle of Cerrelm | Defend homeland | Rest | 80 |  | Tireless, Passionate, Gallant, Mighty warrior |
| 127 | Aluthrim the Bold | Deeping Dwarf | 98,188 | Castle of Dawn | Defend homeland | Rest | 90 |  | Bold, Kind, Headstrong |
| 128 | Alargrith Warhelm | High Fey | 61,159 | Castle of Aradel | Defend homeland | Rest | 80 |  | Loyal, Cold-hearted, Tight-lipped |
| 129 | Gorolan the Benign | Atheling | 94,71 |  | Defend homeland | Rest | 0 |  | Patient, Sceptical |
| 130 | Ilvar the Penitent | Atheling | 77,93 | Castle of Ceremelth | Defend homeland | Rest | 70 |  | Reliable, Persuasive, Knowledgeable |
| 131 | Araleth the White | Free (Midnight) | 8,12 |  | Defend homeland | Rest | 105 |  | Passionate, Gallant, Charming |
| 132 | Arithel the Joybringer | High Fey | 18,129 | Castle of Elinvar | Defend homeland | Rest | 70 |  | Charming, Loyal, Polite |
| 133 | Emedrel of the Fire | High Fey | 145,220 | Castle of Merikith | Be a hostage | Rest | 0 | Princess of the Weirdwood | Passionate, Patient, Knowledgeable, Headstrong |
| 134 | Andremar the Starborn | High Fey | 13,181 | Castle of Forever | Defend homeland | Rest | 80 |  | Brave, Modest, Loyal |
| 135 | Galdreth the Fair | High Fey | 48,176 | Castle of Golden | Defend homeland | Rest | 75 |  | Far-sighted, Selfless, Level-headed |
| 136 | Sagrana Goldenwing | Dragonlord | 145,220 | Castle of Faragor | Be a hostage | Rest | 0 |  | Passionate, Persuasive, Knowledgeable |
| 137 | Elina the Enchantress | Uskarg | 164,124 | Castle of Alathor | Defend homeland | Rest | 80 |  | Passionate, Persuasive, Knowledgeable |
| 138 | Tarella the Intrepid | Dragonlord | 219,91 |  | Defend homeland | Rest | 0 |  | Talkative, Hot-tempered |
| 139 | Oraina the Placid | Arakai | 128,96 |  | Defend homeland | Rest | 0 |  | Gentle, Submissive |
| 140 | Karelda the Carefree | Uskarg | 179,172 |  | Defend homeland | Rest | 0 |  | Kind, Gentle |
| 141 | Imirel Starblade | Deeping Dwarf | 88,162 |  | Defend homeland | Rest | 0 |  | Gallant, Charming |
| 142 | Ylanda the Wishful | High Fey | 41,153 |  | Defend homeland | Rest | 0 |  | Selfish, Apathetic |
| 143 | Morgrissa Hammertongue | Giant | 205,227 | Castle of Thordrin | Defend homeland | Rest | 59 |  | Hot-tempered, Cruel, Treacherous |
| 144 | Trantana the Grand | Atheling | 129,37 |  | Defend homeland | Rest | 0 |  | Modest, Sceptical |
| 145 | Oglissa the Rose | Giant | 195,214 | Castle of Othrym | Defend homeland | Rest | 55 |  | Selfless, Greedy, Impatient |
| 146 | Melinissa the Sweet | Giant | 145,220 | Castle of Athruk | Be a hostage | Rest | 0 |  | Generous, Energetic, Repulsive |
| 147 | Olthruda the Bountiful | Long Dwarf | 145,220 | Castle of Farwain | Be a hostage | Rest | 0 |  | Talkative, Home-loving, Eager |
| 148 | Thalgrima the Betrothed | Deeping Dwarf | 145,220 | Castle of Zaragorn | Be a hostage | Rest | 0 |  | Talkative, Loyal, Hot-tempered |
| 149 | Elessa of the Mists | Gelming | 7,238 | Castle of Emergelm | Defend homeland | Rest | 70 |  | Selfless, Generous, Passionate, Cautious |
| 150 | Ulene Quietheart | Dragonlord | 222,45 |  | Defend homeland | Rest | 0 |  | Warm-hearted, Gentle |
| 151 | Sherinar of Shadows | Eldrin | 59,100 | Castle of Meranor | Defend homeland | Rest | 90 |  | Passionate, Polite, Headstrong |
| 152 | Mirithel the Warm | Arakai | 171,87 |  | Defend homeland | Rest | 0 |  | Warm-hearted, Gentle |
| 153 | Kiranda the Wild | Arakai | 145,220 | Castle of Erefar | Be a hostage | Rest | 0 |  | Bold, Eager, Passionate, Reckless, Restless, Headstrong, Impatient |
| 154 | Galahar the Calm | Golden Fey | 47,71 | Citadel of Immiel | Defend homeland | Rest | 195 |  | Selfless, Kind, Level-headed |

## Appendix G. Peoples and their quarrels

| People | Will not share a garrison with |
| --- | --- |
| Kith | Uskarg |
| Atheling | Arakai |
| Eldrin | High Fey |
| Long Dwarf | Deeping Dwarf |
| Arakai | Atheling |
| Dragonlord | Giant |
| High Fey | Eldrin |
| Dawn Fey | Gelming |
| Uskarg | Kith |
| Gelming | Dawn Fey |
| Deeping Dwarf | Long Dwarf |
| Giant | Dragonlord |
| Dark Fey | (none) |
| Golden Fey | (none) |
