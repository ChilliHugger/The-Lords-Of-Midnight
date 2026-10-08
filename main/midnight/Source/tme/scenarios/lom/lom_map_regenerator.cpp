#include "../../baseinc/tme_internal.h"
#include "lom_map_regenerator.h"

#if defined(_LOM_)

namespace tme {

    // built from the things on the original map: the chance of each thing
    // is how often it is found on that terrain
    lom_map_regenerator::lom_map_regenerator()
    {
        const std::pair<mxterrain_t, respawn_list_t> table[] = {
            { TN_PLAINS,    { { OB_WOLVES, 54 },        { OB_WILDHORSES, 46 } } },
            { TN_CITADEL,   { { OB_SHELTER, 63 },       { OB_WOLFSLAYER, 31 },      { OB_DRAGONSLAYER, 6 } } },
            { TN_FOREST,    { { OB_WOLVES, 60 },        { OB_SKULKRIN, 40 } } },
            { TN_HENGE,     { { OB_CUPOFDREAMS, 38 },   { OB_GUIDANCE, 25 },        { OB_WATERSOFLIFE, 13 },
                              { OB_SHADOWSOFDEATH, 12 }, { OB_HANDOFDARK, 12 } } },
            { TN_TOWER,     { { OB_GUIDANCE, 100 } } },
            { TN_VILLAGE,   { { OB_SHELTER, 100 } } },
            { TN_DOWNS,     { { OB_DRAGONS, 28 },       { OB_SKULKRIN, 23 },        { OB_WILDHORSES, 19 },
                              { OB_WOLVES, 16 },        { OB_ICETROLLS, 14 } } },
            { TN_KEEP,      { { OB_SHELTER, 94 },       { OB_DRAGONSLAYER, 4 },     { OB_WOLFSLAYER, 2 } } },
            { TN_SNOWHALL,  { { OB_SHELTER, 56 },       { OB_HANDOFDARK, 24 },      { OB_CUPOFDREAMS, 12 },
                              { OB_WOLFSLAYER, 8 } } },
            { TN_LAKE,      { { OB_WATERSOFLIFE, 100 } } },
            { TN_RUIN,      { { OB_WOLFSLAYER, 31 },    { OB_DRAGONSLAYER, 26 },    { OB_SHELTER, 24 },
                              { OB_SHADOWSOFDEATH, 10 },{ OB_HANDOFDARK, 9 } } },
            { TN_LITH,      { { OB_CUPOFDREAMS, 31 },   { OB_WOLFSLAYER, 20 },      { OB_DRAGONSLAYER, 20 },
                              { OB_GUIDANCE, 19 },      { OB_WATERSOFLIFE, 10 } } },
            { TN_CAVERN,    { { OB_SHELTER, 25 },       { OB_HANDOFDARK, 24 },      { OB_ICETROLLS, 21 },
                              { OB_DRAGONS, 18 },       { OB_WOLFSLAYER, 9 },       { OB_DRAGONSLAYER, 3 } } },
            { TN_MOUNTAIN,  { { OB_DRAGONS, 37 },       { OB_SKULKRIN, 34 },        { OB_ICETROLLS, 29 } } },
        };

        for ( const auto& row : table )
            SetRespawnList( row.first, row.second );
    }

    void lom_map_regenerator::SetRespawnList( mxterrain_t terrain, const respawn_list_t& list )
    {
        m_respawn[terrain] = list;
    }

    // 0-99, either fixed by the location or random
    u32 lom_map_regenerator::Roll( mxgridref loc ) const
    {
        if ( mx->isRuleEnabled(RF_LOM_REPLENISH_RANDOM) )
            return (u32)mxrandom(99);

        return (u32)(LocationKey(loc) % 100);
    }

    u32 lom_map_regenerator::RespawnDays( mxdifficulty_t difficulty ) const
    {
        switch ( difficulty ) {
            case DF_EASY:   return 2;
            case DF_HARD:   return 10;
            case DF_MEDIUM:
            default:        return 5;
        }
    }

    bool lom_map_regenerator::CanRespawn( mxthing_t thing ) const
    {
        auto object = mx->ObjectById( thing );
        return object != nullptr && object->CanRespawn();
    }

    // walk the terrain's list until the roll lands on a thing,
    // a list that adds up to less than 100% sometimes gives nothing
    mxthing_t lom_map_regenerator::RespawnObject( mxterrain_t terrain, mxgridref loc ) const
    {
        auto row = m_respawn.find( terrain );
        if ( row == m_respawn.end() )
            return OB_NONE;

        u32 roll = Roll( loc );
        u32 total = 0;

        for ( const auto& entry : row->second ) {
            total += entry.chance;
            if ( roll < total )
                return entry.object;
        }

        return OB_NONE;
    }

    void lom_map_regenerator::initialise()
    {
        if ( !mx->isRuleEnabled(RF_LOM_REPLENISH_THINGS) )
            return;

        auto map = mx->gamemap;

        for ( auto [loc, mapsqr] : map->Locations() ) {
            auto tinfo = mx->TerrainById( mapsqr.terrain );
            CONTINUE_IF(tinfo->IsBlock());

            if ( CanRespawn((mxthing_t)mapsqr.object) )
                mapsqr.flags |= lf_respawn;
        }
    }

    // Empty locations flagged lf_respawn get something back every
    // RespawnDays() days. The x/y offset staggers the locations so they
    // do not all replenish on the same day.
    void lom_map_regenerator::process()
    {
        if ( !mx->isRuleEnabled(RF_LOM_REPLENISH_THINGS) )
            return;

        auto map = mx->gamemap;
        const auto size = map->Size();
        const u32 period = RespawnDays( mx->Difficulty() );
        const u32 day = (u32)sv_days;

        for ( auto [loc, mapsqr] : map->Locations() ) {
            CONTINUE_IF ( !(mapsqr.flags & lf_respawn) || mapsqr.object != OB_NONE );

            CONTINUE_IF ( (day + (u32)(loc.x + loc.y*size.cx)) % period != 0 );

            mapsqr.object = RespawnObject( (mxterrain_t)mapsqr.terrain, loc );
        }
    }

}

#endif
