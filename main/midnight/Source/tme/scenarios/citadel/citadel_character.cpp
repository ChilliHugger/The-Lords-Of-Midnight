/*
 * FILE:    citadel_character.cpp
 *
 * PROJECT: citadel
 *
 * PURPOSE: Which lords are at war with Boroth the Wolfheart.
 *
 */

#include "../../baseinc/tme_internal.h"
#include "scenario_citadel_internal.h"

#if defined(_CITADEL_)
namespace tme {

//
// Only the player's lords are at war with Boroth's host yet. Hostages sit in his dungeons, his
// own Dark Fey do not fight him, and the lords of the realms wait on the Citadel's NPC side
// (purposes, reactions) - until that exists, an army that found one of them would only cut down
// a lord who cannot answer.
//
bool citadel_character::TakesPartInBattle() const
{
    return mxcharacter::TakesPartInBattle()
        && IsRecruited()
        && !IsPrisoner()
        && Race() != RA_ENEMY;
}

} // namespace tme
#endif // _CITADEL_
