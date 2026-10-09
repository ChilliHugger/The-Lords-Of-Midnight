//
//  citadel_processor_quest.cpp
//  citadel
//

#include "../../baseinc/tme_internal.h"
#include "scenario_citadel_internal.h"
#include "citadel_processor_quest.h"
#include "citadel_processor_battle.h"

#include <algorithm>
#include <vector>

#if defined(_CITADEL_)
namespace tme {

void citadel_quest_processor::Process ( citadel_character* lord )
{
    this->lord = lord;
    if ( lord->IsRecruited() ) {
        lord->news = QN_NONE;
        Impatience(lord);
    } else {
        React(lord);
    }
    Quest();
}

void citadel_quest_processor::React ( citadel_character* lord )
{
    this->lord = lord;
    if ( lord->IsDead() || lord->IsPrisoner() || lord->Race() == RA_ENEMY )
        return;

    switch ( lord->purpose ) {
        case PU_DEFEND_HOMELAND:
        case PU_BE_A_HOSTAGE:       // out of the dungeons, a hostage goes home and defends it
            Defend();
            break;
        case PU_RANDOMLY_WANDER:
            Wander();
            break;
        default:
            break;                  // Boroth keeps to his Citadel; a lord with no purpose waits
    }
}

std::vector<mxregiment*> citadel_quest_processor::Host () const
{
    std::vector<mxregiment*> host;
    FOR_EACH_REGIMENT(regiment) {
        if ( regiment->Race() == RA_ENEMY && regiment->Total() > 0 && mx->gamemap->IsLocOnMap(regiment->Location()) )
            host.push_back(regiment);
    }
    return host;
}

mxregiment* citadel_quest_processor::Menace ( const std::vector<mxregiment*>& host, mxgridref where, s32 range ) const
{
    mxregiment* nearest = nullptr;
    for ( auto regiment : host ) {
        auto distance = where - regiment->Location();
        if ( distance <= range && ( nearest == nullptr || distance < where - nearest->Location() ) )
            nearest = regiment;
    }
    return nearest;
}

bool citadel_quest_processor::Nearer ( mxstronghold* stronghold, mxstronghold* best ) const
{
    auto here = lord->Location();
    return best == nullptr || here - stronghold->Location() < here - best->Location();
}

//
// His own people's keeps, and a friendly neighbour's he is not feuding with.
//
bool citadel_quest_processor::FightsFor ( mxstronghold* stronghold ) const
{
    auto people = lord->Race();
    auto other = stronghold->Race();
    if ( other == people )
        return true;
    if ( other == RA_ENEMY || !CITADEL_SCENARIO(Borders(people, other)) )
        return false;
    return !CitadelRace(people)->IsFeudingWith(other) && !CitadelRace(other)->IsFeudingWith(people);
}

void citadel_quest_processor::Order ( mxreaction_t reaction, mxquest_t quest, mxid target )
{
    lord->reaction = reaction;
    lord->quest = quest;
    lord->questtarget = target;
}

void citadel_quest_processor::Defend ( void )
{
    auto people = lord->Race();
    auto here = lord->Location();
    auto men = lord->Men();
    auto host = Host();
    auto marches = lord->Marches();

    mxstronghold* home = nullptr;       // the nearest keep his people still hold
    mxstronghold* lost = nullptr;       // the nearest keep of his realm that Boroth has taken
    mxstronghold* help = nullptr;       // the nearest keep Boroth has taken from a friendly neighbour
    mxstronghold* danger = nullptr;     // the nearest keep his people hold with Boroth's host closing in
    std::vector<mxstronghold*> stores;  // keeps his people hold with men to spare, and no danger
    mxregiment* invader = nullptr;      // the nearest of Boroth's regiments inside his realm
    FOR_EACH_STRONGHOLD(stronghold) {
        CONTINUE_IF( !mx->gamemap->IsLocOnMap(stronghold->Location()) );
        if ( stronghold->Race() == people ) {
            auto menace = Menace(host, stronghold->Location(), THREAT);
            if ( menace != nullptr
                 && ( invader == nullptr || here - menace->Location() < here - invader->Location() ) )
                invader = menace;

            if ( stronghold->OccupyingRace() == people ) {
                if ( Nearer(stronghold, home) )
                    home = stronghold;
                if ( menace != nullptr && Nearer(stronghold, danger) )
                    danger = stronghold;
                if ( menace == nullptr && stronghold->TotalTroops() > stronghold->MinTroops() )
                    stores.push_back(stronghold);
            } else if ( stronghold->IsEnemy() && Nearer(stronghold, lost) ) {
                lost = stronghold;
            }
        } else if ( stronghold->IsEnemy() && FightsFor(stronghold) && Nearer(stronghold, help) ) {
            help = stronghold;
        }
    }

    // the keep one of yours is marching on, if this lord would fight for it and is near enough
    mxstronghold* service = nullptr;
    if ( marches ) {
        FOR_EACH_CHARACTER(character) {
            auto ally = CitadelLord(character);
            CONTINUE_IF( !ally->IsRecruited() || ally->IsDead() || ally->quest != QS_SEIZE );
            auto keep = StrongholdTarget(ally->questtarget);
            CONTINUE_IF( keep == nullptr || !keep->IsEnemy() || !FightsFor(keep) || here - keep->Location() > SERVICE );
            if ( Nearer(keep, service) )
                service = keep;
        }
    }

    auto upon = Menace(host, here, 2);
    auto sheltered = home != nullptr && home->Location() == here;

    auto cause = lost != nullptr ? lost : help;
    auto needed = cause != nullptr && cause->TotalTroops() > men ? cause->TotalTroops() - men : 0u;
    mxstronghold* barracks = nullptr;
    for ( auto stronghold : stores ) {
        if ( ( stronghold->TotalTroops() - stronghold->MinTroops() ) / 2 >= needed && Nearer(stronghold, barracks) )
            barracks = stronghold;
    }

    if ( upon != nullptr && !sheltered && ( lord->HasQuality(qf_cowardly) || upon->Total() > men ) && home != nullptr )
        Order(RE_RETREAT, QS_GOTO, mxentity::SafeIdt(home));
    else if ( marches && lost != nullptr && men >= lost->TotalTroops() )
        Order(RE_TAKE_BACK_STRONGHOLD, QS_SEIZE, mxentity::SafeIdt(lost));
    else if ( service != nullptr )
        Order(RE_LEND_SERVICE, QS_SEIZE, mxentity::SafeIdt(service));
    else if ( marches && help != nullptr && men >= help->TotalTroops() )
        Order(RE_HELP_NEIGHBOUR, QS_SEIZE, mxentity::SafeIdt(help));
    else if ( marches && invader != nullptr && men >= invader->Total() )
        Order(RE_ATTACK_ENEMY, QS_KILL, mxentity::SafeIdt(invader));
    else if ( !lord->HasQuality(qf_cowardly) && danger != nullptr && here != danger->Location() )
        Order(RE_COUNTER_THREAT, QS_GOTO, mxentity::SafeIdt(danger));
    else if ( marches && needed > 0 && barracks != nullptr ) {
        if ( here == barracks->Location() ) {
            GatherStrength(barracks, needed);
            Order(RE_GATHER_STRENGTH, QS_GUARD, MAKE_LOCID(here.x, here.y));
        } else {
            Order(RE_GATHER_STRENGTH, QS_GOTO, mxentity::SafeIdt(barracks));
        }
    }
    else if ( home != nullptr && here != home->Location() )
        Order(RE_RETURN_HOME, QS_GOTO, mxentity::SafeIdt(home));
    else
        Order(RE_STAND_FIRM, QS_GUARD, MAKE_LOCID(here.x, here.y));
}

void citadel_quest_processor::GatherStrength ( mxstronghold* stronghold, u32 needed )
{
    auto wanted = std::min(needed, ( stronghold->TotalTroops() - stronghold->MinTroops() ) / 2);

    if ( stronghold->Type() == UT_RIDERS ) {
        auto room = (u32)sv_character_max_riders > lord->riders.Total() ? (u32)sv_character_max_riders - lord->riders.Total() : 0u;
        lord->riders.Total(lord->riders.Total() + stronghold->Remove(lord->Race(), UT_RIDERS, std::min(wanted, room)));
    } else if ( stronghold->Type() == UT_WARRIORS ) {
        auto room = (u32)sv_character_max_warriors > lord->warriors.Total() ? (u32)sv_character_max_warriors - lord->warriors.Total() : 0u;
        lord->warriors.Total(lord->warriors.Total() + stronghold->Remove(lord->Race(), UT_WARRIORS, std::min(wanted, room)));
    }
}

void citadel_quest_processor::Wander ( void )
{
    if ( lord->quest == QS_GOTO && lord->Location() != lord->QuestLocation() )
        return;     // still on his way

    auto here = lord->Location();
    for ( int tries = 0; tries < 8; tries++ ) {
        mxgridref there ( here.x + mxrandom(0, 2 * WANDERING) - WANDERING, here.y + mxrandom(0, 2 * WANDERING) - WANDERING );
        CONTINUE_IF( !mx->gamemap->IsLocOnMap(there) || there == here || !CITADEL_SCENARIO(Reachable(here, there)) );
        lord->reaction = RE_RETURN_HOME;
        lord->quest = QS_GOTO;
        lord->questtarget = MAKE_LOCID(there.x, there.y);
        return;
    }
}

//
// The nearest he can find a road to.
//
template<typename T>
T citadel_quest_processor::FirstReachable ( std::vector<T>& candidates ) const
{
    auto here = lord->Location();
    std::stable_sort(candidates.begin(), candidates.end(), [&here]( T a, T b ) {
        return here - a->Location() < here - b->Location();
    });
    for ( auto candidate : candidates ) {
        if ( CITADEL_SCENARIO(Reachable(here, candidate->Location())) )
            return candidate;
    }
    return nullptr;
}

void citadel_quest_processor::Impatience ( citadel_character* lord )
{
    this->lord = lord;
    if ( lord->idle < IMPATIENCE || !lord->HasQuality(qf_impatient)
         || lord->IsDead() || lord->IsPrisoner() || lord->IsFollowing() )
        return;

    // someone he can win over, his own people first
    std::vector<mxcharacter*> kin;
    std::vector<mxcharacter*> strangers;
    FOR_EACH_CHARACTER(character) {
        CONTINUE_IF( !lord->CanQuest(QS_RECRUIT, mxentity::SafeIdt(character)) || !lord->CheckRecruitChar(character) );
        ( character->Race() == lord->Race() ? kin : strangers ).push_back(character);
    }
    auto recruit = FirstReachable(kin);
    if ( recruit == nullptr )
        recruit = FirstReachable(strangers);

    // otherwise a keep of Boroth's he has the men to take
    mxstronghold* keep = nullptr;
    if ( recruit == nullptr ) {
        std::vector<mxstronghold*> keeps;
        FOR_EACH_STRONGHOLD(stronghold) {
            if ( stronghold->IsEnemy() && lord->Men() >= stronghold->TotalTroops() && mx->gamemap->IsLocOnMap(stronghold->Location()) )
                keeps.push_back(stronghold);
        }
        keep = FirstReachable(keeps);
    }

    auto set = recruit != nullptr ? lord->SetQuest(QS_RECRUIT, mxentity::SafeIdt(recruit))
             : keep != nullptr    ? lord->SetQuest(QS_SEIZE, mxentity::SafeIdt(keep))
             : false;
    if ( set )
        lord->news = QN_IMPATIENT;
}

bool citadel_quest_processor::March ( mxgridref target, bool fight )
{
    const u32 tired = 2 * (u32)sv_energy_scale;
    blocked = false;

    while ( lord->Location() != target && lord->CanWalkForward() && lord->energy >= tired ) {
        mxgridref step;
        if ( !CITADEL_SCENARIO(MarchStep(lord->Location(), target, step)) )
            return false;

        lord->looking = lord->Location().DirFromHere(step);
        auto info = lord->GetLocInfo();

        // asked directly: mxlocinfo never stops an AI-controlled lord for the enemy ahead
        // (it leaves lif_moveforward set for him), so the flag cannot say it
        auto ahead = info->infront.get();
        if ( ahead != nullptr && ( ahead->foe.armies || ahead->foe.characters ) ) {
            if ( fight )
                CitadelBattle()->Attack(lord);
            else
                blocked = true;
            break;
        }

        if ( !info->flags.Is(lif_moveforward) )
            break;

        lord->Cmd_WalkForward(false, false);

        if ( lord->GetLocInfo()->foe.armies )
            break;
    }
    return true;
}

bool citadel_quest_processor::Guard ( void )
{
    if ( !lord->IsRecruited() && !lord->Marches() )
        return false;

    return CitadelBattle()->Guard(lord);
}

void citadel_quest_processor::Done ( mxquestnews_t news )
{
    lord->quest = QS_NONE;      // the target stays: the dawn news names it
    if ( lord->IsRecruited() )
        lord->news = news;
}

void citadel_quest_processor::Quest ( void )
{
    if ( lord->IsDead() || lord->IsPrisoner() || lord->IsFollowing() )
        return;

    auto character = CharacterTarget(lord->questtarget);
    auto stronghold = StrongholdTarget(lord->questtarget);
    auto object = ObjectTarget(lord->questtarget);
    auto regiment = RegimentTarget(lord->questtarget);
    auto holder = object != nullptr ? mx->scenario->WhoHasObject(object) : nullptr;
    mxgridref lies;

    bool live = true;
    switch ( lord->quest ) {
        case QS_RECRUIT:
            live = character != nullptr && character->IsAlive() && !character->IsRecruited();
            break;
        case QS_JOIN:
        case QS_FOLLOW:
            live = character != nullptr && character->IsAlive();
            break;
        case QS_KILL:
            live = character != nullptr ? character->IsAlive() && !character->IsRecruited()
                                        : regiment != nullptr && regiment->Total() > 0;
            break;
        case QS_RESCUE:
            live = lord->InDungeon() && ( character == nullptr || ( character->IsAlive() && character->IsPrisoner() ) );
            break;
        case QS_SEIZE:
            live = stronghold != nullptr;
            break;
        case QS_FIND:
            live = object != nullptr && object->OnMap(lies);
            break;
        case QS_TAKE:
            live = holder != nullptr && holder != lord && holder->IsRecruited() && holder->IsAlive();
            break;
        case QS_DESTROY:
            live = object != nullptr && ( object->OnMap(lies) || holder == lord );
            break;
        case QS_GOTO:
        case QS_GUARD:
            break;
        default:
            return;     // resting, or waiting for orders
    }

    if ( !live ) {
        Done(QN_FAILED);
        return;
    }

    if ( lord->quest == QS_RESCUE ) {
        auto hostage = lord->SearchDungeon((u32)sv_dungeon_search_night);
        if ( hostage != nullptr )
            mx->SetLastActionMsg(mx->LastActionMsg() + mx->text->CookedSystemString(SS_DUNGEON_FREED, hostage));
        if ( ( character != nullptr && !character->IsPrisoner() ) || CITADEL_SCENARIO(HostagesHeldAtMaranor()).empty() )
            Done(QN_DONE);
        return;
    }

    auto target = lord->QuestLocation();

    if ( lord->quest == QS_KILL && character != nullptr ) {
        if ( !lord->Location().IsNear(target) && !March(target, false) ) {
            Done(QN_FAILED);
            return;
        }
        if ( lord->Location().IsNear(character->Location()) ) {
            CitadelBattle()->Duel(lord, character);
            if ( character->IsDead() )
                Done(QN_DONE);
        }
        return;
    }

    if ( lord->quest == QS_DESTROY && holder == lord ) {
        lord->carrying = nullptr;
        object->Lift();
        Done(QN_DONE);
        return;
    }

    if ( !March(target, lord->quest == QS_SEIZE || regiment != nullptr) ) {
        Done(QN_FAILED);
        return;
    }

    if ( lord->Location() != target ) {
        if ( blocked && lord->IsRecruited() )
            lord->news = QN_BLOCKED;
        return;         // still on the road
    }

    switch ( lord->quest ) {
        case QS_RECRUIT:
            if ( lord->CheckRecruitChar(character) && lord->Cmd_Approach(character) != nullptr ) {
                character->Cmd_Follow(lord);
                Done(QN_DONE);
            } else if ( lord->RecruitScore(character) <= -1 ) {
                CitadelBattle()->Duel(character, lord);
                Done(QN_OFFENDED);
            } else {
                Done(QN_REFUSED);
            }
            break;
        case QS_JOIN:
            lord->Cmd_Follow(character);
            Done(QN_DONE);
            break;
        case QS_GOTO:
        case QS_KILL:   // a regiment: the battle is joined where it stands
            Done(QN_DONE);
            break;
        case QS_SEIZE:
            if ( !stronghold->IsEnemy() )
                Done(QN_DONE);
            break;
        case QS_FIND:
            lord->Cmd_PickupObject();
            Done(lord->Carrying() == object ? QN_DONE : QN_FAILED);
            break;
        case QS_TAKE:
            std::swap(lord->carrying, holder->carrying);
            Done(QN_DONE);
            break;
        case QS_DESTROY:
            object->Lift();
            Done(QN_DONE);
            break;
        case QS_GUARD:
            Guard();
            break;
        default:
            break;      // a shadow keeps shadowing
    }
}

} // namespace tme
#endif // _CITADEL_
