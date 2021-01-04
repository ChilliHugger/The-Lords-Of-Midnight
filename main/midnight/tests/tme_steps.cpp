//
//  tme_steps.cpp
//  midnight
//
//  Created by Chris Wild on 04/01/2021.
//

#include "tme_steps.h"
#if defined(_LOM_)
    #include "midnight/common_steps.h"
#endif

void TMEStep::NewStory()
{
    TME_DeInit();
    TME_Init();
}

void TMEStep::NightFalls()
{
    TME_Night(nullptr);
}

void TMEStep::CurrentLordIsDead()
{
    tme::mx->CurrentChar()->Cmd_Dead();
    TME_RefreshCurrentCharacter();
}

void TMEStep::GameWon()
{
#if defined(_LOM_)
        LOMStep::GameWon();
#endif
}

void TMEStep::GameLost()
{
#if defined(_LOM_)
        LOMStep::GameLost();
#endif
}
