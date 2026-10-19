//
//  lord_behaviour_tests.cpp
//  citadel
//
#include "../../steps/tme_steps.h"
#include "../../../Source/tme/scenarios/citadel/citadel_processor_quest.h"
#include "../../../Source/tme/scenarios/citadel/citadel_processor_battle.h"

#include <map>

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
        return processor.LandDanger(processor.PlaceOf(keep->Location()));
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
        if ( !lord->IsRecruited() && ( lord->Purpose() == PU_DEFEND_HOMELAND || lord->Purpose() == PU_BE_A_HOSTAGE ) )
            REQUIRE( lord->Home() != nullptr );
    }

    THEN("it is the keep he holds, or for a hostage the one the database names")
    {
        REQUIRE( Lord("CH_HARAGLAI")->Home() == Keep("SH_CASTLE_SKORDROTH") );
        REQUIRE( Lord("CH_DJALINA")->Home() == Keep("SH_CASTLE_THEODEL") );
    }
}

SCENARIO("A lord at home in a realm at peace stands firm")
{
    TMEStep::NewStory();
    auto haraglai = HaraglaiAtHome();

    React(haraglai);

    REQUIRE( DangerAt(Keep("SH_CASTLE_SKORDROTH")) == LS_AT_PEACE );
    REQUIRE( haraglai->Reaction() == RE_STAND_FIRM );
    REQUIRE( haraglai->Quest() == QS_REST );
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
            REQUIRE( haraglai->Reaction() == RE_RETREAT );
            REQUIRE( haraglai->Quest() == QS_GOTO );
            REQUIRE( haraglai->QuestTarget() != Idt(skordroth) );
        }
    }

    WHEN("he has ten thousand")
    {
        Army(haraglai, 10000);
        React(haraglai);

        THEN("he holds, and is no one's to call on")
        {
            REQUIRE( haraglai->Reaction() == RE_STAND_FIRM );
            REQUIRE( haraglai->Quest() == QS_REST );
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
            REQUIRE( haraglai->Reaction() == RE_STAND_FIRM );
            REQUIRE( haraglai->Quest() == QS_REST );
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

    REQUIRE( haraglai->Reaction() == RE_ATTACK_ENEMY );
    REQUIRE( haraglai->Quest() == QS_KILL );
    REQUIRE( haraglai->QuestTarget() == Idt(regiment) );
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
            REQUIRE( asunai->Quest() == QS_REST );
        }
    }

    GIVEN("that she is free")
    {
        FreeTheHostages();
        React(asunai);

        THEN("he goes to fight the regiment")
        {
            REQUIRE( asunai->Reaction() == RE_ATTACK_ENEMY );
            REQUIRE( asunai->Quest() == QS_KILL );
            REQUIRE( asunai->QuestTarget() == Idt(regiment) );
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
    REQUIRE( haraglai->Reaction() == RE_TAKE_BACK_STRONGHOLD );
    REQUIRE( haraglai->Quest() == QS_SEIZE );
    REQUIRE( haraglai->QuestTarget() == Idt(skordroth) );
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

    REQUIRE( haraglai->Reaction() == RE_HELP_NEIGHBOUR );
    REQUIRE( haraglai->Quest() == QS_JOIN );
    auto subject = CharacterTarget(haraglai->QuestTarget());
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
            REQUIRE( rorthron->Quest() == QS_NONE );
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
    darath->Flags().Set(cf_available);
    AFoeRegimentAt(200, Keep("SH_CASTLE_ATHRUDAN")->Location());

    React(haraglai);

    REQUIRE( haraglai->Reaction() == RE_GATHER_STRENGTH );
    REQUIRE( haraglai->Quest() == QS_RECRUIT );
    REQUIRE( haraglai->QuestTarget() == Idt(darath) );

    WHEN("he reaches Darath")
    {
        haraglai->Location(darath->Location());
        citadel_quest_processor().Process(haraglai);

        THEN("Darath follows him, and is still not yours")
        {
            REQUIRE( darath->Following() == haraglai );
            REQUIRE( darath->Quest() == QS_JOIN );
            REQUIRE_FALSE( darath->IsRecruited() );
            REQUIRE( haraglai->Quest() == QS_NONE );
        }
    }
}

SCENARIO("A lord already on a quest is not turned from it")
{
    TMEStep::NewStory();
    auto haraglai = HaraglaiAtHome();
    haraglai->Quest(QS_GOTO);
    haraglai->QuestTarget(MAKE_LOCID(140, 140));
    AFoeRegimentAt(200, haraglai->Location());

    React(haraglai);

    REQUIRE( haraglai->Quest() == QS_GOTO );
    REQUIRE( haraglai->QuestTarget() == MAKE_LOCID(140, 140) );
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
            REQUIRE( haraglai->Reaction() == RE_RETURN_HOME );
            REQUIRE( haraglai->Quest() == QS_GOTO );
            REQUIRE( haraglai->QuestTarget() == Idt(skordroth) );
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
            REQUIRE( darath->Quest() != QS_GOTO );
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
            REQUIRE( yrgreth->Quest() == QS_GOTO );
            REQUIRE( yrgreth->QuestTarget() == Idt(Keep("SH_CASTLE_GLIWAIN")) );
        }
    }

    WHEN("Boroth is dead, but will rise again in the Dark Citadel")
    {
        boroth->Flags().Reset(cf_alive);
        React(yrgreth);

        THEN("Yrgreth still goes home")
        {
            REQUIRE( yrgreth->QuestTarget() == Idt(Keep("SH_CASTLE_GLIWAIN")) );
        }
    }

    WHEN("Boroth is dead, and has already risen once")
    {
        GetCharacter("CH_BOROTH")->Flags().Reset(cf_alive);
        boroth->Flags().Reset(cf_alive);
        Citadel()->flags.Set(gf_wraith);
        React(yrgreth);

        THEN("Yrgreth marches on the nearest keep that is not the Dark Fey's")
        {
            REQUIRE( yrgreth->Reaction() == RE_ATTACK_ENEMY );
            REQUIRE( yrgreth->Quest() == QS_SEIZE );
            auto keep = StrongholdTarget(yrgreth->QuestTarget());
            REQUIRE( keep != nullptr );
            REQUIRE( keep->Allegiance() != RA_DARK_FEY );
        }
    }
}

SCENARIO("When two fellowships merge, the higher-ranked lord leads")
{
    TMEStep::NewStory();
    auto haraglai = HaraglaiAtHome();
    auto asunai = Lord("CH_ASUNAI");
    asunai->Location(haraglai->Location());

    REQUIRE( asunai->Rank() > haraglai->Rank() );

    WHEN("Haraglai joins the King of the Fallows")
    {
        haraglai->Quest(QS_JOIN);
        haraglai->QuestTarget(Idt(asunai));
        citadel_quest_processor().Process(haraglai);

        THEN("Haraglai follows him")
        {
            REQUIRE( haraglai->Following() == asunai );
        }
    }

    WHEN("the King joins Haraglai")
    {
        asunai->Quest(QS_JOIN);
        asunai->QuestTarget(Idt(haraglai));
        citadel_quest_processor().Process(asunai);

        THEN("it is still the King who leads")
        {
            REQUIRE( haraglai->Following() == asunai );
            REQUIRE_FALSE( asunai->IsFollowing() );
        }
    }

    WHEN("each sets out to win the other over")
    {
        haraglai->qualities = asunai->qualities = qf_kind | qf_gentle | qf_generous;
        haraglai->Quest(QS_RECRUIT);
        haraglai->QuestTarget(Idt(asunai));
        asunai->Quest(QS_RECRUIT);
        asunai->QuestTarget(Idt(haraglai));
        citadel_quest_processor().Process(haraglai);

        THEN("the King leads, and both quests are over")
        {
            REQUIRE( haraglai->Following() == asunai );
            REQUIRE( haraglai->Quest() == QS_NONE );
            REQUIRE( asunai->Quest() == QS_NONE );
        }
    }
}

SCENARIO("A lord on guard falls on any lord at war with his side, and on no one else")
{
    TMEStep::NewStory();
    auto rorthron = Lord("CH_RORTHRON");
    auto ilvar = Lord("CH_ILVAR");
    Army(rorthron, 1000);
    auto here = rorthron->Location();
    ilvar->Location(here + DR_NORTH);
    REQUIRE( rorthron->SetQuest(QS_GUARD, MAKE_LOCID(here.x, here.y)) );

    WHEN("the Lee are at peace with you")
    {
        TMEStep::NightFalls();

        THEN("he lets Ilvar be")
        {
            REQUIRE_FALSE( rorthron->Flags().Is(cf_wonbattle) );
            REQUIRE_FALSE( rorthron->Flags().Is(cf_inbattle) );
        }
    }

    WHEN("you are at war with the Lee")
    {
        Citadel()->DeclareWar(RA_FREE, RA_ATHELING);
        TMEStep::NightFalls();

        THEN("they have fought")
        {
            REQUIRE( ( rorthron->Flags().Is(cf_wonbattle) || rorthron->Flags().Is(cf_inbattle) ) );
        }
    }
}

SCENARIO("Only one siege is fought in a night: the other besieger waits")
{
    TMEStep::NewStory();
    for ( auto regiment : tme::mx->objRegiments )
        regiment->Total(0);
    auto ceremelth = Keep("SH_CASTLE_CEREMELTH");
    auto cerevere = Keep("SH_CASTLE_CEREVERE");
    for ( auto [symbol, keep] : { std::pair{ "CH_RORTHRON", ceremelth }, std::pair{ "CH_ARALETH", cerevere } } ) {
        auto lord = Lord(symbol);
        Army(lord, 3000);
        lord->Location(Beside(keep->Location(), lord));
        REQUIRE( lord->SetQuest(QS_SEIZE, Idt(keep)) );
    }
    std::map<mxstronghold*, u32> before { { ceremelth, ceremelth->TotalTroops() }, { cerevere, cerevere->TotalTroops() } };

    WHEN("the first night passes")
    {
        TMEStep::NightFalls();

        THEN("one keep is besieged and taken, and the other's garrison is untouched")
        {
            auto taken = ( ceremelth->Allegiance() == RA_FREE ) + ( cerevere->Allegiance() == RA_FREE );
            REQUIRE( taken == 1 );
            auto untouched = ceremelth->Allegiance() == RA_FREE ? cerevere : ceremelth;
            REQUIRE( untouched->TotalTroops() == before[untouched] );
        }

        AND_WHEN("the next night passes")
        {
            TMEStep::NightFalls();

            THEN("the other falls too")
            {
                REQUIRE( ceremelth->Allegiance() == RA_FREE );
                REQUIRE( cerevere->Allegiance() == RA_FREE );
            }
        }
    }
}

SCENARIO("A realm is at ransom while its hostage is held, unfettered when freed, at blood feud when killed, and in interregnum without its king")
{
    TMEStep::NewStory();
    auto djalina = Lord("CH_DJALINA");
    auto asunai = Lord("CH_ASUNAI");

    REQUIRE( Citadel()->KingOf(RA_USKARG) == asunai );
    REQUIRE( Citadel()->HostageOf(RA_USKARG) == djalina );
    REQUIRE( Citadel()->RealmStatus(RA_USKARG) == RS_AT_RANSOM );
    REQUIRE( Citadel()->RealmStatus(RA_GOLDEN_FEY) == RS_UNFETTERED );

    WHEN("Djalina is freed")
    {
        djalina->Flags().Reset(cf_prisoner);
        REQUIRE( Citadel()->RealmStatus(RA_USKARG) == RS_UNFETTERED );
    }

    WHEN("Djalina dies in the dungeons")
    {
        djalina->Flags().Reset(cf_alive);
        REQUIRE( Citadel()->RealmStatus(RA_USKARG) == RS_BLOOD_FEUD );

        THEN("the King of the Fallows no longer waits on her")
        {
            for ( auto regiment : tme::mx->objRegiments )
                regiment->Total(0);
            asunai->Location(Keep("SH_CITADEL_ROREON")->Location());
            Army(asunai, 1000);
            AFoeRegimentAt(200, Keep("SH_CASTLE_CERRELM")->Location());
            React(asunai);
            REQUIRE( asunai->Quest() == QS_KILL );
        }
    }

    WHEN("the King of the Fallows is dead")
    {
        asunai->Flags().Reset(cf_alive);
        REQUIRE( Citadel()->RealmStatus(RA_USKARG) == RS_INTERREGNUM );
    }
}
