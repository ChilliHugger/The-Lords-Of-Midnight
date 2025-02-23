
#ifndef _MXSCENARIO_H_INCLUDED_
#define _MXSCENARIO_H_INCLUDED_

namespace tme {

    class mxengine;
    class mxentity;
    class mxgridref ;
    class mxinterface;

    //namespace item {
        class mxitem;
        class mxlocinfo ;
        class mxcharacter ;
        class mxobject ;
    //};

    class mxentityfactory {
    public:
        virtual mxentity* Create ( id_type_t type );
    };



        // ALIAS default_scenario
        class mxscenario 
        {
        public:
            mxscenario();
            virtual ~mxscenario();
            
            virtual scenarioinfo_t* GetInfoBlock() const;
            virtual MXRESULT Command ( const std::string& arg, variant argv[], u32 argc );
            virtual MXRESULT GetProperties ( const std::string& arg, variant argv[], u32 argc );
            virtual MXRESULT Text ( const std::string& command, variant* argv=NULL, u32 args=0 );
            virtual MXRESULT Register ( mxengine* midnightx ) ;
            virtual MXRESULT UnRegister ( mxengine* midnightx );

            virtual void initialise ( u32 version );
            virtual void initialiseAfterCreate ( u32 version );
            virtual void updateAfterLoad ( u32 version );
     
            virtual void Serialize ( chilli::lib::archive& ar ) ;

            virtual void NightStart(void);
            virtual void NightStop(void);
            
            virtual u32 CalcFearAdjuster(mxlocinfo* info) const ;
            virtual u32 CalcStrongholdAdjuster(void) const;
            virtual void MakeMapAreaVisible ( mxgridref l, mxcharacter* character );
            virtual void MakeMapTunnelAreaVisible( mxgridref l );

            virtual void GetDefaultCharacters ( c_character* collection );
            virtual bool CanWeSelectCharacter ( const mxcharacter* character );
                        
            virtual mxcharacter* CurrentMoonringWearer( void );
            virtual mxcharacter* MoonringWearer( void ) ;
            virtual void CheckMoonringWearerDead ( void );
            
            virtual mxcharacter* WhoHasObject( mxobject* object ) const ;
            virtual mxobject* PickupObject ( mxgridref loc );
            virtual void DeadCharactersDropObjects( void );
            virtual bool DropObject ( mxgridref loc, mxobject* object );
            virtual mxobject* FindObjectAtLocation ( mxgridref loc );
            
            virtual mxgridref FindLookingTowards( mxgridref loc, mxdir_t dir );

            virtual void ResetMapArmies ( void );
            virtual void SetMapArmies ( void );
            virtual void RemoveMapArmies ( void );
            virtual u32 FindArmiesAtLocation ( mxgridref loc, u32& enemies, u32& friends, flags32_t flags );
            
            virtual void LookInDirection( mxgridref loc, mxdir_t dir, bool isintunnel );
            virtual void SetCharsLooking( void );

            virtual bool InCharactersMemory ( mxitem* item ) ;
            virtual void ClearFromMemory ( mxgridref loc );

            
            virtual bool GetCharactersAvailableForCommand ( u32 mode, mxgridref loc, c_character* collection );
            virtual u32 FindCharactersAtLocation ( mxgridref loc, c_character* characters, flags32_t flags );
            virtual c_character GetCharacterFollowers ( mxcharacter* leader );
            
            virtual s32 BaseDoomdarkSuccess ( mxrace_t race, mxunit_t unit, const mxlocinfo& locinfo );
            
            virtual void GiveGuidance( mxcharacter* character, s32 hint );
            
            virtual mxterrain_t toGeneralisedTerrain( mxterrain_t t) const ;
            virtual mxterrain_t toScenarioTerrain( mxterrain_t t) const ;

            virtual MXRESULT GetLocInfo ( mxid id, flags32_t flags, std::unique_ptr<mxlocinfo>& info);

            bool IsFeature(u32 flag) const { return (features&flag) == flag; }
            
            bool isTerrainImpassable(mxterrain_t terrain, mxitem* target) const;
            bool isLocationImpassable(mxgridref loc, mxitem* target) const;
            
        public:
            flags32_t           features;
            mxobject*           moonring;
            mxcharacter*        doomdark;
            mxcharacter*        luxor;
            mxcharacter*        dreams;
        };
    
        #define DEF_SCENARIO(x) (mx->scenario)->x
            
}// namespace tme


#endif //_MXSCENARIO_H_INCLUDED_

