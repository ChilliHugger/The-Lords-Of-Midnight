//
//  victory_tests.cpp
//  citadel
//
// "You win when Boroth the Wolfheart is dead AND the Dark Citadel of Maranor has fallen to the Free"
// - the game's help, VICTORY AND DEFEAT
//
#include "../../steps/tme_steps.h"

namespace {

    citadel_x* Citadel()
    {
        return static_cast<citadel_x*>(tme::mx->scenario);
    }

    citadel_character* Boroth()
    {
        return Citadel()->boroth;
    }

    m_gameover_t GameOver()
    {
        return tme::mx->gameover->Process(true);
    }

    // Boroth falls in the night's fighting, and the night ends
    void BorothIsKilled()
    {
        Boroth()->Cmd_Dead();
        Citadel()->NightStop();
    }

    void TheCitadelFalls()
    {
        Citadel()->maranor->MakeChangeSides(RA_FREE, GetCharacter("CH_MORKIN"));
    }
}

SCENARIO("The Citadel is won when Boroth is dead and the Dark Citadel of Maranor has fallen")
{
    TMEStep::NewStory();

    THEN("a new game is neither won nor lost")
    {
        REQUIRE( GameOver() == MG_NONE );
    }

    WHEN("Boroth is killed while the Citadel is still his")
    {
        BorothIsKilled();

        THEN("he rises in it as a wraith, and holds it against the Free")
        {
            REQUIRE( Boroth()->IsAlive() );
            REQUIRE( Citadel()->flags.Is(gf_wraith) );
            REQUIRE( Boroth()->Location() == Citadel()->maranor->Location() );
            REQUIRE( Citadel()->maranor->IsEnemy() );
            REQUIRE( GameOver() == MG_NONE );
        }

        AND_WHEN("the Citadel falls and the wraith is killed")
        {
            TheCitadelFalls();
            BorothIsKilled();

            THEN("he stays dead, and the game is won")
            {
                REQUIRE_FALSE( Boroth()->IsAlive() );
                REQUIRE( GameOver() == MG_WIN );
                REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains("Victory to the Free!") );
            }
        }
    }

    WHEN("the Citadel falls before Boroth is killed")
    {
        TheCitadelFalls();
        REQUIRE( GameOver() == MG_NONE );
        BorothIsKilled();

        THEN("there is no wraith, and the game is won")
        {
            REQUIRE_FALSE( Boroth()->IsAlive() );
            REQUIRE( GameOver() == MG_WIN );
        }
    }
}

SCENARIO("The Citadel is lost with the House of Moon, or with Corelay")
{
    TMEStep::NewStory();

    WHEN("Luxor, Morkin and Corleth are dead")
    {
        for ( auto lord : { "CH_LUXOR", "CH_MORKIN", "CH_CORLETH" } )
            TMEStep::LordIsDead(lord);

        THEN("Anderlane still carries the House of Moon")
        {
            REQUIRE( GameOver() == MG_NONE );
        }

        AND_WHEN("Anderlane dies too")
        {
            TMEStep::LordIsDead("CH_ANDERLANE");

            THEN("Boroth wins")
            {
                REQUIRE( GameOver() == MG_LOSE );
                REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains("the House of Moon is no more") );
            }
        }
    }

    WHEN("the Castle of Corelay falls to Boroth's host")
    {
        GetStronghold("SH_CASTLE_CORELAY")->MakeChangeSides(RA_DARK_FEY, Citadel()->boroth);

        THEN("Boroth wins")
        {
            REQUIRE( GameOver() == MG_LOSE );
            REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains("Corelay has fallen") );
        }
    }
}
