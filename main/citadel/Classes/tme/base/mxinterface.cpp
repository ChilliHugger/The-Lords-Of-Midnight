
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
#include <string.h>

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
	// METHOD:	GetProperty
	//			Returns an engine variable value
	// 
	// PARAMS:	name			Property Name
	// [out]	arg				Property Value
	// 
	// REMARKS:	
	//
	// RETURNS:	MXRESULT
	//
	MXRESULT mxinterface::GetProperty( const string& name, variant& arg) const 
	{
		return variables::GetProperty(name, arg);
	}

	//
	// METHOD:	EntityByName
	//			Returns the Entity Id reference by the name symbol for the given type
	// 
	// PARAMS:	name			Entity Symbol Name
	//			type			Entity Type
	// 
	// REMARKS:	
	//
	// RETURNS:	idt
	//
	mxid mxinterface::EntityByName( const string& name, id_type_t type )
	{
		mxentity* obj  = mx->EntityByName( name, type );
		if ( obj ) {
			return obj->SafeIdt();
		}
		return mx->text->StringByName(name);
	}

	//
	// METHOD:	EntityLinkData
	//			Attaches user data to and Entity
	// 
	// PARAMS:	Id				Entity Id
	//			data			Data to attach to entity
	// 
	// REMARKS:	Data is not used by engine
	//
	// RETURNS:	MXRESULT
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
	// METHOD:	EntityById
	//			Fills and Entity structure for the given Entity Id
	// 
	// PARAMS:	Id				Entity Id
	//			info			Entity Structure
	// 
	// REMARKS:	
	//
	// RETURNS:	MXRESULT
	//
	MXRESULT mxinterface::EntityById( mxid id, info_t* info )
	{
	mxentity*	obj = NULL ;

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
				//	return MX_FAILED ;

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
#if defined(_DDR_)
                if ( m.IsTunnelPassageway() )
                    out->object_tunnel = m.object ? MAKE_ID(IDT_OBJECT,m.object) : NONE ;
                else
#endif
                    out->object = m.object ? MAKE_ID(IDT_OBJECT,m.object) : NONE ;
                    
				out->area = MAKE_ID(IDT_AREAINFO,m.area);
				out->climate = m.climate ;
				out->density = mx->gamemap->Density(loc);

				out->flags.Set( m.flags );
                out->discovery_flags.Clear();
                
                if ( mx->discoverymap != NULL ) {
                    out->discovery_flags.Set( mx->discoverymap->GetAt(loc) );
                }

#if defined(_DDR_)
				if ( m.HasTunnelExit() )
					out->flags.Set(lf_tunnel_exit);
				if ( m.HasTunnelEntrance() )
					out->flags.Set(lf_tunnel_entrance);
                if ( m.IsTunnelPassageway() )
                    out->flags.Set(lf_tunnel_passageway);
                    
                if ( m.HasObject() ) {
                    mxobject* obj = mx->scenario->FindObjectAtLocation(loc);
                    if ( obj && obj->IsSpecial() )
                        out->flags.Set(lf_object_special);
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
	// METHOD:	GetProperties
	//			Returns values for a scenario property
	// 
	// PARAMS:	arg				Property Name
	// [out]	argv			Array to hold returned values
	//			argc			Size of array
	// 
	// REMARKS:	Array must be large enough to take the result + 1
	//			First item in the array will always contain the number of returned values
	//
	// RETURNS:	MXRESULT
	//
	MXRESULT mxinterface::GetProperties ( const string& arg, variant argv[], u32 argc )
	{
		return mx->scenario->GetProperties(arg,argv,argc);
	}

	//
	// METHOD:	Text
	//			Returns a series of text items for a scenario text command
	// 
	// PARAMS:	arg				Text Item name
	// [out]	argv			Array to hold returned values
	//			argc			Size of array
	// 
	// REMARKS:	Array must be large enough to take the result + 1
	//			First item in the array will always contain the number of returned values
	//
	// RETURNS:	MXRESULT
	//	
	MXRESULT mxinterface::Text ( const string& arg, variant* argv, u32 argc )
	{
		return mx->scenario->Text(arg,argv,argc);
	}

	//
	// METHOD:	ProcessCommand
	//			Runs a command inside the engine
	// 
	// PARAMS:	tblCommand		Table of Commands to check against
	//			max				Number of commands in the table
	//			arg				Name of Command to run
	//			argv			Input Arguments	
	//			argc			Input Argument Count
	// 
	// REMARKS:	This is an internal function and is not exposed to the interface
	//
	// RETURNS:	MXRESULT
	//
	MXRESULT mxengine::ProcessCommand ( mxcommand_t tblCommand[], u32 max, const string& arg, variant argv[], u32 argc )
	{
	MXRESULT	status = MX_UNKNOWN;

		if ( arg.Length() == 0 ) {
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
	//			Sets the scenario for the engine to use
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
	//			Tells the engine where to find the scenario data
	//
	COMMAND( OnSetDatabaseDirectory ) 
	{
		MXRESULT result = mx->SetDatabaseDirectory(argv[0]);
		//argv[0]=0;
		return result;
	}

	// COMMAND: @INIT
	//			Loads the scenario database
	//
	COMMAND( OnInit ) 
	{
		//argv[0]=0;
		return mx->LoadDatabase();
	}

	// COMMAND: @DEINIT
	//			Unloads the scenario database
	//
	COMMAND( OnDeInit ) 
	{
		//argv[0]=0;
		return mx->UnloadDatabase();
	}
	
	// COMMAND: @SAVEGAME
	//			Saves the current game to the given path
	//
	COMMAND( OnSaveGame ) 
	{
		//argv[0]=0;
		return mx->SaveGame ( argv[0], (PFNSERIALIZE)((void*)argv[1]));
	}
	
	// COMMAND: @LOADGAME
	//			Loads the current game from the given path
	//
	COMMAND( OnLoadGame ) 
	{
		//argv[0]=0;
		return mx->LoadGame ( argv[0],  (PFNSERIALIZE)((void*)argv[1]) );
	}

	// COMMAND: @DESCRIPTION
	//			Loads the description from the given game
	//
	COMMAND( OnGameDescription )
	{
		//argv[0]=0;
        string& temp = *((string*)argv[1].vPtr) ;
		return mx->SaveGameDescription(argv[0], temp );
	}

    COMMAND( OnLoadDiscoveryMap )
    {
        return mx->LoadDiscoveryMap ( argv[0] );
    }
    
    COMMAND( OnSaveDiscoveryMap )
    {
        return mx->SaveDiscoveryMap ( argv[0] );
    }
    
    static mxcommand_t mx_commands[] = {
        {"@SETSCENARIO",			1, OnSetScenario,			{variant::vptr} },
        {"@SETDATABASEDIRECTORY",	1, OnSetDatabaseDirectory, 	{variant::vstring} },
        {"@INIT",					0, OnInit},
        {"@DEINIT",					0, OnDeInit },
        {"@LOAD",					2, OnLoadGame,				{variant::vstring,variant::vptr} },
        {"@SAVE",					2, OnSaveGame,				{variant::vstring,variant::vptr} },
        {"@DESCRIPTION",			2, OnGameDescription,		{variant::vstring,variant::vptr} },
        {"@LOADDISCOVERYMAP",       1, OnLoadDiscoveryMap,      {variant::vstring} },
        {"@SAVEDISCOVERYMAP",       1, OnSaveDiscoveryMap,      {variant::vstring} },
    };

	//
	// METHOD:	Command
	//			Runs a command inside the engine
	// 
	// PARAMS:	arg				Command Name
	// 
	// REMARKS:	If the command does not exist inside the engine, it is passed to the current scenario
	//
	// RETURNS:	MXRESULT
	//
	MXRESULT mxinterface::Command ( const string& arg, variant* argv, u32 argc )
	{
		MXRESULT result = mx->ProcessCommand ( mx_commands, NUMELE(mx_commands), arg, argv, argc );
		if ( result != MX_UNKNOWN )
			return result ;

		if ( mx->scenario )
			return mx->scenario->Command(arg,argv,argc);

		return MX_UNKNOWN ;
	}


	//
	// METHOD:	GetEntityProperty
	//			Returns a property value from a given Entity
	// 
	// PARAMS:	id				Entity
	//			arg				Property Name
	// [out]	argv			Property Value is returned
	// 
	// REMARKS	
	//
	// RETURNS	MXRESULT
	//
	
	MXRESULT mxinterface::GetEntityProperty( mxid id, const string& arg, variant& argv) const 
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
				mxlocinfo* locinfo = character->GetLocInfo();
				mxobject* obj = mx->ObjectById(locinfo->fightthing);
				argv = (s32)character->CheckFightObject( obj );
				SAFEDELETE ( locinfo );
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
                CONVERT_CHARACTER_ID(argv.vInt,leader);
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
				collection[0] =  MAKE_ARMYID((128|UT_WARRIORS),character->SafeId()) ;
				collection[1] =  MAKE_ARMYID((128|UT_RIDERS),character->SafeId()) ;
				return MX_OK ;
			}

			if ( ISARG("FOLLOWERS") ) {
				c_mxid& collection= (c_mxid&) *((c_mxid*)argv.vPtr) ;
                entities chars;
                mx->scenario->GetCharacterFollowers(character, chars);
                chars.CreateIdtCollection( collection );
				return MX_OK ;
			}
        }

		return MX_UNKNOWN ;
	}


	//
	// METHOD:	GetEntityProperties
	//			Returns an array of values based on an Entity Property Name
	// 
	// PARAMS:	id				Entity Id
	//			arg				Property Name
	// [out]	argv			Array to hold returned values
	//			argc			Size of array
	// 
	// REMARKS:	Array must be large enough to take the result + 1
	//			First item in the array will always contain the number of returned values
	//
	// RETURNS:	MXRESULT
	//
	MXRESULT mxinterface::GetEntityProperties ( mxid id, const string& arg, variant argv[], u32 argc )
	{
		if ( ID_TYPE(id) == IDT_CHARACTER ) {

			CONVERT_CHARACTER_ID(id,character);

			// characters location info
			if ( ISARG("LOCATIONINFO") ) {
			
#if defined(_DDR_)
				if ( argc < 16 )
					return MX_INVALID_ARGUMENT_COUNT;
#endif
#if defined(_LOM_)
                if ( argc < 16 )
                    return MX_INVALID_ARGUMENT_COUNT;
#endif
				
				mxlocinfo* locinfo = character->GetLocInfo();

				argv[0] = (s32)14;
				argv[1] = (s32)locinfo->looking ;
				argv[2] = (s32)MAKE_LOCID (locinfo->loc_lookingat.x,locinfo->loc_lookingat.y) ;
				argv[3] = (s32)MAKE_LOCID (locinfo->loc_infront.x,locinfo->loc_infront.y) ;
				argv[4] = locinfo->flags ;
				argv[5] = MAKE_ID(IDT_OBJECT,locinfo->fightthing) ;

				argv[6] = locinfo->foe.warriors ;
				argv[7] = locinfo->foe.riders ;
				argv[8] = locinfo->friends.warriors ;
				argv[9] = locinfo->friends.riders ;
				argv[10] = locinfo->doomdark.warriors ;
				argv[11] = locinfo->doomdark.riders ;

                argv[13] = locinfo->stubborn_follower_battle->SafeIdt() ;
                argv[14] = locinfo->stubborn_follower_move->SafeIdt() ;
#if defined(_DDR_)
                argv[16] = locinfo->someone_to_give_to->SafeIdt() ;
                if ( locinfo->mapsqr.IsTunnelPassageway()) {
                    argv[15] = MAKE_ID(IDT_OBJECT,locinfo->mapsqr.object) ;
                    argv[12] =  MAKE_ID(IDT_OBJECT,OB_NONE) ;
				}else
#endif
                {
                    argv[12] = MAKE_ID(IDT_OBJECT,locinfo->mapsqr.object) ;
                    argv[15] = MAKE_ID(IDT_OBJECT,OB_NONE) ;
				}
                
				SAFEDELETE ( locinfo );

				return MX_OK ;
			}
		}
		
		if ( ID_TYPE(id) == IDT_LOCATION ) {

			// location info
			if ( ISARG("LOCATIONINFO") ) {
				
				if ( argc < 10 )
					return MX_INVALID_ARGUMENT_COUNT;
				
				mxgridref loc( GET_LOCIDX(id), GET_LOCIDY(id) );

				mxlocinfo* locinfo = new mxlocinfo( loc, NULL, slf_none )  ;
		
				argv[0] = (s32)9;
				argv[1] = locinfo->flags ;
				argv[2] = MAKE_ID(IDT_OBJECT,locinfo->mapsqr.object) ;

				
				
				argv[3] = locinfo->foe.warriors ;
				argv[4] = locinfo->foe.riders ;
				argv[5] = locinfo->friends.warriors ;
				argv[6] = locinfo->friends.riders ;
				argv[7] = locinfo->doomdark.warriors ;
				argv[8] = locinfo->doomdark.riders ;
				        
#if defined(_DDR_)
                if ( locinfo->mapsqr.IsTunnelPassageway()) {
                    argv[10] = MAKE_ID(IDT_OBJECT,locinfo->mapsqr.object) ;
                    argv[9] =  MAKE_ID(IDT_OBJECT,OB_NONE) ;
				}else
#endif
                {
                    argv[9] = MAKE_ID(IDT_OBJECT,locinfo->mapsqr.object) ;
                    argv[10] = MAKE_ID(IDT_OBJECT,OB_NONE) ;
				}
				SAFEDELETE( locinfo );
			
				return MX_OK ;
			}
			
		}

		return MX_UNKNOWN ;
	}

}
// namespace tme
