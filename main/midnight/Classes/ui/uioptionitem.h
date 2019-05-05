//
//  uioptionitem.h
//  midnight
//
//  Created by Chris Wild on 09/12/2017.
//

#pragma once

#include "uielement.h"
#include "uitextmenuitem.h"


class uioptionitem :
    public cocos2d::ui::Scale9Sprite
{
protected:
    using Label = cocos2d::Label;
    
public:
    static uioptionitem* create( f32 width, uitextmenuitem* item );
    void setValue( const std::string& );
  
protected:
    bool initWithItem( f32 width, uitextmenuitem* item );

protected:
    Label* title;
    Label* value;
};

class MenuItemNode : public cocos2d::MenuItemSprite
{
protected:
    using Node = cocos2d::Node;

public:
    static MenuItemNode* create( Node* node );

private:
    bool initWithNode( Node* node);
    virtual void selected();
    virtual void unselected();

};

