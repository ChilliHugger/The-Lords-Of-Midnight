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

template<typename T>
T citadel_quest_processor::Pick ( const std::vector<T>& candidates ) const
{
    return candidates.empty() ? nullptr : candidates[mxrandom(0, (int)candidates.size() - 1)];
}

void citadel_quest_processor::Process ( citadel_character* lord )
{
    this->lord = lord;
    if ( lord->IsRecruited() ) {
        lord->QuestNews(QN_NONE);
        Impatience(lord);
    } else {
        React(lord);
    }
    Quest();
}

void citadel_quest_processor::React ( citadel_character* lord )
{
    this->lord = lord;
    if ( !surveyed )
        Survey();
    if ( !lord->IsAwaitingOrders() )
        return;

    if ( CITADEL_SCENARIO(BorothFallen()) && lord->Home() != nullptr && lord->Home()->Race() == RA_ENEMY ) {
        AttackNearestRivalKeep();
        return;
    }

    switch ( lord->Purpose() ) {
        case PU_DEFEND_HOMELAND:
        case PU_BE_A_HOSTAGE:       // out of the dungeons, a hostage goes home and defends it
            DefendHome();
            break;
        case PU_RANDOMLY_WANDER:
            Wander();
            break;
        case PU_DARK_FEY:
            DefendMaranorOrGotoFrontline();
            break;
        default:
            break;                  // a lord with no purpose waits
    }
}

u32 citadel_quest_processor::PlaceOf ( mxgridref loc ) const
{
    if ( !mx->gamemap->IsLocOnMap(loc) )
        return 0;
    auto land = (u32)mx->gamemap->GetAt(loc).area;
    return land < places.size() && places[land].keep != nullptr ? land : 0;
}

void citadel_quest_processor::Survey ( void )
{
    surveyed = true;
    places.assign(mx->objAreaInfos.Count(), place_t());

    FOR_EACH_STRONGHOLD(stronghold) {
        CONTINUE_IF( !mx->gamemap->IsLocOnMap(stronghold->Location()) );
        auto land = (u32)mx->gamemap->GetAt(stronghold->Location()).area;
        if ( land != 0 && land < places.size() )
            places[land].keep = static_cast<citadel_stronghold*>(stronghold);
    }

    for ( u32 land = 0; land < places.size(); land++ ) {
        CONTINUE_IF( places[land].keep == nullptr );
        auto area = static_cast<citadel_area*>(mx->AreaById(land));
        for ( auto neighbour : area->neighbours ) {
            u32 next = GET_ID(neighbour);
            if ( next != land && next < places.size() && places[next].keep != nullptr )
                places[land].around.push_back(next);
        }
    }

    FOR_EACH_CHARACTER(character) {
        auto who = CitadelLord(character);
        CONTINUE_IF( who->IsDead() || who->IsPrisoner() || who->InDungeon() );
        auto land = PlaceOf(who->Location());
        if ( land != 0 )
            places[land].figures.push_back(who);
    }
    FOR_EACH_REGIMENT(regiment) {
        CONTINUE_IF( regiment->Total() == 0 );
        auto land = PlaceOf(regiment->Location());
        if ( land != 0 )
            places[land].figures.push_back(regiment);
    }

    realms.clear();
    for ( auto& place : places ) {
        if ( place.keep != nullptr )
            realms[place.keep->Race()] = LS_AT_PEACE;
    }
    for ( auto& [realm, danger] : realms ) {
        auto lands = RealmPlaces(realm);
        if ( std::all_of(lands.begin(), lands.end(), [this]( u32 land ) { return places[land].keep->Subjugated(); }) )
            danger = LS_SUBJUGATED;
        else if ( std::any_of(lands.begin(), lands.end(), [this]( u32 land ) { return Invaded(land, land); }) )
            danger = CITADEL_SCENARIO(AtWar(realm)) ? LS_UNDER_ATTACK : LS_THREATENED;
    }

    for ( u32 land = 0; land < places.size(); land++ ) {
        auto& place = places[land];
        CONTINUE_IF( place.keep == nullptr );
        auto here = Invaded(land, land);
        auto around = std::any_of(place.around.begin(), place.around.end(), [this, land]( u32 next ) { return Invaded(next, land); });
        auto realm = realms[place.keep->Race()];
        if ( place.keep->Subjugated() )
            place.danger = LS_SUBJUGATED;
        else if ( realm == LS_UNDER_ATTACK )
            place.danger = here ? LS_UNDER_ATTACK : around ? LS_IN_PERIL : LS_IN_DANGER;
        else if ( realm == LS_THREATENED )
            place.danger = here ? LS_MENACED : around ? LS_THREATENED : LS_TROUBLED;
        else
            place.danger = LS_AT_PEACE;
    }
}

bool citadel_quest_processor::Invaded ( u32 at, u32 of ) const
{
    auto ruler = places[of].keep->Allegiance();
    return std::any_of(places[at].figures.begin(), places[at].figures.end(), [ruler]( const mxitem* figure ) {
        return CITADEL_SCENARIO(IsEnemyOf(CITADEL_SCENARIO(Allegiance(figure)), ruler));
    });
}

mxdanger_t citadel_quest_processor::LandDanger ( u32 place ) const
{
    return place < places.size() ? places[place].danger : LS_AT_PEACE;
}

mxdanger_t citadel_quest_processor::RealmDanger ( mxrace_t realm ) const
{
    auto found = realms.find(realm);
    return found != realms.end() ? found->second : LS_AT_PEACE;
}

u32 citadel_quest_processor::Home ( const citadel_character* who ) const
{
    return who->Home() != nullptr ? PlaceOf(who->Home()->Location()) : 0;
}

mxdanger_t citadel_quest_processor::HomeDanger ( void ) const
{
    return LandDanger(Home(lord));
}

std::vector<u32> citadel_quest_processor::NearPlaces ( void ) const
{
    auto home = Home(lord);
    return home != 0 ? places[home].around : std::vector<u32>();
}

std::vector<u32> citadel_quest_processor::AdjacentPlaces ( void ) const
{
    auto here = mx->gamemap->IsLocOnMap(lord->Location())
        ? static_cast<citadel_area*>(mx->AreaById((u32)mx->gamemap->GetAt(lord->Location()).area)) : nullptr;
    auto lands = NearPlaces();
    std::erase_if(lands, [here]( u32 land ) { return here == nullptr || !here->Borders(mx->AreaById(land)); });
    return lands;
}

std::vector<u32> citadel_quest_processor::RealmPlaces ( mxrace_t realm ) const
{
    std::vector<u32> lands;
    for ( u32 land = 0; land < places.size(); land++ ) {
        if ( places[land].keep != nullptr && places[land].keep->Race() == realm )
            lands.push_back(land);
    }
    return lands;
}

std::vector<u32> citadel_quest_processor::NeighbouringRealmPlaces ( void ) const
{
    std::vector<u32> lands;
    auto here = PlaceOf(lord->Location());
    if ( here == 0 )
        return lands;
    auto realm = places[here].keep->Race();
    for ( u32 land = 0; land < places.size(); land++ ) {
        if ( places[land].keep != nullptr && CITADEL_SCENARIO(Borders(realm, places[land].keep->Race())) )
            lands.push_back(land);
    }
    return lands;
}

std::vector<u32> citadel_quest_processor::AllPlaces ( void ) const
{
    std::vector<u32> lands;
    for ( u32 land = 0; land < places.size(); land++ ) {
        if ( places[land].keep != nullptr )
            lands.push_back(land);
    }
    return lands;
}

std::vector<citadel_character*> citadel_quest_processor::OwnersOf ( const std::vector<u32>& lands ) const
{
    std::vector<citadel_character*> owners;
    for ( auto land : lands ) {
        auto owner = places[land].keep->Owner();
        if ( owner != nullptr && std::find(owners.begin(), owners.end(), owner) == owners.end() )
            owners.push_back(CitadelLord(owner));
    }
    return owners;
}

std::vector<citadel_character*> citadel_quest_processor::Neighbours ( void ) const
{
    auto lands = NearPlaces();
    std::erase_if(lands, [this]( u32 land ) { return LandDanger(land) >= LS_UNDER_ATTACK; });
    auto lords = OwnersOf(lands);
    std::erase_if(lords, [this]( const citadel_character* other ) {
        return other == lord || other->IsDead() || other->IsPrisoner() || other->IsRecruited() || lord->IsHostileTo(other)
            || other->Home() == nullptr || PlaceOf(other->Location()) != Home(other)
            || other->Purpose() != PU_DEFEND_HOMELAND || other->Reaction() == RE_RETREAT;
    });
    return lords;
}

std::vector<citadel_character*> citadel_quest_processor::RecruitableLordsNearby ( void ) const
{
    auto around = NearPlaces();
    auto lords = OwnersOf(around);
    std::erase_if(lords, [this, &around]( const citadel_character* other ) {
        return other == lord || other->IsDead() || other->IsPrisoner() || other->IsRecruited() || lord->IsHostileTo(other)
            || other->HasQuality(qf_cowardly) || !other->IsAvailable()
            || std::find(around.begin(), around.end(), PlaceOf(other->Location())) == around.end();
    });
    return lords;
}

mxitem* citadel_quest_processor::RandomEnemyIn ( const std::vector<u32>& lands ) const
{
    std::vector<mxitem*> foes;
    for ( auto land : lands ) {
        CONTINUE_IF( land == 0 || land >= places.size() );
        for ( auto figure : places[land].figures ) {
            if ( figure != lord && lord->IsHostileTo(figure) )
                foes.push_back(figure);
        }
    }
    return Pick(foes);
}

//
// One of these lands' keeps, at random, that he can find a road to.
//
citadel_stronghold* citadel_quest_processor::RandomReachableKeep ( std::vector<u32> lands ) const
{
    // one march from where he stands answers for every keep: a road runs both ways
    const auto& steps = CITADEL_SCENARIO(StepsFrom(lord->Location(), lord));
    auto width = (u32)mx->gamemap->Size().cx;
    std::erase_if(lands, [&]( u32 land ) {
        auto where = places[land].keep->Location();
        return steps[where.y * width + where.x] < 0;
    });
    return lands.empty() ? nullptr : places[lands[mxrandom(0, (int)lands.size() - 1)]].keep;
}

void citadel_quest_processor::DefendHome ( void )
{
    if ( lord->Home() == nullptr )
        return;

    handover_t next = lord->Reaction();
    for ( u32 handovers = 0; next.has_value() && handovers <= HANDOVERS; handovers++ ) {
        lord->Reaction(*next);
        next = RunReaction(lord->Reaction());
    }
}

citadel_quest_processor::handover_t citadel_quest_processor::RunReaction ( mxreaction_t reaction )
{
    switch ( reaction ) {
        case RE_RETURN_HOME:            return ReturnHome();
        case RE_STAND_FIRM:             return StandFirm();
        case RE_TAKE_BACK_STRONGHOLD:   return TakeBackStronghold();
        case RE_ATTACK_ENEMY:           return AttackEnemy();
        case RE_HELP_NEIGHBOUR:         return HelpNeighbour();
        case RE_RETREAT:                return FleeToSafeKeep();
        case RE_GATHER_STRENGTH:        return GatherStrength();
        case RE_COUNTER_THREAT:         return CounterThreat();
        case RE_LEND_SERVICE:           return ServeLeader();
        default:                        return {};
    }
}

citadel_quest_processor::handover_t citadel_quest_processor::ReturnHome ( void )
{
    if ( HomeDanger() == LS_SUBJUGATED )
        return RE_TAKE_BACK_STRONGHOLD;
    if ( lord->AtHome() )
        return RE_STAND_FIRM;

    lord->Flags().Reset(cf_available);
    if ( lord->QuestIdle() )
        lord->AssignQuest(QS_GOTO, mxentity::SafeIdt(lord->Home()));
    return {};
}

citadel_quest_processor::handover_t citadel_quest_processor::StandFirm ( void )
{
    auto danger = HomeDanger();
    auto brave = lord->HasQuality(qf_brave);
    auto cowardly = lord->HasQuality(qf_cowardly);
    auto cautious = lord->HasQuality(qf_cautious);
    auto reckless = lord->HasQuality(qf_reckless);
    auto selfish = lord->HasQuality(qf_selfish);

    // while his realm's hostage is held, he only defends
    if ( lord->Marches() ) {
        if ( danger == LS_IN_PERIL && lord->HasQuality(qf_farsighted) && !cowardly && !RecruitableLordsNearby().empty() )
            return RE_GATHER_STRENGTH;

        if ( danger == LS_IN_PERIL && ( ( brave && !cautious ) || ( reckless && !cowardly ) ) )
            return RE_ATTACK_ENEMY;

        if ( danger == LS_IN_DANGER && !cowardly && !selfish ) {
            if ( auto subject = Pick(Neighbours()) ) {
                lord->AssignQuest(QS_JOIN, mxentity::SafeIdt(subject));
                return RE_HELP_NEIGHBOUR;
            }
        }

        if ( danger == LS_TROUBLED && !cowardly && !selfish ) {
            auto around = NearPlaces();
            if ( std::any_of(around.begin(), around.end(), [this]( u32 land ) { return LandDanger(land) == LS_THREATENED; }) )
                return RE_COUNTER_THREAT;
        }
    }

    auto attacker = danger == LS_UNDER_ATTACK ? RandomEnemyIn({ Home(lord) }) : nullptr;
    if ( attacker != nullptr && !brave && !lord->CanFaceAtOdds(attacker, 3, 1) )
        return RE_RETREAT;

    lord->Flags().Set(cf_available, danger != LS_UNDER_ATTACK);
    lord->AssignQuest(QS_REST, IDT_NONE);
    return {};
}

citadel_quest_processor::handover_t citadel_quest_processor::TakeBackStronghold ( void )
{
    if ( HomeDanger() != LS_SUBJUGATED )
        return RE_STAND_FIRM;

    lord->Flags().Reset(cf_available);
    if ( lord->QuestIdle() )
        lord->AssignQuest(QS_SEIZE, mxentity::SafeIdt(lord->Home()));
    return {};
}

citadel_quest_processor::handover_t citadel_quest_processor::AttackEnemy ( void )
{
    auto home = Home(lord);
    auto here = PlaceOf(lord->Location());
    auto around = NearPlaces();
    auto inHomeland = here == home;
    auto inNear = std::find(around.begin(), around.end(), here) != around.end();
    auto subject = CharacterTarget(lord->QuestTarget());
    auto host = RegimentTarget(lord->QuestTarget());

    if ( !inHomeland && HomeDanger() == LS_UNDER_ATTACK )
        return RE_RETURN_HOME;
    if ( RandomEnemyIn({ home }) == nullptr && RandomEnemyIn(around) == nullptr )
        return RE_RETURN_HOME;
    if ( !inHomeland && inNear )
        return RE_RETURN_HOME;
    if ( ( subject != nullptr && subject->IsDead() ) || ( host != nullptr && host->Total() == 0 ) )
        return RE_RETURN_HOME;

    lord->Flags().Set(cf_available);
    if ( lord->QuestIdle() ) {
        auto foe = RandomEnemyIn({ here });
        if ( foe == nullptr )
            foe = RandomEnemyIn({ home });
        if ( foe == nullptr )
            foe = RandomEnemyIn(AdjacentPlaces());
        if ( foe == nullptr )
            foe = RandomEnemyIn(around);
        if ( foe != nullptr )
            lord->AssignQuest(QS_KILL, mxentity::SafeIdt(foe));
        else
            lord->AssignQuest(QS_REST, IDT_NONE);
    }
    return {};
}

citadel_quest_processor::handover_t citadel_quest_processor::HelpNeighbour ( void )
{
    auto danger = HomeDanger();
    auto subject = static_cast<citadel_character*>(CharacterTarget(lord->QuestTarget()));

    if ( danger == LS_UNDER_ATTACK || subject == nullptr || subject->IsDead()
         || subject->Purpose() != PU_DEFEND_HOMELAND || subject->Reaction() == RE_RETREAT
         || ( danger == LS_IN_PERIL && LandDanger(Home(subject)) == LS_IN_DANGER ) )
        return RE_RETURN_HOME;

    lord->Flags().Reset(cf_available);
    if ( lord->QuestIdle() )
        lord->AssignQuest(QS_JOIN, mxentity::SafeIdt(subject));
    return {};
}

citadel_quest_processor::handover_t citadel_quest_processor::FleeToSafeKeep ( void )
{
    if ( HomeDanger() != LS_UNDER_ATTACK )
        return RE_RETURN_HOME;

    lord->Flags().Set(cf_available);
    if ( lord->QuestIdle() ) {
        auto troubled = [this]( u32 land ) { return LandDanger(land) >= LS_UNDER_ATTACK; };
        auto unquiet = [this]( u32 land ) { return LandDanger(land) != LS_AT_PEACE; };
        auto around = NearPlaces();
        auto realm = RealmPlaces(lord->Home()->Race());
        auto neighbouring = NeighbouringRealmPlaces();
        auto all = AllPlaces();
        std::erase_if(around, troubled);
        std::erase_if(realm, troubled);
        std::erase_if(neighbouring, unquiet);
        std::erase_if(all, unquiet);

        citadel_stronghold* refuge = nullptr;
        for ( auto lands : { around, realm, neighbouring, all } ) {
            if ( refuge == nullptr )
                refuge = RandomReachableKeep(lands);
        }
        if ( refuge != nullptr )
            lord->AssignQuest(QS_GOTO, mxentity::SafeIdt(refuge));
        else
            lord->AssignQuest(QS_REST, IDT_NONE);
    }
    return {};
}

citadel_quest_processor::handover_t citadel_quest_processor::GatherStrength ( void )
{
    if ( HomeDanger() == LS_UNDER_ATTACK )
        return RE_RETURN_HOME;
    if ( RandomEnemyIn({ PlaceOf(lord->Location()) }) != nullptr )
        return RE_ATTACK_ENEMY;

    lord->Flags().Reset(cf_available);
    if ( lord->QuestIdle() ) {
        auto recruit = Pick(RecruitableLordsNearby());
        if ( recruit != nullptr )
            lord->AssignQuest(QS_RECRUIT, mxentity::SafeIdt(recruit));
        else
            lord->AssignQuest(QS_REST, IDT_NONE);
    }
    return {};
}

citadel_quest_processor::handover_t citadel_quest_processor::CounterThreat ( void )
{
    if ( HomeDanger() > LS_TROUBLED )
        return RE_RETURN_HOME;

    lord->Flags().Reset(cf_available);
    std::vector<citadel_stronghold*> threatened;
    for ( auto land : NearPlaces() ) {
        if ( LandDanger(land) == LS_THREATENED )
            threatened.push_back(places[land].keep);
    }
    if ( threatened.empty() )
        return {};

    auto here = PlaceOf(lord->Location());
    if ( std::any_of(threatened.begin(), threatened.end(), [this, here]( citadel_stronghold* keep ) { return PlaceOf(keep->Location()) == here; }) )
        lord->AssignQuest(QS_REST, IDT_NONE);
    else if ( lord->QuestIdle() )
        lord->AssignQuest(QS_GOTO, mxentity::SafeIdt(Pick(threatened)));
    return {};
}

citadel_quest_processor::handover_t citadel_quest_processor::ServeLeader ( void )
{
// TODO: Not sure this is correct, following is a UI concept
    auto leader = lord->Following();
    if ( HomeDanger() == LS_UNDER_ATTACK || leader == nullptr || leader->IsDead() )
        return RE_RETURN_HOME;

    lord->Flags().Reset(cf_available);
    return {};
}

void citadel_quest_processor::Wander ( void )
{
    if ( lord->Quest() == QS_GOTO && lord->Location() != lord->QuestLocation() )
        return;     // still on his way

    auto here = lord->Location();
    for ( int tries = 0; tries < 8; tries++ ) {
        mxgridref there ( here.x + mxrandom(0, 2 * WANDERING) - WANDERING, here.y + mxrandom(0, 2 * WANDERING) - WANDERING );
        CONTINUE_IF( !mx->gamemap->IsLocOnMap(there) || there == here || !CITADEL_SCENARIO(Reachable(lord, here, there)) );
        lord->Reaction(RE_RETURN_HOME);
        lord->Quest(QS_GOTO);
        lord->QuestTarget(MAKE_LOCID(there.x, there.y));
        return;
    }
}

void citadel_quest_processor::DefendMaranorOrGotoFrontline ( void )
{
    auto maranor = CITADEL_SCENARIO(maranor);

    if ( maranor->HasFallen() ) {
        lord->AssignQuest(QS_SEIZE, mxentity::SafeIdt(maranor));
        return;
    }

    auto front = CITADEL_SCENARIO(FrontlineKeep());
    auto camp = front != nullptr && CITADEL_SCENARIO(Reachable(lord, lord->Location(), front->Location())) ? front : maranor;
    if ( lord->Location() != camp->Location() )
        lord->AssignQuest(QS_GOTO, mxentity::SafeIdt(camp));
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
        if ( CITADEL_SCENARIO(Reachable(lord, here, candidate->Location())) )
            return candidate;
    }
    return nullptr;
}

void citadel_quest_processor::AttackNearestRivalKeep ( void )
{
    std::vector<mxstronghold*> keeps;
    FOR_EACH_STRONGHOLD(stronghold) {
        if ( mx->gamemap->IsLocOnMap(stronghold->Location())
             && static_cast<citadel_stronghold*>(stronghold)->Allegiance() != lord->NormalisedLoyalty() )
            keeps.push_back(stronghold);
    }

    lord->Reaction(RE_ATTACK_ENEMY);
    auto keep = FirstReachable(keeps);
    if ( keep != nullptr )
        lord->AssignQuest(QS_SEIZE, mxentity::SafeIdt(keep));
}

void citadel_quest_processor::Impatience ( citadel_character* lord )
{
    this->lord = lord;
    if ( lord->Idle() < IMPATIENCE || !lord->HasQuality(qf_impatient)
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
        lord->QuestNews(QN_IMPATIENT);
}

bool citadel_quest_processor::MarchToTarget ( mxgridref target, bool fight )
{
    const u32 tired = 2 * (u32)sv_energy_scale;
    blocked = false;

    while ( lord->Location() != target && lord->CanWalkForward() && lord->energy >= tired ) {
        mxgridref step;
        if ( !CITADEL_SCENARIO(MarchStep(lord, lord->Location(), target, step)) )
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

void citadel_quest_processor::Besiege ( citadel_stronghold* keep, mxgridref target )
{
    if ( keep->Allegiance() != lord->NormalisedLoyalty() ) {
        if ( !lord->Location().IsNear(target) && !MarchToTarget(target, false) ) {
            lord->EndQuest(QN_FAILED);
            return;
        }
        if ( !lord->Location().IsNear(target) ) {
            if ( blocked )
                lord->ReportQuestBlocked();
            return;
        }
        if ( !CitadelBattle()->Siege(lord, keep) )
            return;     // another siege is fought tonight: he waits
        if ( lord->IsDead() ) {
            lord->EndQuest(QN_FAILED);
            return;
        }
        if ( keep->Allegiance() != lord->NormalisedLoyalty() )
            return;     // the siege goes on
    }

    if ( lord->Location() != target && !MarchToTarget(target, false) ) {
        lord->EndQuest(QN_FAILED);
        return;
    }
    if ( lord->Location() == target )
        lord->EndQuest(QN_DONE);
}

void citadel_quest_processor::Quest ( void )
{
    if ( lord->IsDead() || lord->IsPrisoner() )
        return;

    blocked = false;
    if ( lord->IsFollowing() ) {
        if ( lord->Quest() == QS_JOIN )
            lord->EndQuest(QN_DONE);
        return;     // a fellowship goes where its leader goes
    }

    auto character = CharacterTarget(lord->QuestTarget());
    auto stronghold = StrongholdTarget(lord->QuestTarget());
    auto object = ObjectTarget(lord->QuestTarget());
    auto regiment = RegimentTarget(lord->QuestTarget());
    auto holder = object != nullptr ? mx->objectprocessor->WhoHasObject(object) : nullptr;

    bool live = true;
    switch ( lord->Quest() ) {
        case QS_RECRUIT:
            live = character != nullptr && character->IsAlive() && !character->IsRecruited();
            break;
        case QS_JOIN:
        case QS_FOLLOW:
            live = character != nullptr && character->IsAlive();
            break;
        case QS_KILL:
            live = character != nullptr ? character->IsAlive() && !( lord->IsRecruited() && character->IsRecruited() )
                                        : regiment != nullptr && regiment->Total() > 0;
            break;
        case QS_RESCUE:
            live = lord->InDungeon() && ( character == nullptr || ( character->IsAlive() && character->IsPrisoner() ) );
            break;
        case QS_SEIZE:
            live = stronghold != nullptr;
            break;
        case QS_FIND:
            live = object != nullptr && object->IsOnMap();
            break;
        case QS_TAKE:
            live = holder != nullptr && holder != lord && holder->IsAlive();
            break;
        case QS_DESTROY:
            live = object != nullptr && ( object->IsOnMap() || holder == lord || ( holder != nullptr && holder->IsAlive() ) );
            break;
        case QS_GOTO:
        case QS_GUARD:
            break;
        default:
            return;     // resting, or waiting for orders
    }

    if ( !live ) {
        lord->EndQuest(QN_FAILED);
        return;
    }

    if ( lord->Quest() == QS_RESCUE ) {
        auto hostage = lord->SearchDungeon((u32)sv_dungeon_search_night);
        if ( hostage != nullptr )
            mx->SetLastActionMsg(mx->LastActionMsg() + mx->text->CookedSystemString(SS_DUNGEON_FREED, hostage));
        if ( ( character != nullptr && !character->IsPrisoner() ) || CITADEL_SCENARIO(HostagesHeldAtMaranor()).empty() )
            lord->EndQuest(QN_DONE);
        return;
    }

    auto target = lord->QuestLocation();

    if ( lord->Quest() == QS_KILL && character != nullptr ) {
        if ( !lord->Location().IsNear(target) && !MarchToTarget(target, false) ) {
            lord->EndQuest(QN_FAILED);
            return;
        }
        if ( lord->Location().IsNear(character->Location()) ) {
            CitadelBattle()->Duel(lord, character);
            if ( character->IsDead() )
                lord->EndQuest(QN_DONE);
        } else if ( blocked && !lord->IsRecruited() ) {
            lord->ReportQuestBlocked();
        }
        return;
    }

    if ( lord->Quest() == QS_SEIZE && !stronghold->IsEnemy() ) {
        Besiege(stronghold, target);
        return;
    }

    if ( lord->Quest() == QS_DESTROY && holder == lord ) {
        mx->objectprocessor->Destroy(object);
        lord->EndQuest(QN_DONE);
        return;
    }

    if ( !MarchToTarget(target, lord->Quest() == QS_SEIZE || regiment != nullptr) ) {
        lord->EndQuest(QN_FAILED);
        return;
    }

    if ( lord->Location() != target ) {
        if ( blocked )
            lord->ReportQuestBlocked();
        return;         // still on the road
    }

    switch ( lord->Quest() ) {
        case QS_RECRUIT:
            if ( !lord->IsRecruited() ) {
                lord->AttemptToRecruit(CitadelLord(character));
            } else if ( lord->CheckRecruitChar(character) && lord->Cmd_Approach(character) != nullptr ) {
                character->Cmd_Follow(lord);
                lord->EndQuest(QN_DONE);
            } else if ( lord->RecruitScore(character) <= -1 ) {
                CitadelBattle()->Duel(character, lord);
                lord->EndQuest(QN_OFFENDED);
            } else {
                lord->EndQuest(QN_REFUSED);
            }
            break;
        case QS_JOIN:
            lord->MergeFellowship(character->IsFollowing() ? character->Following() : character);
            lord->EndQuest(QN_DONE);
            break;
        case QS_GOTO:
        case QS_KILL:   // a regiment: the battle is joined where it stands
            lord->EndQuest(QN_DONE);
            break;
        case QS_SEIZE:
            if ( !stronghold->IsEnemy() )
                lord->EndQuest(QN_DONE);
            break;
        case QS_FIND:
            lord->Cmd_PickupObject();
            lord->EndQuest(lord->Carrying() == object ? QN_DONE : QN_FAILED);
            break;
        case QS_TAKE:
            if ( lord->ObtainFrom(object, holder) )
                lord->EndQuest(QN_DONE);
            break;
            
        case QS_DESTROY:
            if ( holder == nullptr || lord->ObtainFrom(object, holder) ) {
                mx->objectprocessor->Destroy(object);
                lord->EndQuest(QN_DONE);
            }
            break;
        case QS_GUARD:
            lord->StandGuard();
            break;
        default:
            break;      // a shadow keeps shadowing
    }
}

} // namespace tme
#endif // _CITADEL_
