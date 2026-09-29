//
//  prisoner_tests.cpp
//  citadel
//
//  Recruitment, and the hostages Boroth the Wolfheart holds in the dungeons of the Dark Citadel.
//
#include "../../steps/tme_steps.h"

static constexpr LPCSTR ch_morkin   = "CH_MORKIN";
static constexpr LPCSTR ch_hostage  = "CH_DJALINA";     // the Uskarg hostage
static constexpr LPCSTR ch_kinsman  = "CH_HARAGLAI";    // an Uskarg lord
static constexpr LPCSTR ch_stranger = "CH_MOGRIK";      // a Kith
static constexpr LPCSTR ch_darkfey  = "CH_TORANETH";

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

    citadel_character* Lord(LPCSTR name, u64 qualities)
    {
        auto lord = static_cast<citadel_character*>(GetCharacter(name));
        REQUIRE( lord != nullptr );
        lord->qualities = qualities;
        return lord;
    }
}

SCENARIO("Recruitment is a score of shared and opposed attributes")
{
    TMEStep::NewStory();
    ClearAllHostages();

    auto recruiter = Lord(ch_morkin, qf_brave|qf_loyal|qf_cruel);
    auto lord = GetCharacter(ch_kinsman);

    THEN("a shared attribute counts for, good or bad")
    {
        lord->qualities = qf_brave|qf_loyal|qf_cruel;
        REQUIRE( recruiter->RecruitScore(lord) == 3 );
    }

    THEN("an opposed attribute counts against, from either side")
    {
        // brave/cowardly and loyal/treacherous oppose the recruiter's good attributes;
        // kind opposes his cruelty
        lord->qualities = qf_cowardly|qf_treacherous|qf_kind;
        REQUIRE( recruiter->RecruitScore(lord) == -3 );
    }

    THEN("an unrelated attribute counts for nothing")
    {
        lord->qualities = qf_generous|qf_tireless;
        REQUIRE( recruiter->RecruitScore(lord) == 0 );
    }

    THEN("the last pair is opposed too: a mighty warrior and a feeble one")
    {
        auto warrior = Lord(ch_morkin, qf_mightywarrior);
        lord->qualities = qf_feeblewarrior|qf_brave;
        REQUIRE( warrior->RecruitScore(lord) == -1 );
    }
}

SCENARIO("A held hostage raises the bar for his realm from one point to two")
{
    TMEStep::NewStory();
    ClearAllHostages();

    auto recruiter = Lord(ch_morkin, qf_brave|qf_loyal);
    auto hostage = GetCharacter(ch_hostage);
    auto kinsman = Lord(ch_kinsman, qf_brave);   // one point

    // the rule is about a race, so these two must be of one, or the test proves nothing
    REQUIRE( hostage->Race() == kinsman->Race() );

    GIVEN("a lord worth one point, in a realm Boroth holds nothing over")
    {
        REQUIRE( recruiter->RecruitScore(kinsman) == 1 );
        REQUIRE( recruiter->CheckRecruitChar(kinsman) );

        WHEN("one of his race is held hostage in the Dark Citadel")
        {
            hostage->Flags().Set(cf_prisoner);

            THEN("one point is no longer enough")
            {
                REQUIRE( !recruiter->CheckRecruitChar(kinsman) );
            }

            AND_THEN("but a lord of stouter heart, worth two, still joins")
            {
                kinsman->qualities = qf_brave|qf_loyal;
                REQUIRE( recruiter->CheckRecruitChar(kinsman) );
            }

            AND_THEN("and the hostage may be approached whatever his attributes, which is how he is freed")
            {
                hostage->qualities = qf_cowardly|qf_treacherous;
                REQUIRE( recruiter->CheckRecruitChar(hostage) );
            }
        }
    }
}

SCENARIO("Freeing a hostage makes his realm easier to persuade")
{
    TMEStep::NewStory();
    ClearAllHostages();

    auto recruiter = Lord(ch_morkin, qf_brave|qf_loyal);
    auto hostage = GetCharacter(ch_hostage);
    auto kinsman = Lord(ch_kinsman, qf_brave);   // one point

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

            AND_THEN("one point now persuades his people")
            {
                REQUIRE( recruiter->CheckRecruitChar(kinsman) );
            }
        }
    }
}

SCENARIO("A hostage of one realm does not raise the bar in another")
{
    TMEStep::NewStory();
    ClearAllHostages();

    auto recruiter = Lord(ch_morkin, qf_brave|qf_loyal);
    auto hostage = GetCharacter(ch_hostage);
    auto stranger = Lord(ch_stranger, qf_brave);   // one point

    REQUIRE( stranger->Race() != hostage->Race() );

    GIVEN("that the hostage of one realm is held")
    {
        hostage->Flags().Set(cf_prisoner);

        THEN("one point still persuades a lord of another realm")
        {
            REQUIRE( recruiter->CheckRecruitChar(stranger) );
        }
    }
}

SCENARIO("A lord who scores nothing, or less, will not join")
{
    TMEStep::NewStory();
    ClearAllHostages();

    auto recruiter = Lord(ch_morkin, qf_brave|qf_loyal);

    THEN("nothing in common is a refusal")
    {
        auto lord = Lord(ch_kinsman, qf_generous);
        REQUIRE( recruiter->RecruitScore(lord) == 0 );
        REQUIRE( !recruiter->CheckRecruitChar(lord) );
    }

    THEN("an offended lord refuses too - the 1995 attack on the recruiter is not modelled")
    {
        auto lord = Lord(ch_kinsman, qf_cowardly);
        REQUIRE( recruiter->RecruitScore(lord) == -1 );
        REQUIRE( !recruiter->CheckRecruitChar(lord) );
    }
}

SCENARIO("Boroth's own host cannot be recruited")
{
    TMEStep::NewStory();
    ClearAllHostages();

    auto recruiter = Lord(ch_morkin, qf_brave|qf_loyal);
    auto darkfey = Lord(ch_darkfey, qf_brave|qf_loyal);

    REQUIRE( darkfey->Race() == RA_ENEMY );
    REQUIRE( recruiter->RecruitScore(darkfey) == 2 );
    REQUIRE( !recruiter->CheckRecruitChar(darkfey) );
}
