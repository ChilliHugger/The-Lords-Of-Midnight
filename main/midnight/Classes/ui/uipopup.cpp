//
//  uipopup.cpp
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//

#include "uihelper.h"
#include "uipopup.h"
#include "../frontend/language.h"
#include "resolutionmanager.h"

USING_NS_CC;
using namespace cocos2d::ui;

uipopup::uipopup( Scene* parent, point pos, f32 width, LPCSTR text )
{
    auto rect = parent->getBoundingBox();
    
    this->parent = parent;

    this->setContentSize( rect.size );
    this->setPosition( Vec2::ZERO );
    
    auto rectNode = DrawNode::create();
    rectNode->drawSolidRect(Vec2(0,0), Vec2( rect.getMaxX(), rect.getMaxY()), Color4F(0,0,0,0.75f) );
    this->addChild(rectNode);

    //
    f32 layout_padding = RES(20);
    f32 button_height = RES(60);
    f32 button_width = RES(128);
    
    // layout
    //auto
    layout = Layout::create();
    layout->setBackGroundImage(BOX_BACKGROUND_FILENAME);
    layout->setBackGroundImageScale9Enabled(true);
    uihelper::AddCenter( this, layout ) ;
    
    // add text
    auto label = Label::createWithTTF(text, FONT_FILENAME, FONT_SIZE_BIG);
    label->setColor(_clrBlue);
    label->setAlignment(TextHAlignment::CENTER);
    label->setLineHeight(RES(FONT_SIZE_BIG));
    label->setLineSpacing(0);
    label->setMaxLineWidth(width-(2*layout_padding));
    label->enableWrap(true);
    layout->addChild(label);
    label->setAnchorPoint( uihelper::AnchorTopCenter );
    
    // calc label height
    auto h = label->getContentSize().height + (3*layout_padding) + button_height;
    layout->setContentSize(Size(width,h));
    uihelper::PositionParentTopCenter( label, 0, layout_padding ) ;
    
    // TODO: Make these buttons part of a menu
    
    auto yes = uihelper::CreateBoxButton(Size(button_width,button_height));
    yes->setTitleText(BUTTON_YES);
    layout->addChild(yes);
    yes->setAnchorPoint( uihelper::AnchorBottomRight );
    uihelper::PositionParentBottomCenter(yes, -layout_padding, layout_padding);
    yes->addTouchEventListener( [&](Ref* s,Widget::TouchEventType e) {
        OnYes();
    } );
    
    // add cancel
    auto no = uihelper::CreateBoxButton(Size(button_width,button_height));
    no->setTitleText(BUTTON_NO);
    layout->addChild(no);
    no->setAnchorPoint( uihelper::AnchorBottomLeft );
    uihelper::PositionParentBottomCenter(no, layout_padding, layout_padding);
    no->addTouchEventListener( [&](Ref* s,Widget::TouchEventType e) {
        OnNo();
    } );
    

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
        // eat the touch in the message area
        if ( layout->getBoundingBox().containsPoint(touch->getLocation()) )
            return true;
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


