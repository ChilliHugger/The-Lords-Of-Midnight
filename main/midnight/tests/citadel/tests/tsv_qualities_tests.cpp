//
//  tsv_qualities_tests.cpp
//  citadel
//
#include "../../steps/tme_steps.h"
#include "../../../Source/tme/tsv/tsvflags.h"

namespace {

    // a lord with neither warrior attribute — the ordinary case, whoever it happens to be
    mxcharacter* AnOrdinaryLord()
    {
        for ( auto character : tme::mx->objCharacters ) {
            if ( character->Qualities() != qf_none
                 && !character->HasQuality(qf_mightywarrior)
                 && !character->HasQuality(qf_feeblewarrior) )
                return character;
        }
        return nullptr;
    }
}

SCENARIO("The Qualities column is read")
{
    TMEStep::NewStory();

    GIVEN("the Citadel's 154 characters")
    {
        THEN("every one of them carries attributes")
        {
            // 154/154 rows are populated in the file. A zero here means the column is not in
            // the loader's list, not that a lord is featureless.
            for ( auto character : tme::mx->objCharacters ) {
                REQUIRE( character->Qualities() != qf_none );
            }
        }

        THEN("the words parse to the attributes the file spells out")
        {
            auto morkin = GetCharacter("CH_MORKIN");
            REQUIRE( morkin != nullptr );
            // characters.tsv: BOLD+TIRELESS+RELIABLE+GALLANT
            REQUIRE( morkin->HasQuality(qf_bold) );
            REQUIRE( morkin->HasQuality(qf_tireless) );
            REQUIRE( morkin->HasQuality(qf_reliable) );
            REQUIRE( morkin->HasQuality(qf_gallant) );
            REQUIRE_FALSE( morkin->HasQuality(qf_cowardly) );
        }
    }
}

SCENARIO("Combat strength comes from the attributes, not from a flat number")
{
    TMEStep::NewStory();

    GIVEN("Zenethor the Strong, the one lord who is both a superb leader and a mighty warrior")
    {
        auto zenethor = GetCharacter("CH_ZENETHOR");
        REQUIRE( zenethor != nullptr );
        REQUIRE( zenethor->HasQuality(qf_mightywarrior) );
        REQUIRE( zenethor->HasQuality(qf_superbleader) );

        THEN("he fights with the strength of a hundred")
        {
            REQUIRE( zenethor->FightStrength() == 100 );
        }

        AND_THEN("he is the only character holding both attributes")
        {
            // The strategy guide's claim, checked against the shipped data. If this ever fails
            // the data has moved, not the code.
            s32 both = 0;
            for ( auto character : tme::mx->objCharacters ) {
                if ( character->HasQuality(qf_mightywarrior)
                     && character->HasQuality(qf_superbleader) )
                    both++;
            }
            REQUIRE( both == 1 );
        }
    }

    GIVEN("a lord with neither warrior attribute")
    {
        auto lord = AnOrdinaryLord();
        REQUIRE( lord != nullptr );

        THEN("he fights with the strength of fifty")
        {
            REQUIRE( lord->FightStrength() == 50 );
        }
    }

    GIVEN("the one Feeble Warrior in the cast")
    {
        mxcharacter* feeble = nullptr;
        for ( auto character : tme::mx->objCharacters ) {
            if ( character->HasQuality(qf_feeblewarrior) )
                feeble = character;
        }
        REQUIRE( feeble != nullptr );

        THEN("he fights with the strength of twenty five")
        {
            REQUIRE( feeble->FightStrength() == 25 );
        }
    }
}

SCENARIO("Quality words parse to the qf_ bits")
{
    GIVEN("words joined with '+', in any case")
    {
        THEN("each maps to its own bit")
        {
            REQUIRE( tme::ParseCharacterQualities("bold+Gallant") == (qf_bold | qf_gallant) );
        }
    }

    GIVEN("a word the table does not know")
    {
        THEN("it contributes nothing rather than colliding with a real attribute")
        {
            REQUIRE( tme::ParseCharacterQualities("NOTANATTRIBUTE") == qf_none );
        }
    }
}
