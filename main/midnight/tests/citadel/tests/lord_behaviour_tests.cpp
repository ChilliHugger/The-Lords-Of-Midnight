//
//  lord_behaviour_tests.cpp
//  citadel
//
#include "../../steps/tme_steps.h"
#include "../../../Source/tme/scenarios/citadel/citadel_processor_quest.h"
#include "../../../Source/tme/scenarios/citadel/citadel_processor_battle.h"

namespace {

    citadel_character* Lord(LPCSTR symbol)
    {
        auto lord = static_cast<citadel_character*>(GetCharacter(symbol));
        REQUIRE( lord != nullptr );
        return lord;
    }

    citadel_stronghold* Keep(LPCSTR symbol)
    {
        auto keep = static_cast<citadel_stronghold*>(GetStronghold(symbol));
        REQUIRE( keep != nullptr );
        return keep;
    }

    citadel_x* Citadel()
    {
        return static_cast<citadel_x*>(tme::mx->scenario);
    }

    mxid Idt(mxentity* entity)
    {
        return mxentity::SafeIdt(entity);
    }

    void Army(mxcharacter* lord, u32 men)
    {
        lord->warriors.Total(men);
        lord->riders.Total(0);
    }

    void FreeTheHostages()
    {
        for ( auto character : tme::mx->objCharacters )
            character->Flags().Reset(cf_prisoner);
    }

    mxregiment* AFoeRegimentAt(u32 men, mxgridref where)
    {
        for ( auto regiment : tme::mx->objRegiments ) {
            if ( regiment->Race() == RA_ENEMY ) {
                regiment->Total(men);
                regiment->Location(where);
                return regiment;
            }
        }
        return nullptr;
    }

    void React(citadel_character* lord)
    {
        citadel_quest_processor().React(lord);
    }

    mxdanger_t DangerAt(mxstronghold* keep)
    {
        citadel_quest_processor processor;
        processor.Survey();
        return processor.Danger(processor.PlaceOf(keep->Location()));
    }

    mxgridref Beside(mxgridref where, mxcharacter* lord)
    {
        for ( int dir = DR_NORTH; dir <= DR_NORTHWEST; dir++ ) {
            auto there = where + (mxdir_t)dir;
            if ( !tme::mx->scenario->isLocationImpassable(there, lord) )
                return there;
        }
        return where;
    }

    // Haraglai at Skordroth, his own keep, with Boroth's host gone and Yrgreth out of Gliwain, next door
    citadel_character* HaraglaiAtHome()
    {
        for ( auto regiment : tme::mx->objRegiments )
            regiment->Total(0);
        Lord("CH_YRGRETH")->Location(Keep("SH_CITADEL_MARANOR")->Location());
        auto haraglai = Lord("CH_HARAGLAI");
        haraglai->Location(Keep("SH_CASTLE_SKORDROTH")->Location());
        return haraglai;
    }
}

SCENARIO("The Dark Fey are at war with every realm, and every realm with them, but no realm with another")
{
    TMEStep::NewStory();

    REQUIRE( Citadel()->AtWar(RA_USKARG, RA_DARK_FEY) );
    REQUIRE( Citadel()->AtWar(RA_DARK_FEY, RA_USKARG) );
    REQUIRE( Citadel()->AtWar(RA_FREE, RA_DARK_FEY) );
    REQUIRE_FALSE( Citadel()->AtWar(RA_USKARG, RA_KITH) );
    REQUIRE_FALSE( Citadel()->AtWar(RA_ATHELING, RA_FREE) );
}

SCENARIO("Every lord of the realms has a home keep to defend")
{
    TMEStep::NewStory();

    for ( auto character : tme::mx->objCharacters ) {
        auto lord = static_cast<citadel_character*>(character);
        if ( !lord->IsRecruited() && ( lord->purpose == PU_DEFEND_HOMELAND || lord->purpose == PU_BE_A_HOSTAGE ) )
            REQUIRE( lord->home != nullptr );
    }

    THEN("it is the keep he holds, or for a hostage the one the database names")
    {
        REQUIRE( Lord("CH_HARAGLAI")->home == Keep("SH_CASTLE_SKORDROTH") );
        REQUIRE( Lord("CH_DJALINA")->home == Keep("SH_CASTLE_THEODEL") );
    }
}

SCENARIO("A lord at home in a realm at peace stands firm")
{
    TMEStep::NewStory();
    auto haraglai = HaraglaiAtHome();

    React(haraglai);

    REQUIRE( DangerAt(Keep("SH_CASTLE_SKORDROTH")) == LS_AT_PEACE );
    REQUIRE( haraglai->reaction == RE_STAND_FIRM );
    REQUIRE( haraglai->quest == QS_REST );
    REQUIRE( haraglai->IsAvailable() );
}

SCENARIO("A lord whose home is under attack holds it only if he is brave or three times as strong")
{
    TMEStep::NewStory();
    auto haraglai = HaraglaiAtHome();
    auto skordroth = Keep("SH_CASTLE_SKORDROTH");
    AFoeRegimentAt(200, skordroth->Location());

    REQUIRE( DangerAt(skordroth) == LS_UNDER_ATTACK );

    WHEN("he has ten men")
    {
        Army(haraglai, 10);
        React(haraglai);

        THEN("he retreats to another keep")
        {
            REQUIRE( haraglai->reaction == RE_RETREAT );
            REQUIRE( haraglai->quest == QS_GOTO );
            REQUIRE( haraglai->questtarget != Idt(skordroth) );
        }
    }

    WHEN("he has ten thousand")
    {
        Army(haraglai, 10000);
        React(haraglai);

        THEN("he holds, and is no one's to call on")
        {
            REQUIRE( haraglai->reaction == RE_STAND_FIRM );
            REQUIRE( haraglai->quest == QS_REST );
            REQUIRE_FALSE( haraglai->IsAvailable() );
        }
    }

    WHEN("he has ten men, and is brave")
    {
        Army(haraglai, 10);
        haraglai->qualities = qf_brave;
        React(haraglai);

        THEN("he holds")
        {
            REQUIRE( haraglai->reaction == RE_STAND_FIRM );
            REQUIRE( haraglai->quest == QS_REST );
        }
    }
}

SCENARIO("A reckless lord attacks the enemy next door, though his realm's hostage is held")
{
    TMEStep::NewStory();
    auto haraglai = HaraglaiAtHome();
    Army(haraglai, 1000);
    auto regiment = AFoeRegimentAt(200, Keep("SH_CASTLE_ELINBRAND")->Location());

    REQUIRE( DangerAt(Keep("SH_CASTLE_SKORDROTH")) == LS_IN_PERIL );
    REQUIRE( haraglai->HasQuality(qf_reckless) );
    REQUIRE( tme::mx->scenario->HostageOfRace(RA_USKARG) != nullptr );

    React(haraglai);

    REQUIRE( haraglai->reaction == RE_ATTACK_ENEMY );
    REQUIRE( haraglai->quest == QS_KILL );
    REQUIRE( haraglai->questtarget == Idt(regiment) );
}

SCENARIO("While his realm's hostage is held, its king only defends his citadel")
{
    TMEStep::NewStory();
    for ( auto regiment : tme::mx->objRegiments )
        regiment->Total(0);
    auto asunai = Lord("CH_ASUNAI");
    auto roreon = Keep("SH_CITADEL_ROREON");
    asunai->Location(roreon->Location());
    Army(asunai, 1000);
    auto regiment = AFoeRegimentAt(200, Keep("SH_CASTLE_CERRELM")->Location());

    REQUIRE( asunai->IsKing() );
    REQUIRE( asunai->HasQuality(qf_brave) );
    REQUIRE( DangerAt(roreon) == LS_IN_PERIL );

    GIVEN("that Djalina, the Uskarg hostage, is held")
    {
        React(asunai);

        THEN("the King of the Fallows stays in his citadel")
        {
            REQUIRE( asunai->quest == QS_REST );
        }
    }

    GIVEN("that she is free")
    {
        FreeTheHostages();
        React(asunai);

        THEN("he goes to fight the regiment")
        {
            REQUIRE( asunai->reaction == RE_ATTACK_ENEMY );
            REQUIRE( asunai->quest == QS_KILL );
            REQUIRE( asunai->questtarget == Idt(regiment) );
        }
    }
}

SCENARIO("A lord whose keep Boroth has taken goes to take it back")
{
    TMEStep::NewStory();
    auto haraglai = HaraglaiAtHome();
    auto skordroth = Keep("SH_CASTLE_SKORDROTH");
    skordroth->MakeChangeSides(RA_DARK_FEY, GetCharacter("CH_BOROTH"));

    React(haraglai);

    REQUIRE( skordroth->Subjugated() );
    REQUIRE( haraglai->reaction == RE_TAKE_BACK_STRONGHOLD );
    REQUIRE( haraglai->quest == QS_SEIZE );
    REQUIRE( haraglai->questtarget == Idt(skordroth) );
}

SCENARIO("A lord whose realm is invaded elsewhere goes to help a neighbour who is at home")
{
    TMEStep::NewStory();
    FreeTheHostages();
    auto haraglai = HaraglaiAtHome();
    auto darath = Lord("CH_DARATH");
    darath->Location(Keep("SH_CASTLE_ELINBRAND")->Location());
    AFoeRegimentAt(200, Keep("SH_CASTLE_ARABAR")->Location());

    REQUIRE( DangerAt(Keep("SH_CASTLE_SKORDROTH")) == LS_IN_DANGER );

    React(haraglai);

    REQUIRE( haraglai->reaction == RE_HELP_NEIGHBOUR );
    REQUIRE( haraglai->quest == QS_JOIN );
    auto subject = CharacterTarget(haraglai->questtarget);
    REQUIRE( subject != nullptr );
    REQUIRE( subject->Race() == RA_USKARG );
}

SCENARIO("Attacking a lord of the realms puts you on his realm's war list")
{
    TMEStep::NewStory();
    auto rorthron = Lord("CH_RORTHRON");
    auto ilvar = Lord("CH_ILVAR");
    auto ceremelth = Keep("SH_CASTLE_CEREMELTH");
    ilvar->Location(ceremelth->Location());
    rorthron->Location(ceremelth->Location());
    Army(rorthron, 1000);

    THEN("until then one of yours in Ceremelth is no enemy of the Lee")
    {
        REQUIRE( DangerAt(ceremelth) == LS_AT_PEACE );
    }

    WHEN("he is sent to attack Ilvar")
    {
        REQUIRE( rorthron->SetQuest(QS_KILL, Idt(ilvar)) );
        TMEStep::NightFalls();

        THEN("the Lee are at war with you, though you are not with them")
        {
            REQUIRE( Citadel()->AtWar(RA_ATHELING, RA_FREE) );
            REQUIRE_FALSE( Citadel()->AtWar(RA_FREE, RA_ATHELING) );
        }
    }
}

SCENARIO("One of yours besieges a keep of the realms and takes it for the Free")
{
    TMEStep::NewStory();
    for ( auto regiment : tme::mx->objRegiments )
        regiment->Total(0);
    auto rorthron = Lord("CH_RORTHRON");
    auto ceremelth = Keep("SH_CASTLE_CEREMELTH");
    Army(rorthron, 3000);
    rorthron->Location(Beside(ceremelth->Location(), rorthron));
    REQUIRE( rorthron->SetQuest(QS_SEIZE, Idt(ceremelth)) );

    WHEN("the nights pass")
    {
        for ( int night = 0; night < 12 && ceremelth->Allegiance() != RA_FREE; night++ )
            TMEStep::NightFalls();

        THEN("it is his, the Lee are at war with you, and he goes in")
        {
            REQUIRE( ceremelth->Occupier() == rorthron );
            REQUIRE( ceremelth->OccupyingRace() == RA_FREE );
            REQUIRE( Citadel()->AtWar(RA_ATHELING, RA_FREE) );

            TMEStep::NightFalls();
            TMEStep::NightFalls();
            REQUIRE( rorthron->Location() == ceremelth->Location() );
            REQUIRE( rorthron->quest == QS_NONE );
        }
    }
}

SCENARIO("A far-sighted lord whose home is in peril wins a neighbour into his own fellowship")
{
    TMEStep::NewStory();
    FreeTheHostages();
    auto haraglai = HaraglaiAtHome();
    auto darath = Lord("CH_DARATH");
    haraglai->qualities = qf_farsighted | qf_kind | qf_gentle;
    darath->qualities = qf_farsighted | qf_kind | qf_gentle;
    darath->Location(Keep("SH_CASTLE_ELINBRAND")->Location());
    darath->Available(true);
    AFoeRegimentAt(200, Keep("SH_CASTLE_ATHRUDAN")->Location());

    React(haraglai);

    REQUIRE( haraglai->reaction == RE_GATHER_STRENGTH );
    REQUIRE( haraglai->quest == QS_RECRUIT );
    REQUIRE( haraglai->questtarget == Idt(darath) );

    WHEN("he reaches Darath")
    {
        haraglai->Location(darath->Location());
        citadel_quest_processor().Process(haraglai);

        THEN("Darath follows him, and is still not yours")
        {
            REQUIRE( darath->Following() == haraglai );
            REQUIRE( darath->quest == QS_JOIN );
            REQUIRE_FALSE( darath->IsRecruited() );
            REQUIRE( haraglai->quest == QS_NONE );
        }
    }
}

SCENARIO("A lord already on a quest is not turned from it")
{
    TMEStep::NewStory();
    auto haraglai = HaraglaiAtHome();
    haraglai->quest = QS_GOTO;
    haraglai->questtarget = MAKE_LOCID(140, 140);
    AFoeRegimentAt(200, haraglai->Location());

    React(haraglai);

    REQUIRE( haraglai->quest == QS_GOTO );
    REQUIRE( haraglai->questtarget == MAKE_LOCID(140, 140) );
}

SCENARIO("A lord left on his own walks home, and one in a fellowship stays with it")
{
    TMEStep::NewStory();
    auto haraglai = HaraglaiAtHome();
    auto skordroth = Keep("SH_CASTLE_SKORDROTH");

    WHEN("Haraglai is far from home")
    {
        haraglai->Location(Lord("CH_ILVAR")->Location());
        React(haraglai);

        THEN("he goes home")
        {
            REQUIRE( haraglai->reaction == RE_RETURN_HOME );
            REQUIRE( haraglai->quest == QS_GOTO );
            REQUIRE( haraglai->questtarget == Idt(skordroth) );
        }
    }

    WHEN("Darath follows him to Theodel")
    {
        auto darath = Lord("CH_DARATH");
        darath->Location(haraglai->Location());
        REQUIRE( darath->Cmd_Follow(haraglai) == MX_OK );
        haraglai->Location(Keep("SH_CASTLE_THEODEL")->Location());
        darath->Location(haraglai->Location());
        React(darath);

        THEN("Darath stays with him")
        {
            REQUIRE( darath->Following() == haraglai );
            REQUIRE( darath->quest != QS_GOTO );
        }
    }
}

SCENARIO("When Boroth falls for good, his lieutenants pour out of the Marish")
{
    TMEStep::NewStory();
    auto yrgreth = Lord("CH_YRGRETH");
    yrgreth->Location(Keep("SH_CASTLE_SKORDROTH")->Location());
    auto boroth = GetCharacter("CH_BOROTH");

    WHEN("Boroth lives")
    {
        React(yrgreth);

        THEN("Yrgreth goes home to Gliwain")
        {
            REQUIRE( yrgreth->quest == QS_GOTO );
            REQUIRE( yrgreth->questtarget == Idt(Keep("SH_CASTLE_GLIWAIN")) );
        }
    }

    WHEN("Boroth is dead, but will rise again in the Dark Citadel")
    {
        boroth->Flags().Reset(cf_alive);
        React(yrgreth);

        THEN("Yrgreth still goes home")
        {
            REQUIRE( yrgreth->questtarget == Idt(Keep("SH_CASTLE_GLIWAIN")) );
        }
    }

    WHEN("Boroth is dead, and has already risen once")
    {
        boroth->Flags().Reset(cf_alive);
        Citadel()->flags.Set(gf_wraith);
        React(yrgreth);

        THEN("Yrgreth marches on the nearest keep that is not the Dark Fey's")
        {
            REQUIRE( yrgreth->reaction == RE_ATTACK_ENEMY );
            REQUIRE( yrgreth->quest == QS_SEIZE );
            auto keep = StrongholdTarget(yrgreth->questtarget);
            REQUIRE( keep != nullptr );
            REQUIRE( keep->Allegiance() != RA_DARK_FEY );
        }
    }
}
