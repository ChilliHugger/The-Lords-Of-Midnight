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
#include "../../tsv/tsvflags.h"
#include "../../tsv/tsvfields.h"

#include <bit>

#if defined(_CITADEL_)
namespace tme {

void citadel_character::Serialize ( archive& ar )
{
    mxcharacter::Serialize(ar);

    if ( ar.IsStoring() ) {
        ar << qualities ;
        ar << title ;
    }else{
        ar >> qualities ;
        if ( tme::mx->SaveGameVersion() > 18 )
            ar >> title ;
    }
}

void citadel_character::LoadTsv ( const TsvRow& row )
{
    mxcharacter::LoadTsv(row);

    qualities = ParseCharacterQualities(row.GetString(TsvField::Character::Qualities));
    title = row.GetString(TsvField::Character::Title);
}

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

bool citadel_character::ShouldDieInFight() const
{
    if ( WeaponPower() == OP_DWARF_INVINCIBLE && IsDwarf(Race()) )
        return false;

    return mxcharacter::ShouldDieInFight();
}

void citadel_character::InitNightProcessing ( void )
{
    mxcharacter::InitNightProcessing();

    if ( WeaponPower() == OP_LONE_SWIFTNESS && !IsFollowing() && !HasFollowers() )
        energy = (u32)sv_energy_max;
}

s32 citadel_character::RecruitScore ( const mxcharacter* other ) const
{
    auto theirs = other->Qualities();
    return std::popcount(qualities & theirs) - std::popcount(qualities & std::rotl(theirs, 32));
}

s32 citadel_character::RecruitThreshold ( const mxrace_t race ) const
{
    return mx->scenario->HostageOfRace(race) != nullptr ? 2 : 1;
}

bool citadel_character::CheckRecruitChar ( mxcharacter* pChar ) const
{
    if ( pChar == nullptr || pChar == this )
        return false;

    if ( pChar->IsPrisoner() )
        return true;

    if ( pChar->Race() == RA_ENEMY )
        return false;

    if ( WeaponPower() == OP_ARAKAI_LOYALTY
        && pChar->Race() == RA_ARAKAI
        && mx->scenario->HostageOfRace(RA_ARAKAI) == nullptr )
        return true;

    return RecruitScore(pChar) >= RecruitThreshold(pChar->Race());
}

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
