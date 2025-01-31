//
//  battle_tests.cpp
//  revenge
//
//  Created by Chris Wild on 02/09/2023.
//

#include "../steps/battle_steps.h"
#include "../steps/common_steps.h"
#include "../../../Source/tme/scenarios/ddr/ddr_processor_battle.h"

// GIVEN a hard game
// AND a lord has followers
// WHEN the lord loses their army
// THEN the leader should be displaced
// AND the group should be dispanded
// AND the leader should still be available for battle

SCENARIO("In hard mode when leader loses their army")
{
    GIVEN("A hard game")
    {
        TMEStep::GivenHardGame();
        
        AND_GIVEN("that the lord has followers")
        {
            BattleSteps::GivenLeaderWithFollowers();

            WHEN("the lord loses their army")
            {
                BattleSteps::WhenLeaderLosesTheirArmy();
                
                THEN("the lord should be displaced")
                {
                    BattleSteps::ThenTheLeaderShouldBeDisplaced();
                
                    AND_THEN("the group should be dispanded")
                    {
                        BattleSteps::ThenTheLeaderGroupShouldBeDispanded();
                        
                        AND_THEN("the lord should be available for battle")
                        {
                            BattleSteps::ThenTheLeaderShouldBeAvailableForBattle();
                        }
                    }
                }
            }
        }
    }
}

// GIVEN a normal game
// WHEN the lord loses their army
// THEN the leader should not be displaced
// AND the group should not be dispanded
// AND the leader should no longer be available for battle
SCENARIO("In normal mode when leader loses their army")
{
    GIVEN("A normal game")
    {
        TMEStep::GivenNormalGame();
        
        AND_GIVEN("that lord has followers")
        {
            BattleSteps::GivenLeaderWithFollowers();

            WHEN("the lord loses their army")
            {
                BattleSteps::WhenLeaderLosesTheirArmy();
                
                THEN("the lord should not be displaced")
                {
                    BattleSteps::ThenTheLeaderShouldNotBeDisplaced();
                
                    AND_THEN("the group should not be dispanded")
                    {
                        BattleSteps::ThenTheLeaderGroupShouldNotBeDispanded();
                        
                        AND_THEN("the lord should not be available for battle")
                        {
                            BattleSteps::ThenTheLeaderShouldNotBeAvailableForBattle();
                        }
                    }
                }
            }
        }
    }
}

// GIVEN a hard game
// WHEN a follower loses their army
// THEN the follower should be displaced
// AND the group should not be dispanded
// AND the follower should still be available for battle

SCENARIO("In hard mode when follower loses their army")
{
    GIVEN("A hard game")
    {
        TMEStep::GivenHardGame();
        
        AND_GIVEN("that the lord is a follower")
        {
            BattleSteps::GivenLeaderWithFollowers();

            WHEN("the follower loses their army")
            {
                BattleSteps::WhenFollowerLosesTheirArmy();
                
                THEN("the lord should be displaced")
                {
                    BattleSteps::ThenTheFollowerShouldBeDisplaced();
                
                    AND_THEN("the group should not be dispanded")
                    {
                        BattleSteps::ThenTheLeaderGroupShouldNotBeDispanded();
                        
                        AND_THEN("the lord should be available for battle")
                        {
                            BattleSteps::ThenTheFollowerShouldBeAvailableForBattle();
                        }
                    }
                }
            }
        }
    }
}

// GIVEN a normal game
// WHEN the lord loses their army
// THEN the follower should not be displaced
// AND the group should not be dispanded
// AND the follower should no longer be available for battle
SCENARIO("In normal mode when follower loses their army")
{
    GIVEN("A normal game")
    {
        TMEStep::GivenNormalGame();
        
        AND_GIVEN("that the lord is a follower")
        {
            BattleSteps::GivenLeaderWithFollowers();

            WHEN("the lord loses their army")
            {
                BattleSteps::WhenFollowerLosesTheirArmy();
                
                THEN("the lord should not be displaced")
                {
                    BattleSteps::ThenTheFollowerShouldNotBeDisplaced();
                
                    AND_THEN("the group should not be dispanded")
                    {
                        BattleSteps::ThenTheLeaderGroupShouldNotBeDispanded();
                        
                        AND_THEN("the lord should not be available for battle")
                        {
                            BattleSteps::ThenTheFollowerShouldNotBeAvailableForBattle();
                        }
                    }
                }
            }
        }
    }
}


// GIVEN a character's battle is over
// THEN the character should not be included in any more battles
SCENARIO("A character whose battle is over should not be selected for battle")
{
    TMEStep::NewStory();
    
    GIVEN("A characters battle is over")
    {
        BattleSteps::LordBattleIsOver();
        
        THEN("they should not be selected for battle")
        {
            BattleSteps::LordShouldNotBeSelectedForBattle();
        }
    }
}

// GIVEN a character's battle is not over
// THEN the character should be included in any more battles
SCENARIO("A character whose battle is not over should be selected for battle")
{
    TMEStep::NewStory();
    
    GIVEN("A characters battle is not over")
    {
        BattleSteps::LordBattleIsNotOver();
        
        THEN("they should be selected for battle")
        {
            BattleSteps::LordShouldBeSelectedForBattle();
        }
    }
}

// GIVEN a character flees from a battle
// THEN the character should be available for any more battles
SCENARIO("A character flees from a battle should be selected for another battle")
{
    TMEStep::NewStory();
    
    GIVEN("A character flees from battle")
    {
        BattleSteps::LordFleesFromBattle();
        
        THEN("they should be selected for battle")
        {
            BattleSteps::LordShouldBeSelectedForBattle();
        }
    }
}

// GIVEN a hard game
// AND a lord has followers
// WHEN they are killed by another lord
// THEN they should not be displaced
// AND the group should be dispanded

SCENARIO("In hard mode when leader loses to another lord")
{
    GIVEN("A hard game")
    {
        TMEStep::GivenHardGame();
        
        AND_GIVEN("that the lord has followers")
        {
            BattleSteps::GivenLeaderWithFollowers();

            WHEN("they are killed by another lord")
            {
                BattleSteps::WhenLeaderKilledByLord();
                
                THEN("they should not be displaced")
                {
                    BattleSteps::ThenTheLeaderShouldNotBeDisplaced();
                
                    AND_THEN("the group should be dispanded")
                    {
                        BattleSteps::ThenTheLeaderGroupShouldBeDispanded();
                    }
                }
            }
        }
    }
}

// GIVEN a normal game
// AND a lord has followers
// WHEN they are killed by another lord
// THEN the leader should not be displaced
// AND the group should not be dispanded
SCENARIO("In normal mode when leader loses to another lord")
{
    GIVEN("A normal game")
    {
        TMEStep::GivenNormalGame();
        
        AND_GIVEN("that lord has followers")
        {
            BattleSteps::GivenLeaderWithFollowers();

            WHEN("they are killed by another lord")
            {
                BattleSteps::WhenFollowerKilledByLord();
                
                THEN("the lord should not be displaced")
                {
                    BattleSteps::ThenTheLeaderShouldNotBeDisplaced();
                
                    AND_THEN("the group should not be dispanded")
                    {
                        BattleSteps::ThenTheLeaderGroupShouldNotBeDispanded();
                    }
                }
            }
        }
    }
}


// GIVEN a hard game
// AND that the lord is a follower
// WHEN they are killed by another lord
// THEN they should not be displaced
// AND the group should not be dispanded

SCENARIO("In hard mode when follower loses to another lord")
{
    GIVEN("A hard game")
    {
        TMEStep::GivenHardGame();
        
        AND_GIVEN("that the lord is a follower")
        {
            BattleSteps::GivenLeaderWithFollowers();

            WHEN("they are killed by another lord")
            {
                BattleSteps::WhenFollowerKilledByLord();
                
                THEN("they should not be displaced")
                {
                    BattleSteps::ThenTheFollowerShouldNotBeDisplaced();
                
                    AND_THEN("the group should not be dispanded")
                    {
                        BattleSteps::ThenTheLeaderGroupShouldNotBeDispanded();
                    }
                }
            }
        }
    }
}

// GIVEN a normal game
// AND that the lord is a follower
// WHEN they are killed by another lord
// THEN they should not be displaced
// AND the group should not be dispanded
// AND the follower should no longer be available for battle
SCENARIO("In normal mode when follower loses to another lord")
{
    GIVEN("A normal game")
    {
        TMEStep::GivenNormalGame();
        
        AND_GIVEN("that the lord is a follower")
        {
            BattleSteps::GivenLeaderWithFollowers();

            WHEN("they are killed by another lord")
            {
                BattleSteps::WhenFollowerKilledByLord();
                
                THEN("they should not be displaced")
                {
                    BattleSteps::ThenTheFollowerShouldNotBeDisplaced();
                
                    AND_THEN("the group should not be dispanded")
                    {
                        BattleSteps::ThenTheLeaderGroupShouldNotBeDispanded();
                    }
                }
            }
        }
    }
}

// GIVEN a lord is near enemies
// WHEN the lord enters battle
// THEN the lords should be preparing for battle

// GIVEN a lord is near enemies
// WHEN a lord fails an approach
// THEN the lords should be preparing for battle

// GIVEN a lord is at the same location as an enemy
// WHEN dawn breaks
// THEN the lords should be preparing for battle

// GIVEN a lord is preparing for battle
// WHEN the lord moves
// THEN they should no longer be preparing for battle

SCENARIO("Lord prepares to do battle")
{
    auto lord = TMEStep::ch_luxor;
    auto enemy = DDRStep::ch_shareth;
    auto enemyStronghold = "SH_CITY_IMIRIEL";
    auto location = mxgridref(10,10);
    
    auto LordPreparesForBattle = [lord]() {
        REQUIRE( GetCharacter(lord)->Flags().Is(cf_preparesbattle) );
    };

    auto LordNotPreparesForBattle = [lord]() {
        REQUIRE( !GetCharacter(lord)->Flags().Is(cf_preparesbattle) );
    };
    
    TMEStep::NewStory();
    
    LordNotPreparesForBattle();
    
    GIVEN("a lord is near enemies")
    {
        TMEStep::LordAtLocation(enemy, location);
        TMEStep::LordsIsLookingAt(lord, enemy);
    
        WHEN("a lord enters battle")
        {
            GetCharacter(lord)->Cmd_Attack();
            
            THEN("the lords should be preparing for battle")
            {
                LordPreparesForBattle();
            }
        }
        
        WHEN("a lord fails an approach")
        {
            GetCharacter(lord)->Cmd_Approach();
            
            THEN("the lords should be preparing for battle")
            {
                LordPreparesForBattle();
            }
        }
    }
    
    GIVEN("a lord is at the same location as an enemy lord")
    {
        TMEStep::LordAtLocation(enemy, location);
        TMEStep::LordsAtSameLocation(enemy, lord);
        
        WHEN("dawn breaks")
        {
            DDRStep::CharactersDawnBreaks();
        
            THEN("the lords should be preparing for battle")
            {
                LordPreparesForBattle();
            }
        }
    }

    GIVEN("a lord is at the same location as an enemy stronghold")
    {
        TMEStep::LordAtLocation(lord, enemyStronghold);
        
        WHEN("dawn breaks")
        {
            DDRStep::CharactersDawnBreaks();
        
            THEN("the lords should be preparing for battle")
            {
                LordPreparesForBattle();
            }
        }
    }
    
    GIVEN("a lord is preparing for battle")
    {
        GetCharacter(lord)->Flags().Set(cf_preparesbattle);

        WHEN("the lord moves")
        {
            GetCharacter(lord)->Cmd_WalkForward();

            THEN("the lords should not be preparing for battle")
            {
                LordNotPreparesForBattle();
            }
        }
    }

}

// CH_TARITHEL vs CH_VARATRARG @ SH_FORTRESS_VARATRARG
//TME: BATTLE: [  3] CH_TARITHEL
//TME:   [0:3] CH_TARITHEL
//TME:   [1:94] CH_VARATRARG
//TME:   skirmish: CH_TARITHEL
//TME:     Start=0 Rnd=8, Count=2
//TME:     Attacker = CH_TARITHEL     , Defender = CH_VARATRARG
//TME:     CH_TARITHEL      (255) vs CH_VARATRARG     (63)
//TME:     Defender: CH_VARATRARG     lost fight
//TME:     Defender: CH_VARATRARG     is dead killed by CH_TARITHEL
//TME:       Defender: CH_VARATRARG     lost 0 of (1000)
//TME:       Defender: CH_TARITHEL      lost 0 of (0)
//TME:       Attacker: CH_VARATRARG     won battle
//TME:       Defender: CH_TARITHEL      lost battle
//TME:   skirmish: CH_VARATRARG
//TME:     Start=1 Rnd=15, Count=2
//TME:     Attacker = CH_VARATRARG    , Defender = CH_TARITHEL
//TME:     CH_VARATRARG     (201) vs CH_TARITHEL      (33)
//TME:     Defender: CH_TARITHEL      lost fight
//TME:       Defender: CH_TARITHEL      lost 0 of (0)
//TME:       Attacker: CH_VARATRARG     won battle
//TME:       Defender: CH_TARITHEL      lost battle
//TME:       Defender: CH_VARATRARG     lost 0 of (1000)
//TME: Stronghold=SH_FORTRESS_VARATRARG
//TME:     Start=1 Rnd=161, Count=2
//TME:     Attacker = SH_FORTRESS_VARATRARG, Defender = CH_TARITHEL
//TME:       Defender: CH_TARITHEL      lost 0 of (0)
//TME:       Defender: CH_TARITHEL      lost battle
//TME:       Defender: SH_FORTRESS_VARATRARG lost 0 of (1050)

TEST_CASE("Tarithel vs Varatrarg @ fortress of Varatrarg", IGNORE_TAG)
{
    TMEStep::NewStory();

    auto tarithel = GetDDRCharacter("CH_TARITHEL");
    auto varatarg = GetDDRCharacter("CH_VARATRARG");
    auto fortress = GetStronghold("SH_FORTRESS_VARATRARG");
        
    tarithel->Location(fortress->Location());
    varatarg->Location(fortress->Location());
    
    auto battle = static_cast<ddr_battle*>(tme::mx->battle);
    
    battle->Battle(tarithel);
}

TEST_CASE("Ushangrane vs Varatrarg @ fortress of Varatrarg", IGNORE_TAG)
{
    TMEStep::NewStory();

    auto tarithel = GetDDRCharacter("CH_USHANGRANE");
    auto varatarg = GetDDRCharacter("CH_VARATRARG");
    auto fortress = GetStronghold("SH_FORTRESS_VARATRARG");
        
    tarithel->Location(fortress->Location());
    varatarg->Location(fortress->Location());
    
    auto battle = static_cast<ddr_battle*>(tme::mx->battle);
    
    battle->Battle(tarithel);
}

// CH_TARITHEL vs CH_THELAK @ SH_FORTRESS_THELAK
//TME: BATTLE: [  3] CH_TARITHEL
//TME:   [0:3] CH_TARITHEL
//TME:   [1:87] CH_THELAK
//TME:   skirmish: CH_TARITHEL
//TME:     Start=0 Rnd=84, Count=2
//TME:     Attacker = CH_TARITHEL     , Defender = CH_THELAK
//TME:     CH_TARITHEL      (255) vs CH_THELAK        (190)
//TME:     Defender: CH_THELAK        lost fight
//TME:       Defender: CH_THELAK        lost 0 of (1050)
//TME:       Defender: CH_TARITHEL      lost 0 of (0)
//TME:       Attacker: CH_THELAK        won battle
//TME:       Defender: CH_TARITHEL      lost battle
//TME:   skirmish: CH_THELAK
//TME:     Start=0 Rnd=124, Count=2
//TME:     Attacker = CH_THELAK       , Defender = CH_TARITHEL
//TME:     CH_THELAK        (201) vs CH_TARITHEL      (32)
//TME:     Defender: CH_TARITHEL      lost fight
//TME:       Defender: CH_TARITHEL      lost 0 of (0)
//TME:       Attacker: CH_THELAK        won battle
//TME:       Defender: CH_TARITHEL      lost battle
//TME:       Defender: CH_THELAK        lost 0 of (1050)
//TME: Stronghold=SH_FORTRESS_THELAK
//TME:     Start=1 Rnd=227, Count=2
//TME:     Attacker = SH_FORTRESS_THELAK, Defender = CH_TARITHEL
//TME:       Defender: CH_TARITHEL      lost 0 of (0)
//TME:       Defender: CH_TARITHEL      lost battle
//TME:       Defender: SH_FORTRESS_THELAK lost 0 of (1100)

TEST_CASE("Tarithel vs Thelak @ fortress of Thelak", IGNORE_TAG)
{
    TMEStep::NewStory();

    auto tarithel = GetDDRCharacter("CH_TARITHEL");
    auto varatarg = GetDDRCharacter("CH_THELAK");
    auto fortress = GetStronghold("SH_FORTRESS_THELAK");
        
    tarithel->Location(fortress->Location());
    varatarg->Location(fortress->Location());
    
    auto battle = static_cast<ddr_battle*>(tme::mx->battle);
    
    battle->Battle(tarithel);
}

TEST_CASE("Tarithel killed Thelak no object")
{
    TMEStep::NewStory();

    auto tarithel = GetDDRCharacter("CH_TARITHEL");
    auto enemy = GetDDRCharacter("CH_THELAK");

    auto battle = static_cast<ddr_battle*>(tme::mx->battle);
    
    battle->CharacterKilledByCharacter(tarithel, enemy);
    
    REQUIRE(tarithel->killedbyobject == nullptr);
    REQUIRE(tarithel->killedby == KB_LORD);
    REQUIRE(enemy->fighting_against == tarithel);
    REQUIRE(enemy->Flags().Is(cf_wonbattle|cf_killed_foe));
    
}

TEST_CASE("Tarithel killed Thelak with object")
{
    TMEStep::NewStory();

    auto tarithel = GetDDRCharacter("CH_TARITHEL");
    
    auto enemy = GetDDRCharacter("CH_THELAK");
    auto object = GetObject("OB_HAMMER_IMGARORN");
    TMEStep::LordCarryingObject(enemy->Symbol(), object->Symbol());

    auto battle = static_cast<ddr_battle*>(tme::mx->battle);
    
    battle->CharacterKilledByCharacter(tarithel, enemy);
    
    REQUIRE(tarithel->killedbyobject == object);
    REQUIRE(tarithel->killedby == KB_LORD_OBJECT);
    REQUIRE(enemy->fighting_against == tarithel);
    REQUIRE(enemy->Flags().Is(cf_wonbattle|cf_killed_foe));
    
}
