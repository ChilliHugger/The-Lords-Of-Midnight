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

//
// THE SEVEN WEAPONS
//
// Each of the seven carries a power in objects.tsv, and every power below is the design
// document's sentence turned into a rule. What makes them different from Doomdark's Revenge's
// objects is that most of them ask who is holding them.
//

mxobjpower_t citadel_character::WeaponPower() const
{
    auto object = static_cast<citadel_object*>(Carrying());
    return object != nullptr ? object->power : OP_NONE;
}

static bool IsDwarf ( mxrace_t race )
{
    return race == RA_LONG_DWARF || race == RA_DEEPING_DWARF;
}

static bool IsFey ( mxrace_t race )
{
    return race == RA_HIGH_FEY
        || race == RA_DAWN_FEY
        || race == RA_GOLDEN_FEY
        || race == RA_DARK_FEY;
}

//
// Skullcrusher - "a weapon of great power in the hands of any Giant, such that in battle he
// will fight with the strength of two."
// Aranath      - "In the hands of any Fey Lord, it is a weapon of ferocious power. In the
//                 hands of any other, it brings ill-fate."
// Stormblade   - "a weapon of great power", and its bearer never tires (see below).
//
u32 citadel_character::FightStrength() const
{
    auto base = mxcharacter::FightStrength();

    switch ( WeaponPower() ) {
        case OP_GIANT_STRENGTH:
            return Race() == RA_BLOODMARCH_GIANT ? base * 2 : base;
        case OP_FEY_BLADE:
            return IsFey(Race()) ? base * 2 : base / 2;
        case OP_BATTLE_TIRELESS:
            return base * 2;
        default:
            return base;
    }
}

//
// Widowmaker - "A dwarf who wields Widowmaker in battle is said to be invincible."
//
bool citadel_character::ShouldDieInFight() const
{
    if ( WeaponPower() == OP_DWARF_INVINCIBLE && IsDwarf(Race()) )
        return false;

    return mxcharacter::ShouldDieInFight();
}

//
// Swiftwing - "makes its bearer as tireless as a dragon, needing no rest or shelter. Yet the
// wielder of Swiftwing should beware - if he is part of a fellowship, Swiftwing's magic will
// not work for its spirit is as free and lonely as the great beasts of Arungor."
//
// A lord is in a fellowship when he follows another or is followed himself.
//
void citadel_character::InitNightProcessing ( void )
{
    mxcharacter::InitNightProcessing();

    if ( WeaponPower() == OP_LONE_SWIFTNESS && !IsFollowing() && !HasFollowers() )
        energy = (u32)sv_energy_max;
}

//
// Bloodbringer - "He who wields Bloodbringer will command the loyalty of the Lords of the
// Arakai." The hostage rule still stands above it: while the Arakai's hostage lies in Maranor,
// no sword persuades them.
//
bool citadel_character::CheckRecruitChar ( mxcharacter* pChar ) const
{
    if ( WeaponPower() == OP_ARAKAI_LOYALTY
        && pChar != nullptr
        && pChar->Race() == RA_ARAKAI
        && mx->scenario->HostageOfRace(RA_ARAKAI) == nullptr )
        return true;

    return mxcharacter::CheckRecruitChar(pChar);
}

//
// Persuader - "He who wields Persuader is able to draw upon warriors from any stronghold not at
// war with his realm." The stronghold half of that is citadel_stronghold::CanCharacterRecruit
// OrPost; this half is the bearer, who may draw men whether or not he was born to command them.
//
bool citadel_character::IsAllowedWarriors() const
{
    return WeaponPower() == OP_PERSUASION || mxcharacter::IsAllowedWarriors();
}

bool citadel_character::IsAllowedRiders() const
{
    return WeaponPower() == OP_PERSUASION || mxcharacter::IsAllowedRiders();
}

} // namespace tme
#endif // _CITADEL_
