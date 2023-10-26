//
//  recruitment_tests.cpp
//  midnight
//
//  Created by Chris Wild on 25/03/2023.
//
#include "../steps/common_steps.h"
#include "../../steps/map_steps.h"

SCENARIO("Morkin can recruit Shadows")
{
    TMEStep::NewStory();

    GIVEN("That Morkin is at the same location as Shadows")
    {
        TMEStep::LordsAtSameLocation(TMEStep::ch_morkin, LOMStep::ch_shadows);
        
        AND_GIVEN("that Shadows is not recruited")
        {
            TMEStep::LordIsNotRecruited(LOMStep::ch_shadows);
            
            WHEN("Morkin approaches")
            {
                TMEStep::LordPerformsApproach(TMEStep::ch_morkin);
                
                THEN("Shadows should be recruited")
                {
                    REQUIRE( TMEStep::LordHasBeenRecruited(LOMStep::ch_shadows) );
                }
            }
        }
    }
}

SCENARIO("Luxor cannot recruit Dreams")
{
    TMEStep::NewStory();

    GIVEN("That Luxor is at the same location as Dreams")
    {
        TMEStep::LordsAtSameLocation(TMEStep::ch_luxor, LOMStep::ch_dreams);
        
        AND_GIVEN("that Dreams is not recruited")
        {
            TMEStep::LordIsNotRecruited(LOMStep::ch_dreams);
            
            WHEN("Luxor approaches")
            {
                TMEStep::LordPerformsApproach(TMEStep::ch_luxor);
                
                THEN("Shadows should not be recruited")
                {
                    REQUIRE( !TMEStep::LordHasBeenRecruited(LOMStep::ch_dreams) );
                }
            }
        }
    }
}

SCENARIO("Morkin does not recruit Shadows after move without auto approach")
{
    TMEStep::NewStory();

    GIVEN("That Morkin is looking at Shadows")
    {
        TMEStep::LordsIsLookingAt(TMEStep::ch_morkin, LOMStep::ch_shadows);
        MapStep::ClearObjectFromLocation( GetCharacter(TMEStep::ch_morkin)->Location() );
    
        AND_GIVEN("that auto approach is disabled")
        {
            tme::variables::sv_auto_approach = false;
            
            AND_GIVEN("that Shadows is not recruited")
            {
                TMEStep::LordIsNotRecruited(LOMStep::ch_shadows);
                
                WHEN("Morkin moves forward")
                {
                    TMEStep::LordMovesForward(TMEStep::ch_morkin);
                    
                    THEN("Shadows should not be recruited")
                    {
                        REQUIRE( !TMEStep::LordHasBeenRecruited(LOMStep::ch_shadows) );
                    }
                }
            }
        }
    }
}

SCENARIO("Morkin recruits Shadows after move with auto approach")
{
    TMEStep::NewStory();

    GIVEN("That Morkin is looking at Shadows")
    {
        TMEStep::LordsIsLookingAt(TMEStep::ch_morkin, LOMStep::ch_shadows);
        MapStep::ClearObjectFromLocation( GetCharacter(TMEStep::ch_morkin)->Location() );
        
        AND_GIVEN("that auto approach is enabled")
        {
            tme::variables::sv_auto_approach = true;
            
            AND_GIVEN("that Shadows is not recruited")
            {
                TMEStep::LordIsNotRecruited(LOMStep::ch_shadows);
                
                WHEN("Morkin moves forward")
                {
                    TMEStep::LordMovesForward(TMEStep::ch_morkin);
                    
                    THEN("Shadows should be recruited")
                    {
                        REQUIRE( TMEStep::LordHasBeenRecruited(LOMStep::ch_shadows) );
                    }
                }
            }
        }
    }
}

// SCENARIO("Morkin cannot recruit Shadows when he is not alone")
// SCENARIO("Morkin cannot recruit Shadows with auto approach when he is not alone")
// SCENARIO("recruiting with followers")
