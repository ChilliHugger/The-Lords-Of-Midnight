
/*
 *  panel_map_detailed.cpp
 *  midnight
 *
 *  Created by Chris Wild on 10/10/2018.
 *  Copyright (c) 2018 Chilli Hugger Software. All rights reserved.
 *
 */

#include "panel_map_detailed.h"

#include "../system/moonring.h"
//#include "../system/configmanager.h"
#include "../system/resolutionmanager.h"
//#include "../system/panelmanager.h"
#include "../ui/uihelper.h"

#include "../frontend/layout_id.h"

USING_NS_CC;
using namespace tme;
using namespace cocos2d::ui;

bool panel_map_detailed::init()
{
    if ( !uipanel::init() )
    {
        return false;
    }
    
    setBackground(_clrGrey);
    
    
    // Command Window
    // Look Icon
    auto look = uihelper::CreateImageButton("i_look", ID_LOOK, clickCallback);
    uihelper::AddBottomLeft(this, look, RES(10), RES(10) );
  
    auto map = uihelper::CreateImageButton("i_big_map", ID_MAP_OVERVIEW, clickCallback);
    uihelper::AddBottomRight(this, map, RES(10), RES(10) );
    
    return true;
}


void panel_map_detailed::OnNotification( Ref* sender )
{
    auto button = static_cast<Button*>(sender);
    if ( button == nullptr )
        return;
    
    layoutid_t id = static_cast<layoutid_t>(button->getTag());
    
    switch ( id  )
    {
        case ID_LOOK:
            mr->look();
            break;
            
        case ID_MAP_OVERVIEW:
            mr->showPage(MODE_MAP_OVERVIEW);
            break;
        
        default:
            break;
    }
}
