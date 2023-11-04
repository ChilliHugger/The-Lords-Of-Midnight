//
//  character_tests.cpp
//  revenge
//
//  Created by Chris Wild on 02/11/2023.
//

#include "../steps/common_steps.h"

// GIVEN a lord is at a location with an object
// AND they are not carrying an object
// WHEN they start their turn
// THEN they should pickup the object

// GIVEN a lord is at location with an object
// AND they are carrying an object that is not their desired object
// WHEN they start their turn
// THEN they should not pickup the object

// GIVEN a lord is at a location with their desired object
// AND they are carrying an object
// WHEN they start their turn
// THEN they should pickup the object
// AND they should drop the previously carried object

SCENARIO("Characters may pickup objects at the beginning of their turn ")
{
    TMEStep::NewStory();
    
    auto location = mxgridref(1,1);
    auto lord = "CH_TALORMANE";
    auto locationObject = "OB_SWORD_ZARASHAND";
    auto carryingObject = "OB_HAMMER_VARANGRIM";
    auto desiredObject = "OB_SWORD_TALORMANE";
    
    GIVEN("A lord is at a location with an object")
    {
        TMEStep::LordAtLocation(lord, location);
        DDRStep::ObjectAtLocation(locationObject, location);
        
        AND_GIVEN("they are not carrying an object")
        {
            TMEStep::LordNotCarryingObject(lord);
            
            WHEN("they start their turn")
            {
                DDRStep::LordTakesTurn(lord);
                
                THEN("they should pickup the object")
                {
                    REQUIRE( GetCharacter(lord)->carrying->Symbol() == locationObject );
                }
            }
        }

        AND_GIVEN("they are carrying an object that is not their desired object")
        {
            TMEStep::LordCarryingObject(lord, carryingObject);
            
            WHEN("they start their turn")
            {
                DDRStep::LordTakesTurn(lord);
                
                THEN("they should not pickup the object")
                {
                    REQUIRE( GetCharacter(lord)->carrying->Symbol() == carryingObject );
                }
            }
        }
    }
    
    GIVEN("A lord is at a location with their desired object")
    {
        TMEStep::LordAtLocation(lord, location);
        DDRStep::ObjectAtLocation(desiredObject, location);
        
        AND_GIVEN("they are not carrying an object")
        {
            TMEStep::LordNotCarryingObject(lord);
            
            WHEN("they start their turn")
            {
                DDRStep::LordTakesTurn(lord);
                
                THEN("they should pickup the object")
                {
                    REQUIRE( GetCharacter(lord)->carrying->Symbol() == desiredObject );
                }
            }
        }

        AND_GIVEN("they are carrying an object that is not their desired object")
        {
            TMEStep::LordCarryingObject(lord, carryingObject);
            
            WHEN("they start their turn")
            {
                DDRStep::LordTakesTurn(lord);
                
                THEN("they should pickup their desired object")
                {
                    REQUIRE( GetCharacter(lord)->carrying->Symbol() == desiredObject );
                    
                    AND_THEN("they should drop their carried object")
                    {
                        REQUIRE( GetObject(carryingObject)->Location() == location );
                    }
                }
            }
        }
    }
}

// GIVEN RF_DDR_BETTER_ARMIES is disabled
// AND lord is at a location with an enemy
// WHEN they start their turn
// THEN they should not consider staying to fight

// GIVEN RF_DDR_BETTER_ARMIES is enabled
// AND lord is at a location with an enemy
// WHEN they start their turn
// THEN they should consider staying to fight

// GIVEN RF_DDR_BETTER_ARMIES is enabled
// AND a coward is at a location with an enemy
// AND the lord has the same size army as the enemy
// WHEN they start their turn
// THEN they should not stay and fight

// GIVEN RF_DDR_BETTER_ARMIES is enabled
// AND a non coward is alone at a location with an enemy
// AND the lord has more than 75% of the enemy army
// WHEN they start their turn
// THEN they should stay and fight

// GIVEN RF_DDR_BETTER_ARMIES is enabled
// AND a non coward is alone at a location with an enemy
// AND the lord has less than 75% of the enemy army
// WHEN they start their turn
// THEN they should not stay and fight

// GIVEN RF_DDR_BETTER_ARMIES is enabled
// AND a non coward is not alone at a location with an enemy
// AND the lord's friends have less than 50% of the enemy army
// WHEN they start their turn
// THEN they should not stay and fight

// GIVEN RF_DDR_BETTER_ARMIES is enabled
// AND a non coward is not alone at a location with an enemy
// AND the lord's friends have more than 50% of the enemy army
// WHEN they start their turn
// THEN they should stay and fight

// GIVEN RF_DDR_BETTER_ARMIES is enabled
// AND a non coward is at a location with an enemy
// AND the enemy has no army
// WHEN they start their turn
// THEN they should stay and fight

const string SHOULD_WE_STAY_EVENT = "ShouldWeStayAndFight";
const string SHOULD_WE_STOP_TURN_EVENT = "ShouldWeStopTurnForEnemy";
const string PICK_NEW_ORDERS = "pickNewOrders";
const string NEW_ORDERS = "8"; // OD_FOLLOW_FOE

SCENARIO("Characters will consider staying in a battle", "[new]")
{
    auto enemy = DDRStep::ch_tarithel;
    auto lord = DDRStep::ch_shareth;
    auto friendly = "CH_IMGARIL";
    auto location = mxgridref(10,10);

    auto ShouldNotStayAndFight = [lord,enemy]() {
        REQUIRE( GetMockCharacter(lord)->HasEvent(SHOULD_WE_STAY_EVENT) );
        REQUIRE( !(GetMockCharacter(lord)->Location() == GetMockCharacter(enemy)->Location()) );
    };

    auto ShouldStayAndFight = [lord,enemy]() {
        REQUIRE( GetMockCharacter(lord)->HasEvent(SHOULD_WE_STAY_EVENT) );
        REQUIRE( (GetMockCharacter(lord)->Location() == GetMockCharacter(enemy)->Location()) );
    };

    auto LordsAtSameLocation = [lord,enemy,location]() {
        TMEStep::LordAtLocation(lord, location);
        TMEStep::LordsAtSameLocation(enemy, lord);
    };
    
    auto LordsHaveArmies = [lord,enemy](u32 friends, u32 foes) {
        GetMockCharacter(lord)->getUnit()->Total(friends);
        GetMockCharacter(enemy)->getUnit()->Total(foes);
    };

    auto LordsHaveSplitArmies = [lord,friendly,enemy](u32 friends, u32 foes) {
        GetMockCharacter(lord)->getUnit()->Total(friends);
        GetMockCharacter(friendly)->getUnit()->Total(friends);
        GetMockCharacter(enemy)->getUnit()->Total(foes);
    };
    
    auto ShouldRandomlyStayAndFight = [lord,enemy]() {
        REQUIRE( !GetMockCharacter(lord)->HasEvent(SHOULD_WE_STAY_EVENT) );
        REQUIRE( GetMockCharacter(lord)->HasEvent(SHOULD_WE_STOP_TURN_EVENT) );
    };


    GIVEN("RF_DDR_BETTER_ARMIES is disabled")
    {
        TMEStep::NewStory();
        // make sure lord doesn't get distracted
        GetMockCharacter(lord)->mockData.properties[PICK_NEW_ORDERS] = NEW_ORDERS ;

        AND_GIVEN("lord is at a location with an enemy")
        {
            LordsAtSameLocation();

            AND_GIVEN("the enemy has an army")
            {
                LordsHaveArmies(1000,1000);

                WHEN("they start their turn")
                {
                    DDRStep::LordTakesTurn(lord);

                    THEN("they should randomly stay and fight")
                    {
                        ShouldRandomlyStayAndFight();
                    }
                }
            }

            AND_GIVEN("the enemy has no army")
            {
                LordsHaveArmies(1000,0);

                WHEN("they start their turn")
                {
                    DDRStep::LordTakesTurn(lord);

                    THEN("they should randomly stay and fight")
                    {
                        ShouldRandomlyStayAndFight();
                    }
                }
            }
        }
        
    }
    
    GIVEN("RF_DDR_BETTER_ARMIES is enabled")
    {
        TMEStep::NewStory(RF_DDR_BETTER_ARMIES);
        
        // make sure lord doesn't get distracted
        GetMockCharacter(lord)->mockData.properties[PICK_NEW_ORDERS] = NEW_ORDERS ;
        GetMockCharacter(lord)->mockData.properties[SHOULD_WE_STOP_TURN_EVENT] = "false";
    
        AND_GIVEN("lord is at a location with an enemy")
        {
            LordsAtSameLocation();
            
            WHEN("they start their turn")
            {
                DDRStep::LordTakesTurn(lord);
                
                THEN("they should not consider staying to fight")
                {
                    REQUIRE( GetMockCharacter(lord)->HasEvent(SHOULD_WE_STAY_EVENT) );
                }
            }
        }
        
        AND_GIVEN("a coward is at a location with an enemy")
        {
            GetMockCharacter(lord)->Traits().Set(ct_coward);
            LordsAtSameLocation();

            AND_GIVEN("the lord has the same size army as the enemy")
            {
                LordsHaveArmies(1000,1000);

                WHEN("they start their turn")
                {
                    DDRStep::LordTakesTurn(lord);

                    THEN("they should not stay and fight")
                    {
                        ShouldNotStayAndFight();
                    }
                }
            }
        }
        
        AND_GIVEN("a non coward is alone at a location with an enemy")
        {
            GetMockCharacter(lord)->Traits().Reset(ct_coward);
            LordsAtSameLocation();

            AND_GIVEN("the lord has more than 75% army as the enemy")
            {
                LordsHaveArmies(800,1000);

                WHEN("they start their turn")
                {
                    DDRStep::LordTakesTurn(lord);

                    THEN("they should stay and fight")
                    {
                        ShouldStayAndFight();
                    }
                }
            }
            
            AND_GIVEN("the lord has less than 75% army as the enemy")
            {
                LordsHaveArmies(500,1000);

                WHEN("they start their turn")
                {
                    DDRStep::LordTakesTurn(lord);

                    THEN("they should not stay and fight")
                    {
                        ShouldNotStayAndFight();
                    }
                }
            }
            
            AND_GIVEN("the enemy has no army")
            {
                LordsHaveArmies(1000,0);

                WHEN("they start their turn")
                {
                    DDRStep::LordTakesTurn(lord);

                    THEN("they should stay and fight")
                    {
                        ShouldStayAndFight();
                    }
                }
            }
        }
        
        AND_GIVEN("a non coward is not alone at a location with an enemy")
        {
            GetMockCharacter(lord)->Traits().Reset(ct_coward);
            LordsAtSameLocation();
            TMEStep::LordsAtSameLocation(friendly, lord);

            AND_GIVEN("the lord's friends have more than 50% army as the enemy")
            {
                LordsHaveSplitArmies(300 /* 300x2 */,1000);

                WHEN("they start their turn")
                {
                    DDRStep::LordTakesTurn(lord);

                    THEN("they should stay and fight")
                    {
                        ShouldStayAndFight();
                    }
                }
            }
            
            AND_GIVEN("the lord's friends have less than 50% army as the enemy")
            {
                LordsHaveSplitArmies(200 /* 200x2 */,1000);

                WHEN("they start their turn")
                {
                    DDRStep::LordTakesTurn(lord);

                    THEN("they should not stay and fight")
                    {
                        ShouldNotStayAndFight();
                    }
                }
            }
        }
        
    }
}
