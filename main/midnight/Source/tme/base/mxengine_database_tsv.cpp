/*
 *  mxengine_database_tsv.cpp
 *  tme
 *
 *  Created by Chris Wild on 02/02/2011.
 *  Copyright 2011 Chilli Hugger. All rights reserved.
 *
 */

#include "../baseinc/tme_internal.h"
#include "axmol.h"

USING_NS_TME ;

/*
 * Function name    : mxengine::LoadDatabaseFromTsv
 *
 * Return type        : MXRESULT
 *
 * Arguments        : const std::string& tsvDirectory
 */

MXRESULT mxengine::LoadDatabaseFromTsv ( const std::string& tsvDirectory )
{
    MXTRACE( "Building Database from TSV '%s'", tsvDirectory.c_str());

    savegameversion = DATABASEVERSION;

    TsvDatabaseLoader loader;
    if ( !loader.Load(scenario, tsvDirectory) )
        return MX_FAILED;

    sv_characters  = (s32)objCharacters.Count();
    sv_regiments   = (s32)objRegiments.Count();
    sv_routenodes  = (s32)objRoutenodes.Count();
    sv_strongholds = (s32)objStrongholds.Count();
    sv_places      = (s32)objPlaces.Count();
    sv_objects     = (s32)objObjects.Count();
    sv_missions    = (s32)objMissions.Count();
    sv_victories   = (s32)objVictories.Count();
    sv_directions  = (s32)objDirectionInfos.Count();
    sv_units       = (s32)objUnitInfos.Count();
    sv_races       = (s32)objRaceInfos.Count();
    sv_genders     = (s32)objGenderInfos.Count();
    sv_terrains    = (s32)objTerrainInfos.Count();
    sv_areas       = (s32)objAreaInfos.Count();
    sv_commands    = (s32)objCommandInfos.Count();
#if defined(_DDR_) || defined(_CITADEL)
    sv_object_types  = (s32)objObjectTypesInfos.Count();
    sv_object_powers = (s32)objObjectPowersInfos.Count();
#endif

    return MX_OK;
}

/*
 * Function name    : mxengine::SaveDatabaseCache
 *
 * Return type        : MXRESULT
 *
 * Arguments        : const std::string& filename
 */

MXRESULT mxengine::SaveDatabaseCache ( const std::string& filename )
{
    chilli::os::file* pFile = new chilli::os::file ( filename.c_str(), chilli::os::file::modeReadWrite|chilli::os::file::modeCreate );
    if ( pFile == NULL || !pFile->IsOpen() ) {
        if ( pFile ) delete pFile;
        return MX_FAILED;
    }

    archive ar (pFile, archive::store | archive::bNoFlushOnDelete );

    ar << TME_MAGIC_NO ;
    ar << (int)scenario->GetInfoBlock()->Id;
    ar << (u32)DATABASEVERSION ;
    ar << (char*)DATABASEHEADER ;

    // counts first, matching LoadDatabaseBinary's read order exactly -
    // the entity data itself is written afterwards, in a separate pass
    ar << sv_characters ;
    ar << sv_regiments ;
    ar << sv_routenodes ;
    ar << sv_strongholds ;
    ar << sv_places ;
    ar << sv_objects ;
    ar << sv_missions ;
    ar << sv_victories ;
    ar << sv_directions ;
    ar << sv_units ;
    ar << sv_races ;
    ar << sv_genders ;
    ar << sv_terrains ;
    ar << sv_areas ;
    ar << sv_commands ;
    ar << sv_variables ;

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

    text->Serialize(ar);

    for ( int ii=0; ii<sv_variables; ii++ ) {
        ar << (char*)variables[ii].name;
        ar << (char*)variables[ii].currentValue;
        ar << (int)variables[ii].type;
    }

#if defined(_DDR_) || defined(_CITADEL)
    // matches LoadDatabaseBinary's read order exactly: count+data
    // interleaved per table here, unlike the main tables above
    ar << sv_object_types ;
    objObjectTypesInfos.Serialize(ar);

    ar << sv_object_powers ;
    objObjectPowersInfos.Serialize(ar);
#endif

    ar.Close();

    SAFEDELETE ( pFile );

    return MX_OK;
}

/*
 * Function name    : mxengine::LoadVariablesTsv
 *
 * Return type        : void
 *
 * Arguments        : const TsvTable& table
 */

void mxengine::LoadVariablesTsv ( const TsvTable& table )
{
    SAFEDELETEARRAY ( variables );

    sv_variables = (s32)table.Count();
    variables = new cvarreg_t[sv_variables];

    for ( u32 ii=0; ii<table.Count(); ii++ ) {
        const TsvRow& row = table.Row(ii);
        variables[ii].memory = nullptr;
        variables[ii].name = chilli::lib::c_strdup(row.GetString(TsvField::Symbol).c_str());
        variables[ii].currentValue = chilli::lib::c_strdup(row.GetString(TsvField::DatabaseVariable::Value).c_str());
        variables[ii].type = CVar::VNONE;
    }
}
