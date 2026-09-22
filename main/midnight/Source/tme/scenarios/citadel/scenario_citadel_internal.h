#ifndef _CITADELSCENARIO_INTERNAL_H_INCLUDED_
#define _CITADELSCENARIO_INTERNAL_H_INCLUDED_

#include "../../baseinc/tme_internal.h"
#include "../default/default_scenario_internal.h"

#if defined(_CITADEL_)

namespace tme {
    class citadel_x : public mxscenario
    {
    public:
        citadel_x();
        virtual ~citadel_x();
        
        virtual scenarioinfo_t* GetInfoBlock() const;
        virtual MXRESULT Register ( mxengine* midnightx );
        virtual MXRESULT UnRegister ( mxengine* midnightx );    

        virtual void initialise ( u32 version );
        virtual void initialiseAfterCreate ( u32 version );

        virtual mxcharacter* BadGuy() const override;
        virtual bool isTerrainImpassable ( mxterrain_t terrain, const mxitem* target ) const override;
        virtual u32 TerrainMovementModifier ( mxrace_t race, mxterrain_t terrain ) const override;

    public:
        // Boroth the Wolfheart, who holds the Citadel and whose host takes the keeps
        mxcharacter*    boroth;
    };

    class citadel_entityfactory : public mxentityfactory
    {
    public:
        virtual mxentity* Create ( id_type_t type ) override;
    };

    class citadel_stronghold : public mxstronghold
    {
    public:
        virtual bool IsEnemy() const override;
    };

    class citadel_character : public mxcharacter
    {
    public:
        virtual bool TakesPartInBattle() const override;
    };

    #define CITADEL_SCENARIO(x) static_cast<citadel_x*>(mx->scenario)->x
}
#endif

#endif //_CITADELSCENARIO_INTERNAL_H_INCLUDED_

