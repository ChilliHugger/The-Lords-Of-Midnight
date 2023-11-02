//
//  tme_steps.cpp
//  midnight
//
//  Created by Chris Wild on 04/01/2021.
//

#include "tme_steps.h"
#include "../mocks/mocks_entity.h"


void TMEStep::NewStory(mxdifficulty_t difficulty)
{
    TME_DeInit();
        
    //TME_Init(RF_DEFAULT, difficulty);
    
    TME_Init(RF_DEFAULT, difficulty, [] {
        tme::mx->entityfactory = new mockentityfactory();
    });

    
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

void TMEStep::SetCurrentLord(const string& name)
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

void TMEStep::LordCarryingObject(const string& name, mxthing_t thing)
{
    auto lord = GetCharacter(name);
    lord->carrying = tme::mx->ObjectById(thing);
}

void TMEStep::LordNotCarryingObject(const string& name)
{
    auto lord = GetCharacter(name);
    lord->carrying = nullptr;
}

void TMEStep::LordCarryingObject(const string& name, const string& object)
{
    auto lord = GetCharacter(name);
    lord->carrying = GetObject(object);
}

void TMEStep::LordAtLocation(const string& name, const string& location)
{
    auto lord = GetCharacter(name);
    lord->Location( GetEntity<mxitem>(location)->Location() );
}

void TMEStep::LordsAtSameLocation(const string& name1, const string& name2)
{
    auto lord1 = GetCharacter(name1);
    auto lord2 = GetCharacter(name2);

    lord1->Location( lord2->Location() );
}


void TMEStep::LordAtLocation(const string& name, loc_t location)
{
    auto lord = GetCharacter(name);
    lord->Location( location );
}

void TMEStep::LordIsNotRecruited(const string& name)
{
    auto lord = GetCharacter(name);
    lord->Flags().Reset(cf_recruited);
    lord->Flags().Set(cf_ai);
    lord->liege = nullptr ;
}

void TMEStep::LordIsRecruited(const string& name)
{
    auto lord = GetCharacter(name);
    lord->Flags().Set(cf_recruited);
    lord->Flags().Reset(cf_ai);
    lord->liege = GetCharacter(ch_luxor);
}

void TMEStep::LordPerformsApproach(const string& name1, const string& name2)
{
    auto lord = GetCharacter(name1);
    lord->Cmd_Approach(GetCharacter(name2));
}
                
bool TMEStep::LordHasBeenRecruited(const string& name)
{
    auto lord = GetCharacter(name);
    return lord->Flags().Is(cf_recruited);
}

void TMEStep::LordsIsLookingAt(const string& name1, const string& name2)
{
    auto lord1 = GetCharacter(name1);
    auto lord2 = GetCharacter(name2);

    auto loc = lord2->Location();
    loc.AddDirection(DR_SOUTH);

    lord1->Location(loc);
    lord1->looking = DR_NORTH;
}

void TMEStep::LordMovesForward(const string& name)
{
    auto lord1 = GetCharacter(name);
    lord1->Cmd_MoveForward();
}

void TMEStep::LordFights(const string& name)
{
    auto lord1 = GetCharacter(name);
    lord1->Cmd_Fight();
}

void TMEStep::LordIsDead(const string& name)
{
    GetCharacter(name)->Cmd_Dead();
}

void TMEStep::LordKilledByObject(const string& name, const string& killedby)
{
    auto lord = GetCharacter(name);
    lord->Cmd_Dead();
    lord->killedbyobject = GetObject(killedby);
}

void TMEStep::LordHasFollowers(const string& lord, vector<string> names)
{
    auto leader = GetCharacter(lord);
    for (string &name : names) {
        auto lord = GetCharacter(name.c_str());

        // same location
        lord->Location(leader->Location());

        // same loyalty
        lord->Recruited(leader);
        
        lord->Cmd_Follow(leader);
    }
}

void TMEStep::LordShouldDieInFight(const string& lord)
{
    auto mock = GetMockCharacter(lord);
    mock->mockData.properties["ShouldDieInFight"] = "true";
}

void TMEStep::LordShouldNotDieInFight(const string& lord)
{
    auto mock = GetMockCharacter(lord);
    mock->mockData.properties["ShouldDieInFight"] = "false";
}

void TMEStep::LordShouldLoseHorse(const string& lord)
{
    auto mock = GetMockCharacter(lord);
    mock->mockData.properties["ShouldLoseHorse"] = "true";
}

void TMEStep::LordShouldNotLoseHorse(const string& lord)
{
    auto mock = GetMockCharacter(lord);
    mock->mockData.properties["ShouldLoseHorse"] = "false";
}

