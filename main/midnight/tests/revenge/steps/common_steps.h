//
//  common_steps.hpp
//  revenge
//
//  Created by Chris Wild on 25/03/2023.
//

#pragma once

#include "../../steps/tme_steps.h"

using namespace std;

struct DDRStep
{
public:
    static void GameWon();
    static void GameLost();
    
    static void CharactersDawnBreaks();

    static void LordKilledByCharacter(LPCSTR name, LPCSTR killedby);
    static void MorkinRescued();
    static void LordAtGateOfVarenorn(LPCSTR name);
    static void LordNotAtGateOfVarenorn(LPCSTR name);
    
    static void LordTakesTurn(const string& name);

    static void LordsAtGateOfVarenorn(vector<string> names);
    static void LordsNotAtGateOfVarenorn(vector<string> names);

    static void ObjectAtLocation(const string& name, mxgridref loc);


    static constexpr LPCSTR ch_shareth = "CH_SHARETH";
    static constexpr LPCSTR ch_tarithel = "CH_TARITHEL";
};

#define NEW_TAG "[NEW]"
#define IGNORE_TAG "[ignore]"
