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

mxstronghold::mxstronghold()
{
    mxentity::type = IDT_STRONGHOLD ;
    race=RA_NONE;
    total=0;
    owner=NULL;
    lost=0;
    killed=0;
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
        ar << total;
        ar << min;
        ar << max;
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
        ar >> total;
        ar >> min;
        ar >> max;
        ar >> strategical_success;
        ar >> owner_success;
        ar >> enemy_success;
        ar >> influence;
        ar >> respawn;
        ar >> occupier;
        ar >> owner;
        READ_ENUM ( terrain );
        ar >> killed;
    }
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
    if ( OccupyingRace() == RA_DOOMGUARD )
        return enemy_success + mx->battle->BaseDoomdarkSuccess( OccupyingRace(),Type(), locinfo );
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
#if defined(_LOM_)
    if ( OccupyingRace() != race || Type() != type )
        return 0;
#endif
    if ( (Total()-amount) < Min() )
        amount = Total()-Min();
    total-= amount;
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
#if defined(_LOM_)
    if ( OccupyingRace() != race || Type() != type )
        return 0;
#endif
    if ( (Total()+amount) > Max() )
        amount = Max()-Total();
    total += amount;
    return amount;
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
    if ( Owner() && Owner()->Race() != RA_DOOMGUARD && newrace != RA_DOOMGUARD ) {
        newrace = RA_FREE ;
        newoccupier = NULL ;
    }

    if ( newrace != OccupyingRace() )    {
        occupyingrace = newrace ;
        total = mx->RaceById(newrace)->StrongholdStartups();

        occupier = newoccupier ;
        // TODO if the owner is dead and the new occupier is on the same
        // side, then he becomes the owner
    }

    // TODO What about changing unit type dependent on the new occupier

    CheckForZero();
}


void mxstronghold::CheckForZero ( void )
{
    if ( total <= 0  )
        total = (u32)sv_stronghold_default_empty;
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
        out->total = Total() ;
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
    out->total = Total();
    out->min = Min();
    out->max = Max();
    out->influence = Influence();
    out->terrain = Terrain();
    out->killed = Killed();
    out->strategical_success = strategical_success;
    out->owner_success = owner_success;
    out->enemy_success = enemy_success;
    out->respawn = respawn;
    out->occupier = SafeIdt(Occupier());
    out->owner = SafeIdt(Owner());
    out->loyalty = Owner() != NULL ? Owner()->Loyalty() : RA_NONE ;

    return mxitem::FillExportData ( data );
}

    
}
