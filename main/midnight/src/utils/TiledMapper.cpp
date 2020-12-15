//
//  TiledMapper.cpp
//  midnight
//
//  Created by Chris Wild on 13/06/2019.
//  Copyright © 2019 Chilli Hugger Software. All rights reserved.
//

#include "TiledMapper.h"

#include "../system/resolutionmanager.h"
#include "../extensions/TMXTiledMap.h"
#include "../utils/mapbuilder.h"

USING_NS_CC;
//USING_NS_CC_UI;


TMXTiledMap* TiledMapper::createTMXMap(const mapbuilder* builder)
{
    auto res = resolutionmanager::getInstance();
    
    std::string scenario = TME_ScenarioShortName();
    
    auto tmxMapInfo = new (std::nothrow) TMXMapInfo();
    
    // Map Setup
    tmxMapInfo->setMapSize( Size(builder->mapsize.cx,builder->mapsize.cy) );
    tmxMapInfo->setOrientation(TMXOrientationOrtho);
    
    tmxMapInfo->setTileSize(Size(64*res->GraphicsScale(),64*res->GraphicsScale()));
    
    // Tilesets
    auto t1 = new (std::nothrow) TMXTilesetInfo();
    t1->_name = "terrain";
    t1->_firstGid=1;
    t1->_tileSize = tmxMapInfo->getTileSize();
    t1->_originSourceImage = "map_tiles.png";
    t1->_sourceImage = scenario + "/" + res->Folder() +  "/terrain/" + t1->_originSourceImage;
    
    tmxMapInfo->getTilesets().pushBack(t1);
    t1->release();
    
    // Layers
    auto layer = new (std::nothrow) TMXLayerInfo();
    layer->_layerSize = tmxMapInfo->getMapSize();
    layer->_name = "Terrain";
    layer->_visible = true;
    layer->_opacity = ALPHA(1.00f);
    layer->_tiles = builder->terrain;
    tmxMapInfo->getLayers().pushBack(layer);
    layer->release();

    // Layers
    auto layer4 = new (std::nothrow) TMXLayerInfo();
    layer4->_layerSize = tmxMapInfo->getMapSize();
    layer4->_name = "Discovery Terrain";
    layer4->_visible = true;
    layer4->_opacity = ALPHA(0.25f);
    layer4->_tiles = builder->terrain_discovery;
    tmxMapInfo->getLayers().pushBack(layer4);
    layer4->release();
    
    // Layers
    auto layer2 = new (std::nothrow) TMXLayerInfo();
    layer2->_layerSize = tmxMapInfo->getMapSize();
    layer2->_name = "Critters";
    layer2->_visible = true;
    layer2->_opacity = ALPHA(1.00f);
    layer2->_tiles = builder->critters;
    tmxMapInfo->getLayers().pushBack(layer2);
    layer2->release();
    
    // Layers
    auto layer3 = new (std::nothrow) TMXLayerInfo();
    layer3->_layerSize = tmxMapInfo->getMapSize();
    layer3->_name = "Tunnels";
    layer3->_visible = true;
    layer3->_opacity = ALPHA(1.00f);
    layer3->_tiles = builder->tunnels;
    tmxMapInfo->getLayers().pushBack(layer3);
    layer3->release();
    
    return extensions::TMXTiledMap::create(tmxMapInfo);
}
