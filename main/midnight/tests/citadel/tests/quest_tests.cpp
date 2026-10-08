//
//  quest_tests.cpp
//  citadel
//
#include "../../steps/tme_steps.h"
#include "../../../Source/tme/scenarios/citadel/citadel_processor_quest.h"

#include <map>

static constexpr LPCSTR ch_rorthron = "CH_RORTHRON";   // one of yours
static constexpr LPCSTR ch_ilvar    = "CH_ILVAR";      // an Atheling lord Rorthron can win over
static constexpr LPCSTR ch_haraglai = "CH_HARAGLAI";   // an Uskarg lord of the Fallows
static constexpr LPCSTR ch_djalina  = "CH_DJALINA";    // the Uskarg hostage
static constexpr LPCSTR ch_boroth   = "CH_BOROTH";

namespace {

    void React ( citadel_character* lord )
    {
        citadel_quest_processor().React(lord);
    }

    citadel_character* Lord(LPCSTR symbol)
    {
        auto lord = static_cast<citadel_character*>(GetCharacter(symbol));
        REQUIRE( lord != nullptr );
        return lord;
    }

    mxstronghold* Keep(LPCSTR symbol)
    {
        auto keep = GetStronghold(symbol);
        REQUIRE( keep != nullptr );
        return keep;
    }

    mxid Idt(mxentity* entity)
    {
        return mxentity::SafeIdt(entity);
    }

    void BorothTakes(mxstronghold* keep)
    {
        keep->MakeChangeSides(RA_DARK_FEY, GetCharacter(ch_boroth));
        REQUIRE( keep->IsEnemy() );
    }

    void FreeTheHostages()
    {
        for ( auto character : tme::mx->objCharacters )
            character->Flags().Reset(cf_prisoner);
    }

    void Army(mxcharacter* lord, u32 men)
    {
        lord->warriors.Total(men);
        lord->riders.Total(0);
    }

    // an on-map keep of a people, and a lord of the realms of that people
    mxstronghold* AKeepOf(mxrace_t people)
    {
        for ( auto stronghold : tme::mx->objStrongholds ) {
            if ( stronghold->Race() == people && tme::mx->gamemap->IsLocOnMap(stronghold->Location()) )
                return stronghold;
        }
        return nullptr;
    }

    citadel_character* ALordOf(mxrace_t people)
    {
        for ( auto character : tme::mx->objCharacters ) {
            auto lord = static_cast<citadel_character*>(character);
            if ( lord->Race() == people && lord->purpose == PU_DEFEND_HOMELAND && !lord->IsRecruited() )
                return lord;
        }
        return nullptr;
    }

    int VisibleSquares()
    {
        int visible = 0;
        for ( auto [loc, sqr] : tme::mx->gamemap->Locations() ) {
            if ( tme::mx->gamemap->IsLocationVisible(loc) )
                visible++;
        }
        return visible;
    }
}

SCENARIO("Every lord's purpose, reaction and quest come from the database")
{
    TMEStep::NewStory();

    THEN("the lords of the realms were born to defend their homelands, and start by going home")
    {
        REQUIRE( Lord(ch_haraglai)->purpose == PU_DEFEND_HOMELAND );
        REQUIRE( Lord(ch_haraglai)->reaction == RE_RETURN_HOME );
        REQUIRE( Lord(ch_haraglai)->quest == QS_REST );
    }

    THEN("a hostage's purpose is to be one, and Corleth is in the Dark Citadel to rescue")
    {
        REQUIRE( Lord(ch_djalina)->purpose == PU_BE_A_HOSTAGE );
        REQUIRE( Lord("CH_CORLETH")->quest == QS_RESCUE );
    }
}

SCENARIO("Your lords take quests, and the lords of the realms take no orders from you")
{
    TMEStep::NewStory();

    auto rorthron = Lord(ch_rorthron);
    auto ilvar = Lord(ch_ilvar);

    THEN("one of yours can be sent to recruit a lord of the realms")
    {
        REQUIRE( rorthron->SetQuest(QS_RECRUIT, Idt(ilvar)) );
        REQUIRE( rorthron->quest == QS_RECRUIT );
        REQUIRE( rorthron->questtarget == Idt(ilvar) );
    }

    THEN("a lord of the realms cannot be sent anywhere")
    {
        REQUIRE_FALSE( ilvar->SetQuest(QS_GOTO, MAKE_LOCID(80, 90)) );
    }

    THEN("nobody is sent to recruit a lord who has already joined you")
    {
        REQUIRE_FALSE( rorthron->SetQuest(QS_RECRUIT, Idt(Lord("CH_CORLETH"))) );
    }

    THEN("a keep Boroth does not hold cannot be seized")
    {
        REQUIRE_FALSE( rorthron->SetQuest(QS_SEIZE, Idt(Keep("SH_CASTLE_ARABAR"))) );
    }

    THEN("a quest not built yet is refused rather than silently kept")
    {
        auto before = rorthron->quest;
        REQUIRE_FALSE( rorthron->SetQuest(QS_KILL, Idt(ilvar)) );
        REQUIRE( rorthron->quest == before );
    }
}

SCENARIO("Rorthron is sent to find Ilvar the Penitent and win him over")
{
    TMEStep::NewStory();

    auto rorthron = Lord(ch_rorthron);
    auto ilvar = Lord(ch_ilvar);
    REQUIRE( rorthron->CheckRecruitChar(ilvar) );
    REQUIRE( rorthron->SetQuest(QS_RECRUIT, Idt(ilvar)) );

    WHEN("the nights pass")
    {
        for ( int night = 0; night < 30 && !ilvar->IsRecruited(); night++ )
            TMEStep::NightFalls();

        THEN("Ilvar joins you, and Rorthron's fellowship, and the quest is done")
        {
            REQUIRE( ilvar->IsRecruited() );
            REQUIRE( ilvar->Following() == rorthron );
            REQUIRE( rorthron->quest == QS_NONE );
        }
    }
}

SCENARIO("A lord sent somewhere walks there over the nights that follow, then waits")
{
    TMEStep::NewStory();

    auto rorthron = Lord(ch_rorthron);
    auto there = Lord(ch_ilvar)->Location();
    REQUIRE( rorthron->SetQuest(QS_GOTO, MAKE_LOCID(there.x, there.y)) );

    WHEN("the nights pass")
    {
        for ( int night = 0; night < 30 && rorthron->Location() != there; night++ )
            TMEStep::NightFalls();

        THEN("he is there, and waiting for his next quest")
        {
            REQUIRE( rorthron->Location() == there );
            REQUIRE( rorthron->quest == QS_NONE );
        }
    }
}

SCENARIO("A realm whose hostage is held will not march")
{
    TMEStep::NewStory();

    auto haraglai = Lord(ch_haraglai);
    Army(haraglai, 1000);
    auto arabar = Keep("SH_CASTLE_ARABAR");
    BorothTakes(arabar);

    GIVEN("that Djalina, the Uskarg hostage, is still in the dungeons of Maranor")
    {
        REQUIRE( tme::mx->scenario->HostageOfRace(RA_USKARG) != nullptr );

        WHEN("Haraglai considers the war")
        {
            React(haraglai);

            THEN("he does not march on the keep Boroth took from the Fallows")
            {
                REQUIRE( haraglai->quest != QS_SEIZE );
            }
        }
    }

    GIVEN("that she is free")
    {
        Lord(ch_djalina)->Flags().Reset(cf_prisoner);

        WHEN("Haraglai considers the war")
        {
            React(haraglai);

            THEN("he goes to take it back")
            {
                REQUIRE( haraglai->reaction == RE_TAKE_BACK_STRONGHOLD );
                REQUIRE( haraglai->quest == QS_SEIZE );
                REQUIRE( haraglai->questtarget == Idt(arabar) );
            }
        }

        WHEN("he is a coward")
        {
            haraglai->qualities = qf_cowardly;
            React(haraglai);

            THEN("he does not")
            {
                REQUIRE( haraglai->quest != QS_SEIZE );
            }
        }

        WHEN("he has too few men for its garrison")
        {
            Army(haraglai, 10);
            arabar->TotalTroops(500);
            React(haraglai);

            THEN("he does not")
            {
                REQUIRE( haraglai->quest != QS_SEIZE );
            }
        }
    }
}

SCENARIO("A lord whose own realm is whole helps a neighbour - but not one his people feud with")
{
    TMEStep::NewStory();
    FreeTheHostages();

    GIVEN("that Boroth holds a keep of Dawnwood, which borders the Fallows")
    {
        auto haraglai = Lord(ch_haraglai);
        Army(haraglai, 1000);
        auto maralan = Keep("SH_CASTLE_MARALAN");
        BorothTakes(maralan);

        WHEN("an Uskarg lord considers the war")
        {
            React(haraglai);

            THEN("he marches to help the Dawn Fey take it back")
            {
                REQUIRE( haraglai->reaction == RE_HELP_NEIGHBOUR );
                REQUIRE( haraglai->quest == QS_SEIZE );
                REQUIRE( haraglai->questtarget == Idt(maralan) );
            }
        }
    }

    GIVEN("that Boroth holds a keep of the Deeping, which borders the Long Mountains")
    {
        auto dwarf = ALordOf(RA_LONG_DWARF);
        REQUIRE( dwarf != nullptr );
        Army(dwarf, 1000);
        auto keep = AKeepOf(RA_DEEPING_DWARF);
        REQUIRE( keep != nullptr );
        BorothTakes(keep);

        WHEN("a Long Dwarf considers the war")
        {
            React(dwarf);

            THEN("he leaves the Deeping Dwarves to it: the two are at feud")
            {
                REQUIRE( dwarf->quest != QS_SEIZE );
            }
        }
    }
}

SCENARIO("A keep the lords of the realms win back goes back to its own realm")
{
    TMEStep::NewStory();

    auto maralan = Keep("SH_CASTLE_MARALAN");
    BorothTakes(maralan);

    WHEN("an Uskarg lord takes it back for the Dawn Fey")
    {
        maralan->MakeChangeSides(RA_FREE, Lord(ch_haraglai));

        THEN("it is the Dawn Fey's again, not the Uskarg's")
        {
            REQUIRE( maralan->OccupyingRace() == RA_DAWN_FEY );
            REQUIRE_FALSE( maralan->IsEnemy() );
        }
    }

    WHEN("one of your lords takes it")
    {
        maralan->MakeChangeSides(RA_FREE, Lord(ch_rorthron));

        THEN("it is the Free's, as in Lords of Midnight")
        {
            REQUIRE( maralan->OccupyingRace() == RA_FREE );
        }
    }
}

SCENARIO("The lords of the realms march by night without showing you where they went")
{
    TMEStep::NewStory();

    std::map<u32, mxgridref> before;
    for ( auto character : tme::mx->objCharacters ) {
        if ( !character->IsRecruited() )
            before[character->Id()] = character->Location();
    }
    auto visible = VisibleSquares();

    WHEN("night falls and they set off for home")
    {
        TMEStep::NightFalls();

        int moved = 0;
        for ( auto character : tme::mx->objCharacters ) {
            if ( !character->IsRecruited() && character->Location() != before[character->Id()] )
                moved++;
        }

        THEN("some have moved, and your map shows no more than it did")
        {
            REQUIRE( moved > 0 );
            REQUIRE( VisibleSquares() == visible );
        }
    }
}

SCENARIO("The lords of the realms are the computer's, until one of them joins you")
{
    TMEStep::NewStory();

    auto rorthron = Lord(ch_rorthron);
    auto ilvar = Lord(ch_ilvar);

    THEN("yours are yours, and the realms' are AI-controlled")
    {
        REQUIRE_FALSE( rorthron->IsAIControlled() );
        REQUIRE( ilvar->IsAIControlled() );
    }

    WHEN("Ilvar is recruited")
    {
        ilvar->Recruited(rorthron);

        THEN("he is yours")
        {
            REQUIRE( ilvar->IsRecruited() );
            REQUIRE_FALSE( ilvar->IsAIControlled() );
        }
    }
}
