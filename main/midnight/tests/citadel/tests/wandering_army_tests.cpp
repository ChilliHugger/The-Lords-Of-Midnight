//
//  wandering_army_tests.cpp
//  citadel
//
//  Boroth the Wolfheart's host marches the map, takes keeps and fights the lords it finds. Who
//  it counts as an enemy, and who is at war with it, are The Citadel's answers, not Lords of
//  Midnight's: citadel_stronghold, citadel_character and citadel_battle.
//
#include "../../steps/tme_steps.h"

namespace {

    // a keep of some other realm, which Boroth's host can take
    mxstronghold* AKeepOfTheRealms()
    {
        for ( auto stronghold : tme::mx->objStrongholds ) {
            if ( stronghold->OccupyingRace() != RA_DARK_FEY )
                return stronghold;
        }
        return nullptr;
    }

    // one of the Dark Fey's own keeps, which is not at war until the host is in it
    mxstronghold* ADarkFeyKeep()
    {
        for ( auto stronghold : tme::mx->objStrongholds ) {
            if ( stronghold->OccupyingRace() == RA_DARK_FEY )
                return stronghold;
        }
        return nullptr;
    }

    mxregiment* AWanderingArmy()
    {
        for ( auto regiment : tme::mx->objRegiments ) {
            if ( regiment->Race() == RA_DARK_FEY && regiment->Total() > 0 )
                return regiment;
        }
        return nullptr;
    }

    // a lord of the realms the night can see, who is neither Dark Fey nor a hostage
    mxcharacter* ALordOfTheRealms()
    {
        for ( auto character : tme::mx->objCharacters ) {
            if ( character->Race() != RA_DARK_FEY && !character->IsPrisoner()
                 && !character->IsDead() && !character->IsHidden() )
                return character;
        }
        return nullptr;
    }

    const mxgridref nowhere = mxgridref(0,0);
}

SCENARIO("Only a keep Boroth's host has taken is held against you")
{
    TMEStep::NewStory();

    auto boroth = GetCharacter("CH_BOROTH");
    REQUIRE( boroth != nullptr );

    GIVEN("one of the Dark Fey's own keeps")
    {
        auto keep = ADarkFeyKeep();
        REQUIRE( keep != nullptr );

        THEN("it is not at war, though the Dark Fey hold it")
        {
            REQUIRE( keep->OccupyingRace() == RA_ENEMY );
            REQUIRE_FALSE( keep->IsEnemy() );
        }
    }

    GIVEN("a keep of one of the realms")
    {
        auto keep = AKeepOfTheRealms();
        REQUIRE( keep != nullptr );
        REQUIRE_FALSE( keep->IsEnemy() );

        WHEN("Boroth's host takes it")
        {
            keep->MakeChangeSides( RA_ENEMY, boroth );

            THEN("it is held against you")
            {
                REQUIRE( keep->Occupier() == boroth );
                REQUIRE( keep->IsEnemy() );
            }
        }
    }
}

SCENARIO("Who is at war with Boroth")
{
    TMEStep::NewStory();

    auto lord = ALordOfTheRealms();
    REQUIRE( lord != nullptr );

    THEN("a lord who has joined you is")
    {
        lord->Flags().Set(cf_recruited);
        REQUIRE( lord->TakesPartInBattle() );
    }

    THEN("a lord who has not joined you is not")
    {
        lord->Flags().Reset(cf_recruited);
        REQUIRE_FALSE( lord->TakesPartInBattle() );
    }

    THEN("a hostage in the dungeons is not")
    {
        lord->Flags().Set(cf_recruited);
        lord->Flags().Set(cf_prisoner);
        REQUIRE_FALSE( lord->TakesPartInBattle() );
    }

    THEN("Boroth's own Dark Fey are not")
    {
        auto boroth = GetCharacter("CH_BOROTH");
        REQUIRE( boroth != nullptr );
        boroth->Flags().Set(cf_recruited);
        REQUIRE_FALSE( boroth->TakesPartInBattle() );
    }
}

SCENARIO("A wandering army brings battle only where someone stands against it")
{
    TMEStep::NewStory();

    auto army = AWanderingArmy();
    auto lord = ALordOfTheRealms();
    REQUIRE( army != nullptr );
    REQUIRE( lord != nullptr );

    GIVEN("an army where a lord who has not joined you stands")
    {
        lord->Flags().Reset(cf_recruited);
        lord->Location( army->Location() );
        lord->battleloc = nowhere;

        WHEN("the night passes over them")
        {
            tme::mx->battle->ProcessLocation( army->Location() );

            THEN("it passes him by")
            {
                REQUIRE( lord->battleloc == nowhere );
                REQUIRE( !lord->IsInBattle() );
            }
        }

        AND_GIVEN("that he has joined you")
        {
            lord->Flags().Set(cf_recruited);

            WHEN("the night passes over them")
            {
                tme::mx->battle->ProcessLocation( army->Location() );

                THEN("battle is joined")
                {
                    REQUIRE( lord->IsInBattle() );
                }
            }
        }
    }
}
