/*
 * FILE:    mxengine.cpp
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
#include "cocos2d.h"
#include <stdio.h>


using namespace tme ;

mxengine* tme::mx=NULL;

#define MX_REGISTER_SELF        mx = this

#define ACCESS_ITEM(x,y,z) \
    x* mxengine::y(u32 id) \
    {     if ( id==0 ) return NULL ; \
        _MXASSERTE ( id>0 && id<=z.Count() ); \
        return  (x*)z[id-1]; }

#define ACCESS_INFO(x,y,z) \
    x* mxengine::y(u32 id) \
    {     _MXASSERTE ( id>=0 && id<z.Count() ); \
        return  (x*)z[id]; }

#define FIND_ENTITY(x,y) \
    if ( type == IDT_NONE || type == x ) \
        if ( (obj = y.FindSymbol(name)) )\
            return obj;

#define FIND_IDT(x,y) \
    if ( type == x ) return (mxentity*)mx->y(objectno)

/*
 * Function name    : mxengine::engine
 * 
 * Return type        : 
 * 
 * Arguments        : 
 * 
 * Description        : 
 * 
 */

mxengine::mxengine()
{
    MX_REGISTER_SELF;

    text = NULL;
    night = NULL;
    battle = NULL;
    scenario = NULL;
    gamemap = NULL ;
    discoverymap = NULL ;
    variables = NULL ;
//    m_config=NULL;
    pfnNightCallback=NULL;
    m_CurrentCharacter=NULL;
    defaultscenario    = FALSE ;

    m_errorcode=0;
}


/*
 * Function name    : mxengine::~engine
 * 
 * Return type        : 
 * 
 * Arguments        : 
 * 
 * Description        : 
 * 
 */

mxengine::~mxengine()
{
    UnloadScenario ();
}

/*
 * Function name    : mxengine::InstallScenario
 * 
 * Return type        : bool
 * 
 * Arguments        : LPSTR filename
 * 
 * Description        : 
 * 
 */
/*
bool mxengine::InstallScenario ( const c_str& filename )
{
FUNCADDRESS            func;
FNSCENARIOREGISTER    ScenarioRegister;

char    file[MAX_PATH];

#ifdef _DEBUG
    sprintf ( file, "%s_dbg.mxs", filename );
#else
    sprintf ( file, "%s.mxs", filename );
#endif

    if ( (m_hScenarioLib = OS_LoadLibrary ( file )) == NULL )
        return FALSE;

    func = OS_GetFunctionAddress( m_hScenarioLib, "ScenarioRegister" );

    ScenarioRegister = (FNSCENARIOREGISTER)func;

    SAFEDELETE(text);
    SAFEDELETE(night);
    SAFEDELETE(battle);
    SAFEDELETE(scenario);

    ScenarioRegister( this );

    return TRUE ;
}
*/

/*
MXRESULT mxengine::LoadDefaultScenario ( void )
{
    MX_REGISTER_SELF;

    if ( MXFAILED( tme::lom::Create( mxi ) ) ) {
        return FALSE;
    }
    
    scenario = new mxscenario;
    defaultscenario = TRUE ;
    return LoadScenario ( scenario );

}
*/
 
MXRESULT mxengine::LoadScenario ( mxscenario* scenario )
{
    this->scenario = scenario ;
    this->scenario->Register(mx);
    return MX_OK ;
}

/*
 * Function name    : mxengine::UnloadScenario
 * 
 * Return type        : bool
 * 
 * Arguments        : 
 * 
 * Description        : 
 * 
 */

MXRESULT mxengine::UnloadScenario ()
{
    if ( scenario )
        scenario->UnRegister(this);
    
    SAFEDELETE(scenario);

//    if ( m_hScenarioLib )
//        OS_UnloadLibrary ( m_hScenarioLib );
//    m_hScenarioLib = NULL ;

    return MX_OK ;
}


/*
 * Function name    : mxengine::SetDatabaseDirectory
 * 
 * Return type        : bool
 * 
 * Arguments        : 
 * 
 * Description        : 
 * 
 */

MXRESULT mxengine::SetDatabaseDirectory ( const c_str& directory )
{
//char    file[MAX_PATH];

    MX_REGISTER_SELF;

    c_strcpy ( m_szDatabase, directory );
/*
    sprintf ( file,"%s/config", m_szDatabase );
    if ( (m_config = new os::config(file)) == NULL ) {
        Error(-1);
        return MX_FAILED ;
    }
*/
    return MX_OK ;
}

/*
 * Function name    : mxengine::LoadDatabase
 * 
 * Description        : 
 * 
 * Return type        : bool
 * 
 * Arguments        : void
 */
MXRESULT mxengine::LoadDatabase ( void )
{
int ii;
int id;
char filename[MAX_PATH];

    MX_REGISTER_SELF;

    variables::Init(1);

    // We need to move the default database into an accessible folder
#if !defined(_OS_DESKTOP_)
    char database[MAX_PATH];
    sprintf ( database, "%s/%s", m_szDatabase, "database" );
    sprintf ( filename, "%s/database", cocos2d::FileUtils::getInstance()->getWritablePath().c_str() );
    MXTRACE( "Copying Database '%s' from '%s' to '%s'", m_szDatabase, database, filename);
    chilli::os::filemanager::Copy(database, filename);
#else
    sprintf ( filename, "%s/%s", m_szDatabase, "database" );
#endif
    
MXTRACE( "Loading Database '%s'", m_szDatabase);

   chilli::os::file* pFile = new chilli::os::file ( filename, chilli::os::file::modeRead );
    if ( !pFile->IsOpen() ) {
        if ( pFile ) delete pFile;
        //COMPLAIN( "Cannot Load data file %s", filename );
        return MX_FAILED;
    }

    archive ar (pFile, archive::load | archive::bNoFlushOnDelete);

    // serialize is not for save game
    m_savegame = FALSE ;

u32        magicno;
u32        scenarioid;
//u32        versionno;
c_str    header;

    // magic no
    ar >> magicno ;
MXTRACE( "MagicNo=%x", (int)magicno);

    // check for valid header and possible byte swap version
    if ( magicno == chilli::lib::u32Swap(TME_MAGIC_NO) ) {
        // turn on byte swapping
        ar.m_nMode |= archive::bByteSwap ;
MXTRACE( "ByteSwapping ON");

    }else{
        if ( magicno != TME_MAGIC_NO ) {
            MXTRACE("Invalid DATABASE MagicNo");
            return MX_UNKNOWN_FILE;
        }
    }
        
    ar >> scenarioid;

MXTRACE( "ScenarioId=%d", (int)scenarioid);

    ar >> savegameversion ;
MXTRACE( "Version=%d", (int)savegameversion);
    if ( savegameversion < DATABASEVERSION ) {
        MXTRACE("Invalid DATABASE Version");
        return MX_INCORRECT_VERSION ;
    }

    
    
    ar >> header ;
MXTRACE( "Header='%s'", (LPSTR)header);
    
    if (c_stricmp( header, DATABASEHEADER ) != 0 ) {
        MXTRACE("Invalid DATABASE Header");
        return MX_UNKNOWN_FILE;
    }

    ar >> sv_characters ;    objCharacters.Create(scenario,IDT_CHARACTER,sv_characters);
    ar >> sv_regiments ;    objRegiments.Create(scenario,IDT_REGIMENT,sv_regiments);
    ar >> sv_routenodes ;    objRoutenodes.Create(scenario,IDT_ROUTENODE,sv_routenodes);
    ar >> sv_strongholds ;    objStrongholds.Create(scenario,IDT_STRONGHOLD,sv_strongholds);
    ar >> sv_places ;        objPlaces.Create(scenario,IDT_PLACE,sv_places);
    ar >> sv_objects ;        objObjects.Create(scenario,IDT_OBJECT,sv_objects);
    ar >> sv_missions ;        objMissions.Create(scenario,IDT_MISSION,sv_missions);
    ar >> sv_victories ;    objVictories.Create(scenario,IDT_VICTORY,sv_victories);
    ar >> sv_directions ;    objDirectionInfos.Create(scenario,IDT_DIRECTIONINFO,sv_directions);
    ar >> sv_units ;        objUnitInfos.Create(scenario,IDT_UNITINFO,sv_units);
    ar >> sv_races ;        objRaceInfos.Create(scenario,IDT_RACEINFO,sv_races);
    ar >> sv_genders ;        objGenderInfos.Create(scenario,IDT_GENDERINFO,sv_genders);
    ar >> sv_terrains ;        objTerrainInfos.Create(scenario,IDT_TERRAININFO,sv_terrains);
    ar >> sv_areas ;        objAreaInfos.Create(scenario,IDT_AREAINFO,sv_areas);
    ar >> sv_commands ;        objCommandInfos.Create(scenario,IDT_COMMANDINFO,sv_commands);
    ar >> sv_variables ;    variables = new cvarreg_t[sv_variables];

MXTRACE( "Characters =%d", (int)sv_characters);
MXTRACE( "Regiments  =%d", (int)sv_regiments);
MXTRACE( "RouteNodes =%d", (int)sv_routenodes);
MXTRACE( "Strongholds=%d", (int)sv_strongholds);
MXTRACE( "Places     =%d", (int)sv_places);
MXTRACE( "Objects    =%d", (int)sv_objects);
MXTRACE( "Missions   =%d", (int)sv_missions);
MXTRACE( "Victories  =%d", (int)sv_victories);
MXTRACE( "Directions =%d", (int)sv_directions);
MXTRACE( "Units      =%d", (int)sv_units);
MXTRACE( "Races      =%d", (int)sv_races);
MXTRACE( "Genders    =%d", (int)sv_genders);
MXTRACE( "Terrains   =%d", (int)sv_terrains);
MXTRACE( "Areas      =%d", (int)sv_areas);
MXTRACE( "Commands   =%d", (int)sv_commands);
MXTRACE( "Vaiables   =%d", (int)sv_variables);
    
MXTRACE( "Loading Entities");

    objCharacters.Serialize(ar);
    objRegiments.Serialize(ar);
    objRoutenodes.Serialize(ar);
    objStrongholds.Serialize(ar);
    objPlaces.Serialize(ar);
    objObjects.Serialize(ar);
    objMissions.Serialize(ar);
    objVictories.Serialize(ar);
    objDirectionInfos.Serialize(ar);
    objUnitInfos.Serialize(ar);
    objRaceInfos.Serialize(ar);
    objGenderInfos.Serialize(ar);
    objTerrainInfos.Serialize(ar);
    objAreaInfos.Serialize(ar);
    objCommandInfos.Serialize(ar);

MXTRACE( "Loading Text");

    
// load strings
    text->Serialize(ar);

MXTRACE( "Loading Variables");
    
// load variables
    for ( ii=0; ii<sv_variables; ii++ ) {
        variables[ii].memory=NULL;
        variables[ii].name=NULL;
        variables[ii].string=NULL;
        variables[ii].type=CVar::VNONE;

        ar >> (char**)&variables[ii].name;
        ar >> (char**)&variables[ii].string;
        ar >> id;
        variables[ii].type = (CVar::type)id ;
    }

#if defined(_DDR_)
    if ( savegameversion > 10) {
        
        MXTRACE( "Loading Object Types");
        ar >> sv_object_types ;        objObjectTypesInfos.Create(scenario,IDT_OBJECT_TYPE,sv_object_types);
        MXTRACE( "Object Types   =%d", (int)sv_object_types);
        objObjectTypesInfos.Serialize(ar);

        MXTRACE( "Loading Object Powers");
        ar >> sv_object_powers ;        objObjectPowersInfos.Create(scenario,IDT_OBJECT_POWER,sv_object_powers);
        MXTRACE( "Object Powers   =%d", (int)sv_object_powers);
        objObjectPowersInfos.Serialize(ar);
    }
#endif
    
    // 

    // read scenario info

    ar.Close();
    
    SAFEDELETE ( pFile );

MXTRACE( "Update Variables");
    variables::Update();

    // 
    //m_config->SetSection ( "general" );
    //m_bEnergyCheat = m_config->ReadBool ( "EnergyCheat", TRUE );

MXTRACE("Loading MAP");

#if !defined(_OS_DESKTOP_)
    sprintf ( database, "%s/%s", m_szDatabase, sv_map_file );
    sprintf ( filename, "%s/%s", cocos2d::FileUtils::getInstance()->getWritablePath().c_str(), sv_map_file );
    MXTRACE( "Copying Map '%s' from '%s' to '%s'", sv_map_file, database, filename);
    chilli::os::filemanager::Copy(database, filename);
#else
    sprintf ( filename, "%s/%s", m_szDatabase, sv_map_file );
#endif

    
    //sv_map_width = 66;
    //sv_map_height = 63;
    gamemap = new mxmap() ;

    if ( !gamemap->Load ( filename ) ) {
        SAFEDELETE ( gamemap );
        return MX_FAILED;
    }

    
    gamemap->ClearVisible();
    
    
MXTRACE( "Init Variables");
    variables::Init(2);

    sv_days = 0;
    sv_strongholdadjuster = 0;

    scenario->initialiseAfterCreate();
    
    
    
    CurrentChar( (mxcharacter*)mx->EntityByIdt(sv_character_default[0]) ) ;
   
    
    return MX_OK ;
}



/*
 * Function name    : mxengine::UnloadDatabase
 * 
 * Return type        : bool
 * 
 * Arguments        : void
 * 
 * Description        : 
 * 
 */

MXRESULT mxengine::UnloadDatabase ( void )
{
    MX_REGISTER_SELF;

    //SAFEDELETE ( m_config );

    SAFEDELETE( discoverymap );
    SAFEDELETE ( gamemap );

    objCharacters.Destroy();
    objRegiments.Destroy();
    objRoutenodes.Destroy();
    objStrongholds.Destroy();
    objPlaces.Destroy();
    objDirectionInfos.Destroy();
    objObjects.Destroy();
    objUnitInfos.Destroy();
    objRaceInfos.Destroy();
    objGenderInfos.Destroy();
    objTerrainInfos.Destroy();
    objAreaInfos.Destroy();
    objCommandInfos.Destroy();
    objMissions.Destroy();
    objVictories.Destroy();
    
#if defined(_DDR_)
    objObjectPowersInfos.Destroy();
    objObjectTypesInfos.Destroy();
#endif

    /* TODO: Fix this - problem with static definition and non static after load and save */
    for ( int ii=0; ii<sv_variables; ii++ ) {
        void* temp = (void*)variables[ii].name ;
        SAFEFREE(temp);
        temp = (void*)variables[ii].string;
        SAFEFREE(temp);
        variables[ii].name=NULL;
        variables[ii].string=NULL;
    }
     
    variables::DeInit();

    SAFEDELETEARRAY ( variables );


    return MX_OK ;
}


/*
 * Function name    : mxengine::Error
 * 
 * Description        : 
 * 
 * Return type        : void
 * 
 * Arguments        : u32 errorcode
 */

void mxengine::Error ( u32 errorcode )
{
    m_errorcode  = errorcode;
}


/*
 * Function name    : mxengine::Error
 * 
 * Description        : 
 * 
 * Return type        : void
 * 
 * Arguments        : void
 */

u32 mxengine::Error () const
{
    return m_errorcode ;
}

ACCESS_ITEM(mxroutenode,RouteNodeById,objRoutenodes);
ACCESS_ITEM(mxcharacter,CharacterById,objCharacters);
ACCESS_ITEM(mxstronghold,StrongholdById,objStrongholds);
ACCESS_ITEM(mxregiment,RegimentById,objRegiments);
ACCESS_ITEM(mxplace,PlaceById,objPlaces);
ACCESS_ITEM(mxvictory,VictoryById,objVictories);
ACCESS_ITEM(mxmission,MissionById,objMissions);
ACCESS_ITEM(mxobject,ObjectById,objObjects);

ACCESS_INFO(mxdirection,DirectionById,objDirectionInfos);
ACCESS_INFO(mxunitinfo,UnitById,objUnitInfos);
ACCESS_INFO(mxrace,RaceById,objRaceInfos);
ACCESS_INFO(mxgender,GenderById,objGenderInfos);
ACCESS_INFO(mxterrain,TerrainById,objTerrainInfos);
ACCESS_INFO(mxarea,AreaById,objAreaInfos);
ACCESS_INFO(mxcommand,CommandById,objCommandInfos);

#if defined(_DDR_)
ACCESS_INFO(mxobjectpower,ObjectPowerById,objObjectPowersInfos);
ACCESS_INFO(mxobjecttype,ObjectTypeById,objObjectTypesInfos);
#endif

/*
 * Function name    : mxengine::CurrentChar
 * 
 * Return type        : void
 * 
 * Arguments        : item::character* character
 * 
 * Description        : 
 * 
 */

void mxengine::CurrentChar ( mxcharacter* character )
{
    m_CurrentCharacter = character ;
    character->Cmd_LookDir(character->Looking());
    sv_controlled_character = MAKE_ID(IDT_CHARACTER,character->SafeId());

}


/*
 * Function name    : mxengine::CurrentChar
 * 
 * Return type        : item::character*
 * 
 * Arguments        : void
 * 
 * Description        : 
 * 
 */

mxcharacter* mxengine::CurrentChar ( void ) const
{
    return m_CurrentCharacter;
}


void mxengine::NightCallback( callback_t* ptr)
{  
    if ( pfnNightCallback ) 
        pfnNightCallback(ptr); 
}



/*
 * Function name    : mxengine::LoadGame
 * 
 * Return type        : bool
 * 
 * Arguments        : LPSTR filename
 * 
 * Description        : 
 * 
 */

MXRESULT mxengine::LoadGame ( const c_str& filename, PFNSERIALIZE function )
{
    chilli::os::file* pFile = new chilli::os::file ( filename, chilli::os::file::modeRead );
    if ( !pFile->IsOpen() ) {
        if ( pFile ) delete pFile;
        //COMPLAIN( "Cannot Load data file %s", filename );
        return MX_FAILED;
    }

    archive ar (pFile, archive::load | archive::bNoFlushOnDelete);

    // serialize is for save game
    m_savegame = TRUE ;

u32        magicno;
u32        scenarioid;
u16        temp16;
c_str    header;
c_str  description;

    // magic no
    ar >> magicno;
    // check for valid header and possible byte swap version
    if ( magicno == chilli::lib::u32Swap(TME_MAGIC_NO) ) {
        // turn on byte swapping
        ar.m_nMode |= archive::bByteSwap ;
    }else{
        if ( magicno != TME_MAGIC_NO )
            return MX_UNKNOWN_FILE;
    }

    ar >> scenarioid;
    if ( scenarioid != scenario->GetInfoBlock()->Id )
        return MX_UNKNOWN_FILE;

    ar >> m_versionno ;
    //if ( m_versionno != SAVEGAMEVERSION )
    //    return MX_INCORRECT_VERSION;

    savegameversion = m_versionno ;

    ar >> header ;
    if ( strcmp( header, SAVEGAMEHEADER ) != 0 )
        return MX_UNKNOWN_FILE;

    if ( SaveGameVersion()>8 ) {
        ar >>  description;
    }else{
        description="";
    }

    /* save the game map */
    gamemap->Serialize ( ar );

    ar >> sv_characters ;        objCharacters.Create(scenario,IDT_CHARACTER,sv_characters);
    ar >> sv_regiments ;        objRegiments.Create(scenario,IDT_REGIMENT,sv_regiments);
    ar >> sv_routenodes ;        objRoutenodes.Create(scenario,IDT_ROUTENODE,sv_routenodes);
    ar >> sv_strongholds ;        objStrongholds.Create(scenario,IDT_STRONGHOLD,sv_strongholds);
    ar >> sv_missions ;            objMissions.Create(scenario,IDT_MISSION,sv_missions);
    ar >> sv_victories ;        objVictories.Create(scenario,IDT_VICTORY,sv_victories);
    ar >> sv_objects;            objObjects.Create(scenario,IDT_OBJECT,sv_objects);

    /* default engine vars */
     ar >> temp16; m_CurrentCharacter = CharacterById(temp16);
    ar >> sv_days;
    ar >> sv_strongholdadjuster;

    //
    objRoutenodes.Serialize(ar);
    objCharacters.Serialize(ar);
    objStrongholds.Serialize(ar);
    objRegiments.Serialize(ar);
    objMissions.Serialize(ar);
    objVictories.Serialize(ar);
    objObjects.Serialize(ar);

    /* update map */
    if ( SaveGameVersion() < 8 ) {
     for (int ii = 0; ii < sv_strongholds; ii++) {
         mxstronghold* stronghold=StrongholdById(ii+1);
         mxloc& mapsqr = gamemap->GetAt ( stronghold->Location() );
         if ( mapsqr.IsVisible() )
             mapsqr.flags |= lf_visited|lf_looked_at ;

     }
    }

    /* Load Character memories */
    for ( int ii=0; ii<sv_characters; ii++ )
        CharacterById(ii+1)->memory.Serialize ( ar );

    /* Load scenario specific */
    scenario->Serialize ( ar );

    /* load Frontend Specific */
    //if ( SaveGameVersion() > 5 && function!=NULL )
    if ( function!= NULL )
        function(SaveGameVersion(),ar);


    ar.Close();

    SAFEDELETE ( pFile );

    mx->CurrentChar ( m_CurrentCharacter );

#if defined(_DDR_)
    if ( SaveGameVersion() >= 11 ) {
        // fix save games
        mxcharacter* morkin = static_cast<mxcharacter*>(mx->EntityByName("CH_MORKIN"));
        morkin->race = RA_MORKIN;

        if ( !morkin->IsRecruited() )
            morkin->Flags().Set(cf_ai);

        // fix for morkin being AI character
        // after being recruited
        if ( morkin->IsRecruited() && morkin->IsAIControlled() ) {
            morkin->Flags().Reset(cf_ai);
            if ( morkin->IsInTunnel() )
                morkin->looking=DR_NORTH;
        }

    }
#endif

    return MX_OK ;
}


MXRESULT mxengine::SaveGameDescription ( const c_str& filename, c_str& description )
{
    chilli::os::file* pFile = new chilli::os::file ( filename, chilli::os::file::modeRead );
    if ( !pFile->IsOpen() ) {
        if ( pFile ) delete pFile;
        return MX_FAILED;
    }
    
    archive ar (pFile, archive::load | archive::bNoFlushOnDelete);
    
    // serialize is for save game
    m_savegame = TRUE ;
    
    u32        magicno;
    u32        scenarioid;
    c_str    header;
    
    // magic no
    ar >> magicno;
    // check for valid header and possible byte swap version
    if ( magicno == chilli::lib::u32Swap(TME_MAGIC_NO) ) {
        // turn on byte swapping
        ar.m_nMode |= archive::bByteSwap ;
    }else{
        if ( magicno != TME_MAGIC_NO )
            return MX_UNKNOWN_FILE;
    }
    
    ar >> scenarioid;
    if ( scenarioid != scenario->GetInfoBlock()->Id )
        return MX_UNKNOWN_FILE;
    
    ar >> m_versionno ;
    
    savegameversion = m_versionno ;
    
    ar >> header ;
    if ( strcmp( header, SAVEGAMEHEADER ) != 0 )
        return MX_UNKNOWN_FILE;
    
    if ( SaveGameVersion()>8 ) {
        ar >>  description;
    }else{
        description="";
    }

    
    ar.Close();
    
    SAFEDELETE ( pFile );

    return MX_OK ;
}



/*
 * Function name    : mxengine::SaveGame
 * 
 * Return type        : bool
 * 
 * Arguments        : LPSTR filename
 * 
 * Description        : 
 * 
 */

MXRESULT mxengine::SaveGame ( const c_str& filename, PFNSERIALIZE function )
{

/* what needs to be saved?
 *
 * Scenario Id
 * Characters
 * Regiments
 * Strongholds
 * Map

 * the following records are informational only
 * and are thus not saved. If you intend to change the
 * values dynamically, then you will need to take care of load and save
 *
 * directions    : CDirectionInfo[m_cDirections]
 * objects        : CObjectInfo[m_cObjects]
 * units        : CUnitInfo[m_cUnits]
 * races        : info::race[m_cRaces]
 * gender        : info::gender[m_cGenders]
 * terrain        : info::terrain[m_cTerrains]
 * areas        : CAreaInfo[m_cAreas]

*/

    chilli::os::file* pFile = new chilli::os::file ( filename, chilli::os::file::modeReadWrite|chilli::os::file::modeCreate );
    if ( pFile == NULL || !pFile->IsOpen() ) {
        if ( pFile ) delete pFile;
        //COMPLAIN( "Cannot Save data file %s", filename );
        return MX_FAILED;
    }

    archive ar (pFile, archive::store | archive::bNoFlushOnDelete );

    // serialize is for save game
    m_savegame = TRUE ;

    ar << TME_MAGIC_NO ;
    ar << scenario->GetInfoBlock()->Id;
    ar << SAVEGAMEVERSION ;
    ar << (char*)SAVEGAMEHEADER ;

    
    //LPCSTR description = "DAY 000: Luxor - Tower of the Moon";
    int lords=0;
    char buffer[1024];
    
    for ( int ii=0; ii<sv_characters; ii++ ) {
        mxcharacter* c = CharacterById(ii+1);
        if ( scenario->CanWeSelectCharacter(c))
        if ( c->IsRecruited() &&  c->IsAlive() )
            lords++;
    }
    
    sprintf( buffer, "Day %d\n%s\n%s\n%d %s"
            , (int)sv_days+1
            , (char*)m_CurrentCharacter->Longname()
            , mx->text->DescribeLocation(m_CurrentCharacter->Location() )
            , lords
            , lords==1 ? "lord" : "lords"
            );
    
    ar << (char*)buffer ;
    
    /* save the game map */
    gamemap->Serialize ( ar );

    // object collections
    ar << sv_characters ;
    ar << sv_regiments ;
    ar << sv_routenodes ;
    ar << sv_strongholds ;
    ar << sv_missions ;
    ar << sv_victories ;
    ar << sv_objects ;

    /* default engine vars */
    ar << (u16)m_CurrentCharacter->Id();
    ar << sv_days;
    ar << sv_strongholdadjuster;

    objRoutenodes.Serialize(ar);
    objCharacters.Serialize(ar);
    objStrongholds.Serialize(ar);
    objRegiments.Serialize(ar);
    objMissions.Serialize(ar);
    objVictories.Serialize(ar);
    objObjects.Serialize(ar);

    /* Save Character memories */
    for ( int ii=0; ii<sv_characters; ii++ )
        CharacterById(ii+1)->memory.Serialize ( ar );

    /* save scenario specific */
    scenario->Serialize ( ar );

    
    /* save frontend specific */
    if ( function != NULL )
        function(SAVEGAMEVERSION,ar);
    
 
    ar.Close();

    SAFEDELETE ( pFile );

    return MX_OK ;

}




/*
 * Function name    : mxengine::CollectRegiments
 * 
 * Return type        : int
 * 
 * Arguments        : gridref loc
 *                  : item::regiment* array[]
 *                  : int max
 * 
 * Description        : 
 * 
 */

u32 mxengine::CollectRegiments ( mxgridref loc, tme::collections::entities& collection )
{
int ii;
    //int count=0;

    collection.Create(MAX_REGIMENTS_INLOCATION);

    // count
    for (ii = 0; ii < sv_regiments; ii++) {
        mxregiment* pReg = RegimentById(ii+1);
        if ( pReg && pReg->Total() && pReg->Location() == loc )
            collection.Add(pReg) ;
    }

    return collection.Compact();
}



/*
 * Function name    : mxengine::GetStronghold
 * 
 * Return type        : item::stronghold*
 * 
 * Arguments        : gridref loc
 * 
 * Description        : 
 * 
 */

u32 mxengine::CollectStrongholds ( mxgridref loc, tme::collections::entities& collection ) 
{
int ii;
//int count=0;

    collection.Clear();

    mxloc& mapsqr = gamemap->GetAt ( loc );

    if ( !mapsqr.IsStronghold() )
        return 0;

    collection.Create(MAX_STRONGHOLDS_INLOCATION);

    // count
    for (ii = 0; ii < sv_strongholds; ii++) {
        mxstronghold* stronghold = mx->StrongholdById(ii+1);
        if ( stronghold && stronghold->Location() == loc ) {
            collection.Add(stronghold);
        }
    }

    return collection.Compact();
}


u32 mxengine::CollectRoutenodes ( mxgridref loc, tme::collections::entities& collection ) 
{
int ii;
//int count=0;

    collection.Clear();

    mxloc& mapsqr = gamemap->GetAt ( loc );

    if ( !mapsqr.IsRouteNode() )
        return 0;

    collection.Create(MAX_ROUTENODES_INLOCATION);


    for (ii = 0; ii < sv_routenodes; ii++) {
        mxroutenode* pNode = mx->RouteNodeById(ii+1);
        if ( pNode && pNode->Location() == loc ) {
            collection.Add(pNode);
        }
    }

    return collection.Compact();
}





cvarreg_t* mxengine::FindDBVariable ( const c_str& name ) const
{
    for ( int ii=0; ii<sv_variables; ii++ ) {
        if (c_stricmp( name, variables[ii].name ) == 0 )
            return &variables[ii];
    }
    return NULL ;
}

LPCSTR mxengine::EntitySymbolById ( mxid id )
{
    mxentity* entity = EntityByIdt(id);
    if ( entity == NULL )
        return "";
    
    return entity->Symbol();
}

mxentity* mxengine::EntityByName( const c_str& name, id_type_t type )
{
mxentity* obj;
    
    // do this the long way
    // TODO this needs to be optomised out with an index or something

    FIND_ENTITY(IDT_CHARACTER,objCharacters);

    FIND_ENTITY(IDT_REGIMENT,objRegiments);

    FIND_ENTITY(IDT_ROUTENODE,objRoutenodes);

    FIND_ENTITY(IDT_STRONGHOLD,objStrongholds);

    FIND_ENTITY(IDT_PLACE,objPlaces);

    FIND_ENTITY(IDT_MISSION,objMissions);

    FIND_ENTITY(IDT_VICTORY,objVictories);

    FIND_ENTITY(IDT_DIRECTIONINFO,objDirectionInfos);

    FIND_ENTITY(IDT_OBJECT,objObjects);

    FIND_ENTITY(IDT_UNITINFO,objUnitInfos);

    FIND_ENTITY(IDT_RACEINFO,objRaceInfos);

    FIND_ENTITY(IDT_GENDERINFO,objGenderInfos);

    FIND_ENTITY(IDT_TERRAININFO,objTerrainInfos);

    FIND_ENTITY(IDT_AREAINFO,objAreaInfos);

    FIND_ENTITY(IDT_COMMANDINFO,objCommandInfos);

    return NULL ;
}



mxentity* mxengine::EntityByIdt( mxid id )
{
//mxentity*    obj = NULL ;

    id_type_t type = ID_TYPE(id);
    int objectno = GET_ID(id);

    FIND_IDT(IDT_CHARACTER,CharacterById);
    
    FIND_IDT(IDT_REGIMENT,RegimentById);
    
    FIND_IDT(IDT_ROUTENODE,RouteNodeById);
    
    FIND_IDT(IDT_STRONGHOLD,StrongholdById);
    
    FIND_IDT(IDT_PLACE,PlaceById);
    
    FIND_IDT(IDT_MISSION,MissionById);
    
    FIND_IDT(IDT_VICTORY,VictoryById);
    
    FIND_IDT(IDT_DIRECTIONINFO,DirectionById);
    
    FIND_IDT(IDT_OBJECT,ObjectById);
    
    FIND_IDT(IDT_UNITINFO,UnitById);
    
    FIND_IDT(IDT_RACEINFO,RaceById);
    
    FIND_IDT(IDT_GENDERINFO,GenderById);
    
    FIND_IDT(IDT_TERRAININFO,TerrainById);
    
    FIND_IDT(IDT_AREAINFO,AreaById);
    
    FIND_IDT(IDT_COMMANDINFO,CommandById);

    return NULL;
}


MXRESULT mxengine::EntityLinkData( mxid id, const void* data )
{
    mxentity* e = mxengine::EntityByIdt(id);
    if ( e ) {
        e->SetUserData(data);
        return MX_OK ;
    }
    return MX_FAILED ;
}

MXRESULT mxengine::SaveDiscoveryMap ( const c_str& filename )
{
    if ( discoverymap ) {
        if ( !discoverymap->Save( filename) )
            return MX_FAILED ;
    }
    
    return MX_OK ;
}

MXRESULT mxengine::LoadDiscoveryMap ( const c_str& filename )
{
    if ( discoverymap == NULL ) {
        discoverymap = new mxdiscoverymap();
        gamemap->m_discoverymap = discoverymap ;
    }
    
    if ( !discoverymap->Load( filename) ) {
        discoverymap->Create( gamemap->Size() ) ;
    }
    
    // make sure we transfer any of the current map details
    // onto the discovery map
    
    discoverymap->TransferFromMap( gamemap );
    
    return MX_OK ;
}

void mxengine::debug (LPCSTR format, ... )
{
char msg_buffer[1024];
    
    va_list arglist ;

    va_start( arglist, format ) ;
    vsprintf( msg_buffer, format, arglist );
    va_end( arglist ) ;
    
    cocos2d::log("TME: %s",msg_buffer);

}




