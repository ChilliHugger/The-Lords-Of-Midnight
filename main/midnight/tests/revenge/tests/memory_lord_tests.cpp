//
//  memory_lord_tests.cpp
//  revenge
//
//  Lords of the enemy that Luxor's lords have seen.
//

#include "../steps/common_steps.h"
#include "../../steps/map_steps.h"
#include "../../mocks/mocks_entity.h"


namespace {

    const mxgridref k_here = mxgridref(10, 10);
    const mxgridref k_there = mxgridref(10, 11);

    // a lord of Doomdark's, not under Luxor's command
    constexpr LPCSTR ch_enemy = "CH_IMGARORN";

}


SCENARIO("A lord not under Luxor's command is remembered when seen")
{
    TMEStep::NewStory();

    GIVEN("a lord, and another lord of the enemy standing next to them")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_NORTH);
        auto enemy = TMEStep::PlaceLordAt(ch_enemy, k_there, DR_NORTH);
        REQUIRE( enemy->IsRememberable() );

        WHEN("the lord sees the location")
        {
            lord->Cmd_LookDir(DR_SOUTH);

            THEN("the enemy lord is remembered where they stand")
            {
                REQUIRE( tme::mx->memory.LordFragment(enemy->Id()) != nullptr );
                REQUIRE( tme::mx->memory.LordFragment(enemy->Id())->location == k_there );
            }
        }
    }
}

SCENARIO("A lord of the enemy that has moved on is no longer remembered at the old place")
{
    TMEStep::NewStory();

    GIVEN("a remembered enemy lord")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_NORTH);
        auto enemy = TMEStep::PlaceLordAt(ch_enemy, k_there, DR_NORTH);
        lord->Cmd_LookDir(DR_SOUTH);

        WHEN("the enemy lord leaves and the location is seen again")
        {
            enemy->Location(mxgridref(20, 20));
            lord->Cmd_LookDir(DR_SOUTH);

            THEN("the memory is deleted")
            {
                REQUIRE( tme::mx->memory.LordFragment(enemy->Id()) == nullptr );
            }
        }
    }
}

SCENARIO("Luxor's own lords are not remembered")
{
    TMEStep::NewStory();

    GIVEN("two of Luxor's lords")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_NORTH);
        auto friend_ = TMEStep::PlaceLordAt(TMEStep::ch_rorthron, k_there, DR_NORTH);
        TMEStep::LordIsRecruited(TMEStep::ch_rorthron);

        WHEN("one sees the other")
        {
            lord->Cmd_LookDir(DR_SOUTH);

            THEN("nothing is remembered")
            {
                REQUIRE( tme::mx->memory.LordFragment(friend_->Id()) == nullptr );
            }
        }
    }
}

SCENARIO("A lord recruited by one of Luxor's lords is forgotten")
{
    TMEStep::NewStory();

    GIVEN("a remembered enemy lord")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_NORTH);
        auto enemy = TMEStep::PlaceLordAt(ch_enemy, k_there, DR_NORTH);
        tme::mx->memory.SeeLocation(k_there, true, 0, {enemy->Id()});

        WHEN("Luxor recruits them")
        {
            enemy->Recruited(lord);

            THEN("the memory is deleted")
            {
                REQUIRE( tme::mx->memory.LordFragment(enemy->Id()) == nullptr );
            }
        }
    }
}

SCENARIO("A lord recruited by a lord of the enemy keeps fading instead")
{
    TMEStep::NewStory();

    GIVEN("a remembered lord and a recruiter that is not one of Luxor's")
    {
        auto recruiter = TMEStep::PlaceLordAt(ch_enemy, k_here, DR_NORTH);
        auto other = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_there, DR_NORTH);
        tme::mx->memory.SeeLocation(k_there, true, 0, {other->Id()});

        WHEN("the enemy recruits them")
        {
            other->Recruited(recruiter);

            THEN("the memory stays until it fades")
            {
                REQUIRE( tme::mx->memory.LordFragment(other->Id()) != nullptr );
            }
        }
    }
}

SCENARIO("A lord of the enemy that dies is forgotten")
{
    TMEStep::NewStory();

    GIVEN("a remembered enemy lord")
    {
        auto enemy = TMEStep::PlaceLordAt(ch_enemy, k_there, DR_NORTH);
        tme::mx->memory.SeeLocation(k_there, true, 0, {enemy->Id()});

        WHEN("they die")
        {
            enemy->Cmd_Dead();

            THEN("the memory is deleted")
            {
                REQUIRE( tme::mx->memory.LordFragment(enemy->Id()) == nullptr );
            }
        }
    }
}
