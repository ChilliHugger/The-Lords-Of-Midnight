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
#include "../utils/imagebuilder.h"
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

    // the eye is part of the button's image: show the open or the closed one
    moonring::mikesingleton()->imagebuilder->setEnabled(this, "visible", selected);
    moonring::mikesingleton()->imagebuilder->setEnabled(this, "notvisible", !selected);
    
    dispatchSelectChangedEvent(isSelected());
    
}

uifilterbutton* uifilterbutton::createWithImage(const std::string& image)
{    
    auto button = uifilterbutton::create();

    // image is the id of a filter in images (see filters.xml)
    auto builder = moonring::mikesingleton()->imagebuilder;
    auto def = builder->find(image);
    if ( def == nullptr || !builder->setupButton(button, *def) )
        UIDEBUG("uifilterbutton: no filter image '%s'", image.c_str());

    button->setLocalZOrder(ZORDER_UI);    
    button->initChildren();
    
    uihelper::setEnabled(button,true);

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
