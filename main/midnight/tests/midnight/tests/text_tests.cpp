//
//  text_tests.cpp
//  midnight
//
//  Created by Chris Wild on 16/02/2025.
//

#include "../steps/common_steps.h"
#include "../../steps/map_steps.h"

string result;

void WhenLordSees(mxcharacter* character)
{
    result = tme::mx->text->DescribeCharacterSees(character);
};

void ThenLordSees(LPCSTR text) {
    REQUIRE_THAT(result, Catch::Matchers::StartsWith(text));
}

void ThenLordSeesNothing() {
    REQUIRE_THAT(result, Catch::Matchers::Equals(""));
}


SCENARIO("NOVEL: Character Sees something at a location") {
    TMEStep::NewStory(mxscenarioid::LOM_NOVEL, RF_DEFAULT, DF_NORMAL);
    
    
    auto character = GetCharacter(TMEStep::ch_luxor);

    //
    // Tunnels are not supported in standard lom
    // Nor are the 'sees' descriptions
    // Lom_Novel has a tunnel at the cavern of despair
    // so let's make sure we've not bled implementations between scenarios
    //
    GIVEN("Lord is at a location with a tunnel entrance") {
        auto location = mxgridref(25, 5); // Cavern of Despair
        TMEStep::LordAtLocation(character->Symbol().c_str(), location);
        
        AND_GIVEN("there is no object to pickup") {
            // remove any object
            MapStep::SetObjectAtLocation(character->Location(), OB_NONE);
        
            WHEN("the lord 'sees'") {
                WhenLordSees(character);
            
                THEN("they will see the tunnel entrance") {
                    ThenLordSees("Luxor sees an underground entrance.");
                }
            }
        }
        
        AND_GIVEN("there is an object to pickup") {
            MapStep::SetObjectAtLocation(character->Location(), OB_WOLFSLAYER);
        
            WHEN("the lord 'sees'") {
                WhenLordSees(character);
                         
                THEN("they will see the tunnel entrance and the object") {
                    ThenLordSees("Luxor sees the Sword Wolfslayer and an underground entrance.");
                }
            }
        }
    }
    
    GIVEN("Lord is at a location without a tunnel entrance") {
        auto location = mxgridref(13, 41); // Tower of the moon
        TMEStep::LordAtLocation(character->Symbol().c_str(), location);
        
        AND_GIVEN("there is no object to pickup") {
            // remove any object
            MapStep::SetObjectAtLocation(character->Location(), OB_NONE);
            
            WHEN("the lord 'sees'") {
                WhenLordSees(character);
            
                THEN("they will see nothing") {
                    ThenLordSeesNothing();
                }
            }
        }
        
        AND_GIVEN("there is an object to pickup") {
            MapStep::SetObjectAtLocation(character->Location(), OB_WOLFSLAYER);
            
            WHEN("the lord 'sees'") {
                WhenLordSees(character);
                         
               THEN("they will see the object") {
                    ThenLordSees("Luxor sees the Sword Wolfslayer. ");
                }
            }
        }
    }
}


SCENARIO("NORMAL: Character Sees something at a location") {
    TMEStep::NewStory();
    
    auto character = GetCharacter(TMEStep::ch_luxor);

    //
    // Tunnels are not supported in standard lom
    // Nor are the 'sees' descriptions
    // Lom_Novel has a tunnel at the cavern of despair
    // so let's make sure we've not bled implementations between scenarios
    //
    GIVEN("Lord is at a location with a tunnel entrance") {
        auto location = mxgridref(25, 5); // Cavern of Despair
        TMEStep::LordAtLocation(character->Symbol().c_str(), location);
        
        AND_GIVEN("there is no object to pickup") {
            // remove any object
            MapStep::SetObjectAtLocation(character->Location(), OB_NONE);
        
            WHEN("the lord 'sees'") {
                WhenLordSees(character);
            
                THEN("they will see nothing") {
                    ThenLordSeesNothing();
                }
            }
        }
        
        AND_GIVEN("there is an object to pickup") {
            MapStep::SetObjectAtLocation(character->Location(), OB_WOLFSLAYER);
        
            WHEN("the lord 'sees'") {
                WhenLordSees(character);
                         
                THEN("they will see nothing") {
                    ThenLordSeesNothing();
                }
            }
        }
    }
    
    GIVEN("Lord is at a location without a tunnel entrance") {
        auto location = mxgridref(13, 41); // Tower of the moon
        TMEStep::LordAtLocation(character->Symbol().c_str(), location);
        
        AND_GIVEN("there is no object to pickup") {
            // remove any object
            MapStep::SetObjectAtLocation(character->Location(), OB_NONE);
            
            WHEN("the lord 'sees'") {
                WhenLordSees(character);
            
                THEN("they will see nothing") {
                    ThenLordSeesNothing();
                }
            }
        }
        
        AND_GIVEN("there is an object to pickup") {
            MapStep::SetObjectAtLocation(character->Location(), OB_WOLFSLAYER);
            
            WHEN("the lord 'sees'") {
                WhenLordSees(character);
                         
                THEN("they will see the object") {
                    ThenLordSeesNothing();
                }
            }
        }
    }
}

