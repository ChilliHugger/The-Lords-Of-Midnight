//
//  TiledMapper.h
//  midnight
//
//  Created by Chris Wild on 13/06/2019.
//  Copyright © 2019 Chilli Hugger Software. All rights reserved.
//

#ifndef TiledMapper_h
#define TiledMapper_h

#include "../library/libinc/mxtypes.h"

FORWARD_REFERENCE(mapbuilder);

class TiledMapper
{
    using TMXTiledMap = cocos2d::TMXTiledMap;
public:
    TMXTiledMap* createTMXMap(const mapbuilder* builder);
};

#endif /* TiledMapper_h */
