//
//  prisoner_tests.cpp
//  citadel
//
//  The hostages Boroth the Wolfheart holds in the dungeons of the Dark Citadel.
//
//  The Citadel still ships Lords of Midnight's database, so none of the twelve lords named
//  in citadel_scenario.cpp are in it to find yet. What these pin is the RULE, and the rule
//  is tested on whoever the database does hold: mark one lord of a race as a prisoner and
//  his kinsmen shut their doors until he is out.
//
#include "../../steps/tme_steps.h"

static constexpr LPCSTR ch_corleth = "CH_CORLETH";
static constexpr LPCSTR ch_dreams  = "CH_DREAMS";
static constexpr LPCSTR ch_shadows = "CH_SHADOWS";

SCENARIO("A realm whose hostage is still held will not join you")
{
    TMEStep::NewStory();

    auto corleth = GetCharacter(ch_corleth);
    auto hostage = GetCharacter(ch_dreams);
    auto kinsman = GetCharacter(ch_shadows);

    // the rule is about a race, so these two must be of one, or the test proves nothing
    REQUIRE( hostage->Race() == kinsman->Race() );

    GIVEN("a lord who would otherwise join you")
    {
        REQUIRE( corleth->CheckRecruitChar(kinsman) );

        WHEN("one of his own race is held hostage in the Dark Citadel")
        {
            hostage->Flags().Set(cf_prisoner);

            THEN("he will not join")
            {
                REQUIRE( !corleth->CheckRecruitChar(kinsman) );
            }

            AND_THEN("but the hostage may still be approached, which is how he is freed")
            {
                REQUIRE( corleth->CheckRecruitChar(hostage) );
            }
        }
    }
}

SCENARIO("Freeing a hostage opens his realm")
{
    TMEStep::NewStory();

    auto corleth = GetCharacter(ch_corleth);
    auto hostage = GetCharacter(ch_dreams);
    auto kinsman = GetCharacter(ch_shadows);

    GIVEN("that a realm's hostage is held")
    {
        hostage->Flags().Set(cf_prisoner);
        REQUIRE( !corleth->CheckRecruitChar(kinsman) );

        WHEN("the hostage is recruited where he is held")
        {
            hostage->Recruited(corleth);

            THEN("he is a prisoner no longer")
            {
                REQUIRE( !hostage->IsPrisoner() );
            }

            AND_THEN("his people can be persuaded to join you")
            {
                REQUIRE( corleth->CheckRecruitChar(kinsman) );
            }
        }
    }
}

SCENARIO("A hostage of one realm does not shut the door on another")
{
    TMEStep::NewStory();

    auto hostage = GetCharacter(ch_dreams);
    auto luxor   = GetCharacter(TMEStep::ch_luxor);
    auto freeman = GetCharacter("CH_XAJORKITH");

    REQUIRE( freeman->Race() != hostage->Race() );

    GIVEN("that the hostage of one realm is held")
    {
        hostage->Flags().Set(cf_prisoner);

        THEN("a lord of another realm is untouched by it")
        {
            REQUIRE( luxor->CheckRecruitChar(freeman) );
        }
    }
}
