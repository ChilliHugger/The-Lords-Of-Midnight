//
//  prisoner_tests.cpp
//  citadel
//
//  The hostages Boroth the Wolfheart holds in the dungeons of the Dark Citadel.
//
#include "../../steps/tme_steps.h"

static constexpr LPCSTR ch_morkin = "CH_MORKIN";
static constexpr LPCSTR ch_lord1  = "CH_MORATHRON";
static constexpr LPCSTR ch_lord2 = "CH_GUTHRANE";

namespace {
    // The shipped database already holds some lords hostage (Boroth's dungeons aren't empty
    // by default). These tests set up their own hostage state, so start from a clean slate
    // rather than depend on who the data happens to be holding.
    void ClearAllHostages()
    {
        for ( auto character : tme::mx->objCharacters ) {
            character->Flags().Reset(cf_prisoner);
        }
    }
}

SCENARIO("A realm whose hostage is still held will not join you")
{
    TMEStep::NewStory();
    ClearAllHostages();

    auto recruiter = GetCharacter(ch_morkin);
    auto hostage = GetCharacter(ch_lord1);
    auto kinsman = GetCharacter(ch_lord2);

    // the rule is about a race, so these two must be of one, or the test proves nothing
    REQUIRE( hostage->Race() == kinsman->Race() );

    GIVEN("a lord who would otherwise join you")
    {
        REQUIRE( recruiter->CheckRecruitChar(kinsman) );

        WHEN("one of his own race is held hostage in the Dark Citadel")
        {
            hostage->Flags().Set(cf_prisoner);

            THEN("he will not join")
            {
                REQUIRE( !recruiter->CheckRecruitChar(kinsman) );
            }

            AND_THEN("but the hostage may still be approached, which is how he is freed")
            {
                REQUIRE( recruiter->CheckRecruitChar(hostage) );
            }
        }
    }
}

SCENARIO("Freeing a hostage opens his realm")
{
    TMEStep::NewStory();
    ClearAllHostages();

    auto recruiter = GetCharacter(ch_morkin);
    auto hostage = GetCharacter(ch_lord1);
    auto kinsman = GetCharacter(ch_lord2);

    GIVEN("that a realm's hostage is held")
    {
        hostage->Flags().Set(cf_prisoner);
        REQUIRE( !recruiter->CheckRecruitChar(kinsman) );

        WHEN("the hostage is recruited where he is held")
        {
            hostage->Recruited(recruiter);

            THEN("he is a prisoner no longer")
            {
                REQUIRE( !hostage->IsPrisoner() );
            }

            AND_THEN("his people can be persuaded to join you")
            {
                REQUIRE( recruiter->CheckRecruitChar(kinsman) );
            }
        }
    }
}

SCENARIO("A hostage of one realm does not shut the door on another")
{
    TMEStep::NewStory();
    ClearAllHostages();

    auto hostage = GetCharacter(ch_lord1);
    auto recruiter = GetCharacter(TMEStep::ch_luxor);
    auto freeman = GetCharacter("CH_MOGRIK");

    REQUIRE( freeman->Race() != hostage->Race() );

    GIVEN("that the hostage of one realm is held")
    {
        hostage->Flags().Set(cf_prisoner);

        THEN("a lord of another realm is untouched by it")
        {
            REQUIRE( recruiter->CheckRecruitChar(freeman) );
        }
    }
}
