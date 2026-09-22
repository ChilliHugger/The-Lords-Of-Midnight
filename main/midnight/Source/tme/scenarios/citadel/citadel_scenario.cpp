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
#include <string>

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
// Named by symbol rather than flagged in the database because the database shipped with
// The Citadel is still Lords of Midnight's. A name that is not in the database is passed
// over, so this roster costs nothing today and wakes on its own the day the Citadel
// characters land.
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

mxentity* citadel_entityfactory::Create ( id_type_t type )
{
    switch ( type ) {
        case IDT_CHARACTER:
            return new citadel_character;
        case IDT_STRONGHOLD:
            return new citadel_stronghold;
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
