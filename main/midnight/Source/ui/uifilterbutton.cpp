//
//  uifilterbutton.cpp
//  midnight
//
//  Created by Chris Wild on 28/10/2018.
//  Copyright © 2018 Chilli Hugger Software. All rights reserved.
//

#include "../axmol_sdk.h"
#include "uifilterbutton.h"
#include "uihelper.h"
#include "../system/moonring.h"
#include "../system/resolutionmanager.h"

USING_NS_AX;
USING_NS_AX_UI;


uifilterbutton::uifilterbutton() :
      _checkBoxEventCallback(nullptr)
    , selected(false)
{
}

bool uifilterbutton::isSelected()
{
    return selected;
}

void uifilterbutton::setSelected(bool selected)
{
    this->selected = selected;
    auto visible = getChildByName<Sprite*>("visible");
    auto notvisible = getChildByName<Sprite*>("notvisible");
    visible->setVisible(selected);
    notvisible->setVisible(!selected);
    
    dispatchSelectChangedEvent(isSelected());
    
}

uifilterbutton* uifilterbutton::createWithImage(const std::string& image)
{    
    auto button = uifilterbutton::create();
    
    if ( Director::getInstance()->getTextureCache()->getTextureForKey(image) != nullptr ) {
        button->loadTextures(image,"","", ax::ui::Widget::TextureResType::LOCAL);
    }else{
        button->loadTextures(image,"","", ax::ui::Widget::TextureResType::PLIST);
    }
    button->setLocalZOrder(ZORDER_UI);    
    button->initChildren();
    
    uihelper::setEnabled(button,true);

    button->onFocusChanged = [button](Widget* widgetLostFocus, Widget* widgetGetFocus) {
        
        if (widgetGetFocus == button) {
            button->setScale(1.2f);
            button->setFocused(true);
            button->setColor(_clrGreen);
        }
        if (widgetLostFocus == button) {
            button->setScale(scale_normal);
            button->setFocused(false);
            button->setColor(_clrWhite);
        }
    };

    return button;
}

bool uifilterbutton::init()
{
    if ( !Button::init() )
    {
        return false;
    }
    return true;
}

void uifilterbutton::initChildren()
{
    // main image
    auto r = getBoundingBox();
    
    // eye visible
    // "i_visible"
    auto visible = Sprite::createWithSpriteFrameName("i_visible");
    visible->setLocalZOrder(ZORDER_DEFAULT);
    visible->setName("visible");
    visible->setAnchorPoint(uihelper::AnchorCenter);
    visible->setPosition(r.size.width-RES(10), r.size.height-RES(10) );
    addChild(visible);
    
    // eye not visible
    // "i_not_visible"
    auto notvisible = Sprite::createWithSpriteFrameName("i_not_visible");
    notvisible->setLocalZOrder(ZORDER_DEFAULT);
    notvisible->setName("notvisible");
    notvisible->setAnchorPoint(uihelper::AnchorCenter);
    notvisible->setPosition(r.size.width-RES(10), r.size.height-RES(10) );
    addChild(notvisible);
    
    setSelected(false);
    setLocalZOrder(ZORDER_UI);
    
    chilli::ui::WidgetClickCallback callback = [&] (Ref* ref ) {
        
        setSelected( !isSelected() );
        
    };
    
    addClickEventListener(callback);

}

constexpr int CHECKBOX_STATE_EVENT_SELECTED          = 1;
constexpr int CHECKBOX_STATE_EVENT_UNSELECTED        = 0;

void uifilterbutton::dispatchSelectChangedEvent(bool selected)
{
    int eventType = (selected ? CHECKBOX_STATE_EVENT_SELECTED : CHECKBOX_STATE_EVENT_UNSELECTED);
   
    this->retain();
    if (_checkBoxEventCallback)
    {
        _checkBoxEventCallback(this, eventType);
    }
    
    this->release();
    
}

void uifilterbutton::addEventListener( WidgetEventCallback callback )
{
    _checkBoxEventCallback = callback;
}
