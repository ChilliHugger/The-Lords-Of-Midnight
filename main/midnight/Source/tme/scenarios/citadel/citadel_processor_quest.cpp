//
//  citadel_processor_quest.cpp
//  citadel
//

#include "../../baseinc/tme_internal.h"
#include "scenario_citadel_internal.h"
#include "citadel_processor_quest.h"
#include "citadel_processor_battle.h"

#include <algorithm>
#include <type_traits>
#include <vector>

#if defined(_CITADEL_)
namespace tme {

static constexpr u32 IMPATIENCE = 3;    // nights an impatient lord of yours waits before he sets out alone
static constexpr s32 THREAT     = 4;    // how near Boroth's host comes before a keep is in danger
static constexpr s32 SERVICE    = 12;   // how far a lord of the realms will march to lend you his service
static constexpr s32 WANDERING  = 8;    // how far a wanderer strays on one journey

static bool Near ( mxgridref a, mxgridref b )
{
    return ABS((s32)a.x - (s32)b.x) <= 1 && ABS((s32)a.y - (s32)b.y) <= 1;
}

static u32 Men ( const mxcharacter* lord )
{
    return lord->warriors.Total() + lord->riders.Total();
}

static bool Marches ( const mxcharacter* lord )
{
    return mx->scenario->HostageOfRace(lord->Race()) == nullptr && !lord->HasQuality(qf_cowardly);
}

static bool Reachable ( mxgridref from, mxgridref to )
{
    mxgridref step;
    return from == to || CITADEL_SCENARIO(MarchStep(from, to, step));
}

static mxstronghold* HomeKeep ( mxrace_t people, mxgridref from )
{
    mxstronghold* home = nullptr;
    FOR_EACH_STRONGHOLD(stronghold) {
        CONTINUE_IF( stronghold->Race() != people || stronghold->OccupyingRace() != people
                     || !mx->gamemap->IsLocOnMap(stronghold->Location()) );
        if ( home == nullptr || from - stronghold->Location() < from - home->Location() )
            home = stronghold;
    }
    return home;
}

static std::vector<mxregiment*> Host ()
{
    std::vector<mxregiment*> host;
    FOR_EACH_REGIMENT(regiment) {
        if ( regiment->Race() == RA_ENEMY && regiment->Total() > 0 && mx->gamemap->IsLocOnMap(regiment->Location()) )
            host.push_back(regiment);
    }
    return host;
}

static mxregiment* Menace ( const std::vector<mxregiment*>& host, mxgridref where, s32 range )
{
    mxregiment* nearest = nullptr;
    for ( auto regiment : host ) {
        auto distance = where - regiment->Location();
        if ( distance <= range && ( nearest == nullptr || distance < where - nearest->Location() ) )
            nearest = regiment;
    }
    return nearest;
}

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

void citadel_quest_processor::Defend ( void )
{
    auto people = lord->Race();
    auto here = lord->Location();
    auto men = Men(lord);
    auto host = Host();
    auto marches = Marches(lord);

    auto nearer = [&here]( mxstronghold* stronghold, mxstronghold* best ) {
        return best == nullptr || here - stronghold->Location() < here - best->Location();
    };
    auto feuding = [people]( mxrace_t other ) {
        return CitadelRace(people)->IsFeudingWith(other) || CitadelRace(other)->IsFeudingWith(people);
    };
    auto fightsFor = [&]( mxstronghold* stronghold ) {
        return stronghold->Race() == people
            || ( stronghold->Race() != RA_ENEMY
                 && CITADEL_SCENARIO(Borders(people, stronghold->Race())) && !feuding(stronghold->Race()) );
    };
    auto strongEnough = [men]( mxstronghold* stronghold ) {
        return men >= stronghold->TotalTroops();
    };
    auto order = [this]( mxreaction_t reaction, mxquest_t quest, mxid target ) {
        lord->reaction = reaction;
        lord->quest = quest;
        lord->questtarget = target;
    };

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
                if ( nearer(stronghold, home) )
                    home = stronghold;
                if ( menace != nullptr && nearer(stronghold, danger) )
                    danger = stronghold;
                if ( menace == nullptr && stronghold->TotalTroops() > stronghold->MinTroops() )
                    stores.push_back(stronghold);
            } else if ( stronghold->IsEnemy() && nearer(stronghold, lost) ) {
                lost = stronghold;
            }
        } else if ( stronghold->IsEnemy() && fightsFor(stronghold) && nearer(stronghold, help) ) {
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
            CONTINUE_IF( keep == nullptr || !keep->IsEnemy() || !fightsFor(keep) || here - keep->Location() > SERVICE );
            if ( nearer(keep, service) )
                service = keep;
        }
    }

    auto upon = Menace(host, here, 2);
    auto sheltered = home != nullptr && home->Location() == here;

    auto cause = lost != nullptr ? lost : help;
    auto needed = cause != nullptr && cause->TotalTroops() > men ? cause->TotalTroops() - men : 0u;
    mxstronghold* barracks = nullptr;
    for ( auto stronghold : stores ) {
        if ( ( stronghold->TotalTroops() - stronghold->MinTroops() ) / 2 >= needed && nearer(stronghold, barracks) )
            barracks = stronghold;
    }

    if ( upon != nullptr && !sheltered && ( lord->HasQuality(qf_cowardly) || upon->Total() > men ) && home != nullptr )
        order(RE_RETREAT, QS_GOTO, mxentity::SafeIdt(home));
    else if ( marches && lost != nullptr && strongEnough(lost) )
        order(RE_TAKE_BACK_STRONGHOLD, QS_SEIZE, mxentity::SafeIdt(lost));
    else if ( service != nullptr )
        order(RE_LEND_SERVICE, QS_SEIZE, mxentity::SafeIdt(service));
    else if ( marches && help != nullptr && strongEnough(help) )
        order(RE_HELP_NEIGHBOUR, QS_SEIZE, mxentity::SafeIdt(help));
    else if ( marches && invader != nullptr && men >= invader->Total() )
        order(RE_ATTACK_ENEMY, QS_KILL, mxentity::SafeIdt(invader));
    else if ( !lord->HasQuality(qf_cowardly) && danger != nullptr && here != danger->Location() )
        order(RE_COUNTER_THREAT, QS_GOTO, mxentity::SafeIdt(danger));
    else if ( marches && needed > 0 && barracks != nullptr ) {
        if ( here == barracks->Location() ) {
            GatherStrength(barracks, needed);
            order(RE_GATHER_STRENGTH, QS_GUARD, MAKE_LOCID(here.x, here.y));
        } else {
            order(RE_GATHER_STRENGTH, QS_GOTO, mxentity::SafeIdt(barracks));
        }
    }
    else if ( home != nullptr && here != home->Location() )
        order(RE_RETURN_HOME, QS_GOTO, mxentity::SafeIdt(home));
    else
        order(RE_STAND_FIRM, QS_GUARD, MAKE_LOCID(here.x, here.y));
}

void citadel_quest_processor::GatherStrength ( mxstronghold* stronghold, u32 needed )
{
    auto spare = ( stronghold->TotalTroops() - stronghold->MinTroops() ) / 2;
    auto type = stronghold->Type() == UT_RIDERS ? UT_RIDERS : UT_WARRIORS;
    auto& unit = type == UT_RIDERS ? static_cast<mxunit&>(lord->riders) : static_cast<mxunit&>(lord->warriors);
    auto room = (u32)( type == UT_RIDERS ? sv_character_max_riders : sv_character_max_warriors );
    auto wanted = std::min({ needed, spare, room > unit.Total() ? room - unit.Total() : 0u });
    unit.Total(unit.Total() + stronghold->Remove(lord->Race(), type, wanted));
}

void citadel_quest_processor::Wander ( void )
{
    if ( lord->quest == QS_GOTO && lord->Location() != lord->QuestLocation() )
        return;     // still on his way

    auto here = lord->Location();
    for ( int tries = 0; tries < 8; tries++ ) {
        mxgridref there ( here.x + mxrandom(0, 2 * WANDERING) - WANDERING, here.y + mxrandom(0, 2 * WANDERING) - WANDERING );
        CONTINUE_IF( !mx->gamemap->IsLocOnMap(there) || there == here || !Reachable(here, there) );
        lord->reaction = RE_RETURN_HOME;
        lord->quest = QS_GOTO;
        lord->questtarget = MAKE_LOCID(there.x, there.y);
        return;
    }
}

void citadel_quest_processor::Impatience ( citadel_character* lord )
{
    this->lord = lord;
    if ( lord->idle < IMPATIENCE || !lord->HasQuality(qf_impatient)
         || lord->IsDead() || lord->IsPrisoner() || lord->IsFollowing() )
        return;

    auto here = lord->Location();
    auto first = [&here]( auto& candidates, auto preferred ) {
        std::stable_sort(candidates.begin(), candidates.end(), [&]( auto a, auto b ) {
            auto pa = preferred(a);
            auto pb = preferred(b);
            return pa != pb ? pa : here - a->Location() < here - b->Location();
        });
        using candidate_t = typename std::decay_t<decltype(candidates)>::value_type;
        for ( candidate_t candidate : candidates ) {
            if ( Reachable(here, candidate->Location()) )
                return candidate;
        }
        return candidate_t(nullptr);
    };

    std::vector<mxcharacter*> recruits;
    FOR_EACH_CHARACTER(character) {
        if ( lord->CanQuest(QS_RECRUIT, mxentity::SafeIdt(character)) && lord->CheckRecruitChar(character) )
            recruits.push_back(character);
    }
    auto recruit = first(recruits, [lord]( mxcharacter* c ) { return c->Race() == lord->Race(); });

    std::vector<mxstronghold*> keeps;
    if ( recruit == nullptr ) {
        FOR_EACH_STRONGHOLD(stronghold) {
            if ( stronghold->IsEnemy() && Men(lord) >= stronghold->TotalTroops() && mx->gamemap->IsLocOnMap(stronghold->Location()) )
                keeps.push_back(stronghold);
        }
    }
    auto keep = first(keeps, []( mxstronghold* ) { return false; });

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
                Attack();
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

void citadel_quest_processor::Attack ( void )
{
    if ( !lord->IsAIControlled() ) {
        lord->Cmd_Attack();
        return;
    }

    if ( lord->Cmd_WalkForward(false, false) == MX_OK )
        lord->EnterBattle();
}

bool citadel_quest_processor::Guard ( void )
{
    if ( !lord->IsRecruited() && !Marches(lord) )
        return false;

    auto here = lord->Location();
    FOR_EACH_CHARACTER(foe) {
        CONTINUE_IF( foe->Race() != RA_ENEMY || foe->IsDead() || foe->IsPrisoner() || !Near(here, foe->Location()) );
        CitadelBattle()->Duel(lord, foe);
        return true;
    }

    for ( auto regiment : Host() ) {
        CONTINUE_IF( regiment->Location() == here || !Near(here, regiment->Location()) || Men(lord) < regiment->Total() );
        lord->looking = here.DirFromHere(regiment->Location());
        Attack();
        return true;
    }
    return false;
}

void citadel_quest_processor::FreeHostages ( void )
{
    FOR_EACH_CHARACTER(character) {
        CONTINUE_IF( !character->IsPrisoner() || character->IsDead() || character->Location() != lord->Location() );
        lord->Cmd_Approach(character);
        auto hostage = CitadelLord(character);
        auto home = HomeKeep(hostage->Race(), hostage->Location());
        if ( home != nullptr )
            hostage->SetQuest(QS_GOTO, MAKE_LOCID(home->Location().x, home->Location().y));
    }
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
            live = character != nullptr && character->IsAlive() && character->IsPrisoner();
            break;
        case QS_SEIZE:
            live = stronghold != nullptr;
            break;
        case QS_FIND:
            live = object != nullptr && ObjectOnMap(object, lies);
            break;
        case QS_TAKE:
            live = holder != nullptr && holder != lord && holder->IsRecruited() && holder->IsAlive();
            break;
        case QS_DESTROY:
            live = object != nullptr && ( ObjectOnMap(object, lies) || holder == lord );
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

    auto target = lord->QuestLocation();

    if ( lord->quest == QS_KILL && character != nullptr ) {
        if ( !Near(lord->Location(), target) && !March(target, false) ) {
            Done(QN_FAILED);
            return;
        }
        if ( Near(lord->Location(), character->Location()) ) {
            CitadelBattle()->Duel(lord, character);
            if ( character->IsDead() )
                Done(QN_DONE);
        }
        return;
    }

    if ( lord->quest == QS_DESTROY && holder == lord ) {
        lord->carrying = nullptr;
        LiftObject(object);
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
        case QS_RESCUE:
            FreeHostages();
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
            LiftObject(object);
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
