//
//  qualities_tests.cpp
//  citadel
//
//  The 1995 personality attributes — the `Qualities` column that shipped in the Citadel's
//  characters.tsv and that nothing read until now. Two things are pinned here:
//
//    1. the column is actually WIRED (it was present in the file and absent from the loader's
//       column list, which is a failure that looks exactly like "everyone is ordinary");
//    2. the strength the design specifies falls out of it — 100 for a Mighty Warrior, 25 for a
//       Feeble Warrior, 50 for everyone else.
//
//  The affinity arithmetic is tested against the rule as the strategy guide states it, because
//  it is what the recruitment score will be built on and it is pure enough to pin exactly.
//
#include "../../steps/tme_steps.h"
#include "../../../Source/tme/tsv/tsvflags.h"

namespace {

    u64 Q ( const char* word )
    {
        return tme::ParseCharacterQualities(word);
    }

    // a lord with neither warrior attribute — the ordinary case, whoever it happens to be
    mxcharacter* AnOrdinaryLord()
    {
        for ( auto character : tme::mx->objCharacters ) {
            if ( character->Qualities() != 0
                 && !character->HasQuality(Q("MIGHTYWARRIOR"))
                 && !character->HasQuality(Q("FEEBLEWARRIOR")) )
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
                REQUIRE( character->Qualities() != 0 );
            }
        }

        THEN("the words parse to the attributes the file spells out")
        {
            auto morkin = GetCharacter("CH_MORKIN");
            REQUIRE( morkin != nullptr );
            // characters.tsv: BOLD+TIRELESS+RELIABLE+GALLANT
            REQUIRE( morkin->HasQuality(Q("BOLD")) );
            REQUIRE( morkin->HasQuality(Q("TIRELESS")) );
            REQUIRE( morkin->HasQuality(Q("RELIABLE")) );
            REQUIRE( morkin->HasQuality(Q("GALLANT")) );
            REQUIRE_FALSE( morkin->HasQuality(Q("COWARDLY")) );
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
        REQUIRE( zenethor->HasQuality(Q("MIGHTYWARRIOR")) );
        REQUIRE( zenethor->HasQuality(Q("SUPERBLEADER")) );

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
                if ( character->HasQuality(Q("MIGHTYWARRIOR"))
                     && character->HasQuality(Q("SUPERBLEADER")) )
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
            if ( character->HasQuality(Q("FEEBLEWARRIOR")) )
                feeble = character;
        }
        REQUIRE( feeble != nullptr );

        THEN("he fights with the strength of twenty five")
        {
            REQUIRE( feeble->FightStrength() == 25 );
        }
    }
}

SCENARIO("Attribute affinity scores agreement against opposition")
{
    // No story needed — this is arithmetic over two bit sets.

    GIVEN("two characters who agree on one attribute and differ on nothing")
    {
        const u64 a = Q("BRAVE+GALLANT");
        const u64 b = Q("BRAVE+SOLITARY");

        THEN("they score one point")
        {
            REQUIRE( tme::QualityAffinity(a, b) == 1 );
        }
    }

    GIVEN("two characters holding opposite attributes")
    {
        const u64 brave    = Q("BRAVE");
        const u64 cowardly = Q("COWARDLY");

        THEN("the point is taken away, whichever way round they are asked")
        {
            REQUIRE( tme::QualityAffinity(brave, cowardly) == -1 );
            REQUIRE( tme::QualityAffinity(cowardly, brave) == -1 );
        }
    }

    GIVEN("attributes with nothing to do with one another")
    {
        THEN("they score nothing, as the design says they should")
        {
            REQUIRE( tme::QualityAffinity(Q("BRAVE"), Q("TALKATIVE")) == 0 );
        }
    }

    GIVEN("an attribute with no opposite among the sixty two")
    {
        THEN("it can be shared but can never count against anyone")
        {
            REQUIRE( tme::QualityAffinity(Q("SUPERBLEADER"), Q("SUPERBLEADER")) == 1 );
            REQUIRE( tme::QualityAffinity(Q("SUPERBLEADER"), Q("MAD")) == 0 );
        }
    }

    GIVEN("agreement and opposition in the same pair of characters")
    {
        const u64 a = Q("BRAVE+LOYAL+PATIENT");
        const u64 b = Q("BRAVE+TREACHEROUS+PATIENT");

        THEN("they cancel — two shared, one opposed")
        {
            REQUIRE( tme::QualityAffinity(a, b) == 1 );
        }
    }

    GIVEN("a word the table does not know")
    {
        THEN("it contributes nothing rather than colliding with a real attribute")
        {
            REQUIRE( Q("NOTANATTRIBUTE") == 0 );
        }
    }
}
