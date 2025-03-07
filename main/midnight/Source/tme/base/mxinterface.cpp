
/*
 * FILE:    mxinterface.cpp
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

#include "../baseinc/tme_internal.h"
#if defined(_DDR_)
#include "../scenarios/ddr/scenario_ddr_internal.h"
#endif
#include <string.h>
#include <string>
#include <memory>


namespace tme {

    // some globals
    mxid MakeLocId(s16 x, s16 y){
        u16 x1 = ( x<0 ) ? 4096 + x : x ;
        u16 y1 = ( y<0 ) ? 4096 + y : y ;
        return MAKE_ID(IDT_LOCATION,(((x1<<12)|y1)));
    }
    
    mxid MakeMapLocId(s16 x, s16 y){
        u16 x1 = ( x<0 ) ? 4096 + x : x ;
        u16 y1 = ( y<0 ) ? 4096 + y : y ;
        return MAKE_ID(IDT_MAPLOCATION,(((x1<<12)|y1)));
    }
    
    s16 GetLocX(mxid id){
        u16 x = ((id>>12)&0xfff);
        return ( x<2048 ) ? x : x - 4096;
    }

    s16 GetLocY(mxid id){
        u16 y = (id&0xfff);
        return ( y<2048 ) ? y : y - 4096 ;
    }
    //
    

    mxinterface::mxinterface(void)
    {
        mx = new mxengine ;
    }

    mxinterface::~mxinterface(void)
    {
        SAFEDELETE(mx);
    }

    //
    // METHOD:    GetProperty
    //            Returns an engine variable value
    // 
    // PARAMS:    name            Property Name
    // [out]    arg                Property Value
    // 
    // REMARKS:    
    //
    // RETURNS:    MXRESULT
    //
    MXRESULT mxinterface::GetProperty( const std::string& name, variant& arg) const
    {
        return variables::GetProperty(name, arg);
    }

    //
    // METHOD:    EntityByName
    //            Returns the Entity Id reference by the name symbol for the given type
    // 
    // PARAMS:    name            Entity Symbol Name
    //            type            Entity Type
    // 
    // REMARKS:    
    //
    // RETURNS:    idt
    //
    mxid mxinterface::EntityByName( const std::string& name, id_type_t type )
    {
        mxentity* obj  = mx->EntityByName( name, type );
        if ( obj ) {
            return mxentity::SafeIdt(obj);
        }
        return mx->text->StringByName(name);
    }

    //
    // METHOD:    EntityLinkData
    //            Attaches user data to and Entity
    // 
    // PARAMS:    Id                Entity Id
    //            data            Data to attach to entity
    // 
    // REMARKS:    Data is not used by engine
    //
    // RETURNS:    MXRESULT
    //
    MXRESULT mxinterface::EntityLinkData( mxid id, const void* data )
    {
        return mx->EntityLinkData(id,data);
    }

    
    void* mxinterface::EntityUserData ( mxid id )
    {
        mxentity* entity = mx->EntityByIdt( id );
        if ( entity )
            return (void*)entity->GetUserData();
            
        return NULL ;
        
    }
    
    LPCSTR mxinterface::EntitySymbolById(mxid id)
    {
        return mx->EntitySymbolById(id);
    }
    
    
    //
    // METHOD:    EntityById
    //            Fills and Entity structure for the given Entity Id
    // 
    // PARAMS:    Id                Entity Id
    //            info            Entity Structure
    // 
    // REMARKS:    
    //
    // RETURNS:    MXRESULT
    //
    MXRESULT mxinterface::EntityById( mxid id, info_t* info )
    {
    mxentity*    obj = NULL ;

        id_type_t type = (id_type_t)ID_TYPE(id);
        int objectno = GET_ID(id);

        switch ( type ) {
    
            case IDT_ARMY:
                {
                int type = GET_ARMYIDTYPE(id);
                objectno = GET_ARMYID(id);

                VALIDATE_INFO_BLOCK(info,INFO_ARMY,defaultexport::army_t);

                defaultexport::army_t* out = (defaultexport::army_t*) info ;

                if ( type == AT_REGIMENT ) {
                    obj = (mxentity*)mx->RegimentById(objectno);
                } else if ( type == AT_STRONGHOLD ) {
                    obj = (mxentity*)mx->StrongholdById(objectno);
#if defined(_LOM_)
                } else if ( type == AT_CHARACTER ) {
                    // character
                    obj = (mxentity*)mx->CharacterById(objectno);
                    // select which unit ( Warriors/Riders etc... )
                    out->unit = (mxunit_t) (type&0x7f) ;
                    // now let export handle correctly
                } else {
                    return MX_INVALID_ID;
                }
#endif
#if defined(_DDR_)
                } else {
                    // character
                    obj = (mxentity*)mx->CharacterById(objectno);
                    // select which unit ( Warriors/Riders etc... )
                    out->unit = (mxunit_t) (type&0x7f) ;
                    // now let export handle correctly
                }
#endif

                }
                break;

            case IDT_MAPLOCATION:
            case IDT_LOCATION:
                {

                mxgridref loc = mxgridref(GET_LOCIDX(id),GET_LOCIDY(id)) ;
                //if ( !mx->gamemap->IsLocOnMap(loc) )
                //    return MX_FAILED ;

                if ( type == IDT_MAPLOCATION ) {
                    
                    if ( mx->discoverymap != NULL ) {
                        loc.x += mx->discoverymap->Top().x;
                        loc.y += mx->discoverymap->Top().y;
                    }else{
                        loc.x += mx->gamemap->Top().x;
                        loc.y += mx->gamemap->Top().y;
                    }
                    
                }
                    
                mxloc& m = mx->gamemap->GetAt( loc ) ;
                defaultexport::location_t* out = (defaultexport::location_t*) info ;
                out->id = id ;
                out->location.x=loc.x;
                out->location.y=loc.y;
                out->terrain = (mxterrain_t)m.terrain;
                
                out->object = IDT_NONE ;
                
         
                out->object_tunnel = IDT_NONE ;
                if ( m.IsTunnelObject() )
                    out->object_tunnel = m.object ? MAKE_ID(IDT_OBJECT,m.object) : OB_NONE ;
                else
                    out->object = m.object ? MAKE_ID(IDT_OBJECT,m.object) : OB_NONE ;
                    
                out->area = MAKE_ID(IDT_AREAINFO,m.area);
                out->climate = m.climate ;
                out->density = mx->gamemap->Density(loc);

                out->flags.Set( m.flags );
                out->discovery_flags.Clear();
                
                if ( mx->discoverymap != NULL ) {
                    out->discovery_flags.Set( mx->discoverymap->GetAt(loc) );
                }

                if (mx->scenario->IsFeature(SF_TUNNELS)) {
                    if ( m.HasTunnelExit() )
                        out->flags.Set(lf_tunnel_exit);
                    if ( m.HasTunnelEntrance() )
                        out->flags.Set(lf_tunnel_entrance);
                    if ( m.IsTunnelObject() )
                        out->flags.Set(lf_tunnel_object);
                }
                    
#if defined(_DDR_)
                if ( m.HasObject() ) {
                    auto scenario = static_cast<ddr_x*>(mx->scenario);
                    auto obj = static_cast<ddr_object*>(scenario->FindObjectAtLocation(loc));
                    if ( obj && obj->IsSpecial() )
                        out->flags.Set(lf_object);
                }
#endif
                return MX_OK ;
                }
                break;
            default:
                obj = mx->EntityByIdt(id);
                break;
        }
        
        if ( obj ) {
            return obj->FillExportData(info);
        }

        return MX_FAILED ;
    }

    //
    // METHOD:    GetProperties
    //            Returns values for a scenario property
    // 
    // PARAMS:    arg                Property Name
    // [out]    argv            Array to hold returned values
    //            argc            Size of array
    // 
    // REMARKS:    Array must be large enough to take the result + 1
    //            First item in the array will always contain the number of returned values
    //
    // RETURNS:    MXRESULT
    //
    MXRESULT mxinterface::GetProperties ( const std::string& arg, variant argv[], u32 argc )
    {
        return mx->scenario->GetProperties(arg,argv,argc);
    }

    //
    // METHOD:    Text
    //            Returns a series of text items for a scenario text command
    // 
    // PARAMS:    arg                Text Item name
    // [out]    argv            Array to hold returned values
    //            argc            Size of array
    // 
    // REMARKS:    Array must be large enough to take the result + 1
    //            First item in the array will always contain the number of returned values
    //
    // RETURNS:    MXRESULT
    //    
    MXRESULT mxinterface::Text ( const std::string& arg, variant* argv, u32 argc )
    {
        return mx->scenario->Text(arg,argv,argc);
    }

    //
    // METHOD:    ProcessCommand
    //            Runs a command inside the engine
    // 
    // PARAMS:    tblCommand        Table of Commands to check against
    //            max                Number of commands in the table
    //            arg                Name of Command to run
    //            argv            Input Arguments    
    //            argc            Input Argument Count
    // 
    // REMARKS:    This is an internal function and is not exposed to the interface
    //
    // RETURNS:    MXRESULT
    //
    MXRESULT mxengine::ProcessCommand ( mxcommand_t tblCommand[], u32 max, const std::string& arg, variant argv[], u32 argc )
    {
    MXRESULT    status = MX_UNKNOWN;

        if ( arg.empty() ) {
            argc=0;
            return MX_FAILED ;
        }

        for ( u32 ii=0; ii<max; ii++ ) {

            if ( ISARG( tblCommand[ii].name ) ) {

                if ( argc != tblCommand[ii].argcount ) {
                    status = MX_INVALID_ARGUMENT_COUNT;
                    continue;
                }

                // generic validate commands
                for ( u32 jj=0; jj<tblCommand[ii].argcount; jj++ ) {

                    if ( tblCommand[ii].arguments[jj] < 128 ) {
                        if ( (u32)argv[jj].vType != (u32)tblCommand[ii].arguments[jj] ) {
                            status = MX_INVALID_ARGUMENT;
                            continue;
                        }
                    }else {
                        if ( ID_TYPE(argv[jj].vId) != tblCommand[ii].arguments[jj]-128 ) {
                            status = MX_INVALID_ARGUMENT;
                            continue;
                        }
                    }
                }

                if ( tblCommand[ii].pfnCommand )
                    return tblCommand[ii].pfnCommand ( arg, argv, argc );

                break;
            }
        }


        return status ;
    }

    // COMMAND: @SETSCENARIO
    //            Sets the scenario for the engine to use
    //
    COMMAND( OnSetScenario ) 
    {
        mxscenario* scenario = (mxscenario*)argv[0].vPtr;
        if ( scenario == NULL )
            return MX_FAILED ;
        argv[0]=(s32)0;
        return mx->LoadScenario(scenario);
    }

    // COMMAND: @SETDATABASEDIRECTORY
    //            Tells the engine where to find the scenario data
    //
    COMMAND( OnSetDatabaseDirectory ) 
    {
        std::string directory;
        directory = (LPCSTR)argv[0] ;
//        std::string directory("lom") ;

        MXRESULT result = mx->SetDatabaseDirectory(directory);
        //argv[0]=0;
        return result;
    }

    // COMMAND: @INIT
    //            Loads the scenario database
    //
    COMMAND( OnInit ) 
    {
        auto rules = (RULEFLAGS) (u64)argv[0] ;
        auto difficulty = (mxdifficulty_t) (u64)argv[1];
        return mx->LoadDatabase(rules,difficulty);
    }

    // COMMAND: @DEINIT
    //            Unloads the scenario database
    //
    COMMAND( OnDeInit ) 
    {
        //argv[0]=0;
        return mx->UnloadDatabase();
    }
    
    // COMMAND: @SAVEGAME
    //            Saves the current game to the given path
    //
    COMMAND( OnSaveGame ) 
    {
        std::string filename = (LPCSTR)argv[0];
        return mx->SaveGame ( filename, (PFNSERIALIZE)((void*)argv[1]));
    }
    
    // COMMAND: @LOADGAME
    //            Loads the current game from the given path
    //
    COMMAND( OnLoadGame ) 
    {
        std::string filename = (LPCSTR)argv[0];
        return mx->LoadGame ( filename,  (PFNSERIALIZE)((void*)argv[1]) );
    }

    // COMMAND: @DESCRIPTION
    //            Loads the description from the given game
    //
    COMMAND( OnGameDescription )
    {
        static std::string descripton;
        std::string filename = (LPCSTR)argv[0];
        mx->SaveGameDescription(filename, descripton );
        argv[1] = descripton;
        return MX_OK;
    }

    COMMAND( OnLoadDiscoveryMap )
    {
        std::string filename = (LPCSTR)argv[0];
        return mx->LoadDiscoveryMap ( filename );
    }
    
    COMMAND( OnSaveDiscoveryMap )
    {
        std::string filename = (LPCSTR)argv[0];
        return mx->SaveDiscoveryMap ( filename );
    }

    // COMMAND: @SETSCENARIO
    //            Sets the scenario for the engine to use
    //
    COMMAND( OnSetMap )
    {
        mxmap* map = (mxmap*)argv[0].vPtr;
        if ( map == NULL )
            return MX_FAILED ;
        
        argv[0]=(s32)0;
        
        SAFEDELETE(mx->gamemap);
        
        mx->gamemap = map;
        mx->gamemap->CalculateVisibleArea();
        mx->gamemap->ClearVisible();

        return MX_OK;
    }

    // COMMAND: @SETDIFFICULTY
    //
    COMMAND( OnSetDifficulty )
    {
        mx->Difficulty((mxdifficulty_t)(u32)argv[0]);
        return MX_OK;
    }

    // COMMAND: @GETDIFFICULTY
    //
    COMMAND( OnGetDifficulty )
    {
        argv[0]=(u32)mx->Difficulty();
        return MX_OK;
    }

    // COMMAND: @GETSCENARIOFEATURES
    //
    COMMAND( OnGetScenarioFeatures )
    {
        argv[0]=(u32)mx->scenario->features;
        return MX_OK;
    }

    static mxcommand_t mx_commands[] = {
        {"@SETSCENARIO",                1, OnSetScenario,           {variant::vptr} },
        {"@SETDATABASEDIRECTORY",       1, OnSetDatabaseDirectory,  {variant::vstring} },
        {"@INIT",                       2, OnInit,                  {variant::vuint64,variant::vnumber} },
        {"@DEINIT",                     0, OnDeInit },
        {"@LOAD",                       2, OnLoadGame,              {variant::vstring,variant::vptr} },
        {"@SAVE",                       2, OnSaveGame,              {variant::vstring,variant::vptr} },
        {"@DESCRIPTION",                1, OnGameDescription,       {variant::vstring} },
        {"@LOADDISCOVERYMAP",           1, OnLoadDiscoveryMap,      {variant::vstring} },
        {"@SAVEDISCOVERYMAP",           1, OnSaveDiscoveryMap,      {variant::vstring} },
    
        {"@SETMAP",                     1, OnSetMap,                {variant::vptr} },
        {"@SETDIFFICULTY",              1, OnSetDifficulty,         {variant::vnumber} },
        {"@GETDIFFICULTY",              0, OnGetDifficulty},
        {"@GETSCENARIOFEATURES",        0, OnGetScenarioFeatures},
    };

    //
    // METHOD:    Command
    //            Runs a command inside the engine
    // 
    // PARAMS:    arg                Command Name
    // 
    // REMARKS:    If the command does not exist inside the engine, it is passed to the current scenario
    //
    // RETURNS:    MXRESULT
    //
    MXRESULT mxinterface::Command ( const std::string& arg, variant* argv, u32 argc )
    {
        MXRESULT result = mx->ProcessCommand ( mx_commands, NUMELE(mx_commands), arg, argv, argc );
        if ( result != MX_UNKNOWN )
            return result ;

        if ( mx->scenario )
            return mx->scenario->Command(arg,argv,argc);

        return MX_UNKNOWN ;
    }


    //
    // METHOD:    GetEntityProperty
    //            Returns a property value from a given Entity
    // 
    // PARAMS:    id                Entity
    //            arg                Property Name
    // [out]    argv            Property Value is returned
    // 
    // REMARKS    
    //
    // RETURNS    MXRESULT
    //
    
    MXRESULT mxinterface::GetEntityProperty( mxid id, const std::string& arg, variant& argv) const
    {
        //character
        if ( ID_TYPE(id) == IDT_CHARACTER ) {

            CONVERT_CHARACTER_ID(id,character);

            if ( ISARG("ISALIVE") ) {
                argv = (s32)character->IsAlive();
                return MX_OK ;
            }

            if ( ISARG("ISINBATTLE") ) {
                argv = (s32)character->IsInBattle();
                return MX_OK ;
            }

            if ( ISARG("ISRECRUITED") ) {
                argv = (s32)character->IsRecruited();
                return MX_OK ;
            }

            if ( ISARG("ISRESTING") ) {    
                argv = (s32)character->IsResting();
                return MX_OK ;
            }

            if ( ISARG("CANFIGHT") ) {
                auto locinfo = character->GetLocInfo() ;
                mxobject* obj = mx->ObjectById(locinfo->fightthing);
                argv = (s32)character->CheckFightObject( obj );
                return MX_OK ;
            }
            
            if ( ISARG("ISALLOWEDHIDE") ) {    
                argv = (s32)character->IsAllowedHide();
                return MX_OK ;
            }

            if ( ISARG("ISHIDDEN") ) {
                argv = (s32)character->IsHidden();
                return MX_OK ;
            }

            if ( ISARG("ISRIDING") ) {
                argv = (s32)character->IsRiding();
                return MX_OK ;
            }

            if ( ISARG("CANFOLLOW") ) {
                CONVERT_CHARACTER_ID(argv.vSInt32,leader);
                argv = (s32)character->CanFollow(leader);
                return MX_OK ;
            }
            
            if ( ISARG("ISCONTROLLABLE") ) {
                argv = (s32)mx->scenario->CanWeSelectCharacter(character);
                return MX_OK ;
            }

            if ( ISARG("LOCATIONOBJECT") ) {
                argv = (s32)character->LocationThing();
                return MX_OK ;
            }

            if ( ISARG("HASBATTLEINFO") ) {
                argv = (s32)character->HasBattleInfo();
                return MX_OK ;
            }

            // armies/units
            if ( ISARG("ARMIES") ) {
                c_mxid& collection= (c_mxid&) *((c_mxid*)argv.vPtr) ;
                collection.Create(2);
                collection[0] =  MAKE_ARMYID((128|UT_WARRIORS),mxentity::SafeId(character)) ;
                collection[1] =  MAKE_ARMYID((128|UT_RIDERS),mxentity::SafeId(character)) ;
                return MX_OK ;
            }

            if ( ISARG("FOLLOWERS") ) {
                c_mxid& collection= (c_mxid&) *((c_mxid*)argv.vPtr) ;
                auto chars = mx->scenario->GetCharacterFollowers(character);
                chars.CreateIdtCollection( collection );
                return MX_OK ;
            }
        }

        return MX_UNKNOWN ;
    }


    //
    // METHOD:    GetEntityProperties
    //            Returns an array of values based on an Entity Property Name
    // 
    // PARAMS:    id                Entity Id
    //            arg                Property Name
    // [out]    argv            Array to hold returned values
    //            argc            Size of array
    // 
    // REMARKS:    Array must be large enough to take the result + 1
    //            First item in the array will always contain the number of returned values
    //
    // RETURNS:    MXRESULT
    //
    MXRESULT mxinterface::GetEntityProperties ( mxid id, const std::string& arg, variant argv[], u32 argc )
    {
        if ( ID_TYPE(id) == IDT_CHARACTER ) {

            CONVERT_CHARACTER_ID(id,character);

            // characters location info
            if ( ISARG("LOCATIONINFO") ) {
                if ( argc < 17 )
                    return MX_INVALID_ARGUMENT_COUNT;
                
                auto locinfo = character->GetLocInfo() ;
                
                argv[0] = (s32)17;
                argv[1] = (s32)locinfo->looking ;
                argv[2] = (s32)MAKE_LOCID (locinfo->loc_lookingat.x,locinfo->loc_lookingat.y) ;
                argv[3] = (s32)MAKE_LOCID (locinfo->loc_infront.x,locinfo->loc_infront.y) ;
                argv[4] = locinfo->flags ;
                argv[5] = MAKE_ID(IDT_OBJECT,locinfo->fightthing) ;

                argv[6] = locinfo->foe.warriors ;
                argv[7] = locinfo->foe.riders ;
                argv[8] = locinfo->friends.warriors ;
                argv[9] = locinfo->friends.riders ;
                argv[10] = locinfo->regiments.warriors ;
                argv[11] = locinfo->regiments.riders ;

                argv[13] = mxentity::SafeIdt(locinfo->stubborn_follower_battle) ;
                argv[14] = mxentity::SafeIdt(locinfo->stubborn_follower_move) ;
#if defined(_DDR_)
                argv[16] = mxentity::SafeIdt(locinfo->someone_to_give_to) ;
                argv[17] = mxentity::SafeIdt(locinfo->object_to_take) ;
#else
                argv[16] = IDT_NONE ;
                argv[17] = IDT_NONE ;
#endif


                if ( locinfo->mapsqr.IsTunnelObject()) {
                    argv[15] = MAKE_ID(IDT_OBJECT,locinfo->mapsqr.object) ;
                    argv[12] = MAKE_ID(IDT_OBJECT,OB_NONE) ;
                }else{
                    argv[12] = MAKE_ID(IDT_OBJECT,locinfo->mapsqr.object) ;
                    argv[15] = MAKE_ID(IDT_OBJECT,OB_NONE) ;
                }
                
                return MX_OK ;
            }
        }
        
        if ( ID_TYPE(id) == IDT_LOCATION ) {

            // location info
            if ( ISARG("LOCATIONINFO") || ISARG("TUNNELINFO") ) {
                
                if ( argc < 10 )
                    return MX_INVALID_ARGUMENT_COUNT;
                
                mxgridref loc( GET_LOCIDX(id), GET_LOCIDY(id) );

                flags32_t f = slf_none;
                if (ISARG("TUNNELINFO")) {
                    f = slf_tunnel;
                }

                std::unique_ptr<mxlocinfo> locinfo(new mxlocinfo( loc, nullptr, f ));
        
                argv[0] = (s32)10;
                argv[1] = locinfo->flags ;
                argv[2] = MAKE_ID(IDT_OBJECT,locinfo->mapsqr.object) ;
                argv[3] = locinfo->foe.warriors ;
                argv[4] = locinfo->foe.riders ;
                argv[5] = locinfo->friends.warriors ;
                argv[6] = locinfo->friends.riders ;
                argv[7] = locinfo->regiments.warriors ;
                argv[8] = locinfo->regiments.riders ;
                        

                if ( locinfo->mapsqr.IsTunnelObject()) {
                    argv[10] = MAKE_ID(IDT_OBJECT,locinfo->mapsqr.object) ;
                    argv[9] =  MAKE_ID(IDT_OBJECT,OB_NONE) ;
                }else{
                    argv[9] = MAKE_ID(IDT_OBJECT,locinfo->mapsqr.object) ;
                    argv[10] = MAKE_ID(IDT_OBJECT,OB_NONE) ;
                }
            
                return MX_OK ;
            }
            
        }

        return MX_UNKNOWN ;
    }

}
// namespace tme
