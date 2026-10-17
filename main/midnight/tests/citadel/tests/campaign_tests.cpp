//
//  campaign_tests.cpp
//  citadel
//
#include "../../steps/tme_steps.h"
#include "../../../Source/tme/scenarios/citadel/citadel_processor_quest.h"

#include <algorithm>
#include <cstdlib>
#include <vector>

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

    citadel_character* Boroth()
    {
        return static_cast<citadel_character*>(GetCharacter("CH_BOROTH"));
    }

    std::vector<mxgridref> Wolves()
    {
        std::vector<mxgridref> wolves;
        for ( auto [loc, sqr] : tme::mx->gamemap->Locations() ) {
            if ( sqr.object == OB_WOLVES )
                wolves.push_back(loc);
        }
        return wolves;
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
        REQUIRE( Citadel()->RealmUnderAttack() == RA_DAWN_FEY );
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
            REQUIRE( Citadel()->RealmUnderAttack() == RA_USKARG );
        }
    }

    GIVEN("that Dawnwood is wholly his")
    {
        Conquer(RA_DAWN_FEY);

        THEN("he moves on to a kingdom that borders it, the nearest Midnight - the Long Mountains")
        {
            REQUIRE( Citadel()->RealmUnderAttack() == RA_LONG_DWARF );
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
        REQUIRE( Citadel()->RealmUnderAttack() == RA_USKARG );

        WHEN("the Free take one of his Dawnwood keeps")
        {
            TakeBack(Keep("SH_CASTLE_MARALAN"));

            THEN("he goes back to secure Dawnwood first - he is further through it")
            {
                REQUIRE( Citadel()->RealmUnderAttack() == RA_DAWN_FEY );
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

SCENARIO("A regiment walks the cheapest way round, every step costing what marching charges")
{
    TMEStep::NewStory();

    auto regiment = GetEntity<mxregiment>("RG_1");
    REQUIRE( regiment != nullptr );
    auto target = Keep("SH_CASTLE_MARALAN")->Location();
    auto width = (size_t)tme::mx->gamemap->Size().cx;
    auto rinfo = tme::mx->RaceById(regiment->Race());

    const auto& costs = Citadel()->StepsFrom(target, regiment);
    auto here = regiment->Location();
    REQUIRE( costs[here.y * width + here.x] > 0 );

    while ( regiment->Location() != target ) {
        here = regiment->Location();
        mxgridref step;
        REQUIRE( Citadel()->RegimentStep(regiment, target, step) );
        auto price = (s32)rinfo->InitialMovementValue()
            + ( IS_DIAGONAL(here.DirFromHere(step)) ? rinfo->DiagonalMovementModifier() : 0 )
            + (s32)Citadel()->TerrainMovementModifier(regiment->Race(), (mxterrain_t)tme::mx->gamemap->GetAt(step).terrain);
        REQUIRE( costs[step.y * width + step.x] == costs[here.y * width + here.x] - price );
        REQUIRE( !tme::mx->scenario->isLocationImpassable(step, regiment) );
        regiment->Location(step);
    }

    AND_THEN("a target it cannot reach is left to the old steering")
    {
        mxgridref step;
        REQUIRE( !Citadel()->RegimentStep(regiment, Keep("SH_CITADEL_ASHNAR")->Location(), step) );
    }
}

SCENARIO("Boroth marches to the realm his host is taking, and leaves Toraneth in the Dark Citadel")
{
    TMEStep::NewStory();
    auto maranor = Keep("SH_CITADEL_MARANOR");

    WHEN("his host has taken nothing yet")
    {
        citadel_quest_processor().React(Boroth());

        THEN("he stays in the Dark Citadel")
        {
            REQUIRE( Boroth()->Quest() == QS_REST );
        }
    }

    WHEN("his host has taken a keep of the realm it is at war with")
    {
        auto target = Citadel()->RealmUnderAttack();
        auto maralan = Keep("SH_CASTLE_MARALAN");
        REQUIRE( maralan->Race() == target );
        maralan->MakeChangeSides(RA_DARK_FEY, Boroth());
        citadel_quest_processor().React(Boroth());

        THEN("he sets out for it, and Toraneth holds the Dark Citadel")
        {
            REQUIRE( Citadel()->FrontlineKeep() == maralan );
            REQUIRE( Boroth()->Quest() == QS_GOTO );
            REQUIRE( Boroth()->QuestTarget() == mxentity::SafeIdt(maralan) );
            REQUIRE( maranor->Occupier() == GetCharacter("CH_TORANETH") );
        }
    }

    WHEN("the Dark Citadel has fallen")
    {
        maranor->MakeChangeSides(RA_FREE, GetCharacter(TMEStep::ch_morkin));
        REQUIRE( maranor->HasFallen() );
        citadel_quest_processor().React(Boroth());

        THEN("he goes back to take it again")
        {
            REQUIRE( Boroth()->Quest() == QS_SEIZE );
            REQUIRE( Boroth()->QuestTarget() == mxentity::SafeIdt(maranor) );
        }
    }
}

SCENARIO("Kill Boroth while Luxor is still held, and the Dark Fey put Luxor to death")
{
    TMEStep::NewStory();
    auto luxor = GetCharacter("CH_LUXOR");
    REQUIRE( luxor->IsPrisoner() );

    WHEN("Boroth is killed while Luxor is in the dungeons")
    {
        Boroth()->Flags().Reset(cf_alive);
        Citadel()->NightStop();

        THEN("Luxor dies, and Boroth rises as a wraith")
        {
            REQUIRE( luxor->IsDead() );
            REQUIRE( Boroth()->IsAlive() );
        }
    }

    WHEN("Boroth is killed after Luxor is free")
    {
        luxor->Flags().Reset(cf_prisoner);
        Boroth()->Flags().Reset(cf_alive);
        Citadel()->NightStop();

        THEN("Luxor lives")
        {
            REQUIRE( luxor->IsAlive() );
        }
    }
}

SCENARIO("Soldiers gather in the keeps the Alliance and Boroth hold, in a citadel twice as fast")
{
    TMEStep::NewStory();
    auto corelay = Keep("SH_CASTLE_CORELAY");
    auto elorthord = Keep("SH_CITADEL_ELORTHORD");
    auto maralan = Keep("SH_CASTLE_MARALAN");
    elorthord->MakeChangeSides(RA_DARK_FEY, Boroth());
    auto free = corelay->TotalTroops();
    auto his = elorthord->TotalTroops();
    auto realm = maralan->TotalTroops();

    Citadel()->NightStop();

    REQUIRE( corelay->TotalTroops() == free + corelay->Respawn() );
    REQUIRE( elorthord->TotalTroops() == his + elorthord->Respawn() );
    REQUIRE( elorthord->Respawn() == 2 * corelay->Respawn() );
    REQUIRE( maralan->TotalTroops() == realm );
}

SCENARIO("Wolves run ahead of each of Boroth's armies on the march, and move on with it")
{
    TMEStep::NewStory();

    mxregiment* army = nullptr;
    for ( auto regiment : tme::mx->objRegiments ) {
        if ( regiment->Race() == RA_DARK_FEY && army == nullptr )
            army = regiment;
        else
            regiment->Total(0);
    }
    REQUIRE( army != nullptr );
    tme::mx->scenario->NightStart();
    REQUIRE( army->Orders() == OD_GOTO );
    auto before = Wolves();

    WHEN("the night ends")
    {
        Citadel()->NightStop();
        auto after = Wolves();

        THEN("one pack runs a few squares ahead of the army")
        {
            REQUIRE( after.size() == before.size() + 1 );
            mxgridref pack;
            for ( auto loc : after ) {
                if ( std::find(before.begin(), before.end(), loc) == before.end() )
                    pack = loc;
            }
            auto here = army->Location();
            auto squares = std::max(std::abs(here.x - pack.x), std::abs(here.y - pack.y));
            REQUIRE( squares >= 1 );
            REQUIRE( squares <= 3 );
        }

        AND_WHEN("another night ends")
        {
            Citadel()->NightStop();

            THEN("it is still one pack, not two")
            {
                REQUIRE( Wolves().size() == after.size() );
            }
        }
    }
}

SCENARIO("Once Luxor is free or dead, five thousand warriors of the Free gather at Corelay, once")
{
    TMEStep::NewStory();
    auto corelay = Keep("SH_CASTLE_CORELAY");
    auto luxor = GetCharacter("CH_LUXOR");
    auto before = corelay->TotalTroops();

    WHEN("he is still held")
    {
        Citadel()->NightStop();
        REQUIRE( corelay->TotalTroops() == before + corelay->Respawn() );
    }

    WHEN("he is freed")
    {
        luxor->Flags().Reset(cf_prisoner);
        Citadel()->NightStop();

        THEN("they gather, and only that once")
        {
            REQUIRE( corelay->TotalTroops() == before + 5000 + corelay->Respawn() );
            REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains("warriors of the Free gather at the Castle of Corelay") );
            Citadel()->NightStop();
            REQUIRE( corelay->TotalTroops() == before + 5000 + 2 * corelay->Respawn() );
        }
    }

    WHEN("he dies in the dungeons")
    {
        luxor->Flags().Reset(cf_alive);
        Citadel()->NightStop();
        REQUIRE( corelay->TotalTroops() == before + 5000 + corelay->Respawn() );
    }
}

SCENARIO("A kingdom borders the kingdoms its own row lists, as the table has it")
{
    TMEStep::NewStory();

    REQUIRE( Citadel()->Borders(RA_GOLDEN_FEY, RA_ELDRIN) );
    REQUIRE_FALSE( Citadel()->Borders(RA_ELDRIN, RA_GOLDEN_FEY) );
    REQUIRE( Citadel()->Borders(RA_KITH, RA_GOLDEN_FEY) );
    REQUIRE( Citadel()->Borders(RA_GOLDEN_FEY, RA_KITH) );
}
