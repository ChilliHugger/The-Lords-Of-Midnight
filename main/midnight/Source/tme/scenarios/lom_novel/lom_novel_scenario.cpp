/*
 * FILE:    lom_scenario.cpp
 * 
 * PROJECT: lom
 * 
 * CREATED: 
 * 
 * AUTHOR:  Chris Wild
 * 
 * Copyright 2011 Chilli Hugger. All rights reserved.
 * 
 * PURPOSE: 
 * 
 * 
 */

#include "../../baseinc/tme_internal.h"
#include "scenario_lom_novel.h"
#include "scenario_lom_novel_internal.h"
#include <string>

#if defined(_LOM_)

namespace tme {

static scenarioinfo_t    lom_scenario_info = {
    mxscenarioid::LOM_NOVEL,
    100,
    "LOM Novel Scenario",
    "Chris Wild",
    "rorthron@thelordsofmidnight.com",
    "http://www.thelordsofmidnight.com",
    "The Lords of Midnight for the Midnight Engine",
    "Copyright 1984 - 2025 Mike Singleton & Chris Wild"
};

static lom_novel_x* lom_novel_scenario = nullptr ;

MXRESULT MXAPI lom_novel::Create ( tme::mxinterface* engine )
{
variant args;

    RETURN_IF_NULL(engine) MX_FAILED;

    lom_novel_scenario = new lom_novel_x ;

    RETURN_IF_NULL(lom_novel_scenario) MX_FAILED;

    args = lom_novel_scenario ;
    return engine->Command("@SETSCENARIO", &args, 1);

}
    

scenarioinfo_t* lom_novel::GetInfoBlock() const
{
    return lom_novel_scenario->GetInfoBlock();
}
    

//
// LOM_NOVEL_X
// Internal Scenario Class Code
//
//

lom_novel_x::lom_novel_x()
{
}

lom_novel_x::~lom_novel_x()
{
}

scenarioinfo_t* lom_novel_x::GetInfoBlock() const
{
    return &lom_scenario_info ;
}

MXRESULT lom_novel_x::Register ( mxengine* midnightx )
{
    if (lom_x::Register(midnightx) != MX_OK) {
        return MX_FAILED;
    }

    mx->scenario = lom_novel_scenario;

    // set initial feature flags
    mx->scenario->features = SF_MOONRING|SF_ICEFEAR|SF_TUNNELS ;
    
    return MX_OK ;
}

MXRESULT lom_novel_x::UnRegister ( mxengine* midnightx )
{
    // mx will delete the scenario, so just lose our
    // reference to it
    lom_novel_scenario = nullptr ;
    
    return lom_x::UnRegister( midnightx ) ;
}

void lom_novel_x::updateAfterLoad( u32 version )
{
    lom_x::updateAfterLoad(version);
    
    // fixup strings
    mx->text->ModifySystemString(SS_MESSAGE1, "{char:text:time} and {char:text:energy}. {char:text:fear}. {char:text:courage}. {char:text:obj}. {char:text:sees}" );
    mx->text->ModifySystemString(SS_MESSAGE3, "{char:loc:text} stands {char:longname}. {char:text:time} and {char:text:energy}. {char:text:fear}. {char:text:courage}. {char:text:obj}. {char:text:group} {char:text:sees}" );
    mx->text->ModifySystemString(SS_MESSAGE4, "{char:loc:text} stands {char:longname}. {char:text:energy}. {char:text:fear}. {char:text:courage}. {char:text:group} {char:text:sees}" );
    mx->text->ModifySystemString(SS_SEES_1, "{char:name} sees the {char:loc:obj:name}. ");
    mx->text->ModifySystemString(SS_SEES_2, "{char:name} sees an underground entrance. ");
    mx->text->ModifySystemString(SS_SEES_3, "{char:name} sees then {char:loc:obj:name} and an underground entrance. ");
    

}

} // namespace tme

#endif // _LOM_


