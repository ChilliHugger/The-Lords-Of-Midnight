/*
 * FILE:    citadel_scenario.cpp
 * 
 * PROJECT: citadel
 * 
 * CREATED: 
 * 
 * AUTHOR:  Chris Wild
 * 
 * Copyright 2017 Chilli Hugger. All rights reserved.
 * 
 * PURPOSE: 
 * 
 * 
 */

#include "../../baseinc/tme_internal.h"
#include "scenario_citadel.h"
#include "scenario_citadel_internal.h"
#include "citadel_processor_battle.h"
#include "citadel_processor_quest.h"
#include "../lom/lom_gameover.h"
#include <algorithm>
#include <map>
#include <memory>
#include <queue>
#include <string>
#include <vector>

#if defined(_CITADEL_)
namespace tme {

    static scenarioinfo_t    citadel_scenario_info = {
        mxscenarioid::CITADEL,
        100,
        "CITADEL Scenario",
        "Chris Wild",
        "rorthron@thelordsofmidnight.com",
        "http://www.thelordsofmidnight.com",
        "The Lords of Midnight 3 : The Citadel for the Midnight Engine",
        "Copyright 1994 - 2026 Mike Singleton & Chris Wild"
    };

static citadel_x* citadel_scenario = NULL ;

    
MXRESULT MXAPI citadel::Create ( tme::mxinterface* engine )
{
variant args;

    if ( engine == NULL )
        return MX_FAILED ;

    citadel_scenario = new citadel_x ;

    if ( citadel_scenario == NULL )
        return MX_FAILED ;

    args = citadel_scenario ;
    return engine->Command("@SETSCENARIO", &args, 1);

}
    

scenarioinfo_t* citadel::GetInfoBlock() const
    {
        return citadel_scenario->GetInfoBlock();
    }
    
MXRESULT citadel::Command ( const std::string& arg, variant argv[], u32 argc )
    {
        return citadel_scenario->Command(arg, argv, argc);
    }
    
MXRESULT citadel::GetProperties ( const std::string& arg, variant argv[], u32 argc )
    {
        return citadel_scenario->GetProperties(arg, argv, argc);
    }
    
MXRESULT citadel::Text ( const std::string& command, variant* argv, u32 args )
    {
        return citadel_scenario->Text(command, argv, args);
    }
    

//
// citadel_X
// Internal Scenario Class Code
//
//

citadel_x::citadel_x() :
    boroth(nullptr),
    luxor(nullptr),
    maranor(nullptr)
{
}

citadel_x::~citadel_x()
{
}

scenarioinfo_t* citadel_x::GetInfoBlock() const
{
    return &citadel_scenario_info ;
}

//
// THE HOSTAGES
//
// Boroth the Wolfheart holds one lord of each realm of the Bloodmarch in the dungeons of
// the Dark Citadel of Maranor, and while a realm's hostage is held there, its King will
// not march. Free the hostage and his people can be persuaded to your cause - which is,
// as the help puts it, the surest way to raise an army large enough to march on Maranor.
//
// This is the 1995 game's own roster. Every lord here stands at Maranor (145,220) in
// citadel/data/citadel_character.txt, and The Citadel design document names each of them
// in turn as the hostage of his or her realm. The Golden Fey alone have no hostage -
// their magic upon Immiel is such that Boroth dares not assail them - and the Dark Fey
// are the enemy, so twelve realms, twelve hostages.
//
// Luxor is held in those same dungeons and is deliberately NOT one of these. Freeing him
// is a story of the House of Moon rather than a realm won over, and he is the lord the
// engine hands the player first.
//
static LPCSTR citadel_hostages[] = {
    "CH_MOGRIK",        // Kith          - Mogrik the Witless, Prince of the Witherlands
    "CH_ZENETHOR",      // Atheling      - Zenethor the Strong, King of the Lee
    "CH_AREMELA",       // Eldrin        - Princess Aremela, the Queen's daughter
    "CH_OLTHRUDA",      // Long Dwarf    - Olthruda the Bountiful, the King's wife
    "CH_KIRANDA",       // Arakai        - Kiranda the Wild, the King's daughter
    "CH_SAGRANA",       // Dragonlord    - Sagrana Goldenwing, the King's sister
    "CH_EMEDREL",       // High Fey      - Emedrel of the Fire, the High King's daughter
    "CH_ALOROTH",       // Dawn Fey      - Aloroth the Fey, the King's brother
    "CH_DJALINA",       // Uskarg        - Djalina Snowheart, the King's daughter
    "CH_WYTHRAN",       // Gelming       - Wythran the Weaver, the King's only son
    "CH_THALGRIMA",     // Deeping Dwarf - Thalgrima the Betrothed, the King's bride to be
    "CH_MELINISSA",     // Giant         - Melinissa the Sweet, the King's stepdaughter
};

//
// Run on every start and every load. Boroth the Wolfheart is the Citadel's enemy, as Doomdark is
// Lords of Midnight's: a keep his wandering host takes becomes his, held against the player
// until it is taken back.
//
void citadel_x::initialise ( u32 version )
{
    mxscenario::initialise(version);
    paths.clear();
    passable.clear();
    boroth = CitadelLord(mx->CharacterBySymbol("CH_BOROTH"));
    luxor = CitadelLord(mx->CharacterBySymbol("CH_LUXOR"));
    maranor =static_cast<citadel_stronghold*>(mx->EntityByName("SH_CITADEL_MARANOR", IDT_STRONGHOLD));

    FOR_EACH_CHARACTER(character) {
        if ( character->HasQuality(qf_brave) )
            character->cowardess = 127;
        else if ( character->HasQuality(qf_cowardly) )
            character->cowardess = 30;
        else
            character->cowardess = 64;
        character->RefreshLocationBasedVariables(0);
    }
}

mxcharacter* citadel_x::BadGuy () const
{
    return boroth;
}

//
// Nothing in the Citadel's terrain blocks the sea yet - lords may one day take ship - but a
// regiment marches, it does not sail. Rivers are forded.
//
bool citadel_x::isTerrainImpassable ( mxterrain_t terrain, const mxitem* target ) const
{
    if ( target != nullptr && target->IsType(IDT_REGIMENT) ) {
        switch ( toGeneralisedTerrain(terrain) ) {
            case TN_SEA:
            case TN_BAY:
            case TN_LAKE:
                return true;
            default:
                break;
        }
    }

    return mxscenario::isTerrainImpassable(terrain, target);
}

//
// The Citadel's map is drawn in its own terrain codes; the marching costs are Lords of
// Midnight's, so generalise before asking for them.
//
u32 citadel_x::TerrainMovementModifier ( mxrace_t race, mxterrain_t terrain ) const
{
    return mxscenario::TerrainMovementModifier(race, toGeneralisedTerrain(terrain));
}

struct citadel_kingdom_t {
    mxrace_t    people;
    mxrace_t    borders[6];     // RA_NONE ends a shorter list
};

static const citadel_kingdom_t citadel_kingdoms[] = {
    { RA_KITH,             { RA_FREE, RA_ATHELING, RA_GOLDEN_FEY, RA_ELDRIN } },
    { RA_ATHELING,         { RA_KITH, RA_LONG_DWARF, RA_GOLDEN_FEY, RA_ELDRIN } },
    { RA_ELDRIN,           { RA_KITH, RA_ATHELING, RA_LONG_DWARF, RA_HIGH_FEY } },
    { RA_LONG_DWARF,       { RA_ATHELING, RA_ARAKAI, RA_DAWN_FEY, RA_DEEPING_DWARF, RA_HIGH_FEY, RA_ELDRIN } },
    { RA_ARAKAI,           { RA_LONG_DWARF, RA_DAWN_FEY, RA_DRAGONLORD } },
    { RA_DRAGONLORD,       { RA_ARAKAI } },
    { RA_HIGH_FEY,         { RA_ELDRIN, RA_LONG_DWARF, RA_DEEPING_DWARF, RA_GELMING } },
    { RA_DAWN_FEY,         { RA_LONG_DWARF, RA_ARAKAI, RA_USKARG, RA_DARK_FEY, RA_DEEPING_DWARF } },
    { RA_USKARG,           { RA_DAWN_FEY, RA_BLOODMARCH_GIANT, RA_DARK_FEY } },
    { RA_GELMING,          { RA_HIGH_FEY, RA_DEEPING_DWARF, RA_DARK_FEY } },
    { RA_DEEPING_DWARF,    { RA_LONG_DWARF, RA_DAWN_FEY, RA_DARK_FEY, RA_GELMING, RA_HIGH_FEY } },
    { RA_BLOODMARCH_GIANT, { RA_USKARG, RA_DARK_FEY } },
    { RA_DARK_FEY,         { RA_GELMING, RA_DEEPING_DWARF, RA_DAWN_FEY, RA_USKARG, RA_BLOODMARCH_GIANT } },
    { RA_GOLDEN_FEY,       { RA_KITH, RA_ATHELING, RA_ELDRIN } },
    { RA_FREE,             { RA_KITH } },
};

static const citadel_kingdom_t* KingdomOf ( mxrace_t people )
{
    for ( const auto& kingdom : citadel_kingdoms ) {
        if ( kingdom.people == people )
            return &kingdom;
    }
    return nullptr;
}

static bool KingdomsBorder ( mxrace_t a, mxrace_t b )
{
    for ( auto [from, to] : { std::pair{a, b}, std::pair{b, a} } ) {
        auto kingdom = KingdomOf(from);
        CONTINUE_IF_NULL(kingdom);
        for ( auto border : kingdom->borders ) {
            if ( border == to )
                return true;
        }
    }
    return false;
}

bool citadel_x::Borders ( mxrace_t a, mxrace_t b ) const
{
    return KingdomsBorder(a, b);
}

static u32 StepsToMidnight ( mxrace_t people )
{
    std::vector<mxrace_t> frontier { RA_FREE };
    std::vector<mxrace_t> seen { RA_FREE };
    for ( u32 steps = 0; !frontier.empty(); steps++ ) {
        std::vector<mxrace_t> next;
        for ( auto here : frontier ) {
            if ( here == people )
                return steps;
            for ( const auto& kingdom : citadel_kingdoms ) {
                CONTINUE_IF( std::find(seen.begin(), seen.end(), kingdom.people) != seen.end() );
                if ( KingdomsBorder(here, kingdom.people) ) {
                    seen.push_back(kingdom.people);
                    next.push_back(kingdom.people);
                }
            }
        }
        frontier = next;
    }
    return NUMELE(citadel_kingdoms);
}

citadel_x::pace_t citadel_x::PaceOf ( const mxitem* walker ) const
{
    if ( walker->IsType(IDT_CHARACTER) ) {
        auto lord = static_cast<const mxcharacter*>(walker);
        return { lord->Race(), !lord->IsRiding() };
    }
    return { static_cast<const mxregiment*>(walker)->Race(), false };
}

u32 citadel_x::StepCost ( mxrace_t race, bool onfoot, mxgridref into, mxdir_t dir ) const
{
    auto rinfo = mx->RaceById(race);
    u32 cost = rinfo->InitialMovementValue() + ( IS_DIAGONAL(dir) ? rinfo->DiagonalMovementModifier() : 0 );
    if ( onfoot )
        cost = (u32)((f64)cost * rinfo->RidingMovementMultiplier());
    return cost + TerrainMovementModifier(race, (mxterrain_t)mx->gamemap->GetAt(into).terrain);
}

const std::vector<s32>& citadel_x::StepsFrom ( mxgridref from, const mxitem* walker ) const
{
    auto [race, onfoot] = PaceOf(walker);

    auto width = (size_t)mx->gamemap->Size().cx;
    auto height = (size_t)mx->gamemap->Size().cy;
    auto start = (u32)(from.y * width + from.x);
    auto key = start | (u64)race << 32 | (u64)onfoot << 40;

    auto found = paths.find(key);
    if ( found != paths.end() )
        return found->second;

    if ( passable.empty() )
        passable.assign(width * height, -1);

    auto& costs = paths[key];
    costs.assign(width * height, -1);
    costs[start] = 0;
    using entry = std::pair<s32, u32>;
    std::priority_queue<entry, std::vector<entry>, std::greater<entry>> queue;
    queue.push({ 0, start });
    while ( !queue.empty() ) {
        auto [cost, at] = queue.top();
        queue.pop();
        CONTINUE_IF( cost > costs[at] );
        mxgridref here ( (s32)(at % width), (s32)(at / width) );
        s32 into[] = { (s32)StepCost(race, onfoot, here, DR_NORTH), (s32)StepCost(race, onfoot, here, DR_NORTHEAST) };
        for ( int dir = DR_NORTH; dir <= DR_NORTHWEST; dir++ ) {
            auto there = here + (mxdir_t)dir;
            CONTINUE_IF( !mx->gamemap->IsLocOnMap(there) );
            auto next = (u32)(there.y * width + there.x);
            auto total = cost + into[IS_DIAGONAL(dir) ? 1 : 0];
            CONTINUE_IF( costs[next] >= 0 && costs[next] <= total );
            if ( passable[next] < 0 )
                passable[next] = isLocationImpassable(there, mx->objRegiments[0]) ? 0 : 1;
            CONTINUE_IF( passable[next] == 0 );
            costs[next] = total;
            queue.push({ total, next });
        }
    }
    return costs;
}

bool citadel_x::MarchStep ( const mxitem* walker, mxgridref here, mxgridref target, mxgridref& step ) const
{
    if ( mx->objRegiments.Count() == 0 || here == target )
        return false;

    auto [race, onfoot] = PaceOf(walker);
    const auto& costs = StepsFrom(target, walker);
    auto width = (size_t)mx->gamemap->Size().cx;
    if ( costs[here.y * width + here.x] < 0 )
        return false;

    s32 best = -1;
    for ( int dir = DR_NORTH; dir <= DR_NORTHWEST; dir++ ) {
        auto there = here + (mxdir_t)dir;
        CONTINUE_IF( !mx->gamemap->IsLocOnMap(there) );
        auto cost = costs[there.y * width + there.x];
        CONTINUE_IF( cost < 0 );
        cost += (s32)StepCost(race, onfoot, there, (mxdir_t)dir);
        if ( best < 0 || cost < best ) {
            best = cost;
            step = there;
        }
    }
    return best >= 0;
}

bool citadel_x::Reachable ( const mxitem* walker, mxgridref from, mxgridref to ) const
{
    mxgridref step;
    return from == to || MarchStep(walker, from, to, step);
}

bool citadel_x::RegimentStep ( const mxregiment* regiment, mxgridref target, mxgridref& step ) const
{
    return MarchStep(regiment, regiment->Location(), target, step);
}

citadel_object* citadel_x::ArtefactAt ( mxgridref loc ) const
{
    FOR_EACH_OBJECT(object) {
        mxgridref where;
        if ( CitadelObject(object)->OnMap(where) && where == loc )
            return CitadelObject(object);
    }
    return nullptr;
}

mxobject* citadel_x::FindObjectAtLocation ( mxgridref loc )
{
    if ( auto artefact = ArtefactAt(loc) )
        return artefact;
    return mxscenario::FindObjectAtLocation(loc);
}

mxobject* citadel_x::PickupObject ( mxgridref loc )
{
    if ( auto artefact = ArtefactAt(loc) ) {
        artefact->Lift();
        return artefact;
    }
    return mxscenario::PickupObject(loc);
}

bool citadel_x::DropObject ( mxgridref loc, mxobject* object )
{
    if ( object != nullptr && CitadelObject(object)->IsArtefact() ) {
        object->Location(loc);
        return true;
    }
    return mxscenario::DropObject(loc, object);
}

bool citadel_x::IsFoughtOver ( mxstronghold* stronghold ) const
{
    if ( stronghold->Race() == RA_GOLDEN_FEY
        || KingdomOf(stronghold->Race()) == nullptr
        || !mx->gamemap->IsLocOnMap(stronghold->Location()) )
        return false;

    if ( boroth == nullptr || mx->objRegiments.Count() == 0 )
        return true;

    auto width = (size_t)mx->gamemap->Size().cx;
    auto where = stronghold->Location();
    return StepsFrom(boroth->Location(), mx->objRegiments[0])[where.y * width + where.x] >= 0;
}

mxrace_t citadel_x::CampaignTarget () const
{
    struct tally { u32 held = 0; u32 total = 0; };
    std::map<mxrace_t, tally> kingdoms;
    FOR_EACH_STRONGHOLD(stronghold) {
        CONTINUE_IF( !IsFoughtOver(stronghold) );
        auto& kingdom = kingdoms[stronghold->Race()];
        kingdom.total++;
        if ( stronghold->OccupyingRace() == RA_ENEMY )
            kingdom.held++;
    }

    auto isHis = [&kingdoms]( mxrace_t people ) {
        auto it = kingdoms.find(people);
        return it != kingdoms.end() && it->second.held == it->second.total;
    };

    mxrace_t target = RA_NONE;
    for ( const auto& [people, kingdom] : kingdoms ) {
        CONTINUE_IF( kingdom.held == kingdom.total );

        bool inReach = kingdom.held > 0;
        for ( const auto& other : citadel_kingdoms ) {
            if ( isHis(other.people) && Borders(other.people, people) )
                inReach = true;
        }
        CONTINUE_IF( !inReach );

        if ( target == RA_NONE ) {
            target = people;
            continue;
        }
        const auto& best = kingdoms.at(target);
        auto mine = (u64)kingdom.held * best.total;
        auto theirs = (u64)best.held * kingdom.total;
        if ( mine > theirs || (mine == theirs && StepsToMidnight(people) < StepsToMidnight(target)) )
            target = people;
    }
    return target;
}

mxstronghold* citadel_x::Front () const
{
    auto target = CampaignTarget();
    std::vector<mxstronghold*> holding;
    FOR_EACH_STRONGHOLD(stronghold) {
        if ( stronghold->Race() == target && stronghold->OccupyingRace() != RA_ENEMY && IsFoughtOver(stronghold) )
            holding.push_back(stronghold);
    }

    mxstronghold* front = nullptr;
    s32 best = 0;
    FOR_EACH_STRONGHOLD(stronghold) {
        CONTINUE_IF( stronghold->Race() == RA_ENEMY || stronghold->OccupyingRace() != RA_ENEMY
                     || !mx->gamemap->IsLocOnMap(stronghold->Location()) );
        for ( auto keep : holding ) {
            auto distance = stronghold->Location() - keep->Location();
            if ( front == nullptr || distance < best ) {
                front = stronghold;
                best = distance;
            }
        }
    }
    return front;
}

void citadel_x::NightStart ( void )
{
    mxscenario::NightStart();

    paths.clear();
    passable.clear();

    auto target = CampaignTarget();

    FOR_EACH_REGIMENT(regiment) {
        CONTINUE_IF( regiment->Race() != RA_ENEMY || regiment->Total() == 0 );

        mxstronghold* nearest = nullptr;
        if ( target != RA_NONE ) {
            FOR_EACH_STRONGHOLD(stronghold) {
                CONTINUE_IF( stronghold->Race() != target
                             || stronghold->OccupyingRace() == RA_ENEMY
                             || !IsFoughtOver(stronghold) );
                if ( nearest == nullptr
                     || regiment->Location() - stronghold->Location() < regiment->Location() - nearest->Location() )
                    nearest = stronghold;
            }
        }

        if ( nearest != nullptr ) {
            regiment->Orders(OD_GOTO);
            regiment->TargetId(mxentity::SafeIdt(nearest));
        } else {
            regiment->Orders(OD_WANDER);
        }
    }
}

void citadel_x::NightStop ( void )
{
    mxscenario::NightStop();

    FOR_EACH_STRONGHOLD(stronghold) {
        if ( mx->gamemap->IsLocOnMap(stronghold->Location()) )
            static_cast<citadel_stronghold*>(stronghold)->Gather();
    }
    Scout();
    ExecuteLuxor();
    RaiseWraith();

    FOR_EACH_CHARACTER(character) {
        auto lord = CitadelLord(character);
        CONTINUE_IF( !lord->IsRecruited() || lord->IsDead() || lord->news == QN_NONE );
        mx->SetLastActionMsg(mx->LastActionMsg() + mx->text->CookedSystemString(SS_QUEST_NEWS_REPORT, lord));
    }
}

void citadel_x::RaiseWraith ()
{
    if ( boroth == nullptr || maranor == nullptr || boroth->IsAlive() || flags.Is(gf_wraith) || maranor->HasFallen() )
        return;

    flags.Set(gf_wraith);
    boroth->Flags().Set(cf_alive);
    boroth->Location(maranor->Location());
    boroth->quest = QS_NONE;

    mx->SetLastActionMsg(mx->LastActionMsg() + mx->text->CookedSystemString(SS_WRAITH, boroth));
}

void citadel_x::ExecuteLuxor ()
{
    if ( boroth == nullptr || luxor == nullptr || boroth->IsAlive() || luxor->IsDead() || !luxor->IsPrisoner() )
        return;

    luxor->Cmd_Dead();
    mx->SetLastActionMsg(mx->LastActionMsg() + mx->text->CookedSystemString(SS_LUXOR_EXECUTED, luxor));
}

bool citadel_x::BorothFallen () const
{
    return boroth != nullptr && boroth->IsDead() && ( flags.Is(gf_wraith) || maranor == nullptr || maranor->HasFallen() );
}

void citadel_x::Scout ()
{
    mxgridref nowhere ( mx->gamemap->Size().cx, 0 );
    packs.resize(mx->objRegiments.Count(), nowhere);

    u32 index = 0;
    FOR_EACH_REGIMENT(regiment) {
        auto& pack = packs[index++];
        if ( mx->gamemap->IsLocOnMap(pack) && mx->gamemap->GetAt(pack).object == OB_WOLVES )
            mx->gamemap->GetAt(pack).object = OB_NONE;
        pack = nowhere;

        CONTINUE_IF( regiment->Race() != RA_ENEMY || regiment->Total() == 0 || regiment->Orders() != OD_GOTO
                     || !mx->gamemap->IsLocOnMap(regiment->Location()) );
        auto target = static_cast<mxitem*>(mx->EntityByIdt(regiment->TargetId()));
        CONTINUE_IF_NULL(target);

        auto ahead = regiment->Location();
        for ( u32 step = 0; step < SCOUTING && ahead != target->Location(); step++ ) {
            mxgridref next;
            if ( !MarchStep(regiment, ahead, target->Location(), next) )
                break;
            ahead = next;
        }

        auto& square = mx->gamemap->GetAt(ahead);
        CONTINUE_IF( ahead == regiment->Location() || square.object != OB_NONE || square.IsStronghold()
                     || square.HasCharacter() || square.HasArmy() );
        square.object = OB_WOLVES;
        pack = ahead;
    }
}

mxstronghold* citadel_x::HomeKeep ( mxrace_t people, mxgridref from ) const
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

mxstronghold* citadel_x::KeepOf ( const mxcharacter* owner ) const
{
    FOR_EACH_STRONGHOLD(stronghold) {
        if ( stronghold->Owner() == owner && mx->gamemap->IsLocOnMap(stronghold->Location()) )
            return stronghold;
    }
    return nullptr;
}

mxrace_t citadel_x::Allegiance ( const mxitem* figure ) const
{
    if ( figure->IsType(IDT_CHARACTER) )
        return static_cast<const mxcharacter*>(figure)->NormalisedLoyalty();
    return static_cast<const mxregiment*>(figure)->Race();
}

bool citadel_x::AtWar ( mxrace_t side ) const
{
    return (u32)side < wars.size() && wars[side] != 0;
}

bool citadel_x::AtWar ( mxrace_t side, mxrace_t with ) const
{
    return (u32)side < wars.size() && ( wars[side] >> with & 1 );
}

bool citadel_x::IsEnemyOf ( mxrace_t who, mxrace_t of ) const
{
    return who != of && AtWar(of, who);
}

void citadel_x::DeclareWar ( mxrace_t on, mxrace_t by )
{
    if ( on != by && (u32)on < wars.size() )
        wars[on] |= (u64)1 << by;
}

std::vector<citadel_character*> citadel_x::HostagesHeldAtMaranor () const
{
    std::vector<citadel_character*> held;
    FOR_EACH_CHARACTER(character) {
        if ( character->IsPrisoner() && character->IsAlive() && maranor != nullptr && character->Location() == maranor->Location() )
            held.push_back(CitadelLord(character));
    }
    return held;
}

void citadel_x::LordsTurn ( void )
{
    std::unique_ptr<citadel_quest_processor> processor ( new citadel_quest_processor() );
    for ( auto players : { true, false } ) {
        FOR_EACH_CHARACTER(character) {
            auto lord = CitadelLord(character);
            CONTINUE_IF( lord->IsRecruited() != players );
            processor->Process(lord);
        }
    }
}

COMMAND( OnCharQuest )
{
    CONVERT_CHARACTER_ID( argv[0].vId, character );
    auto ok = CitadelLord(character)->SetQuest((mxquest_t)argv[1].vSInt32, argv[2].vId);
    argv[0] = (s32)0;
    return ok ? MX_OK : MX_FAILED;
}

COMMAND( OnCharQuestInfo )
{
    CONVERT_CHARACTER_ID( argv[0].vId, character );
    auto lord = CitadelLord(character);
    argv[0] = (s32)lord->quest;
    argv[1] = lord->questtarget;
    argv[2] = (s32)lord->news;
    argv[3] = (s32)lord->CanQuest();
    return MX_OK;
}

COMMAND( OnCharQuestTargets )
{
    auto& targets = *static_cast<c_mxid*>(argv[0].vPtr);
    CONVERT_CHARACTER_ID( argv[1].vId, character );
    auto lord = CitadelLord(character);
    lord->QuestTargets((mxquest_t)argv[2].vSInt32, targets);
    argv[0] = (s32)targets.Count();
    return MX_OK;
}

COMMAND( OnQuestNews )
{
    auto& lords = *static_cast<c_mxid*>(argv[0].vPtr);
    lords.Clear();
    FOR_EACH_CHARACTER(character) {
        auto lord = CitadelLord(character);
        if ( lord->IsRecruited() && lord->IsAlive() && lord->news != QN_NONE )
            lords.Add(mxentity::SafeIdt(lord));
    }
    argv[0] = (s32)lords.Count();
    return MX_OK;
}

static mxcommand_t citadel_commands[] = {
    { "QUEST",          3,  OnCharQuest,        { arguments::character, variant::vnumber, variant::vid } },
    { "QUESTINFO",      1,  OnCharQuestInfo,    { arguments::character } },
    { "QUESTTARGETS",   3,  OnCharQuestTargets, { variant::vptr, arguments::character, variant::vnumber } },
    { "QUESTNEWS",      1,  OnQuestNews,        { variant::vptr } },
};

MXRESULT citadel_x::Command ( const std::string& arg, variant argv[], u32 argc )
{
    auto result = mx->ProcessCommand(citadel_commands, NUMELE(citadel_commands), arg, argv, argc);
    return result != MX_UNKNOWN ? result : mxscenario::Command(arg, argv, argc);
}

citadel_object::citadel_object()
    : type(OT_NONE), power(OP_NONE)
{
}

void citadel_object::Serialize ( archive& ar )
{
    mxobject::Serialize ( ar );

    if ( ar.IsStoring() ) {
        WRITE_ENUM(type);
        WRITE_ENUM(power);
    } else {
        READ_ENUM(type);
        READ_ENUM(power);
    }
}

void citadel_object::LoadTsv ( const TsvRow& row )
{
    mxobject::LoadTsv(row);

    type = row.GetObjectType(TsvField::Object::Type);
    power = row.GetObjectPower(TsvField::Object::Power);
}

bool citadel_object::IsArtefact () const
{
    return power != OP_NONE;
}

bool citadel_object::OnMap ( mxgridref& where ) const
{
    if ( !IsArtefact() || mx->scenario->WhoHasObject(const_cast<citadel_object*>(this)) != nullptr )
        return false;
    if ( !mx->gamemap->IsLocOnMap(Location()) )
        return false;
    where = Location();
    return true;
}

void citadel_object::Lift ()
{
    if ( IsArtefact() )
        Location(mxgridref(mx->gamemap->Size().cx, 0));
}

void citadel_routebranch::SetRoute ( mxdir_t dir, u32 w )
{
    direction = dir;
    weight = w;
}

void citadel_routebranch::Serialize ( archive& ar )
{
    mxroutebranch::Serialize ( ar );

    if ( ar.IsStoring() ) {
        WRITE_ENUM(direction);
        ar << weight;
    } else {
        READ_ENUM(direction);
        ar >> weight;
    }
}

mxroutebranch* citadel_entityfactory::CreateRouteBranch ()
{
    return new citadel_routebranch;
}

mxentity* citadel_entityfactory::Create ( id_type_t type )
{
    switch ( type ) {
        case IDT_CHARACTER:
            return new citadel_character;
        case IDT_STRONGHOLD:
            return new citadel_stronghold;
        case IDT_OBJECT:
            return new citadel_object;
        case IDT_AREAINFO:
            return new citadel_area;
        case IDT_RACEINFO:
            return new citadel_race;
        // the base factory knows neither, and the two info tables are created by count
        case IDT_OBJECT_POWER:
            return new mxobjectpower;
        case IDT_OBJECT_TYPE:
            return new mxobjecttype;
        default:
            break;
    }

    return mxentityfactory::Create ( type );
}

//
// Run once, when a story begins - never on load. Being held is a character flag, and
// character flags are saved, so a hostage freed on day forty is still free when that
// story is picked up again.
//
void citadel_x::initialiseAfterCreate ( u32 version )
{
    flags.Clear();
    packs.clear();

    for ( auto symbol : citadel_hostages ) {
        auto hostage = mx->CharacterBySymbol(symbol);
        if ( hostage != nullptr )
            hostage->Flags().Set(cf_prisoner);
    }

    wars.assign(mx->objRaceInfos.Count(), 0);
    for ( u32 race = RA_NONE + 1; race < wars.size(); race++ ) {
        DeclareWar((mxrace_t)race, RA_ENEMY);
        DeclareWar(RA_ENEMY, (mxrace_t)race);
    }

    FOR_EACH_CHARACTER(character) {
        auto lord = CitadelLord(character);
        if ( maranor != nullptr && lord->IsRecruited() && !lord->IsPrisoner() && lord->Location() == maranor->Location() )
            lord->Flags().Set(cf_dungeon);
        if ( lord->home == nullptr )
            lord->home = KeepOf(lord);
        if ( lord->home == nullptr )
            lord->home = HomeKeep(lord->Race(), lord->Location());
        if ( character->HasQuality(qf_mightywarrior) )
            character->strength = 100;
        else if ( character->HasQuality(qf_feeblewarrior) )
            character->strength = 25;
        else if ( character->Qualities() != qf_none )
            character->strength = 50;
    }

    mxscenario::initialiseAfterCreate(version);
}

void citadel_x::Serialize ( archive& ar )
{
    mxscenario::Serialize(ar);

    if ( ar.IsStoring() ) {
        ar << flags ;
        ar << (u32)wars.size() ;
        for ( auto war : wars )
            ar << war ;
        ar << (u32)packs.size() ;
        for ( auto& pack : packs )
            ar << pack ;
    } else {
        ar >> flags ;
        u32 count;
        ar >> count ;
        wars.resize(count);
        for ( auto& war : wars )
            ar >> war ;
        ar >> count ;
        packs.resize(count);
        for ( auto& pack : packs )
            ar >> pack ;
    }
}

MXRESULT citadel_x::Register ( mxengine* midnightx )
{
    // mx = midnightx ;
    // add in the interfaces
    mx->text = new mxtext;
    mx->night = new mxnight;
    mx->battle = new citadel_battle;
    mx->gameover = new lom_gameover;
    mx->entityfactory = new citadel_entityfactory;
    mx->scenario = (mxscenario*)citadel_scenario;
    
    // set initial feature flags
    mx->scenario->features = SF_MOONRING|SF_ICEFEAR|SF_HOSTAGES ;
    
    return MX_OK ;
}

MXRESULT citadel_x::UnRegister ( mxengine* midnightx )
{
    SAFEDELETE ( mx->text ) ;
    SAFEDELETE ( mx->night ) ;
    SAFEDELETE ( mx->battle ) ;
    SAFEDELETE ( mx->gameover ) ;
    SAFEDELETE ( mx->entityfactory );
    
    // mx will delete the scenario, so just lose our
    // reference to it
    citadel_scenario = NULL ;
    return MX_OK ;
}

}
// TME

#endif
