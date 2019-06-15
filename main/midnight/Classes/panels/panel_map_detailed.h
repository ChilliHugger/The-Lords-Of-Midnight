/*
 *  panel_map_detailed.h
 *  midnight
 *
 *  Created by Chris Wild on 10/10/2018.
 *  Copyright (c) 2018 Chilli Hugger Software. All rights reserved.
 *
 */

#pragma once
#include "../cocos.h"
#include "../Extensions/ScrollView.h"
#include "../ui/uipanel.h"
#include "../ui/uifilterbutton.h"
#include "../models/mapmodel.h"


FORWARD_REFERENCE(mapbuilder);

class panel_map_detailed : public uipanel
{
    using ScrollView = extensions::ScrollView;
    using TMXTiledMap = cocos2d::TMXTiledMap;
    using Node = cocos2d::Node;
    
public:
    virtual bool init() override;
    
    CREATE_FUNC(panel_map_detailed);
    
protected:
    panel_map_detailed();

    void OnNotification( Ref* sender ) override;

    void centreOnCharacter( character& c, bool animate );
    void centreOnCurrentCharacter(bool animate);

    uifilterbutton* createFilterButton( layoutid_t id, s32 y, const std::string& image, map_filters flag);
    
    void updateFilterButton(Ref* sender,map_filters flag);

    void updateFilters();
    
private:
    
    ScrollView*     scrollView;
    TMXTiledMap*    tmxMap;
    mapbuilder*     mapBuilder;
    mapmodel*       model;
    Node*           characters;
    
};
