//
//  common_steps.cpp
//  midnight
//
//  Created by Chris Wild on 03/01/2021.
//
// "No, Morkin, Corleth was testing your spirit: it is better to know your
// comrade’s mettle before the real battle begins, is it not?"
//

#include "common_steps.h"

void LOMStep::UshgarakHasFallenInBattle()
{
    tme::mx->battle->ResetBattlesFought();

    auto luxor = GetCharacter("CH_LUXOR");
    auto ushgarak = GetStronghold("SH_CITADEL_USHGARAK");

    // reset all armies to guaranty victory
    for ( u32 ii=0; ii<tme::mx->objRegiments.Count(); ii++ )
        ((mxregiment*)tme::mx->objRegiments[ii])->Total(0);
            
    luxor->warriors.Total(1200);
    luxor->riders.Total(1200);
    luxor->Location( ushgarak->Location() );

    // empty ushgarak
    ushgarak->TotalTroops(10);
        
    // battle
    tme::mx->battle->KickOffAtLocation(luxor->Location());
}

void LOMStep::XajorkithHasFallenInBattle()
{
    tme::mx->battle->ResetBattlesFought();

    auto sh_xajorkith = GetStronghold("SH_CITADEL_XAJORKITH");

    // Move a regiment to Xajorkith
    ((mxregiment*)tme::mx->objRegiments[0])->Location( sh_xajorkith->Location() );
    ((mxregiment*)tme::mx->objRegiments[0])->Total(1200);

    auto xajorkith = GetCharacter("CH_XAJORKITH");
    xajorkith->warriors.Total(0);
    xajorkith->riders.Total(0);
            
    // empty xajorkith
    sh_xajorkith->TotalTroops(10);
        
    // battle
    tme::mx->battle->KickOffAtLocation(xajorkith->Location());
}

void LOMStep::MorkinIsDead()
{
    GetCharacter("CH_MORKIN")->Cmd_Dead();
}

void LOMStep::LuxorIsDead()
{
    GetCharacter("CH_LUXOR")->Cmd_Dead();
}

void LOMStep::IceCrownHasBeenDestroyed()
{
    auto morkin = GetCharacter("CH_MORKIN");
    morkin->carrying = tme::mx->ObjectById(OB_ICECROWN);
    GetCharacter("CH_FARFLAME")->Location( morkin->Location() ) ;
}

void LOMStep::GameWon()
{
    IceCrownHasBeenDestroyed();
    TMEStep::NightFalls();
}

void LOMStep::GameLost()
{
    MorkinIsDead();
    LuxorIsDead();
    TMEStep::NightFalls();
}


