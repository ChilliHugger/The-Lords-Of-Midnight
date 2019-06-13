
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

#include "../Extensions/TMXTiledMap.h"
#include "../Utils/mapbuilder.h"

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
    
    builder->build();
    
    auto path = FileUtils::getInstance()->fullPathForFilename("all");
    
    auto res = resolutionmanager::getInstance();
    std::string scenario = TME_ScenarioShortName();
    
    auto tmxMapInfo = new (std::nothrow) TMXMapInfo();
    
    // Map Setup
    tmxMapInfo->setMapSize( Size(builder->mapsize.cx,builder->mapsize.cy) );
    tmxMapInfo->setOrientation(TMXOrientationOrtho);
    tmxMapInfo->setTileSize(Size(RES(64),RES(64)));
    
    // Tilesets
    auto t1 = new (std::nothrow) TMXTilesetInfo();
    t1->_name = "terrain";
    t1->_firstGid=1;
    t1->_tileSize = tmxMapInfo->getTileSize();
    t1->_originSourceImage = "map_tiles.png";
    t1->_sourceImage = scenario + "/" + res->current_resolution.folder +  "/terrain/" + t1->_originSourceImage;

    tmxMapInfo->getTilesets().pushBack(t1);
    t1->release();
    
    // Layers
    auto layer = new (std::nothrow) TMXLayerInfo();
    layer->_layerSize = tmxMapInfo->getMapSize();
    layer->_name = "Terrain";
    layer->_visible = true;
    layer->_opacity = 255;
    layer->_tiles = builder->terrain;
    tmxMapInfo->getLayers().pushBack(layer);
    layer->release();

    // Layers
    auto layer2 = new (std::nothrow) TMXLayerInfo();
    layer2->_layerSize = tmxMapInfo->getMapSize();
    layer2->_name = "Critters";
    layer2->_visible = true;
    layer2->_opacity = 255;
    layer2->_tiles = builder->critters;
    tmxMapInfo->getLayers().pushBack(layer2);
    layer2->release();

    // Layers
    auto layer3 = new (std::nothrow) TMXLayerInfo();
    layer3->_layerSize = tmxMapInfo->getMapSize();
    layer3->_name = "Tunnels";
    layer3->_visible = true;
    layer3->_opacity = 255;
    layer3->_tiles = builder->tunnels;
    tmxMapInfo->getLayers().pushBack(layer3);
    layer3->release();
    
    auto tmxMap = extensions::TMXTiledMap::create(tmxMapInfo);
    scrollView->addChild(tmxMap);

    scrollView->setInnerContainerSize( tmxMap->getContentSize() );
    scrollView->setDirection(ScrollView::Direction::BOTH);
    
    
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
