//
//  tsvdatabaseloader.cpp
//  midnight
//

#include "../baseinc/tme_internal.h"
#include "tsvdatabaseloader.h"

namespace tme {

namespace {

    template<typename T>
    void PopulateItems ( tme::collections::entities<T>& collection, const TsvTable& table )
    {
        for ( auto& row : table ) {
            u32 id = row.GetU32(TsvField::Id);
            if ( id == 0 || id > collection.Count() )
                continue;
            auto entity = collection[id-1];
            entity->Id(id);
            entity->LoadTsv(row);
        }
    }

    template<typename T>
    void PopulateInfos ( tme::collections::entities<T>& collection, const TsvTable& table )
    {
        for ( auto& row : table ) {
            u32 id = row.GetU32(TsvField::Id);
            if ( id >= collection.Count() )
                continue;
            auto entity = collection[id];
            entity->Id(id);
            entity->LoadTsv(row);
        }
    }

    // every entity/info table carries these, regardless of what else it has
    std::vector<std::string> EntityColumns ( std::initializer_list<const char*> extra )
    {
        std::vector<std::string> columns = { TsvField::Version, TsvField::Id, TsvField::Symbol, TsvField::Flags };
        for ( auto e : extra )
            columns.push_back(e);
        return columns;
    }

    // mxitem-derived tables additionally carry a Location
    std::vector<std::string> ItemColumns ( std::initializer_list<const char*> extra )
    {
        std::vector<std::string> columns = EntityColumns({});
        columns.push_back(TsvField::Location);
        for ( auto e : extra )
            columns.push_back(e);
        return columns;
    }

    // mxinfo-derived tables additionally carry a Name
    std::vector<std::string> InfoColumns ( std::initializer_list<const char*> extra )
    {
        std::vector<std::string> columns = EntityColumns({});
        columns.push_back(TsvField::Name);
        for ( auto e : extra )
            columns.push_back(e);
        return columns;
    }

}
// anonymous namespace

bool TsvDatabaseLoader::Load ( mxscenario* scenario, const std::string& tsvDirectory )
{
    // declared up front so every TsvRow constructed below (even the
    // manifest's) can carry it - it's only populated once every table has
    // loaded, in the harvest pass further down
    TsvSymbolTable symbols;

    // the manifest declares the version every table below must agree with -
    // the TSV equivalent of the binary database's magic/version header
    TsvTable manifest;
    if ( !manifest.Load(tsvDirectory + "/database.tsv", symbols) || manifest.Count() == 0 ) {
        MXTRACE("TsvDatabaseLoader: failed to load 'database.tsv' manifest");
        return false;
    }
    if ( !manifest.ValidateHeader({ TsvField::Version }) )
        return false;

    u32 tsvVersion = manifest.Row(0).GetU32(TsvField::Version);
    if ( tsvVersion < DATABASEVERSION ) {
        MXTRACE("TsvDatabaseLoader: tsv database version %d is older than the supported version %d", (int)tsvVersion, (int)DATABASEVERSION);
        return false;
    }

    TsvTable areaInfo, commandInfo, directionInfo, genderInfo, raceInfo, terrainInfo, unitInfo;
    TsvTable routeNodes, waypoints, strongholds, regiments, objects, missions, victories, characters;
    TsvTable stringsTable, variablesTable;
#if defined(_DDR_) || defined(_CITADEL_)
    TsvTable objectTypeInfo, objectPowerInfo;
#endif

    struct { TsvTable* table; const char* file; std::vector<std::string> columns; } files[] = {
        { &areaInfo,        "areainfo.tsv",      InfoColumns({ TsvField::Area::Prefix }) },
        { &commandInfo,     "commandinfo.tsv",   InfoColumns({ TsvField::Command::SuccessTime, TsvField::Command::FailureTime }) },
        { &directionInfo,   "directioninfo.tsv", InfoColumns({}) },
        { &genderInfo,      "genderinfo.tsv",    InfoColumns({ TsvField::Gender::PersonalPronoun, TsvField::Gender::PossessivePronoun, TsvField::Gender::SingularPronoun }) },
        { &raceInfo,        "raceinfo.tsv",      InfoColumns({ TsvField::Race::DefaultSoldiersName, TsvField::Race::Success, TsvField::Race::InitialMovement, TsvField::Race::DiagonalMovement, TsvField::Race::RidingMultiplier, TsvField::Race::MovementMax, TsvField::Race::RestAmount, TsvField::Race::StrongholdStartups, TsvField::Race::MistTimeAdjustment, TsvField::Race::MistDespondencyAdjustment, TsvField::Race::EnergyAmount, TsvField::Race::EnergyAmountRiding }) },
        { &terrainInfo,     "terraininfo.tsv",   InfoColumns({ TsvField::Terrain::Preposition, TsvField::Terrain::Description, TsvField::Terrain::Success, TsvField::Terrain::Visibility, TsvField::Terrain::Obstruction, TsvField::Terrain::MovementCost }) },
        { &unitInfo,        "unitinfo.tsv",      InfoColumns({ TsvField::Unit::Success, TsvField::Unit::RestModifier }) },
        { &routeNodes,      "routenodes.tsv",    ItemColumns({ TsvField::RouteNode::RouteNodes }) },
        { &waypoints,       "waypoints.tsv",     ItemColumns({}) },
#if defined(_DDR_)
        { &strongholds,     "strongholds.tsv",   ItemColumns({ TsvField::Stronghold::OccupyingRace, TsvField::Stronghold::Race, TsvField::Stronghold::Type, TsvField::Stronghold::Total, TsvField::Stronghold::Min, TsvField::Stronghold::Max, TsvField::Stronghold::StrategicalSuccess, TsvField::Stronghold::OwnerSuccess, TsvField::Stronghold::EnemySuccess, TsvField::Stronghold::Influence, TsvField::Stronghold::Respawn, TsvField::Stronghold::Occupier, TsvField::Stronghold::Owner, TsvField::Stronghold::Terrain, TsvField::Stronghold::Energy }) },
#else
        { &strongholds,     "strongholds.tsv",   ItemColumns({ TsvField::Stronghold::OccupyingRace, TsvField::Stronghold::Race, TsvField::Stronghold::Type, TsvField::Stronghold::Total, TsvField::Stronghold::Min, TsvField::Stronghold::Max, TsvField::Stronghold::StrategicalSuccess, TsvField::Stronghold::OwnerSuccess, TsvField::Stronghold::EnemySuccess, TsvField::Stronghold::Influence, TsvField::Stronghold::Respawn, TsvField::Stronghold::Occupier, TsvField::Stronghold::Owner, TsvField::Stronghold::Terrain }) },
#endif
        { &regiments,       "regiments.tsv",     ItemColumns({ TsvField::Regiment::Race, TsvField::Regiment::Type, TsvField::Regiment::Total, TsvField::Regiment::Target, TsvField::Regiment::Orders, TsvField::Regiment::Success, TsvField::Regiment::Loyalty, TsvField::Regiment::Delay }) },
#if defined(_DDR_)
        { &objects,         "objects.tsv",       ItemColumns({ TsvField::Object::Kills, TsvField::Name, TsvField::Object::Description, TsvField::Object::UseDescription, TsvField::Object::CarriedBy, TsvField::Object::Type, TsvField::Object::Power }) },
#else
        { &objects,         "objects.tsv",       ItemColumns({ TsvField::Object::Kills, TsvField::Name, TsvField::Object::Description, TsvField::Object::UseDescription, TsvField::Object::CarriedBy }) },
#endif
        { &missions,        "missions.tsv",      EntityColumns({ TsvField::Mission::Priority, TsvField::Mission::Objective, TsvField::Mission::Condition, TsvField::Mission::References, TsvField::Mission::Points, TsvField::Mission::Scorer, TsvField::Mission::Action, TsvField::Mission::ActionId }) },
        { &victories,       "victories.tsv",     EntityColumns({ TsvField::Victory::Priority, TsvField::Victory::Mission, TsvField::Victory::String }) },
#if defined(_DDR_)
        { &characters,      "characters.tsv",    ItemColumns({ TsvField::Character::LongName, TsvField::Character::ShortName, TsvField::Character::Recruit, TsvField::Character::Group, TsvField::Character::Looking, TsvField::Character::Time, TsvField::Character::Race, TsvField::Character::Gender, TsvField::Character::Loyalty, TsvField::Character::Energy, TsvField::Character::Reckless, TsvField::Character::Strength, TsvField::Character::Cowardly, TsvField::Character::Courage, TsvField::Character::Despondency, TsvField::Character::Fear, TsvField::Character::Orders, TsvField::Character::Carrying, TsvField::Character::Warriors, TsvField::Character::Riders, TsvField::Character::Following, TsvField::Character::Foe, TsvField::Character::Liege, TsvField::Character::Traits, TsvField::Character::Home, TsvField::Character::DesiredObject }) },
#else
        { &characters,      "characters.tsv",    ItemColumns({ TsvField::Character::LongName, TsvField::Character::ShortName, TsvField::Character::Recruit, TsvField::Character::Group, TsvField::Character::Looking, TsvField::Character::Time, TsvField::Character::Race, TsvField::Character::Gender, TsvField::Character::Loyalty, TsvField::Character::Energy, TsvField::Character::Reckless, TsvField::Character::Strength, TsvField::Character::Cowardly, TsvField::Character::Courage, TsvField::Character::Despondency, TsvField::Character::Fear, TsvField::Character::Orders, TsvField::Character::Carrying, TsvField::Character::Warriors, TsvField::Character::Riders, TsvField::Character::Following, TsvField::Character::Foe, TsvField::Character::Liege, TsvField::Character::Traits }) },
#endif
        { &stringsTable,    "strings.tsv",       { TsvField::Version, TsvField::Id, TsvField::Symbol, TsvField::DatabaseString::Text } },
        { &variablesTable,  "variables.tsv",     { TsvField::Version, TsvField::Symbol, TsvField::DatabaseVariable::Value } },
#if defined(_DDR_)
        { &objectTypeInfo,  "objecttypeinfo.tsv",  InfoColumns({}) },
        { &objectPowerInfo, "objectpowerinfo.tsv", InfoColumns({}) },
#endif
    };

    for ( auto& entry : files ) {
        if ( !entry.table->Load(tsvDirectory + "/" + entry.file, symbols) ) {
            MXTRACE("TsvDatabaseLoader: failed to load '%s'", entry.file);
            return false;
        }
        if ( !entry.table->ValidateHeader(entry.columns) ) {
            MXTRACE("TsvDatabaseLoader: '%s' is missing required columns", entry.file);
            return false;
        }
        if ( !entry.table->ValidateVersion(tsvVersion) ) {
            MXTRACE("TsvDatabaseLoader: '%s' has rows that don't match database version %d", entry.file, (int)tsvVersion);
            return false;
        }
    }

    // pass 1: harvest every symbol first, so a reference from any table
    // (e.g. a character's Liege) resolves regardless of table order
    auto harvest = [&symbols] ( const TsvTable& table, id_type_t type ) {
        for ( auto& row : table )
            symbols.Add(MAKE_ID(type, row.GetU32(TsvField::Id)), row.GetString(TsvField::Symbol));
    };

    harvest(areaInfo, IDT_AREAINFO);
    harvest(commandInfo, IDT_COMMANDINFO);
    harvest(directionInfo, IDT_DIRECTIONINFO);
    harvest(genderInfo, IDT_GENDERINFO);
    harvest(raceInfo, IDT_RACEINFO);
    harvest(terrainInfo, IDT_TERRAININFO);
    harvest(unitInfo, IDT_UNITINFO);
    harvest(routeNodes, IDT_ROUTENODE);
    harvest(waypoints, IDT_PLACE);
    harvest(strongholds, IDT_STRONGHOLD);
    harvest(regiments, IDT_REGIMENT);
    harvest(objects, IDT_OBJECT);
    harvest(missions, IDT_MISSION);
    harvest(victories, IDT_VICTORY);
    harvest(characters, IDT_CHARACTER);
#if defined(_DDR_) || defined(_CITADEL)
    harvest(objectTypeInfo, IDT_OBJECT_TYPE);
    harvest(objectPowerInfo, IDT_OBJECT_POWER);
#endif

    // strings resolve to a 1-based mxid - see mxtext::StringByName
    for ( auto& row : stringsTable )
        symbols.Add(MAKE_ID(IDT_STRING, row.GetU32(TsvField::Id)+1), row.GetString(TsvField::Symbol));

    // pass 2: create blank shells for every table, same order as
    // mxengine::LoadDatabase, so any reference resolved below always
    // finds a live (if not yet populated) target
    mx->objCharacters.Create(scenario, IDT_CHARACTER, characters.Count());
    mx->objRegiments.Create(scenario, IDT_REGIMENT, regiments.Count());
    mx->objRoutenodes.Create(scenario, IDT_ROUTENODE, routeNodes.Count());
    mx->objStrongholds.Create(scenario, IDT_STRONGHOLD, strongholds.Count());
    mx->objPlaces.Create(scenario, IDT_PLACE, waypoints.Count());
    mx->objObjects.Create(scenario, IDT_OBJECT, objects.Count());
    mx->objMissions.Create(scenario, IDT_MISSION, missions.Count());
    mx->objVictories.Create(scenario, IDT_VICTORY, victories.Count());
    mx->objDirectionInfos.Create(scenario, IDT_DIRECTIONINFO, directionInfo.Count());
    mx->objUnitInfos.Create(scenario, IDT_UNITINFO, unitInfo.Count());
    mx->objRaceInfos.Create(scenario, IDT_RACEINFO, raceInfo.Count());
    mx->objGenderInfos.Create(scenario, IDT_GENDERINFO, genderInfo.Count());
    mx->objTerrainInfos.Create(scenario, IDT_TERRAININFO, terrainInfo.Count());
    mx->objAreaInfos.Create(scenario, IDT_AREAINFO, areaInfo.Count());
    mx->objCommandInfos.Create(scenario, IDT_COMMANDINFO, commandInfo.Count());
#if defined(_DDR_) || defined(_CITADEL)
    mx->objObjectTypesInfos.Create(scenario, IDT_OBJECT_TYPE, objectTypeInfo.Count());
    mx->objObjectPowersInfos.Create(scenario, IDT_OBJECT_POWER, objectPowerInfo.Count());
#endif

    // pass 3: populate every field. Items are 1-based, infos are 0-based -
    // matches the ACCESS_ITEM/ACCESS_INFO indexing in mxengine.cpp
    PopulateItems<mxcharacter*>(mx->objCharacters, characters);
    PopulateItems<mxregiment*>(mx->objRegiments, regiments);
    PopulateItems<mxroutenode*>(mx->objRoutenodes, routeNodes);
    PopulateItems<mxstronghold*>(mx->objStrongholds, strongholds);
    PopulateItems<mxplace*>(mx->objPlaces, waypoints);
    PopulateItems<mxobject*>(mx->objObjects, objects);
    PopulateItems<mxmission*>(mx->objMissions, missions);
    PopulateItems<mxvictory*>(mx->objVictories, victories);

    PopulateInfos<mxdirection*>(mx->objDirectionInfos, directionInfo);
    PopulateInfos<mxunitinfo*>(mx->objUnitInfos, unitInfo);
    PopulateInfos<mxrace*>(mx->objRaceInfos, raceInfo);
    PopulateInfos<mxgender*>(mx->objGenderInfos, genderInfo);
    PopulateInfos<mxterrain*>(mx->objTerrainInfos, terrainInfo);
    PopulateInfos<mxarea*>(mx->objAreaInfos, areaInfo);
    PopulateInfos<mxcommand*>(mx->objCommandInfos, commandInfo);
#if defined(_DDR_) || defined(_CITADEL)
    PopulateInfos<mxobjecttype*>(mx->objObjectTypesInfos, objectTypeInfo);
    PopulateInfos<mxobjectpower*>(mx->objObjectPowersInfos, objectPowerInfo);
#endif

    mx->text->LoadTsv(stringsTable);
    mx->LoadVariablesTsv(variablesTable);

    return true;
}

}
// namespace tme
