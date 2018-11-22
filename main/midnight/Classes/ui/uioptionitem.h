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
    static uioptionitem* create( f32 width, uitextmenuitem* item );
    void setValue( const std::string& );
  
protected:
    bool initWithItem( f32 width, uitextmenuitem* item );
    cocos2d::Label* title;
    cocos2d::Label* value;
};

class MenuItemNode : public cocos2d::MenuItemSprite
{
public:
    static MenuItemNode* create( Node* node );

private:
    bool initWithNode( Node* node);
    virtual void selected();
    virtual void unselected();

};

