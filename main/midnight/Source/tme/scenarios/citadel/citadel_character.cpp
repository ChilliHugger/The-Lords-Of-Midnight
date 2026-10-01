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
    bool waited = IsRecruited() && ( quest == QS_NONE || quest == QS_REST ) && time == (mxtime_t)sv_time_dawn;
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

bool citadel_character::Recruited ( mxcharacter* recruiter )
{
    flags.Reset(cf_ai);
    return mxcharacter::Recruited(recruiter);
}

bool citadel_character::CanQuest ( mxquest_t newquest, mxid target ) const
{
    if ( !IsRecruited() || IsDead() || IsPrisoner() )
        return false;

    auto character = CharacterTarget(target);
    auto other = [this, character]() {
        return character != nullptr && character != this && character->IsAlive();
    };

    auto object = ObjectTarget(target);
    auto artefact = IsArtefact(object);
    mxgridref where;

    switch ( newquest ) {
        case QS_NONE:
        case QS_REST:
            return true;
        case QS_RECRUIT:
        case QS_KILL:
            return other() && !character->IsRecruited() && !character->IsPrisoner();
        case QS_RESCUE:
            return other() && character->IsPrisoner();
        case QS_JOIN:
        case QS_FOLLOW:
            return other() && character->IsRecruited();
        case QS_GOTO:
        case QS_GUARD:
            return ID_TYPE(target) == IDT_LOCATION
                && mx->gamemap->IsLocOnMap(mxgridref(GET_LOCIDX(target), GET_LOCIDY(target)));
        case QS_SEIZE:
            return StrongholdTarget(target) != nullptr && StrongholdTarget(target)->IsEnemy();
        case QS_FIND:
            return artefact && ObjectOnMap(object, where);
        case QS_TAKE: {
            auto holder = artefact ? mx->scenario->WhoHasObject(object) : nullptr;
            return holder != nullptr && holder != this && holder->IsRecruited() && holder->IsAlive();
        }
        case QS_DESTROY:
            return artefact && ( ObjectOnMap(object, where) || Carrying() == object );
        default:
            return false;
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
        if ( ObjectOnMap(object, where) )
            return where;
        auto holder = mx->scenario->WhoHasObject(object);
        return holder != nullptr ? holder->Location() : Location();
    }

    auto item = static_cast<mxitem*>(mx->EntityByIdt(questtarget));
    return item != nullptr ? item->Location() : Location();
}

std::string citadel_character::QuestText () const
{
    auto character = CharacterTarget(questtarget);
    auto object = ObjectTarget(questtarget);
    std::string who = character != nullptr ? character->Longname() : "";
    std::string holder;
    if ( object != nullptr ) {
        mx->text->oinfo = object;
        auto carrier = mx->scenario->WhoHasObject(object);
        holder = carrier != nullptr ? carrier->Longname() : "";
    }
    mx->text->loc = QuestLocation();

    std::string text;
    switch ( quest ) {
        case QS_RECRUIT:    text = "{char:name} is questing to recruit " + who; break;
        case QS_JOIN:       text = "{char:name} is journeying to join " + who; break;
        case QS_KILL:       text = character != nullptr ? "{char:name} is hunting " + who + " to slay {gender:himher}"
                                                        : "{char:name} is marching against the host of the Wolfheart"; break;
        case QS_RESCUE:     text = "{char:name} is questing to rescue " + who; break;
        case QS_FOLLOW:     text = "{char:name} is shadowing " + who; break;
        case QS_GOTO:       text = "{char:name} is journeying to {loc:name}"; break;
        case QS_GUARD:      text = "{char:name} stands guard at {loc:name}"; break;
        case QS_SEIZE:      text = "{char:name} is marching to seize {loc:name}"; break;
        case QS_FIND:       text = "{char:name} is searching for {obj:name}"; break;
        case QS_TAKE:       text = "{char:name} is going to " + holder + " for {obj:name}"; break;
        case QS_DESTROY:    text = "{char:name} is seeking {obj:name}, to destroy it"; break;
        default:            text = "{char:name} waits for your orders"; break;
    }
    return mx->text->CookText(text, this);
}

std::string citadel_character::NewsText () const
{
    auto character = CharacterTarget(questtarget);
    std::string who = character != nullptr ? character->Longname() : "";

    std::string text;
    switch ( news ) {
        case QN_DONE:       text = "{char:name} has done as you asked, and waits for your orders"; break;
        case QN_FAILED:     text = "{char:name} has given up {gender:hisher} quest, for what {gender:heshe} sought is gone or out of reach"; break;
        case QN_REFUSED:    text = who + " would not be persuaded by {char:name}"; break;
        case QN_OFFENDED:   text = who + " took offence at the approach of {char:name}, and fell upon {gender:himher}"; break;
        case QN_BLOCKED:    text = "{char:name} finds the enemy barring {gender:hisher} road"; break;
        case QN_IMPATIENT:  text = "Weary of waiting for orders, {char:name} has set out on a quest of {gender:hisher} own"; break;
        default:            return "";
    }
    return mx->text->CookText(text, this);
}

bool IsArtefact ( const mxobject* object )
{
    return object != nullptr && static_cast<const citadel_object*>(object)->power != OP_NONE;
}

bool ObjectOnMap ( const mxobject* object, mxgridref& where )
{
    if ( !IsArtefact(object) || mx->scenario->WhoHasObject(const_cast<mxobject*>(object)) != nullptr )
        return false;
    if ( !mx->gamemap->IsLocOnMap(object->Location()) )
        return false;
    where = object->Location();
    return true;
}

mxobject* ArtefactAt ( mxgridref loc )
{
    FOR_EACH_OBJECT(object) {
        mxgridref where;
        if ( ObjectOnMap(object, where) && where == loc )
            return object;
    }
    return nullptr;
}

void LiftObject ( mxobject* object )
{
    if ( IsArtefact(object) )
        object->Location(mxgridref(mx->gamemap->Size().cx, 0));     // off the map, like the placeholder keeps
}

} // namespace tme
#endif // _CITADEL_
