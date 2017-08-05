
#ifndef _MXSCENARIO_H_INCLUDED_
#define _MXSCENARIO_H_INCLUDED_

#include "../../baseinc/tme_internal.h"

namespace tme {

	class mxengine;
	class mxentity;
	class mxgridref ;
	class mxinterface;

	namespace collections {
		class entities ;
	}

	//namespace item {
		class mxitem;
		class mxlocinfo ;
		class mxcharacter ;
		class mxobject ;
	//};

		// ALIAS default_scenario
		class mxscenario 
		{
		public:
			mxscenario();
			virtual ~mxscenario();
			
			virtual scenarioinfo_t* GetInfoBlock() const;
			virtual MXRESULT Command ( const string& arg, variant argv[], u32 argc );
			virtual MXRESULT GetProperties ( const string& arg, variant argv[], u32 argc );
			virtual MXRESULT Text ( const string& command, variant* argv=NULL, u32 args=0 );
			virtual MXRESULT Register ( mxengine* midnightx ) ;
			virtual MXRESULT UnRegister ( mxengine* midnightx );

			virtual void initialiseAfterCreate ( void );
            virtual mxentity* CreateEntity ( id_type_t type );
			virtual void Serialize ( chilli::lib::archive& ar ) ;

			virtual void NightStart(void);
			virtual void NightStop(void);
			virtual u32 CalcFearAdjuster(mxlocinfo* info) const ;
			virtual u32 CalcStrongholdAdjuster(void) const;
			virtual void MakeMapAreaVisible ( mxgridref l, mxcharacter* character );
			virtual void GetDefaultCharacters ( entities& collection );
			virtual BOOL CanWeSelectCharacter ( const mxcharacter* character );
						
            virtual mxcharacter* CurrentMoonringWearer( void );
			virtual mxcharacter* MoonringWearer( void ) ;
			virtual mxcharacter* WhoHasObject( mxobject* object ) const ;
			virtual mxobject* PickupObject ( mxgridref loc );
			virtual void DeadCharactersDropObjects( void );
			virtual mxgridref FindLookingTowards( mxgridref loc, mxdir_t dir );
			virtual void ResetMapArmies ( void );
			virtual void SetMapArmies ( void );
            
#if defined(_DDR_)
            virtual void PlaceObjectsOnMap ( void );
            virtual mxobject* FindObjectAtLocation ( mxgridref loc );
            virtual mxstronghold* StrongholdFromLocation ( mxgridref loc );
            virtual mxterrain_t NormaliseTerrain( mxterrain_t t) const;

#endif
			virtual void RemoveMapArmies ( void );
			virtual void LookInDirection( mxgridref loc, mxdir_t dir, BOOL isintunnel );
			virtual void SetCharsLooking( void );
			virtual BOOL DropObject ( mxgridref loc, mxobject* object );
			virtual BOOL InCharactersMemory ( mxitem* item ) ;
			virtual void ClearFromMemory ( mxgridref loc );

            virtual void CheckMoonringWearerDead ( void );
            
			virtual BOOL GetCharactersAvailableForCommand ( u32 mode, mxgridref loc, entities& collection );
			virtual u32 FindArmiesAtLocation ( mxgridref loc, u32& enemies, u32& friends, flags32_t flags );
			virtual u32 FindCharactersAtLocation ( mxgridref loc, entities& characters, flags32_t flags );

            virtual void GetCharacterFollowers ( mxcharacter* leader, collections::entities& collection );
            
            
            virtual void GiveGuidance( mxcharacter* character, s32 hint );

            
            
			virtual MXRESULT GetLocInfo ( mxid id, mxlocinfo*&	info  );

			BOOL IsFeature(u32 flag) const { return (features&flag); }
			
		public:
			flags32_t	features;

		};
	
	
}// namespace tme


#endif //_MXSCENARIO_H_INCLUDED_

