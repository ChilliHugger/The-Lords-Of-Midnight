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


		virtual scenarioinfo_t* GetInfoBlock() const;
		virtual MXRESULT Register ( mxengine* midnightx );
		virtual MXRESULT UnRegister ( mxengine* midnightx );	
		virtual mxentity* CreateEntity ( id_type_t type );

        virtual void NightStop(void);

        virtual void initialiseAfterCreate( void );
        virtual mxterrain_t NormaliseTerrain( mxterrain_t t) const;

        virtual void GiveGuidance(tme::mxcharacter *character, s32 hint);
        
	};

    
    class ddr_stronghold : public mxstronghold
    {
    public:
        ddr_stronghold();
        virtual ~ddr_stronghold();
        
        virtual void MakeChangeSides( mxrace_t newrace, mxcharacter* newoccupier );

        
    };
	
	//namespace ddr {
		
		// info classes
		class ddr_character  : public mxcharacter
		{
		public:
			ddr_character();
			virtual ~ddr_character();
			
            
			virtual BOOL CheckRecruitChar ( mxcharacter* character )  const ;
			virtual BOOL Recruited ( mxcharacter* character );
            virtual void Serialize ( archive& ar );
            virtual MXRESULT FillExportData ( info_t* data );

            
			virtual void DecreaseEnergy ( s32 amount );
            virtual void IncreaseEnergy ( s32 amount );
            
			virtual MXRESULT Cmd_WalkForward ( BOOL perform_seek );
            virtual MXRESULT Cmd_Use ( void );
            virtual mxobject* Cmd_Fight( void );
			            
            virtual void StartDawn ( void );
            virtual void Turn ( void );
            

			virtual void LostFight ( void );
            virtual mxlocinfo* GetLocInfo();
        
            virtual void CheckKilledFoe ( void );
            virtual void AICheckRecruitSoldiers ( void );
        
            void Target ( const mxitem* newtarget );
            void whatIsCharacterDoing ( void );
            BOOL retarget ();
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

        public:

            // not saved
            mxid            targetId;
            mxgridref       targetLocation;
            //
            
            mxobject*       desired_object;
            mxstronghold*   home_stronghold;
            ddr_character*  fighting_against;
            u32             battlelost;

            
            //
            
        };
		
	//}
	
}
#endif // _DDR_

#endif //_CDDRSCENARIO_H_INCLUDED_
