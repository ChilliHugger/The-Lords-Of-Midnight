//
//  citadel_processor_quest.cpp
//  citadel
//

#include "../../baseinc/tme_internal.h"
#include "scenario_citadel_internal.h"
#include "citadel_processor_quest.h"

#if defined(_CITADEL_)
namespace tme {

void citadel_quest_processor::Process ( citadel_character* lord )
{
    this->lord = lord;
    if ( !lord->IsRecruited() )
        React(lord);
    Quest();
}

void citadel_quest_processor::React ( citadel_character* lord )
{
    this->lord = lord;
    if ( lord->purpose != PU_DEFEND_HOMELAND || lord->IsDead() || lord->IsPrisoner() || lord->Race() == RA_ENEMY )
        return;

    auto people = lord->Race();
    auto here = lord->Location();
    auto nearer = [&here]( mxstronghold* stronghold, mxstronghold* best ) {
        return best == nullptr || here - stronghold->Location() < here - best->Location();
    };
    auto feuding = [people]( mxrace_t other ) {
        return CitadelRace(people)->IsFeudingWith(other) || CitadelRace(other)->IsFeudingWith(people);
    };
    auto strongEnough = [lord]( mxstronghold* stronghold ) {
        return lord->warriors.Total() + lord->riders.Total() >= stronghold->TotalTroops();
    };

    mxstronghold* home = nullptr;   // the nearest keep his people still hold
    mxstronghold* lost = nullptr;   // the nearest keep of his realm that Boroth has taken
    mxstronghold* help = nullptr;   // the nearest keep Boroth has taken from a friendly neighbour
    FOR_EACH_STRONGHOLD(stronghold) {
        CONTINUE_IF( !mx->gamemap->IsLocOnMap(stronghold->Location()) );
        if ( stronghold->Race() == people ) {
            if ( stronghold->OccupyingRace() == people && nearer(stronghold, home) )
                home = stronghold;
            else if ( stronghold->IsEnemy() && nearer(stronghold, lost) )
                lost = stronghold;
        } else if ( stronghold->IsEnemy() && CITADEL_SCENARIO(Borders(people, stronghold->Race()))
                    && !feuding(stronghold->Race()) && nearer(stronghold, help) ) {
            help = stronghold;
        }
    }

    bool marches = mx->scenario->HostageOfRace(people) == nullptr && !lord->HasQuality(qf_cowardly);

    auto order = [lord]( mxreaction_t reaction, mxquest_t quest, mxid target ) {
        lord->reaction = reaction;
        lord->quest = quest;
        lord->questtarget = target;
    };

    if ( marches && lost != nullptr && strongEnough(lost) )
        order(RE_TAKE_BACK_STRONGHOLD, QS_SEIZE, mxentity::SafeIdt(lost));
    else if ( marches && help != nullptr && strongEnough(help) )
        order(RE_HELP_NEIGHBOUR, QS_SEIZE, mxentity::SafeIdt(help));
    else if ( home != nullptr && here != home->Location() )
        order(RE_RETURN_HOME, QS_GOTO, mxentity::SafeIdt(home));
    else
        order(RE_STAND_FIRM, QS_GUARD, MAKE_LOCID(here.x, here.y));
}

bool citadel_quest_processor::March ( mxgridref target, bool fight )
{
    const u32 tired = 2 * (u32)sv_energy_scale;

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

void citadel_quest_processor::Quest ( void )
{
    if ( lord->IsDead() || lord->IsPrisoner() || lord->IsFollowing() )
        return;

    auto character = CharacterTarget(lord->questtarget);
    auto stronghold = StrongholdTarget(lord->questtarget);

    switch ( lord->quest ) {
        case QS_RECRUIT:
            if ( character == nullptr || character->IsDead() || character->IsRecruited() ) {
                lord->quest = QS_NONE;
                return;
            }
            break;
        case QS_JOIN:
        case QS_FOLLOW:
            if ( character == nullptr || character->IsDead() ) {
                lord->quest = QS_NONE;
                return;
            }
            break;
        case QS_SEIZE:
            if ( stronghold == nullptr ) {
                lord->quest = QS_NONE;
                return;
            }
            break;
        case QS_GOTO:
        case QS_GUARD:
            break;
        default:
            return;     // resting, waiting, or a quest not built yet
    }

    if ( !March(lord->QuestLocation(), lord->quest == QS_SEIZE) ) {
        lord->quest = QS_NONE;
        return;
    }

    if ( lord->Location() != lord->QuestLocation() )
        return;         // still on the road

    switch ( lord->quest ) {
        case QS_RECRUIT:
            if ( lord->CheckRecruitChar(character) && lord->Cmd_Approach(character) != nullptr )
                character->Cmd_Follow(lord);
            lord->quest = QS_NONE;
            break;
        case QS_JOIN:
            lord->Cmd_Follow(character);
            lord->quest = QS_NONE;
            break;
        case QS_GOTO:
            lord->quest = QS_NONE;
            break;
        case QS_SEIZE:
            if ( !stronghold->IsEnemy() )
                lord->quest = QS_NONE;
            break;
        default:
            break;      // a shadow keeps shadowing, a guard keeps guarding
    }
}

} // namespace tme
#endif // _CITADEL_
