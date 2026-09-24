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
        
        virtual scenarioinfo_t* GetInfoBlock() const override;
        virtual MXRESULT Register ( mxengine* midnightx ) override;
        virtual MXRESULT UnRegister ( mxengine* midnightx ) override;

        virtual void initialise ( u32 version ) override;
        virtual void initialiseAfterCreate ( u32 version ) override;

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

    class citadel_object : public mxobject
    {
    public:
        citadel_object();

        virtual void Serialize ( archive& ar ) override;
        virtual void LoadTsv ( const TsvRow& row ) override;

    public:
        mxobjtype_t     type;
        mxobjpower_t    power;
    };

    class citadel_stronghold : public mxstronghold
    {
    public:
        virtual bool IsEnemy() const override;
        virtual bool CanCharacterRecruitOrPost ( const mxcharacter* character ) const override;
    };

    class citadel_character : public mxcharacter
    {
    public:
        virtual bool TakesPartInBattle() const override;
        virtual u32  FightStrength() const override;
        virtual bool ShouldDieInFight() const override;
        virtual void InitNightProcessing ( void ) override;
        virtual bool CheckRecruitChar ( mxcharacter* pChar ) const override;
        virtual bool IsAllowedWarriors() const override;
        virtual bool IsAllowedRiders() const override;
        mxobjpower_t WeaponPower() const;
    };

    #define CITADEL_SCENARIO(x) static_cast<citadel_x*>(mx->scenario)->x
}
#endif

#endif //_CITADELSCENARIO_INTERNAL_H_INCLUDED_

