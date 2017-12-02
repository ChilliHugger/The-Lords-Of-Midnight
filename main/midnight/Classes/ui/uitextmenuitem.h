//
//  uitextmenuitem.h
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//

#ifndef uitextmenuitem_h
#define uitextmenuitem_h

typedef struct uitextmenuitem
{
    u32     id;
    union {
        const char*     text;
        //Node*      element;
    } type ;
    
    cocos2d::EventKeyboard::KeyCode     keyboard_shortcut;
    LPCSTR  keyboard_shortcut_description;
    
    // nasty hack
    //eTextButton text_button_type;
    //const char* text2;
    
} uitextmenuitem ;

#endif /* uitextmenuitem_h */
