//
//  map_steps.cpp
//  midnight
//
//  Created by Chris Wild on 25/03/2023.
//

#include "tme_steps.h"
#include "map_steps.h"

void MapStep::ClearObjectFromLocation(loc_t loc)
{
    tme::mx->gamemap->GetAt(loc).object = OB_NONE;
}
