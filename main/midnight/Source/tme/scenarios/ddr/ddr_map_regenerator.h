#ifndef _DDR_MAP_REGENERATOR_H_INCLUDED_
#define _DDR_MAP_REGENERATOR_H_INCLUDED_

#include "../../baseinc/map_regenerator.h"

namespace tme {

    // Every night the things on the map are rebuilt: buildings always
    // get a thing, and a random terrain class gets things as well.
    class ddr_map_regenerator : public map_regenerator
    {
    public:
        virtual void initialise() override;
        virtual void process() override;
    };

}

#endif //_DDR_MAP_REGENERATOR_H_INCLUDED_
