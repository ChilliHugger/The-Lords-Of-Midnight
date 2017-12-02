//
//  uipopup.cpp
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//
#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "uipopup.h"
#include "resolutionmanager.h"

USING_NS_CC;

using namespace cocos2d::ui;

uipopup::uipopup( Scene* parent, point pos, size s, LPCSTR text )
{
    auto rect = parent->getBoundingBox();
    
    this->parent = parent;

    this->setContentSize( rect.size );
    this->setPosition(0, 0);

    auto rectNode = DrawNode::create();
    rectNode->drawSolidRect(Vec2(0,0), Vec2( rect.getMaxX(), rect.getMaxY()), Color4F(0,0,0,0.75f) );
    this->addChild(rectNode);

    // layout
    auto layout = Layout::create();
    layout->setBackGroundImage("misc/box_16.png");
    layout->setBackGroundImageScale9Enabled(true);
    layout->setContentSize(Size(s.cx,s.cy));
    layout->setPosition( Vec2( rect.getMidX(), rect.getMidY() ) ) ;
    layout->setAnchorPoint( Vec2(0.5,0.5) );
    this->addChild(layout);
    
    // add text
    auto label = Text::create(text, "fonts/sherwood.ttf", RES(40));
    label->ignoreContentAdaptWithSize(false);
    // TODO: calc height of text
    label->setContentSize(Size(s.cx,s.cy));
    label->setPosition( Vec2( s.cx/2, (s.cy/2) + RES(60) ) ) ;
    label->setTextHorizontalAlignment(TextHAlignment::CENTER);
    label->setTextVerticalAlignment(TextVAlignment::CENTER);
    label->setColor(Color3B::RED);
    layout->addChild(label);
    
    // TODO: Make these buttons part of a menu
    
    // add ok
    u32 paddingX = RES(10);
    u32 paddingY = RES(10);
    u32 middleX = s.cx / 2 ;
    
    auto yes = Button::create("misc/box_16.png");
    yes->setTitleFontName("fonts/celtic.ttf");
    yes->setTitleFontSize(RES(30));
    yes->setTitleColor(Color3B::RED);
    yes->setTouchEnabled(true);
    yes->setScale9Enabled(true);
    yes->setContentSize(Size(RES(128),RES(60)));
    yes->setTitleText("YES");
    yes->setAnchorPoint( Vec2(0,0) );
    yes->setPosition( Vec2(middleX+paddingX,paddingY ) );
    // Adjust for centreY without trailing character
    yes->getTitleRenderer()->setLineHeight(RES(25));
    yes->addTouchEventListener( [&](Ref* s,Widget::TouchEventType e) {
        OnYes();
    } );
    layout->addChild(yes);
    
    // add cancel
    auto no = Button::create("misc/box_16.png");
    no->setTitleFontName("fonts/celtic.ttf");
    no->setTitleFontSize(RES(30));
    no->setTitleColor(Color3B::RED);
    no->setTouchEnabled(true);
    no->setScale9Enabled(true);
    no->setContentSize(Size(RES(128),RES(60)));
    no->setTitleText("NO");
    no->setAnchorPoint( Vec2(1,0) );
    no->setPosition( Vec2(middleX-paddingX,paddingY) );
    no->getTitleRenderer()->setLineHeight(RES(25));
    no->addTouchEventListener( [&](Ref* s,Widget::TouchEventType e) {
        OnNo();
    } );
    layout->addChild(no);
    
}

void uipopup::Show()
{
    // stop the underlying parent
    // from getting any of the events
    parent->getEventDispatcher()->pauseEventListenersForTarget(parent,true);

    // now add an event listener
    // for the using cancelling
    // by touching off the main window
    auto listener1 = EventListenerTouchOneByOne::create();
    listener1->onTouchBegan = [&](Touch* touch, Event* event){
        OnNo();
        return true;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener1, this);
    
    // and show
    parent->addChild(this);
}

void uipopup::OnYes()
{
    Close();
    if ( onOk!=nullptr )
        onOk();
}

void uipopup::OnNo()
{
    Close();
    if ( onCancel!=nullptr )
        onCancel();
}


void uipopup::Close()
{
    //
    parent->removeChild(this);
    
    // give the parent events back
    parent->getEventDispatcher()->resumeEventListenersForTarget(parent,true);

}


