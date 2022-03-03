//
//  uitextmenuitem.h
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//

#ifndef uitextmenuitem_h
#define uitextmenuitem_h

#include "../frontend/layout_id.h"

enum eTextButton {
    TB_NORMAL=0,
    TB_DOUBLE=1,
};

typedef struct uitextmenuitem
{
    layoutid_t     id;
    union {
        const char*     text;
      //  uioptionitem*   element;
    } type ;
    
    cocos2d::EventKeyboard::KeyCode     keyboard_shortcut;
    LPCSTR  keyboard_shortcut_description;
    
    // nasty hack
    eTextButton text_button_type;
    //const char* text2;
    
} uitextmenuitem ;


#endif /* uitextmenuitem_h */
