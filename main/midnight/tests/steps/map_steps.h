//
//  map_steps.h
//  midnight
//
//  Created by Chris Wild on 25/03/2023.
//

#pragma once

#include "tme_steps.h"


class MapStep {
public:
    static void ClearObjectFromLocation(loc_t loc);
    static void SetObjectAtLocation(loc_t loc, mxthing_t thing);
    static mxthing_t GetObjectAtLocation(loc_t loc);
    static void SetImpassable(loc_t loc);
    static void SetTerrain(loc_t loc, mxterrain_t terrain);
    static void ResetLocation(loc_t loc, mxterrain_t terrain = TN_PLAINS);

};
