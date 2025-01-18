//
//  mapmodel.h
//  midnight
//
//  Created by Chris Wild on 03/05/2019.
//  Copyright © 2019 Chilli Hugger Software. All rights reserved.
//

#pragma once
#include "panelmodel.h"

#include "../tme/tme_interface.h"
#include "../frontend/panel_id.h"


enum class map_filters : u32 {
    show_critters   = MXBIT(0),
    centre_char     = MXBIT(1),
    overview_map    = MXBIT(2),
    show_lords      = MXBIT(4),
#if defined(_TUNNELS_)
    show_tunnels    = MXBIT(3),
#endif
    all             = show_critters
                        | centre_char
                        | show_lords
                        | overview_map
#if defined(_TUNNELS_)
                        | show_tunnels
#endif
};

constexpr float INITIAL_MAP_SCALE = 1.0f;

class mapmodel : public panelmodel
{
public:
    void serialize( u32 version, lib::archive& ar ) override;
    virtual void setDefaults() override;
    
public:

    // game specific
    eflags<map_filters,u32> filters;
    point   oldoffset;
    f32     mapscale;
    f32     lastmapscale;
    
    // TODO: Current Map
    // 
    panelmode_t currentMapPanel;
};
