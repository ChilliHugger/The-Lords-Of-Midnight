//
//  common_steps.hpp
//  revenge
//
//  Created by Chris Wild on 25/03/2023.
//

#pragma once

#include "../tme_steps.h"

class DDRStep
{
public:
    static void GameWon();
    static void GameLost();
    
    static constexpr LPCSTR ch_shareth = "CH_SHARETH";
};
