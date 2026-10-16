//
//  quest_tests.cpp
//  citadel
//
#include "../../steps/tme_steps.h"
#include "../../../Source/tme/scenarios/citadel/citadel_processor_quest.h"
#include "../../../Source/tme/scenarios/citadel/citadel_processor_battle.h"

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

    void Army(mxcharacter* lord, u32 men)
    {
        lord->warriors.Total(men);
        lord->riders.Total(0);
    }

    citadel_object* Artefact(LPCSTR symbol)
    {
        auto object = GetObject(symbol);
        REQUIRE( object != nullptr );
        return CitadelObject(object);
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

    THEN("a keep of the realms can be seized, which is war, but not a keep of yours")
    {
        REQUIRE( rorthron->SetQuest(QS_SEIZE, Idt(Keep("SH_CASTLE_ARABAR"))) );
        REQUIRE_FALSE( rorthron->SetQuest(QS_SEIZE, Idt(Keep("SH_CASTLE_CORELAY"))) );
    }

    THEN("one of yours can be sent to attack a lord of the realms")
    {
        REQUIRE( rorthron->SetQuest(QS_KILL, Idt(ilvar)) );
    }

    THEN("but not to attack one of yours, and a refusal changes nothing")
    {
        auto before = rorthron->quest;
        REQUIRE_FALSE( rorthron->SetQuest(QS_KILL, Idt(Lord("CH_CORLETH"))) );
        REQUIRE( rorthron->quest == before );
    }

    THEN("Find, Take and Destroy are for the seven weapons, not the Moon Ring")
    {
        REQUIRE( rorthron->SetQuest(QS_FIND, Idt(Artefact("OB_STORMBLADE"))) );
        REQUIRE_FALSE( rorthron->SetQuest(QS_FIND, Idt(Artefact("OB_MOONRING"))) );
    }

    THEN("nothing can be taken from a lord who is not yours")
    {
        REQUIRE_FALSE( rorthron->SetQuest(QS_TAKE, Idt(Artefact("OB_BLOODBRINGER"))) );
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

SCENARIO("One lord sets upon another, and the side left without men loses")
{
    TMEStep::NewStory();

    auto rorthron = Lord(ch_rorthron);
    auto haraglai = Lord(ch_haraglai);
    Army(rorthron, 1000);
    Army(haraglai, 0);
    haraglai->Location(rorthron->Location());
    auto where = haraglai->Location();

    WHEN("Rorthron and a thousand men set upon Haraglai, who is alone")
    {
        CitadelBattle()->Duel(rorthron, haraglai);

        THEN("Rorthron wins, and Haraglai falls or flees")
        {
            REQUIRE( rorthron->Flags().Is(cf_wonbattle) );
            REQUIRE( ( haraglai->IsDead() || haraglai->Location() != where ) );
        }
    }
}

SCENARIO("Corleth searches the dungeons by night, and each hostage he finds is flown home")
{
    TMEStep::NewStory();

    auto citadel = static_cast<citadel_x*>(tme::mx->scenario);
    auto corleth = Lord("CH_CORLETH");
    auto djalina = Lord(ch_djalina);
    REQUIRE( corleth->InDungeon() );
    REQUIRE( corleth->SetQuest(QS_RESCUE, Idt(djalina)) );
    Lord("CH_ARIN")->quest = QS_NONE;
    tme::variables::sv_dungeon_search_night = 256;      // tonight's search cannot miss

    WHEN("night falls")
    {
        auto held = citadel->HostagesHeldAtMaranor();
        TMEStep::NightFalls();

        THEN("one hostage is free and yours, and stands in his home castle")
        {
            REQUIRE( citadel->HostagesHeldAtMaranor().size() == held.size() - 1 );
            for ( auto hostage : held ) {
                if ( hostage->IsPrisoner() )
                    continue;
                REQUIRE( hostage->IsRecruited() );
                REQUIRE( hostage->Location() == hostage->home->Location() );
            }
        }
    }
}

SCENARIO("A lord offended by an approach sets upon the lord who made it")
{
    TMEStep::NewStory();

    auto rorthron = Lord(ch_rorthron);
    auto haraglai = Lord(ch_haraglai);
    haraglai->Location(rorthron->Location());

    GIVEN("that the two of them are opposites")
    {
        rorthron->qualities = qf_kind;
        haraglai->qualities = qf_cruel;
        REQUIRE( rorthron->RecruitScore(haraglai) <= -1 );
        REQUIRE( rorthron->SetQuest(QS_RECRUIT, Idt(haraglai)) );

        WHEN("Rorthron makes his approach")
        {
            TMEStep::NightFalls();

            THEN("Haraglai is not won over: he is offended, and they have fought")
            {
                REQUIRE_FALSE( haraglai->IsRecruited() );
                REQUIRE( rorthron->news == QN_OFFENDED );
                REQUIRE( ( rorthron->Flags().Is(cf_inbattle) || rorthron->Flags().Is(cf_wonbattle) || rorthron->IsDead() ) );
            }
        }
    }

    GIVEN("that they have nothing in common either way")
    {
        rorthron->qualities = qf_none;
        haraglai->qualities = qf_none;
        REQUIRE( rorthron->SetQuest(QS_RECRUIT, Idt(haraglai)) );

        WHEN("Rorthron makes his approach")
        {
            TMEStep::NightFalls();

            THEN("Haraglai simply refuses")
            {
                REQUIRE_FALSE( haraglai->IsRecruited() );
                REQUIRE( rorthron->news == QN_REFUSED );
            }
        }
    }
}

SCENARIO("An artefact can be found, borrowed from one of yours, and destroyed")
{
    TMEStep::NewStory();

    auto rorthron = Lord(ch_rorthron);
    auto corleth = Lord("CH_CORLETH");
    auto stormblade = Artefact("OB_STORMBLADE");
    mxgridref where;
    REQUIRE( stormblade->OnMap(where) );

    WHEN("Rorthron, where Stormblade lies, is sent to find it")
    {
        rorthron->Location(where);
        REQUIRE( rorthron->SetQuest(QS_FIND, Idt(stormblade)) );
        TMEStep::NightFalls();

        THEN("he carries it, and it lies there no longer")
        {
            REQUIRE( rorthron->Carrying() == stormblade );
            REQUIRE_FALSE( stormblade->OnMap(where) );
            REQUIRE( rorthron->news == QN_DONE );
        }
    }

    WHEN("Corleth, beside him, carries Bloodbringer, and Rorthron is sent to take it")
    {
        auto bloodbringer = Artefact("OB_BLOODBRINGER");
        rorthron->carrying = nullptr;
        corleth->carrying = bloodbringer;
        corleth->Location(rorthron->Location());
        REQUIRE( rorthron->SetQuest(QS_TAKE, Idt(bloodbringer)) );
        TMEStep::NightFalls();

        THEN("Rorthron has it")
        {
            REQUIRE( rorthron->Carrying() == bloodbringer );
            REQUIRE( corleth->Carrying() == nullptr );
        }
    }

    WHEN("Rorthron, carrying the Persuader, is sent to destroy it")
    {
        auto persuader = Artefact("OB_PERSUADER");
        rorthron->carrying = persuader;
        REQUIRE( rorthron->SetQuest(QS_DESTROY, Idt(persuader)) );
        TMEStep::NightFalls();

        THEN("nobody has it and it lies nowhere")
        {
            REQUIRE( rorthron->Carrying() == nullptr );
            REQUIRE( tme::mx->scenario->WhoHasObject(persuader) == nullptr );
            REQUIRE_FALSE( persuader->OnMap(where) );
        }
    }
}

SCENARIO("A lord's quest and his news are told from strings.tsv, naming whom or what it is for")
{
    TMEStep::NewStory();

    auto rorthron = Lord(ch_rorthron);
    auto corleth = Lord("CH_CORLETH");
    auto ilvar = Lord(ch_ilvar);
    auto haraglai = Lord(ch_haraglai);
    auto text = []( u32 id, mxcharacter* character ) { return tme::mx->text->CookedSystemString(id, character); };

    THEN("the quest names its lord, keep or weapon, and the one he hunts by that lord's own pronoun")
    {
        REQUIRE( rorthron->SetQuest(QS_RECRUIT, Idt(ilvar)) );
        REQUIRE( rorthron->QuestText() == "Rorthron is on a quest to recruit " + ilvar->Longname() + "." );

        REQUIRE( rorthron->SetQuest(QS_KILL, Idt(haraglai)) );
        REQUIRE( rorthron->QuestText() == "Rorthron is hunting " + haraglai->Longname() + " to slay "
                                          + tme::mx->GenderById(haraglai->gender)->pronoun3 + "." );

        auto bloodbringer = Artefact("OB_BLOODBRINGER");
        corleth->carrying = bloodbringer;
        REQUIRE( rorthron->SetQuest(QS_TAKE, Idt(bloodbringer)) );
        REQUIRE( rorthron->QuestText() == "Rorthron is going to " + corleth->Longname() + " for " + bloodbringer->name + "." );

        REQUIRE( rorthron->SetQuest(QS_REST, IDT_NONE) );
        REQUIRE( rorthron->QuestText() == "Rorthron waits for your orders." );
    }

    THEN("the dawn news is one sentence on the quest page, and a paragraph of the night's report")
    {
        REQUIRE( rorthron->SetQuest(QS_RECRUIT, Idt(ilvar)) );
        rorthron->news = QN_REFUSED;
        auto news = ilvar->Longname() + " would not be persuaded by Rorthron";
        REQUIRE( rorthron->NewsText() == news );
        REQUIRE( text(SS_QUEST_NEWS_LINE, rorthron) == news + "." );
        REQUIRE_THAT( text(SS_QUEST_NEWS_REPORT, rorthron), Catch::Matchers::StartsWith("\n") && Catch::Matchers::EndsWith(" " + news) );
    }

    THEN("the quest page asks by the lord's name")
    {
        REQUIRE( text(SS_QUEST_ASK_RECRUIT, rorthron) == "Choose a person for Rorthron to recruit..." );
        REQUIRE( text(SS_QUEST_PICK_GUARD, rorthron) == "Choose a place for Rorthron to guard..." );
    }

    THEN("thinking about a lord of yours ends with his quest, in the strings rather than the screen")
    {
        REQUIRE( rorthron->SetQuest(QS_REST, IDT_NONE) );
        REQUIRE_THAT( text(SS_MESSAGE1, rorthron), Catch::Matchers::EndsWith(" Rorthron waits for your orders.") );
        REQUIRE_THAT( text(SS_MESSAGE4, rorthron), Catch::Matchers::EndsWith("\n\nRorthron waits for your orders.") );
        REQUIRE( text(SS_QUEST_STATUS, rorthron) == "Rorthron waits for your orders." );
    }
}

SCENARIO("Luxor begins the game held captive, and can take no quest until he is free")
{
    TMEStep::NewStory();

    auto luxor = Lord("CH_LUXOR");
    auto here = luxor->Location();
    REQUIRE( luxor->IsPrisoner() );

    THEN("no quest is his to take, not even a walk, and his quest is told as his captivity")
    {
        REQUIRE_FALSE( luxor->CanQuest() );
        REQUIRE_FALSE( luxor->SetQuest(QS_GOTO, MAKE_LOCID(here.x + 1, here.y)) );
        REQUIRE( luxor->QuestText() == "Luxor is held captive, and can set out on no quest." );
    }

    THEN("he is told as a hostage, not as a lord yet to be persuaded")
    {
        REQUIRE_THAT( tme::mx->text->CookedSystemString(SS_PRISONER, luxor),
            Catch::Matchers::StartsWith("Luxor is held hostage here in the dungeons of the Dark Citadel") );
    }

    WHEN("he is set free")
    {
        luxor->Flags().Reset(cf_prisoner);

        THEN("he can be sent anywhere on the map")
        {
            REQUIRE( luxor->CanQuest() );
            REQUIRE( luxor->SetQuest(QS_GOTO, MAKE_LOCID(here.x + 1, here.y)) );
        }
    }
}

SCENARIO("A guard sets upon one of Boroth's lords who comes close")
{
    TMEStep::NewStory();

    auto rorthron = Lord(ch_rorthron);
    auto foe = Lord("CH_YRGRETH");
    Army(rorthron, 1000);
    auto here = rorthron->Location();
    foe->Location(here + DR_NORTH);
    REQUIRE( rorthron->SetQuest(QS_GUARD, MAKE_LOCID(here.x, here.y)) );

    WHEN("night falls")
    {
        TMEStep::NightFalls();

        THEN("they have fought")
        {
            REQUIRE( ( rorthron->Flags().Is(cf_wonbattle) || rorthron->Flags().Is(cf_inbattle) ) );
        }
    }
}

SCENARIO("An impatient lord of yours, left waiting, sets out on a quest of his own")
{
    TMEStep::NewStory();

    auto rorthron = Lord(ch_rorthron);
    REQUIRE( rorthron->SetQuest(QS_REST, IDT_NONE) );

    GIVEN("that he is impatient")
    {
        rorthron->qualities = ( rorthron->qualities & ~(u64)qf_patient ) | qf_impatient;

        WHEN("he waits for orders night after night")
        {
            for ( int night = 0; night < 5 && rorthron->news != QN_IMPATIENT; night++ )
                TMEStep::NightFalls();

            THEN("he chooses his own quest, and tells you so")
            {
                REQUIRE( rorthron->news == QN_IMPATIENT );
                REQUIRE( rorthron->quest != QS_REST );
                REQUIRE( rorthron->quest != QS_NONE );
            }
        }
    }

    GIVEN("that he is patient, as the database has him")
    {
        REQUIRE( rorthron->HasQuality(qf_patient) );

        WHEN("he waits for orders night after night")
        {
            for ( int night = 0; night < 5; night++ )
                TMEStep::NightFalls();

            THEN("he waits")
            {
                REQUIRE( rorthron->quest == QS_REST );
            }
        }
    }
}

SCENARIO("A wanderer wanders, and a hostage set free goes home to defend it")
{
    TMEStep::NewStory();

    WHEN("a lord of the realms is given to wandering")
    {
        auto haraglai = Lord(ch_haraglai);
        haraglai->purpose = PU_RANDOMLY_WANDER;
        React(haraglai);

        THEN("he sets off somewhere")
        {
            REQUIRE( haraglai->quest == QS_GOTO );
            REQUIRE( haraglai->QuestLocation() != haraglai->Location() );
        }
    }

    WHEN("Djalina is out of the dungeons, though not yours")
    {
        auto djalina = Lord(ch_djalina);
        djalina->Flags().Reset(cf_prisoner);
        React(djalina);

        THEN("she reacts to the war like any lord of her realm")
        {
            REQUIRE( djalina->quest != QS_REST );
        }
    }
}


SCENARIO("CITADEL: Prefix for what is here and what is ahead")
{
    TMEStep::NewStory();

    GIVEN("a lord looking north")
    {
        auto character = TMEStep::PlaceLordAt(TMEStep::ch_rorthron, mxgridref(25, 5), DR_NORTH);

        WHEN("the here and ahead prefixes are read")
        {
            auto here  = tme::mx->text->CookedSystemString(SS_PREFIX_HERE, character);
            auto ahead = tme::mx->text->CookedSystemString(SS_PREFIX_AHEAD, character);

            THEN("they are the original game's wording")
            {
                REQUIRE_THAT( here,  Catch::Matchers::Equals("  Here ") );
                REQUIRE_THAT( ahead, Catch::Matchers::Equals("  Ahead ") );
            }
        }
    }
}
