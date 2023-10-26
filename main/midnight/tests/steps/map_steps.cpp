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

void MapStep::SetObjectAtLocation(loc_t loc, mxthing_t thing)
{
    tme::mx->gamemap->GetAt(loc).object = thing;
}

mxthing_t MapStep::GetObjectAtLocation(loc_t loc)
{
    return (mxthing_t)tme::mx->gamemap->GetAt(loc).object;
}
