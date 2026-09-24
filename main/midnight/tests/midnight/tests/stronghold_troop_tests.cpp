//
//  stronghold_troop_tests.cpp
//  midnight
//

#include "../steps/common_steps.h"
#include "../../steps/map_steps.h"
#include "../../mocks/mocks_entity.h"


namespace {

    // Has no Owner/Occupier set in the TSV data, so MakeChangeSides tests
    // are not entangled with its "owner keeps it free" special case.
    constexpr LPCSTR sh_fadrath = "SH_KEEP_OF_FADRATH";

}


SCENARIO("A stronghold cannot be reduced below its minimum troops")
{
    TMEStep::NewStory();

    GIVEN("a stronghold with 500 troops and a minimum of 100")
    {
        auto stronghold = GetStronghold(sh_fadrath);
        stronghold->TotalTroops(500);
        stronghold->MinTroops(100);

        WHEN("more than it can spare is removed")
        {
            auto removed = stronghold->Remove(RA_FREE, UT_WARRIORS, 600);

            THEN("only enough to reach the minimum is actually removed")
            {
                REQUIRE( removed == 400 );
                REQUIRE( stronghold->TotalTroops() == 100 );
            }
        }
    }
}

SCENARIO("A stronghold gives up the full amount requested when it can spare it")
{
    TMEStep::NewStory();

    GIVEN("a stronghold with plenty of troops to spare")
    {
        auto stronghold = GetStronghold(sh_fadrath);
        stronghold->TotalTroops(500);
        stronghold->MinTroops(100);

        WHEN("a modest amount is removed")
        {
            auto removed = stronghold->Remove(RA_FREE, UT_WARRIORS, 50);

            THEN("the full amount is removed")
            {
                REQUIRE( removed == 50 );
                REQUIRE( stronghold->TotalTroops() == 450 );
            }
        }
    }
}

SCENARIO("A stronghold cannot be filled beyond its maximum troops")
{
    TMEStep::NewStory();

    GIVEN("a stronghold with only a little room to spare")
    {
        auto stronghold = GetStronghold(sh_fadrath);
        stronghold->TotalTroops(900);
        stronghold->MaxTroops(1000);

        WHEN("more than it has room for is added")
        {
            auto added = stronghold->Add(RA_FREE, UT_WARRIORS, 300);

            THEN("only enough to reach the maximum is actually added")
            {
                REQUIRE( added == 100 );
                REQUIRE( stronghold->TotalTroops() == 1000 );
            }
        }
    }
}

SCENARIO("A stronghold accepts the full amount offered when it has room")
{
    TMEStep::NewStory();

    GIVEN("a stronghold with plenty of room to spare")
    {
        auto stronghold = GetStronghold(sh_fadrath);
        stronghold->TotalTroops(500);
        stronghold->MaxTroops(1200);

        WHEN("a modest amount is added")
        {
            auto added = stronghold->Add(RA_FREE, UT_WARRIORS, 50);

            THEN("the full amount is added")
            {
                REQUIRE( added == 50 );
                REQUIRE( stronghold->TotalTroops() == 550 );
            }
        }
    }
}

SCENARIO("An emptied stronghold respawns to the default empty level")
{
    TMEStep::NewStory();

    GIVEN("a stronghold with no troops left")
    {
        auto stronghold = GetStronghold(sh_fadrath);
        stronghold->TotalTroops(0);

        WHEN("it is checked for being empty")
        {
            stronghold->CheckForZero();

            THEN("it respawns to the default empty level")
            {
                REQUIRE( stronghold->TotalTroops() == (u32) sv_stronghold_default_empty );
            }
        }
    }
}

SCENARIO("A non-empty stronghold is untouched by the empty check")
{
    TMEStep::NewStory();

    GIVEN("a stronghold that still has troops")
    {
        auto stronghold = GetStronghold(sh_fadrath);
        stronghold->TotalTroops(50);

        WHEN("it is checked for being empty")
        {
            stronghold->CheckForZero();

            THEN("its troop level is unchanged")
            {
                REQUIRE( stronghold->TotalTroops() == 50 );
            }
        }
    }
}

SCENARIO("MakeChangeSides hands a stronghold to the new occupying race")
{
    TMEStep::NewStory();

    GIVEN("a free stronghold, with no army-carrying occupier taking it")
    {
        auto stronghold = GetStronghold(sh_fadrath);
        REQUIRE( stronghold->OccupyingRace() == RA_FREE );

        WHEN("it changes sides to the enemy")
        {
            stronghold->MakeChangeSides(RA_DOOMGUARD, nullptr);

            THEN("it is restaffed at the new race's stronghold startup level")
            {
                REQUIRE( stronghold->OccupyingRace() == RA_DOOMGUARD );
                REQUIRE( stronghold->Occupier() == nullptr );
                REQUIRE( stronghold->TotalTroops() == tme::mx->RaceById(RA_DOOMGUARD)->StrongholdStartups() );
            }
        }
    }
}

SCENARIO("MakeChangeSides uses the new occupier's own race when they carry an army")
{
    TMEStep::NewStory();

    GIVEN("an enemy-occupied stronghold and an army-carrying lord taking it")
    {
        auto stronghold = GetStronghold(sh_fadrath);
        auto luxor = GetCharacter(TMEStep::ch_luxor);
        REQUIRE( luxor->IsAllowedArmy() );

        // start from enemy hands, so the new occupying race actually differs
        stronghold->MakeChangeSides(RA_DOOMGUARD, nullptr);
        REQUIRE( stronghold->OccupyingRace() == RA_DOOMGUARD );

        WHEN("the enemy race is requested, but the new occupier carries an army")
        {
            stronghold->MakeChangeSides(RA_DOOMGUARD, luxor);

            THEN("the stronghold instead takes on the new occupier's own race")
            {
                REQUIRE( stronghold->OccupyingRace() == luxor->Race() );
                REQUIRE( stronghold->Occupier() == luxor );
            }
        }
    }
}

SCENARIO("MakeChangeSides does nothing when the new race already matches")
{
    TMEStep::NewStory();

    GIVEN("a free stronghold")
    {
        auto stronghold = GetStronghold(sh_fadrath);
        stronghold->TotalTroops(500);
        REQUIRE( stronghold->OccupyingRace() == RA_FREE );

        WHEN("it 'changes sides' to the race it already belongs to")
        {
            stronghold->MakeChangeSides(RA_FREE, nullptr);

            THEN("its troop level and occupier are unchanged")
            {
                REQUIRE( stronghold->OccupyingRace() == RA_FREE );
                REQUIRE( stronghold->TotalTroops() == 500 );
                REQUIRE( stronghold->Occupier() == nullptr );
            }
        }
    }
}
