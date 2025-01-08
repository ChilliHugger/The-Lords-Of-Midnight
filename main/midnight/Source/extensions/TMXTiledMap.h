//
//  TMXTiledMap.h
//  midnight
//
//  Created by Chris Wild on 09/06/2019.
//  Copyright © 2019 Chilli Hugger Software. All rights reserved.
//

#pragma once
#include "../cocos.h"

namespace extensions {
    
    class TMXTiledMap : public cocos2d::TMXTiledMap
    {
        using TMXMapInfo = cocos2d::TMXMapInfo;
    public:
        
        static TMXTiledMap * create(TMXMapInfo* mapinfo);
        bool initWithMapInfo(TMXMapInfo* mapinfo);
        
    };

}

