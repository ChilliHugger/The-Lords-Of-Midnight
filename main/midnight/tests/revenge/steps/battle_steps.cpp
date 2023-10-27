//
//  battle_steps.cpp
//  revenge
//
//  Created by Chris Wild on 24/10/2023.
//

#include "battle_steps.h"
#include "../../../src/tme/scenarios/ddr/ddr_processor_battle.h"

static const LPCSTR ch_leader = "CH_CARORMAND";
static const LPCSTR ch_follower = "CH_GLORTHARG";
static const LPCSTR ch_shareth = "CH_SHARETH";
static ddr_character* leader = nullptr;
static ddr_character* follower = nullptr;
static ddr_character* shareth = nullptr;
static mxgridref previous_location;

void BattleSteps::GivenLeaderWithFollowers()
{
    TMEStep::LordHasFollowers(ch_leader, {ch_follower, "CH_THORTHEON", "CH_LORANGRORN"});

    leader = static_cast<ddr_character*>(GetCharacter(ch_leader));
    follower = static_cast<ddr_character*>(GetCharacter(ch_follower));
    shareth = static_cast<ddr_character*>(GetCharacter(ch_shareth));
    

    previous_location = leader->Location();
}

//
// Leader
//
void BattleSteps::WhenLeaderLosesTheirArmy()
{
    auto battle = new ddr_battle();
    battle->CharacterLosesArmy(leader);
}

void BattleSteps::WhenLeaderKilledByLord()
{
    auto battle = new ddr_battle();
    battle->CharacterKilledByCharacter(leader, shareth);
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

void BattleSteps::ThenTheLeaderShouldBeAvailableForBattle()
{
    REQUIRE_FALSE( leader->IsBattleOver() );
}

void BattleSteps::ThenTheLeaderShouldNotBeAvailableForBattle()
{
    REQUIRE( leader->IsBattleOver() );
}

//
// Follower
//
void BattleSteps::WhenFollowerLosesTheirArmy()
{
    //follower->LostBattle(true);
    auto battle = new ddr_battle();
    battle->CharacterLosesArmy(follower);
}

void BattleSteps::WhenFollowerKilledByLord()
{
    auto battle = new ddr_battle();
    battle->CharacterKilledByCharacter(follower, shareth);
}

void BattleSteps::ThenTheFollowerShouldBeDisplaced()
{
    REQUIRE_FALSE( follower->Location() == previous_location );
}

void BattleSteps::ThenTheFollowerShouldNotBeDisplaced()
{
    REQUIRE( follower->Location() == previous_location );
}

void BattleSteps::ThenTheFollowerShouldBeAvailableForBattle()
{
    REQUIRE_FALSE( follower->IsBattleOver() );
}

void BattleSteps::ThenTheFollowerShouldNotBeAvailableForBattle()
{
    REQUIRE( follower->IsBattleOver() );
}


void BattleSteps::LordFleesFromBattle()
{
    leader = static_cast<ddr_character*>(GetCharacter(ch_leader));
    shareth = static_cast<ddr_character*>(GetCharacter(ch_shareth));

    // will flee
    leader->LostBattle(true);

    // but the enemy moves in
    shareth->Location(leader->Location());
}

void BattleSteps::LordBattleIsOver()
{
    leader = static_cast<ddr_character*>(GetCharacter(ch_leader));
    shareth = static_cast<ddr_character*>(GetCharacter(ch_shareth));

    // same location ready for battle
    leader->Location(shareth->Location());

    leader->Flags().Set(cf_battleover);
}

void BattleSteps::LordBattleIsNotOver()
{
    leader = static_cast<ddr_character*>(GetCharacter(ch_leader));
    shareth = static_cast<ddr_character*>(GetCharacter(ch_shareth));

    // same location ready for battle
    leader->Location(shareth->Location());
    leader->Flags().Reset(cf_battleover);
}

void BattleSteps::LordShouldNotBeSelectedForBattle()
{
    c_character characters;

    auto battle = new ddr_battle();
    auto enemies = battle->GetCharactersHereForBattle(characters, shareth);

    REQUIRE_FALSE( characters.IsInList(leader) );
    REQUIRE( enemies == 0 );
    
    auto defender = battle->SelectDefender(characters,shareth->Loyalty());
    REQUIRE( defender == nullptr );
}

void BattleSteps::LordShouldBeSelectedForBattle()
{
    c_character characters;

    auto battle = new ddr_battle();
    auto enemies = battle->GetCharactersHereForBattle(characters, shareth);

    REQUIRE( characters.IsInList(leader) );
    REQUIRE( enemies == 1 );

    auto defender = battle->SelectDefender(characters,shareth->Loyalty());
    REQUIRE_FALSE( defender == nullptr );

}
