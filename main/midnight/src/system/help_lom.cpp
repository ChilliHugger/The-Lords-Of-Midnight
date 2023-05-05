//
//  help_lom.cpp
//  midnight
//
//  Created by Chris Wild on 11/12/2017.
//

#include "helpmanager.h"

#if defined(_LOM_)
const helpitem_t help_messages[] = {
    
    { 0, NULL }
    
    // 1 - PLAINS
    ,
    {
        hf_none,
        "PLAINS\n\n"
        "\tThe flat expanses of the Plains of Midnight. It is only on the plains that you will actually see the banners and ranks of the armies that march across the land."
    }
    
    ,
    {
        hf_none,
        "CITADEL\n\n"
        "\tA strongly fortified city which may harbour enemy forces or offer shelter to a friendly army. Storming a citadel will be a hard task."
    }
    
    ,
    {
        hf_none,
        "FOREST\n\n"
        "\tMovement through a forest will not be swift. The minions of Doomdark, however. will find it doubly difficult for forests are the homes of the magical Fey who hold no love for the foul creatures of the Witchking."
    }
    
    ,
    {
        hf_none,
        "HENGE\n\n"
        "\tBuilt in the dawn of the world, these ancient temples have strange powers, not always benign."
    }
    
    ,
    {
        hf_none,
        "TOWER\n\n"
        "\tThe refuges of the Wise, the Towers of Midnight are almost impregnable from attack but help may be sought at one of these. It may not always be granted."
    }
    
    ,
    {
        hf_none,
        "VILLAGE\n\n"
        "\tA village can offer warmth and shelter to the lonely traveller if its people prove friendly."
    }
    
    ,
    {
        hf_none,
        "DOWNS\n\n"
        "\tGently rolling hills, the downs slow a traveller only slightly but they may hide unseen dangers."
    }
    
    ,
    {
        hf_none,
        "KEEP\n\n"
        "\tThe fortress of a minor Lord, a keep will offer protection against occasional raids but will not withstand a determined assault for long."
    }
    
    ,
    {
        hf_none,
        "SNOWHALL\n\n"
        "\tBuilt by the wandering peoples of Midnight during the long winter, snowhalls are quite large structures which can offer shelter to many hundreds if need be."
    }
    
    ,
    {
        hf_none,
        "LAKE\n\n"
        "\tThe remaining lakes of Midnight are fed by warm springs. They have powers to revive and heal those who oppose Doomdark and the forces of cold."
    }
    
    ,
    {
        hf_none,
        "FROZEN WASTE\n\n"
        "\tSurrounding the land of Midnight are the Frozen Wastes. They cannot be entered by any. Free, Foul or Fey."
    }
    
    ,
    {
        hf_none,
        "RUIN\n\n"
        "\tAbandoned fortresses of former wars, ruins may harbour dark and dangerous things but may, in times of need, offer some protection against attack."
    }
    
    ,
    {
        hf_none,
        "LITH\n\n"
        "\tThese ancient standing stones often have magical powers."
    }
    
    ,
    {
        hf_none,
        "CAVERN\n\n"
        "\tA cavern can provide shelter and a hiding place but it may already have done so for fouler creatures!"
    }
    
    ,
    {
        hf_none,
        "MOUNTAIN\n\n"
        "\tMoving across a mountain will take many hours of travel and leave you exhausted at the end of your journey."
    }
    
    ,
    {
        hf_none,
        ""
    }
#if defined(_TUNNELS_)
    ,
    {
        hf_important,
        "TUNNEL\n\n"
        "\tIn an underground tunnel, tall pillars crowned with fire mark the way forward. If there is no way ahead, you will see naught but darkness. Tunnels only lead north, south, east or west."
    }
#endif
    ,
    {
        hf_important,
        "SELECTING A CHARACTER\n\n"
        "\tYou have four characters under your control. These are LUXOR the Moonprince, MORKIN his son, CORLETH the Fey and RORTHRON the Wise. You may SELECT a character at any time by using the SELECT icon or the CHOOSE icon."
    }
    ,
    {
        hf_important,
        "LOOKING AROUND\n\n"
        "\tYou can look around by pressing either side of the landscape view or by dragging the landscape from left to right or from right to left."
    }
    
    ,
    {
        hf_important,
        "MOVEMENT\n\n"
        "\tYou can move forward in the direction that you are looking by either pressing the top of the screen or by dragging the screen downwards."
    }
    
    ,
    {
        hf_important,
        "THINKING?\n\n"
        "\tThinking gives you more information about what your character knows about their situation. Pressing on the shield will take you to the THINK screen at any stage. Alternatively you can select the THINK icon from the CHOOSE menu."
    }
    // HELP_OVERVIEW_MAP
    ,
    {
        hf_important,
        "THE MAP OF MIDNIGHT\n\n"
        "\tThe map of Midnight depicts the major features of the geography of Midnight, but like any map it does not show every single detail. You will find surprises where ever you roam. It will serve well, however, as a guide to your journeys through Midnight and be a good helper when you become lost. But do not forget that the landscape has its own secrets!"
    }
    // HELP_DISCOVERY_MAP
    ,
    {
        hf_important,
        "THE MAP OF MIDNIGHT\n\n"
        "\tThe discovery map will slowly build up as you travel around Midnight. If you can clearly see something with your eyes, then it will be added to your map."
    }
    // HELP_CHOOSE
    ,
    {
        hf_none,
        "CHOOSE\n\n"
        "\tThe CHOOSE menu presents you with a list of special options not covered by the looking around and moving.\n\n\tWhat special options are open will depend upon the situation the character finds himself in but will include such choices as searching, hiding, attacking an enemy, repairing defences and so on.\n\n\tThe CHOOSE options will also reflect the personality of the particular character. All the choices you are presented with are only those the character would be likely to choose by himself. So, the choices open to a cowardly character will seldom include brave deeds, the choices open to a greedy character will seldom include acts of generosity."
    }
    
    // HELP_NASTIES
    ,
    {
        hf_none,
        "THE VIEW AHEAD\n\n"
        "\tAs you look around during your travels, large figures may appear in the foreground of each panorama you see. These are the warriors, characters or creatures that lie immediately ahead of you on the borders of the next domain. You do not always, however, see all that lies ahead.\n\tThe wise travellers must be both bold and wary."
    }
    //  HELP_ARMIES
    ,
    {
        hf_none,
        "ARMIES\n\n"
        "\tA friendly army offers no hindrance to the traveller, but an attempt to go through the midst of an enemy army offers the gravest of peril, by day or by night. Armies in mountains, forests or any of the other places to be found will hide themselves well and not be seen."
    }
    
    // HELP_FEY
    ,
    {
        hf_none,
        "THE FEY\n\n"
        "\tThe Fey are in loose alliance with the Free. They do not seek war but neither do they relish the thought of Doomdark overrunning Midnight. Their part in the War of the Solstice will be mostly passive. Their homes are the forests of Midnight and Doomdark's armies will not willingly be allowed passage through these. Corleth the Fey, however. should be able to rally enough of his people to his banner to form one army."
    }
    
    // HELP_WISE
    ,
    {
        hf_none,
        "THE WISE\n\n"
        "\tThe Wise have isolated themselves from the world and live like hermits in their tall towers. Doomdark will not bother them so long as they remain withdrawn from the affairs of Men and most certainly, they will not aid him. In the right circumstances, it may be possible to seek their help and be granted it. Rorthron the Wise could prove a useful ally in this."
    }
    
    // THE FREE
    ,
    {
        hf_none,
        "THE FREE\n\n"
        "\tOf the Free themselves, there are many Lords. Luxor should first set himself the task of seeking their loyalty, thus gaining control of many armies. Most powerful are the Lord Marshals of the great Citadels but the Moonprince will find other Lords who will also bow to his command. He should not, however, waste too much time seeking out allies; there are others who will make fine ambassadors."
    }
    

    // HELP_DAY1
    ,
    {
        hf_important,
        "THE FREE AND THE FOUL\n\n"
        "\tDoomdark's forces hold the north whilst the Free hold the South. Few of Doomdark's armies will be found south of the Mountains of Ithril and the Plains of Valethor. Of the major citadels, Doomdark holds Ushgarak, Grarg, Vorgath and Kor. The only armies of the Free to be found north of the Mountains of Ithril and the Plains of Valethor are in Ithrorn and the Plains of Ithril.\n\n\tOf the major citadels, the Free still hold Ithrorn, Kumar, Marakith, Shimeril, Gard and Xajorkith. In the east, the barbarian tribes of the Targ remain independent of both Doomdark and the Moonprince. In the west, save for the Citadel of Gard, the lands lie mostly empty and under no one's sway."
    }
    // HELP_GAME_WORKS
    ,
    {
        hf_important,
        "HOW THE GAME WORKS\n\n"
        "\tThe game begins on the Winter Solstice. Initially, you control Luxor the Moonprince, Morkin, Corleth the Fey and Rorthron the Wise. These characters all start the game at the Tower of the Moon in the Forest of Shadows. The game proceeds by day and by night."
        "\n\n\tYou may like to take a little time to read the Novella that tells Luxor and Morkin's story up to the start of the Solstice - press the icon in the bottom left of the main menu."
    }
    // HELP_MOVING
    ,
    {
        hf_important,
        "MOVING\n\n"
        "\tDuring the day, you can move any or all of the characters you control and any armies that are with them. The distance a character can move in one day depends on the difficulty of the terrain and whether he is walking or riding as well as his state of health. You must learn by experience precisely how far you can travel under given circumstances.\n\n\tHowever, there is one important thing to remember: when you travel directly north, south, east or west you are moving just one league at a time: when you move northeast, northwest, southeast or southwest you are moving along the diagonal of a square one league by one league, a distance of approximately 1.4 leagues. Therefore, this will take you longer and leave less hours of daylight for the rest of your journey."
    }
    
    // HELP_PRESS_NIGHT
    ,
    {
        hf_important,
        "NIGHT\n\n"
        "\tWhen a character has exhausted his hours of daylight, night will fall for him, and unless there are exceptional circumstances (the THINK screen will tell you if there are), he will not be able to do any more until the following day. You can still, however, move other characters under your control.\n\n"
        "\tOnce you have moved all the characters you wish to, you must press the NIGHT icon. This lets nightfall everywhere and signals the start of activities for the forces of Doomdark. During the night, Doomdark will move his characters and armies across Midnight and there will be a pause as he 'thinks'. Soon, however, a new day will dawn and you can command your characters afresh.\n\n"
        "\tDO NOT FORGET TO PRESS THE NIGHT ICON WHEN YOU HAVE FINISHED YOUR DAY'S MOVEMENT AND ACTION. IF YOU DO NOT PRESS THE NIGHT ICON NOTHING FURTHER WILL HAPPEN AT ALL!"
    }
    
    // HELP_CHOICE_OF_GAMES
    ,
    {
        hf_none,
        "A CHOICE OF GAMES\n\n"
        "\tThere are two distinct ways of winning a victory over Doomdark. The first is by war, by sending armies north to the Plains of Despair and seizing the Citadel of Ushgarak from whence Doomdark commands his foul hordes. In such a strategy, Luxor himself will play a major part as a commander in the field.\n\n"
        "\tThe second way of winning is by quest, by guiding Morkin, Luxor's son, to the Tower of Doom to destroy the Ice Crown, the source of Doomdark's power. Morkin can have no army to help him on his journey, for the Ice Crown sends forth the icefear which withers men's minds. By virtue of his birth, halfhuman, halffey, only Morkin can resist the utter coldness of the Ice Crown's power.\n\n"
        "\tIf you prefer a pure adventure just concentrate on the quest of Morkin. The armies of Doomdark will still march south to conquer Midnight but the armies of the Free will defend themselves without your guidance, even though they will not make any countermoves.\n\n"
        "\tIf you prefer a pure war game, ignore the quest of Morkin and concentrate on the assault of Ushgarak.\n\n"
        "\tTo play the complete epic, however, you should place equal importance on the war that Luxor directs and the quest that Morkin journeys on. Naturally enough, the complete epic takes the longest to play. Of the other options, you will find the quest the quicker game."
    }
    
    // HELP_VICTORY
    ,
    {
        hf_none,
        "VICTORY FOR DOOMDARK\n\n"
        "\tTo win, Doomdark must achieve two objectives. First, he must eliminate Morkin: as long as Morkin is alive, the game will continue. Second, he must subdue the armies of the Free. This can be done in two ways, either by eliminating Luxor the Moonprince who is their commander or by taking the Citadel of Xajorkith in the land of Corelay, the home of all their hopes.\n\n"
        "\tIf Luxor is killed, you, the player, lose all control over the other characters in the game except for Morkin, his son. If, by any chance, Morkin manages to find the Moon Ring that Luxor wore and which was the source of Luxor's Powers of Vision and Command, he can put it on and you will regain control over those characters still loyal to the Free. However, once he does this, Morkin will immediately become known to Doomdark and his quest to seize the Ice Crown will become almost impossible.\n\n"
        "\tIf Xajorkith is taken by Doomdark but Morkin is still alive. Luxor can continue the armed struggle against the Witchking. For Doomdark to win, Xajorkith must be Doomdark's and Morkin must be dead, OR both Luxor and Morkin must be dead."
    }
    
    // HELP_ICEFEAR
    ,
    {
        hf_none,
        "THE ICEFEAR AND THE MOON RING\n\n"
        "The icefear is Doomdark's greatest weapon, sapping men's courage and reducing armies to rabble. If strong enough, it may even cause characters once loyal to Luxor and the Free to desert to Doomdark's control. He can use it either as a general effect, spread equally over the lands of Midnight or concentrate it in particular places.\n\n"
        "\tThe only shield against it is the Moon Ring that Luxor wears: this radiates the strength and warmth of his mind. The closer a character or Army is to Luxor, the less will be the demoralising effect of the icefear. The same applies if Morkin is the wearer of the Moon Ring. There is one drawback: the Witchking can sense the warmth of the Moon Ring and so, at any stage of the game, he knows the precise whereabouts of its wearer.\n\n"
        "\tThe strength of the icefear also depends on Doomdark's confidence. As the Witchking takes citadel after citadel of the Free so does the icefear grow. but where he suffers defeat or doubt the icefear dwindles. The Ice Crown has a cold intelligence of its own and as Morkin comes closer towards it. it will feel the approaching danger and bend a greater and greater part of its force towards its own protection. So, as Morkin approaches the Ice Crown, more and more of the icefear will be directed at him alone but it will not affect him. Instead, the burden of its terror will begin to lift from the armies and commanders of the Free."
    }
    
    // HELP_BATTLE
    ,
    {
        hf_none,
        "ENGAGING IN BATTLE\n\n"
        "\tMinor skirmishes involving individual characters and small warbands are quick affairs and can take place at any stage throughout the day. Battles between armies, however, that will not be decided until the day is over.\n\n"
        "\tBecause a battle between armies is such a major event, you will not be able to move an army to the same location as an enemy army by using the simple MOVE option. Such a move is always one of the special actions you can opt for during the CHOOSE option. Some of the commanders you control may be so afraid that the choice to move them into battle does not even appear as one of the possibilities.\n"
        "\tDuring the course of the day, you can move as many armies into battle as you wish. If you move more than one army into the same battle, the program will keep track of their times of arrival (which may influence the outcome of the contest). However, once an army or a character has been moved into a battle, it will not be able to move again until the following day.\n\n"
        "\tAt dawn on the following day, the outcome will be known to your commanders. If the enemy has lost, his armies will have been destroyed in the night or have fled, leaving your armies and characters free to move. If the enemy has not lost, you have the choice of retreating with what is left of your armies or continuing the struggle for another day, possibly throwing in more forces. If, however, the enemy has won a decisive victory, when dawn breaks you will find your armies destroyed and your surviving commanders scattered: the enemy forces may have advanced far beyond the battlefield.\n\n"
        "\tMany things will influence the outcome of a battle: the number of troops, the type of terrain. the quality of the commanders and, of course, the strength of the icefear. But, as any warrior must, you must learn by experience."
    }
    
    // HELP_GROUPED
    ,
    {
        hf_important,
        "GROUPED LORDS\n\n"
        "\tWhen you take control of a lord who is following another, you will not be able to move them. Their leader's crest will be shown in the top corner of their own shield. This will allow you to quickly choose the leading lord. To take independent control of the lord for movement, then you must ungroup them on the SELECT screen."
    }
    
    
    // HELP_TUTORIAL
    ,
    {
        hf_important,
        "HELP\n\n"
        "\tAt key points during the game, you will be presented with helpful information. Sometimes this information is indicated by a flashing TUTORIAL icon in the top right of the screen. If you want to read this helpful information then press the icon.\n\n"
        "\tYou can turn this tutorial off from the options screen. You may also like to read the full online guide by pressing the icon in bottom right of the main menu."
        
    }
    
    
    // HELP_TUTORIAL_OFF
    ,
    {
        hf_important|hf_always,
        "Tutorial mode - TURNED OFF.\n\n"
        "\tYou will not recieve any tutorial information while playing the game.\n\nYou can however read the online guide by pressing the icon in the bottom right corner of the main  menu."
        
    }
    
    // HELP_TUTORIAL_ON
    ,
    {
        hf_important|hf_always,
        "Tutorial mode - TURNED ON.\n\n"
        "\tAt key points during the game, you will be presented with helpful information. Sometimes this information is indicated by a flashing TUTORIAL icon in the top right of the screen. If you want to read this helpful information then press the icon.\n\nAlternatively you can read the online guide by pressing the icon in the bottom right corner of the main menu."
        
    }
    
};

#endif

