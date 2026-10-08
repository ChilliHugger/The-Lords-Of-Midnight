//
//  respawn_tests.cpp
//  midnight
//

#include "../steps/common_steps.h"
#include "../../steps/map_steps.h"
#include "../../mocks/mocks_entity.h"


namespace {

    const mxgridref k_here = mxgridref(1, 1);

    lom_map_regenerator* Regenerator()
    {
        return static_cast<lom_map_regenerator*>(tme::mx->mapregenerator);
    }

    void FlagRespawn(mxgridref loc)
    {
        tme::mx->gamemap->GetAt(loc).flags |= lf_respawn;
    }

    void RegenerateOnDay(u32 day)
    {
        tme::sv_days = day;
        tme::mx->mapregenerator->process();
    }

    // a day on which the location is due to respawn
    u32 DueDay(mxgridref loc, u32 period)
    {
        auto width = (u32)tme::mx->gamemap->Size().cx;
        return (period - ((u32)(loc.x + loc.y * width) % period)) % period;
    }

}

SCENARIO("Respawn days follow the difficulty")
{
    REQUIRE( Regenerator()->RespawnDays(DF_EASY) == 2 );
    REQUIRE( Regenerator()->RespawnDays(DF_MEDIUM) == 5 );
    REQUIRE( Regenerator()->RespawnDays(DF_HARD) == 10 );
    REQUIRE( Regenerator()->RespawnDays(DF_NORMAL) == 5 );
}

SCENARIO("An emptied respawn location gets a thing back on its due day")
{
    TMEStep::NewStory(RF_LOM_REPLENISH_THINGS);

    GIVEN("a flagged plains location with no object")
    {
        MapStep::ResetLocation(k_here, TN_PLAINS);
        FlagRespawn(k_here);
        MapStep::ClearObjectFromLocation(k_here);

        WHEN("it is replenished on its due day")
        {
            RegenerateOnDay(DueDay(k_here, 5));

            THEN("a thing from the plains table is back")
            {
                REQUIRE( MapStep::GetObjectAtLocation(k_here) == Regenerator()->RespawnObject(TN_PLAINS, k_here) );
                REQUIRE( MapStep::GetObjectAtLocation(k_here) != OB_NONE );
            }
        }

        WHEN("it is replenished on a day it is not due")
        {
            RegenerateOnDay(DueDay(k_here, 5) + 1);

            THEN("it stays empty")
            {
                REQUIRE( MapStep::GetObjectAtLocation(k_here) == OB_NONE );
            }
        }
    }
}

SCENARIO("A location without the respawn flag is never replenished")
{
    TMEStep::NewStory(RF_LOM_REPLENISH_THINGS);

    GIVEN("an unflagged empty location")
    {
        MapStep::ResetLocation(k_here, TN_PLAINS);
        MapStep::ClearObjectFromLocation(k_here);

        WHEN("it is replenished on every day of the cycle")
        {
            for ( u32 day = 0; day < 10; day++ )
                RegenerateOnDay(day);

            THEN("it stays empty")
            {
                REQUIRE( MapStep::GetObjectAtLocation(k_here) == OB_NONE );
            }
        }
    }
}

SCENARIO("A respawn location that still has its object is left alone")
{
    TMEStep::NewStory(RF_LOM_REPLENISH_THINGS);

    GIVEN("a flagged location holding another object")
    {
        MapStep::ResetLocation(k_here, TN_PLAINS);
        FlagRespawn(k_here);
        MapStep::SetObjectAtLocation(k_here, OB_WILDHORSES);

        WHEN("it is replenished on its due day")
        {
            RegenerateOnDay(DueDay(k_here, 2));

            THEN("the object is unchanged")
            {
                REQUIRE( MapStep::GetObjectAtLocation(k_here) == OB_WILDHORSES );
            }
        }
    }
}

SCENARIO("A terrain with no respawn entries stays empty")
{
    TMEStep::NewStory(RF_LOM_REPLENISH_THINGS);

    GIVEN("a flagged frozen wastes location with no object")
    {
        MapStep::ResetLocation(k_here, TN_FROZENWASTE);
        FlagRespawn(k_here);
        MapStep::ClearObjectFromLocation(k_here);

        WHEN("it is replenished on its due day")
        {
            RegenerateOnDay(DueDay(k_here, 5));

            THEN("it stays empty")
            {
                REQUIRE( MapStep::GetObjectAtLocation(k_here) == OB_NONE );
            }
        }
    }
}

SCENARIO("Night processing regenerates the map only when the replenish rule is enabled")
{
    const bool ruleOn = GENERATE(true, false);

    TMEStep::NewStory(ruleOn ? RF_LOM_REPLENISH_THINGS : RF_NONE);

    GIVEN("a flagged plains location with no object on its due day")
    {
        MapStep::ResetLocation(k_here, TN_PLAINS);
        FlagRespawn(k_here);
        MapStep::ClearObjectFromLocation(k_here);
        tme::sv_days = DueDay(k_here, Regenerator()->RespawnDays(tme::mx->Difficulty()));

        WHEN("night processing runs")
        {
            mxnight().Process();

            THEN("a thing is back only if the rule is enabled")
            {
                REQUIRE( (MapStep::GetObjectAtLocation(k_here) != OB_NONE) == ruleOn );
            }
        }
    }
}

SCENARIO("Starting a game with the replenish rule flags the locations that hold things")
{
    TMEStep::NewStory(RF_LOM_REPLENISH_THINGS);

    GIVEN("locations holding a respawning thing, another thing and nothing")
    {
        MapStep::ResetLocation(mxgridref(1, 1), TN_PLAINS);
        MapStep::ResetLocation(mxgridref(2, 1), TN_PLAINS);
        MapStep::ResetLocation(mxgridref(3, 1), TN_PLAINS);
        MapStep::SetObjectAtLocation(mxgridref(1, 1), OB_CUPOFDREAMS);
        MapStep::SetObjectAtLocation(mxgridref(2, 1), OB_ICECROWN);
        MapStep::ClearObjectFromLocation(mxgridref(3, 1));

        WHEN("the regenerator is initialised")
        {
            tme::mx->mapregenerator->initialise();

            THEN("only the respawning thing is flagged")
            {
                REQUIRE( (tme::mx->gamemap->GetAt(mxgridref(1, 1)).flags & lf_respawn) != 0 );
                REQUIRE( (tme::mx->gamemap->GetAt(mxgridref(2, 1)).flags & lf_respawn) == 0 );
                REQUIRE( (tme::mx->gamemap->GetAt(mxgridref(3, 1)).flags & lf_respawn) == 0 );
            }
        }
    }
}

SCENARIO("Without the replenish rule locations are not flagged")
{
    TMEStep::NewStory();

    GIVEN("a location holding a cup of dreams")
    {
        MapStep::ResetLocation(k_here, TN_PLAINS);
        MapStep::SetObjectAtLocation(k_here, OB_CUPOFDREAMS);

        WHEN("the regenerator is initialised")
        {
            tme::mx->mapregenerator->initialise();

            THEN("it is not flagged")
            {
                REQUIRE( (tme::mx->gamemap->GetAt(k_here).flags & lf_respawn) == 0 );
            }
        }
    }
}

SCENARIO("A location respawns the same thing every time by default")
{
    TMEStep::NewStory(RF_LOM_REPLENISH_THINGS);

    GIVEN("a flagged plains location")
    {
        MapStep::ResetLocation(k_here, TN_PLAINS);

        WHEN("it is asked what it respawns many times")
        {
            auto first = Regenerator()->RespawnObject(TN_PLAINS, k_here);

            THEN("it is always the same thing")
            {
                for ( int ii=0; ii<100; ii++ )
                    REQUIRE( Regenerator()->RespawnObject(TN_PLAINS, k_here) == first );
            }
        }
    }
}

SCENARIO("With the random replenish rule a location respawns any of its terrain's things")
{
    TMEStep::NewStory((RULEFLAGS)(RF_LOM_REPLENISH_THINGS | RF_LOM_REPLENISH_RANDOM));

    GIVEN("a flagged plains location")
    {
        MapStep::ResetLocation(k_here, TN_PLAINS);

        WHEN("it is asked what it respawns many times")
        {
            std::set<mxthing_t> seen;
            for ( int ii=0; ii<200; ii++ )
                seen.insert( Regenerator()->RespawnObject(TN_PLAINS, k_here) );

            THEN("it gives both plains things and nothing else")
            {
                REQUIRE( seen == std::set<mxthing_t>({ OB_WOLVES, OB_WILDHORSES }) );
            }
        }
    }
}

SCENARIO("A new terrain can be given its own respawn list")
{
    TMEStep::NewStory(RF_LOM_REPLENISH_THINGS);

    GIVEN("frozen wastes that respawn nothing by default")
    {
        REQUIRE( Regenerator()->RespawnObject(TN_FROZENWASTE, k_here) == OB_NONE );

        WHEN("a list is set for the terrain")
        {
            Regenerator()->SetRespawnList(TN_FROZENWASTE, { { OB_ICETROLLS, 100 } });

            THEN("it respawns from that list")
            {
                REQUIRE( Regenerator()->RespawnObject(TN_FROZENWASTE, k_here) == OB_ICETROLLS );
            }
        }

        WHEN("a list adds up to less than 100%")
        {
            Regenerator()->SetRespawnList(TN_FROZENWASTE, { { OB_ICETROLLS, 50 } });

            THEN("some locations respawn nothing")
            {
                int things = 0;
                for ( int x=1; x<=40; x++ )
                    things += Regenerator()->RespawnObject(TN_FROZENWASTE, mxgridref(x, 1)) != OB_NONE ? 1 : 0;

                REQUIRE( things > 0 );
                REQUIRE( things < 40 );
            }
        }
    }
}
