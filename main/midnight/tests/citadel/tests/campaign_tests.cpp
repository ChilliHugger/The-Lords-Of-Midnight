//
//  campaign_tests.cpp
//  citadel
//
#include "../../steps/tme_steps.h"

namespace {

    citadel_x* Citadel()
    {
        return static_cast<citadel_x*>(tme::mx->scenario);
    }

    mxstronghold* Keep(LPCSTR symbol)
    {
        auto keep = GetStronghold(symbol);
        REQUIRE( keep != nullptr );
        return keep;
    }

    // Boroth takes every keep of a kingdom he can fight over
    void Conquer(mxrace_t people)
    {
        for ( auto stronghold : tme::mx->objStrongholds ) {
            if ( stronghold->Race() == people && Citadel()->IsFoughtOver(stronghold) )
                stronghold->MakeChangeSides(RA_DARK_FEY, nullptr);
        }
    }

    // the Free take one of his keeps from him
    void TakeBack(mxstronghold* stronghold)
    {
        stronghold->MakeChangeSides(RA_FREE, GetCharacter(TMEStep::ch_morkin));
        REQUIRE( stronghold->OccupyingRace() != RA_DARK_FEY );
    }
}

SCENARIO("Boroth opens his war on a kingdom bordering the Marish")
{
    TMEStep::NewStory();

    THEN("of the Gelm, the Deeping and Dawnwood - each four kingdoms from Midnight - he takes the first in the table")
    {
        REQUIRE( Citadel()->CampaignTarget() == RA_DAWN_FEY );
    }

    AND_THEN("every one of his regiments marches on a keep of that kingdom")
    {
        tme::mx->scenario->NightStart();
        for ( auto regiment : tme::mx->objRegiments ) {
            CONTINUE_IF( regiment->Race() != RA_DARK_FEY );
            REQUIRE( regiment->Orders() == OD_GOTO );
            auto keep = static_cast<mxstronghold*>(tme::mx->EntityByIdt(regiment->TargetId()));
            REQUIRE( keep != nullptr );
            REQUIRE( keep->Race() == RA_DAWN_FEY );
            REQUIRE( keep->OccupyingRace() != RA_DARK_FEY );
        }
    }
}

SCENARIO("Boroth takes one kingdom at a time")
{
    TMEStep::NewStory();

    GIVEN("that he has a foot in the Fallows")
    {
        Keep("SH_CASTLE_ARABAR")->MakeChangeSides(RA_DARK_FEY, nullptr);

        THEN("he finishes the Fallows before starting anywhere nearer Midnight")
        {
            REQUIRE( Citadel()->CampaignTarget() == RA_USKARG );
        }
    }

    GIVEN("that Dawnwood is wholly his")
    {
        Conquer(RA_DAWN_FEY);

        THEN("he moves on to a kingdom that borders it, the nearest Midnight - the Long Mountains")
        {
            REQUIRE( Citadel()->CampaignTarget() == RA_LONG_DWARF );
        }
    }
}

SCENARIO("A keep taken back behind Boroth turns him round")
{
    TMEStep::NewStory();

    GIVEN("that Dawnwood is his and he has begun on the Fallows")
    {
        Conquer(RA_DAWN_FEY);
        Keep("SH_CASTLE_ARABAR")->MakeChangeSides(RA_DARK_FEY, nullptr);
        REQUIRE( Citadel()->CampaignTarget() == RA_USKARG );

        WHEN("the Free take one of his Dawnwood keeps")
        {
            TakeBack(Keep("SH_CASTLE_MARALAN"));

            THEN("he goes back to secure Dawnwood first - he is further through it")
            {
                REQUIRE( Citadel()->CampaignTarget() == RA_DAWN_FEY );
            }
        }
    }
}

SCENARIO("Only the keeps his host can reach are fought over")
{
    TMEStep::NewStory();

    THEN("the Dark Citadel and the keeps of the realms are")
    {
        REQUIRE( Citadel()->IsFoughtOver(Keep("SH_CITADEL_MARANOR")) );
        REQUIRE( Citadel()->IsFoughtOver(Keep("SH_CASTLE_MARALAN")) );
    }

    THEN("the Isle of Arungor is not: no army can march to it")
    {
        REQUIRE( !Citadel()->IsFoughtOver(Keep("SH_CITADEL_ASHNAR")) );
    }

    THEN("Immiel is not: Boroth dares not assail the Golden Fey")
    {
        REQUIRE( !Citadel()->IsFoughtOver(Keep("SH_CITADEL_IMMIEL")) );
    }

    THEN("nor are the 1995 game's placeholders at 256,0, off the map")
    {
        REQUIRE( !Citadel()->IsFoughtOver(Keep("SH_CASTLE_IMILVIR")) );
    }
}

SCENARIO("A regiment walks the shortest way round, one square nearer every step")
{
    TMEStep::NewStory();

    auto regiment = GetEntity<mxregiment>("RG_1");
    REQUIRE( regiment != nullptr );
    auto target = Keep("SH_CASTLE_MARALAN")->Location();
    auto width = (size_t)tme::mx->gamemap->Size().cx;

    const auto& steps = Citadel()->StepsFrom(target, regiment);
    auto here = regiment->Location();
    auto distance = steps[here.y * width + here.x];
    REQUIRE( distance > 0 );

    for ( s32 left = distance; left > 0; left-- ) {
        mxgridref step;
        REQUIRE( Citadel()->RegimentStep(regiment, target, step) );
        REQUIRE( steps[step.y * width + step.x] == left - 1 );
        REQUIRE( !tme::mx->scenario->isLocationImpassable(step, regiment) );
        regiment->Location(step);
    }
    REQUIRE( regiment->Location() == target );

    AND_THEN("a target it cannot reach is left to the old steering")
    {
        mxgridref step;
        REQUIRE( !Citadel()->RegimentStep(regiment, Keep("SH_CITADEL_ASHNAR")->Location(), step) );
    }
}
