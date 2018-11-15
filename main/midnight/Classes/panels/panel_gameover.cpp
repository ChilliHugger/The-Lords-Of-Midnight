
/*
 *  panel_gameover.cpp
 *  midnight
 *
 *  Created by Chris Wild on 10/10/2018.
 *  Copyright (c) 2018 Chilli Hugger Software. All rights reserved.
 *
 */

#include "panel_gameover.h"

#include "../system/moonring.h"
//#include "../system/configmanager.h"
#include "../system/resolutionmanager.h"
//#include "../system/panelmanager.h"
#include "../ui/uihelper.h"
#include "../frontend/layout_id.h"

#include "../tme_interface.h"

USING_NS_CC;
using namespace tme;
using namespace cocos2d::ui;

bool panel_gameover::init()
{
    if ( !uipanel::init() )
    {
        return false;
    }
    
    setBackground(_clrBlack);
    
    // Look Icon
    auto home = uihelper::CreateImageButton("i_home", ID_HOME, clickCallback);
    uihelper::AddBottomLeft(safeArea, home, RES(10), RES(10) );
    
    return true;
}

void panel_gameover::OnShown()
{
    auto size = safeArea->getContentSize();
//    setContentSize(size);
    
    // Description
    auto lblDescription = Label::createWithTTF( uihelper::font_config_big, TME_LastActionMsg() );
    lblDescription->getFontAtlas()->setAntiAliasTexParameters();
    lblDescription->setLocalZOrder(ZORDER_DEFAULT);
    lblDescription->setWidth(size.width-RES(64));
    lblDescription->setAlignment(TextHAlignment::LEFT);
    lblDescription->setLineSpacing(0);
    lblDescription->setMaxLineWidth(size.width-RES(64));
    lblDescription->enableWrap(true);
    lblDescription->setAnchorPoint(uihelper::AnchorTopLeft);
    lblDescription->setPosition(RES(32), size.height);
    lblDescription->setHorizontalAlignment(TextHAlignment::CENTER);
    safeArea->addChild(lblDescription);

    if ( currentmode == MODE_WIN ) {
        setBackgroundToWidth( "screens/misc/win.png" );
        lblDescription->enableOutline(Color4B(_clrRed,ALPHA(0.75f)),RES(2));
        lblDescription->setTextColor(Color4B(_clrWhite));
        
    } else {
        setBackgroundToWidth( "screens/misc/lose.png" );
        lblDescription->enableOutline(Color4B(_clrWhite,ALPHA(0.75f)),RES(2));
        lblDescription->setTextColor(Color4B(_clrDarkRed));
    }
    
}

void panel_gameover::OnNotification( Ref* sender )
{
    auto button = static_cast<Button*>(sender);
    if ( button == nullptr )
        return;
    
    layoutid_t id = static_cast<layoutid_t>(button->getTag());
    
    switch ( id  )
    {
        case ID_HOME:
            mr->showPage(MODE_MAINMENU);
            break;
            
        default:
            break;
    }
}
