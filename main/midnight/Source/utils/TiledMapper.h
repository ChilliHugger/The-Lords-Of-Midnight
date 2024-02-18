//
//  TiledMapper.h
//  midnight
//
//  Created by Chris Wild on 13/06/2019.
//  Copyright © 2019 Chilli Hugger Software. All rights reserved.
//

#ifndef TiledMapper_h
#define TiledMapper_h
#include "../cocos.h"

#include "../library/inc/mxtypes.h"

FORWARD_REFERENCE(mapbuilder);

class TiledMapper : public cocos2d::Ref
{
    using TMXTiledMap = cocos2d::TMXTiledMap;
    using TMXMapInfo = cocos2d::TMXMapInfo;

public:
    TiledMapper();
    virtual ~TiledMapper() override;

    TMXTiledMap* createTMXMap(const mapbuilder* builder);
    
protected:
    TMXMapInfo* tmxMapInfo;
};

#endif /* TiledMapper_h */
