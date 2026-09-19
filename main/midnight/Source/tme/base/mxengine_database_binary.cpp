/*
 *  mxengine_database_binary.cpp
 *  tme
 *
 *  Created by Chris Wild on 02/02/2011.
 *  Copyright 2011 Chilli Hugger. All rights reserved.
 *
 */

#include "../baseinc/tme_internal.h"

USING_NS_TME ;

/*
 * Function name    : mxengine::LoadDatabaseBinary
 *
 * Return type        : MXRESULT
 *
 * Arguments        : const std::string& filename
 */

MXRESULT mxengine::LoadDatabaseBinary ( const std::string& filename )
{
int ii;
int id;

MXTRACE( "Loading Database '%s'", filename.c_str());

   chilli::os::file* pFile = new chilli::os::file ( filename.c_str(), chilli::os::file::modeRead );
    if ( !pFile->IsOpen() ) {
        if ( pFile ) delete pFile;
        MXTRACE( "Cannot Load data file %s", filename.c_str() );
        return MX_FAILED;
    }

    archive ar (pFile, archive::load | archive::bNoFlushOnDelete);

    // serialize is not for save game
    m_savegame = FALSE ;

u32        magicno;
u32        scenarioid;
//u32        versionno;
std::string    header;

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
MXTRACE( "Header='%s'", header.c_str());

    if (c_stricmp( header.c_str(), DATABASEHEADER ) != 0 ) {
        MXTRACE("Invalid DATABASE Header");
        return MX_UNKNOWN_FILE;
    }

    ar >> sv_characters ;   objCharacters.Create(scenario,IDT_CHARACTER,sv_characters);
    ar >> sv_regiments ;    objRegiments.Create(scenario,IDT_REGIMENT,sv_regiments);
    ar >> sv_routenodes ;   objRoutenodes.Create(scenario,IDT_ROUTENODE,sv_routenodes);
    ar >> sv_strongholds ;  objStrongholds.Create(scenario,IDT_STRONGHOLD,sv_strongholds);
    ar >> sv_places ;       objPlaces.Create(scenario,IDT_PLACE,sv_places);
    ar >> sv_objects ;      objObjects.Create(scenario,IDT_OBJECT,sv_objects);
    ar >> sv_missions ;     objMissions.Create(scenario,IDT_MISSION,sv_missions);
    ar >> sv_victories ;    objVictories.Create(scenario,IDT_VICTORY,sv_victories);
    ar >> sv_directions ;   objDirectionInfos.Create(scenario,IDT_DIRECTIONINFO,sv_directions);
    ar >> sv_units ;        objUnitInfos.Create(scenario,IDT_UNITINFO,sv_units);
    ar >> sv_races ;        objRaceInfos.Create(scenario,IDT_RACEINFO,sv_races);
    ar >> sv_genders ;      objGenderInfos.Create(scenario,IDT_GENDERINFO,sv_genders);
    ar >> sv_terrains ;     objTerrainInfos.Create(scenario,IDT_TERRAININFO,sv_terrains);
    ar >> sv_areas ;        objAreaInfos.Create(scenario,IDT_AREAINFO,sv_areas);
    ar >> sv_commands ;     objCommandInfos.Create(scenario,IDT_COMMANDINFO,sv_commands);
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
        variables[ii].memory=nullptr;
        variables[ii].name=nullptr;
        variables[ii].currentValue=nullptr;
        variables[ii].type=CVar::VNONE;

        ar >> (char**)&variables[ii].name;
        ar >> (char**)&variables[ii].currentValue;
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

    return MX_OK ;
}
