//
//  object_processor_tests.cpp
//  midnight
//

#include "../steps/common_steps.h"
#include "../../steps/map_steps.h"
#include "../../mocks/mocks_entity.h"
#include "../../../Source/tme/utils/savegamemapping.h"


namespace {

    const mxgridref k_here = mxgridref(10, 10);

    constexpr LPCSTR ob_wolfslayer = "OB_WOLFSLAYER";
    constexpr LPCSTR ob_dragonslayer = "OB_DRAGONSLAYER";

}


SCENARIO("What a lord carries is carried by that lord")
{
    TMEStep::NewStory();

    GIVEN("a lord with empty hands")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_NORTH);
        lord->CarryObject(nullptr);
        auto sword = GetObject(ob_wolfslayer);

        WHEN("they are given an object to carry")
        {
            lord->CarryObject(sword);

            THEN("the object knows who carries it")
            {
                REQUIRE( sword->CarriedBy() == lord );
                REQUIRE( tme::mx->objectprocessor->WhoHasObject(sword) == lord );
            }

            AND_WHEN("they are given another")
            {
                auto other = GetObject(ob_dragonslayer);
                lord->CarryObject(other);

                THEN("the first is carried by nobody")
                {
                    REQUIRE( sword->CarriedBy() == nullptr );
                    REQUIRE( other->CarriedBy() == lord );
                }
            }

            AND_WHEN("they are carrying nothing again")
            {
                lord->CarryObject(nullptr);

                THEN("nobody carries the object")
                {
                    REQUIRE( tme::mx->objectprocessor->WhoHasObject(sword) == nullptr );
                }
            }
        }
    }
}

SCENARIO("A lord dropping an object leaves it where they stand")
{
    TMEStep::NewStory();

    GIVEN("a lord carrying a sword, on empty ground")
    {
        MapStep::ResetLocation(k_here);
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_NORTH);
        auto sword = GetObject(ob_wolfslayer);
        lord->CarryObject(sword);

        WHEN("they drop it")
        {
            lord->Cmd_DropObject();

            THEN("nobody carries it, and it lies there to be found")
            {
                REQUIRE( lord->Carrying() == nullptr );
                REQUIRE( tme::mx->objectprocessor->WhoHasObject(sword) == nullptr );
                REQUIRE( tme::mx->objectprocessor->FindObjectAtLocation(k_here) == sword );
            }
        }
    }
}

SCENARIO("Lords can hand over and swap what they carry")
{
    TMEStep::NewStory();

    GIVEN("two lords, one with a sword and one with a different sword")
    {
        auto first = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_NORTH);
        auto second = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_NORTH);
        auto wolfslayer = GetObject(ob_wolfslayer);
        auto dragonslayer = GetObject(ob_dragonslayer);
        first->CarryObject(wolfslayer);
        second->CarryObject(nullptr);

        WHEN("one gives to the other")
        {
            tme::mx->objectprocessor->Give(first, second);

            THEN("the other has it, and the first has nothing")
            {
                REQUIRE( first->Carrying() == nullptr );
                REQUIRE( second->Carrying() == wolfslayer );
                REQUIRE( tme::mx->objectprocessor->WhoHasObject(wolfslayer) == second );
            }
        }

        WHEN("they swap")
        {
            second->CarryObject(dragonslayer);
            tme::mx->objectprocessor->Exchange(first, second);

            THEN("each has what the other had, and each object knows its carrier")
            {
                REQUIRE( first->Carrying() == dragonslayer );
                REQUIRE( second->Carrying() == wolfslayer );
                REQUIRE( tme::mx->objectprocessor->WhoHasObject(dragonslayer) == first );
                REQUIRE( tme::mx->objectprocessor->WhoHasObject(wolfslayer) == second );
            }
        }
    }
}

SCENARIO("A destroyed object is no longer carried")
{
    TMEStep::NewStory();

    GIVEN("a lord carrying a sword")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_NORTH);
        auto sword = GetObject(ob_wolfslayer);
        lord->CarryObject(sword);

        WHEN("it is destroyed")
        {
            tme::mx->objectprocessor->Destroy(sword);

            THEN("the lord's hands are empty")
            {
                REQUIRE( lord->Carrying() == nullptr );
                REQUIRE( sword->IsDestroyed() );
            }
        }
    }
}

SCENARIO("Objects are not artefacts in Lords of Midnight")
{
    TMEStep::NewStory();

    THEN("none of them is")
    {
        REQUIRE_FALSE( GetObject(ob_wolfslayer)->IsArtefact() );
    }
}

SCENARIO("A saved game from before lords kept up with what they carry has its carriers repaired")
{
    TMEStep::NewStory();

    GIVEN("a moon ring that still names Luxor as its carrier, though Morkin has it now")
    {
        auto luxor = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_NORTH);
        auto morkin = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_NORTH);
        auto moonring = GetObject("OB_MOONRING");
        auto icecrown = GetObject("OB_ICECROWN");
        auto towerOfDoom = icecrown->CarriedBy();

        luxor->Carrying(nullptr);
        morkin->Carrying(moonring);
        moonring->CarriedBy(luxor);
        REQUIRE( tme::mx->objectprocessor->WhoHasObject(moonring) == luxor );

        WHEN("a game saved before this is loaded")
        {
            tme::mx->scenario->updateAfterLoad(17);

            THEN("the moon ring is Morkin's, and an object held by a place stays with it")
            {
                REQUIRE( tme::mx->objectprocessor->WhoHasObject(moonring) == morkin );
                REQUIRE( tme::mx->scenario->CurrentMoonringWearer() == morkin );
                REQUIRE( icecrown->CarriedBy() == towerOfDoom );
            }
        }

        WHEN("a game saved since then is loaded")
        {
            tme::mx->scenario->updateAfterLoad(18);

            THEN("what it saved is trusted")
            {
                REQUIRE( tme::mx->objectprocessor->WhoHasObject(moonring) == luxor );
            }
        }
    }

    GIVEN("a moon ring left lying on the ground, still naming the lord who dropped it")
    {
        auto luxor = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_NORTH);
        auto moonring = GetObject("OB_MOONRING");
        luxor->Carrying(nullptr);
        moonring->CarriedBy(luxor);

        WHEN("a game saved before this is loaded")
        {
            tme::mx->scenario->updateAfterLoad(17);

            THEN("nobody has it")
            {
                REQUIRE( tme::mx->objectprocessor->WhoHasObject(moonring) == nullptr );
                REQUIRE_FALSE( moonring->IsCarried() );
            }
        }
    }
}
