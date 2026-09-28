//
//  garrison_tests.cpp
//  citadel
//
#include "../../steps/tme_steps.h"
#include "../../../Source/tme/scenarios/citadel/citadel_processor_battle.h"

namespace {

    const mxrace_t feuds[][2] = {
        { RA_DRAGONLORD,    RA_BLOODMARCH_GIANT },
        { RA_DEEPING_DWARF, RA_LONG_DWARF },
        { RA_DAWN_FEY,      RA_GELMING },
        { RA_ELDRIN,        RA_HIGH_FEY },
        { RA_USKARG,        RA_KITH },
        { RA_ARAKAI,        RA_ATHELING },
    };

    mxstronghold* AKeepHeldBy ( mxrace_t people )
    {
        for ( auto stronghold : tme::mx->objStrongholds ) {
            if ( stronghold->OccupyingRace() == people )
                return stronghold;
        }
        return nullptr;
    }

    mxcharacter* ALordOf ( mxrace_t people )
    {
        for ( auto character : tme::mx->objCharacters ) {
            if ( character->Race() == people )
                return character;
        }
        return nullptr;
    }

    mxcharacter* PersuaderAt ( LPCSTR name, mxstronghold* keep )
    {
        TMEStep::LordCarryingObject(name, "OB_PERSUADER");
        auto lord = GetCharacter(name);
        lord->Flags().Set(cf_army);
        lord->warriors.Total(500);
        lord->Location(keep->Location());
        keep->TotalTroops(500);
        return lord;
    }

    struct walls : public citadel_battle
    {
        void At ( mxgridref loc )
        {
            info.reset(new mxlocinfo(loc, nullptr, slf_none));
            status = BA_CONTINUES;
        }

        mxarmy* Garrison()
        {
            for ( auto army : info->armies ) {
                if ( army->armytype == AT_STRONGHOLD )
                    return army;
            }
            return nullptr;
        }

        using citadel_battle::PrepareArmies;
        using citadel_battle::UpdateStrongholdArmy;
    };
}

SCENARIO("Six peoples of the Blood March will not stand in each other's garrisons")
{
    TMEStep::NewStory();

    for ( auto& feud : feuds ) {
        for ( int side = 0; side < 2; side++ ) {
            auto lord = ALordOf(feud[side]);
            auto keep = AKeepHeldBy(feud[1-side]);
            REQUIRE( lord != nullptr );
            REQUIRE( keep != nullptr );

            CAPTURE( lord->Symbol(), keep->Symbol() );
            REQUIRE_FALSE( keep->CanCharacterPost(lord) );
        }
    }

    THEN("peoples with no quarrel may")
    {
        auto lord = ALordOf(RA_DRAGONLORD);
        REQUIRE( lord != nullptr );
        REQUIRE( AKeepHeldBy(RA_GELMING)->CanCharacterPost(lord) );
        REQUIRE( AKeepHeldBy(RA_DRAGONLORD)->CanCharacterPost(lord) );
    }
}

SCENARIO("The Persuader draws men from a feuding people's keep, but will not leave any there")
{
    TMEStep::NewStory();

    GIVEN("a Dragonlord with the Persuader in a keep of the Giants of the Delve")
    {
        auto keep = GetStronghold("SH_CASTLE_IRON");
        REQUIRE( keep->OccupyingRace() == RA_BLOODMARCH_GIANT );
        auto lord = PersuaderAt("CH_GALAGRIM", keep);
        REQUIRE( lord->Race() == RA_DRAGONLORD );

        auto info = lord->GetLocInfo();

        THEN("he may recruit there")
        {
            REQUIRE( info->flags.Is(lif_recruitmen) );
        }

        THEN("but he may not post men there")
        {
            REQUIRE_FALSE( info->flags.Is(lif_guardmen) );
        }
    }

    GIVEN("the same lord in a keep of the Gelmings, who have no quarrel with Arungor")
    {
        auto keep = GetStronghold("SH_CASTLE_FINFYR");
        REQUIRE( keep->OccupyingRace() == RA_GELMING );
        auto lord = PersuaderAt("CH_GALAGRIM", keep);

        THEN("he may post men there")
        {
            REQUIRE( lord->GetLocInfo()->flags.Is(lif_guardmen) );
        }
    }
}

SCENARIO("A keep's walls multiply its garrison in battle")
{
    TMEStep::NewStory();

    GIVEN("the Citadel of Beomir with 500 men within its walls")
    {
        auto keep = static_cast<citadel_stronghold*>(GetStronghold("SH_CITADEL_BEOMIR"));
        REQUIRE( keep->Terrain() == TN_CITADEL );
        keep->TotalTroops(500);

        walls battle;
        battle.At(keep->Location());
        battle.PrepareArmies();
        auto garrison = battle.Garrison();
        REQUIRE( garrison != nullptr );

        THEN("they fight as 2,000")
        {
            REQUIRE( keep->DefenceMultiplier() == 4 );
            REQUIRE( garrison->total == 2000 );
        }

        WHEN("the battle leaves 1,001 of the 2,000 standing")
        {
            garrison->total = 1001;
            battle.UpdateStrongholdArmy(garrison);

            THEN("251 men are left in the keep")
            {
                REQUIRE( keep->TotalTroops() == 251 );
                REQUIRE( keep->Lost() == 249 );
            }
        }

        WHEN("the battle leaves only the walls' last share of a man")
        {
            garrison->total = 1;
            battle.UpdateStrongholdArmy(garrison);

            THEN("the keep still holds")
            {
                REQUIRE( keep->TotalTroops() == 1 );
            }
        }

        WHEN("the battle leaves no one")
        {
            garrison->total = 0;
            battle.UpdateStrongholdArmy(garrison);

            THEN("all 500 are lost, and the keep keeps only what any emptied keep does")
            {
                REQUIRE( keep->Lost() == 500 );
                REQUIRE( keep->TotalTroops() == (u32)tme::variables::sv_stronghold_default_empty );
            }
        }
    }

    GIVEN("a castle of the Delve with 500 men")
    {
        auto keep = static_cast<citadel_stronghold*>(GetStronghold("SH_CASTLE_IRON"));
        REQUIRE( keep->Terrain() == TN_KEEP );
        keep->TotalTroops(500);

        walls battle;
        battle.At(keep->Location());
        battle.PrepareArmies();

        THEN("they fight as 1,500")
        {
            REQUIRE( keep->DefenceMultiplier() == 3 );
            REQUIRE( battle.Garrison()->total == 1500 );
        }
    }
}
