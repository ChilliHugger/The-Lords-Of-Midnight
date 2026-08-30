/*
 * FILE:    citadel_scenario.cpp
 *
 * PROJECT: citadel
 *
 * AUTHOR:  Chris Wild
 *
 * Copyright 2017 Chilli Hugger. All rights reserved.
 *
 * PURPOSE: The Lords of Midnight III : The Citadel
 *
 * The Citadel is a Lords-of-Midnight-family scenario, so it reuses the LOM
 * battle resolution, stronghold model and the generic mission/victory game
 * over scan. What it does not share is the moon ring and the ice fear -
 * neither exists in the Citadel data - so those features stay off.
 *
 */

#include "../../baseinc/tme_internal.h"
#include "../../utils/savegamemapping.h"
#include "scenario_citadel.h"
#include "scenario_citadel_internal.h"
#include "../lom/scenario_lom_internal.h"
#include "../lom/lom_processor_battle.h"
#include "../lom/lom_gameover.h"
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
        "Copyright 1994 - 2017 Mike Singleton & Chris Wild"
    };

static citadel_x* citadel_scenario = nullptr ;


MXRESULT MXAPI citadel::Create ( tme::mxinterface* engine )
{
variant args;

    if ( engine == nullptr )
        return MX_FAILED ;

    citadel_scenario = new citadel_x ;

    if ( citadel_scenario == nullptr )
        return MX_FAILED ;

    args = citadel_scenario ;
    return engine->Command("@SETSCENARIO", &args, 1);

}

citadel::citadel()
{
}

citadel::~citadel()
{
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
    darkCitadel(nullptr),
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

MXRESULT citadel_x::Register ( mxengine* midnightx )
{
    // add in the interfaces
    mx->text = new mxtext;
    mx->night = new mxnight;
    mx->battle = new lom_battle;
    mx->gameover = new lom_gameover;
    mx->entityfactory = new lom_entityfactory;

    if ( citadel_scenario != nullptr ) {
        mx->scenario = citadel_scenario ;
        // No moon ring and no ice fear in the Citadel. Recruiting costs time
        // across a 256x256 map, and riders may dismount.
        mx->scenario->features = SF_RECRUIT_TIME|SF_DISMOUNT ;
    }

    return MX_OK ;
}

MXRESULT citadel_x::UnRegister ( mxengine* midnightx )
{
    SAFEDELETE ( mx->gameover ) ;
    SAFEDELETE ( mx->text ) ;
    SAFEDELETE ( mx->night ) ;
    SAFEDELETE ( mx->battle ) ;
    SAFEDELETE ( mx->entityfactory );

    // mx will delete the scenario, so just lose our
    // reference to it
    citadel_scenario = nullptr ;
    return MX_OK ;
}

void citadel_x::initialise ( u32 version )
{
    darkCitadel = static_cast<mxstronghold*>(
        mx->EntityByName( "ST_MARANOR", IDT_STRONGHOLD ));
    boroth = mx->CharacterBySymbol("CH_BOROTH");

    mxscenario::initialise(version);
}

void citadel_x::initialiseAfterCreate ( u32 version )
{
    mxscenario::initialiseAfterCreate(version);
}

}
// namespace tme

#endif // _CITADEL_
