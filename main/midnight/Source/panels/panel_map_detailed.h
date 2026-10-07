/*
 *  panel_map_detailed.h
 *  midnight
 *
 *  Created by Chris Wild on 10/10/2018.
 *  Copyright (c) 2018 Chilli Hugger Software. All rights reserved.
 *
 */

#pragma once
#include "../axmol_sdk.h"
#include "../extensions/ScrollView.h"
#include "../ui/uipanel.h"
#include "../ui/uifilterbutton.h"
#include "../models/mapmodel.h"

#if !defined(_MOUSE_ENABLED_)
#include <unordered_map>
#endif


FORWARD_REFERENCE(mapbuilder);
FORWARD_REFERENCE(uigrouplord);
FORWARD_REFERENCE(map_object);

constexpr f32 MAP_SCALE_MIN = 0.5f;
constexpr f32 MAP_SCALE_MAX = 2.0f;
constexpr f32 MAP_SCALE_CLICK_DELTA = 0.1f;

// pixels of shift+mouse movement for one full unit of map scale (desktop)
constexpr f32 MAP_SCALE_MOUSE_SENSITIVITY = 400.0f;

class panel_map_detailed : public uipanel
{
    using ScrollView = extensions::ScrollView;
    using TMXTiledMap = ax::TMXTiledMap;
    using Node = ax::Node;
    using DrawNode = ax::DrawNode;
    using Vec2 = ax::Vec2;
    using Label = ax::Label;
    using Widget = ax::ui::Widget;
    
public:
    virtual bool init() override;
    virtual ~panel_map_detailed() override;
    
    CREATE_FUNC(panel_map_detailed);
    
protected:
    panel_map_detailed();

    void OnNotification( Ref* sender ) override;

#if defined(_MOUSE_ENABLED_)
    bool OnMouseMove( Vec2 pos ) override;
#else
    void addPinchZoomListener();
#endif

    void centreOnCharacter( character& c, bool animate );
    void centreOnCurrentCharacter(bool animate);

    uifilterbutton* createFilterButton( layoutid_t id, s32 y, const std::string& image, map_filters flag);
    
    void updateFilterButton(Ref* sender,map_filters flag);

    void updateFilters();

    void setupTooltip();
    void setupCharacterButtons();
    void setupPlaceLabels();
    void setupStrongholds();
    
    void hideGroupLord();
    void showGroupLord( Widget* button );
    
    void updateScale();
    
    void addTouchListener();
    tme::mxgridref gridAt( const Vec2& location ) const;

#if defined(_CITADEL_)
    void pickQuestPlace( tme::mxgridref place );
#endif

private:
    
    ScrollView*     scrollView;
    TMXTiledMap*    tmxMap;
    mapbuilder*     mapBuilder;
    mapmodel*       model;
    f32             minMapScale;
    Node*           characters;
    Node*           descriptions;
    DrawNode*       groupLordBackground;
    Widget*         groupLordButton;
    uigrouplord*    grouplord;
    Label*          toolTip;

#if defined(_MOUSE_ENABLED_)
    bool    shiftZooming;
    Vec2    shiftZoomLastPos;
#else
    bool    pinchActive;
    f32     pinchLastDistance;
    std::unordered_map<int,Vec2>   pinchTouches;
#endif

};
