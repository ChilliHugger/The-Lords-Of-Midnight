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
        WRITE_ENUM(quest);
        ar << questtarget ;
        WRITE_ENUM(purpose);
        WRITE_ENUM(reaction);
    } else {
        ar >> qualities ;
        if ( tme::mx->SaveGameVersion() > 18 ) {
            READ_ENUM(quest);
            ar >> questtarget ;
            READ_ENUM(purpose);
            READ_ENUM(reaction);
        }
    }
}

void citadel_character::LoadTsv ( const TsvRow& row )
{
    mxcharacter::LoadTsv(row);

    qualities = ParseCharacterQualities(row.GetString(TsvField::Character::Qualities));
    quest = ParseQuest(row.GetString(TsvField::Character::Quest));
    purpose = ParsePurpose(row.GetString(TsvField::Character::Purpose));
    reaction = ParseReaction(row.GetString(TsvField::Character::Reaction));
}

bool citadel_character::TakesPartInBattle() const
{
    return mxcharacter::TakesPartInBattle()
        && ( IsRecruited() || purpose == PU_DEFEND_HOMELAND )
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

static mxcharacter* CharacterTarget ( mxid target )
{
    return ID_TYPE(target) == IDT_CHARACTER ? mx->CharacterById(GET_ID(target)) : nullptr;
}

static citadel_stronghold* StrongholdTarget ( mxid target )
{
    return ID_TYPE(target) == IDT_STRONGHOLD
        ? static_cast<citadel_stronghold*>(mx->StrongholdById(GET_ID(target)))
        : nullptr;
}

bool citadel_character::SetQuest ( mxquest_t newquest, mxid target )
{
    if ( !IsRecruited() || IsDead() || IsPrisoner() )
        return false;

    auto character = CharacterTarget(target);

    switch ( newquest ) {
        case QS_NONE:
        case QS_REST:
            target = IDT_NONE;
            break;
        case QS_RECRUIT:
            if ( character == nullptr || character == this || character->IsDead() || character->IsRecruited() )
                return false;
            break;
        case QS_JOIN:
        case QS_FOLLOW:
            if ( character == nullptr || character == this || character->IsDead() || !character->IsRecruited() )
                return false;
            break;
        case QS_GOTO:
        case QS_GUARD:
            if ( ID_TYPE(target) != IDT_LOCATION
                 || !mx->gamemap->IsLocOnMap(mxgridref(GET_LOCIDX(target), GET_LOCIDY(target))) )
                return false;
            break;
        case QS_SEIZE:
            if ( StrongholdTarget(target) == nullptr || !StrongholdTarget(target)->IsEnemy() )
                return false;
            break;
        default:
            return false;   // not built yet
    }

    quest = newquest;
    questtarget = target;
    return true;
}

mxgridref citadel_character::QuestLocation () const
{
    if ( ID_TYPE(questtarget) == IDT_LOCATION )
        return mxgridref(GET_LOCIDX(questtarget), GET_LOCIDY(questtarget));

    auto item = static_cast<mxitem*>(mx->EntityByIdt(questtarget));
    return item != nullptr ? item->Location() : Location();
}

bool citadel_character::March ( mxgridref target, bool fight )
{
    const u32 tired = 2 * (u32)sv_energy_scale;

    while ( Location() != target && CanWalkForward() && energy >= tired ) {
        mxgridref step;
        if ( !CITADEL_SCENARIO(MarchStep(Location(), target, step)) )
            return false;

        looking = Location().DirFromHere(step);
        auto info = GetLocInfo();
        if ( !info->flags.Is(lif_moveforward) ) {
            if ( fight )
                Cmd_Attack();
            break;
        }

        Cmd_WalkForward(false, false);

        if ( GetLocInfo()->foe.armies )
            break;
    }
    return true;
}

void citadel_character::Quest ( void )
{
    if ( IsDead() || IsPrisoner() || IsFollowing() )
        return;

    auto character = CharacterTarget(questtarget);
    auto stronghold = StrongholdTarget(questtarget);

    switch ( quest ) {
        case QS_RECRUIT:
            if ( character == nullptr || character->IsDead() || character->IsRecruited() ) {
                quest = QS_NONE;
                return;
            }
            break;
        case QS_JOIN:
        case QS_FOLLOW:
            if ( character == nullptr || character->IsDead() ) {
                quest = QS_NONE;
                return;
            }
            break;
        case QS_SEIZE:
            if ( stronghold == nullptr ) {
                quest = QS_NONE;
                return;
            }
            break;
        case QS_GOTO:
        case QS_GUARD:
            break;
        default:
            return;     // resting, waiting, or a quest not built yet
    }

    if ( !March(QuestLocation(), quest == QS_SEIZE) ) {
        quest = QS_NONE;
        return;
    }

    if ( Location() != QuestLocation() )
        return;         // still on the road

    switch ( quest ) {
        case QS_RECRUIT:
            if ( CheckRecruitChar(character) && Cmd_Approach(character) != nullptr )
                character->Cmd_Follow(this);
            quest = QS_NONE;
            break;
        case QS_JOIN:
            Cmd_Follow(character);
            quest = QS_NONE;
            break;
        case QS_GOTO:
            quest = QS_NONE;
            break;
        case QS_SEIZE:
            if ( !stronghold->IsEnemy() )
                quest = QS_NONE;
            break;
        default:
            break;      // a shadow keeps shadowing, a guard keeps guarding
    }
}

} // namespace tme
#endif // _CITADEL_
