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

namespace tme {

	namespace scenarios {

    static scenarioinfo_t	citadel_scenario_info = {
        18,
        100,
        "CITADEL Scenario",
        "Chris Wild",
        "rorthron@thelordsofmidnight.com",
        "http://www.thelordsofmidnight.com",
        "The Lords of Midnight 3 : The Citadel for the Midnight Engine",
        "Copyright 1994 - 2017 Mike Singleton & Chris Wild"
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
	
MXRESULT citadel::Command ( const c_str& arg, variant argv[], u32 argc )
	{
		return citadel_scenario->Command(arg, argv, argc);
	}
	
MXRESULT citadel::GetProperties ( const c_str& arg, variant argv[], u32 argc )
	{
		return citadel_scenario->GetProperties(arg, argv, argc);
	}
	
MXRESULT citadel::Text ( const c_str& command, variant* argv, u32 args )
	{
		return citadel_scenario->Text(command, argv, args);
	}
	

//
// citadel_X
// Internal Scenario Class Code
//
//

citadel_x::citadel_x()
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
	// mx = midnightx ;
	// add in the interfaces
	mx->text = new mxtext;
	mx->night = new mxnight;
	mx->battle = new mxbattle;
	mx->scenario = (mxscenario*)citadel_scenario;
	
	// set initial feature flags
	//mx->scenario->features = SF_MOONRING|SF_ICEFEAR  ;
	
	return MX_OK ;
}

MXRESULT citadel_x::UnRegister ( mxengine* midnightx )
{
	SAFEDELETE ( mx->text ) ;
	SAFEDELETE ( mx->night ) ;
	SAFEDELETE ( mx->battle ) ;
	
	// mx will delete the scenario, so just lose our
	// reference to it
	citadel_scenario = NULL ;
	return MX_OK ;
}

	
	
mxentity* citadel_x::CreateEntity ( id_type_t type )
{
//	switch ( type ) {
//		case IDT_STRONGHOLD:
//			return (mxentity*) new lom_stronghold ;
//	}

	return mxscenario::CreateEntity ( type );
}

} // SCENARIOS
    
}
// TME
