//
//  TMXTiledMap.cpp
//  midnight
//
//  Created by Chris Wild on 09/06/2019.
//  Copyright © 2019 Chilli Hugger Software. All rights reserved.
//

#include "TMXTiledMap.h"

USING_NS_AX;

namespace extensions {
    
    TMXTiledMap * TMXTiledMap::create(TMXMapInfo* mapinfo)
    {
        TMXTiledMap *ret = new (std::nothrow) TMXTiledMap();
        if (ret->initWithMapInfo(mapinfo))
        {
            ret->autorelease();
            return ret;
        }
        AX_SAFE_DELETE(ret);
        return nullptr;
    }
    
    
    bool TMXTiledMap::initWithMapInfo(TMXMapInfo* mapInfo)
    {
        AXASSERT(mapInfo != nullptr, "TMXTiledMap: tmx file should not be empty");
        
        setContentSize(Size::ZERO);
        
        if (mapInfo == nullptr)
            return false;
        
        AXASSERT( !mapInfo->getTilesets().empty(), "TMXTiledMap: Map not found. Please check the filename.");
        
        buildWithMapInfo(mapInfo, true);
        
        return true;
    }

    
}
