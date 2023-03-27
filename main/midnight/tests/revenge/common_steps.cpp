//
//  common_steps.cpp
//  revenge
//
//  Created by Chris Wild on 25/03/2023.
//
// "No, Morkin, Corleth was testing your spirit: it is better to know your
// comrade’s mettle before the real battle begins, is it not?"
//

#include "common_steps.h"

void DDRStep::GameWon()
{
    GetCharacter(DDRStep::ch_shareth)->Cmd_Dead();
    TMEStep::NightFalls();
}

void DDRStep::GameLost()
{
    GetCharacter(TMEStep::ch_luxor)->Cmd_Dead();
    TMEStep::NightFalls();
}
