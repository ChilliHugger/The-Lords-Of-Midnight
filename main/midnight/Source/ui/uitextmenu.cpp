//
//  uitextmenu.cpp
//  midnight
//
//  Created by Chris Wild on 09/12/2017.
//
//
#include "uihelper.h"
#include "uitextmenu.h"
#include "../system/moonring.h"
#include "../system/resolutionmanager.h"
#include "uitextmenu.h"


USING_NS_AX;
USING_NS_AX_UI;

uitextmenu* uitextmenu::create( f32 width, uitextmenuitem* items, u32 count )
{
    uitextmenu* node = new (std::nothrow) uitextmenu();
    if (node && node->initWithItems(width,items,count))
    {
        node->autorelease();
        return node;
    }
    AX_SAFE_DELETE(node);
    return nullptr;
}

uitextmenu::~uitextmenu()
{
}

bool uitextmenu::initWithItems( f32 menuwidth, uitextmenuitem* items, u32 count )
{
    if ( count == 0 || items == nullptr )
        return false;
    
    if ( !initWithFile(Rect::ZERO, BOX_BACKGROUND_FILENAME) )
        return false;
 
    auto clickCallback = [&] (Ref* sender ) {
        notifyItem(sender);
    };
     
 // map keyboard shortcut keys to layout children
    uishortcutkeys::registerCallback(this, clickCallback);

    auto rm = moonring::mikesingleton()->resolution;
    
    phoneYPadding = RES(12);
    
    this->items = items;
    this->items_count = count;
    
    paddingY = PHONE_SCALE(RES(20));
    this->width = menuwidth;

    if(rm->IsPhoneScaleEnabled())
    {
        width+=RES(64);
        phoneYPadding = RES(20);
    }
    
    this->setContentSize(Size(width, PHONE_SCALE(RES(128))) );
    this->setColor(Color3B::BLACK);
    this->setOpacity(ALPHA(alpha_1qtr));
    
    // menu
    mainmenu = Menu::create();
    
    
    // add items
    f32 height=0;
    for ( int ii=0; ii<items_count; ii++ ) {
        
        auto item = &items[ii];
        
        auto label = Label::createWithTTF( uihelper::font_config_big, item->type.text );
        label->getFontAtlas()->setAntiAliasTexParameters();
        label->setTextColor(Color4B::WHITE);
        label->setWidth(width);
        label->setAlignment(TextHAlignment::CENTER);

        auto menuItem = MenuItemLabel::create(label);
        menuItem->setTag(item->id);
        menuItem->setUserData(item);
        mainmenu->addChild(menuItem);
      
        addShortcutKey(menuItem, item->id, item->keyboard_shortcut);

        auto r = menuItem->getBoundingBox();
        height += r.size.height;

        menuItem->setCallback(clickCallback);
    }

    height += (phoneYPadding*(items_count-1));
    
    // resize background based on content
    this->setContentSize(Size(width,height+(2*paddingY)) );

    uihelper::AddCenter(this,mainmenu);
    mainmenu->setContentSize(Size(width,height+(2*paddingY)) );
    mainmenu->setPosition(Vec2::ZERO);
    mainmenu->setAnchorPoint(uihelper::AnchorTopLeft);
    mainmenu->setIgnoreAnchorPointForPosition(true);
    
    refresh();

    return true;
}

void uitextmenu::refresh()
{
    auto r1 = this->getBoundingBox();
    auto r2 = mainmenu->getBoundingBox();
    
    s32 height=0;
    s32 visibleItems=0;
    for ( auto item : mainmenu->getChildren() )
    {
        if(item->isVisible())
        {
            height += item->getBoundingBox().size.height;
            visibleItems++;
        }
    }
    height += (phoneYPadding*(visibleItems-1));

    setContentSize(Size(width,height+(2*paddingY)) );
    mainmenu->setContentSize(Size(width,height+(2*paddingY)) );
        
    // refresh positions
    auto offset = Vec2( width/2, height + paddingY );
    for ( auto item : mainmenu->getChildren() )
    {
        if(item->isVisible())
        {
            item->setAnchorPoint( uihelper::AnchorTopCenter );
            item->setPosition(offset);
            offset.y -= item->getBoundingBox().size.height+phoneYPadding;
        }
    }
}

MenuItem* uitextmenu::getItem( u32 id )
{
    for ( auto item : mainmenu->getChildren() ) {
        if ( item->getTag() == id ) {
            return static_cast<MenuItem*>(item);
        }
    }
    return nullptr;
}


void uitextmenu::enableItem( u32 id, bool enabled )
{
    auto menuitem = getItem(id);
    if (menuitem!=nullptr) {
        menuitem->setEnabled(enabled);
    }
}

void uitextmenu::showItem( u32 id, bool visible )
{
    auto menuitem = getItem(id);
    if (menuitem!=nullptr) {
        menuitem->setVisible(visible);
    }
    refresh();
}

void uitextmenu::notifyItem(Ref* sender)
{
    auto menuItem = dynamic_cast<MenuItemLabel*>(sender);
    if ( menuItem!= nullptr ) {
        menueventargs args;
        args.menuitem = static_cast<uitextmenuitem*>(menuItem->getUserData());
        Notify( &args );
    }
}
