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
// party stands in it, and some fifty lords of the realms are still parked in Castle Burning - a
// hostile Dark Fey keep would fight all of them on the first night. When the Citadel's own war
// (war states, ransom, reactions) lands, this is the rule to widen.
//
bool citadel_stronghold::IsEnemy() const
{
    return mxstronghold::IsEnemy()
        && Occupier() != nullptr
        && Occupier() == CITADEL_SCENARIO(boroth);
}

bool citadel_stronghold::CanCharacterRecruitOrPost ( const mxcharacter* character ) const
{
    auto lord = static_cast<const citadel_character*>(character);

    if ( lord != nullptr && lord->WeaponPower() == OP_PERSUASION && !IsEnemy() )
        return true;

    return mxstronghold::CanCharacterRecruitOrPost(character);
}

static const mxrace_t feuds[][2] = {
    { RA_DRAGONLORD,    RA_BLOODMARCH_GIANT },
    { RA_DEEPING_DWARF, RA_LONG_DWARF },
    { RA_DAWN_FEY,      RA_GELMING },
    { RA_ELDRIN,        RA_HIGH_FEY },
    { RA_USKARG,        RA_KITH },
    { RA_ARAKAI,        RA_ATHELING },
};

bool citadel_stronghold::CanCharacterPost ( const mxcharacter* character ) const
{
    for ( auto& feud : feuds ) {
        if ( (feud[0] == character->Race() && feud[1] == OccupyingRace())
             || (feud[1] == character->Race() && feud[0] == OccupyingRace()) )
            return false;
    }
    return true;
}

u32 citadel_stronghold::DefenceMultiplier() const
{
    return Terrain() == TN_CITADEL ? 4 : 3;
}

} // namespace tme
#endif // _CITADEL_
