//
//  help_ddr.cpp
//  midnight
//
//  Created by Chris Wild on 11/12/2017.
//

#include "helpmanager.h"

#if defined(_DDR_)
const helpitem_t help_messages[] = {
    
    { HELP_NONE, hf_none, nullptr }
    
    //
    // ddr
    ,
    {
        HELP_TN_PLAINS2,
        hf_none,
        "PLAINS\n\n"
        "\tThe flat expanses of the Plains of the Icemark. It is only on the plains that you will actually see the banners and ranks of the armies that march across the land."
    }
    
    ,
    {
        HELP_TN_MOUNTAIN2,
        hf_none,
        "MOUNTAIN\n\n"
        "\tMoving across a mountain range will take many hours of travel and leave you exhausted at the end of your Journey."
    }
    ,
    {
        HELP_TN_FOREST2,
        hf_none,
        "FOREST\n\n"
        "\tMovement through a forest will not be swift, save for the Fey whose homes lie there."
    }
    ,
    {
        HELP_TN_HILLS,
        hf_none,
        "HILLS\n\n"
        "\tHills slow a traveller, but not severely. Unseen dangers may lie beyond."
    }
    ,
    {
        HELP_TN_GATE,
        hf_none,
        "GATE\n\n"
        "\tSet at the entrances of subterranean roads, the Gates of the Icemark stand at the threshold of the dark world below."
    }
    ,
    {
        HELP_TN_TEMPLE,
        hf_none,
        "TEMPLE\n\n"
        "\tA temple is apt to harbour dark and mysterious forces. Some give access to benighted ways beneath the earth."
    }
    ,
    {
        HELP_TN_PIT,
        hf_none,
        "PIT\n\n"
        "\tThrough the dark mouth of the abyss. you may find access to subterranean passages, but foul creatures may issue forth from the bowels of the earth."
    }
    ,
    {
        HELP_TN_PALACE,
        hf_none,
        "PALACE\n\n"
        "\tThough not designed to withstand armed assault, a palace is still a centre of power."
    }
    ,
    {
        HELP_TN_FORTRESS,
        hf_none,
        "FORTRESS\n\n"
        "\tThe stronghold of minor Lords of the Icemark. a fortress will offer some protection."
    }
    ,
    {
        HELP_TN_HALL,
        hf_none,
        "HALL\n\n"
        "\tA war-chief's hall can offer warmth and shelter to the lonely traveller, if its people prove friendly."
    }
    ,
    {
        HELP_TN_HUT,
        hf_none,
        "HUT\n\n"
        "\tBrief respite can be found here from the cold mists and winds of the Icemark."
    }
    ,
    {
        HELP_TN_WATCHTOWER,
        hf_none,
        "WATCHTOWER\n\n"
        "\tThe Watchtowers of the Icemark may hold dark secrets; approach with care!"
    }
    ,
    {
        HELP_TN_CITY,
        hf_none,
        "CITY\n\n"
        "\tA strongly fortified township which may harbour enemy forces or offer shelter to a friendly army. Storming a city will be a hard task."
    }
    ,
    {
        HELP_TN_FOUNTAIN,
        hf_none,
        "FOUNTAIN\n\n"
        "\tLittle in the bleak landscape of the Icemark offers succour to the traveller but from a sparkling fountain, he may drink his fill and be refreshed."
    }
    ,
    {
        HELP_TN_STONES,
        hf_none,
        "STONES\n\n"
        "\tStones mark the route of ancient, forgotten roads beneath the icy carpet that covers the land. Strange powers oft gather about them."
    }
    ,
    {
        HELP_TN_ICYWASTES,
        hf_none,
        "ICY WASTES\n\n"
        "\tThe barrier ice of the Northern glaciers allows no traveller to pass through. The Icy Wastes are impenetrable to all."
    }
    ,
    {
        HELP_TN_MISTS,
        hf_important,
        "MISTS\n\n"
        "\tCold mists block the landscape from view as they roll across the Icemark. All save Shareth's Iceguard find their courage and strength sapped if caught in their grip."
    }
    
    ,
    {
        HELP_TN_TUNNEL,
        hf_important,
        "TUNNEL\n\n"
        "\tIn an underground tunnel, tall pillars crowned with fire mark the way forward. If there is no way ahead, you will see naught but darkness. Tunnels only lead north, south, east or west."
    }
    
    ,
    {
        HELP_SELECTING_CHARACTER,
        hf_important,
        "SELECTING A CHARACTER\n\n"
        "\tYou start with three characters under your control. These are LUXOR the Moonprince, TARITHEL the Fey, and RORTHRON the Wise. You may SELECT a character at any time by using the SELECT icon or the CHOOSE icon."
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
        "\tThinking gives you more information about what your character knows about their situation. Pressing on the shield will take you to the THINK screen at any stage. Alternatively you can select the THINK icon from the CHOOSE menu."
    }
    ,
    {
        HELP_OVERVIEW_MAP,
        hf_important,
        "THE KINGDOMS OF ICEMARK\n\n"
        "\tThe land of Icemark is ruled by five major races and the map that Luxor carries with him, though lamentably lacking in detail, shows their Kingdoms. To the North-east, the Men of the Frozen Empire are governed by Shareth the Heartstealer. To the North-west lies the Kingdom of the Giants, to the East the Kingdom of the Dwarves whilst to the South are the Outlands of the Barbarians and the Realm of the Fey."
    }
    ,
    {
        HELP_DISCOVERY_MAP,
        hf_important,
        "THE MAP OF THE ICEMARK\n\n"
        "\tThe discovery map will slowly build up as you travel around the Icemark. If you can clearly see something with your eyes, then it will be added to your map."
    }
    ,
    {
        HELP_CHOOSE,
        hf_none,
        "CHOOSE\n\n"
        "\tThe CHOOSE menu presents you with a list of special options not covered by the looking around and moving.\n\n\tWhat special options are open will depend upon the situation the character finds himself in but will include such choices as searching, hiding, attacking an enemy, repairing defences and so on.\n\n\tThe CHOOSE options will also reflect the personality of the particular character. All the choices you are presented with are only those the character would be likely to choose by himself. So, the choices open to a cowardly character will seldom include brave deeds, the choices open to a greedy character will seldom include acts of generosity."
    }
    
    ,
    {
        HELP_NASTIES,
        hf_none,
        "THE VIEW AHEAD\n\n"
        "\tAs you look around during your travels, large figures may appear in the foreground of each panorama you see. These are the warriors, characters or creatures that lie immediately ahead of you on the borders of the next domain. You do not always, however, see all that lies ahead.\n\tThe wise travellers must be both bold and wary."
    }
    
    ,
    {
        HELP_ARMIES,
        hf_none,
        "ARMIES\n\n"
        "\tA friendly army offers no hindrance to the traveller but an attempt to go through the midst of an enemy army offers the gravest of peril."
    }
    
    ,
    {
        HELP_DAY1,
        hf_important,
        "THE RACES OF THE ICEMARK\n\n"
        "\tThe land of Icemark is ruled by five major races.\n\n"
        "\tTo the North-east, the Men of the Frozen Empire are governed by Shareth the Heartstealer. To the North-west lies the Kingdom of the Giants, to the East the Kingdom of the Dwarves whilst to the South are the Outlands of the Barbarians and the Realm of the Fey.\n\n"
        "\tAll these are intermittently at war with each other, involved in petty feuds and border raids but of them all, the Frozen Empire is the strongest and the most feared. To defeat Shareth, Luxor must find allies in these foreign domains. The Lords of the Fey should be counted most friendly, the Lords of the Iceguard least so, but a cunning Moonprince will exploit their differences to his own purpose. There is only one in the whole of the Icemark who will never rally to his banner - Shareth Heartstealer, his mortal enemy."
    }
    ,
    {
        HELP_GAME_WORKS,
        hf_important,
        "HOW THE GAME WORKS\n\n"
        "\tThe game begins on the day that the Moonprince rides forth from the Gate of Varenorn into the Icemark and proceeds by day and by night. Initially, you control Luxor the Moonprince, Tarithel the Fey, and Rorthron the Wise.\n\n"
        "\tBy virtue of the Moonring, which lends you the Power of Vision and the Power of Command, you can control those characters in the land of Icemark who are friendly to your cause, looking through their eyes at the surrounding landscape and guiding them in their many tasks. Some will be just individuals, some will be commanders at the head of whole armies; when you move a commander, his army moves with him.\n\n"
        "\tIndependent characters may be persuaded to join your cause, but equally may be persuaded by Shareth to fall in with her.\n\n"
        "\tYou may like to take a little time to read the Novella that tells the story up to the start of the journey into The Icemark - press the icon in the bottom left of the main menu."
    }
    ,
    {
        HELP_MOVING,
        hf_important,
        "MOVING\n\n"
        "\tDuring the day, you can move any or all of the characters you control and any armies that are with them. The distance a character can move in one day depends on the difficulty of the terrain and whether he is walking or riding as well as his state of health. You must learn by experience precisely how far you can travel under given circumstances.\n\n\tHowever, there is one important thing to remember: when you travel directly north, south, east or west you are moving just one league at a time: when you move northeast, northwest, southeast or southwest you are moving along the diagonal of a square one league by one league, a distance of approximately 1.4 leagues. Therefore, this will take you longer and leave less hours of daylight for the rest of your journey."
    }
    
    ,
    {
        HELP_PRESS_NIGHT,
        hf_important,
        "NIGHT\n\n"
        "\tWhen a character has exhausted his hours of daylight, night will fall for him, and unless there are exceptional circumstances (the THINK screen will tell you if there are), he will not be able to do any more until the following day. You can still, however, move other characters under your control.\n\n"
        "\tOnce you have moved all the characters you wish to, you must press the NIGHT icon. This lets night fall everywhere and signals the beginning of movement for the forces of the Heartstealer. Independent characters will also move at this time. Soon, however, the night will be over and the message, 'Dawn breaks' will appear on the screen.\n\n"
        "\tDO NOT FORGET TO PRESS THE NIGHT ICON WHEN YOU HAVE FINISHED YOUR DAY'S MOVEMENT AND ACTION. IF YOU DO NOT PRESS THE NIGHT ICON NOTHING FURTHER WILL HAPPEN AT ALL!"
    }
    
    ,
    {
        HELP_CHOICE_OF_GAMES,
        hf_none,
        "VICTORY FOR YOU, THE MOONPRINCE\n\n"
        "\tMany choices face Luxor, the Moonprince. Should he simply rescue Morkin and retreat to the Gate of Varenorn and thence to Midnight? Should he seek to limit Shareth's power before returning through the Frozen Gates? Or should he seek ultimate victory and, risking all attempt to destroy her forever?\n\n"
        "\tAccordingly, Doomdark's Revenge allows for different kinds of victory, some lesser, some greater, but before any of these victories can be claimed. Luxor must return safely to the Gate of Varenorn. The most basic victory that can be achieved is the rescue of Morkin, Luxor's son. He must be brought, alive and well, to the Gate of Varenorn. In this task Tarithel, Fey daughter of the Lord of Dreams, will be the main protagonist but she will not succeed alone.\n\n"
        "\tGreater victories can be won by bringing other characters safe to the Gate of Varenorn. Tarithel’s safe return is important and so is Rorthron's. The spoils of war play their part too. Bring to Varenorn any of the Crowns of Icemark and your victory will be the greater. It will be the greater still if you discover and return with any of the arcane objects upon which Shareth's power depends. Knowledge of these must be found during your struggles.\n\n"
        "\tIf, by any chance Morkin should be slain, only one victory remains to Luxor - Shareth's complete destruction. How this can be achieved, you must discover during your travels through the Icemark but be warned, it is no easy task. If Morkin dies. Luxor, racked with grief, will diminish in power.\n\n"
        "\tThe greatest victory of all is the destruction of Shareth and the safe return of Luxor, Morkin, Tarithel and Rorthron to Varenorn. Upon this event. glowing letters will flame on the surface of the Moonring, revealing to Luxor the ancient Watchwords of Midnight that when spoken will awaken all the land to whatever peril gathers. With these words at his command, the Moonprince will be sure to protect Midnight for as long as he lives."
        
    }
    
    ,
    {
        HELP_VICTORY,
        hf_none,
        "VICTORY FOR THE HEARTSTEALER\n\n"
        "\tShareth the Heartstealer has one single goal, the destruction of Luxor the Moonprince, he who slew her father, Doomdark, Witchking of Midnight and who, in her eyes, usurped her rightful inheritance.\n\n"
        "\tHer cold powers are, even now, greater than Doomdark's ever were and if Shareth can bring about the death of the Moonprince, Midnight will inevitably fall under her sway eventually. Shareth, therefore wins the game outright if Luxor is slain."
    }
    
    ,
    {
        HELP_BATTLE,
        hf_none,
        "ENGAGING IN BATTLE\n\n"
        "\tMinor skirmishes involving individual characters and small warbands are quick affairs and can take place at any stage throughout the day. Battles between armies, however, that will not be decided until the day is over.\n\n"
        "\tBecause a battle between armies is such a major event, you will not be able to move an army to the same location as an enemy army by using the simple MOVE option. Such a move is always one of the special actions you can opt for during the CHOOSE option. Some of the commanders you control may be so afraid that the choice to move them into battle does not even appear as one of the possibilities.\n"
        "\tDuring the course of the day, you can move as many armies into battle as you wish. If you move more than one army into the same battle, the program will keep track of their times of arrival (which may influence the outcome of the contest). However, once an army or a character has been moved into a battle, it will not be able to move again until the following day.\n\n"
        "\tAt dawn on the following day, the outcome will be known to your commanders. If the enemy has lost, his armies will have been destroyed in the night or have fled, leaving your armies and characters free to move. If the enemy has not lost, you have the choice of retreating with what is left of your armies or continuing the struggle for another day, possibly throwing in more forces. If, however, the enemy has won a decisive victory, when dawn breaks you will find your armies destroyed and your surviving commanders scattered: the enemy forces may have advanced far beyond the battlefield.\n\n"
        "\tMany things will influence the outcome of a battle: the number of troops, the type of terrain. the quality of the commanders and, of course, the strength of the icefear. But, as any warrior must, you must learn by experience."
    }
    
    ,
    {
        HELP_GROUPED,
        hf_important,
        "GROUPED LORDS\n\n"
        "\tWhen you take control of a lord who is following another, you will not be able to move them. Their leader's face will be shown at the bottom of the character's image. This will allow you to quickly choose the leading lord. To take independent control of the lord for movement, then you must ungroup them on the SELECT screen."
    }
    
    ,
    {
        HELP_TUTORIAL,
        hf_important,
        "HELP\n\n"
        "\tAt key points during the game, you will be presented with helpful information. Sometimes this information is indicated by a flashing TUTORIAL icon in the top right of the screen. If you want to read this helpful information then press the icon.\n\n"
        "\tYou can turn this tutorial off from the options screen. You may also like to read the full online guide by pressing the icon in bottom right of the main menu."
        
    }
    
    ,
    {
        HELP_TUTORIAL_OFF,
        hf_important|hf_always,
        "Tutorial mode - TURNED OFF.\n\n"
        "\tYou will not recieve any tutorial information while playing the game.\n\nYou can however read the online guide by pressing the icon in the bottom right corner of the main  menu."
        
    }
    
    ,
    {
        HELP_TUTORIAL_ON,
        hf_important|hf_always,
        "Tutorial mode - TURNED ON.\n\n"
        "\tAt key points during the game, you will be presented with helpful information. Sometimes this information is indicated by a flashing TUTORIAL icon in the top right of the screen. If you want to read this helpful information then press the icon.\n\nAlternatively you can read the online guide by pressing the icon in the bottom right corner of the main menu."
        
    }
    
};
#endif
 
