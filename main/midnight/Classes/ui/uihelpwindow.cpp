//
//  uihelpwindow.cpp
//  midnight
//
//  Created by Chris Wild on 11/12/2017.
//
//

#include "uihelper.h"
#include "uihelpwindow.h"
#include "uipanel.h"
#include "../frontend/language.h"
#include "resolutionmanager.h"
#include "../system/moonring.h"


USING_NS_CC;
using namespace cocos2d::ui;

uihelpwindow::uihelpwindow( uipanel* parent, helpid_t id)
{
    auto rect = parent->getBoundingBox();
    
    this->parent = parent;
    this->id = id;
    
    auto text = parent->GetMoonring()->help->Get(id);
    
    this->setContentSize( rect.size );
    this->setPosition( Vec2::ZERO );
    
    auto rectNode = DrawNode::create();
    rectNode->drawSolidRect(Vec2(0,0), Vec2( rect.getMaxX(), rect.getMaxY()), Color4F(0,0,0,0.75f) );
    this->addChild(rectNode);
    
    //
    f32 layout_padding = RES(32);
    f32 width = RES(800);
    f32 maxHeight = RES(500);
    f32 innerWidth = width-(2*layout_padding);
    
    // layout
    //auto
    layout = Layout::create();
    layout->setBackGroundImage(BOX_BACKGROUND_FILENAME);
    layout->setBackGroundImageScale9Enabled(true);
    uihelper::AddCenter( this,layout ) ;

    auto scrollView = ui::ScrollView::create();
    scrollView->setDirection(ui::ScrollView::Direction::VERTICAL);
    layout->addChild(scrollView);

    // add text
    auto label = Label::createWithTTF(text, FONT_FILENAME, FONT_SIZE_BIG);
    label->setColor(_clrDarkRed);
    label->setAlignment(TextHAlignment::LEFT);
    label->setLineHeight(RES(FONT_SIZE_BIG));
    label->setLineSpacing(0);
    label->setMaxLineWidth(innerWidth);
    label->enableWrap(true);
    scrollView->addChild(label);

    // calc label height
    auto textHeight = MIN(label->getContentSize().height,maxHeight) ;
    auto boxheight = textHeight + (2*layout_padding) ;
    
    layout->setContentSize(Size(width,boxheight));
    uihelper::PositionParentTopLeft( label, 0, layout_padding ) ;
    
    scrollView->setInnerContainerSize(Size(innerWidth,label->getContentSize().height));
    
    // allow some extra padding for the scrollview
    // so that the scrollbar is in the right padding area
    scrollView->setContentSize(Size(innerWidth+layout_padding,textHeight));
    
    bool scrollingEnabled = label->getContentSize().height != textHeight ;
    
    scrollView->setBounceEnabled( scrollingEnabled );
    scrollView->setScrollBarEnabled( scrollingEnabled );
    
    uihelper::PositionParentTopLeft(scrollView,layout_padding,layout_padding);
    uihelper::PositionParentTopLeft(label);
    

    // Close Button in top left corner
    auto close = uihelper::CreateImageButton("close", 0, [&](Ref* ref)
                                             {
                                                 OnClose();
                                             });
    uihelper::AddTopLeft(layout,close, RES(8), RES(8) );
    close->setAnchorPoint(uihelper::AnchorCenter);
    
    this->setLocalZOrder(ZORDER_NEAR);
}

void uihelpwindow::Show( MXVoidCallback callback )
{
    closeCallback = callback ;
    
    // stop the underlying parent
    // from getting any of the events
    parent->getEventDispatcher()->pauseEventListenersForTarget(parent,true);
    
    // now add an event listener
    // for the using cancelling
    // by touching off the main window
    auto listener1 = EventListenerTouchOneByOne::create();
    listener1->onTouchBegan = [&](Touch* touch, Event* event){
        // eat the touch in the message area
        if ( layout->getBoundingBox().containsPoint(touch->getLocation()) )
            return true;
        OnClose();
        return true;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener1, this);
    
    // and show
    parent->addChild(this);
}

void uihelpwindow::OnClose()
{
    // Flsg this particular help event as having been shown
    auto mr = moonring::mikesingleton();
    mr->help->Shown( id );
    mr->help->Save( mr->stories->current_story() );
    
    Close();
    if ( closeCallback!=nullptr )
        closeCallback();
}

void uihelpwindow::Close()
{
    //
    parent->removeChild(this);
    
    // give the parent events back
    parent->getEventDispatcher()->resumeEventListenersForTarget(parent,true);
    
}



