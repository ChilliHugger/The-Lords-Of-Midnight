//
//  recruitment_tests.cpp
//  revenge
//

#include "../steps/common_steps.h"

// Discussion #293 / Issue #294
//
// Glormane the Dwarf is the liege of Berangrorn the Dwarf.
// The player recruits Glormane, but Berangrorn keeps Glormane as his liege.
// When Berangrorn then recruits Glormane back, Glormane's liege becomes
// Berangrorn - a circular liege.

static bool IsLiegeOf(const string& liege, const string& name)
{
    auto target = GetCharacter(liege);
    auto c = GetCharacter(name)->Liege();
    for ( u32 depth=0; c != nullptr && depth < tme::mx->objCharacters.Count(); depth++ ) {
        if ( c == target )
            return true;
        c = c->Liege();
    }
    return false;
}

SCENARIO("A recruited lord cannot become circular in the liege tree")
{
    // RF_DDR_CIRCULAR_LIEGE_FIX is on by default
    TMEStep::NewStory();
    REQUIRE( tme::mx->isRuleEnabled(RF_DDR_CIRCULAR_LIEGE_FIX) );

    auto glormane = "CH_GLORMANE";
    auto berangrorn = "CH_BERANGRORN";

    GIVEN("Berangrorn has Glormane as his liege")
    {
        REQUIRE( IsLiegeOf(glormane, berangrorn) );
        REQUIRE( GetCharacter(berangrorn)->Liege() == GetCharacter(glormane) );

        AND_GIVEN("the player has recruited Glormane")
        {
            TMEStep::LordIsRecruitedBy(glormane, TMEStep::ch_luxor);

            REQUIRE( GetCharacter(glormane)->Liege() == GetCharacter(TMEStep::ch_luxor) );
            REQUIRE( IsLiegeOf(glormane, berangrorn) );

            WHEN("Berangrorn recruits Glormane")
            {
                TMEStep::LordIsRecruitedBy(glormane, berangrorn);

                THEN("Glormane should not have Berangrorn as a liege")
                {
                    REQUIRE_FALSE( IsLiegeOf(berangrorn, glormane) );
                }

                AND_THEN("Glormane should have no liege, as he is the top of the tree")
                {
                    REQUIRE( GetCharacter(glormane)->Liege() == nullptr );
                }

                AND_THEN("Glormane should now be loyal to Berangrorn")
                {
                    REQUIRE( GetCharacter(glormane)->NormalisedLoyalty() == GetCharacter(berangrorn)->NormalisedLoyalty() );
                }
            }
        }
    }
}

SCENARIO("A recruited lord can become circular in the liege tree when the fix is disabled")
{
    TMEStep::NewStory(RF_NONE);

    auto glormane = "CH_GLORMANE";
    auto berangrorn = "CH_BERANGRORN";

    GIVEN("The circular liege fix rule is disabled")
    {
        REQUIRE_FALSE( tme::mx->isRuleEnabled(RF_DDR_CIRCULAR_LIEGE_FIX) );

        AND_GIVEN("Berangrorn has Glormane as his liege, and the player has recruited Glormane")
        {
            TMEStep::LordIsRecruitedBy(glormane, TMEStep::ch_luxor);

            WHEN("Berangrorn recruits Glormane")
            {
                TMEStep::LordIsRecruitedBy(glormane, berangrorn);

                THEN("Glormane and Berangrorn are each other's liege (original game bug)")
                {
                    REQUIRE( IsLiegeOf(berangrorn, glormane) );
                    REQUIRE( IsLiegeOf(glormane, berangrorn) );
                }
            }
        }
    }
}
