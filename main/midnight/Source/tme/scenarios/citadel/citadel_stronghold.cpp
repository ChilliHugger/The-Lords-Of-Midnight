/*
 * FILE:    citadel_stronghold.cpp
 *
 * PROJECT: citadel
 *
 * PURPOSE: Which keeps are held against the player.
 *
 */

#include "../../baseinc/tme_internal.h"
#include "scenario_citadel_internal.h"

#if defined(_CITADEL_)
namespace tme {

//
// A keep Boroth's host has TAKEN is his to hold, and so is the Dark Citadel, where only Corleth and
// Arin may go (#25) - they and the hostages sit its battles out. The Dark Fey's other keeps stay out
// of the war: some fifty lords of the realms start in Castle Burning, and a hostile keep would fight
// all of them on the first night, before they can march home (#9).
//
bool citadel_stronghold::IsEnemy() const
{
    return mxstronghold::IsEnemy()
        && ( this == CITADEL_SCENARIO(maranor)
             || ( Occupier() != nullptr && Occupier() == CITADEL_SCENARIO(boroth) ) );
}

void citadel_stronghold::MakeChangeSides ( mxrace_t newrace, mxcharacter* newoccupier )
{
    if ( newoccupier == nullptr || newoccupier->IsRecruited() || newoccupier->Race() == RA_ENEMY ) {
        mxstronghold::MakeChangeSides(newrace, newoccupier);
        return;
    }

    if ( Race() != OccupyingRace() ) {
        occupyingrace = Race();
        totaltroops = mx->RaceById(occupyingrace)->StrongholdStartups();
        occupier = newoccupier;
    }
}

bool citadel_stronghold::CanCharacterRecruitOrPost ( const mxcharacter* character ) const
{
    auto lord = static_cast<const citadel_character*>(character);

    if ( lord != nullptr && lord->WeaponPower() == OP_PERSUASION && !IsEnemy() )
        return true;

    return mxstronghold::CanCharacterRecruitOrPost(character);
}

static bool Feuding ( mxrace_t race, mxrace_t other )
{
    return CitadelRace(race)->IsFeudingWith(other);
}

bool citadel_stronghold::CanCharacterPost ( const mxcharacter* character ) const
{
    return !Feuding(character->Race(), OccupyingRace())
        && !Feuding(OccupyingRace(), character->Race());
}

mxrace_t citadel_stronghold::Allegiance () const
{
    return Occupier() != nullptr ? Occupier()->NormalisedLoyalty() : OccupyingRace();
}

bool citadel_stronghold::Subjugated () const
{
    if ( Owner() == nullptr || Occupier() == Owner() )
        return false;
    return CITADEL_SCENARIO(IsEnemyOf(Allegiance(), Owner()->NormalisedLoyalty()));
}

void citadel_stronghold::Conquer ( mxcharacter* lord )
{
    occupyingrace = lord->IsRecruited() ? RA_FREE : lord->Race();
    occupier = lord;
    totaltroops = 0;
    CheckForZero();
}

void citadel_stronghold::Gather ()
{
    auto side = Allegiance();
    if ( side == RA_FREE || side == RA_ENEMY )
        Add(OccupyingRace(), Type(), Respawn());
}

u32 citadel_stronghold::DefenceMultiplier() const
{
    switch ( Terrain() ) {
        case TN_CITADEL:
            return 4;
        case TN_CASTLE:
            return 3;
        default:
            return 1;
    }
}

} // namespace tme
#endif // _CITADEL_
