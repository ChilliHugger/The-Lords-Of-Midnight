//
//  uitextmenu.h
//  midnight
//
//  Created by Chris Wild on 09/12/2017.
//
//

#pragma once

#include "uielement.h"
#include "../ui/uitextmenuitem.h"
#include "../ui/uieventargs.h"

using namespace chilli::types;

class uitextmenu :
    public cocos2d::ui::Scale9Sprite,
    public chilli::ui::NotificationInterface
{
protected:
    using Menu = cocos2d::Menu;
    
private:
    uitextmenu() {};
    
public:
    static uitextmenu* create( f32 width, uitextmenuitem* items, u32 count );
    void enableItem( u32 id, bool enabled );
    void showItem( u32 id, bool enabled );
    

protected:

    bool initWithItems( f32 width, uitextmenuitem* items, u32 count );

    void refresh();

protected:
    uitextmenuitem*     items;
    u32                 items_count;
    f32                 paddingY;
    f32                 phoneYPadding;
    f32                 width;
    Menu*               mainmenu;
};


