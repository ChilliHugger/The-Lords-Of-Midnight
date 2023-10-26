//
//  ui_steps.cpp
//  midnight
//
//  Created by Chris Wild on 04/01/2021.
//

#include "ui_steps.h"
#include "tme_steps.h"


void UIStep::PlayerTapsLook()
{
    moonring::mikesingleton()->look();
}

void UIStep::PlayerContinuesAStory()
{
    moonring::mikesingleton()->continueStory(-1);
}

bool UIStep::CurrentPanelIs(panelmode_t mode)
{
    auto mr = moonring::mikesingleton();
    return mr->panels->currentmode == mode;
}

bool UIStep::CurrentPanelObjectIs(mxid id)
{
    auto panel = static_cast<uipanelmock*>(moonring::mikesingleton()->panels->currentpanel);
    return panel->currentObject == id;
}

void UIStep::PlayerClosesTheCurrentStory()
{
    moonring::mikesingleton()->closeStory();
}

bool UIStep::PlayerAddsLordToAGroup(LPCSTR lord, LPCSTR leader)
{
    auto id = mxentity::SafeIdt( GetCharacter(lord) );
    auto leaderid = mxentity::SafeIdt( GetCharacter(leader) );
    return moonring::mikesingleton()->joinGroup(id, leaderid);
}
