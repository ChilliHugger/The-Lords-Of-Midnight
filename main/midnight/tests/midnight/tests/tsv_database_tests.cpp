//
//  tsv_database_tests.cpp
//  midnight
//
//  Direct checks that the runtime TSV loader (ticket #329) populates known
//  LOM entity fields correctly, straight from lom/tsv/*.tsv - independent
//  of the higher-level gameplay tests, which only exercise these values
//  indirectly and would still pass even if an unrelated field were wrong.
//

#include "../steps/common_steps.h"

SCENARIO("TSV database loads known character fields correctly")
{
    TMEStep::NewStory();

    GIVEN("Luxor")
    {
        auto luxor = GetCharacter("CH_LUXOR");
        REQUIRE( luxor != nullptr );

        THEN("his base fields match characters.tsv")
        {
            REQUIRE( luxor->Longname() == "Luxor the Moonprince" );
            REQUIRE( luxor->Shortname() == "Luxor" );
            REQUIRE( luxor->Location() == mxgridref(13,41) );
            REQUIRE( luxor->Race() == RA_FREE );
            REQUIRE( luxor->gender == GD_MALE );
            REQUIRE( luxor->energy == 127 );
            REQUIRE( luxor->reckless == 180 );
            REQUIRE( luxor->strength == 25 );
        }

        THEN("his flags are set from the Flags column")
        {
            REQUIRE( luxor->IsRiding() );
            REQUIRE( luxor->IsAlive() );
            REQUIRE( luxor->IsRecruited() );
            REQUIRE( luxor->IsAllowedArmy() );
            REQUIRE( luxor->IsAllowedHide() );
            REQUIRE( luxor->IsAllowedHorse() );
            REQUIRE( luxor->IsAllowedMoonring() );
            REQUIRE_FALSE( luxor->IsHidden() );
        }

        THEN("his cross-references resolve to the correct entities")
        {
            REQUIRE( luxor->Carrying() == GetObject("OB_MOONRING") );
            REQUIRE( luxor->Foe() == GetCharacter("CH_DOOMDARK") );
            REQUIRE( luxor->Liege() == nullptr );
        }
    }

    GIVEN("Morkin")
    {
        auto morkin = GetCharacter("CH_MORKIN");
        REQUIRE( morkin != nullptr );

        THEN("his fields match characters.tsv")
        {
            REQUIRE( morkin->Longname() == "Morkin" );
            REQUIRE( morkin->Race() == RA_MORKIN );
            REQUIRE( morkin->Foe() == GetCharacter("CH_DOOMDARK") );
            REQUIRE( morkin->IsAllowedIcecrown() );
        }
    }
}

SCENARIO("TSV database resolves routenode connections correctly")
{
    TMEStep::NewStory();

    GIVEN("routenode RN_KEEP_OF_LORGRIM")
    {
        auto node = GetEntity<mxroutenode>("RN_KEEP_OF_LORGRIM");
        REQUIRE( node != nullptr );

        THEN("its paths resolve to the routenodes named in routenodes.tsv")
        {
            REQUIRE( node->Left() == GetEntity<mxroutenode>("RN_KEEP_OF_LORGRIM") );
            REQUIRE( node->Right() == GetEntity<mxroutenode>("RN_KEEP_OF_ITHRIL") );
        }
    }
}

SCENARIO("TSV database loads info tables and variables correctly")
{
    TMEStep::NewStory();

    GIVEN("terrain TN_CITADEL")
    {
        auto citadel = tme::mx->TerrainById(TN_CITADEL);
        REQUIRE( citadel != nullptr );

        THEN("its fields match terraininfo.tsv")
        {
            REQUIRE( citadel->Name() == "Citadel" );
            REQUIRE( citadel->Preposition() == "at" );
            REQUIRE( citadel->IsInteresting() );
        }
    }

    GIVEN("gender GD_MALE")
    {
        auto male = tme::mx->GenderById(GD_MALE);
        REQUIRE( male != nullptr );

        THEN("its pronouns match genderinfo.tsv")
        {
            REQUIRE( male->pronoun1 == "he" );
            REQUIRE( male->pronoun2 == "his" );
            REQUIRE( male->pronoun3 == "him" );
        }
    }

    GIVEN("a known variable")
    {
        THEN("its value matches variables.tsv")
        {
            REQUIRE( (int)tme::variables::sv_battle_default_char_energy_drain == 20 );
        }
    }
}
