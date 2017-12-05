//
//  language.h
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//

#ifndef language_h
#define language_h

#if defined(_LOM_)
    #define EXIT_MSG            "Do you want to exit The Lords of Midnight?"
    #define _UPDATE_PROMPT_     "You are about to open a browser window to download a new version\n\nAre you sure that you wish to continue?"
    #define _NOVELLA_PROMPT_    "You are about to open a browser window to read the Lords of Midnight Novella\n\nAre you sure that you wish to continue?"
    #define _GUIDE_PROMPT_      "You are about to open a browser window to read the Lords of Midnight Playing Guide\n\nAre you sure that you wish to continue?"
    #define _GUIDE_DOCUMENT_    "http://www.thelordsofmidnight.com/docs/Lords%20of%20Midnight_guide.pdf"
    #define _NOVELLA_DOCUMENT_PDF_  "http://www.thelordsofmidnight.com/docs/Lords%20of%20Midnight.pdf"
    #if defined(_OS_IOS_) || defined(_OS_OSX_)
        #define _NOVELLA_DOCUMENT_  "http://www.thelordsofmidnight.com/docs/Lords%20of%20Midnight.epub"
    #else
        #define _NOVELLA_DOCUMENT_  _NOVELLA_DOCUMENT_PDF_
    #endif

#else

#if defined(_DDR_)
    #define EXIT_MSG            "Do you want to exit Doomdark's Revenge?"
    #define _NOVELLA_PROMPT_    "You are about to open a browser window to read the Doomdark's Revenge Novella\n\nAre you sure that you wish to continue?"
    #define _GUIDE_PROMPT_      "You are about to open a browser window to read the Doomdark's Revenge Playing Guide\n\nAre you sure that you wish to continue?"
    #define _NOVELLA_DOCUMENT_PDF_  "http://www.doomdarksrevenge.com/docs/Doomdark's_Revenge.pdf"
    #define _GUIDE_DOCUMENT_    "http://www.doomdarksrevenge.com/docs/Doomdark's_Revenge_guide.pdf"
    #if defined(_OS_IOS_) || defined(_OS_OSX_)
        #define _NOVELLA_DOCUMENT_  "http://www.doomdarksrevenge.com/docs/Doomdark's_Revenge.epub"
    #else
        #define _NOVELLA_DOCUMENT_  _NOVELLA_DOCUMENT_PDF_
    #endif
#else
    #define EXIT_MSG        "Do you want to exit?"
#endif
#endif

#define CLOSE_STORY_MSG "Are you sure that you wish to close this story?"
#define NIGHT_MSG       "Are you sure that you are ready to end the day?"
#define UNDO_NIGHT_MSG  "Are you sure that you wish to return to night?"
#define UNDO_DAWN_MSG   "Are you sure that you wish to return to dawn?"

#define BUTTON_YES      "YES"
#define BUTTON_NO       "NO"






#endif /* language_h */
