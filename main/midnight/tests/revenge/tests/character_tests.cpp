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
