//
//  help_citadel.cpp
//  citadel
//
//  The Lords of Midnight III : The Citadel
//  World text follows 'The Citadel - Design.pdf'.
//

#include "helpmanager.h"

#if defined(_CITADEL_)
const helpitem_t help_messages[] = {

    { HELP_NONE, hf_none, nullptr }
    ,
    {
        HELP_TN_PLAINS,
        hf_none,
        "PLAINS\n\n"
        "\tThe flat expanses of the Bloodmarch. It is only on open ground that you will see the banners and ranks of the armies that march across the land."
    }
    ,
    {
        HELP_TN_CITADEL,
        hf_none,
        "CITADEL\n\n"
        "\tThe seat of a King of the Bloodmarch, and the strongest fortress of his realm. Storming a citadel will be a hard task."
    }
    ,
    {
        HELP_TN_FOREST,
        hf_none,
        "FOREST\n\n"
        "\tMovement through a forest will not be swift. The Fey, however, know every path beneath the branches, for the deep woods are their home."
    }
    ,
    {
        HELP_TN_HENGE,
        hf_none,
        "HENGE\n\n"
        "\tBuilt in the dawn of the world, these ancient temples have strange powers, not always benign."
    }
    ,
    {
        HELP_TN_TOWER,
        hf_none,
        "TOWER\n\n"
        "\tA watchtower set to guard the marches of a realm. Help may be sought here. It may not always be granted."
    }
    ,
    {
        HELP_TN_VILLAGE,
        hf_none,
        "VILLAGE\n\n"
        "\tThe homes of the common folk of the Bloodmarch. Warriors may be recruited here, and shelter found."
    }
    ,
    {
        HELP_TN_DOWNS,
        hf_none,
        "DOWNS\n\n"
        "\tRolling open country. The going is easy and the view is long."
    }
    ,
    {
        HELP_TN_KEEP,
        hf_none,
        "KEEP\n\n"
        "\tA fortified castle holding the warriors of a land. Take it and its garrison passes to you; lose it and the land is opened to Boroth."
    }
    ,
    {
        HELP_TN_SNOWHALL,
        hf_none,
        "HALL\n\n"
        "\tA great hall of one of the peoples of the Bloodmarch."
    }
    ,
    {
        HELP_TN_LAKE,
        hf_none,
        "LAKE\n\n"
        "\tStill water. Armies must go around it."
    }
    ,
    {
        HELP_TN_FROZENWASTE,
        hf_none,
        "FROZEN WASTE\n\n"
        "\tThe bitter north, beyond the Mists of Oblivion. Little lives here."
    }
    ,
    {
        HELP_TN_RUIN,
        hf_none,
        "RUIN\n\n"
        "\tWhat remains of an older age of the Bloodmarch. Things are sometimes found in ruins."
    }
    ,
    {
        HELP_TN_LITH,
        hf_none,
        "LITH\n\n"
        "\tA standing stone raised by hands long forgotten."
    }
    ,
    {
        HELP_TN_CAVERN,
        hf_none,
        "CAVERN\n\n"
        "\tA dark opening in the rock. Not all of them are empty."
    }
    ,
    {
        HELP_TN_MOUNTAIN,
        hf_none,
        "MOUNTAINS\n\n"
        "\tHigh and hard going. The dwarves and the giants know the mountain ways far better than other folk."
    }
    ,
    { HELP_TN_PLAINS2, hf_none, nullptr }
    ,
    { HELP_TN_MOUNTAIN2, hf_none, nullptr }
    ,
    { HELP_TN_FOREST2, hf_none, nullptr }
    ,
    { HELP_TN_HILLS, hf_none, nullptr }
    ,
    { HELP_TN_GATE, hf_none, nullptr }
    ,
    { HELP_TN_TEMPLE, hf_none, nullptr }
    ,
    { HELP_TN_PIT, hf_none, nullptr }
    ,
    { HELP_TN_PALACE, hf_none, nullptr }
    ,
    { HELP_TN_FORTRESS, hf_none, nullptr }
    ,
    { HELP_TN_HALL, hf_none, nullptr }
    ,
    { HELP_TN_HUT, hf_none, nullptr }
    ,
    { HELP_TN_WATCHTOWER, hf_none, nullptr }
    ,
    { HELP_TN_CITY, hf_none, nullptr }
    ,
    { HELP_TN_FOUNTAIN, hf_none, nullptr }
    ,
    { HELP_TN_STONES, hf_none, nullptr }
    ,
    { HELP_TN_ICYWASTES, hf_none, nullptr }
    ,
    { HELP_TN_UNUSED32, hf_none, nullptr }
    ,
    {
        HELP_TN_LAND,
        hf_none,
        "LAND\n\n"
        "\tOpen country of the Bloodmarch, neither tilled nor wild."
    }
    ,
    {
        HELP_TN_ISLE,
        hf_none,
        "ISLE\n\n"
        "\tLand ringed by water. The Isle of Immiel and the Isle of Arungor are the greatest of them."
    }
    ,
    {
        HELP_TN_LAKELAND,
        hf_none,
        "LAKELAND\n\n"
        "\tLow, wet ground about the shores of a great lake."
    }
    ,
    {
        HELP_TN_PLAIN,
        hf_none,
        "PLAIN\n\n"
        "\tOpen ground where armies may be seen from far off."
    }
    ,
    {
        HELP_TN_PLAINS3,
        hf_none,
        "PLAINS\n\n"
        "\tThe broad plains of the Bloodmarch, easily crossed and easily watched."
    }
    ,
    {
        HELP_TN_FOREST3,
        hf_none,
        "FOREST\n\n"
        "\tDeep woodland. The High Fey, the Dawn Fey and the Golden Fey hold the forests of the Bloodmarch."
    }
    ,
    { HELP_TN_UNUSED39, hf_none, nullptr }
    ,
    {
        HELP_TN_TREES,
        hf_none,
        "TREES\n\n"
        "\tScattered woodland. Slower going than open ground, but not by much."
    }
    ,
    {
        HELP_TN_MOUNTAIN3,
        hf_none,
        "MOUNTAINS\n\n"
        "\tThe Long Mountains and the peaks of the Delve and the Deeping. Hard going for any but dwarves and giants."
    }
    ,
    {
        HELP_TN_ICY_MOUNTAIN,
        hf_none,
        "ICY MOUNTAINS\n\n"
        "\tHigh peaks under snow the year round. The slowest going in all the Bloodmarch."
    }
    ,
    {
        HELP_TN_DOWNS3,
        hf_none,
        "DOWNS\n\n"
        "\tRolling grassland. Easy going, and little cover."
    }
    ,
    {
        HELP_TN_HILLS3,
        hf_none,
        "HILLS\n\n"
        "\tBroken upland country."
    }
    ,
    {
        HELP_TN_FOOTHILLS,
        hf_none,
        "FOOTHILLS\n\n"
        "\tThe lower slopes below the mountains."
    }
    ,
    {
        HELP_TN_VALLEY,
        hf_none,
        "VALLEY\n\n"
        "\tA way through the high country, and often the only one."
    }
    ,
    {
        HELP_TN_BAY,
        hf_none,
        "BAY\n\n"
        "\tAn arm of the Great Ocean. No army crosses it."
    }
    ,
    {
        HELP_TN_SEA,
        hf_none,
        "SEA\n\n"
        "\tThe Great Ocean, which bounds the Bloodmarch to the east. No army crosses it."
    }
    ,
    {
        HELP_TN_RIVER,
        hf_none,
        "RIVER\n\n"
        "\tThe Imilvir, the Falthrang, the Arelon and their kin. Rivers may be crossed, but slowly."
    }
    ,
    {
        HELP_TN_MARSH,
        hf_none,
        "MARSH\n\n"
        "\tSodden ground that drags at the feet of an army."
    }
    ,
    {
        HELP_TN_LAKE3,
        hf_none,
        "LAKE\n\n"
        "\tDeep still water. Glimormir is the greatest of the lakes."
    }
    ,
    { HELP_TN_TUNNEL, hf_none, nullptr }
    ,
    { HELP_SMALL_TUNNEL, hf_none, nullptr }
    ,
    {
        HELP_SELECTING_CHARACTER,
        hf_important,
        "SELECTING A CHARACTER\n\n"
        "\tYou begin with five lords of the House of Moon and its friends: MORKIN Prince, ARALETH the White, RORTHRON the Wise, CORLETH of Corelay and ARIN Lord Blood. You may SELECT a lord at any time by using the SELECT icon or the CHOOSE icon."
    }
    ,
    {
        HELP_LOOKING_AROUND,
        hf_important,
        "LOOKING AROUND\n\n"
        "\tYou can look around by pressing either side of the landscape view or by dragging the landscape from left to right or from right to left."
    }
    ,
    {
        HELP_MOVEMENT,
        hf_important,
        "MOVEMENT\n\n"
        "\tYou can move forward in the direction that you are looking by either pressing the top of the screen or by dragging the screen downwards."
    }
    ,
    {
        HELP_THINKING,
        hf_important,
        "THINKING?\n\n"
        "\tThinking gives you more information about what your lord knows about their situation. Pressing on the shield will take you to the THINK screen at any stage."
    }
    ,
    {
        HELP_OVERVIEW_MAP,
        hf_important,
        "THE MAP OF THE BLOODMARCH\n\n"
        "\tThe map depicts the major features of the Bloodmarch, but like any map it does not show every detail. It will serve as a guide to your journeys and a good helper when you become lost. But do not forget that the landscape has its own secrets."
    }
    ,
    {
        HELP_DISCOVERY_MAP,
        hf_important,
        "THE MAP OF THE BLOODMARCH\n\n"
        "\tThe discovery map will slowly build up as you travel. If you can clearly see something with your eyes, it will be added to your map."
    }
    ,
    {
        HELP_CHOOSE,
        hf_none,
        "CHOOSE\n\n"
        "\tThe CHOOSE menu presents a list of special options not covered by looking around and moving."
        "\n\n\tWhat is open to you depends on the situation the lord finds himself in, and on his own nature. The choices open to a cowardly lord will seldom include brave deeds."
    }
    ,
    {
        HELP_NASTIES,
        hf_none,
        "THE VIEW AHEAD\n\n"
        "\tAs you look around, large figures may appear in the foreground. These are the warriors, lords or creatures that lie immediately ahead of you. You do not always see all that lies ahead."
        "\n\n\tThe wise traveller must be both bold and wary."
    }
    ,
    {
        HELP_ARMIES,
        hf_none,
        "ARMIES\n\n"
        "\tA friendly army offers no hindrance to the traveller, but to go through the midst of an enemy army is the gravest peril, by day or by night. Armies in mountains and forests will hide themselves well."
    }
    ,
    {
        HELP_FEY,
        hf_none,
        "THE FEY\n\n"
        "\tThree peoples of the Fey dwell in the Bloodmarch. The High Fey hold Weirdwood and the Dawn Fey hold Dawnwood. The Golden Fey keep to the Isle of Immiel in Glimormir, where their magic is so strong against evil that Boroth dares not assail them - and they alone of all the peoples have no hostage in the Dark Citadel."
        "\n\n\tThe Dark Fey are the fourth. Touched by evil long ago, they are the enemy."
    }
    ,
    {
        HELP_WISE,
        hf_none,
        "THE PEOPLES OF THE BLOODMARCH\n\n"
        "\tFifteen peoples share the Bloodmarch: the Kith, the Athelings, the Eldrin, the Long Dwarves, the Arakai, the Dragonlords, the High Fey, the Dawn Fey, the Uskarg, the Gelmings, the Deeping Dwarves, the Giants, the Golden Fey, the Corsairs and the Free."
        "\n\n\tEach realm has one of its own held hostage in Maranor, and while that hostage lives in Boroth's dungeons its King will not march."
    }
    ,
    {
        HELP_FREE,
        hf_none,
        "THE HOSTAGES\n\n"
        "\tBoroth keeps a prisoner from each realm of the Bloodmarch in the dungeons of the Dark Citadel, to ensure their reluctant compliance."
        "\n\n\tFree a realm's hostage and its lords may be persuaded to join you. This is the surest way to raise an army large enough to march on Maranor."
    }
    ,
    {
        HELP_DAY1,
        hf_important,
        "THE BLOODMARCH\n\n"
        "\tBoroth the Wolfheart, High King of the Dark Fey, rules the Marish from the Dark Citadel of Maranor, and will not rest until the whole Bloodmarch is enslaved. He holds a hostage from every realm, and so the Kings of the Bloodmarch do not move against him."
        "\n\n\tCorleth and Arin have only lately been set free from his dungeons, and Luxor the Moonprince is still held there. Morkin, Araleth and Rorthron are abroad in the north."
        "\n\n\tBoroth will take citadels and castles as he marches toward Midnight. You must take them back to slow him."
    }
    ,
    {
        HELP_GAME_WORKS,
        hf_important,
        "HOW THE GAME WORKS\n\n"
        "\tYou control the lords of the House of Moon and those who join them. The game proceeds by day and by night: you move your lords, then press NIGHT, and the lords of the Bloodmarch and the armies of the Dark Fey take their turn."
    }
    ,
    {
        HELP_MOVING,
        hf_important,
        "MOVING\n\n"
        "\tDuring the day you can move any or all of the lords you control and any armies with them. How far a lord can travel depends on the terrain, on whether he walks or rides, and on his state of health."
        "\n\n\tRemember that moving north, south, east or west covers one league, while moving diagonally covers about 1.4 leagues, and so costs more of the day."
    }
    ,
    {
        HELP_PRESS_NIGHT,
        hf_important,
        "NIGHT\n\n"
        "\tWhen a lord has exhausted his hours of daylight, night falls for him and he can do no more until the following day. You can still move other lords."
        "\n\n\tOnce you have moved all the lords you wish to, press the NIGHT icon."
        "\n\n\tDO NOT FORGET TO PRESS THE NIGHT ICON WHEN YOU HAVE FINISHED YOUR DAY. IF YOU DO NOT PRESS IT NOTHING FURTHER WILL HAPPEN AT ALL."
    }
    ,
    {
        HELP_CHOICE_OF_GAMES,
        hf_none,
        "A CHOICE OF GAMES\n\n"
        "\tThere is one road to victory, and two halves to it. Boroth must be killed, and the Dark Citadel of Maranor must be taken - for if you kill him and do not hold the citadel, he rises again there as a wraith."
        "\n\n\tTo do either you need armies, and to raise armies you must win the realms of the Bloodmarch to your cause by freeing the hostages Boroth holds in Maranor."
        "\n\n\tKeep at least one of the House of Moon alive: Luxor, Morkin, Corleth or Anderlane. If all four die, the Bloodmarch is lost."
    }
    ,
    {
        HELP_VICTORY,
        hf_none,
        "VICTORY AND DEFEAT\n\n"
        "\tYou win when Boroth the Wolfheart is dead AND the Dark Citadel of Maranor has fallen to the Free, so long as one of the House of Moon still lives."
        "\n\n\tBoroth wins if Luxor, Morkin, Corleth and Anderlane are all dead."
    }
    ,
    {
        HELP_ICEFEAR,
        hf_none,
        "THE SEVEN WEAPONS\n\n"
        "\tSeven weapons lie scattered across the Bloodmarch. STORMBLADE and SWIFTWING were forged with dragonsfire; BLOODBRINGER was the Sword of the Kings of the Last Northing; WIDOWMAKER was the Great Axe of Dwarfdom; ARANATH is the Golden Sword of the Fey; PERSUADER draws warriors from any stronghold not at war with its bearer's realm; and SKULLCRUSHER was wrought in the Iron Mountains for the King of the Delve."
        "\n\n\tEach is unique, and each will only be found by those who go looking."
    }
    ,
    {
        HELP_BATTLE,
        hf_none,
        "ENGAGING IN BATTLE\n\n"
        "\tMinor skirmishes are quick affairs. Battles between armies are not decided until the day is over."
        "\n\n\tBecause a battle is such a major event you cannot walk an army into an enemy army with a simple MOVE; it is always one of the special actions under CHOOSE."
        "\n\n\tMany things influence the outcome: the number of troops, the terrain and the quality of the commanders. As any warrior must, you will learn by experience."
    }
    ,
    {
        HELP_GROUPED,
        hf_important,
        "GROUPED LORDS\n\n"
        "\tWhen you take control of a lord who is following another you will not be able to move him. His leader's crest is shown in the corner of his shield. To take independent control, ungroup him on the SELECT screen."
    }
    ,
    {
        HELP_TUTORIAL,
        hf_important,
        "HELP\n\n"
        "\tAt key points you will be presented with helpful information, sometimes indicated by a flashing TUTORIAL icon in the top right of the screen."
        "\n\n\tYou can turn the tutorial off from the options screen."
    }
    ,
    {
        HELP_TUTORIAL_OFF,
        hf_important|hf_always,
        "TUTORIAL MODE - TURNED OFF\n\n"
        "\tYou will not receive any tutorial information while playing."
    }
    ,
    {
        HELP_TUTORIAL_ON,
        hf_important|hf_always,
        "TUTORIAL MODE - TURNED ON\n\n"
        "\tAt key points you will be presented with helpful information."
    }

};

#endif
