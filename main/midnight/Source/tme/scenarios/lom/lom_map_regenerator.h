#ifndef _LOM_MAP_REGENERATOR_H_INCLUDED_
#define _LOM_MAP_REGENERATOR_H_INCLUDED_

#include <map>
#include <vector>
#include "../../baseinc/map_regenerator.h"

namespace tme {

    class lom_map_regenerator : public map_regenerator
    {
    public:
        struct respawn_chance_t {
            mxthing_t   object;
            u32         chance;     // percent
        };

        typedef std::vector<respawn_chance_t> respawn_list_t;

        lom_map_regenerator();

        virtual void initialise() override;
        virtual void process() override;

        virtual u32 RespawnDays( mxdifficulty_t difficulty ) const;
        virtual mxthing_t RespawnObject( mxterrain_t terrain, mxgridref loc ) const;
        virtual bool CanRespawn( mxthing_t thing ) const;
        virtual void SetRespawnList( mxterrain_t terrain, const respawn_list_t& list );

    protected:
        virtual u32 Roll( mxgridref loc ) const;

    protected:
        std::map<mxterrain_t, respawn_list_t> m_respawn;
    };

}

#endif //_LOM_MAP_REGENERATOR_H_INCLUDED_
