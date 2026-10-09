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
// Only a keep Boroth's host has TAKEN is his to hold. The Dark Fey's own keeps stay out of the
// war for now: Maranor holds the hostages the player frees by approaching them, the opening
// party stands in it, and some fifty lords of the realms start in Castle Burning - a hostile
// Dark Fey keep would fight all of them on the first night, before they can march home (#9).
//
bool citadel_stronghold::IsEnemy() const
{
    return mxstronghold::IsEnemy()
        && Occupier() != nullptr
        && Occupier() == CITADEL_SCENARIO(boroth);
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

void citadel_stronghold::Hold ( mxcharacter* lord )
{
    occupier = lord;
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
