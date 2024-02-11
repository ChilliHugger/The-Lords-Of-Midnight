//
//  battle_steps.cpp
//  midight
//
//  Created by Chris Wild on 24/10/2023.
//

#include "battle_steps.h"
#include "../../../Source/tme/baseinc/processor_battle.h"

static const string ch_leader = "CH_LUXOR";
static const string ch_follower = "CH_MORKIN";
static mxcharacter* leader = nullptr;
static mxcharacter* follower = nullptr;
static mxgridref previous_location;

void BattleSteps::GivenLeaderWithFollowers()
{
    TMEStep::LordHasFollowers(ch_leader, {ch_follower, "CH_CORLETH", "CH_RORTHRON"});

    leader = static_cast<mxcharacter*>(GetCharacter(ch_leader));
    follower = static_cast<mxcharacter*>(GetCharacter(ch_follower));
    
    previous_location = leader->Location();
}

//
// Leader
//
void BattleSteps::WhenLeaderLosesBattle()
{
    leader->LostBattle(true);
}

void BattleSteps::ThenTheLeaderShouldBeDisplaced()
{
    REQUIRE_FALSE( leader->Location() == previous_location );
}

void BattleSteps::ThenTheLeaderShouldNotBeDisplaced()
{
    REQUIRE( leader->Location() == previous_location );
}

void BattleSteps::ThenTheLeaderGroupShouldBeDispanded()
{
    REQUIRE_FALSE( leader->HasFollowers() );
}

void BattleSteps::ThenTheLeaderGroupShouldNotBeDispanded()
{
    REQUIRE( leader->HasFollowers() );
}


//
// Follower
//
void BattleSteps::WhenFollowerLosesBattle()
{
    follower->LostBattle(true);
}

void BattleSteps::ThenTheFollowerShouldBeDisplaced()
{
    REQUIRE_FALSE( follower->Location() == previous_location );
}

void BattleSteps::ThenTheFollowerShouldNotBeDisplaced()
{
    REQUIRE( follower->Location() == previous_location );
}


