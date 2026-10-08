#include "../../baseinc/tme_internal.h"
#include "ddr_map_regenerator.h"

#if defined(_DDR_)

namespace tme {

    namespace {

        mxthing_t location_objects[16][4] = {
            { OB_DRAGONS,   OB_SKULKRIN,    OB_WOLVES,      OB_WILDHORSES },    //For    Plains
            { OB_DRAGONS,   OB_DRAGONS,     OB_ICETROLLS,   OB_WOLVES },        //       Mountins
            { OB_DRAGONS,   OB_SKULKRIN,    OB_WOLVES,      OB_WOLVES },        //       Forest
            { OB_DRAGONS,   OB_ICETROLLS,   OB_WOLVES,      OB_WOLVES },        //       Hills

            { OB_DRAGONS,   OB_CLAWS,       OB_FLAMES,      OB_BLOOD },         //       Gate
            { OB_FLAMES,    OB_THORNS,      OB_BLOOD,       OB_LANGUOR },       //       Temple
            { OB_ICETROLLS, OB_WOLVES,      OB_THORNS,      OB_SPRINGS },       //       Pit
            { OB_DRAGONS,   OB_WILDHORSES,  OB_CLAWS,       OB_BLOOD },         //       Palace
            { OB_DRAGONS,   OB_WOLVES,      OB_SHELTER,     OB_SHELTER },       //       Fortress
            { OB_ICETROLLS, OB_SHELTER,     OB_SHELTER,     OB_BLOOD },         //       Hall
            { OB_SKULKRIN,  OB_WOLVES,      OB_SHELTER,     OB_SHELTER },       //       Hut
            { OB_GUIDANCE,  OB_GUIDANCE,    OB_GUIDANCE,    OB_GUIDANCE },      //       Tower
            { OB_DRAGONS,   OB_SHELTER,     OB_SHELTER,     OB_SPRINGS },       //       City
            { OB_SKULKRIN,  OB_SPRINGS,     OB_SPRINGS,     OB_SPRINGS },       //       Fountain
            { OB_DRAGONS,   OB_FLAMES,      OB_FLAMES,      OB_LANGUOR },       //       Stones

            { OB_DRAGONS,   OB_SKULKRIN,    OB_ICETROLLS,   OB_ICETROLLS }      //       Wastes
        };

    }

    void ddr_map_regenerator::initialise()
    {
    }

    void ddr_map_regenerator::process()
    {
        auto map = mx->gamemap;

        for ( auto [loc, mapsqr] : map->Locations() ) {
            u32 r = mxrandom(3);
            int key = LocationKey(loc) & 3;

            // all building refresh their thing status
            // every night

            // plains, mountains, forest, hills all reset randomly

            // TODO: should be governed by bit flag on TERRAIN
            mxterrain_t t = (mxterrain_t)mapsqr.terrain ;

            // remap ddr/lom
            t = mx->scenario->toScenarioTerrain(t);

            if ( (t >= TN_GATE || (u32)(t-TN_PLAINS2) == r) && t != TN_ICYWASTE ) {
                mapsqr.flags |= lf_creature ;

                // if it is a tunnel passageway
                // then use the same types
                if ( mapsqr.IsTunnelObject() )
                    t = TN_ICYWASTE ;

                mapsqr.object = location_objects[t-TN_PLAINS2][key];

            }else{
                mapsqr.flags &= ~lf_creature ;
                mapsqr.object = OB_NONE ;
            }
        }
    }

}

#endif
