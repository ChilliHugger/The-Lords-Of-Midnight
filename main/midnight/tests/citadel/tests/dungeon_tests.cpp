//
//  dungeon_tests.cpp
//  citadel
//
//  The Dark Citadel of Maranor: Corleth and Arin begin in its dungeons, and a hostage is freed by walking to him.
//
#include "../../steps/tme_steps.h"

namespace {

    citadel_x* Citadel()
    {
        return static_cast<citadel_x*>(tme::mx->scenario);
    }

    citadel_character* Lord(LPCSTR symbol)
    {
        auto lord = static_cast<citadel_character*>(GetCharacter(symbol));
        REQUIRE( lord != nullptr );
        return lord;
    }

    mxid Idt(mxentity* entity)
    {
        return mxentity::SafeIdt(entity);
    }

    mxdir_t DoorOf(mxgridref room)
    {
        for ( auto dir : { DR_NORTH, DR_EAST, DR_SOUTH, DR_WEST } ) {
            if ( Citadel()->dungeon.HasPassage(room, dir) )
                return dir;
        }
        return DR_NONE;
    }

    void StandOutside(citadel_character* lord, mxgridref room)
    {
        auto door = DoorOf(room);
        REQUIRE( door != DR_NONE );
        lord->DungeonLocation(Citadel()->dungeon.Neighbour(room, door));
        lord->looking = (mxdir_t)((door + 4) % 8);
        lord->time = (mxtime_t)tme::variables::sv_time_dawn;
    }
}

SCENARIO("Only Corleth and Arin are in the dungeons, and the Citadel is held against everyone else")
{
    TMEStep::NewStory();

    auto corleth = Lord("CH_CORLETH");
    auto luxor = Lord("CH_LUXOR");

    THEN("they search the dungeons and sit the Citadel's battles out")
    {
        REQUIRE( corleth->InDungeon() );
        REQUIRE( Lord("CH_ARIN")->InDungeon() );
        REQUIRE_FALSE( corleth->TakesPartInBattle() );
        REQUIRE_FALSE( Lord("CH_MORKIN")->InDungeon() );
        REQUIRE( Citadel()->maranor->IsEnemy() );
        REQUIRE( Citadel()->HostagesHeldAtMaranor().size() == 13 );
    }

    THEN("a hostage is found, not approached, and only they may set out to rescue him")
    {
        REQUIRE_FALSE( corleth->CheckRecruitChar(luxor) );
        REQUIRE( corleth->CanQuest(QS_RESCUE, Idt(luxor)) );
        REQUIRE_FALSE( Lord("CH_MORKIN")->CanQuest(QS_RESCUE, Idt(luxor)) );
    }

    WHEN("Corleth walks out of the way out")
    {
        corleth->time = (mxtime_t)tme::variables::sv_time_dawn;
        corleth->DungeonLocation(Citadel()->dungeon.Entrance());
        corleth->looking = DR_SOUTH;
        REQUIRE( corleth->GetLocInfo()->flags.Is(lif_exit_tunnel) );
        REQUIRE( corleth->Cmd_WalkForward(false, false) == MX_OK );

        THEN("he is in the Citadel, but not in its dungeons")
        {
            REQUIRE( corleth->Location() == Citadel()->maranor->Location() );
            REQUIRE_FALSE( corleth->InDungeon() );
            REQUIRE_FALSE( corleth->CanQuest(QS_RESCUE, Idt(luxor)) );
            REQUIRE( corleth->TakesPartInBattle() );
            REQUIRE( corleth->GetLocInfo()->flags.Is(lif_enter_tunnel) );
        }

        AND_WHEN("he goes back down")
        {
            REQUIRE( corleth->Cmd_EnterTunnel() == MX_OK );

            THEN("he stands at the foot of the way out, looking in")
            {
                REQUIRE( corleth->InDungeon() );
                REQUIRE( corleth->DungeonLocation() == Citadel()->dungeon.Entrance() );
                REQUIRE( corleth->Looking() == DR_NORTH );
            }
        }
    }

    WHEN("Morkin, far from the Citadel, tries to go down")
    {
        THEN("he cannot")
        {
            REQUIRE( Lord("CH_MORKIN")->Cmd_EnterTunnel() == MX_FAILED );
        }
    }

    WHEN("the Citadel has fallen to the Free")
    {
        Citadel()->maranor->MakeChangeSides(RA_FREE, GetCharacter("CH_MORKIN"));

        THEN("the hostages left in it may be approached")
        {
            REQUIRE( corleth->CheckRecruitChar(luxor) );
        }
    }
}

SCENARIO("A hostage is freed by the lord who walks into his cell")
{
    TMEStep::NewStory();

    auto corleth = Lord("CH_CORLETH");
    auto djalina = Lord("CH_DJALINA");
    auto cell = Citadel()->dungeon.CellOf(djalina);

    GIVEN("Corleth outside her cell, looking in")
    {
        StandOutside(corleth, cell);
        REQUIRE( corleth->GetLocInfo()->flags.Is(lif_moveforward) );

        WHEN("he walks forward")
        {
            auto time = corleth->Time();
            REQUIRE( corleth->Cmd_WalkForward(false, false) == MX_OK );

            THEN("she is free, a dragon has borne her home, and the step took time")
            {
                REQUIRE( corleth->DungeonLocation() == cell );
                REQUIRE_FALSE( djalina->IsPrisoner() );
                REQUIRE( djalina->IsRecruited() );
                REQUIRE( djalina->Location() == djalina->Home()->Location() );
                REQUIRE( corleth->Time() < time );
                REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains("found in the dungeons of Maranor") );
            }

            AND_WHEN("he walks on into the wall of the cell")
            {
                THEN("the wall stops him")
                {
                    REQUIRE( corleth->GetLocInfo()->flags.Is(lif_blocked) );
                    REQUIRE( corleth->Cmd_WalkForward(false, false) == MX_FAILED );
                    REQUIRE( corleth->DungeonLocation() == cell );
                }
            }
        }
    }
}

SCENARIO("The dungeons are built from their seed")
{
    TMEStep::NewStory();

    auto& dungeon = Citadel()->dungeon;
    auto djalina = Lord("CH_DJALINA");

    WHEN("the same seed is used twice")
    {
        dungeon.Generate(1234);
        auto start = dungeon.Start();
        auto cell = dungeon.CellOf(djalina);
        dungeon.Generate(4321);
        dungeon.Generate(1234);

        THEN("the same dungeon is built")
        {
            REQUIRE( dungeon.Start() == start );
            REQUIRE( dungeon.CellOf(djalina) == cell );
        }

        THEN("every cell can be walked to from the way out, and no hostage is held where Corleth begins")
        {
            REQUIRE( dungeon.StepTowards(dungeon.Entrance(), start) != DR_NONE );
            for ( auto hostage : Citadel()->HostagesHeldAtMaranor() ) {
                REQUIRE( dungeon.StepTowards(dungeon.Entrance(), dungeon.CellOf(hostage)) != DR_NONE );
                REQUIRE( dungeon.CellOf(hostage) != start );
            }
        }
    }
}

SCENARIO("A hostage whose castle Boroth holds is flown to the nearest keep his people still hold")
{
    TMEStep::NewStory();

    auto djalina = Lord("CH_DJALINA");
    REQUIRE( djalina->Home() != nullptr );
    djalina->Home()->MakeChangeSides(RA_DARK_FEY, GetCharacter("CH_BOROTH"));

    WHEN("she is flown home")
    {
        djalina->FlyHome();

        THEN("she lands in a keep of the Uskarg, not in Boroth's")
        {
            auto keep = Citadel()->HomeKeep(djalina->Race(), djalina->Home()->Location());
            REQUIRE( keep != nullptr );
            REQUIRE( djalina->Location() == keep->Location() );
            REQUIRE( djalina->Location() != djalina->Home()->Location() );
        }
    }
}
