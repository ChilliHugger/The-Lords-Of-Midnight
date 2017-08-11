/*
 *  tme_interface.h
 *  tme
 *
 *  Created by Chris Wild on 10/02/2011.
 *  Copyright 2011 Chilli Hugger. All rights reserved.
 *
 */

#ifndef _TME_INTERFACE_H_INCLUDED
#define _TME_INTERFACE_H_INCLUDED

#include "tme.h"
#include "collections.h"
#include "variables.h"

#ifdef _DDR_
#include "scenario_ddr.h"
#define ddrexport	tme::scenarios::ddr
#endif

#ifdef _LOM_
#include "scenario_lom.h"
#define lomexport	tme::scenarios::lom
#endif


//#include "library.h"

using namespace tme ;
using namespace tme::scenarios::exports;
using namespace chilli::collections;

#define character	defaultexport::character_t
#define regiment	defaultexport::regiment_t
#define stronghold	defaultexport::stronghold_t
#define anobject	defaultexport::object_t
#define unitinfo	defaultexport::unitinfo_t
#define maplocation	defaultexport::location_t 
#define terraininfo	defaultexport::terraininfo_t 
#define raceinfo	defaultexport::raceinfo_t 
//#define army		defaultexport::army_t


typedef struct loc_armyinfo_t {
	c_mxid	armies;
	u32		friends_warriors;
	u32		friends_riders;
	u32		friends_armies;
	u32		foe_warriors;
	u32		foe_riders;
	u32		foes_armies;
	u32		regiment_warriors;
	u32		regiment_riders;
} loc_armyinfo_t ;

class NightNotificationDelegate
{
public:
	virtual void OnNightNotification( callback_t* event ) = 0 ;
};



LPCSTR		TME_ScenarioDirectory ( void );
LPCSTR		TME_ScenarioName ( void );

BOOL		TME_Init ( void );
BOOL		TME_DeInit ( void );
void		TME_PurgeTextCache();
LPCSTR		TME_GetCharacterText ( const character& c, const string& command ) ;
LPCSTR		TME_GetText ( const string& command ) ;
LPCSTR		TME_GetText ( const string& command, variant args[], int count ) ;
LPCSTR		TME_GetSystemString ( const character& c, int msg ) ;
LPCSTR		TME_LastActionMsg();
LPCSTR		TME_GetNumber(int number);
LPCSTR		TME_GetLocationText ( mxgridref loc );

character&	TME_CurrentCharacter ( void );
character&	TME_CurrentCharacter ( mxid characterid );
character&	TME_RefreshCurrentCharacter ( void );

BOOL		TME_GetCharacter( character& out, mxid id );
BOOL		TME_GetStronghold( stronghold& out, mxid id );
BOOL		TME_GetTerrainInfo( terraininfo& out, mxid id );
BOOL		TME_GetRaceInfo( raceinfo& out, mxid id );
BOOL		TME_GetObject( anobject& out, mxid id );
BOOL		TME_GetUnitInfo( unitinfo& out, mxid id );
BOOL		TME_GetLocation( maplocation& out, mxid id );
BOOL		TME_GetLocation( maplocation& out, loc_t loc );

BOOL		TME_GetMapLocation( maplocation& out, mxid id );
BOOL		TME_GetMapLocation( maplocation& out, loc_t loc );

BOOL        TME_GetRegiment( regiment& out, mxid id );

BOOL		TME_GetLocationInfo( loc_t loc );
BOOL		TME_GetCharacterLocationInfo ( const character& c );
LPCSTR		TME_GetUnitTypeName(mxunit_t type );

m_gameover_t TME_CheckWinLose (void);
BOOL		TME_Night ( NightNotificationDelegate* delegate );
BOOL		TME_Save ( LPSTR filespec, PFNSERIALIZE function );
BOOL		TME_Load ( LPSTR filespec, PFNSERIALIZE function );
BOOL        TME_SaveDescription ( LPSTR filespec, string& description );

BOOL		TME_SaveDiscoveryMap ( LPSTR filespec );
BOOL		TME_LoadDiscoveryMap ( LPSTR filespec );


void		TME_SelectChar ( mxid newid );

size		TME_MapSize ( void );
void		TME_GetArmies ( mxid loc, loc_armyinfo_t* army );
s32         TME_GetAllStrongholds (c_mxid& collection ) ;
s32         TME_GetAllRegiments (c_mxid& collection );
s32			TME_GetCharacters ( mxid id, c_mxid& collection, u32& recruited );
s32         TME_GetAllCharacters (c_mxid& collection );
s32         TME_GetFollowers ( mxid id, c_mxid& collection );
s32			TME_GetCharactersAtLocation ( mxid id, c_mxid& collection, BOOL showall, BOOL showtunnel  );
s32			TME_GetCharactersAtLocation ( loc_t loc, c_mxid& collection, BOOL showall  );
MXRESULT	TME_GetArmiesAtLocation( mxid loc, u32& enemies, u32& friends );
MXRESULT	TME_GetArmiesAtLocation( loc_t loc, u32& enemies, u32& friends );
LPCSTR      TME_GetSymbol( mxid id );
mxid		TME_LinkData ( LPCSTR symbol, void* data );
void*		TME_GetEntityUserData ( mxid );
mxid		TME_GetId( LPCSTR symbol, idtype_t type );

typedef struct MapInfo_t {
    loc_t   top;
    loc_t   bottom;
    chilli::types::size    size;
} MapInfo_t ;


BOOL        TME_MapInfo ( MapInfo_t* info );


extern mxinterface*		mxi ;
extern c_mxid			default_characters ;

extern c_mxid			recruitable_characters;
extern c_mxid			location_characters;
extern u32				location_recruited;
extern c_mxid			location_strongholds;
extern mxid				location_infrontid;
extern mxid				location_lookingatid;
extern loc_t			location_infront;
extern loc_t			location_lookingat;
extern flags32			location_flags;
extern mxid				location_fightthing;
extern stronghold		location_stronghold;
extern mxid				location_object;
extern mxid             location_stubborn_lord_attack;
extern mxid             location_stubborn_lord_move;

#if defined(_DDR_)
extern mxid				location_object_tunnel;
extern mxid             location_someone_to_give_to;
#endif

extern c_mxid			location_infront_strongholds;
extern mxid				location_infront_object;

extern	loc_armyinfo_t	location_infront_armies;
extern	loc_armyinfo_t	location_armies;


// character helpers
inline BOOL Character_IsResting(const character& c)			{ return c.flags.Is(cf_resting); }
inline mxtime_t Character_Time(const character& c)		{ return Character_IsResting(c) ? sv_time_night : c.time ; }
inline BOOL Character_IsNight(const character& c)			{ return Character_Time(c)==(mxtime_t)sv_time_night; }
inline BOOL Character_IsDawn(const character& c)			{ return Character_Time(c)==(mxtime_t)sv_time_dawn; }
inline BOOL Character_IsWaiting(const character& c)			{ return c.wait!=WM_NONE; }
inline BOOL Character_IsHidden(const character& c)			{ return c.flags.Is(cf_hidden); }
inline BOOL Character_IsRiding(const character& c)			{ return c.flags.Is(cf_riding); }
inline BOOL Character_IsAlive(const character& c)			{ return c.flags.Is(cf_alive); }
inline BOOL Character_IsDead(const character& c)			{ return !c.flags.Is(cf_alive); }
inline BOOL Character_IsRecruited(const character& c)		{ return c.flags.Is(cf_recruited); }
inline BOOL Character_IsAllowedArmy(const character& c)		{ return c.flags.Is(cf_army); }
inline BOOL Character_IsAllowedHide(const character& c)		{ return c.flags.Is(cf_hide); }
inline BOOL Character_IsAllowedHorse(const character& c)	{ return c.flags.Is(cf_horse); }
inline BOOL Character_IsAllowedMoonring(const character& c)	{ return c.flags.Is(cf_moonring); }
inline BOOL Character_IsAllowedIcecrown(const character& c)	{ return c.flags.Is(cf_icecrown); }
inline BOOL Character_CanDestroyIcecrown(const character& c){ return c.flags.Is(cf_destroyicecrown); }
inline BOOL Character_IsInBattle(const character& c)		{ return c.flags.Is(cf_inbattle); }
inline BOOL Character_HasWonBattle(const character& c)		{ return c.flags.Is(cf_wonbattle); }

#if defined(_DDR_)
inline BOOL Character_HasUsedObject(const character& c)		{ return c.flags.Is(cf_usedobject); }
inline BOOL Character_IsInTunnel(const character& c)		{ return c.flags.Is(cf_tunnel); }
inline BOOL Character_InTunnel(const character& c)		{ return c.flags.Is(cf_tunnel); }
inline BOOL Character_IsPreparingForBattle(const character& c)		{ return c.flags.Is(cf_preparesbattle); }
BOOL Character_EnterTunnel ( const character& c );
//BOOL Character_ExitTunnel ( const character& c );
BOOL Character_Use ( const character& c );
BOOL Character_Give ( const character& c, mxid to );
BOOL Character_Take ( const character& c );
#endif

inline BOOL Character_IsFollowing(const character& c)		{ return c.following!=IDT_NONE; }
inline BOOL Character_IsLeading(const character& c)		{ return c.followers!=0; }

BOOL Character_IsControllable( mxid id );
void Character_Lookat ( const character& c, loc_t location );
void Character_Wait ( const character& c );
void Character_Rest ( const character& c );
void Character_Look ( const character& c, mxdir_t dir );
void Character_LookLeft ( const character& c );
void Character_LookRight ( const character& c );
BOOL Character_Approach ( const character& c );
BOOL Character_Hide ( const character& c );
BOOL Character_HasBattleInfo ( const character& c );
BOOL Character_Move ( const character& c );
BOOL Character_Attack ( const character& c );
BOOL Character_RecruitMen ( const character& c );
BOOL Character_PostMen ( const character& c );
mxid Character_LocationObject (  const character& c );
mxid Character_Fight ( const character& c );
mxid Character_Seek ( const character& c );



BOOL Character_Follow ( const character& c, mxid id );
BOOL Character_UnFollow ( const character& c, mxid id );
BOOL Character_CanFollow( const character& c, mxid id );
BOOL Character_Disband( const character& c );
BOOL Character_SwapGroupLeader( const character& c, mxid id );

BOOL Character_Army ( mxid id, army_t& out );

#if defined(_DDR_)
inline BOOL Location_IsVisible(const maplocation& l)			{ return l.flags.Is(lf_seen); }
inline BOOL Location_HasCharacters(const maplocation& l)		{ return l.flags.Is(lf_character); }
inline BOOL Location_HasTunnel(const maplocation& l)		{ return l.flags.Is(lf_tunnel); }
inline BOOL Location_HasTunnelExit(const maplocation& l)		{ return l.flags.Is(lf_tunnel_exit); }
inline BOOL Location_HasTunnelEntrance(const maplocation& l)		{ return l.flags.Is(lf_tunnel_entrance); }
#endif


// global_tme_vars

void Vars_Init ( void );
void Vars_DeInit ( void );
void Vars_EmptyCache ( void );
info_t* Vars_GetVar( mxid id, const string& varname, CVar var );


// Debug!!
BOOL TME_DebugInstallMap( void* map );



#endif // _TME_INTERFACE_H_INCLUDED


