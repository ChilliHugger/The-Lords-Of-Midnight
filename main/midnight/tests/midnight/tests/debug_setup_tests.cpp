//
//  debug_setup_tests.cpp
//  midnight
//

#include "../steps/common_steps.h"
#include "../../../Source/tme/utils/debugsetup.h"

#if defined(_DEBUG_NEW_GAME_GROUP_)

SCENARIO("The debug large group setup recruits lords into the starting lord's group")
{
    TMEStep::NewStory();

    GIVEN("a new game")
    {
        auto leader = tme::mx->CurrentChar();

        WHEN("a large group is created")
        {
            auto count = tme::utils::DebugCreateLargeGroup(12);

            THEN("the starting lord is leading a group of more than the visible lords")
            {
                REQUIRE( count == 12 );
                REQUIRE( leader->followers == 12 );
                REQUIRE( leader->HasFollowers() );
            }

            THEN("all the followers are recruited and at the leader's location")
            {
                int found = 0;
                for ( auto lord : tme::mx->objCharacters ) {
                    if ( lord->Following() != leader )
                        continue;
                    found++;
                    REQUIRE( lord->IsRecruited() );
                    REQUIRE( lord->Location() == leader->Location() );
                }
                REQUIRE( found == 12 );
            }
        }
    }
}

#endif // _DEBUG_NEW_GAME_GROUP_
