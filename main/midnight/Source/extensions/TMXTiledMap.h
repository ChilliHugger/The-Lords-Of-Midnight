//
//  TMXTiledMap.h
//  midnight
//
//  Created by Chris Wild on 09/06/2019.
//  Copyright © 2019 Chilli Hugger Software. All rights reserved.
//

#pragma once
#include "../axmol_sdk.h"

namespace extensions {
    
    class TMXTiledMap : public ax::TMXTiledMap
    {
        using TMXMapInfo = ax::TMXMapInfo;
    public:
        
        static TMXTiledMap * create(TMXMapInfo* mapinfo);
        bool initWithMapInfo(TMXMapInfo* mapinfo);
        
    };

}

