

#include "tme_interface.h"


mxinterface*	mxi ;
character		cur_Character ;

c_mxid			default_characters ;
c_mxid			recruitable_characters;

c_mxid			location_characters;
u32				location_recruited;
c_mxid			location_strongholds;
mxid			location_infrontid;
mxid			location_lookingatid;
loc_t			location_infront;
loc_t			location_lookingat;
flags32			location_flags;
mxid			location_fightthing;
mxid			location_object;

stronghold		location_stronghold;

mxid			location_stubborn_lord_attack;
mxid			location_stubborn_lord_move;

c_mxid			location_infront_strongholds;
mxid			location_infront_object;

loc_armyinfo_t	location_infront_armies;
loc_armyinfo_t	location_armies;

#if defined(_DDR_)
mxid            location_object_tunnel;
mxid            location_someone_to_give_to;
#endif

string			tme_text ;
std::string          tme_path;
static			variant args[20];



#if defined(_LOM_)
LPCSTR TME_ScenarioDirectory ( void )
{
    tme_path = cocos2d::FileUtils::getInstance()->fullPathForFilename( "lom" ) ;
    return tme_path.c_str();
}

LPCSTR TME_ScenarioName ( void )
{
	return "The Lords of Midnight" ; 
}
#endif

#if defined(_DDR_)
LPCSTR TME_ScenarioDirectory ( void )
{
	return "ddr";
}

LPCSTR TME_ScenarioName ( void )
{
	return "Doomdark's Revenge" ;
}
#endif

LPCSTR TME_GetSymbol( mxid id )
{
    return mxi->EntitySymbolById(id);
}

mxid TME_LinkData ( LPCSTR symbol, void* data )
{
	mxid id = mxi->EntityByName(symbol);
	mxi->EntityLinkData( id, data );
	return id ;
}

size TME_MapSize ( void )
{
	if ( MXSUCCESS( mxi->GetProperties ( "MAPSIZE", args, 0 ) ) ) {
		return size((s32)args[1],(s32)args[2]);
	}
	return size(0,0);
}

BOOL TME_MapInfo ( MapInfo_t* info ) 
{    
    if ( info == NULL )
        return false;
    
	if ( MXSUCCESS( mxi->GetProperties ( "MAPINFO", args, 0 ) ) ) {
		if ( (s32)args[0] < 6 )
            return false;
        
        info->top.x = (s32)args[1];
        info->top.y = (s32)args[2];
        info->bottom.x = (s32)args[3];
        info->bottom.y = (s32)args[4];
        info->size = size((s32)args[5], (s32)args[6]);
        
        return true;
	}
	return false;
}


void TME_PurgeTextCache()
{
	if ( MXSUCCESS( mxi->Text( "Purge", args, 0 ) ) ) {
		//return (LPSTR)args[1];
	}
}

LPCSTR TME_GetNumber(int number)
{
	args[0] = (s32)number ;
	return TME_GetText("NumberPart",args,1);
}

LPCSTR TME_LastActionMsg()
{
	return TME_GetText("LastActionMsg");
}

LPCSTR TME_GetCharacterText ( const character& c, const string& command ) 
{
	args[0] = c.id ;
	return TME_GetText(command,args,1);
}

LPCSTR TME_GetSystemString ( const character& c, int msg ) 
{
	args[0] = MAKE_ID(IDT_STRING,msg) ;
	args[1] = c.id ;
	return TME_GetText("SpecialStrings",args,2);
}

LPCSTR TME_GetText ( const string& command ) 
{
	return TME_GetText(command,args,0);
}

LPCSTR TME_GetText ( const string& command, variant args[], int count ) 
{
	CStrBuf cache;
	
	if ( MXSUCCESS( mxi->Text( command, args, count ) ) ) {
		tme_text = (LPSTR)args[1];
	}else
		tme_text = "";
	
	_ASSERTE(CDebug::CheckMemory());
	return tme_text ;
}

LPCSTR TME_GetLocationText ( mxgridref loc )
{
	args[0] = MAKE_LOCID(loc.x,loc.y) ;
	return TME_GetText( "Location", args, 1 );
}

BOOL TME_GetLocation( maplocation& out, mxid id )
{
	CLEARINFOSTRUCT ( out, IDT_LOCATION );
	if ( MXSUCCESS(mxi->EntityById ( id, &out )) )
		return TRUE;
	return FALSE;
}

BOOL TME_GetLocation( maplocation& out, loc_t loc )
{
	return TME_GetLocation(out,MAKE_LOCID(loc.x,loc.y));
}


LPCSTR TME_GetMapLocationText ( mxgridref loc )
{
	args[0] = MAKE_MAPLOCID(loc.x,loc.y) ;
	return TME_GetText( "MapLocation", args, 1 );
}

BOOL TME_GetMapLocation( maplocation& out, mxid id )
{
	CLEARINFOSTRUCT ( out, IDT_MAPLOCATION );
	if ( MXSUCCESS(mxi->EntityById ( id, &out )) )
		return TRUE;
	return FALSE;
}

BOOL TME_GetMapLocation( maplocation& out, loc_t loc )
{
	return TME_GetLocation(out,MAKE_MAPLOCID(loc.x,loc.y));
}


BOOL TME_GetCharacter( character& out, mxid id )
{
	CLEARINFOSTRUCT ( out, IDT_CHARACTER );
	if ( MXSUCCESS(mxi->EntityById ( id, &out )) )
		return TRUE;
	return FALSE;
}

BOOL TME_GetRegiment( regiment& out, mxid id )
{
	CLEARINFOSTRUCT ( out, IDT_REGIMENT );
	if ( MXSUCCESS(mxi->EntityById ( id, &out )) )
		return TRUE;
	return FALSE;
}

BOOL TME_GetArmy( army_t& out, mxid id )
{
	CLEARINFOSTRUCT ( out, IDT_ARMY );
	if ( MXSUCCESS(mxi->EntityById ( id, &out )) )
		return TRUE;
	return FALSE;
}


BOOL TME_GetStronghold( stronghold& out, mxid id )
{
	CLEARINFOSTRUCT ( out, IDT_STRONGHOLD );
	if ( MXSUCCESS(mxi->EntityById ( id, &out )) )
		return TRUE;
	return FALSE;
}

BOOL TME_GetTerrainInfo( terraininfo& out, mxid id )
{
	CLEARINFOSTRUCT ( out, IDT_TERRAININFO );
	if ( MXSUCCESS(mxi->EntityById ( id, &out )) )
		return TRUE;
	return FALSE;
}

BOOL TME_GetRaceInfo( raceinfo& out, mxid id )
{
	CLEARINFOSTRUCT ( out, IDT_RACEINFO );
	if ( MXSUCCESS(mxi->EntityById ( id, &out )) )
		return TRUE;
	return FALSE;
}

BOOL TME_GetObject( anobject& out, mxid id )
{
	CLEARINFOSTRUCT ( out, IDT_OBJECT );
	if ( MXSUCCESS(mxi->EntityById ( id, &out )) )
		return TRUE;
	return FALSE;
}

BOOL TME_GetUnitInfo( unitinfo& out, mxid id )
{
	CLEARINFOSTRUCT ( out, IDT_UNITINFO );
	if ( MXSUCCESS(mxi->EntityById ( id, &out )) )
		return TRUE;
	return FALSE;
}

character& TME_CurrentCharacter ( void )
{
	return cur_Character; 
}

character& TME_RefreshCurrentCharacter ( void )
{
	TME_GetCharacter(cur_Character,cur_Character.id);
	return cur_Character; 
}

character& TME_CurrentCharacter ( mxid characterid )
{
	args[0] = characterid ;
	mxi->Command("CONTROL", args, 1);
	TME_GetCharacter(cur_Character,characterid);
	return cur_Character; 
}

s32 TME_GetAllRegiments (c_mxid& collection )
{
	args[0] = &collection ;
	mxi->GetProperties ( "REGIMENTS", args, 1 );
	return collection.Count();
}

s32 TME_GetAllStrongholds (c_mxid& collection )
{
    args[0] = &collection ;
    mxi->GetProperties ( "STRONGHOLDS", args, 1 );
    return collection.Count();
}

s32 TME_GetAllCharacters (c_mxid& collection )
{
	args[0] = &collection ;
	mxi->GetProperties ( "CHARACTERS", args, 1 );
	return collection.Count();
}

s32 TME_GetCharacters ( mxid id, c_mxid& collection, u32& recruited )
{
	args[0] = &collection ;
	args[1] = id ;
	mxi->GetProperties ( "CHARACTERS", args, 2 );
	recruited = args[1];
	return collection.Count();
}

s32 TME_GetFollowers ( mxid id, c_mxid& collection )
{
	args[0] = &collection ;
	mxi->GetEntityProperty ( id, "FOLLOWERS", args[0] );
	return collection.Count();
}


s32 TME_GetCharactersAtLocation ( loc_t loc, c_mxid& collection, BOOL showall )
{
	return TME_GetCharactersAtLocation ( MAKE_LOCID(loc.x,loc.y), collection, showall, FALSE );
}

s32 TME_GetCharactersAtLocation ( mxid id, c_mxid& collection, BOOL showall, BOOL showtunnel )
{
	args[0] = &collection ;
	args[1] = id ;
#if defined(_LOM_)
	args[2] = (s32)(showall?slf_all:slf_none);
#endif
#if defined(_DDR_)
	args[2] = (s32)(showall?slf_all:slf_none)|(s32)(showtunnel?slf_tunnel:slf_none);
#endif
    
	mxi->GetProperties ( "CharsAtLoc", args, 3 );
	return collection.Count();
}



MXRESULT TME_GetArmiesAtLocation( loc_t loc, u32& enemies, u32& friends)
{
	return TME_GetArmiesAtLocation( MAKE_LOCID(loc.x,loc.y), enemies, friends );
}

MXRESULT TME_GetArmiesAtLocation( mxid loc, u32& enemies, u32& friends)
{
	args[0] = loc ;
	enemies = 0;
	friends = 0;
	if ( MXSUCCESS(mxi->GetProperties ( "ArmiesAtLocation", args, 1 )) ) {
		enemies = args[1];
		friends = args[2];
		return MX_OK;
	}
	return MX_FAILED;
}


BOOL TME_GetLocationInfo( loc_t loc )
{
	location_lookingatid = IDT_NONE ;
	location_infrontid = IDT_NONE ;
	location_flags.Clear() ;
	location_fightthing = NONE;
	location_lookingat.x = 0;
	location_lookingat.y = 0;
	location_infront.x = 0;
	location_infront.y = 0;
	location_infront_object = NONE ;
	location_object= NONE ;
#if defined(_DDR_)
    location_object_tunnel = NONE ;
#endif
    location_stubborn_lord_attack=IDT_NONE;
    location_stubborn_lord_move=IDT_NONE;
    
	location_armies.foe_warriors = 0;
	location_armies.foe_riders = 0;
	location_armies.friends_warriors = 0;
	location_armies.friends_riders = 0;
	location_armies.regiment_warriors = 0;
	location_armies.regiment_riders = 0;
	
	
    
    
	//
	// Get the info for this location
	//
	if ( MXSUCCESS( mxi->GetEntityProperties ( MAKE_LOCID(loc.x,loc.y), "LOCATIONINFO", args, 11 ) ) ) {

		location_flags.Set(args[1]);
		location_fightthing = args[2].vInt;
		
		location_armies.foe_warriors = args[3];
		location_armies.foe_riders = args[4];
		location_armies.friends_warriors = args[5];
		location_armies.friends_riders = args[6];
		location_armies.regiment_warriors = args[7];
		location_armies.regiment_riders = args[8];
		
		location_object= args[9].vInt; ;
#if defined(_DDR_)
        location_object_tunnel= args[10].vInt;
#endif
    
        
		return true;
		
	}
	
	
	return false;
}


BOOL TME_GetCharacterLocationInfo ( const character& c )
{
	location_lookingatid = IDT_NONE ;
	location_infrontid = IDT_NONE ;
	location_flags.Clear() ;
	location_fightthing = NONE;
	location_lookingat.x = 0;
	location_lookingat.y = 0;
	location_infront.x = 0;
	location_infront.y = 0;
	location_infront_object = NONE ;
	location_object= NONE ;
    location_stubborn_lord_attack=IDT_NONE;
    location_stubborn_lord_move=IDT_NONE;
    
#if defined(_DDR_)
    location_object_tunnel = NONE ;
    location_someone_to_give_to=IDT_NONE;
#endif
    
	location_armies.foe_warriors = 0;
	location_armies.foe_riders = 0;
	location_armies.friends_warriors = 0;
	location_armies.friends_riders = 0;
	location_armies.regiment_warriors = 0;
	location_armies.regiment_riders = 0;
	
	//
	// Get the characters info for this location
	//
#if defined(_DDR_)
    int args_count = 17 ;
#endif
#if defined(_LOM_)
    int args_count = 16 ;
#endif
    
	if ( MXSUCCESS( mxi->GetEntityProperties ( c.id, "LOCATIONINFO", args, args_count ) ) ) {
		// dir_t			argv[1]		direction looking
		location_lookingatid = args[2];
		location_infrontid = args[3];

		location_flags.Set(args[4]);
		location_fightthing = args[5].vInt;

		location_lookingat.x = GET_LOCIDX(location_lookingatid);
		location_lookingat.y = GET_LOCIDY(location_lookingatid);
		location_infront.x = GET_LOCIDX(location_infrontid);
		location_infront.y = GET_LOCIDY(location_infrontid);
		
		location_armies.foe_warriors = args[6];
		location_armies.foe_riders = args[7];
		location_armies.friends_warriors = args[8];
		location_armies.friends_riders = args[9];
		location_armies.regiment_warriors = args[10];
		location_armies.regiment_riders = args[11];
		
		location_object= args[12].vInt;
        
        location_stubborn_lord_attack=args[13].vInt;
        location_stubborn_lord_move=args[14].vInt;
        
#if defined(_DDR_)
        location_object_tunnel = args[15].vInt;
        location_someone_to_give_to = args[16].vInt;
#endif
        
	}
	
	// all characters here
	TME_GetCharacters( c.id, location_characters, location_recruited );
	
	// characters that could be recruited
	args[0] = &recruitable_characters ;
	args[1] = c.id ;
	mxi->GetProperties ( "RECRUITABLE", args, 2 );
	
	// strongholds
	args[0] = &location_strongholds ;
	args[1] = c.id ;
	mxi->GetProperties ( "STRONGHOLDS", args, 2 );
	
	CLEARINFOSTRUCT ( location_stronghold, IDT_STRONGHOLD );
	if ( location_strongholds.Count() ) {
		TME_GetStronghold ( location_stronghold, location_strongholds[0] );
	}
	
	// strongholds in front
	args[0] = &location_infront_strongholds ;
	args[1] = location_infrontid ;
	mxi->GetProperties ( "STRONGHOLDS", args, 2 );
	
	//
	maplocation loc;
	TME_GetLocation ( loc, location_infrontid );
	location_infront_object = loc.object;
	
	//
	// Get the characters info for infront location
	//
	TME_GetArmies ( location_infrontid, &location_infront_armies );
	
	
	return TRUE;
}

LPCSTR TME_GetUnitTypeName(mxunit_t type )
{
	defaultexport::unitinfo_t unit;
	
	if ( TME_GetUnitInfo(  unit, MAKE_ID(IDT_UNITINFO,type) ) )
	{ 
		return unit.name ;
	}
	return "" ;
}

NightNotificationDelegate* nightDelegate=NULL;

static void NightCallback ( callback_t* ptr )
{
	if ( nightDelegate )
		nightDelegate->OnNightNotification(ptr);

}


BOOL TME_Night ( NightNotificationDelegate* delegate )
{
	nightDelegate=delegate;
	args[0] = (void*) &NightCallback ;
	if ( MXSUCCESS(mxi->Command( "NIGHT", args, 1 ) ) ) {
        
        mxi->GetProperty( "CONTROLLED_CHARACTER", args[0] );
        TME_CurrentCharacter ( args[0] );
        TME_RefreshCurrentCharacter ();
		
        return (BOOL)args[1].vInt;
	}

	return FALSE;
}

m_gameover_t TME_CheckWinLose (void)
{
	if ( MXSUCCESS(mxi->Command( "CHECKWINLOSE", args, 0 ) ) ) {
		return (m_gameover_t)args[1].vInt;
	}
    
	return MG_NONE;
}


BOOL TME_Save ( LPSTR filespec, PFNSERIALIZE function )
{
	args[0] = filespec ;
    args[1] = (void*)function ;
	if ( MXFAILED(mxi->Command( "@SAVE", args, 2 ) ) ) {
		return FALSE;
	}
	return TRUE;
}

BOOL TME_Load ( LPSTR filespec, PFNSERIALIZE function )
{
	args[0] = filespec ;
    args[1] = (void*)function ;
	if ( MXFAILED(mxi->Command( "@LOAD", args, 2 ) ) ) {
		return FALSE;
	}

	mxi->GetProperty( "CONTROLLED_CHARACTER", args[0] );
	TME_CurrentCharacter ( args[0] );
	TME_RefreshCurrentCharacter ();
	return TRUE;
}

BOOL TME_SaveDescription ( LPSTR filespec, string& description )
{
	args[0] = filespec ;
    args[1] = (void*) &description ;
	if ( MXFAILED(mxi->Command( "@DESCRIPTION", args, 2 ) ) ) {
		return FALSE;
	}
    
	return TRUE;
}

BOOL TME_LoadDiscoveryMap ( LPSTR filespec )
{
    args[0] = filespec ;
    if ( MXFAILED(mxi->Command( "@LOADDISCOVERYMAP", args, 1 ) ) ) {
        return FALSE;
    }
    return TRUE;
}

BOOL TME_SaveDiscoveryMap ( LPSTR filespec )
{
    args[0] = filespec ;
    if ( MXFAILED(mxi->Command( "@SAVEDISCOVERYMAP", args, 1 ) ) ) {
        return FALSE;
    }
    return TRUE;
}

#if defined(_DDR_)
BOOL Character_EnterTunnel ( const character& c )
{
	args[0] = c.id ;
	if ( MXFAILED( mxi->Command("ENTERTUNNEL",args,1) ) )
		return FALSE;
	TME_RefreshCurrentCharacter();
	return TRUE;
}

//void Character_ExitTunnel ( const character& c );
#endif

void Character_Lookat ( const character& c, loc_t location )
{
	args[0] = c.id;
	args[1] = MAKE_LOCID(location.x,location.y) ;
	mxi->Command("LOOKAT", args, 2);	
	TME_RefreshCurrentCharacter();
}

void Character_Wait ( const character& c )
{
	u32			mode;
	
	mode = c.wait+1;
	if ( mode>WM_FOREVER ) mode=WM_NONE;
	args[0] = c.id;
	args[1] = mode;
	mxi->Command("WAIT",args,2);
	TME_RefreshCurrentCharacter();
}

BOOL Character_IsControllable( mxid id )
{
	if ( MXSUCCESS( mxi->GetEntityProperty ( id, "ISCONTROLLABLE", args[0] ) ) ){
		return (BOOL)(s32)args[0] ;
	}
	return FALSE;
}

void TME_SelectChar ( mxid newid )
{
	if ( Character_IsControllable( newid ) ) {
		TME_CurrentCharacter ( newid );
	}
}

mxid Character_LocationObject (  const character& c )
{
	mxi->GetEntityProperty(c.id,"LOCATIONOBJECT",args[0]);
	return args[0].vInt;
}

BOOL Character_HasBattleInfo ( const character& c )
{
	if ( MXSUCCESS( mxi->GetEntityProperty(c.id,"HASBATTLEINFO",args[0]) ) ) 
		return (BOOL)(s32)args[0];
	return FALSE;
}

void Character_Rest ( const character& c )
{
	args[0] = c.id;
	if ( MXSUCCESS(mxi->Command("REST",args,1)) ) {
		TME_RefreshCurrentCharacter();
	}
}

void Character_Look ( const character& c, mxdir_t dir )
{
	args[0] = c.id ;
	args[1] = MAKE_ID(IDT_DIRECTIONINFO,dir) ;
	mxi->Command("LOOK",args,2) ;
	TME_RefreshCurrentCharacter();
}

void Character_LookLeft ( const character& c )
{
	args[0] = c.id ;
	mxi->Command("LOOKLEFT",args,1) ;
	TME_RefreshCurrentCharacter();
}

void Character_LookRight ( const character& c )
{
	args[0] = c.id ;
	mxi->Command("LOOKRIGHT",args,1) ;
	TME_RefreshCurrentCharacter();
}

BOOL Character_Move ( const character& c )
{
	args[0] = c.id ;
	if ( MXFAILED( mxi->Command("FORWARD",args,1) ) )
		return FALSE;
	TME_RefreshCurrentCharacter();
	return TRUE;
}


BOOL Character_Place ( const character& c, loc_t location )
{
    args[0] = c.id;
    args[1] = MAKE_LOCID(location.x,location.y) ;
    mxi->Command("PLACE", args, 2);
    TME_RefreshCurrentCharacter();
    return true;
}

BOOL Character_Approach ( const character& c )
{
	args[0] = c.id ;
	args[1] = recruitable_characters[0] ;
	if ( MXSUCCESS( mxi->Command("APPROACH",args,2) ) ) {
		if ( (s32)args[1] ) {
			TME_CurrentCharacter ( args[1] );
		}else{
			TME_RefreshCurrentCharacter();
		}
        return true;
	}
    return false;
}

BOOL Character_Follow ( const character& c, mxid id )
{
	args[0] = c.id ;
	args[1] = id ;
	if ( MXSUCCESS( mxi->Command("FOLLOW",args,2) ) ) {
        TME_RefreshCurrentCharacter();
        return TRUE;
    }
    return FALSE;
}

BOOL Character_UnFollow ( const character& c, mxid id )
{
	args[0] = c.id ;
	args[1] = id ;
	if ( MXSUCCESS( mxi->Command("UNFOLLOW",args,2) ) ) {
        TME_RefreshCurrentCharacter();
        return TRUE;
    }
    return FALSE;
}

BOOL Character_CanFollow( const character& c, mxid id )
{
    args[0] = id ; 
	if ( MXSUCCESS( mxi->GetEntityProperty ( c.id, "CANFOLLOW", args[0] ) ) ){
		return (BOOL)(s32)args[0] ;
	}
	return FALSE;
}

BOOL Character_Disband ( const character& c )
{
	args[0] = c.id ;
	if ( MXSUCCESS( mxi->Command("DISBANDGROUP",args,1) ) ) {
        TME_RefreshCurrentCharacter();
        return TRUE;
    }
    return FALSE;
}

BOOL Character_SwapGroupLeader ( const character& c, mxid id )
{
	args[0] = c.id ;
	args[1] = id ;
	if ( MXSUCCESS( mxi->Command("SWAPGROUPLEADER",args,2) ) ) {
        TME_RefreshCurrentCharacter();
        return TRUE;
    }
    return FALSE;
}



BOOL Character_RecruitMen ( const character& c )
{
	TME_GetCharacterLocationInfo ( c );
	args[0] = c.id ;
	args[1] = location_stronghold.id;
	args[2] = (s32)0 ; // default
	if ( MXSUCCESS( mxi->Command("RECRUITMEN",args,3) ) ) {
		TME_RefreshCurrentCharacter();
		return TRUE;
	}
	return FALSE;
}

BOOL Character_PostMen ( const character& c )
{
	TME_GetCharacterLocationInfo ( c );
	args[0] = c.id ;
	args[1] = location_stronghold.id;
	args[2] = (s32)0 ; // default
	if ( MXSUCCESS( mxi->Command("POSTMEN",args,3) ) ) {
		TME_RefreshCurrentCharacter();
		return TRUE;
	}
	return FALSE;
}

BOOL Character_Hide ( const character& c )
{
	MXRESULT res;
	
	args[0] = c.id;
	if ( Character_IsHidden(c) )
		res = mxi->Command("UNHIDE",args,1);
	else
		res = mxi->Command("HIDE",args,1);
	
	if ( MXSUCCESS(res) ) {
		TME_RefreshCurrentCharacter();
		return TRUE;
	}
	return FALSE;
}

mxid Character_Fight ( const character& c )
{
	args[0] = c.id;
	if ( MXSUCCESS( mxi->Command("FIGHT",args,1) ) ) {
		TME_RefreshCurrentCharacter();
		return args[1].vInt;
	}
	return NONE ;
}

BOOL Character_Attack ( const character& c )
{
	args[0] = c.id;
	if ( MXSUCCESS( mxi->Command("ATTACK",args,1) ) ) {
		TME_RefreshCurrentCharacter();
		return TRUE;
	}
	return FALSE;
}

mxid Character_Seek ( const character& c )
{
	args[0] = c.id;
	if ( MXSUCCESS( mxi->Command("SEEK",args,1) ) ) {
		TME_RefreshCurrentCharacter();
		return args[1].vInt;
	}
	return NONE ;
}

#if defined(_DDR_)
BOOL Character_Use ( const character& c )
{
	args[0] = c.id;
	if ( MXSUCCESS( mxi->Command("USE",args,1) ) ) {
		TME_RefreshCurrentCharacter();
		return TRUE;
	}
	return FALSE;
}

BOOL Character_Give ( const character& c, mxid to )
{
	args[0] = c.id;
    args[1] = to;
	if ( MXSUCCESS( mxi->Command("GIVE",args,1) ) ) {
		TME_RefreshCurrentCharacter();
		return TRUE;
	}
	return FALSE;
}

BOOL Character_Take ( const character& c )
{
	args[0] = c.id;
	if ( MXSUCCESS( mxi->Command("TAKE",args,1) ) ) {
		TME_RefreshCurrentCharacter();
		return TRUE;
	}
	return FALSE;
}
#endif



BOOL Character_Army ( mxid id, army_t& out )
{
    CLEARINFOSTRUCT ( out, IDT_ARMY );
    
    if ( MXSUCCESS( mxi->EntityById(id, &out) ) ) {
        return TRUE;
    }
    return FALSE ;
}

void TME_GetArmies ( mxid loc, loc_armyinfo_t* armyinfo )
{
	args[0] = &armyinfo->armies ;
	args[1] = loc; //MAKE_LOCID(loc.x,loc.y) ;
	
	armyinfo->foe_warriors = 0;
	armyinfo->foe_riders = 0;
	armyinfo->friends_warriors = 0 ;
	armyinfo->friends_riders = 0 ;
	armyinfo->regiment_warriors = 0;
	armyinfo->regiment_riders = 0;
	armyinfo->friends_armies = 0;
	armyinfo->foes_armies = 0;
	
	if ( MXSUCCESS( mxi->GetProperties ( "ARMIES", args, 2 ) ) ) {
		armyinfo->foe_warriors = args[1];
		armyinfo->foe_riders = args[2];
		armyinfo->friends_warriors = args[3] ;
		armyinfo->friends_riders = args[4] ;
		armyinfo->regiment_warriors = args[5];
		armyinfo->regiment_riders = args[6];
		armyinfo->foes_armies = args[8];
		armyinfo->friends_armies = args[7];
	}
	
}



mxid TME_GetId( LPCSTR symbol, id_type_t type )
{
	return mxi->EntityByName(symbol, type);
}

void* TME_GetEntityUserData ( mxid id) 
{
	return mxi->EntityUserData(id);
}


BOOL TME_Init ( void )
{
	// first we need an interface object
	mxi = new mxinterface ;
	
	args[0] = mxi;
	
	// A scenario must provide a create function
#if defined(_LOM_)
	if ( MXFAILED( tme::lom::Create( mxi ) ) ) {
		return FALSE;
	}
#endif
#if defined(_DDR_)
	if ( MXFAILED( tme::ddr::Create( mxi ) ) ) {
		return FALSE;
	}
#endif
    

    randomno::instance.randomize();
	
	// Tell the engine where to find its data
	args[0] = (LPSTR)TME_ScenarioDirectory() ;
	if ( MXFAILED ( mxi->Command("@SETDATABASEDIRECTORY",args,1) ) ) {
		return FALSE ;
	}
	
	
	// initialise the engine
	if ( MXFAILED ( mxi->Command("@INIT") ) ) {
		return FALSE ;
	}
	
	
	// get the default characters
	args[0] = &default_characters ;
	if ( MXFAILED ( mxi->GetProperties("DEFAULTCHARACTERS", args, 1 ) ) ) {
		return FALSE ;
	}
	
	TME_CurrentCharacter ( default_characters[0] );
	
	return TRUE ;
}

BOOL TME_DeInit ( void )
{
	if ( mxi ) {
		
		mxi->Command("@DEINIT");
		
		SAFEDELETE ( mxi );
		
	}
	
	tme_text = NULL ;


	default_characters.Clear() ;
	recruitable_characters.Clear();
	location_characters.Clear();
	location_strongholds.Clear();
	location_infront_strongholds.Clear();

	location_infront_armies.armies.Clear();
	location_armies.armies.Clear();

	return TRUE ;
	
}

BOOL TME_DebugInstallMap( void* map )
{
    args[0] = map ;
    if ( MXSUCCESS( mxi->Command("@SETMAP",args,1) ) ) {
        TME_RefreshCurrentCharacter();
        return TRUE;
    }
    return FALSE;
}


