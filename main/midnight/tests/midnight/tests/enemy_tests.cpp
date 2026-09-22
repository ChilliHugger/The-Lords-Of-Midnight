//
//  enemy_tests.cpp
//  midnight
//
//  Who the night counts as the enemy, and who stands against him. These pin Lords of Midnight's
//  own answers - mxstronghold::IsEnemy, mxcharacter::TakesPartInBattle and lom_battle's
//  HasDefenders / TakesPart - which The Citadel's scenario classes override.
//
#include "../../steps/tme_steps.h"

namespace {

    mxstronghold* AKeepOfTheFree()
    {
        for ( auto stronghold : tme::mx->objStrongholds ) {
            if ( stronghold->OccupyingRace() != RA_DOOMGUARD && stronghold->Owner() != nullptr )
                return stronghold;
        }
        return nullptr;
    }

    mxregiment* ARegimentOfTheDoomguard()
    {
        for ( auto regiment : tme::mx->objRegiments ) {
            if ( regiment->Race() == RA_DOOMGUARD && regiment->Total() > 0 )
                return regiment;
        }
        return nullptr;
    }

    // a lord the night can see, who has not yet joined you
    mxcharacter* ALordWhoHasNotJoinedYou()
    {
        for ( auto character : tme::mx->objCharacters ) {
            if ( !character->IsDead() && !character->IsHidden() && !character->IsRecruited()
                 && character->Race() != RA_DOOMGUARD && character->Race() != RA_MIDWINTER )
                return character;
        }
        return nullptr;
    }

    // somewhere no lord and no army is, so a location reads as untouched
    const mxgridref nowhere = mxgridref(0,0);
}

SCENARIO("A keep Doomdark has taken is held against you")
{
    TMEStep::NewStory();

    auto keep = AKeepOfTheFree();
    REQUIRE( keep != nullptr );

    GIVEN("a keep that is not Doomdark's")
    {
        REQUIRE_FALSE( keep->IsEnemy() );

        WHEN("Doomdark takes it")
        {
            keep->MakeChangeSides( RA_DOOMGUARD, tme::mx->scenario->BadGuy() );

            THEN("it is held against you")
            {
                REQUIRE( keep->IsEnemy() );
            }

            // Taking a keep changes who sits in it, never who owns it, so the keep is still
            // loyal to the lord it belongs to. That is why being the enemy is a question about
            // the occupying race, and IsFriend cannot answer it here.
            AND_THEN("it is still loyal to the lord who owns it")
            {
                REQUIRE( keep->IsFriend( keep->Owner() ) );
            }
        }
    }
}

SCENARIO("Who stands in a battle")
{
    TMEStep::NewStory();

    auto luxor = GetCharacter(TMEStep::ch_luxor);
    auto lord = ALordWhoHasNotJoinedYou();
    REQUIRE( lord != nullptr );

    THEN("a lord of the free does")
    {
        REQUIRE( luxor->TakesPartInBattle() );
    }

    THEN("a lord who has not joined you does too")
    {
        REQUIRE( lord->TakesPartInBattle() );
    }

    THEN("a lord who is dead does not")
    {
        lord->Flags().Reset(cf_alive);
        REQUIRE_FALSE( lord->TakesPartInBattle() );
    }

    THEN("a lord who is hidden does not")
    {
        lord->Flags().Set(cf_hidden);
        REQUIRE_FALSE( lord->TakesPartInBattle() );
    }

    THEN("a lord of Midwinter does not")
    {
        lord->race = RA_MIDWINTER;
        REQUIRE_FALSE( lord->TakesPartInBattle() );
    }
}

SCENARIO("The Doomguard bring battle wherever they find a lord")
{
    TMEStep::NewStory();

    auto luxor = GetCharacter(TMEStep::ch_luxor);
    auto regiment = ARegimentOfTheDoomguard();
    REQUIRE( regiment != nullptr );

    GIVEN("a lord with no Doomguard about")
    {
        luxor->battleloc = nowhere;

        WHEN("the night passes over him")
        {
            tme::mx->battle->ProcessLocation( luxor->Location() );

            THEN("there is no battle")
            {
                REQUIRE( luxor->battleloc == nowhere );
            }
        }
    }

    GIVEN("a regiment of the Doomguard where he stands")
    {
        regiment->Location( luxor->Location() );
        luxor->battleloc = nowhere;

        WHEN("the night passes over him")
        {
            tme::mx->battle->ProcessLocation( luxor->Location() );

            THEN("battle is joined")
            {
                REQUIRE( luxor->battleloc == luxor->Location() );
            }
        }

        // in Lords of Midnight every lord is at war, joined or not - The Citadel is the
        // scenario where that is no longer true
        AND_GIVEN("that the lord there has not joined you")
        {
            auto lord = ALordWhoHasNotJoinedYou();
            REQUIRE( lord != nullptr );
            lord->Location( regiment->Location() );
            lord->battleloc = nowhere;

            WHEN("the night passes over him")
            {
                tme::mx->battle->ProcessLocation( lord->Location() );

                THEN("battle is joined all the same")
                {
                    REQUIRE( lord->battleloc == lord->Location() );
                }
            }
        }
    }
}
