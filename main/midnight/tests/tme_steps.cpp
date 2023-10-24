//
//  tme_steps.cpp
//  midnight
//
//  Created by Chris Wild on 04/01/2021.
//

#include "tme_steps.h"


void TMEStep::NewStory()
{
    TME_DeInit();
    TME_Init(RF_DEFAULT, DF_NORMAL);
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

void TMEStep::SetCurrentLord(LPCSTR name)
{
    auto lord = GetCharacter(name);
    TME_CurrentCharacter(lord->SafeIdt(lord));
}


void TMEStep::GameWon()
{
#if defined(_LOM_)
    LOMStep::GameWon();
#endif
#if defined(_DDR_)
    DDRStep::GameWon();
#endif
}

void TMEStep::GameLost()
{
#if defined(_LOM_)
    LOMStep::GameLost();
#endif
#if defined(_DDR_)
    DDRStep::GameLost();
#endif
}

void TMEStep::LordCarryingObject(LPCSTR name, mxthing_t thing)
{
    auto lord = GetCharacter(name);
    lord->carrying = tme::mx->ObjectById(thing);
 }

void TMEStep::LordCarryingObject(LPCSTR name, LPCSTR object)
{
    auto lord = GetCharacter(name);
    lord->carrying = GetObject(object);
 }

void TMEStep::LordAtLocation(LPCSTR name, LPCSTR location)
{
    auto lord = GetCharacter(name);
    lord->Location( GetEntity<mxitem>(location)->Location() );
}

void TMEStep::LordsAtSameLocation(LPCSTR name1, LPCSTR name2)
{
    auto lord1 = GetCharacter(name1);
    auto lord2 = GetCharacter(name2);

    lord1->Location( lord2->Location() );
}


void TMEStep::LordAtLocation(LPCSTR name, loc_t location)
{
    auto lord = GetCharacter(name);
    lord->Location( location );
}

void TMEStep::LordIsNotRecruited(LPCSTR name)
{
    auto lord = GetCharacter(name);
    lord->Flags().Reset(cf_recruited);
    lord->Flags().Set(cf_ai);
    lord->liege = nullptr ;
}

void TMEStep::LordIsRecruited(LPCSTR name)
{
    auto lord = GetCharacter(name);
    lord->Flags().Set(cf_recruited);
    lord->Flags().Reset(cf_ai);
    lord->liege = GetCharacter(ch_luxor);
}

void TMEStep::LordPerformsApproach(LPCSTR name1, LPCSTR name2)
{
    auto lord = GetCharacter(name1);
    lord->Cmd_Approach(GetCharacter(name2));
}
                
bool TMEStep::LordHasBeenRecruited(LPCSTR name)
{
    auto lord = GetCharacter(name);
    return lord->Flags().Is(cf_recruited);
}

void TMEStep::LordsIsLookingAt(LPCSTR name1, LPCSTR name2)
{
    auto lord1 = GetCharacter(name1);
    auto lord2 = GetCharacter(name2);

    auto loc = lord2->Location();
    loc.AddDirection(DR_SOUTH);

    lord1->Location(loc);
    lord1->looking = DR_NORTH;
}

void TMEStep::LordMovesForward(LPCSTR name)
{
    auto lord1 = GetCharacter(name);
    lord1->Cmd_MoveForward();
}

void TMEStep::LordIsDead(LPCSTR name)
{
    GetCharacter(name)->Cmd_Dead();
}

void TMEStep::LordKilledByObject(LPCSTR name, LPCSTR killedby)
{
    auto lord = GetCharacter(name);
    lord->Cmd_Dead();
    lord->killedbyobject = GetObject(killedby);
}

