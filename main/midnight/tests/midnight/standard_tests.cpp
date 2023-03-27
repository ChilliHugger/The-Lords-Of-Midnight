//
//  standard_tests.cpp
//  midnight
//
//  Created by Chris Wild on 03/01/2021.
//

#include "common_steps.h"

TEST_CASE( "Lom Scenario has been setup corretly" )
{
    REQUIRE_THAT( TME_ScenarioDirectory(),  Catch::Matchers::EndsWith("/lom") );
    REQUIRE_THAT( TME_ScenarioName(),  Catch::Matchers::Equals("The Lords of Midnight") );
    REQUIRE_THAT( TME_ScenarioShortName(),  Catch::Matchers::Equals("lom") );
}

SCENARIO( "Cocos Root Path set correctly")
{
    GIVEN("A the default resource path has been setup")
    {
        auto resourcePath = cocos2d::FileUtils::getInstance()->getDefaultResourceRootPath();
        
        cocos2d::FileUtils::getInstance()->setDefaultResourceRootPath("Resources");

        WHEN("the scenario directory is requested")
        {
            auto directory = std::string(TME_ScenarioDirectory());

            THEN("the folder should point to the correct scenario path")
            {
              REQUIRE_THAT( TME_ScenarioDirectory(),  Catch::Matchers::Equals("Resources/lom") );
            }
        }

        cocos2d::FileUtils::getInstance()->setDefaultResourceRootPath(resourcePath);

    }
}

SCENARIO("For a new story Luxor should be the initial lord")
{
    GIVEN("A new story")
    {
        WHEN("the story is started")
        {
            auto created = TME_Init(RF_DEFAULT, DF_NORMAL);

            THEN("the story creation should be successfull")
            {
                REQUIRE( created == true );
               
                AND_THEN("the selected lord should be luxor")
                {
                    REQUIRE_THAT( std::string(TME_CurrentCharacter().shortname), Catch::Matchers::Equals("Luxor") );
                }
            }
        }
    }
}
