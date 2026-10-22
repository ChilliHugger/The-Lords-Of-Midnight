//
//  memory_tests.cpp
//  midnight
//
//  What Luxor's lords remember of the enemy armies they have seen.
//

#include "../steps/common_steps.h"
#include "../../steps/map_steps.h"
#include "../../mocks/mocks_entity.h"

#include <filesystem>


namespace {

    const mxgridref k_here = mxgridref(10, 10);
    const mxgridref k_far = mxgridref(10, 14);
    const std::vector<u32> k_nolords;

    void SeeArmy(mxgridref loc, bool close, u32 total)
    {
        tme::mx->memory.SeeLocation(loc, close, total, k_nolords);
    }

    void ArmyAtLocationOnMap(mxgridref loc)
    {
        MapStep::ResetLocation(loc);
        tme::mx->gamemap->GetAt(loc).flags |= lf_army;
    }

}


SCENARIO("An army seen from a distance is remembered with an unknown size")
{
    TMEStep::NewStory();

    WHEN("a lord sees an army far away")
    {
        SeeArmy(k_far, false, 4000);

        THEN("it is remembered, but its size is not known")
        {
            auto fragment = tme::mx->memory.ArmyAt(k_far);
            REQUIRE( fragment != nullptr );
            REQUIRE( fragment->Size() == MS_UNKNOWN );
        }
    }
}

SCENARIO("An army seen close up is remembered by size")
{
    TMEStep::NewStory();

    auto total = GENERATE( std::make_pair(1u, MS_SMALL),
                           std::make_pair(1000u, MS_SMALL),
                           std::make_pair(1001u, MS_MEDIUM),
                           std::make_pair(5000u, MS_MEDIUM),
                           std::make_pair(5001u, MS_LARGE) );

    WHEN("a lord counts the army")
    {
        SeeArmy(k_here, true, total.first);

        THEN("it is small up to 1000, medium up to 5000, and large above that")
        {
            REQUIRE( tme::mx->memory.ArmyAt(k_here)->Size() == total.second );
        }
    }
}

SCENARIO("A count taken close up is kept when the army is later seen from afar")
{
    TMEStep::NewStory();

    GIVEN("a lord has counted a large army")
    {
        SeeArmy(k_here, true, 6000);

        WHEN("another lord only sees it from a distance")
        {
            SeeArmy(k_here, false, 6000);

            THEN("the size is still remembered")
            {
                REQUIRE( tme::mx->memory.ArmyAt(k_here)->Size() == MS_LARGE );
            }
        }
    }
}

SCENARIO("A later lord who gets close updates a distant sighting")
{
    TMEStep::NewStory();

    GIVEN("an army seen from a distance")
    {
        SeeArmy(k_here, false, 800);

        WHEN("another lord counts it")
        {
            SeeArmy(k_here, true, 800);

            THEN("the size is now known")
            {
                REQUIRE( tme::mx->memory.ArmyAt(k_here)->Size() == MS_SMALL );
            }
        }
    }
}

SCENARIO("A later close sighting replaces an earlier count")
{
    TMEStep::NewStory();

    GIVEN("a small army was counted")
    {
        SeeArmy(k_here, true, 500);

        WHEN("another lord counts a bigger army at the same place")
        {
            SeeArmy(k_here, true, 7000);

            THEN("there is still only one memory, and it is large")
            {
                REQUIRE( tme::mx->memory.Fragments().size() == 1 );
                REQUIRE( tme::mx->memory.ArmyAt(k_here)->Size() == MS_LARGE );
            }
        }
    }
}

SCENARIO("An empty location forgets the army that was remembered there")
{
    TMEStep::NewStory();

    GIVEN("a remembered army")
    {
        SeeArmy(k_here, true, 2000);
        SeeArmy(k_far, false, 2000);

        WHEN("a lord sees that location with no army in it")
        {
            SeeArmy(k_here, false, 0);

            THEN("that memory is deleted, and others are untouched")
            {
                REQUIRE( tme::mx->memory.ArmyAt(k_here) == nullptr );
                REQUIRE( tme::mx->memory.ArmyAt(k_far) != nullptr );
            }
        }
    }
}

SCENARIO("Memories fade with age")
{
    TMEStep::NewStory();

    GIVEN("an army seen today")
    {
        sv_days = 5;
        SeeArmy(k_here, true, 2000);

        WHEN("the memory is the default age")
        {
            sv_days += sv_character_default_memory_age;
            tme::mx->memory.Update();

            THEN("it is still remembered")
            {
                REQUIRE( tme::mx->memory.ArmyAt(k_here) != nullptr );
            }
        }

        WHEN("the memory is older than the default age")
        {
            sv_days += sv_character_default_memory_age + 1;
            tme::mx->memory.Update();

            THEN("it is forgotten")
            {
                REQUIRE( tme::mx->memory.ArmyAt(k_here) == nullptr );
            }
        }

        WHEN("it is seen again before it fades")
        {
            sv_days += sv_character_default_memory_age;
            SeeArmy(k_here, false, 2000);
            sv_days += 1;
            tme::mx->memory.Update();

            THEN("the memory is refreshed")
            {
                REQUIRE( tme::mx->memory.ArmyAt(k_here) != nullptr );
            }
        }
    }
}

SCENARIO("Remembered lords are keyed by lord and move when seen elsewhere")
{
    TMEStep::NewStory();

    GIVEN("a lord seen in one place")
    {
        tme::mx->memory.SeeLocation(k_here, true, 0, {7});

        WHEN("the same lord is seen somewhere else")
        {
            tme::mx->memory.SeeLocation(k_far, true, 0, {7});

            THEN("there is one memory of the lord, at the new place")
            {
                REQUIRE( tme::mx->memory.Fragments().size() == 1 );
                REQUIRE( tme::mx->memory.LordFragment(7)->location == k_far );
            }
        }

        WHEN("the place is seen again without the lord")
        {
            tme::mx->memory.SeeLocation(k_here, true, 0, k_nolords);

            THEN("the lord is forgotten")
            {
                REQUIRE( tme::mx->memory.LordFragment(7) == nullptr );
            }
        }

        WHEN("the lord is forgotten")
        {
            tme::mx->memory.ForgetLord(7);

            THEN("nothing is remembered")
            {
                REQUIRE( tme::mx->memory.Fragments().empty() );
            }
        }
    }
}

SCENARIO("A lord looking at a location tells Luxor about the enemy there")
{
    TMEStep::NewStory();

    GIVEN("a lord and a regiment of the enemy at the same place")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_NORTH);
        TMEStep::RegimentAtLocation(k_far, 1500);
        ArmyAtLocationOnMap(k_far);

        WHEN("the lord sees it from a distance")
        {
            lord->CanSeeLocation(k_far, false);

            THEN("it is remembered with an unknown size")
            {
                REQUIRE( tme::mx->memory.ArmyAt(k_far)->Size() == MS_UNKNOWN );
            }
        }

        WHEN("the lord sees it close up")
        {
            lord->CanSeeLocation(k_far, true);

            THEN("the regiment is counted")
            {
                REQUIRE( tme::mx->memory.ArmyAt(k_far)->total == 1500 );
                REQUIRE( tme::mx->memory.ArmyAt(k_far)->Size() == MS_MEDIUM );
            }
        }
    }
}

SCENARIO("An enemy stronghold adds its garrison to the armies there")
{
    TMEStep::NewStory();

    GIVEN("an enemy stronghold with a regiment outside its walls")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_NORTH);
        auto stronghold = TMEStep::StrongholdAtLocation(k_far, UT_WARRIORS, RA_ENEMY, 800, 100, 1275);
        REQUIRE( stronghold != nullptr );
        TMEStep::RegimentAtLocation(k_far, 700);
        ArmyAtLocationOnMap(k_far);
        MapStep::SetStronghold(k_far);

        WHEN("the lord counts it")
        {
            lord->CanSeeLocation(k_far, true);

            THEN("the garrison and the regiment are added together")
            {
                REQUIRE( tme::mx->memory.ArmyAt(k_far)->total == 1500 );
            }
        }

        WHEN("the lord only sees it from a distance")
        {
            lord->CanSeeLocation(k_far, false);

            THEN("the size is unknown")
            {
                REQUIRE( tme::mx->memory.ArmyAt(k_far)->Size() == MS_UNKNOWN );
            }
        }
    }
}

SCENARIO("A lord seeing an empty location corrects Luxor's memory")
{
    TMEStep::NewStory();

    GIVEN("a remembered army where there is none now")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_NORTH);
        SeeArmy(k_far, true, 3000);
        MapStep::ResetLocation(k_far);

        WHEN("a lord looks at the location")
        {
            lord->CanSeeLocation(k_far, false);

            THEN("the memory is deleted")
            {
                REQUIRE( tme::mx->memory.ArmyAt(k_far) == nullptr );
            }
        }
    }
}

SCENARIO("What Luxor's lords remember is saved and loaded")
{
    TMEStep::NewStory();

    GIVEN("some memories")
    {
        sv_days = 3;
        SeeArmy(k_here, true, 6000);
        SeeArmy(k_far, false, 100);
        tme::mx->memory.SeeLocation(mxgridref(1, 1), true, 0, {4});

        auto path = (std::filesystem::temp_directory_path() / "memory_tests.sav").string();

        WHEN("they are saved and loaded again")
        {
            {
                auto file = new chilli::os::file(path.c_str(), chilli::os::file::modeReadWrite | chilli::os::file::modeCreate);
                chilli::lib::archive ar(file, chilli::lib::archive::store | chilli::lib::archive::bNoFlushOnDelete);
                tme::mx->memory.Serialize(ar);
                ar.Close();
                delete file;
            }

            tme::mx->memory.Clear();
            REQUIRE( tme::mx->memory.Fragments().empty() );

            {
                auto file = new chilli::os::file(path.c_str(), chilli::os::file::modeRead);
                chilli::lib::archive ar(file, chilli::lib::archive::load | chilli::lib::archive::bNoFlushOnDelete);
                tme::mx->memory.Serialize(ar);
                ar.Close();
                delete file;
            }
            std::filesystem::remove(path);

            THEN("they are all back")
            {
                REQUIRE( tme::mx->memory.Fragments().size() == 3 );
                REQUIRE( tme::mx->memory.ArmyAt(k_here)->Size() == MS_LARGE );
                REQUIRE( tme::mx->memory.ArmyAt(k_here)->day == 3 );
                REQUIRE( tme::mx->memory.ArmyAt(k_far)->Size() == MS_UNKNOWN );
                REQUIRE( tme::mx->memory.LordFragment(4)->location == mxgridref(1, 1) );
            }
        }
    }
}

SCENARIO("Starting a new game forgets everything")
{
    GIVEN("a game with memories")
    {
        TMEStep::NewStory();
        SeeArmy(k_here, true, 2000);

        WHEN("a new game is started")
        {
            TMEStep::NewStory();

            THEN("nothing is remembered")
            {
                REQUIRE( tme::mx->memory.Fragments().empty() );
            }
        }
    }
}

SCENARIO("The map can get the memories through the interface")
{
    TMEStep::NewStory();

    GIVEN("a remembered army")
    {
        SeeArmy(k_here, true, 2000);

        WHEN("the interface is asked for the memories")
        {
            std::vector<tme::memory_item_t> items;
            TME_GetMemories(items);

            THEN("it is there")
            {
                REQUIRE( items.size() == 1 );
                REQUIRE( items[0].size == MS_MEDIUM );
            }
        }
    }
}

SCENARIO("A lord entering a location sees the enemy next to them")
{
    TMEStep::NewStory();

    GIVEN("a regiment next to a lord")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_NORTH);
        TMEStep::RegimentAtLocation(mxgridref(10, 9), 1500);
        tme::mx->gamemap->GetAt(mxgridref(10, 9)).flags |= lf_army;

        WHEN("the lord looks")
        {
            lord->Cmd_LookDir(DR_NORTH);

            THEN("the army is remembered")
            {
                REQUIRE( tme::mx->memory.ArmyAt(mxgridref(10, 9)) != nullptr );
            }
        }
    }
}

SCENARIO("A keep held by a lord Luxor has not recruited is remembered with its garrison")
{
    TMEStep::NewStory();

    GIVEN("a keep held by a lord who is not recruited")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_NORTH);
        auto keep = GetStronghold("SH_KEEP_OF_BLOOD");
        keep->Location(k_far);
        keep->TotalTroops(400);
        ArmyAtLocationOnMap(k_far);
        MapStep::SetStronghold(k_far);
        REQUIRE( keep->Occupier() != nullptr );
        keep->Occupier()->Flags().Reset(cf_recruited);

        WHEN("a lord counts it")
        {
            lord->CanSeeLocation(k_far, true);

            THEN("the garrison is remembered")
            {
                REQUIRE( tme::mx->memory.ArmyAt(k_far)->total == 400 );
            }
        }

        WHEN("the lord holding it has been recruited")
        {
            keep->Occupier()->Flags().Set(cf_recruited);
            lord->CanSeeLocation(k_far, true);

            THEN("Luxor already knows about it, so nothing is remembered")
            {
                REQUIRE( tme::mx->memory.ArmyAt(k_far) == nullptr );
            }
        }
    }
}

SCENARIO("A keep seen in the distance is remembered with an unknown garrison")
{
    TMEStep::NewStory();

    GIVEN("a keep three squares away over open ground")
    {
        const mxgridref keepLoc(10, 7);
        for ( int y = 5; y <= 11; y++ )
            MapStep::ResetLocation(mxgridref(10, y));
        MapStep::ResetLocation(keepLoc, TN_KEEP);

        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_NORTH);
        auto keep = GetStronghold("SH_KEEP_OF_BLOOD");
        keep->Location(keepLoc);
        keep->TotalTroops(400);
        ArmyAtLocationOnMap(keepLoc);
        MapStep::SetStronghold(keepLoc);
        keep->Occupier()->Flags().Reset(cf_recruited);
        tme::mx->memory.Clear();

        WHEN("the lord looks towards it")
        {
            lord->Cmd_LookDir(DR_NORTH);

            THEN("the keep is remembered, but not its size")
            {
                REQUIRE( tme::mx->memory.ArmyAt(keepLoc) != nullptr );
                REQUIRE( tme::mx->memory.ArmyAt(keepLoc)->Size() == MS_UNKNOWN );
            }
        }
    }
}

SCENARIO("A lord looking sees an army far off across the plains")
{
    TMEStep::NewStory();

    GIVEN("an army on the plains six squares away, beyond what is around the lord")
    {
        const mxgridref armyLoc(10, 4);
        for ( int y = 3; y <= 11; y++ )
            MapStep::ResetLocation(mxgridref(10, y));

        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_NORTH);
        TMEStep::RegimentAtLocation(armyLoc, 1200);
        ArmyAtLocationOnMap(armyLoc);
        tme::mx->memory.Clear();

        WHEN("the lord looks north")
        {
            lord->Cmd_LookDir(DR_NORTH);

            THEN("it is remembered, but not its size")
            {
                REQUIRE( tme::mx->memory.ArmyAt(armyLoc) != nullptr );
                REQUIRE( tme::mx->memory.ArmyAt(armyLoc)->Size() == MS_UNKNOWN );
            }
        }

        WHEN("the lord looks the other way")
        {
            lord->Cmd_LookDir(DR_SOUTH);

            THEN("it is not seen")
            {
                REQUIRE( tme::mx->memory.ArmyAt(armyLoc) == nullptr );
            }
        }
    }
}

SCENARIO("A lord only remembers what is in the direction they are looking")
{
    TMEStep::NewStory();

    GIVEN("armies on the plains, one ahead and one behind the lord")
    {
        for ( int y = 3; y <= 17; y++ )
            MapStep::ResetLocation(mxgridref(10, y));

        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, mxgridref(10, 10), DR_NORTH);
        TMEStep::RegimentAtLocation(mxgridref(10, 9), 1200);
        ArmyAtLocationOnMap(mxgridref(10, 9));
        tme::mx->memory.Clear();

        WHEN("the lord looks the other way, so the army is next to them but behind")
        {
            lord->Cmd_LookDir(DR_SOUTH);

            THEN("it is not remembered")
            {
                REQUIRE( tme::mx->memory.ArmyAt(mxgridref(10, 9)) == nullptr );
            }
        }

        WHEN("the lord looks at it")
        {
            lord->Cmd_LookDir(DR_NORTH);

            THEN("it is remembered, and counted as it is right in front of them")
            {
                REQUIRE( tme::mx->memory.ArmyAt(mxgridref(10, 9))->Size() == MS_MEDIUM );
            }
        }
    }
}

SCENARIO("A lord is only seen from the same square or when looked at directly")
{
    TMEStep::NewStory();

    GIVEN("a lord not recruited, and one of Luxor's lords looking north")
    {
        for ( int y = 3; y <= 17; y++ )
            MapStep::ResetLocation(mxgridref(10, y));

        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, mxgridref(10, 10), DR_NORTH);
        auto blood = TMEStep::PlaceLordAt("CH_BLOOD", mxgridref(10, 9), DR_NORTH);
        REQUIRE( blood->IsRememberable() );
        tme::mx->memory.Clear();

        WHEN("the lord looks directly at them")
        {
            lord->Cmd_LookDir(DR_NORTH);

            THEN("they are remembered")
            {
                REQUIRE( tme::mx->memory.LordFragment(blood->Id()) != nullptr );
            }
        }

        WHEN("they are further away, in the view")
        {
            blood->Location(mxgridref(10, 6));
            lord->Cmd_LookDir(DR_NORTH);

            THEN("they are not remembered")
            {
                REQUIRE( tme::mx->memory.LordFragment(blood->Id()) == nullptr );
            }
        }

        WHEN("they are in the same location")
        {
            blood->Location(mxgridref(10, 10));
            lord->Cmd_LookDir(DR_SOUTH);

            THEN("they are remembered, whichever way the lord faces")
            {
                REQUIRE( tme::mx->memory.LordFragment(blood->Id()) != nullptr );
            }
        }
    }
}

SCENARIO("A lord sees as far as the look screen draws, within the view and no further")
{
    TMEStep::NewStory();

    GIVEN("a lord looking north, on open plains")
    {
        for ( int y = 0; y <= 20; y++ )
            for ( int x = 5; x <= 20; x++ )
                MapStep::ResetLocation(mxgridref(x, y));

        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, mxgridref(10, 15), DR_NORTH);
        TMEStep::RegimentAtLocation(mxgridref(10, 15), 0);

        auto armyAt = [&](int x, int y) {
            TMEStep::RegimentAtLocation(mxgridref(x, y), 800);
            ArmyAtLocationOnMap(mxgridref(x, y));
            tme::mx->memory.Clear();
            lord->Cmd_LookDir(DR_NORTH);
            return tme::mx->memory.ArmyAt(mxgridref(x, y)) != nullptr;
        };

        THEN("an army six squares ahead is seen")        { REQUIRE( armyAt(10, 9) ); }
        THEN("an army seven squares ahead is too far")   { REQUIRE( !armyAt(10, 8) ); }
        THEN("an army ahead and to the side is seen")    { REQUIRE( armyAt(12, 11) ); }
        THEN("an army to the side is not in view")       { REQUIRE( !armyAt(14, 15) ); }
        THEN("an army beside the lord is not in view")   { REQUIRE( !armyAt(11, 15) ); }
        THEN("an army behind is not in view")            { REQUIRE( !armyAt(10, 18) ); }
    }
}

SCENARIO("A lord sees the Citadel of Shimeril from as far as the look screen draws it")
{
    TMEStep::NewStory();

    GIVEN("a lord looking at the citadel")
    {
        for ( int y = 37; y <= 42; y++ )
            MapStep::ResetLocation(mxgridref(29, y));
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, mxgridref(29, 37), DR_SOUTH);

        WHEN("it is six squares ahead over open ground")
        {
            tme::mx->memory.Clear();
            lord->Cmd_LookDir(DR_SOUTH);

            THEN("it is remembered, with an unknown garrison")
            {
                auto fragment = tme::mx->memory.ArmyAt(mxgridref(29, 43));
                REQUIRE( fragment != nullptr );
                REQUIRE( fragment->Size() == MS_UNKNOWN );
            }
        }
    }
}

SCENARIO("Looking marks as seen what the look screen draws, unless something is in the way")
{
    TMEStep::NewStory();

    GIVEN("a lord looking north, on open plains")
    {
        for ( int y = 0; y <= 20; y++ )
            for ( int x = 5; x <= 20; x++ )
                MapStep::ResetLocation(mxgridref(x, y));

        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, mxgridref(10, 15), DR_NORTH);
        MapStep::ResetLocation(mxgridref(10, 9), TN_KEEP);
        MapStep::ResetLocation(mxgridref(12, 11), TN_FOREST);
        MapStep::ResetLocation(mxgridref(14, 15), TN_MOUNTAIN);
        MapStep::ResetLocation(mxgridref(10, 18), TN_MOUNTAIN);
        MapStep::ResetLocation(mxgridref(10, 8), TN_MOUNTAIN);

        auto seen = [](int x, int y) { return tme::mx->gamemap->GetAt(mxgridref(x, y)).IsVisible(); };

        WHEN("the lord looks")
        {
            lord->Cmd_LookDir(DR_NORTH);

            THEN("high squares in the view, as far as six away, are seen over the plains")
            {
                REQUIRE( seen(10, 9) );
                REQUIRE( seen(12, 11) );
            }

            THEN("low ground that is far off is seen too, as nothing is in the way")
            {
                REQUIRE( seen(11, 10) );
            }

            THEN("squares out of the view, or beyond it, are not")
            {
                REQUIRE( !seen(10, 8) );
                REQUIRE( !seen(14, 15) );
                REQUIRE( !seen(10, 18) );
            }
        }

        WHEN("a forest is in the way")
        {
            MapStep::ResetLocation(mxgridref(10, 12), TN_FOREST);
            lord->Cmd_LookDir(DR_NORTH);

            THEN("what is behind it is not seen, and the forest itself is")
            {
                REQUIRE( seen(10, 12) );
                REQUIRE( !seen(10, 9) );
            }
        }
    }
}

SCENARIO("A lord does not see an army behind a forest")
{
    TMEStep::NewStory();

    GIVEN("an army on the plains with a forest between it and the lord")
    {
        for ( int y = 0; y <= 20; y++ )
            for ( int x = 5; x <= 20; x++ )
                MapStep::ResetLocation(mxgridref(x, y));

        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, mxgridref(10, 15), DR_NORTH);
        TMEStep::RegimentAtLocation(mxgridref(10, 11), 800);
        ArmyAtLocationOnMap(mxgridref(10, 11));
        tme::mx->memory.Clear();

        WHEN("it is in the open")
        {
            lord->Cmd_LookDir(DR_NORTH);

            THEN("it is seen")
            {
                REQUIRE( tme::mx->memory.ArmyAt(mxgridref(10, 11)) != nullptr );
            }
        }

        WHEN("there is a forest in the way")
        {
            MapStep::ResetLocation(mxgridref(10, 13), TN_FOREST);
            lord->Cmd_LookDir(DR_NORTH);

            THEN("it is not seen")
            {
                REQUIRE( tme::mx->memory.ArmyAt(mxgridref(10, 11)) == nullptr );
            }
        }
    }
}

SCENARIO("Rolling downs do not hide a citadel, but a forest hides a keep")
{
    TMEStep::NewStory();

    GIVEN("a lord looking east over downs")
    {
        for ( int x = 5; x <= 20; x++ )
            for ( int y = 5; y <= 15; y++ )
                MapStep::ResetLocation(mxgridref(x, y), TN_DOWNS);

        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, mxgridref(8, 10), DR_EAST);
        MapStep::ResetLocation(mxgridref(14, 10), TN_CITADEL);
        MapStep::ResetLocation(mxgridref(14, 12), TN_KEEP);
        MapStep::ResetLocation(mxgridref(11, 11), TN_FOREST);

        lord->Cmd_LookDir(DR_EAST);

        auto seen = [](int x, int y) { return tme::mx->gamemap->GetAt(mxgridref(x, y)).IsVisible(); };

        THEN("the citadel six squares away is seen")
        {
            REQUIRE( seen(14, 10) );
        }

        THEN("a keep behind a forest is not")
        {
            REQUIRE( !seen(14, 12) );
        }
    }
}

SCENARIO("A lord not recruited with an army, looked at directly, gives the map both")
{
    TMEStep::NewStory();

    GIVEN("Lord Blood with a regiment next to a lord")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_NORTH);
        auto blood = TMEStep::PlaceLordAt("CH_BLOOD", mxgridref(10, 9), DR_NORTH);
        TMEStep::LordIsNotRecruited("CH_BLOOD");
        TMEStep::RegimentAtLocation(mxgridref(10, 9), 1200);
        tme::mx->gamemap->GetAt(mxgridref(10, 9)).flags |= lf_army;
        tme::mx->memory.Clear();

        WHEN("the lord looks at him")
        {
            lord->Cmd_LookDir(DR_NORTH);

            THEN("the lord and the counted army are remembered at the same place")
            {
                std::vector<tme::memory_item_t> items;
                TME_GetMemories(items);
                REQUIRE( items.size() == 2 );
                for ( auto& item : items ) {
                    REQUIRE( item.location == mxgridref(10, 9) );
                    if ( item.type == MT_ARMY )
                        REQUIRE( item.size != MS_UNKNOWN );
                }
            }
        }
    }
}

SCENARIO("A dead lord remembers nothing")
{
    TMEStep::NewStory();

    GIVEN("a recruited lord who has died, next to a regiment")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_NORTH);
        TMEStep::RegimentAtLocation(mxgridref(10, 9), 1500);
        ArmyAtLocationOnMap(mxgridref(10, 9));
        lord->Flags().Reset(cf_alive);
        tme::mx->memory.Clear();

        WHEN("they look")
        {
            lord->Cmd_LookDir(DR_NORTH);

            THEN("nothing is remembered")
            {
                REQUIRE( tme::mx->memory.Fragments().empty() );
            }
        }

        WHEN("the dead lord is asked to see the location")
        {
            lord->CanSeeLocation(mxgridref(10, 9), true);

            THEN("nothing is remembered")
            {
                REQUIRE( tme::mx->memory.Fragments().empty() );
            }
        }
    }
}

SCENARIO("A dead lord reveals nothing of the map")
{
    TMEStep::NewStory();

    GIVEN("a recruited lord who has died")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_NORTH);
        MapStep::ResetLocation(mxgridref(10, 9));
        lord->Flags().Reset(cf_alive);

        WHEN("they look")
        {
            lord->Cmd_LookDir(DR_NORTH);

            THEN("the square ahead is not marked as seen")
            {
                REQUIRE( !tme::mx->gamemap->GetAt(mxgridref(10, 9)).IsVisible() );
            }
        }
    }
}
