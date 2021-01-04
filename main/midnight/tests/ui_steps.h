//
//  ui_steps.hpp
//  midnight
//
//  Created by Chris Wild on 04/01/2021.
//
#pragma once
#include "mocks.h"

class UIStep
{
public:
    static void PlayerTapsLook();
    static bool CurrentPanelIs(panelmode_t mode);
    static void PlayerContinuesAStory();
    static void PlayerClosesTheCurrentStory();
    static bool CurrentPanelObjectIs(mxid id);

    static bool PlayerAddsLordToAGroup(LPCSTR lord, LPCSTR leader);

};

