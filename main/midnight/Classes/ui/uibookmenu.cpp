//
//  uibookmenu.cpp
//  midnight
//
//  Created by Chris Wild on 16/12/2017.
//
//

#include "uibookmenu.h"
#include "uibook.h"
#include "uihelper.h"
#include "../frontend/resolutionmanager.h"
#include "../ui/uioptionitem.h"
#include "MyButton.h"


USING_NS_CC;

uibookmenu::uibookmenu( storyinfo_t* story )
{
    if ( story == nullptr || story->count == 0 )
        return;
    
    f32 menuHeight = RES(768);
    f32 menuWidth = RES(1024);
    f32 scrollWidth = menuWidth/2;
    
    initWithColor(Color4B(_clrBlack), menuWidth, menuHeight);
    this->setOpacity(ALPHA(0.75));
    this->setAnchorPoint(uihelper::AnchorCenter);
    
    auto scrollview = ui::ScrollView::create();
    scrollview->setDirection(ui::ScrollView::Direction::HORIZONTAL);
    this->addChild(scrollview);


    for ( int ii=0; ii< story->count; ii++ ) {
        f32 pad = ii == 0 ? 0 : RES(32);
        
        auto book1 = new uibook( &story->chapter[ii] );
        
        auto menuItem = MyButton::create(BOX_BACKGROUND_FILENAME);
        menuItem->setContentSize(Size(RES(512), RES(400)) );
        menuItem->setScale9Enabled(true);
        uihelper::AddCenter(menuItem, book1);
        
        scrollview->addChild(menuItem);
        
        menuItem->setPosition(Vec2(scrollWidth+pad,menuHeight/2));
        menuItem->setAnchorPoint(uihelper::AnchorCenter);
        menuItem->setIgnoreAnchorPointForPosition(false);
        scrollWidth += menuItem->getBoundingBox().size.width + pad;

        auto storyid = story->chapter[ii].id;
        
        bookeventargs args;
        menuItem->addClickEventListener( [&,storyid] (Ref* ref ) {
            args.id = storyid;
            Notify( &args );
        });

    }
    
    scrollview->setInnerContainerSize(Size(scrollWidth,menuHeight));
    scrollview->setContentSize(Size(getBoundingBox().size.width,menuHeight));
    
    bool scrollingEnabled = story->count > 1 ;
    
    scrollview->setBounceEnabled( scrollingEnabled );
    scrollview->setScrollBarEnabled( scrollingEnabled );
    scrollview->setPosition(Vec2::ZERO);
    scrollview->setAnchorPoint(Vec2::ZERO);

    // Close Button in top left corner
    auto close = uihelper::CreateImageButton("close", 0, [&](Ref* ref) {
                                                    Notify( nullptr );
                                             });
    uihelper::AddTopLeft(this,close, RES(8), RES(8) );

    
    
    
}

