//
//  dungeon_tests.cpp
//  citadel
//
//  The Dark Citadel of Maranor: only Corleth and Arin search its dungeons, and only until they leave.
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

    WHEN("Corleth steps out of the Citadel")
    {
        corleth->time = (mxtime_t)tme::variables::sv_time_dawn;
        corleth->looking = DR_NORTH;
        REQUIRE( corleth->Cmd_WalkForward(false, false) == MX_OK );

        THEN("he can never search again")
        {
            REQUIRE_FALSE( corleth->InDungeon() );
            REQUIRE_FALSE( corleth->CanQuest(QS_RESCUE, Idt(luxor)) );
        }

        AND_WHEN("he walks back in")
        {
            corleth->looking = DR_SOUTH;
            corleth->Cmd_WalkForward(false, false);

            THEN("he is in the Citadel, but not in its dungeons")
            {
                REQUIRE( corleth->Location() == Citadel()->maranor->Location() );
                REQUIRE_FALSE( corleth->InDungeon() );
                REQUIRE( corleth->TakesPartInBattle() );
            }
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

SCENARIO("Seek searches the dungeons until nightfall")
{
    TMEStep::NewStory();

    auto corleth = Lord("CH_CORLETH");

    GIVEN("a search that cannot miss")
    {
        tme::variables::sv_dungeon_search_day = 256;
        auto held = Citadel()->HostagesHeldAtMaranor();

        WHEN("Corleth seeks")
        {
            corleth->Cmd_Seek();

            THEN("one hostage is free, his day is over, and a dragon has borne the hostage home")
            {
                REQUIRE( Citadel()->HostagesHeldAtMaranor().size() == held.size() - 1 );
                REQUIRE( corleth->Time() == (mxtime_t)tme::variables::sv_time_night );
                for ( auto hostage : held ) {
                    if ( !hostage->IsPrisoner() )
                        REQUIRE( hostage->Location() == hostage->home->Location() );
                }
                REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains("found in the dungeons of Maranor") );
            }
        }
    }

    GIVEN("a search that cannot succeed")
    {
        tme::variables::sv_dungeon_search_day = 0;

        WHEN("Corleth seeks")
        {
            corleth->Cmd_Seek();

            THEN("nobody is freed, and his day is over all the same")
            {
                REQUIRE( Citadel()->HostagesHeldAtMaranor().size() == 13 );
                REQUIRE( corleth->Time() == (mxtime_t)tme::variables::sv_time_night );
                REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains("finds no one") );
            }
        }
    }
}

SCENARIO("A hostage whose castle Boroth holds is flown to the nearest keep his people still hold")
{
    TMEStep::NewStory();

    auto djalina = Lord("CH_DJALINA");
    REQUIRE( djalina->home != nullptr );
    djalina->home->MakeChangeSides(RA_DARK_FEY, GetCharacter("CH_BOROTH"));

    WHEN("she is flown home")
    {
        djalina->FlyHome();

        THEN("she lands in a keep of the Uskarg, not in Boroth's")
        {
            auto keep = Citadel()->HomeKeep(djalina->Race(), djalina->home->Location());
            REQUIRE( keep != nullptr );
            REQUIRE( djalina->Location() == keep->Location() );
            REQUIRE( djalina->Location() != djalina->home->Location() );
        }
    }
}
