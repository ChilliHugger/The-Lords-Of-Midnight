/*
 * FILE:    default_scenario.cpp
 * 
 * PROJECT: MidnightEngine
 * 
 * CREATED: 
 * 
 * AUTHOR:  Chris Wild
 * 
 * Copyright 2011 Chilli Hugger. All rights reserved.
 * 
 * PURPOSE: 
 * 
 * 
 */

#include "../../baseinc/tme_internal.h"
#include "../../utils/savegamemapping.h"
#if defined (_DDR_)
#include "../../scenarios/ddr/scenario_ddr_internal.h"
#endif

#include <string>

using namespace chilli;
using namespace chilli::lib;
using namespace chilli::collections;

namespace tme {

    mxentity* CreateEntity(mxscenario* scenario, id_type_t type)
    {
        return mx->entityfactory->Create(type);
    }

#define CONVERT_COLLECTION(x,y) \
    c_mxid& y = (c_mxid&) *((c_mxid*)x.vPtr)

#define CONVERT_GRIDREF(x,y) \
    mxgridref y = mxgridref( GET_LOCIDX((mxid)x),GET_LOCIDY((mxid)x) );

    //namespace scenarios {

    scenarioinfo_t    scenario_info = {
        1,
        100,
        "Default Scenario",
        "Chris Wild",
        "rorthron@thelordsofmidnight.com",
        "http://www.thelordsofmidnight.com",
        "Default scenario for the Midnight Engine",
        "Copyright 1984 - 2017 Mike Singleton & Chris Wild"
    };

    mxscenario* def_scenario = nullptr ;

    
    MXRESULT MXAPI default_scenario::Create ( mxinterface* engine )
    {
        variant args;
        
        if ( engine == nullptr )
            return MX_FAILED ;
        
        def_scenario = new mxscenario ;
        
        if ( def_scenario == nullptr )
            return MX_FAILED ;
        
        args = def_scenario ;
        return engine->Command("@SETSCENARIO", &args, 1);
        
    }
            
    scenarioinfo_t* default_scenario::GetInfoBlock() const
    {
        return def_scenario->GetInfoBlock();
    }
    
    
    MXRESULT default_scenario::Register ( mxengine* midnightx )
    {
        return def_scenario->Register(midnightx);
    }
    
    MXRESULT default_scenario::UnRegister ( mxengine* midnightx )
    {
        return def_scenario->UnRegister(midnightx);
    }
    
    MXRESULT default_scenario::Command ( const std::string& arg, variant argv[], u32 argc )
    {
        return def_scenario->Command(arg, argv, argc);
    }
    
    MXRESULT default_scenario::GetProperties ( const std::string& arg, variant argv[], u32 argc )
    {
        return def_scenario->GetProperties(arg, argv, argc);
    }
    
    MXRESULT default_scenario::Text ( const std::string& command, variant* argv, u32 args )
    {
        return def_scenario->Text(command, argv, args);
    }
    
    
    
    
    
    
    
    
    
    
        MXRESULT mxscenario::Register ( mxengine* midnightx )
        {
            // mx = midnightx ;
            // add in the interfaces
            mx->text = new mxtext;
            mx->night = new mxnight;
            mx->battle = new mxbattle;
            mx->entityfactory = new mxentityfactory;
            mx->scenario = this;
            return MX_OK ;
        }

        MXRESULT mxscenario::UnRegister ( mxengine* midnightx )
        {
            SAFEDELETE ( mx->text ) ;
            SAFEDELETE ( mx->night ) ;
            SAFEDELETE ( mx->battle ) ;
            SAFEDELETE ( mx->entityfactory );
            //mx->scenario = this;
            //default_scenario=nullptr ;
            return MX_OK ;
        }
         
    
    
        mxscenario::mxscenario() :
            luxor(nullptr),
            doomdark(nullptr),
            moonring(nullptr)
        {
        }

        mxscenario::~mxscenario()
        {
        }

        /*
        * Function name    : base::GetInfoBlock
        * 
        * Return type        : scenarioinfo_t*
        * 
        * Arguments        : 
        * 
        * Description        : 
        * 
        */

        scenarioinfo_t* mxscenario::GetInfoBlock() const
        {
            return &scenario_info ;
        }


        mxentity* mxentityfactory::Create ( id_type_t type )
        {
            switch ( type ) {
                case IDT_ARMY:
                    return (mxentity*)new mxarmy ;
                case IDT_CHARACTER:
                    return (mxentity*)new mxcharacter ;
                case IDT_LOCATIONINFO:
                    return (mxentity*)new mxlocinfo ;
                case IDT_REGIMENT:
                    return (mxentity*)new mxregiment ;
                case IDT_ROUTENODE:
                    return (mxentity*)new mxroutenode ;
                case IDT_STRONGHOLD:
                    return (mxentity*)new mxstronghold ;
                case IDT_PLACE:
                    return (mxentity*)new mxplace ;
                case IDT_OBJECT:
                    return (mxentity*)new mxobject ;
                case IDT_VICTORY:
                    return (mxentity*)new mxvictory ;
                case IDT_MISSION:
                    return (mxentity*)new mxmission ;

                case IDT_DIRECTIONINFO:
                    return (mxentity*)new mxdirection ;
                case IDT_UNITINFO:
                    return (mxentity*)new mxunitinfo ;
                case IDT_RACEINFO:
                    return (mxentity*)new mxrace ;
                case IDT_GENDERINFO:
                    return (mxentity*)new mxgender ;
                case IDT_TERRAININFO:
                    return (mxentity*)new mxterrain ;
                case IDT_AREAINFO:
                    return (mxentity*)new mxarea ;
                case IDT_COMMANDINFO:
                    return (mxentity*)new mxcommand ;
                default:
                    break;
            }
            return nullptr;
        }

        void mxscenario::NightStart(void)
        {
        }

        void mxscenario::NightStop(void)
        {
        }

        mxterrain_t mxscenario::toGeneralisedTerrain(mxterrain_t t) const
        {
            switch (t) {
                case TN_PLAINS2:
                case TN_PLAINS3:
                case TN_LAND:
                case TN_PLAIN:
                    return TN_PLAINS;
                    
                case TN_FOREST2:
                case TN_FOREST3:
                case TN_TREES:
                    return TN_FOREST;
                    
                case TN_MOUNTAIN2:
                case TN_MOUNTAIN3:
                case TN_ICY_MOUNTAIN:
                    return TN_MOUNTAIN;
                    
                case TN_WATCHTOWER:
                    return TN_TOWER;
                    
                case TN_ICYWASTE:
                    return TN_FROZENWASTE;
                    
                case TN_LAKE3:
                    return TN_LAKE;
                    
                case TN_HILLS3:
                case TN_DOWNS:
                case TN_FOOTHILLS:
                    return TN_HILLS;
                    
                case TN_STONES:
                    return TN_LITH;
                    
                default:
                    return t;
            }
    
        }

        mxterrain_t mxscenario::toScenarioTerrain(mxterrain_t t) const
        {
            return t;
        }

        bool mxscenario::isLocationImpassable(mxgridref loc, mxitem* target) const
        {
            auto mapLoc = mx->gamemap->GetAt(loc);
            return isTerrainImpassable((mxterrain_t)mapLoc.terrain, target);
        }

        //
        // This adds in Mountains as a potential impassable terrain above and beyond
        // and setup in the database. This is controlled by two game rules
        // RF_AI_IMPASSABLE_MOUNTAINS and RF_IMPASSABLE_MOUNTAINS
        // DWARVES, GIANTS, and DRAGONS can all pass impassable mountains
        // and when RF_SOLE_MOUNTAINEER is enabled so can lords without armies.
        //
        bool mxscenario::isTerrainImpassable(mxterrain_t terrain, mxitem* target) const
        {
            if(target!=nullptr) {
                bool isAI = true;
                mxrace_t race = RA_NONE;
                bool army = true;
                
                if(target->IsType(IDT_CHARACTER)) {
                    auto character = dynamic_cast<mxcharacter*>(target);
                    if (character != nullptr) {
                        if(character->IsInTunnel()) {
                            return false;
                        }
                        isAI = character->IsAIControlled();
                        race = character->Race();
                        if(mx->isRuleEnabled(RF_SOLE_MOUNTAINEER) && !character->HasArmy() ) {
                            army = false;
                        }
                    }
                }
                else if(target->IsType(IDT_REGIMENT)) {
                    auto regiment = dynamic_cast<mxregiment*>(target);
                    if (regiment != nullptr) {
                        race = regiment->Race();
                    }
                }
            
                if( race != RA_DWARF && race != RA_GIANT && race != RA_DRAGON && army ) {
                    if( toGeneralisedTerrain(terrain) == TN_MOUNTAIN ) {
                        RULEFLAGS rule = isAI ? RF_AI_IMPASSABLE_MOUNTAINS : RF_IMPASSABLE_MOUNTAINS ;
                        if ( mx->isRuleEnabled(rule) ) {
                            return true;
                        }
                    }
                }
            }
            return mx->TerrainById(terrain)->IsBlock();
        }


        /*
        * Function name    : base::Serialize
        * 
        * Return type        : void
        * 
        * Arguments        : archive& ar
        * 
        * Description        : 
        * 
        */

        void mxscenario::Serialize ( archive& ar )
        {
        }

        /*
        * Function name    : base::CalcFearAdjuster
        * 
        * Return type        : void
        * 
        * Arguments        : mxCLocInfo* info
        * 
        * Description        : 
        * 
        */

        u32 mxscenario::CalcFearAdjuster(mxlocinfo* locinfo) const
        {
            return 0 ;
        }

        /*
        * Function name    : base::Calitem::strongholdAdjuster
        * 
        * Return type        : int
        * 
        * Arguments        : mxCLocInfo* locinfo
        * 
        * Description        : 
        * 
        */

        u32 mxscenario::CalcStrongholdAdjuster(void) const
        {
        u32 adj_stronghold = 0;
            FOR_EACH_STRONGHOLD(stronghold) {
                if ( stronghold->OccupyingRace() == RA_DOOMGUARD ) {
                    adj_stronghold += stronghold->Influence() ;
                }
            }
            return adj_stronghold ;
        }

        c_character mxscenario::GetCharacterFollowers ( mxcharacter* leader )
        {
            c_character collection;

            FOR_EACH_CHARACTER(character) {
                if ( character->following == leader )
                    collection += character ;
            }
            
            return collection;
        }
    
        void mxscenario::GetDefaultCharacters ( c_character* collection )
        {
            RETURN_IF_NULL(collection);

            collection->Clear();
            for ( u32 ii=0; ii<sv_character_default.Count(); ii++ )
                collection->Add( static_cast<mxcharacter*>(mx->EntityByIdt(sv_character_default[ii]))
                );
        }

        bool mxscenario::CanWeSelectCharacter ( const mxcharacter* character )
        {
            // TODO shouldn't we be checking if they are recruited here?
            if ( IsFeature(SF_MOONRING) ) {
                //if ( ob_moonring == NULL ) return true ;
                //const mxcharacter* moonringcarrier = WhoHasObject(moonring);
                
                const mxcharacter* moonringcarrier = CurrentMoonringWearer();
                if ( moonringcarrier==nullptr && !character->IsAllowedMoonring() )
                    return false;
            }
            return true ;
        }
    

    

        static s32 modes[4][2] = {
            {  1,  1 },
            { -1,  1 },
            {  1, -1 },
            { -1, -1 },
        };

        static s32 adjust[21][2] = {

            { -2, -4 }, // 1
            { -1, -4 }, // 2
            {  0, -4 }, // 3

            { -3, -3 }, // 4
            { -2, -3 }, // 5
            { -1, -3 }, // 6
            {  0, -3 }, // 7

            { -4, -2 }, // 8
            { -3, -2 }, // 9
            { -2, -2 }, // 10
            { -1, -2 }, // 11
            {  0, -2 }, // 12

            { -4, -1 }, // 13
            { -3, -1 }, // 14
            { -2, -1 }, // 15
            { -1, -1 }, // 16
            {  0, -1 }, // 17

            { -4,  0 }, // 18
            { -3,  0 }, // 19
            { -2,  0 }, // 20
            { -1,  0 }, // 21
        };

        static u32 steps[21][6] = {
            { 17, 16, 12, 11,  6,  5 },    // 1
            { 17, 12, 11,  7,  6,  0 },    // 2
            { 17, 12,  7,  0,  0,  0 },    // 3
            { 16, 10,  0,  0,  0,  0 },    // 4
            { 17, 16, 11, 10,  0,  0 },    // 5
            { 17, 16, 12, 11,  0,  0 },    // 6
            { 17, 12,  0,  0,  0,  0 },    // 7
            { 21, 16, 15, 14,  9,  0 },    // 8
            { 21, 16, 15, 10,  0,  0 },    // 9
            { 16,  0,  0,  0,  0,  0 },    // 10
            { 16, 17,  0,  0,  0,  0 },    // 11
            { 17,  0,  0,  0,  0,  0 },    // 12
            { 21, 16, 20, 15, 14,  0 },    // 13
            { 21, 16, 20, 15,  0,  0 },    // 14
            { 21, 16,  0,  0,  0,  0 },    // 15
            {  0,  0,  0,  0,  0,  0 },    // 16
            {  0,  0,  0,  0,  0,  0 },    // 17
            { 21, 20, 19,  0,  0,  0 }, // 18
            { 21, 20,  0,  0,  0,  0 },    // 19
            { 21,  0,  0,  0,  0,  0 },    // 20
            {  0,  0,  0,  0,  0,  0 },    // 21
        };

        static u32 affect[21][6] = {
            {  2,  2,  2,  2,  2,  3 },    // 1
            {  2,  2,  3,  3,  3,  0 },    // 2
            {  1,  1,  1,  0,  0,  0 },    // 3
            {  1,  1,  0,  0,  0,  0 },    // 4
            {  3,  2,  2,  3,  0,  0 },    // 5
            {  2,  2,  3,  3,  0,  0 },    // 6
            {  1,  1,  0,  0,  0,  0 },    // 7
            {  3,  2,  3,  3,  3,  0 },    // 8
            {  3,  2,  2,  3,  0,  0 },    // 9
            {  1,  0,  0,  0,  0,  0 },    // 10
            {  2,  2,  0,  0,  0,  0 },    // 11
            {  1,  0,  0,  0,  0,  0 },    // 12
            {  2,  3,  2,  3,  3,  0 },    // 13
            {  2,  2,  2,  2,  0,  0 },    // 14
            {  2,  2,  0,  0,  0,  0 },    // 15
            {  0,  0,  0,  0,  0,  0 },    // 16
            {  0,  0,  0,  0,  0,  0 },    // 17
            {  1,  1,  1,  0,  0,  0 }, // 18
            {  1,  1,  0,  0,  0,  0 },    // 19
            {  1,  0,  0,  0,  0,  0 },    // 20
            {  0,  0,  0,  0,  0,  0 },    // 21
        };


        void mxscenario::MakeMapAreaVisible ( mxgridref l, mxcharacter* character )
        {
        mxgridref dst;
        mxgridref newdst;
        mxterrain*    tinfo;

            mx->gamemap->SetLocationVisible(l, true);
            
            character->CanSeeLocation( l );

            for ( u32 i=0; i<NUMELE(modes); i++ ) {
                
                int dx = modes[i][0];
                int dy = modes[i][1];

                for ( u32 j=0; j<NUMELE(steps); j++ ) {

                    int blocking=0;

                    dst.x = l.x + ( adjust[j][0] * dx ) ;
                    dst.y = l.y + ( adjust[j][1] * dy ) ;

                    if ( !mx->gamemap->IsLocOnMap(dst) ) 
                        continue;

                    mxloc& lookingat = mx->gamemap->GetAt( dst );

                    tinfo = mx->TerrainById(lookingat.terrain);
                    int visibility = tinfo->Visibility();

                    u32 k=0;
                    for ( k=0; k<NUMELE(steps[0]); k++ ) {
                    
                        int d = steps[j][k] ;
                        if ( d == 0 )
                            break;

                        newdst.x = l.x + ( adjust[d-1][0] * dx ) ;
                        newdst.y = l.y + ( adjust[d-1][1] * dy ) ;

                        if ( mx->gamemap->IsLocOnMap(newdst) )  {
                        mxloc& current = mx->gamemap->GetAt( newdst );
                            mxterrain* tinfo = mx->TerrainById(current.terrain);
                            blocking += tinfo->Obstruction()/affect[j][k];
                            if ( blocking>=visibility ) break;
                        }
                    }

                    if ( blocking<visibility ) {
                        lookingat.flags|=lf_seen;
                        mx->gamemap->CheckVisibleRange(dst);
                        // k==0 when the location dst - is adjacent to loc
                        // at that point you will be able to see the army
                        if ( k==0 || tinfo->IsArmyVisible() )
                            character->CanSeeLocation( dst );
                        
                    }
                }
            }
        }

        COMMAND( OnCharLookLeft ) 
        {
            CONVERT_CHARACTER_ID( argv[0].vId, character );
            argv[0]=(s32)0;
            return character->Cmd_LookLeft();
        }

        COMMAND( OnCharLookRight ) 
        {
            CONVERT_CHARACTER_ID( argv[0].vId, character );
            argv[0]=(s32)0;
            return character->Cmd_LookRight();
        }

        COMMAND( OnCharLook ) 
        {
            CONVERT_CHARACTER_ID( argv[0].vId, character );
            argv[0]=(s32)0;
            return character->Cmd_LookDir( (mxdir_t) GET_ID( argv[1].vId ) );
        }

        COMMAND( OnCharForward ) 
        {
            CONVERT_CHARACTER_ID( argv[0].vId, character );
            argv[0]=(s32)0;
            return character->Cmd_MoveForward();
        }

        COMMAND( OnCharPostMen ) 
        {
            CONVERT_CHARACTER_ID( argv[0].vId, character );
            CONVERT_STRONGHOLD_ID( argv[1].vId, stronghold );
            argv[0]=(s32)0;
            return character->Cmd_PostMen( stronghold, 0 );
        }

        COMMAND( OnCharRecruitMen ) 
        {
            CONVERT_CHARACTER_ID( argv[0].vId, character );
            CONVERT_STRONGHOLD_ID( argv[1].vId, stronghold );
            argv[0]=(s32)0;
            return character->Cmd_RecruitMen( stronghold, 0 );
        }

        COMMAND( OnCharAttack ) 
        {
            CONVERT_CHARACTER_ID( argv[0].vId, character );
            argv[0]=(s32)0;
            return character->Cmd_Attack();
        }

        COMMAND( OnCharRest ) 
        {
            CONVERT_CHARACTER_ID( argv[0].vId, character );
            argv[0]=(s32)0;
            return character->Cmd_Rest();
        }

        COMMAND( OnCharHide ) 
        {
            CONVERT_CHARACTER_ID( argv[0].vId, character );
            argv[0]=(s32)0;
            return character->Cmd_Hide();
        }

        COMMAND( OnCharUnHide ) 
        {
            CONVERT_CHARACTER_ID( argv[0].vId, character );
            argv[0]=(s32)0;
            return character->Cmd_UnHide();
        }

        COMMAND( OnCharLookAt ) 
        {
            CONVERT_CHARACTER_ID( argv[0].vId, character );
            CONVERT_GRIDREF(argv[1],loc);
            argv[0]=(s32)0;
            return character->Cmd_Lookat(loc);
        }

        COMMAND( OnCharPlace )
        {
            CONVERT_CHARACTER_ID( argv[0].vId, character );
            CONVERT_GRIDREF(argv[1],loc);
            argv[0]=(s32)0;
            return character->Cmd_Place(loc);
        }
    
        COMMAND( OnCharApproach ) 
        {
            CONVERT_CHARACTER_ID( argv[0].vId, character );
            CONVERT_CHARACTER_ID( argv[1].vId, recruit );
            mxcharacter* recruited = character->Cmd_Approach(recruit);
            if ( recruited ) {
                argv[1] = mxentity::SafeIdt(recruited);
                argv[0] = (s32)1;
                return MX_OK ;
            }
            argv[0]=(s32)0;
            return MX_FAILED ;
        }

        COMMAND( OnCharFollow ) 
        {
            CONVERT_CHARACTER_ID( argv[0].vId, character );
            CONVERT_CHARACTER_ID( argv[1].vId, c );

            if ( character->Cmd_Follow(c) ) {
                argv[0] = (s32)0;
                return MX_OK ;
            }
            argv[0]=(s32)0;
            return MX_FAILED ;
        }

        COMMAND( OnCharDisbandGroup )
        {
            CONVERT_CHARACTER_ID( argv[0].vId, character );
            argv[0]=(s32)0;
            return character->Cmd_DisbandGroup();
        }
        
        COMMAND( OnCharSwapGroupLeader )
        {
            CONVERT_CHARACTER_ID( argv[0].vId, character );
            CONVERT_CHARACTER_ID( argv[1].vId, c );
            argv[0]=(s32)0;
            return character->Cmd_SwapGroupLeader(c);
        }
    
        COMMAND( OnCharUnFollow )
        {
            CONVERT_CHARACTER_ID( argv[0].vId, character );
            CONVERT_CHARACTER_ID( argv[1].vId, c );
            
            if ( character->Cmd_UnFollow(c) ) {
                argv[0] = (s32)0;
                return MX_OK ;
            }
            argv[0]=(s32)0;
            return MX_FAILED ;
        }
    
        COMMAND( OnCharFight ) 
        {
            CONVERT_CHARACTER_ID( argv[0].vId, character );
            mxobject* object = character->Cmd_Fight();
            if ( object != NULL ) {
                argv[1] = mxentity::SafeIdt(object);
                argv[0] = (s32)1;
                return MX_OK ;
            }
            argv[0]=(s32)0;
            return MX_FAILED ;
        }

        COMMAND( OnCharSeek ) 
        {
            CONVERT_CHARACTER_ID( argv[0].vId, character );
            mxobject* object = character->Cmd_Seek();
            if ( object != NULL ) {
                argv[1] = mxentity::SafeIdt(object);
                argv[0] = (s32)1;
                return MX_OK ;
            }
            argv[0]=(s32)0;
            return MX_FAILED ;
        }

        COMMAND( OnCharPickup ) 
        {
            argc=0;
            CONVERT_CHARACTER_ID( argv[0].vId, character );
            // TODO Check return Type
            mxobject* object  = character->Cmd_PickupObject();
            if ( object != NULL ) {
                argv[1] = mxentity::SafeIdt(object);
                argv[0] = (s32)1;
                return MX_OK ;
            }
            argv[0]=(s32)0;
            return MX_FAILED ;
        }

        COMMAND( OnCharDrop ) 
        {
            CONVERT_CHARACTER_ID( argv[0].vId, character );
            argv[0]=(s32)0;
            return character->Cmd_DropObject();
        }

        COMMAND( OnCharWait ) 
        {
            CONVERT_CHARACTER_ID( argv[0].vId, character );
            argv[0]=(s32)0;
            return character->Cmd_Wait((mxwait_t)argv[1].vSInt32);
        }

        COMMAND( OnCharControl ) 
        {
            CONVERT_CHARACTER_ID( argv[0].vId, character );
            argv[0]=(s32)0;
            mx->CurrentChar(character);
            return MX_OK;
        }

#if defined(_TUNNELS_)
        COMMAND( OnCharEnterTunnel )
        {
            CONVERT_CHARACTER_ID( argv[0].vId, character );
            argv[0]=(s32)0;
            return character->Cmd_EnterTunnel();
        }

        COMMAND( OnCharExitTunnel ) 
        {
            CONVERT_CHARACTER_ID( argv[0].vId, character );
            argv[0]=(s32)0;
            return character->Cmd_ExitTunnel();
        }
#endif

#if defined(_DDR_)
        COMMAND( OnCharGive )
        {
            CONVERT_DDR_CHARACTER_ID( argv[0].vId, character );
            CONVERT_DDR_CHARACTER_ID( argv[1].vId, to );
            argv[0]=(s32)0;
            return character->Cmd_Give(to);
        }

        COMMAND( OnCharTake )
        {
            CONVERT_DDR_CHARACTER_ID( argv[0].vId, character );
            argv[0]=(s32)0;
            return character->Cmd_Take();
        }

        COMMAND( OnCharUse )
        {
            CONVERT_DDR_CHARACTER_ID( argv[0].vId, character );
            argv[0]=(s32)0;
            return character->Cmd_Use();
        }

#endif
    
        COMMAND( OnRegimentHold )
        {
            CONVERT_REGIMENT_ID( argv[0].vId, regiment );
            regiment->Orders( OD_HOLD );
            regiment->TargetId( 0 );
            argv[0]=(s32)0;
            return MX_OK ;
        }

        COMMAND( OnRegimentGoto )
        {
            CONVERT_REGIMENT_ID( argv[0].vId, regiment );
            CONVERT_ENTITY_ID( argv[1].vId, dst );
            regiment->Orders( OD_GOTO );
            regiment->TargetId( mxentity::SafeIdt(dst) );
            argv[0]=(s32)0;
            return MX_OK ;
        }

        COMMAND( OnRegimentWander )
        {
            CONVERT_REGIMENT_ID( argv[0].vId, regiment );
            regiment->Orders( OD_WANDER );
            argv[0]=(s32)0;
            return MX_OK ;
        }

        COMMAND( OnRegimentFollow )
        {
            CONVERT_REGIMENT_ID( argv[0].vId, regiment );
            CONVERT_ENTITY_ID( argv[1].vId, dst );
            regiment->Orders( OD_FOLLOW );
            regiment->TargetId( mxentity::SafeIdt(dst) );
            argv[0]=(s32)0;
            return MX_OK ;
        }

        COMMAND( OnRegimentRoute )
        {
            CONVERT_REGIMENT_ID( argv[0].vId, regiment );
            CONVERT_ENTITY_ID( argv[1].vId, dst );
            regiment->Orders( OD_ROUTE );
            regiment->TargetId( mxentity::SafeIdt(dst) );
            argv[0]=(s32)0;
            return MX_OK ;
        }


        COMMAND ( OnNight )
        {
            mx->pfnNightCallback = nullptr ;
            mx->SetLastActionMsg("");
            
#if defined(_DDR_)
            sv_days++;
#endif
            
            mx->pfnNightCallback = (PFNNIGHTCALLBACK)argv[0].vPtr ;
            argv[0]=(s32)0;
            
            m_gameover_t win = mx->gameover->Process(true);
            if ( win != MG_NONE ) {
                gameover_callback_t event;
                event.type = callback_t::gameover ;
                event.condition = win ;
                mx->NightCallback(&event);
                argv[1] = (s32)FALSE;
                return MX_OK ;
            }
            
#if defined(_LOM_)
            sv_days++;
#endif
            mx->scenario->DeadCharactersDropObjects();

            mx->SetLastActionMsg(mx->LastActionMsg() + mx->text->CookedSystemString(SS_NIGHT));
            
            mx->NightCallback(NULL);

            mx->scenario->RemoveMapArmies();
            
            sv_strongholdadjuster = mx->scenario->CalcStrongholdAdjuster();
            
            mx->night->Process();

            mx->scenario->SetMapArmies();

            mx->scenario->SetCharsLooking();

            mx->SetLastActionMsg(mx->LastActionMsg() + mx->text->CookedSystemString(SS_DAWN));
            
            argv[1] = (s32)FALSE;

            return MX_OK ;

        }

    
        COMMAND( OnWinLose ) 
        {
            mx->LastActionMsg()[0] = '\0';
            
            m_gameover_t win = mx->gameover->Process(false);
            
            argv[1] = (u32)win;
            
            return MX_OK ;
            
        }
    

        COMMAND( OnGetPanoramic ) 
        {
            // location
            CONVERT_GRIDREF(argv[0],loc);

            // direction
            mxdir_t dir = (mxdir_t)GET_ID(argv[1].vId) ;

            panloc_t* locations;

            int count = mx->gamemap->GetPanoramic ( loc, dir, &locations );

            argv[0] = (s32)count;
            argv[1] = locations ;

            
            return MX_OK ;
        }

        static mxcommand_t mx_commands[] = {
            
            // character commands} },
            {"LOOKLEFT",        1, OnCharLookLeft,          {arguments::character} },
            {"LOOKRIGHT",       1, OnCharLookRight,         {arguments::character} },
            {"LOOK",            2, OnCharLook,              {arguments::character, arguments::direction} },
            {"FORWARD",         1, OnCharForward,           {arguments::character} },
            {"POSTMEN",         3, OnCharPostMen,           {arguments::character, arguments::stronghold, variant::vnumber} },
            {"RECRUITMEN",      3, OnCharRecruitMen,        {arguments::character, arguments::stronghold, variant::vnumber} },
            {"ATTACK",          1, OnCharAttack,            {arguments::character} },
            {"REST",            1, OnCharRest,              {arguments::character} },
            {"HIDE",            1, OnCharHide,              {arguments::character} },
            {"UNHIDE",          1, OnCharUnHide,            {arguments::character} },
            {"LOOKAT",          2, OnCharLookAt,            {arguments::character, arguments::location} },
            {"APPROACH",        2, OnCharApproach,          {arguments::character, arguments::character} },
            {"FIGHT",           1, OnCharFight,             {arguments::character} },
            {"SEEK",            1, OnCharSeek,              {arguments::character} },
            {"PICKUP",          1, OnCharPickup,            {arguments::character} },
            {"DROP",            1, OnCharDrop,              {arguments::character} },
            {"WAIT",            2, OnCharWait,              {arguments::character, variant::vnumber} }, //[mode]
            {"CONTROL",         1, OnCharControl,           {arguments::character} },
#if defined(_TUNNELS_)
            {"ENTERTUNNEL",     1, OnCharEnterTunnel,       {arguments::character} },
            {"EXITTUNNEL",      1, OnCharExitTunnel,        {arguments::character} },
#endif
            
#if defined(_DDR_)
            {"GIVE",            1, OnCharGive,              {arguments::character} },
            {"TAKE",            1, OnCharTake,              {arguments::character} },
            {"USE",             1, OnCharUse,               {arguments::character, arguments::character} },
#endif
            {"FOLLOW",          2, OnCharFollow,            {arguments::character, arguments::character} },
            {"UNFOLLOW",        2, OnCharUnFollow,          {arguments::character, arguments::character} },
            {"DISBANDGROUP",    1, OnCharDisbandGroup,      {arguments::character} },
            {"SWAPGROUPLEADER", 2, OnCharSwapGroupLeader,   {arguments::character, arguments::character} },
            {"PLACE",           2, OnCharPlace,             {arguments::character, arguments::location} },
            // Regiment Commands
            {"HOLD",            1, OnRegimentHold,          {arguments::regiment} },
            {"GOTO",            2, OnRegimentGoto,          {arguments::regiment, arguments::location} },
            {"WANDER",          1, OnRegimentWander,        {arguments::regiment} },
            {"FOLLOW",          2, OnRegimentFollow,        {arguments::regiment, arguments::character} },
            {"ROUTE",           2, OnRegimentRoute,         {arguments::regiment, arguments::routenode} },
            // other
            { "NIGHT",          1, OnNight,                 {variant::vptr} },
            { "CHECKWINLOSE",   0, OnWinLose },
            { "GetPanoramic",   2, OnGetPanoramic,      {arguments::location, arguments::direction}},
        };

        MXRESULT mxscenario::Command ( const std::string& arg, variant argv[], u32 argc )
        {
            return mx->ProcessCommand ( mx_commands, NUMELE(mx_commands), arg, argv, argc );
        }

        
        COMMAND ( PropCurrentCharacter )
        {
            argv[0]=(s32)1;
            argv[1]=mx->CurrentChar()->Id();
            return MX_OK ;
        }

        COMMAND( PropDefaultCharacters )
        {
            CONVERT_COLLECTION(argv[0],collection);
            c_character mycollection;
            mx->scenario->GetDefaultCharacters ( &mycollection );
            if ( mycollection.CreateIdtCollection(collection) )
                return MX_OK ;
            return MX_FAILED ;
        }

        COMMAND ( PropAllCharacters )
        {
            CONVERT_COLLECTION(argv[0],collection);

            collection.Clear();
            FOR_EACH_CHARACTER(c) {
                if ( c!=mx->scenario->doomdark )
                    collection.Add(mxentity::SafeIdt(c));
            }
            return MX_OK ;
            
        }

        COMMAND ( PropAllRegiments )
        {
            CONVERT_COLLECTION(argv[0],collection);
            if ( mx->objRegiments.CreateIdtCollection(collection) )
                return MX_OK ;
            return MX_FAILED ;
        }

        COMMAND ( PropAllRouteNodes )
        {
            CONVERT_COLLECTION(argv[0],collection);
            if ( mx->objRoutenodes.CreateIdtCollection(collection) )
                return MX_OK ;
            return MX_FAILED ;
        }

        COMMAND ( PropAllPlaces )
        {
            CONVERT_COLLECTION(argv[0],collection);
            if ( mx->objPlaces.CreateIdtCollection(collection) )
                return MX_OK ;
            return MX_FAILED ;
        }

        COMMAND ( PropAllStrongholds )
        {
            CONVERT_COLLECTION(argv[0],collection);
            if ( mx->objStrongholds.CreateIdtCollection(collection) )
                return MX_OK ;
            return MX_FAILED ;
        }

        COMMAND ( PropAllObjects )
        {
            CONVERT_COLLECTION(argv[0],collection);
            if ( mx->objObjects.CreateIdtCollection(collection) )
                return MX_OK ;
            return MX_FAILED ;
        }

        COMMAND ( PropRecruitable )
        {
            CONVERT_COLLECTION(argv[0],collection);
            std::unique_ptr<mxlocinfo> locinfo;
            if ( mx->scenario->GetLocInfo ( argv[1], slf_none, locinfo ) != MX_OK )
                return MX_FAILED;
            locinfo->objRecruit.CreateIdtCollection(collection);
            return MX_OK ;
        }

        COMMAND ( PropArmies )
        {
            CONVERT_COLLECTION(argv[0],collection);
            
            if ( ID_TYPE(argv[1].vid) != IDT_LOCATION )
                return MX_FAILED;

            flags32_t flags = slf_none;
#if defined(_TUNNELS_)
                if ( argv[2].vyesno )
                    flags |= slf_tunnel;
#endif
            
            std::unique_ptr<mxlocinfo> locinfo;
            if (mx->scenario->GetLocInfo ( argv[1], flags, locinfo ) != MX_OK )
                return MX_FAILED;

            collection.Create( locinfo->nArmies );
            for ( u32 ii=0; ii<locinfo->nArmies; ii++ ) {
                auto army = locinfo->armies[ii] ;
                int type = army->armytype == AT_CHARACTER ? (128|army->type) : army->armytype ;
                collection[ii] = MAKE_ARMYID(type,mxentity::SafeId(army->parent) );
            }
            argv[0] = (s32)8 ;
            argv[1] = locinfo->foe.warriors ;
            argv[2] = locinfo->foe.riders ;
            argv[3] = locinfo->friends.warriors ;
            argv[4] = locinfo->friends.riders ;
            argv[5] = locinfo->regiments.warriors ;
            argv[6] = locinfo->regiments.riders ;
            argv[7] = locinfo->friends.armies;
            argv[8] = locinfo->foe.armies;

            return MX_OK ;
        }

        COMMAND ( PropStrongholdsById )
        {
            CONVERT_COLLECTION(argv[0],collection);
            
            std::unique_ptr<mxlocinfo> locinfo;
            if (mx->scenario->GetLocInfo ( argv[1], slf_none, locinfo ) != MX_OK )
                return MX_FAILED;
            
            locinfo->objStrongholds.CreateIdtCollection(collection);
            return MX_OK ;
        }

        COMMAND ( PropCharsForCommand )
        {
            CONVERT_COLLECTION(argv[0],collection);
            CONVERT_GRIDREF(argv[2],loc);
            
            c_character chars;
            mx->scenario->GetCharactersAvailableForCommand ( argv[1], loc, &chars );
            chars.CreateIdtCollection( collection );

            return MX_OK ;
        }

        COMMAND ( PropArmiesAtLocation )
        {
            CONVERT_GRIDREF(argv[0],loc);
            u32 enemies=0;
            u32 friends=0;
            mx->scenario->FindArmiesAtLocation ( loc, enemies, friends, argv[1] );
            argv[0]=(s32)2;
            argv[1]=enemies;
            argv[2]=friends;
            return MX_OK ;
        }

        COMMAND ( PropCharsAtLoc )
        {
            CONVERT_COLLECTION(argv[0],collection);
            CONVERT_GRIDREF(argv[1],loc);
            c_character chars;
            mx->scenario->FindCharactersAtLocation ( loc, &chars, argv[2] );
            chars.CreateIdtCollection( collection );
            return MX_OK ;
        }

        COMMAND ( PropMapSize )
        {
            argv[0]=(s32)2;
            argv[1]=(s32)mx->gamemap->Size().cx;
            argv[2]=(s32)mx->gamemap->Size().cy;
            return MX_OK ;
        }

        COMMAND ( PropMapInfo )
        {
            argv[0]=(s32)6;
            
            if ( mx->discoverymap != NULL ) {
                argv[1]=mx->discoverymap->Top().x;
                argv[2]=mx->discoverymap->Top().y;
                argv[3]=mx->discoverymap->Bottom().x;
                argv[4]=mx->discoverymap->Bottom().y;
            }else{
                argv[1]=mx->gamemap->Top().x;
                argv[2]=mx->gamemap->Top().y;
                argv[3]=mx->gamemap->Bottom().x;
                argv[4]=mx->gamemap->Bottom().y;
            }
            
            argv[5]=(s32)argv[3]-(s32)argv[1];
            argv[6]=(s32)argv[4]-(s32)argv[2];
            return MX_OK ;
        }
    
        static mxcommand_t mx_properties[] = {
            { "CONTROLLED_CHARACTER",     0, PropCurrentCharacter },

            { "DEFAULTCHARACTERS",  1, PropDefaultCharacters,   {variant::vptr} },
            { "ALLCHARACTERS",      1, PropAllCharacters,       {variant::vptr} },
            { "ALLREGIMENTS",       1, PropAllRegiments,        {variant::vptr} },
            { "ALLROUTENODES",      1, PropAllRouteNodes,       {variant::vptr} },
            { "ALLSTRONGHOLDS",     1, PropAllStrongholds,      {variant::vptr} },
            { "ALLPLACES",          1, PropAllPlaces,           {variant::vptr} },
            { "AllOBJECTS",         1, PropAllObjects,          {variant::vptr} },

            { "RECRUITABLE",        2, PropRecruitable,         {variant::vptr, variant::vid} },
            { "ARMIES",             2, PropArmies,              {variant::vptr, variant::vid} },
            { "STRONGHOLDS",        2, PropStrongholdsById,     {variant::vptr, variant::vid} },
            
            { "CharsForCommand",    3, PropCharsForCommand,     {variant::vptr, variant::vnumber, arguments::location} },
            { "ArmiesAtLocation",   2, PropArmiesAtLocation,    {arguments::location, variant::vnumber} },
            { "CharsAtLoc",         3, PropCharsAtLoc,          {variant::vptr, arguments::location, variant::vnumber} },
            
            { "MAPINFO",            0, PropMapInfo,             },
            { "MAPSIZE",            0, PropMapSize,             },

            
        };

        MXRESULT mxscenario::GetProperties ( const std::string& arg, variant argv[], u32 argc )
        {
            return mx->ProcessCommand ( mx_properties, NUMELE(mx_properties), arg, argv, argc );
        }

        std::string  stringBuffer;
        
        #define RETURN_STRING(x) \
            stringBuffer = x; \
            argv[0] = (s32)1 ; \
            argv[1].vString = (LPSTR)stringBuffer.c_str(); \
            return MX_OK
        

        COMMAND ( OnTextNumberPart ) 
        {
            RETURN_STRING(mx->text->DescribeNumberPart ( (s32)argv[0] ))  ;
        }
                
        COMMAND ( OnTextEnergy ) 
        {
            RETURN_STRING(mx->text->DescribeEnergy ( argv[0] ));
        }
                    
        COMMAND ( OnTextFear ) 
        {
            RETURN_STRING(mx->text->DescribeFear ( argv[0] ));
        }
                        
        COMMAND ( OnTextCourage ) 
        {
            RETURN_STRING(mx->text->DescribeCourage ( argv[0] ));
        }
                    
        COMMAND ( OnTextCharRecruitMen ) 
        {
            CONVERT_CHARACTER_ID( argv[0].vId, character );
            CONVERT_STRONGHOLD_ID( argv[1].vId, stronghold );
            RETURN_STRING(mx->text->DescribeCharacterRecruitMen ( character, stronghold, argv[2] ));
        }
            
        COMMAND ( OnTextCharPostMen ) 
        {
            CONVERT_CHARACTER_ID( argv[0].vId, character );
            CONVERT_STRONGHOLD_ID( argv[1].vId, stronghold );
            RETURN_STRING(mx->text->DescribeCharacterPostMen ( character, stronghold, argv[2] ));
        }
                
        COMMAND ( OnTextCharTime ) 
        {
            CONVERT_CHARACTER_ID( argv[0].vId, character );
            RETURN_STRING(mx->text->DescribeCharacterTime( character ));
        }
                    
        COMMAND ( OnTextCharEnergy ) 
        {
            CONVERT_CHARACTER_ID( argv[0].vId, character );
            RETURN_STRING(mx->text->DescribeCharacterEnergy ( character )) ;
        }
                
        COMMAND ( OnTextCharCourage ) 
        {
            CONVERT_CHARACTER_ID( argv[0].vId, character );
            RETURN_STRING(mx->text->DescribeCharacterCourage ( character )) ;
        }
                
        COMMAND ( OnTextCharFear ) 
        {
            CONVERT_CHARACTER_ID( argv[0].vId, character );
            RETURN_STRING(mx->text->DescribeCharacterFear ( character ));
        }
                    
        COMMAND ( OnTextCharObject ) 
        {
            CONVERT_CHARACTER_ID( argv[0].vId, character );
            RETURN_STRING(mx->text->DescribeCharacterObject ( character ));
        }
                
        COMMAND ( OnTextCharDeath ) 
        {
            CONVERT_CHARACTER_ID( argv[0].vId, character );
            RETURN_STRING(mx->text->DescribeCharacterDeath ( character ));
        }
                    
        COMMAND ( OnTextCharBattle ) 
        {
            CONVERT_CHARACTER_ID( argv[0].vId, character );
            RETURN_STRING( mx->text->DescribeCharacterBattle ( character ));
        }
                
        COMMAND ( OnTextCharArmy ) 
        {
            CONVERT_CHARACTER_ID( argv[0].vId, character );
            RETURN_STRING(mx->text->DescribeCharacterArmy ( character ));
        }
       
        COMMAND ( OnTextCharLocation ) 
        {
            CONVERT_CHARACTER_ID( argv[0].vId, character );
            mx->CurrentChar(character);
            RETURN_STRING(mx->text->DescribeCharacterLocation( character ));
        }
                
        COMMAND ( OnTextStronghold ) 
        {
            CONVERT_STRONGHOLD_ID( argv[0].vId, stronghold );
            RETURN_STRING(mx->text->DescribeStronghold (stronghold)) ;
        }
    
        COMMAND ( OnTextLocation ) 
        {
            mxgridref loc = mxgridref( GET_LOCIDX(argv[0].vId),GET_LOCIDY(argv[0].vId) );
            RETURN_STRING(mx->text->DescribeLocation(loc));
        }
                    
        COMMAND ( OnTextArea ) 
        {
            RETURN_STRING(mx->text->DescribeArea(GET_ID(argv[0].vId)));
        }

        COMMAND ( OnTextObject ) 
        {
            CONVERT_OBJECT_ID(argv[0].vId,object);
            RETURN_STRING(mx->text->DescribeObject(object));
        }
                        
        COMMAND ( OnTextTerrainPlural ) 
        {
            RETURN_STRING(mx->text->DescribeTerrainPlural((mxterrain_t)GET_ID(argv[0].vId)));
        }
                
        COMMAND ( OnTextTerrain ) 
        {
            RETURN_STRING(mx->text->DescribeTerrainSingularPlural((mxterrain_t)GET_ID(argv[0].vId)));
        }
                    
        COMMAND ( OnTextNumber )
        {
            RETURN_STRING(mx->text->DescribeNumber ( (s32)argv[0], (mxtext::ZERO_MODE)(s32)argv[1] ));
        }

        COMMAND ( OnTextSpecialStrings )
        {
            CONVERT_CHARACTER_ID( argv[1].vId, character );
            RETURN_STRING(mx->text->CookedSystemString(GET_ID(argv[0].vId), character ));
        }

        COMMAND ( OnTextPurge )
        {
            argv[0] = (s32)0 ;
            return MX_OK;
        }

        COMMAND ( OnLastActionMsg )
        {
            RETURN_STRING(mx->LastActionMsg());
        }

    
        static mxcommand_t mx_text[] = {
            {"Number",          2, OnTextNumber,            {variant::vnumber, variant::vnumber} },
            {"NumberPart",      1, OnTextNumberPart,        {variant::vnumber} },
            {"Energy",          1, OnTextEnergy,            {variant::vnumber} },
            {"Fear",            1, OnTextFear,              {variant::vnumber} },
            {"Courage",         1, OnTextCourage,           {variant::vnumber} },
            {"CharRecruitMen",  3, OnTextCharRecruitMen,    {arguments::character, arguments::stronghold, variant::vnumber} },
            {"CharPostMen",     3, OnTextCharPostMen,       {arguments::character, arguments::stronghold, variant::vnumber} },
            {"CharTime",        1, OnTextCharTime,          {arguments::character} },
            {"CharEnergy",      1, OnTextCharEnergy,        {arguments::character} },
            {"CharCourage",     1, OnTextCharCourage,       {arguments::character} },
            {"CharFear",        1, OnTextCharFear,          {arguments::character} },
            {"CharObject",      1, OnTextCharObject,        {arguments::character} },
            {"CharDeath",       1, OnTextCharDeath,         {arguments::character} },
            {"CharBattle",      1, OnTextCharBattle,        {arguments::character} },
            {"CharArmy",        1, OnTextCharArmy,          {arguments::character} },
            {"CharLocation",    1, OnTextCharLocation,      {arguments::character} },
            {"Stronghold",      1, OnTextStronghold,        {arguments::stronghold} },
            {"Location",        1, OnTextLocation,          {arguments::location} },
            {"Area",            1, OnTextArea,              {arguments::area} },
            {"Object",          1, OnTextObject,            {arguments::objectinfo} },
            {"TerrainPlural",   1, OnTextTerrainPlural,     {arguments::terrain} },
            {"Terrain",         1, OnTextTerrain,           {arguments::terrain} },
            {"SpecialStrings",  2, OnTextSpecialStrings,    {variant::vnumber, arguments::character} },
            {"LastActionMsg",   0, OnLastActionMsg,         },
            {"Purge",           0, OnTextPurge,             },
        };


        MXRESULT mxscenario::Text ( const std::string& arg, variant* argv, u32 argc )
        {
            return mx->ProcessCommand ( mx_text, NUMELE(mx_text), arg, argv, argc );
        }

        mxcharacter* mxscenario::CurrentMoonringWearer ( void )
        {
            if ( IsFeature(SF_MOONRING) ) {
                return moonring != nullptr ? WhoHasObject(moonring) : NULL ;
            }
            return NULL ;
        }

        mxcharacter* mxscenario::MoonringWearer( void ) 
        {
            if ( IsFeature(SF_MOONRING) ) {
                FOR_EACH_CHARACTER(character) {
                    if ( character->IsAllowedMoonring() && character->IsAlive() )
                        return character ;
                }
            }
            return nullptr ;
        }

        void mxscenario::DeadCharactersDropObjects( void )
        {
            FOR_EACH_CHARACTER(character){
                if ( character->IsDead() && character->carrying ) {
                    if ( character->carrying->IsUnique() )
                        character->Cmd_DropObject() ;
                }
            }
        }
    
        void mxscenario::CheckMoonringWearerDead ( void )
        {
            if ( !IsFeature(SF_MOONRING) )
                return;
            
            mxcharacter* moonringcarrier = CurrentMoonringWearer();
            if ( moonringcarrier != NULL )
                return;
            
            FOR_EACH_CHARACTER(character) {
                if ( character->IsAllowedMoonring() && character->IsAlive() ) {
                    // if character is alive
                    // and they can carry the moonring
                    // then remove them from any grouping
                    
                    if ( character->IsFollowing() )
                        character->Cmd_UnFollow(character->Following());
                    
                    for (auto f : mx->scenario->GetCharacterFollowers(character)) {
                        if ( f->Following() == character )
                            f->Cmd_UnFollow(character);
                    }
                }
            }
        }
    

        mxgridref mxscenario::FindLookingTowards( mxgridref loc, mxdir_t dir )
        {
        int            ii;
        mxloc       mapsqr;
            
            for (ii = 0; ii < sv_lookforwarddistance; ii++) {
                loc += dir ;
                mapsqr = mx->gamemap->GetAt(loc);
                if ( mapsqr.IsInteresting())
                    return loc;
#if defined(_DDR_)
                if ( mapsqr.IsMisty() )
                    return loc;
#endif
            }

            return loc ;
        }


        void mxscenario::ResetMapArmies ( void )
        {
            RemoveMapArmies();
            SetMapArmies();
        }

        void mxscenario::SetMapArmies ( void )
        {
            FOR_EACH_CHARACTER(character) {
                auto processLocation = true;
                        
                if (character->IsInTunnel() )
                    processLocation = false;

#if defined(_DDR_)
                if ( character->IsDead() || character->IsHidden() )
                    processLocation = false;
#endif
                if ( processLocation ) {
                    if ( character->warriors.Total()+character->riders.Total() ) {
                        mx->gamemap->SetLocationArmy(character->Location(),1);
                    }
                    mx->gamemap->SetLocationCharacter(character->Location(),1);
                }
            }
            
#if !defined(_DDR_)
            FOR_EACH_STRONGHOLD(stronghold) {
                if ( stronghold->TotalTroops() )
                    mx->gamemap->SetLocationArmy(stronghold->Location(),1);
            }
#endif

            FOR_EACH_REGIMENT(regiment) {
                if ( regiment->Total() )
                    mx->gamemap->SetLocationArmy(regiment->Location(),1);
            }

        }


        void mxscenario::RemoveMapArmies ( void )
        {
            FOR_EACH_CHARACTER(character) {
                mx->gamemap->SetLocationArmy(character->Location(),0);
                mx->gamemap->SetLocationCharacter(character->Location(),0);
            }

            FOR_EACH_STRONGHOLD(stronghold) {
                mx->gamemap->SetLocationArmy(stronghold->Location(),0);
            }

            FOR_EACH_REGIMENT(regiment) {
                mx->gamemap->SetLocationArmy(regiment->Location(),0);
            }
        }

        void mxscenario::LookInDirection(mxgridref loc, mxdir_t dir, bool isintunnel)
        {
        
        panloc_t*    pview;
        int            ii;
        int            count;

            loc_t looked_at = loc + dir ;
            
            //
            if ( isintunnel ) {
                mx->gamemap->SetTunnelVisible(loc, true);
                if ( !IS_DIAGONAL(dir) && mx->gamemap->IsTunnel(looked_at))
                    mx->gamemap->SetTunnelVisible(looked_at, true);
                return;
            }

            mx->gamemap->SetLocationVisible(loc, true);
            
            mx->gamemap->SetLocationLookedAt(looked_at, true);
            
            count = mx->gamemap->GetPanoramic ( loc, dir, &pview );

            for ( ii=0; ii<count; ii++ ) {
#if defined(_DDR_)
                mxloc& mapsqr = mx->gamemap->GetAt(pview[ii].location);
                if ( mapsqr.flags & lf_mist )
                    continue;
#endif
                // if there is an army here then we need to draw the army
                //info::terrain* tinfo = TerrainById(map.terrain);

                if ( pview[ii].size < 4 ) {
                    mx->gamemap->SetLocationVisible(pview[ii].location, true);
                }
            }
            
        

        }


        void mxscenario::SetCharsLooking( void )
        {
            // setup the initial stuff that characters looking will do
            FOR_EACH_CHARACTER(character) {
                if ( character->IsRecruited() ) {
                    character->Cmd_LookDir ( character->Looking() );
                    character->memory.Update();
                    character->EnterLocation ( character->Location() );
                }
            }
        }

        mxcharacter* mxscenario::WhoHasObject( mxobject* object ) const
        {
            FOR_EACH_CHARACTER(character) {
                if ( character->carrying == object )
                    return character ;
            }
            return nullptr ;
        }


        bool mxscenario::DropObject ( mxgridref loc, mxobject* obj )
        {
            mx->gamemap->GetAt ( loc ).object = obj ? obj->Id() : OB_NONE ;
            return true ;
        }


        mxobject* mxscenario::PickupObject ( mxgridref loc )
        {
            mxthing_t oldobject ;

            oldobject = (mxthing_t)mx->gamemap->GetAt ( loc ).object ;

            mx->gamemap->GetAt ( loc ).RemoveObject();
            
            return mx->ObjectById(oldobject) ;
        }

        bool mxscenario::InCharactersMemory ( mxitem* item ) 
        {
            FOR_EACH_CHARACTER(character) {
                if ( character->memory.IsMemorised(item) )
                    return true;
            }
            return false;
        }


        void mxscenario::ClearFromMemory ( mxgridref loc )
        {
            FOR_EACH_CHARACTER(character) {
                auto item = character->memory.IsMemorised(loc,RA_DOOMGUARD);
                if ( item ) {
                    character->memory.DeleteFragment ( item );
                }
                item = character->memory.IsMemorised(loc,RA_FREE);
                if ( item ) {
                    character->memory.DeleteFragment ( item );
                }
            }
        }
        
        MXRESULT mxscenario::GetLocInfo ( mxid id, flags32_t flags, std::unique_ptr<mxlocinfo>& info)
        {
            if ( ID_TYPE(id) == IDT_LOCATION ) {
                CONVERT_GRIDREF(id,loc);
                info.reset( new mxlocinfo(loc, nullptr, flags) );
                return MX_OK;
            }else if ( ID_TYPE(id) == IDT_STRONGHOLD ) {
                CONVERT_STRONGHOLD_ID(id,stronghold);
                info.reset( new mxlocinfo(stronghold->Location(), nullptr, slf_none) );
                return MX_OK;
            }else if ( ID_TYPE(id) == IDT_ROUTENODE ) {
                CONVERT_ROUTENODE_ID(id,routenode);
                info.reset( new mxlocinfo(routenode->Location(), nullptr, slf_none) );
                return MX_OK;
            }else if ( ID_TYPE(id) == IDT_CHARACTER ) {
                CONVERT_CHARACTER_ID(id,character);
                info = character->GetLocInfo() ;
                return MX_OK;
            }
            
            return MX_FAILED;
        }

        bool mxscenario::GetCharactersAvailableForCommand ( u32 mode, mxgridref loc, c_character* collection )
        {
        mxcharacter*    moonringcarrier=nullptr;
        
            RETURN_IF_NULL(collection) false;
            collection->Clear();

            if ( IsFeature(SF_MOONRING) ) {
                //moonringcarrier = moonring ? WhoHasObject(moonring) : NULL ;
                moonringcarrier = CurrentMoonringWearer();
            }

            FOR_EACH_CHARACTER(c) {

                if ( mode != CMDG_ALL ) {

                    CONTINUE_IF ( !c->IsRecruited() );

                    if ( IsFeature(SF_MOONRING) ) {
                        CONTINUE_IF ( moonring && moonringcarrier==nullptr && !c->IsAllowedMoonring() );
                    }

                    if ( mode==CMDG_WAITING ) {
                        CONTINUE_IF ( c->IsWaiting() || c->IsNight() || c->IsDead() );
                    }else if ( mode==CMDG_CURRENTLOC ) {
                        CONTINUE_IF ( c->Location() != loc );
                    }

                }else{
                    CONTINUE_IF ( c == doomdark );
                }

                collection->Add(c);

            }

            return true ;
        }


    // only recruited/moonring
    // above ground
    // underground

        u32 mxscenario::FindCharactersAtLocation ( mxgridref loc, c_character* characters, flags32_t flags )
        {
        mxcharacter*    moonringcarrier=nullptr;
        bool            in_tunnel = ( flags & slf_tunnel ) == slf_tunnel;
        bool            show_dead = ( flags & slf_dead ) == slf_dead;

            RETURN_IF_NULL(characters) false;

            if ( IsFeature(SF_MOONRING) ) {
                //moonringcarrier = moonring ? WhoHasObject(moonring) : NULL ;
                moonringcarrier = CurrentMoonringWearer();
            }

            characters->Clear();

            FOR_EACH_CHARACTER(c) {
                CONTINUE_IF_NULL(c);

                CONTINUE_IF ( c == doomdark );

                CONTINUE_IF ( c->Location() != loc );
                
                CONTINUE_IF( c->IsInTunnel() != in_tunnel );
                
                CONTINUE_IF( !show_dead && c->IsDead() );
                
                if ( !(flags & slf_all) ) {
                    CONTINUE_IF ( !c->IsRecruited() );
                    if ( IsFeature(SF_MOONRING) ) {
                        CONTINUE_IF ( moonring && moonringcarrier==nullptr && !c->IsAllowedMoonring() );
                    }
                }

                characters->Add(c) ;
            }

            return characters->Count();
        }


        u32 mxscenario::FindArmiesAtLocation ( mxgridref loc, u32& enemies, u32& friends, flags32_t flags )
        {
            enemies=0;
            friends=0;

            FOR_EACH_CHARACTER(character) {
                if ( character->memory.IsMemorised(loc,RA_DOOMGUARD) )
                    enemies++;
                if ( character->memory.IsMemorised(loc,RA_FREE) )
                    friends++;
                if ( character->Location() == loc && character->HasArmy() )
                    friends++;
            }
            return friends+enemies ;
        }

    s32 mxscenario::BaseDoomdarkSuccess ( mxrace_t race, mxunit_t unit, const mxlocinfo& locinfo )
    {
        // TODO this should be a lookup table
        // TODO this should be race dependent
        s32 modifier = (unit == UT_WARRIORS)
            ? (s32)sv_battle_success_regiment_warriors
            : (s32)sv_battle_success_regiment_riders ;
            
        return locinfo.foe.adjustment + ( locinfo.adj_fear / modifier );
    }


    void mxscenario::initialise( u32 version )
    {
        moonring = static_cast<mxobject*>(mx->EntityByName("OB_MOONRING",IDT_OBJECT));
        luxor = mx->CharacterBySymbol("CH_LUXOR");
        doomdark = mx->CharacterBySymbol("CH_DOOMDARK");
        // the Lord of Dreams is a key character for Fey recruiting
        dreams = mx->CharacterBySymbol("CH_DREAMS");
    }
    
    void mxscenario::initialiseAfterCreate( u32 version )
    {
        MXTRACE( "Init Map" );
        SetMapArmies();
        SetCharsLooking();
        
#if defined(_DDR_)
        FOR_EACH_CHARACTER(c) {
            static_cast<ddr_character*>(c)->lastlocation=c->Location();
        }
#endif
        
        MXTRACE( "Place Strongholds On Map");
        // mark the strongholds onto the map
        FOR_EACH_STRONGHOLD(stronghold) {
            mxloc& mapsqr = mx->gamemap->GetAt ( stronghold->Location() );
            mapsqr.flags|=lf_stronghold;
        }
        
        MXTRACE( "Place RouteNodes On Map");
        // mark the routenodes onto the map
        FOR_EACH_ROUTENODE(routenode) {
            mxloc& mapsqr = mx->gamemap->GetAt ( routenode->Location() );
            mapsqr.flags|=lf_routenode;
        }

        
        // let's add some Terrain Info
//#define ADD_TERRAIN(x) \
//        mx->objTerrainInfos.Add( new mxterrain( x, #x ) )
    
// NOTE: These can not be added to the container
// because the container is owned by itself
// this was temp hack for Citadel testing
//        ADD_TERRAIN(TN_LAND);
//        ADD_TERRAIN(TN_ISLE);
//        ADD_TERRAIN(TN_LAKELAND);
//        ADD_TERRAIN(TN_PLAIN);
//        ADD_TERRAIN(TN_PLAINS3);
//        ADD_TERRAIN(TN_FOREST3);
//        ADD_TERRAIN(TN_UNUSED_39);
//        ADD_TERRAIN(TN_TREES);
//        ADD_TERRAIN(TN_MOUNTAIN3);
//        ADD_TERRAIN(TN_ICY_MOUNTAIN);
//        ADD_TERRAIN(TN_DOWNS3);
//        ADD_TERRAIN(TN_HILLS3);
//        ADD_TERRAIN(TN_FOOTHILLS);
//        ADD_TERRAIN(TN_VALLEY);
//        ADD_TERRAIN(TN_BAY);
//        ADD_TERRAIN(TN_SEA);
//        ADD_TERRAIN(TN_RIVER);
//        ADD_TERRAIN(TN_MARSH);
//        ADD_TERRAIN(TN_LAKE3);
//        ADD_TERRAIN(TN_UNUSED_52);
//        ADD_TERRAIN(TN_UNUSED_53);
//        ADD_TERRAIN(TN_UNUSED_54);
//        ADD_TERRAIN(TN_MIST);
//        ADD_TERRAIN(TN_UNUSED_56);
        
        
    }
    
    void mxscenario::updateAfterLoad(u32 version)
    {
        /* update map */
        if ( version <= 7 ) {
            utils::UpdateStrongholdsOnMap();
        }
    }

    void mxscenario::GiveGuidance( mxcharacter* character, s32 hint )
    {
        bool mikeseek = (hint == 1);
        
        bool found = false;
        if ( mxchance(0.80f) || mikeseek ) {
            while ( !found  ) {
                int id = mxrandom(1, sv_characters - 1);
                
                auto c = mikeseek
                    ? mx->CharacterBySymbol("CH_MIDWINTER")
                    : mx->CharacterById(id);

                // lets not find doomdark
                if ( c->Race()==RA_DOOMGUARD )
                    continue;
                
                c_character collection;
                mx->scenario->GetDefaultCharacters ( &collection );
                if ( collection.FindSymbol(c->Symbol()) == NULL ) {
                    std::string guidance = mx->text->SystemString(SS_GUIDANCE1);
                    mx->SetLastActionMsg(mx->text->CookText(guidance,c));
                    found=TRUE;
                }
                
                
                
                
            }
        }
        if ( !found ) {
            int msg = mxrandom(0, sv_guidance.Count() - 1);
            std::string guidance = mx->text->SystemStringById(sv_guidance[msg]);
            mx->SetLastActionMsg(mx->text->CookText(guidance));
        }

    }

    //}
    // namespace scenarios
}
// namespace tme





