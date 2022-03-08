//
//  uibookmenu.cpp
//  midnight
//
//  Created by Chris Wild on 16/12/2017.
//
//
#include "uihelper.h"
#include "uibookmenu.h"
#include "uipanel.h"
#include "uibook.h"
#include "../system/moonring.h"
#include "../system/resolutionmanager.h"
#include "../ui/uioptionitem.h"
#include "MyButton.h"

USING_NS_CC;


uibookmenu::uibookmenu() :
    parent(nullptr)
{
}

uibookmenu* uibookmenu::createWithStory( uipanel* parent, storyinfo_t* story )
{
    uibookmenu* node = new (std::nothrow) uibookmenu();
    if (node && node->initWithStory(parent,story))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}


bool uibookmenu::initWithStory( uipanel* parent, storyinfo_t* story )
{
    if ( story == nullptr || story->count == 0 )
        return false;
 
    this->parent = parent;
 
    auto visibleSize = Director::getInstance()->getVisibleSize();
    f32 menuHeight = visibleSize.height;
    f32 menuWidth = visibleSize.width;
    
    
    if ( !initWithColor(Color4B(_clrBlack), menuWidth, menuHeight) )
        return false;
    
    chilli::ui::WidgetClickCallback callback = [&] (Ref* ref ) {
        auto button = static_cast<Widget*>(ref);
        if ( button == nullptr )
            return;
        
        layoutid_t id = static_cast<layoutid_t>(button->getTag());
        
        switch ( id ) {
            case ID_CLOSE: Notify(nullptr); break;
            default: break;
        }
        
    };
        
    f32 scrollWidth = menuWidth/2;
    
    this->setOpacity(ALPHA(0.75));
    this->setAnchorPoint(uihelper::AnchorCenter);
    
    auto scrollview = cocos2d::ui::ScrollView::create();
    scrollview->setDirection(cocos2d::ui::ScrollView::Direction::HORIZONTAL);
    this->addChild(scrollview);


    for ( int ii=0; ii< story->count; ii++ ) {
        f32 pad = ii == 0 ? 0 : RES(32);
        
        auto book1 = uibook::createWithChapter( &story->chapter[ii] );
        
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
        
        menuItem->addClickEventListener( [&,storyid] (Ref* ref ) {
                bookeventargs args;
                args.id = storyid;
                Notify(&args);
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
    auto close = uihelper::CreateImageButton("close", ID_CLOSE, callback ) ;
    uihelper::AddTopLeft(this,close, RES(8), RES(8) );

    uishortcutkeys::registerCallback(this,callback);
    addShortcutKey(ID_CLOSE, K_ESC);

    return true;
}


void uibookmenu::Show()
{    
    // stop the underlying parent
    // from getting any of the events
    parent->pauseEvents();
    
    uishortcutkeys::addKeyboardListener(this);

    // and show
    setLocalZOrder(ZORDER_POPUP);
    uihelper::AddCenter(parent,this);
    
}

void uibookmenu::Close()
{
    //
    parent->removeChild(this);
    
    // give the parent events back
    parent->resumeEvents();
}
