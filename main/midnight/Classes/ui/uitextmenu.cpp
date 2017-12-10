//
//  uitextmenu.cpp
//  midnight
//
//  Created by Chris Wild on 09/12/2017.
//
//

#include "uitextmenu.h"
#include "../frontend/resolutionmanager.h"
#include "uihelper.h"
#include "uitextmenu.h"


USING_NS_CC;

using namespace cocos2d::ui;


uitextmenu::uitextmenu( f32 width, uitextmenuitem* items, u32 count )
{
    this->items = items;
    this->items_count = count;
    
    itemHeight = RES(40);
    paddingY = RES(30);
    this->width = width;
    
    if ( initWithFile(Rect::ZERO, BOX_BACKGROUND_FILENAME) )
    {
        autorelease();
    }
    
    Init();
}


void uitextmenu::Init()
{
    this->setContentSize(Size(width, RES(128)) );
    this->setColor(Color3B::BLACK);
    this->setOpacity(ALPHA(0.25));
    
    // menu
    auto mainmenu = Menu::create();
    this->addChild(mainmenu);

    // add items
    f32 height=0;
    for ( int ii=0; ii<items_count; ii++ ) {
        
        auto item = &items[ii];
        
        auto label = Label::createWithTTF( uihelper::font_config_big, item->type.text );
        label->getFontAtlas()->setAntiAliasTexParameters();
        label->setTextColor(Color4B::WHITE);
        label->setLineHeight(itemHeight);

        auto menuItem = MenuItemLabel::create(label);
        mainmenu->addChild(menuItem);
        
        auto r = menuItem->getBoundingBox();
        height += r.size.height;
        
        menueventargs args;
        menuItem->setCallback([&,item](cocos2d::Ref *sender) {
            args.menuitem = item;
            Notify( &args );
        } );

    }
    
    // resize background based on content
    this->setContentSize(Size(width,height+(2*paddingY)) );
    uihelper::PositionParentTopLeft(mainmenu);
    
    // refresh positions
    auto offset = Vec2( width/2, 0 - paddingY );
    for ( auto item : mainmenu->getChildren() ) {

        item->setAnchorPoint( uihelper::AnchorTopCenter );
        item->setPosition(offset);
        offset.y -= item->getBoundingBox().size.height;
    }


}
    

