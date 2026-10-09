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
        WRITE_ENUM(quest);
        ar << questtarget ;
        WRITE_ENUM(purpose);
        WRITE_ENUM(reaction);
        ar << idle ;
        WRITE_ENUM(news);
        ar << (u8)wraith ;
        ar << (u8)dungeon ;
        ar << home ;
    } else {
        ar >> qualities ;
        if ( tme::mx->SaveGameVersion() > 18 )
            ar >> title ;
        if ( tme::mx->SaveGameVersion() > 19 ) {
            READ_ENUM(quest);
            ar >> questtarget ;
            READ_ENUM(purpose);
            READ_ENUM(reaction);
        }
        if ( tme::mx->SaveGameVersion() > 20 ) {
            ar >> idle ;
            READ_ENUM(news);
        }
        if ( tme::mx->SaveGameVersion() > 21 ) {
            u8 risen = 0;
            ar >> risen ;
            wraith = risen != 0;
        }
        if ( tme::mx->SaveGameVersion() > 22 ) {
            u8 searching = 0;
            ar >> searching ;
            dungeon = searching != 0;
            ar >> home ;
        }
    }
}

void citadel_character::LoadTsv ( const TsvRow& row )
{
    mxcharacter::LoadTsv(row);

    qualities = ParseCharacterQualities(row.GetString(TsvField::Character::Qualities));
    title = row.GetString(TsvField::Character::Title);
    quest = ParseQuest(row.GetString(TsvField::Character::Quest));
    purpose = ParsePurpose(row.GetString(TsvField::Character::Purpose));
    reaction = ParseReaction(row.GetString(TsvField::Character::Reaction));
    home = row.GetStronghold(TsvField::Character::Home);
}

bool citadel_character::TakesPartInBattle() const
{
    return mxcharacter::TakesPartInBattle()
        && ( IsRecruited() || purpose == PU_DEFEND_HOMELAND )
        && !IsPrisoner()
        && !InDungeon()
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
    bool waited = IsRecruited() && ( quest == QS_NONE || quest == QS_REST ) && IsDawn();
    idle = waited ? idle + 1 : 0;

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
        return CITADEL_SCENARIO(maranor) != nullptr && CITADEL_SCENARIO(maranor)->HasFallen();

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

bool citadel_character::Recruited ( mxcharacter* recruiter )
{
    flags.Reset(cf_ai);
    return mxcharacter::Recruited(recruiter);
}

bool citadel_character::InDungeon () const
{
    auto maranor = CITADEL_SCENARIO(maranor);
    return dungeon && maranor != nullptr && Location() == maranor->Location();
}

MXRESULT citadel_character::Cmd_WalkForward ( bool seek, bool approach )
{
    dungeon = InDungeon();
    auto result = mxcharacter::Cmd_WalkForward(seek, approach);
    dungeon = InDungeon();
    return result;
}

citadel_character* citadel_character::SearchDungeon ( u32 chance )
{
    if ( !InDungeon() || mxrandom(255) >= (int)chance )
        return nullptr;

    auto held = CITADEL_SCENARIO(Held());
    if ( held.empty() )
        return nullptr;

    auto hostage = held[mxrandom(0, (int)held.size() - 1)];
    Cmd_Approach(hostage);
    hostage->FlyHome();
    return hostage;
}

void citadel_character::FlyHome ()
{
    auto keep = home;
    if ( keep == nullptr || keep->IsEnemy() )
        keep = CITADEL_SCENARIO(HomeKeep(Race(), home != nullptr ? home->Location() : Location()));
    if ( keep != nullptr )
        Location(keep->Location());
}

mxobject* citadel_character::Cmd_Seek ( void )
{
    if ( !InDungeon() )
        return mxcharacter::Cmd_Seek();

    SetLastCommand(CMD_SEEK, IDT_NONE);
    CommandTakesTime(true);
    auto hostage = SearchDungeon((u32)sv_dungeon_search_day);
    time = (mxtime_t)sv_time_night;
    mx->SetLastActionMsg(hostage != nullptr
        ? mx->text->CookedSystemString(SS_DUNGEON_FOUND, hostage)
        : mx->text->CookedSystemString(SS_DUNGEON_NOTHING, this));
    return nullptr;
}

bool citadel_character::Marches () const
{
    return mx->scenario->HostageOfRace(Race()) == nullptr && !HasQuality(qf_cowardly);
}

bool citadel_character::CanQuest () const
{
    return IsRecruited() && !IsDead() && !IsPrisoner();
}

bool citadel_character::CanQuest ( mxquest_t newquest, mxid target ) const
{
    if ( !CanQuest() )
        return false;

    auto character = CharacterTarget(target);
    auto other = character != nullptr && character != this && character->IsAlive();

    auto object = ObjectTarget(target);
    auto artefact = object != nullptr && object->IsArtefact();
    mxgridref where;

    switch ( newquest ) {
        case QS_NONE:
        case QS_REST:
            return true;
        case QS_RECRUIT:
        case QS_KILL:
            return other && !character->IsRecruited() && !character->IsPrisoner();
        case QS_RESCUE:
            return InDungeon() && other && character->IsPrisoner();
        case QS_JOIN:
        case QS_FOLLOW:
            return other && character->IsRecruited();
        case QS_GOTO:
        case QS_GUARD:
            return ID_TYPE(target) == IDT_LOCATION
                && mx->gamemap->IsLocOnMap(mxgridref(GET_LOCIDX(target), GET_LOCIDY(target)));
        case QS_SEIZE:
            return StrongholdTarget(target) != nullptr && StrongholdTarget(target)->IsEnemy();
        case QS_FIND:
            return artefact && object->OnMap(where);
        case QS_TAKE: {
            auto holder = artefact ? mx->scenario->WhoHasObject(object) : nullptr;
            return holder != nullptr && holder != this && holder->IsRecruited() && holder->IsAlive();
        }
        case QS_DESTROY:
            return artefact && ( object->OnMap(where) || Carrying() == object );
        default:
            return false;
    }
}

void citadel_character::QuestTargets ( mxquest_t newquest, c_mxid& targets ) const
{
    targets.Clear();
    FOR_EACH_CHARACTER(character) {
        if ( CanQuest(newquest, mxentity::SafeIdt(character)) )
            targets.Add(mxentity::SafeIdt(character));
    }
    FOR_EACH_STRONGHOLD(stronghold) {
        if ( mx->gamemap->IsLocOnMap(stronghold->Location()) && CanQuest(newquest, mxentity::SafeIdt(stronghold)) )
            targets.Add(mxentity::SafeIdt(stronghold));
    }
    FOR_EACH_OBJECT(object) {
        if ( CanQuest(newquest, mxentity::SafeIdt(object)) )
            targets.Add(mxentity::SafeIdt(object));
    }
}

bool citadel_character::SetQuest ( mxquest_t newquest, mxid target )
{
    if ( !CanQuest(newquest, target) )
        return false;

    quest = newquest;
    questtarget = ( newquest == QS_NONE || newquest == QS_REST ) ? IDT_NONE : target;
    idle = 0;
    news = QN_NONE;
    return true;
}

mxgridref citadel_character::QuestLocation () const
{
    if ( ID_TYPE(questtarget) == IDT_LOCATION )
        return mxgridref(GET_LOCIDX(questtarget), GET_LOCIDY(questtarget));

    if ( auto object = ObjectTarget(questtarget) ) {
        mxgridref where;
        if ( object->OnMap(where) )
            return where;
        auto holder = mx->scenario->WhoHasObject(object);
        return holder != nullptr ? holder->Location() : Location();
    }

    auto item = static_cast<mxitem*>(mx->EntityByIdt(questtarget));
    return item != nullptr ? item->Location() : Location();
}

std::string citadel_character::QuestText () const
{
    if ( IsDead() )
        return "";
    if ( IsPrisoner() )
        return mx->text->CookedSystemString(SS_QUEST_HELD, this);

    u32 id;
    switch ( quest ) {
        case QS_RECRUIT:    id = SS_QUEST_RECRUIT; break;
        case QS_JOIN:       id = SS_QUEST_JOIN; break;
        case QS_KILL:       id = CharacterTarget(questtarget) != nullptr ? SS_QUEST_KILL_LORD : SS_QUEST_KILL_HOST; break;
        case QS_RESCUE:     id = SS_QUEST_RESCUE; break;
        case QS_FOLLOW:     id = SS_QUEST_FOLLOW; break;
        case QS_GOTO:       id = SS_QUEST_GOTO; break;
        case QS_GUARD:      id = SS_QUEST_GUARD; break;
        case QS_SEIZE:      id = SS_QUEST_SEIZE; break;
        case QS_FIND:       id = SS_QUEST_FIND; break;
        case QS_TAKE:       id = SS_QUEST_TAKE; break;
        case QS_DESTROY:    id = SS_QUEST_DESTROY; break;
        default:            id = SS_QUEST_REST; break;
    }
    return mx->text->CookedSystemString(id, this);
}

std::string citadel_character::NewsText () const
{
    u32 id;
    switch ( news ) {
        case QN_DONE:       id = SS_QUEST_NEWS_DONE; break;
        case QN_FAILED:     id = SS_QUEST_NEWS_FAILED; break;
        case QN_REFUSED:    id = SS_QUEST_NEWS_REFUSED; break;
        case QN_OFFENDED:   id = SS_QUEST_NEWS_OFFENDED; break;
        case QN_BLOCKED:    id = SS_QUEST_NEWS_BLOCKED; break;
        case QN_IMPATIENT:  id = SS_QUEST_NEWS_IMPATIENT; break;
        default:            return "";
    }
    return mx->text->CookedSystemString(id, this);
}

} // namespace tme
#endif // _CITADEL_
