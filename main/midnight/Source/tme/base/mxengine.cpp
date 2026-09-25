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
#include "../base/collections.h"
#include "../utils/savegamemapping.h"
#include "axmol.h"
#include <stdio.h>


USING_NS_TME ;

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

    text = nullptr;
    night = nullptr;
    battle = nullptr;
    scenario = nullptr;
    gamemap = nullptr ;
    discoverymap = nullptr ;
    variables = nullptr ;
    pfnNightCallback = nullptr;
    m_CurrentCharacter = nullptr;
    defaultscenario = false ;
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
bool mxengine::InstallScenario ( const std::string& filename )
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

MXRESULT mxengine::SetDatabaseDirectory ( const std::string& directory )
{
    MX_REGISTER_SELF;

    m_szDatabase =  directory;
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
MXRESULT mxengine::LoadDatabase ( RULEFLAGS rules, mxdifficulty_t difficulty )
{
    MX_REGISTER_SELF;

    variables::Init(1);
    m_savegame = FALSE ;

    auto writablePath = ax::FileUtils::getInstance()->getWritablePath();
    auto scenarioIdStr = std::to_string((int)scenario->GetInfoBlock()->Id);
    auto tsvDirectory = m_szDatabase + "/" + scenarioIdStr + "/tsv";
    auto tmxFilename  = m_szDatabase + "/" + scenarioIdStr + "/tmx/map.tmx";

    // the fully-resolved database+map, written once at the end of the slow
    // path below (TSV/TMX parsed, TMX character positions applied) so a
    // later run can skip straight past TSV/TMX parsing entirely
    auto databaseCacheFile = writablePath + "/database_cache_" + scenarioIdStr;
    auto mapCacheFile      = writablePath + "/map_cached_" + scenarioIdStr;

    // the cache only lives for this app run - a leftover from a previous
    // launch could be stale (e.g. .tsv/.tmx edits during development), so
    // wipe it once per process before ever trusting one
    static bool cacheChecked = false;
    if ( !cacheChecked ) {
        cacheChecked = true;
        chilli::os::filemanager::Remove(databaseCacheFile);
        chilli::os::filemanager::Remove(mapCacheFile);
    }

    MXRESULT result = MX_FAILED ;
    gamemap = nullptr;

    if ( chilli::os::filemanager::Exists(databaseCacheFile) && chilli::os::filemanager::Exists(mapCacheFile) ) {
        MXTRACE( "Loading cached Database '%s'", databaseCacheFile.c_str());
        result = LoadDatabaseBinary(databaseCacheFile);
        if ( result == MX_OK ) {
            MXTRACE( "Loading cached Map '%s'", mapCacheFile.c_str());
            gamemap = new mxmap();
            if ( !gamemap->Load(mapCacheFile) ) {
                SAFEDELETE ( gamemap );
                result = MX_FAILED;
            }
        }
        if ( result != MX_OK )
            MXTRACE( "Cached Database/Map invalid, rebuilding" );
    }

    bool builtFromSource = false;

    if ( result != MX_OK ) {

        // prefer the bundled .tsv source files when present (ticket #329) -
        // falls through to the pre-generated binary below when they aren't
        if ( chilli::os::filemanager::ExistsDir(tsvDirectory) ) {
            result = LoadDatabaseFromTsv(tsvDirectory);
        }

        if ( result != MX_OK ) {

            std::string filename = m_szDatabase + "/" + scenarioIdStr + "/database";

        // We need to move the default database into an accessible folder
#if !defined(_OS_DESKTOP_)

            auto database = filename ;
            filename = writablePath + "/database_" + scenarioIdStr;

            MXTRACE( "Copying Database '%s' from '%s' to '%s'",
                m_szDatabase.c_str(),
                database.c_str(),
                filename.c_str());

            chilli::os::filemanager::Copy(database.c_str(), filename.c_str());
#endif

            result = LoadDatabaseBinary(filename);
        }

        if ( result != MX_OK )
            return result;

MXTRACE("Loading MAP");

        // prefer a bundled .tmx map file when present (ticket #329) - falls
        // through to the pre-generated binary map file below when it isn't
        bool loadedFromTmx = LoadMapTmx(tmxFilename) == MX_OK;

        if ( !loadedFromTmx ) {
            auto mapFilename = m_szDatabase + "/" + scenarioIdStr + "/map";
            if ( LoadMapBinary(mapFilename) != MX_OK )
                return MX_FAILED;
        }

        if ( loadedFromTmx )
            ApplyMapEntitiesFromTmx(tmxFilename);

        builtFromSource = true;
    }

MXTRACE( "Update Variables");
    variables::Update();

    if ( builtFromSource ) {
        // cache the fully-resolved database+map regardless of which route
        // built them, so the next run can load both straight from cache
        SaveDatabaseCache(databaseCacheFile);
        gamemap->Save(mapCacheFile);
    }

    gamemap->ClearVisible();


MXTRACE( "Init Variables");
    variables::Init(2);

    sv_days = 0;
    sv_strongholdadjuster = 0;
    m_difficulty = difficulty ;
    setRules(rules);

    scenario->initialise(SaveGameVersion());
    scenario->initialiseAfterCreate(SaveGameVersion());

    CurrentChar( (mxcharacter*)mx->EntityByIdt(sv_character_default[0])) ;

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
    
#if defined(_DDR_) || defined(_CITADEL_)
    objObjectPowersInfos.Destroy();
    objObjectTypesInfos.Destroy();
#endif

    /* TODO: Fix this - problem with static definition and non static after load and save */
    for ( int ii=0; ii<sv_variables; ii++ ) {
        void* temp = (void*)variables[ii].name ;
        SAFEFREE(temp);
        temp = (void*)variables[ii].currentValue;
        SAFEFREE(temp);
        variables[ii].name=NULL;
        variables[ii].currentValue=NULL;
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

#if defined(_DDR_) || defined(_CITADEL_)
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
    sv_controlled_character = MAKE_ID(IDT_CHARACTER,mxentity::SafeId(character));

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

MXRESULT mxengine::LoadGame ( const std::string& filename, PFNSERIALIZE function )
{
    chilli::os::file* pFile = new chilli::os::file ( filename.c_str(), chilli::os::file::modeRead );
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
std::string    header;
std::string  description;

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
    if ( scenarioid != (int)scenario->GetInfoBlock()->Id )
        return MX_UNKNOWN_FILE;

    ar >> savegameversion ;

    ar >> header ;
    if ( strcmp( header.c_str(), SAVEGAMEHEADER ) != 0 )
        return MX_UNKNOWN_FILE;

    if ( SaveGameVersion()>=9 ) {
        ar >>  description;
    }else{
        description="";
    }

    if ( SaveGameVersion()>=13 ) {
        ar >> m_ruleFlags;
    }else{
        setRules(RF_DEFAULT);
    }
    if ( SaveGameVersion()>=14 ) {
        READ_ENUM(m_difficulty);
        u32 seed; ar >> seed;
        if(m_difficulty == DF_MEDIUM || m_difficulty == DF_HARD ) {
            randomno::instance->seed(seed);
        }
    }else{
        m_difficulty = DF_NORMAL;
    }

    /* load the game map */
    gamemap->Serialize ( ar );

    ar >> sv_characters ;   objCharacters.Create(scenario,IDT_CHARACTER,sv_characters);
    ar >> sv_regiments ;    objRegiments.Create(scenario,IDT_REGIMENT,sv_regiments);
    ar >> sv_routenodes ;   objRoutenodes.Create(scenario,IDT_ROUTENODE,sv_routenodes);
    ar >> sv_strongholds ;  objStrongholds.Create(scenario,IDT_STRONGHOLD,sv_strongholds);
    ar >> sv_missions ;     objMissions.Create(scenario,IDT_MISSION,sv_missions);
    ar >> sv_victories ;    objVictories.Create(scenario,IDT_VICTORY,sv_victories);
    ar >> sv_objects;       objObjects.Create(scenario,IDT_OBJECT,sv_objects);

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

    /* Load Character memories */
    FOR_EACH_CHARACTER(character) {
        character->memory.Serialize ( ar );
    }

    /* Load scenario specific */
    scenario->Serialize ( ar );

    /* load Frontend Specific */
    if ( function!= NULL )
        function(SaveGameVersion(),ar);


    ar.Close();

    SAFEDELETE ( pFile );

    mx->CurrentChar ( m_CurrentCharacter );

    scenario->initialise(SaveGameVersion());
    scenario->updateAfterLoad(SaveGameVersion());

    return MX_OK ;
}


MXRESULT mxengine::SaveGameDescription ( const std::string& filename, std::string& description )
{
    chilli::os::file* pFile = new chilli::os::file ( filename.c_str(), chilli::os::file::modeRead );
    if ( !pFile->IsOpen() ) {
        if ( pFile ) delete pFile;
        return MX_FAILED;
    }
    
    archive ar (pFile, archive::load | archive::bNoFlushOnDelete);
    
    // serialize is for save game
    m_savegame = TRUE ;
    
    u32         magicno;
    u32         scenarioid;
    std::string header;
    
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
    if ( scenarioid != (int)scenario->GetInfoBlock()->Id )
        return MX_UNKNOWN_FILE;
    
    ar >> savegameversion ;
     
    ar >> header ;
    if ( strcmp( header.c_str(), SAVEGAMEHEADER ) != 0 )
        return MX_UNKNOWN_FILE;
    
    // version 9
    std::string temp;
    if ( SaveGameVersion() >= 9 ) {
        ar >>  temp;
    }else{
        temp="";
    }
    description = temp;
    
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

MXRESULT mxengine::SaveGame ( const std::string& filename, PFNSERIALIZE function )
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

    chilli::os::file* pFile = new chilli::os::file ( filename.c_str(), chilli::os::file::modeReadWrite|chilli::os::file::modeCreate );
    if ( pFile == NULL || !pFile->IsOpen() ) {
        if ( pFile ) delete pFile;
        //COMPLAIN( "Cannot Save data file %s", filename );
        return MX_FAILED;
    }

    archive ar (pFile, archive::store | archive::bNoFlushOnDelete );

    // serialize is for save game
    m_savegame = TRUE ;

    ar << TME_MAGIC_NO ;
    ar << (int)scenario->GetInfoBlock()->Id;
    ar << SAVEGAMEVERSION ;
    ar << (char*)SAVEGAMEHEADER ;

    // version 9
    // LPCSTR description = "DAY 000: Luxor - Tower of the Moon";
    int lords=0;
    char buffer[1024];
    
    FOR_EACH_CHARACTER(c) {
        if ( scenario->CanWeSelectCharacter(c))
        if ( c->IsRecruited() && c->IsAlive() )
            lords++;
    }
    
    snprintf( buffer, NUMELE(buffer), "Day %d\n%s\n%s\n%d %s"
            , (int)sv_days+1
            , m_CurrentCharacter->Longname().c_str()
            , mx->text->DescribeLocation(m_CurrentCharacter->Location()).c_str()
            , lords
            , lords==1 ? "lord" : "lords"
            );
    
    ar << (char*)buffer ;
    
    // version 13
    ar << m_ruleFlags;
    
    // version 14
    WRITE_ENUM(m_difficulty);
    ar << randomno::instance->seed();

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
    FOR_EACH_CHARACTER(c) {
        c->memory.Serialize ( ar );
    }

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

u32 mxengine::CollectRegiments ( mxgridref loc, c_regiment& collection )
{
    collection.Create(MAX_REGIMENTS_INLOCATION);

    FOR_EACH_REGIMENT(regiment) {
        if ( regiment && regiment->Total() && regiment->Location() == loc )
            collection.Add(regiment) ;
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

u32 mxengine::CollectStrongholds ( mxgridref loc, c_stronghold& collection )
{
    collection.Clear();

    mxloc& mapsqr = gamemap->GetAt ( loc );

    if ( !mapsqr.IsStronghold() )
        return 0;

    collection.Create(MAX_STRONGHOLDS_INLOCATION);

    // count
    FOR_EACH_STRONGHOLD(stronghold) {
        if ( stronghold && stronghold->Location() == loc ) {
            collection.Add(stronghold);
        }
    }

    return collection.Compact();
}


u32 mxengine::CollectRoutenodes ( mxgridref loc, c_routenode& collection ) 
{
    collection.Clear();

    mxloc& mapsqr = gamemap->GetAt ( loc );

    if ( !mapsqr.IsRouteNode() )
        return 0;

    collection.Create(MAX_ROUTENODES_INLOCATION);

    FOR_EACH_ROUTENODE(routenode) {
        if ( routenode && routenode->Location() == loc ) {
            collection.Add(routenode);
        }
    }

    return collection.Compact();
}





cvarreg_t* mxengine::FindDBVariable ( const std::string& name ) const
{
    for ( int ii=0; ii<sv_variables; ii++ ) {
        if (c_stricmp( name.c_str(), variables[ii].name ) == 0 )
            return &variables[ii];
    }
    return NULL ;
}

LPCSTR mxengine::EntitySymbolById ( mxid id )
{
    mxentity* entity = EntityByIdt(id);
    if ( entity == NULL )
        return "";
    
    return entity->Symbol().c_str();
}

mxentity* mxengine::EntityByName( const std::string& name, id_type_t type )
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

MXRESULT mxengine::SaveDiscoveryMap ( const std::string& filename )
{
    if ( discoverymap ) {
        if ( !discoverymap->Save(filename) )
            return MX_FAILED ;
    }
    
    return MX_OK ;
}

MXRESULT mxengine::LoadDiscoveryMap ( const std::string& filename )
{
    if ( discoverymap == NULL ) {
        discoverymap = new mxdiscoverymap();
        gamemap->m_discoverymap = discoverymap ;
    }
    
    if ( !discoverymap->Load(filename) ) {
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
    vsnprintf( msg_buffer, NUMELE(msg_buffer), format, arglist );
    va_end( arglist ) ;
    printf("TME: %s\n", msg_buffer);

}




