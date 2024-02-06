//
//  language.h
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//

#ifndef language_h
#define language_h

    constexpr LPCSTR CREDITS_ORIGINAL_GAME              = "Original game by";
    constexpr LPCSTR CREDITS_PROGRAMMING                = "Programming";
    constexpr LPCSTR CREDITS_GRAPHICS                   = "Graphics redrawn by";
    constexpr LPCSTR CREDITS_TESTING                    = "Testing by";
    constexpr LPCSTR CREDITS_SPECIAL_THANKS             = "Special thanks to";
    constexpr LPCSTR CREDITS_WEBSITE                    = "Website";

    constexpr LPCSTR CREDITS_MIKE                       = "Mike Singleton";
    constexpr LPCSTR CREDITS_PROGRAMMERS                = "Chris Wild";
    constexpr LPCSTR CREDITS_ARTISTS                    = "Jure Rogelj";
    constexpr LPCSTR CREDITS_TESTERS                    = "Scott Gibbens & Nicholas Muge";

    constexpr LPCSTR CREDITS_THANKS                     = R"(Alasdair Allan, Chris Booker, Timothy Bromige, Iain Brown, Basem Chbaklo, Richard Clements, Menno Everaarts, Martin Graaskov, Ross Harris, Bill Hoggett, Thanos Koukoulis, Harvey Lodder, Gary Mcdonald, Fergus Mcneill, Jordan Myers, Kai Panholzer, Guy Preston, Gemma Rigby, Julian Rigby, Tam Rigby, Francisco Assis Rosa, Jean-Yves Rouffiac, David Ivan Perez Salazar, Andrew Smart, Stuart Voice, Seamus Waldron, Chris Webster, Jason Webster, David West,
and all the members of midnight/mu - www.midnightmu.com)";
 

    constexpr LPCSTR CREDITS_VERSION                    = "Version: " ;
        
    constexpr LPCSTR MAINMENU_NEW_STORY                 = "NEW STORY";
    constexpr LPCSTR MAINMENU_CONTINUE_STORY            = "CONTINUE STORY";
    constexpr LPCSTR MAINMENU_END_STORY                 = "END STORY";
    constexpr LPCSTR MAINMENU_CREDITS                   = "CREDITS";
    constexpr LPCSTR MAINMENU_OPTIONS                   = "OPTIONS";
    constexpr LPCSTR MAINMENU_EXIT                      = "EXIT";
    constexpr LPCSTR MAINMENU_UPDATE                    = "UPDATE";

    constexpr LPCSTR KEYBOARD_KEY_1                     = "1";
    constexpr LPCSTR KEYBOARD_KEY_2                     = "2";
    constexpr LPCSTR KEYBOARD_KEY_3                     = "3";
    constexpr LPCSTR KEYBOARD_KEY_4                     = "4";
    constexpr LPCSTR KEYBOARD_KEY_5                     = "5";
    constexpr LPCSTR KEYBOARD_KEY_6                     = "6";
    constexpr LPCSTR KEYBOARD_KEY_7                     = "7";
    constexpr LPCSTR KEYBOARD_KEY_8                     = "8";
    constexpr LPCSTR KEYBOARD_KEY_9                     = "9";
    constexpr LPCSTR KEYBOARD_KEY_0                     = "0";

    constexpr LPCSTR KEYBOARD_KEY_C                     = "C";
    constexpr LPCSTR KEYBOARD_KEY_D                     = "D";
    constexpr LPCSTR KEYBOARD_KEY_E                     = "E";
    constexpr LPCSTR KEYBOARD_KEY_G                     = "G";
    constexpr LPCSTR KEYBOARD_KEY_H                     = "H";
    constexpr LPCSTR KEYBOARD_KEY_N                     = "N";
    constexpr LPCSTR KEYBOARD_KEY_O                     = "O";
    constexpr LPCSTR KEYBOARD_KEY_R                     = "R";
    constexpr LPCSTR KEYBOARD_KEY_U                     = "U";
    constexpr LPCSTR KEYBOARD_KEY_Z                     = "Z";
    constexpr LPCSTR KEYBOARD_KEY_ESC                   = "ESC";
        
    constexpr LPCSTR OPTIONS_ONOFF_OFF                  = "OFF";
    constexpr LPCSTR OPTIONS_ONOFF_ON                   = "ON";
    constexpr LPCSTR OPTIONS_YESNO_NO                   = "NO";
    constexpr LPCSTR OPTIONS_YESNO_YES                  = "YES";
    
    constexpr LPCSTR OPTIONS_MOVEMENT_SWIPE_PUSH        = "SWIPE & PUSH";
    constexpr LPCSTR OPTIONS_MOVEMENT_PUSH              = "PUSH";
    constexpr LPCSTR OPTIONS_MOVEMENT_SWIPE             = "SWIPE";
    constexpr LPCSTR OPTIONS_MOVEMENT_PRESS_SWIPE       = "PRESS LOOK/SWIPE MOVE";
    
    constexpr LPCSTR OPTIONS_THINK_SWIPE                = "SWIPE LEFT/RIGHT";
    constexpr LPCSTR OPTIONS_THINK_PUSH                 = "PUSH";
    constexpr LPCSTR OPTIONS_THINK_ARROWS               = "ARROWS ONLY";
    
    constexpr LPCSTR OPTIONS_COMPASS_DELAY_OFF          = "OFF";
    constexpr LPCSTR OPTIONS_COMPASS_DELAY_NORMAL       = "NORMAL PRESS";
    constexpr LPCSTR OPTIONS_COMPASS_DELAY_SHORT        = "SHORT PRESS";
    constexpr LPCSTR OPTIONS_COMPASS_DELAY_LONG         = "LONG PRESS";

    constexpr LPCSTR OPTIONS_SLOW                       = "SLOW";
    constexpr LPCSTR OPTIONS_FAST                       = "FAST";
    
    constexpr LPCSTR OPTIONS_BRIEF                      = "BRIEF";
    constexpr LPCSTR OPTIONS_FULL                       = "FULL";
    
    constexpr LPCSTR OPTIONS_KEYBOARD_CLASSIC           = "CLASSIC";
    constexpr LPCSTR OPTIONS_KEYBOARD_NEW               = "NEW";
    
    constexpr LPCSTR OPTIONS_NOVELLA_EBOOK              = "EBOOK";
    constexpr LPCSTR OPTIONS_NOVELLA_PDF                = "PDF";
    
    constexpr LPCSTR OPTIONS_SCREEN_FULLSCREEN          = "FULLSCREEN";
    constexpr LPCSTR OPTIONS_SCREEN_SMALL               = "SMALL";
    constexpr LPCSTR OPTIONS_SCREEN_MEDIUM              = "MEDIUM";
    constexpr LPCSTR OPTIONS_SCREEN_LARGE               = "LARGE";
    constexpr LPCSTR OPTIONS_SCREEN_NOTSUPPORTED        = "FULLSCREEN - NOT SUPPORTED";
    
    constexpr LPCSTR OPTIONS_CURSOR_SMALL               = "SMALL";
    constexpr LPCSTR OPTIONS_CURSOR_MEDIUM              = "MEDIUM";
    constexpr LPCSTR OPTIONS_CURSOR_LARGE               = "LARGE";
    
    constexpr LPCSTR OPTIONS_SCREEN_DISPLAY             = "DISPLAY";
    constexpr LPCSTR OPTIONS_SCREEN_GAME                = "GAME";
    constexpr LPCSTR OPTIONS_SCREEN_CONTROL             = "CONTROL";
    constexpr LPCSTR OPTIONS_SCREEN_HELP                = "HELP";
    constexpr LPCSTR OPTIONS_SCREEN_RULES               = "GAME RULES";
    constexpr LPCSTR OPTIONS_SCREEN_MAINMENU            = "MAIN MENU";
    constexpr LPCSTR OPTIONS_SCREEN_SCREENMODE          = "SCREEN MODE";
    constexpr LPCSTR OPTIONS_SCREEN_TRANSITIONS         = "SCREEN TRANSITIONS";
    constexpr LPCSTR OPTIONS_SCREEN_FLIPSCREEN          = "FLIP SCREEN";
    constexpr LPCSTR OPTIONS_SCREEN_AUTOFIGHT           = "AUTO FIGHT";
    constexpr LPCSTR OPTIONS_SCREEN_AUTOUNHIDE          = "AUTO UNHIDE";
    constexpr LPCSTR OPTIONS_SCREEN_AUTOSEEK            = "AUTO SEEK";
    constexpr LPCSTR OPTIONS_SCREEN_AUTOAPPROACH        = "AUTO APPROACH";
    constexpr LPCSTR OPTIONS_SCREEN_APPROACH            = "AFTER APPROACH";
    constexpr LPCSTR OPTIONS_SCREEN_NIGHTDISPLAY        = "NIGHT DISPLAY";
    constexpr LPCSTR OPTIONS_SCREEN_NIGHTCONFIRM        = "NIGHT CONFIRM";
    constexpr LPCSTR OPTIONS_SCREEN_BATTLEREPORT        = "BATTLE REPORT";
    constexpr LPCSTR OPTIONS_SCREEN_COMPASSDELAY        = "COMPASS DELAY";
    constexpr LPCSTR OPTIONS_SCREEN_NAVIGATIONSTYLE     = "NAVIGATION STYLE";
    constexpr LPCSTR OPTIONS_SCREEN_KEYBOARDSTYLE       = "KEYBOARD STYLE";
    constexpr LPCSTR OPTIONS_SCREEN_CURSORSIZE          = "CURSOR SIZE";
    constexpr LPCSTR OPTIONS_SCREEN_DIFFICULTY          = "DIFFICULTY";

    constexpr LPCSTR OPTIONS_SCREEN_TUTORIAL            = "TUTORIAL";
    constexpr LPCSTR OPTIONS_SCREEN_MOVEMENTINDICATORS  = "MOVEMENT INDICATORS";
    constexpr LPCSTR OPTIONS_SCREEN_NOVELLA             = "NOVELLA";
 
    constexpr LPCSTR OPTIONS_SCREEN_RULE_1              = "IMPASSABLE MOUNTAINS";
    constexpr LPCSTR OPTIONS_SCREEN_RULE_2              = "AI IMPASSABLE MOUNTAINS";
    constexpr LPCSTR OPTIONS_SCREEN_RULE_3              = "ADD MOUNTAIN PASSES";
    constexpr LPCSTR OPTIONS_SCREEN_RULE_4              = "SOLE MOUNTAINEER";
    constexpr LPCSTR OPTIONS_SCREEN_RULE_5              = "FEY RECRUITING";
    constexpr LPCSTR OPTIONS_SCREEN_FEY_RECRUIT_ON      = "ON";
    constexpr LPCSTR OPTIONS_SCREEN_FEY_RECRUIT_OFF     = "OFF";
    constexpr LPCSTR OPTIONS_SCREEN_FEY_RECRUIT_DREAMS  = "LORD OF DREAMS RECRUITED";
    constexpr LPCSTR OPTIONS_SCREEN_RULE_6              = "FAST TUNNELS";

    constexpr LPCSTR OPTIONS_SCREEN_RULE_7              = "MOVEMENT MODIFIERS";
    constexpr LPCSTR OPTIONS_SCREEN_RULE_ORIGINAL       = "ORIGINAL SPECTRUM";
    constexpr LPCSTR OPTIONS_SCREEN_RULE_INTENDED       = "INTENDED SPECTRUM";
    constexpr LPCSTR OPTIONS_SCREEN_RULE_C64            = "ORIGINAL C64";
    constexpr LPCSTR OPTIONS_SCREEN_RULE_8              = "UPGRADE SHARETH ARMY AI";


    
    constexpr LPCSTR OPTIONS_SCREEN_DF_NORMAL           = "NORMAL";
    constexpr LPCSTR OPTIONS_SCREEN_DF_EASY             = "EASY";
    constexpr LPCSTR OPTIONS_SCREEN_DF_MEDIUM           = "MEDIUM";
    constexpr LPCSTR OPTIONS_SCREEN_DF_HARD             = "HARD";
    
    constexpr LPCSTR OPTIONS_SCREEN_APPROACH_SWAP       = "NEW LORD";
    constexpr LPCSTR OPTIONS_SCREEN_APPROACH_STAY       = "DO NOTHING";
    
#if defined(_LOM_)
    constexpr LPCSTR SPLASH_SCREEN_EXPLORE              = "Now explore the epic world of";
    constexpr LPCSTR SPLASH_SCREEN_TITLE                = "THE LORDS OF MIDNIGHT";
    constexpr LPCSTR SPLASH_SCREEN_BY                   = "         by";
    constexpr LPCSTR SPLASH_SCREEN_MIKE                 = "Mike Singleton";
        
    constexpr LPCSTR EXIT_MSG                           = "Do you want to exit\nThe Lords of Midnight?";
    constexpr LPCSTR _UPDATE_PROMPT_                    = "You are about to open a browser window to download a new version\n\nAre you sure that you wish to continue?";
    constexpr LPCSTR _NOVELLA_PROMPT_                   = "You are about to open a browser window to read the Lords of Midnight Novella\n\nAre you sure that you wish to continue?";
    constexpr LPCSTR _GUIDE_PROMPT_                     = "You are about to open a browser window to read the Lords of Midnight Playing Guide\n\nAre you sure that you wish to continue?";
    constexpr LPCSTR _GUIDE_DOCUMENT_                   = "http://www.thelordsofmidnight.com/docs/Lords%20of%20Midnight_guide.pdf";
    constexpr LPCSTR _NOVELLA_DOCUMENT_PDF_             = "http://www.thelordsofmidnight.com/docs/Lords%20of%20Midnight.pdf";
    #if defined(_OS_IOS_) || defined(_OS_OSX_)
    constexpr LPCSTR _NOVELLA_DOCUMENT_                 = "http://www.thelordsofmidnight.com/docs/Lords%20of%20Midnight.epub";
    #else
    constexpr LPCSTR _NOVELLA_DOCUMENT_                 = _NOVELLA_DOCUMENT_PDF_;
    #endif

    constexpr LPCSTR CREDITS_WEBSITE_ADDRESS            = "www.thelordsofmidnight.com\n\n\n\n\n\n\n\n\n\n";
#else

#if defined(_DDR_)
    constexpr LPCSTR EXIT_MSG                           = "Do you want to exit\nDoomdark's Revenge?";
    constexpr LPCSTR _UPDATE_PROMPT_                    = "You are about to open a browser window to download a new version\n\nAre you sure that you wish to continue?";
    constexpr LPCSTR _NOVELLA_PROMPT_                   = "You are about to open a browser window to read the Doomdark's Revenge Novella\n\nAre you sure that you wish to continue?";
    constexpr LPCSTR _GUIDE_PROMPT_                     = "You are about to open a browser window to read the Doomdark's Revenge Playing Guide\n\nAre you sure that you wish to continue?";
    constexpr LPCSTR _NOVELLA_DOCUMENT_PDF_             = "http://www.doomdarksrevenge.com/docs/Doomdark's_Revenge.pdf";
    constexpr LPCSTR _GUIDE_DOCUMENT_                   = "http://www.doomdarksrevenge.com/docs/Doomdark's_Revenge_guide.pdf";
    #if defined(_OS_IOS_) || defined(_OS_OSX_)
    constexpr LPCSTR _NOVELLA_DOCUMENT_                 = "http://www.doomdarksrevenge.com/docs/Doomdark's_Revenge.epub";
    #else
    constexpr LPCSTR _NOVELLA_DOCUMENT_                 = _NOVELLA_DOCUMENT_PDF_;
    #endif
    constexpr LPCSTR CREDITS_WEBSITE_ADDRESS            = "www.doomdarksrevenge.com\n\n\n\n\n\n\n\n\n\n";
#else
    constexpr LPCSTR EXIT_MSG                           = "Do you want to exit?";
#endif
#endif

    constexpr LPCSTR CLOSE_STORY_MSG                    = "Are you sure that you wish to close this story?";
    constexpr LPCSTR NIGHT_MSG                          = "Are you sure that you are ready to end the day?";
    constexpr LPCSTR UNDO_NIGHT_MSG                     = "Are you sure that you wish to return to night?";
    constexpr LPCSTR UNDO_DAWN_MSG                      = "Are you sure that you wish to return to dawn?";
    constexpr LPCSTR CHANGE_DISPLAY_MSG                 = "Are you sure that you wish to change your game resolution?";
    
    constexpr LPCSTR BUTTON_YES                         = "YES";
    constexpr LPCSTR BUTTON_NO                          = "NO";
    constexpr LPCSTR BUTTON_OK                          = "OK";


    constexpr LPCSTR DIRECTION_N                        = "N";
    constexpr LPCSTR DIRECTION_NE                       = "NE";
    constexpr LPCSTR DIRECTION_E                        = "E";
    constexpr LPCSTR DIRECTION_SE                       = "SE";
    constexpr LPCSTR DIRECTION_S                        = "S";
    constexpr LPCSTR DIRECTION_SW                       = "SW";
    constexpr LPCSTR DIRECTION_W                        = "W";
    constexpr LPCSTR DIRECTION_NW                       = "NW";
  

#endif /* language_h */
