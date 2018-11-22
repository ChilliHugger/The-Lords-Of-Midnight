//
//  uitextmenu.cpp
//  midnight
//
//  Created by Chris Wild on 09/12/2017.
//
//

#include "uitextmenu.h"
#include "../system/resolutionmanager.h"
#include "uihelper.h"
#include "uitextmenu.h"


USING_NS_CC;

using namespace cocos2d::ui;

uitextmenu* uitextmenu::create( f32 width, uitextmenuitem* items, u32 count )
{
    uitextmenu* node = new (std::nothrow) uitextmenu();
    if (node && node->initWithItems(width,items,count))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}



bool uitextmenu::initWithItems( f32 width, uitextmenuitem* items, u32 count )
{
    if ( count == 0 || items == nullptr )
        return false;
    
    if ( !initWithFile(Rect::ZERO, BOX_BACKGROUND_FILENAME) )
        return false;
    
    this->items = items;
    this->items_count = count;
    
    paddingY = RES(30);
    this->width = width;
    this->setContentSize(Size(width, RES(128)) );
    this->setColor(Color3B::BLACK);
    this->setOpacity(ALPHA(0.25));
    
    // menu
    mainmenu = Menu::create();

    // add items
    f32 height=0;
    for ( int ii=0; ii<items_count; ii++ ) {
        
        auto item = &items[ii];
        
        auto label = Label::createWithTTF( uihelper::font_config_big, item->type.text );
        label->getFontAtlas()->setAntiAliasTexParameters();
        label->setTextColor(Color4B::WHITE);

        auto menuItem = MenuItemLabel::create(label);
        menuItem->setTag(item->id);
        mainmenu->addChild(menuItem);
    
        auto r = menuItem->getBoundingBox();
        height += r.size.height;
        
        menuItem->setCallback([&,item](cocos2d::Ref *sender) {
            menueventargs args;
            args.menuitem = item;
            Notify( &args );
        } );

    }
    
    // resize background based on content
    this->setContentSize(Size(width,height+(2*paddingY)) );

    uihelper::AddCenter(this,mainmenu);
    mainmenu->setContentSize(Size(width,height+(2*paddingY)) );
    mainmenu->setPosition(Vec2::ZERO);
    mainmenu->setAnchorPoint(uihelper::AnchorTopLeft);
    mainmenu->setIgnoreAnchorPointForPosition(true);
    
    auto r1 = this->getBoundingBox();
    auto r2 = mainmenu->getBoundingBox();
    
    // refresh positions
    auto offset = Vec2( width/2, height + paddingY );
    for ( auto item : mainmenu->getChildren() ) {

        item->setAnchorPoint( uihelper::AnchorTopCenter );
        item->setPosition(offset);
        offset.y -= item->getBoundingBox().size.height;
    }

    return true;
}

void uitextmenu::EnableItem( u32 id, bool enabled )
{
    for ( auto item : mainmenu->getChildren() ) {
        if ( item->getTag() == id ) {
            auto menuitem = static_cast<MenuItem*>(item);
            menuitem->setEnabled(enabled);
            break;
        }
    }
}

