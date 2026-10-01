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
#include <algorithm>
#include <map>
#include <memory>
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
    boroth(nullptr)
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
    boroth = mx->CharacterBySymbol("CH_BOROTH");

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

const std::vector<s32>& citadel_x::StepsFrom ( mxgridref from, const mxregiment* walker ) const
{
    auto width = (size_t)mx->gamemap->Size().cx;
    auto height = (size_t)mx->gamemap->Size().cy;
    auto key = (u32)(from.y * width + from.x);

    auto found = paths.find(key);
    if ( found != paths.end() )
        return found->second;

    auto& steps = paths[key];
    steps.assign(width * height, -1);
    steps[key] = 0;
    std::vector<mxgridref> queue { from };
    for ( size_t next = 0; next < queue.size(); next++ ) {
        auto here = queue[next];
        for ( int dir = DR_NORTH; dir <= DR_NORTHWEST; dir++ ) {
            auto there = here + (mxdir_t)dir;
            CONTINUE_IF( !mx->gamemap->IsLocOnMap(there) );
            auto& count = steps[there.y * width + there.x];
            CONTINUE_IF( count >= 0 || isLocationImpassable(there, walker) );
            count = steps[here.y * width + here.x] + 1;
            queue.push_back(there);
        }
    }
    return steps;
}

bool citadel_x::MarchStep ( mxgridref here, mxgridref target, mxgridref& step ) const
{
    if ( mx->objRegiments.Count() == 0 )
        return false;

    const auto& steps = StepsFrom(target, mx->objRegiments[0]);
    auto width = (size_t)mx->gamemap->Size().cx;
    auto best = steps[here.y * width + here.x];
    if ( best < 0 )
        return false;

    bool found = false;
    for ( int dir = DR_NORTH; dir <= DR_NORTHWEST; dir++ ) {
        auto there = here + (mxdir_t)dir;
        CONTINUE_IF( !mx->gamemap->IsLocOnMap(there) );
        auto count = steps[there.y * width + there.x];
        if ( count >= 0 && count < best ) {
            best = count;
            step = there;
            found = true;
        }
    }
    return found;
}

bool citadel_x::RegimentStep ( const mxregiment* regiment, mxgridref target, mxgridref& step ) const
{
    return MarchStep(regiment->Location(), target, step);
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
        LiftObject(artefact);
        return artefact;
    }
    return mxscenario::PickupObject(loc);
}

bool citadel_x::DropObject ( mxgridref loc, mxobject* object )
{
    if ( IsArtefact(object) ) {
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

void citadel_x::NightStart ( void )
{
    mxscenario::NightStart();

    paths.clear();

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

    FOR_EACH_CHARACTER(character) {
        auto lord = CitadelLord(character);
        CONTINUE_IF( !lord->IsRecruited() || lord->IsDead() || lord->news == QN_NONE );
        std::string paragraph = ".{lf}{crlf}{cr} ";
        mx->SetLastActionMsg(mx->LastActionMsg() + mx->text->CookText(paragraph, lord) + lord->NewsText());
    }
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
    return MX_OK;
}

COMMAND( OnCharQuestTargets )
{
    auto& targets = *static_cast<c_mxid*>(argv[0].vPtr);
    CONVERT_CHARACTER_ID( argv[1].vId, character );
    auto lord = CitadelLord(character);
    auto quest = (mxquest_t)argv[2].vSInt32;

    targets.Clear();
    auto offer = [&]( mxentity* entity ) {
        auto id = mxentity::SafeIdt(entity);
        if ( lord->CanQuest(quest, id) )
            targets.Add(id);
    };
    FOR_EACH_CHARACTER(other) {
        offer(other);
    }
    FOR_EACH_STRONGHOLD(stronghold) {
        if ( mx->gamemap->IsLocOnMap(stronghold->Location()) )
            offer(stronghold);
    }
    FOR_EACH_OBJECT(object) {
        offer(object);
    }
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

static std::string citadel_text_buffer;

static MXRESULT ReturnText ( variant argv[], const std::string& text )
{
    citadel_text_buffer = text;
    argv[0] = (s32)1;
    argv[1].vString = (LPSTR)citadel_text_buffer.c_str();
    return MX_OK;
}

COMMAND( OnTextCharQuest )
{
    CONVERT_CHARACTER_ID( argv[0].vId, character );
    return ReturnText(argv, CitadelLord(character)->QuestText());
}

COMMAND( OnTextCharQuestNews )
{
    CONVERT_CHARACTER_ID( argv[0].vId, character );
    return ReturnText(argv, CitadelLord(character)->NewsText());
}

static mxcommand_t citadel_text[] = {
    { "CharQuest",      1,  OnTextCharQuest,        { arguments::character } },
    { "CharQuestNews",  1,  OnTextCharQuestNews,    { arguments::character } },
};

MXRESULT citadel_x::Text ( const std::string& arg, variant argv[], u32 argc )
{
    auto result = mx->ProcessCommand(citadel_text, NUMELE(citadel_text), arg, argv, argc);
    return result != MX_UNKNOWN ? result : mxscenario::Text(arg, argv, argc);
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
        if ( tme::mx->SaveGameVersion() > 17 ) {
            READ_ENUM(type);
            READ_ENUM(power);
        }
    }
}

void citadel_object::LoadTsv ( const TsvRow& row )
{
    mxobject::LoadTsv(row);

    type = row.GetObjectType(TsvField::Object::Type);
    power = row.GetObjectPower(TsvField::Object::Power);
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
    for ( auto symbol : citadel_hostages ) {
        auto hostage = mx->CharacterBySymbol(symbol);
        if ( hostage != nullptr )
            hostage->Flags().Set(cf_prisoner);
    }

    FOR_EACH_CHARACTER(character) {
        if ( character->HasQuality(qf_mightywarrior) )
            character->strength = 100;
        else if ( character->HasQuality(qf_feeblewarrior) )
            character->strength = 25;
        else if ( character->Qualities() != qf_none )
            character->strength = 50;
    }

    // said of a lord still in the dungeons, in place of "has not yet been persuaded to
    // join you", which would be a poor way to describe a prisoner
    mx->text->ModifySystemString(SS_PRISONER,
        "{case:first}{char:name} is held hostage here in the dungeons of the Dark Citadel, "
        "and while {gender:heshe} is held the {race:name} will not march.");

    mxscenario::initialiseAfterCreate(version);
}

MXRESULT citadel_x::Register ( mxengine* midnightx )
{
    // mx = midnightx ;
    // add in the interfaces
    mx->text = new mxtext;
    mx->night = new mxnight;
    mx->battle = new citadel_battle;
    mx->gameover = new mxgameover;
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
