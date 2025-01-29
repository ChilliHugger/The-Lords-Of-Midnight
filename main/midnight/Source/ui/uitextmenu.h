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
#include "uishortcutkeys.h"

using namespace chilli::types;

class uitextmenu :
    public ax::ui::Scale9Sprite,
    public uishortcutkeys,
    public chilli::ui::NotificationInterface
{
protected:
    using Menu = ax::Menu;
    using MenuItem = ax::MenuItem;
    
private:
    uitextmenu() {};
    
public:
    ~uitextmenu();
    
    static uitextmenu* create( f32 width, uitextmenuitem* items, u32 count );
    void enableItem( u32 id, bool enabled );
    void showItem( u32 id, bool enabled );

protected:

    bool initWithItems( f32 width, uitextmenuitem* items, u32 count );
    MenuItem* getItem( u32 id );
    void notifyItem(Ref* ref);

    void refresh();

protected:
    uitextmenuitem*     items;
    u32                 items_count;
    f32                 paddingY;
    f32                 phoneYPadding;
    f32                 width;
    Menu*               mainmenu;
};


