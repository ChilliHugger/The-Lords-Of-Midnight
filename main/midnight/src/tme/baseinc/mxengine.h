#ifndef _CMIDNIGHTX_H_INCLUDED_
#define _CMIDNIGHTX_H_INCLUDED_

#include "info.h"
#include "../baseinc/scenario.h"

//#define _TME_DEMO_MODE_

#define ISARG(x)    c_stricmp( arg.c_str(), x ) == 0
#define COMMAND(x)  static MXRESULT (x)( const std::string& arg, variant argv[], u32 argc)
#define mxrandom    randomno::instance.get
#define mxchance    randomno::instance.chance
#define EOS(x)      x+c_strlen(x)


namespace tme {
    
    //using namespace tme::collections;
    
    DECLARE_ENUM(arguments) {
        character   =    128+IDT_CHARACTER,
        stronghold  =    128+IDT_STRONGHOLD,
        routenode   =    128+IDT_ROUTENODE,
        regiment    =    128+IDT_REGIMENT,
        objectinfo  =    128+IDT_OBJECT,
        location    =    128+IDT_LOCATION,
        direction   =    128+IDT_DIRECTIONINFO,
        area        =    128+IDT_AREAINFO,
        terrain     =    128+IDT_TERRAININFO,
        string      =    128+IDT_STRING,
    END_ENUM(arguments);

    typedef struct mxcommand_t {
        LPCSTR      name;
        u32         argcount;
        PFNCOMMAND  pfnCommand;
        u32         arguments[16];
    } mxcommand_t;

    using collections::entities;
    using collections::infos;
        
    class mxengine
    {
    public:
        mxengine();
        virtual ~mxengine();

        virtual MXRESULT SetDatabaseDirectory ( const std::string& directory ) ;
        virtual MXRESULT LoadDatabase ( RULEFLAGS rules, mxdifficulty_t difficulty ) ;
        virtual MXRESULT UnloadDatabase ( void ) ;
        //virtual MXRESULT LoadDefaultScenario ( void ) ;
        virtual MXRESULT LoadScenario ( mxscenario* scenario ) ;
        virtual MXRESULT UnloadScenario () ;
        virtual MXRESULT SaveGame ( const std::string& filename, PFNSERIALIZE function ) ;
        virtual MXRESULT LoadGame ( const std::string& filename, PFNSERIALIZE function ) ;
        virtual MXRESULT SaveGameDescription ( const std::string& filename, std::string& description );

        virtual MXRESULT ProcessCommand ( mxcommand_t tblCommand[], u32 max, const std::string& arg, variant argv[], u32 argc )  ;

        virtual void NightCallback( callback_t* )  ;

        std::string LastActionMsg() ;
        void SetLastActionMsg(const std::string& text) ;
  
        virtual void Error ( u32 errorcode )  ;
        virtual u32 Error () const   ;
        
        //
        MXRESULT LoadDiscoveryMap ( const std::string& filename );
        MXRESULT SaveDiscoveryMap ( const std::string& filename );
        
        static void debug (LPCSTR format, ... );
        
        // tme::item
        mxroutenode*    RouteNodeById(u32 id) ;
        mxcharacter*    CharacterById(u32 id) ;
        mxstronghold*   StrongholdById(u32 id) ;
        mxregiment*     RegimentById(u32 id) ;
        mxplace*        PlaceById(u32 id) ;
        mxobject*       ObjectById(u32 id) ;
        mxobject*       ObjectFromThing(mxthing_t thing) ;
        mxvictory*      VictoryById(u32 id) ;
        mxmission*      MissionById(u32 id) ;

        // tme::info
        mxdirection*    DirectionById(u32 id) ;
        mxunitinfo*     UnitById(u32 id) ;
        mxrace*         RaceById(u32 id) ;
        mxgender*       GenderById(u32 id) ;
        mxterrain*      TerrainById(u32 id) ;
        mxarea*         AreaById(u32 id) ;
        mxcommand*      CommandById(u32 id) ;
#if defined(_DDR_)
        mxobjectpower*  ObjectPowerById(u32 id);
        mxobjecttype*   ObjectTypeById(u32 id);
#endif
        // current char
        void CurrentChar ( mxcharacter* character);
        mxcharacter* CurrentChar ( void ) const;
        
        u32 CollectRegiments ( mxgridref loc, entities& collection ) ;
        u32 CollectStrongholds ( mxgridref loc, entities& collection ) ;
        u32 CollectRoutenodes ( mxgridref loc, entities& collection ) ;

        cvarreg_t* FindDBVariable ( const std::string& name ) const;
        mxentity* EntityByName( const std::string& name, id_type_t type=IDT_NONE );
        mxentity* EntityByIdt( mxid id );
        LPCSTR EntitySymbolById ( mxid id );
        MXRESULT EntityLinkData( mxid id, const void* data );

        u32 SaveGameVersion() const { return savegameversion; }
        bool isSavedGame() const { return m_savegame; }
        bool isDatabase() const { return !isSavedGame(); }
        
        void setRules( RULEFLAGS flags) { m_ruleFlags.Set(flags); }
        bool isRuleEnabled( RULEFLAGS flags ) { return m_ruleFlags.Is(flags); }
        
        void Difficulty( mxdifficulty_t difficulty ) { m_difficulty = difficulty; }
        mxdifficulty_t Difficulty() const { return m_difficulty ; }
        
    public:
        mxmap*                  gamemap;
        mxdiscoverymap*         discoverymap;
        mxtext*                 text;
        mxnight*                night;
        mxbattle*               battle;
        mxscenario*             scenario;
        
        entities                objCharacters;
        entities                objRegiments;
        entities                objRoutenodes;
        entities                objStrongholds;
        entities                objPlaces;
        entities                objMissions;
        entities                objVictories;
        entities                objObjects;
        infos                   objDirectionInfos;
        infos                   objUnitInfos;
        infos                   objRaceInfos;
        infos                   objGenderInfos;
        infos                   objTerrainInfos;
        infos                   objAreaInfos;
        infos                   objCommandInfos;
#if defined(_DDR_)
        infos                   objObjectPowersInfos;
        infos                   objObjectTypesInfos;
#endif
        PFNNIGHTCALLBACK        pfnNightCallback ;

        bool                    m_savegame;
        mxobject*               thing_remap_table[MAX_THINGS];

    private:
        cvarreg_t*              variables ;
        bool                    defaultscenario;
        u32                     m_errorcode;
        mxcharacter*            m_CurrentCharacter;
        std::string             m_szDatabase;
        std::string             lastactiontext;
        
        u32                     savegameversion;
        eflags<RULEFLAGS,u64>   m_ruleFlags;
        mxdifficulty_t          m_difficulty;
    };

    inline std::string mxengine::LastActionMsg()
        { return lastactiontext; }

    inline void mxengine::SetLastActionMsg(const std::string& text)
        { lastactiontext = text; }

    extern mxengine* mx;


}
// namespace tme


    
#ifdef MX_DEBUG
#define MX_DEBUG_ON 1
#define MXTRACE        mxengine::debug
#else
#define MXTRACE        if(0) printf
#endif
  
#define USING_NS_TME    using namespace tme


#endif //_CMIDNIGHTX_H_INCLUDED_
    
