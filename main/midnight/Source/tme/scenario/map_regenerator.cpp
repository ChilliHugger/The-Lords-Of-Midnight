#include "../baseinc/tme_internal.h"

namespace tme {

    int map_regenerator::LocationKey( mxgridref loc ) const
    {
        // the map has a one location border all round it
        const int width = mx->gamemap->Size().cx - 2;
        const int x = loc.x - 1;
        const int y = loc.y - 1;
        return (444*((y*width)+x))%6151;
    }

}
