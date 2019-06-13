
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
#include "../system/resolutionmanager.h"
#include "../ui/uihelper.h"

#include "../frontend/layout_id.h"

#include "../Extensions/TMXTiledMap.h"
#include "../Utils/mapbuilder.h"
#include "../Utils/TiledMapper.h"

#include "../ui/characters/uisinglelord.h"

USING_NS_CC;
USING_NS_CC_UI;
using namespace tme;

bool panel_map_detailed::init()
{
    if ( !uipanel::init() )
    {
        return false;
    }
    
    setBackground(_clrGrey);
    
    
    auto scrollView = ScrollView::create();
    scrollView->setContentSize(getContentSize());
    addChild(scrollView);
    
    // Command Window
    // Look Icon
    auto look = uihelper::CreateImageButton("i_look", ID_LOOK, clickCallback);
    uihelper::AddBottomLeft(this, look, RES(10), RES(10) );
  
    auto map = uihelper::CreateImageButton("i_big_map", ID_MAP_OVERVIEW, clickCallback);
    uihelper::AddBottomRight(this, map, RES(10), RES(10) );
    
    std::unique_ptr<mapbuilder> builder( new mapbuilder );
    std::unique_ptr<TiledMapper> mapper( new TiledMapper );
    auto tmxMap = mapper->createTMXMap(builder->build());
    
    scrollView->addChild(tmxMap);
    scrollView->setInnerContainerSize( tmxMap->getContentSize() );
    scrollView->setDirection(ScrollView::Direction::BOTH);
    scrollView->setInnerContainerPosition(Vec2::ZERO);
    
    character c;
    
    for( auto m : builder->objects ) {
        
        TME_GetCharacter(c,m->id);
        auto pos = builder->convertToPosition(c.location);
        auto lord = uisinglelord::createWithLord(c.id);
        lord->setAnchorPoint(uihelper::AnchorCenter);
        lord->setPosition( Vec2(pos.x+RES(32),tmxMap->getContentSize().height-(pos.y+RES(32))) );
        lord->addClickEventListener(clickCallback);
        lord->setTag((layoutid_t) (ID_SELECT_CHAR+c.id));
        lord->setUserData(c.userdata);

        scrollView->addChild(lord);

    }
    
    builder->critters = nullptr;
    builder->tunnels = nullptr;
    builder->terrain = nullptr;
    builder->terrain_discovery = nullptr;
    
    
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
