//
//  uioptionitem.h
//  midnight
//
//  Created by Chris Wild on 09/12/2017.
//

#pragma once

#include "uielement.h"
#include "uitextmenuitem.h"


class uioptionitem : public cocos2d::ui::Scale9Sprite
{
public:
    uioptionitem( f32 width, uitextmenuitem* item );
    
    void setValue( const std::string& );
  
protected:
    cocos2d::Label* title;
    cocos2d::Label* value;
};


//class MenuItemOption : public cocos2d::MenuItemSprite
//{
//public:
//    static MenuItemOption* create( uioptionitem* option);
//
//    virtual void selected();
//    virtual void unselected();
//
//};

class MenuItemNode : public cocos2d::MenuItemSprite
{
public:
    static MenuItemNode* create( Node* node );

    virtual void selected();
    virtual void unselected();

};

