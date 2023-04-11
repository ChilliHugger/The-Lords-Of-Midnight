#ifndef _DDRSCENARIO_INTERNAL_H_INCLUDED_
#define _DDRSCENARIO_INTERNAL_H_INCLUDED_

#include "../../baseinc/tme_internal.h"
#include "../default/default_scenario_internal.h"

#if defined(_DDR_)
namespace tme {


    class ddr_x  : public mxscenario
    {
    public:
        ddr_x();
        virtual ~ddr_x();


        virtual scenarioinfo_t* GetInfoBlock() const override;
        virtual MXRESULT Register ( mxengine* midnightx ) override;
        virtual MXRESULT UnRegister ( mxengine* midnightx ) override;
        virtual mxentity* CreateEntity ( id_type_t type ) override;

        virtual void NightStop(void) override;

        virtual void initialiseAfterCreate( u32 version ) override;
     
        virtual void GiveGuidance(mxcharacter *character, s32 hint) override;
        
        virtual void MakeMapAreaVisible ( mxgridref l, mxcharacter* character ) override;

        virtual mxcharacter* WhoHasObject( mxobject* object ) const override;
        virtual bool DropObject ( mxgridref loc, mxobject* obj ) override;
        virtual mxobject* PickupObject ( mxgridref loc ) override;

        virtual void PlaceObjectsOnMap ( void );
        virtual mxobject* FindObjectAtLocation ( mxgridref loc );
        virtual mxstronghold* StrongholdFromLocation ( mxgridref loc );
        virtual mxterrain_t toScenarioTerrain( mxterrain_t t) const override;
    };

    
    class ddr_object : public mxobject
    {
    public:
        ddr_object();
        virtual ~ddr_object();
        
        virtual MXRESULT FillExportData ( info_t* data );
        virtual void Serialize ( archive& ar );

        FLAG_PROPERTY( CanHelpRecruitment,        of_recruitment )
        FLAG_PROPERTY( IsRandomStart,    of_randomstart )
    
        bool IsSpecial() const;
        
    public:
        mxobjtype_t   type;
        mxobjpower_t  power;
    };
    
    class ddr_stronghold : public mxstronghold
    {
    public:
        ddr_stronghold();
        virtual ~ddr_stronghold();
        virtual MXRESULT FillExportData ( info_t* data ) override;
        virtual void Serialize ( archive& ar ) override;
        virtual void MakeChangeSides( mxrace_t newrace, mxcharacter* newoccupier ) override;
        virtual void OnRespawn();
        virtual mxrace_t Loyalty() override ;
        PROPERTY ( u32, Energy, energy )
    protected:
        u32 energy;
    };
    
    class ddr_character  : public mxcharacter
    {
    public:
        ddr_character();
        virtual ~ddr_character();
        
        virtual bool CheckRecruitChar ( mxcharacter* character )  const ;
        virtual bool Recruited ( mxcharacter* recruiter );
        virtual void Serialize ( archive& ar );
        virtual MXRESULT FillExportData ( info_t* data );

        virtual void DecreaseEnergy ( s32 amount );
        virtual void IncreaseEnergy ( s32 amount );
        
        virtual MXRESULT Cmd_WalkForward ( bool perform_seek );
        virtual mxobject* Cmd_Fight( void );
        virtual mxcharacter* Cmd_Approach ( mxcharacter* character );
        
        virtual MXRESULT Cmd_EnterTunnel ( void );
        virtual MXRESULT Cmd_ExitTunnel ( void );
        virtual MXRESULT Cmd_Use ( void ) ;
        virtual MXRESULT Cmd_Take ( void ) ;
        virtual MXRESULT Cmd_Give ( mxcharacter* character ) ;
        virtual mxcharacter* GetNextFoe() const;
        virtual mxcharacter* GetNextLiege() const;
        
        virtual void StartDawn ( void );
        virtual void Turn ( void );
        
        virtual void LostFight();
        virtual void Displace();
        virtual mxlocinfo* GetLocInfo();
        virtual MXRESULT EnterBattle ();
        
        virtual void CheckKilledFoe ( void );
        virtual void AICheckRecruitSoldiers ( void );
        
        GET_PROPERTY ( bool, IsCarryingDesiredObject, IsCarryingObject() && carrying == desired_object )
    
        void Target ( const mxitem* newtarget );
        void whatIsCharacterDoing ( void );
        bool retarget ();
        void moveCharacterSomewhere ( void );

        void IncreaseDespondency( s32 amount );
        void DecreaseDespondency( s32 amount );
        
        mxunit_t getArmyType() const ;
        mxunit* getUnit();
        s32 getArmySize();
        void setArmySize(s32 value);
        void setArmyLost(s32 value);
        void setArmyKilled(s32 value);
        
        mxorders_t pickNewOrders (void) const;
        mxcharacter* AI_Approach ( mxcharacter* character );

    protected:
        bool RecruitMorkin ( mxcharacter* recruiter );

        virtual void UseCrownOfPersuassion();
        virtual void UseSpellOfSwiftness();
        virtual void UseRunesOfProtection();
        virtual void UseAllOtherObjects();


    public:

        mxgridref       lastlocation;
        
        // not saved
        mxid            targetId;
        mxgridref       targetLocation;
        //
        
        mxobject*       desired_object;
        mxstronghold*   home_stronghold;
        ddr_character*  fighting_against;
        u32             battlelost;
    };
    

    
}
#endif // _DDR_

#endif //_CDDRSCENARIO_H_INCLUDED_
