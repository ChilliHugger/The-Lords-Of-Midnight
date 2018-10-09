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

class uitextmenu : public cocos2d::ui::Scale9Sprite
                 , public uinotificationinterface
{
private:
    uitextmenu() {};
    
public:
	uitextmenu( f32 width, uitextmenuitem* items, u32 count );
    
    void EnableItem( u32 id, bool enabled );
    
public:
private:

    void Init();

protected:
    uitextmenuitem*     items;
    u32                 items_count;
    f32                 itemHeight;
    f32                 paddingY;
    f32                 width;
    cocos2d::Menu*      mainmenu;
};


