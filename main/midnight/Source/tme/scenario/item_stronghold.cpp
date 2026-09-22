/*
 * FILE:    item_stronghold.cpp
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

namespace tme {

const int STRONGHOLD_SUCCESS_NONE = 0;

mxstronghold::mxstronghold() :
    race(RA_NONE),
    occupyingrace(RA_NONE),
    type(UT_NONE),
    terrain(TN_NONE),
    owner(nullptr),
    occupier(nullptr),
    totaltroops(0),
    lost(0),
    killed(0),
    mintroops(0),
    maxtroops(0),
    strategical_success(0),
    owner_success(0),
    enemy_success(0),
    respawn(0),
    influence(0)
{
    mxentity::idType = IDT_STRONGHOLD ;
}

mxstronghold::~mxstronghold()
{
}


/*
 * Function name    : mxstronghold::Serialize
 * 
 * Return type        : void
 * 
 * Arguments        : archive& ar
 * 
 * Description        : 
 * 
 */

void mxstronghold::Serialize ( archive& ar )
{
    mxitem::Serialize(ar);
    if ( ar.IsStoring() ) {
        WRITE_ENUM( occupyingrace );
        WRITE_ENUM( race );
        WRITE_ENUM( type );
        ar << totaltroops;
        ar << mintroops;
        ar << maxtroops;
        ar << strategical_success;
        ar << owner_success;
        ar << enemy_success;
        ar << influence;
        ar << respawn;
        ar << occupier;
        ar << owner;
        WRITE_ENUM( terrain );
        ar << killed;

    }else{
        READ_ENUM ( occupyingrace );
        READ_ENUM ( race );
        READ_ENUM ( type );
        ar >> totaltroops;
        ar >> mintroops;
        ar >> maxtroops;
        ar >> strategical_success;
        ar >> owner_success;
        ar >> enemy_success;
        ar >> influence;
        ar >> respawn;
        ar >> occupier;
        ar >> owner;
        READ_ENUM ( terrain );
        ar >> killed;
        lost = 0;
    }
}

void mxstronghold::LoadTsv ( const TsvRow& row )
{
    mxitem::LoadTsv(row);

    occupyingrace = row.GetRace(TsvField::Stronghold::OccupyingRace);
    race = row.GetRace(TsvField::Stronghold::Race);
    type = row.GetUnitType(TsvField::Stronghold::Type);
    totaltroops = row.GetU32(TsvField::Stronghold::Total);
    mintroops = row.GetU32(TsvField::Stronghold::Min);
    maxtroops = row.GetU32(TsvField::Stronghold::Max);
    strategical_success = row.GetU32(TsvField::Stronghold::StrategicalSuccess);
    owner_success = row.GetU32(TsvField::Stronghold::OwnerSuccess);
    enemy_success = row.GetU32(TsvField::Stronghold::EnemySuccess);
    influence = row.GetU32(TsvField::Stronghold::Influence);
    respawn = row.GetU32(TsvField::Stronghold::Respawn);
    occupier = row.GetCharacter(TsvField::Stronghold::Occupier);
    owner = row.GetCharacter(TsvField::Stronghold::Owner);
    terrain = row.GetTerrain(TsvField::Stronghold::Terrain);
    killed = 0;
    lost = 0;
}

/*
s32 mxstronghold::BattleSuccess ( const CLocInfo& locinfo )
{
int temp;
    if ( this == NULL ) return STRONGHOLD_SUCCESS_NONE ;
    temp=0;
    temp = HasFallen() ? enemy_success : owner_success ;
    temp += mx->TerrainById(terrain)->Success();
    temp += mx->UnitById(Type())->Success();
    temp += mx->RaceById(Race())->Success();
    temp += strategical_success ;
    return  temp ;
}
*/


/*
 * Function name    : mxstronghold::BattleSuccess
 * 
 * Return type        : int
 * 
 * Arguments        : const CLocInfo& locinfo
 * 
 * Description        : 
 * 
 */

#if !defined _MSC_VER
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-undefined-compare"
#endif
    
s32 mxstronghold::BattleSuccess ( const mxlocinfo& locinfo )
{
    if ( this == nullptr ) return STRONGHOLD_SUCCESS_NONE ;
    if ( OccupyingRace() == RA_ENEMY )
        return enemy_success + mx->scenario->BaseDoomdarkSuccess( OccupyingRace(),Type(), locinfo );
    return mx->UnitById(Type())->Success()+mx->RaceById(OccupyingRace())->Success() ;
}

#if !defined _MSC_VER
#pragma clang diagnostic pop
#endif

    
/*
 * Function name    : mxstronghold::Remove
 * 
 * Return type        : int
 * 
 * Arguments        : mxrace_t race
 *                  : mxunit_t type
 *                  : int amount
 * 
 * Description        : 
 * 
 */

u32 mxstronghold::Remove ( mxrace_t race, mxunit_t type, u32 amount )
{
    auto result = (s32)(TotalTroops()-amount);
    if ( result < (s32)MinTroops() )
        amount = TotalTroops() - MinTroops();

    totaltroops -= amount;
    return amount;
}


/*
 * Function name    : mxstronghold::Add
 * 
 * Return type        : int
 * 
 * Arguments        : mxrace_t race
 *                  : mxunit_t type
 *                  : int amount
 * 
 * Description        : 
 * 
 */

u32 mxstronghold::Add ( mxrace_t race, mxunit_t type, u32 amount )
{
    if ( (TotalTroops()+amount) > MaxTroops() )
        amount = MaxTroops()- TotalTroops();
    totaltroops += amount;
    return amount;
}

mxrace_t mxstronghold::Loyalty() const
{
    RETURN_IF_NULL(Owner()) Race();
    return Owner()->NormalisedLoyalty();
}

bool mxstronghold::IsFriend( const mxcharacter* character ) const
{
    RETURN_IF_NULL(character) false;
    return Loyalty() == character->NormalisedLoyalty();
}

bool mxstronghold::CanCharacterRecruitOrPost(const mxcharacter* character) const
{
    RETURN_IF_NULL(character) false;
    return OccupyingRace() == character->Race();
}

//
// Is this stronghold held against the player - do its armies fight a lord who stands in it?
//
// In The Citadel only a keep Boroth's host has TAKEN is his to hold. The Dark Fey's own keeps
// stay out of the war for now: Maranor holds the hostages the player frees by approaching them,
// the opening party stands in it, and some fifty lords of the realms are still parked in Castle
// Burning - a hostile Dark Fey keep would fight all of them on the first night. When the
// Citadel's own war (war states, ransom, reactions) lands, this is the rule to widen.
//
bool mxstronghold::IsEnemy() const
{
#if defined(_CITADEL_)
    return OccupyingRace() == RA_ENEMY && Occupier() != nullptr && Occupier() == DEF_SCENARIO(doomdark);
#else
    return OccupyingRace() == RA_ENEMY;
#endif
}

/*
 * Function name    : MakeChangeSides
 * 
 * Arguments        : int total
 *                  : int stronghold
 *
 * Return type        : void
 * 
 * Description        : this stronghold has been lost so
 *                    place new guards in it from the victorious army
 */

void mxstronghold::MakeChangeSides( mxrace_t newrace, mxcharacter* newoccupier )
{
    // if the new occupier is allowed an army then
    // the stronghold army will be his race
    if ( newoccupier && newoccupier->IsAllowedArmy() )
        newrace = newoccupier->Race();
    
    // if and old doomdark stronghold falls to the free then
    // the stronghold can go to whoever took it
    // if the stronghold used to belong to the alliance then armies of the
    // free retain it
    if ( Owner() && Owner()->Race() != RA_ENEMY && newrace != RA_ENEMY ) {
        newrace = RA_FREE ;
        newoccupier = NULL ;
    }

    if ( newrace != OccupyingRace() )    {
        occupyingrace = newrace ;
        totaltroops = mx->RaceById(newrace)->StrongholdStartups();

        occupier = newoccupier ;
        // TODO if the owner is dead and the new occupier is on the same
        // side, then he becomes the owner
    }

    // TODO What about changing unit type dependent on the new occupier

    CheckForZero();
}


void mxstronghold::CheckForZero ( void )
{
    if ( (s32)totaltroops <= 0  )
        totaltroops = (u32)sv_stronghold_default_empty;
}

archive& operator<<(archive& ar, mxstronghold* ptr )
{
    return ar << (u32) mxentity::SafeId(ptr);
}

archive& operator>>( archive& ar, mxstronghold*& ptr )
{
    int temp;
    ar >> temp ; ptr = mx->StrongholdById(temp);
    return ar ;
}

MXRESULT mxstronghold::FillExportData ( info_t* data )
{
    // check if we are being asked to export the stronghold as
    // army
    if ( (u32)ID_TYPE(data->id) == (u32)INFO_ARMY ) {
        defaultexport::army_t* out = (defaultexport::army_t*)data;
        out->parent = SafeIdt(this);
        out->race = OccupyingRace();
        out->type = AT_STRONGHOLD ;
        out->total = TotalTroops() ;
        out->energy = 0 ;    //unit->Energy();
        out->lost = 0;        //unit->Lost();
        out->killed = Killed();        
        return MX_OK ;
    }



    defaultexport::stronghold_t* out = (defaultexport::stronghold_t*)data;

    VALIDATE_INFO_BLOCK(out,INFO_STRONGHOLD,defaultexport::stronghold_t);

    out->type = Type();
    out->occupyingrace = OccupyingRace();
    out->race = Race();
    out->totaltroops = TotalTroops();
    out->mintroops = MinTroops();
    out->maxtroops = MaxTroops();
    out->influence = Influence();
    out->terrain = Terrain();
    out->killed = Killed();
    out->strategical_success = strategical_success;
    out->owner_success = owner_success;
    out->enemy_success = enemy_success;
    out->respawn = respawn;
    out->occupier = SafeIdt(Occupier());
    out->owner = SafeIdt(Owner());
    out->loyalty = Loyalty();

    return mxitem::FillExportData ( data );
}

    
}
