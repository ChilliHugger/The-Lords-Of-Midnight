//
//  battle_resolution_tests.cpp
//  midnight
//

#include "../steps/common_steps.h"
#include "../../steps/map_steps.h"
#include "../../mocks/mocks_entity.h"
#include "../../../Source/tme/scenarios/lom/lom_processor_battle.h"


namespace {

    const mxgridref k_here = mxgridref(1, 1);

    // Exposes lom_battle's protected internals for direct testing - only
    // ProcessLocation() is public, and a full end-to-end battle needs a lot
    // of real map/army state to reach the numeric core (Fight) at all.
    class TestableLomBattle : public lom_battle
    {
    public:
        using lom_battle::Fight;
        using lom_battle::HasDefenders;
        using lom_battle::TakesPart;
        using lom_battle::CalculateBattleStatus;
        using lom_battle::UpdateStrongholdArmy;
        using lom_battle::UpdateCharacterArmy;
        using lom_battle::CharacterWinsBattle;
        using lom_battle::CharacterLosesBattle;
        using lom_battle::CharacterContinuesBattle;

        c_army& Foes() { return foes; }
        c_army& Friends() { return friends; }
        mxbattlestatus_t Status() const { return status; }
        void SetStatus(mxbattlestatus_t s) { status = s; }

        void SetInfo(mxgridref loc)
        {
            location = loc;
            info.reset(new mxlocinfo(loc, nullptr, slf_none));
        }
    };

    mxarmy* MakeArmy(mxitem* parent, mxarmytype_t type, mxrace_t race, u32 total, u32 success)
    {
        auto army = new mxarmy();
        army->parent = parent;
        army->armytype = type;
        army->race = race;
        army->total = total;
        army->success = success;
        army->killed = 0;
        return army;
    }

}


SCENARIO("Fight does nothing with no attacks to make")
{
    TMEStep::NewStory();

    GIVEN("an attacker with no attacks left, and a foe to fight")
    {
        TestableLomBattle battle;
        c_army foes;
        foes.push_back(MakeArmy(nullptr, AT_REGIMENT, RA_ENEMY, 10, 50));

        WHEN("they fight")
        {
            auto killed = battle.Fight(0, 200, foes);

            THEN("nothing happens")
            {
                REQUIRE( killed == 0 );
                REQUIRE( foes[0]->total == 10 );
            }
        }
    }
}

SCENARIO("Fight does nothing with no foes to fight")
{
    TMEStep::NewStory();

    GIVEN("an attacker with attacks to make, but no foes")
    {
        TestableLomBattle battle;
        c_army foes;

        WHEN("they fight")
        {
            auto killed = battle.Fight(3, 200, foes);

            THEN("nothing happens")
            {
                REQUIRE( killed == 0 );
            }
        }
    }
}

SCENARIO("A successful engagement and kill reduces the foe's troops")
{
    TMEStep::NewStory();

    GIVEN("a foe with 10 troops and a defensive success of 50")
    {
        TestableLomBattle battle;
        c_army foes;
        foes.push_back(MakeArmy(nullptr, AT_REGIMENT, RA_ENEMY, 10, 50));

        WHEN("the engagement and kill rolls both succeed")
        {
            ScopedRandom random({ 100, 0, 200 }); // engage (100<200), pick foe 0, kill (200>50)
            auto killed = battle.Fight(1, 200, foes);

            THEN("up to five of the foe's troops are killed")
            {
                REQUIRE( killed == 5 );
                REQUIRE( foes[0]->total == 5 );
                REQUIRE_FALSE( foes.empty() );
            }
        }
    }
}

SCENARIO("A failed engagement roll means no attack happens at all")
{
    TMEStep::NewStory();

    GIVEN("a foe with 10 troops")
    {
        TestableLomBattle battle;
        c_army foes;
        foes.push_back(MakeArmy(nullptr, AT_REGIMENT, RA_ENEMY, 10, 50));

        WHEN("the engagement roll is too high to succeed")
        {
            ScopedRandom random({ 250 }); // 250 is not < success(200), so never engages
            auto killed = battle.Fight(1, 200, foes);

            THEN("the foe is untouched")
            {
                REQUIRE( killed == 0 );
                REQUIRE( foes[0]->total == 10 );
            }
        }
    }
}

SCENARIO("Engaging but failing the kill roll leaves the foe unharmed")
{
    TMEStep::NewStory();

    GIVEN("a foe with 10 troops and a defensive success of 50")
    {
        TestableLomBattle battle;
        c_army foes;
        foes.push_back(MakeArmy(nullptr, AT_REGIMENT, RA_ENEMY, 10, 50));

        WHEN("the engagement succeeds but the kill roll does not")
        {
            ScopedRandom random({ 100, 0, 10 }); // engage, pick foe 0, 10 is not > success(50)
            auto killed = battle.Fight(1, 200, foes);

            THEN("the foe is untouched")
            {
                REQUIRE( killed == 0 );
                REQUIRE( foes[0]->total == 10 );
            }
        }
    }
}

SCENARIO("A foe whose troops are exhausted is removed from the fight")
{
    TMEStep::NewStory();

    GIVEN("a foe with only 3 troops left, and a defensive success of zero")
    {
        TestableLomBattle battle;
        c_army foes;
        foes.push_back(MakeArmy(nullptr, AT_REGIMENT, RA_ENEMY, 3, 0));

        WHEN("the attack kills the last of them")
        {
            ScopedRandom random({ 100, 0, 255 }); // engage, pick foe 0, 255>0 kills
            auto killed = battle.Fight(1, 200, foes);

            THEN("all their remaining troops are killed and they leave the fight")
            {
                REQUIRE( killed == 3 );
                REQUIRE( foes.empty() );
            }
        }
    }
}

SCENARIO("Fighting across multiple attacks can eliminate more than one foe")
{
    TMEStep::NewStory();

    GIVEN("two foes, each with 3 troops and no defence")
    {
        TestableLomBattle battle;
        c_army foes;
        auto foeA = MakeArmy(nullptr, AT_REGIMENT, RA_ENEMY, 3, 0);
        auto foeB = MakeArmy(nullptr, AT_REGIMENT, RA_ENEMY, 3, 0);
        foes.push_back(foeA);
        foes.push_back(foeB);

        WHEN("two attacks each engage, pick a foe, and kill")
        {
            // attack 1: engage, pick index 1 (foeB), kill -> foeB removed, foeA now at index 0
            // attack 2: engage, pick index 0 (foeA), kill -> foeA removed
            ScopedRandom random({ 100, 1, 255,   100, 0, 255 });
            auto killed = battle.Fight(2, 200, foes);

            THEN("both foes are eliminated")
            {
                REQUIRE( killed == 6 );
                REQUIRE( foes.empty() );
            }
        }
    }
}

SCENARIO("There are no defenders when no enemy armies are present")
{
    TMEStep::NewStory();

    GIVEN("a location with no enemy armies")
    {
        MapStep::ResetLocation(k_here);

        TestableLomBattle battle;
        battle.SetInfo(k_here);

        THEN("there are no defenders")
        {
            REQUIRE_FALSE( battle.HasDefenders() );
        }
    }
}

SCENARIO("There are defenders when an enemy regiment occupies the location")
{
    TMEStep::NewStory();

    GIVEN("a location held by an enemy regiment")
    {
        MapStep::ResetLocation(k_here);
        TMEStep::RegimentAtLocation(k_here, 10);

        TestableLomBattle battle;
        battle.SetInfo(k_here);

        THEN("there are defenders")
        {
            REQUIRE( battle.HasDefenders() );
        }
    }
}

SCENARIO("An ordinary character takes part in battle")
{
    TMEStep::NewStory();

    GIVEN("an ordinary lord")
    {
        auto lord = GetCharacter(TMEStep::ch_morkin);

        THEN("they take part")
        {
            TestableLomBattle battle;
            REQUIRE( battle.TakesPart(lord) );
        }
    }
}

SCENARIO("The Lord of Midwinter never takes part in battle")
{
    TMEStep::NewStory();

    GIVEN("the Lord of Midwinter")
    {
        auto midwinter = GetCharacter("CH_MIDWINTER");

        THEN("he does not take part")
        {
            TestableLomBattle battle;
            REQUIRE_FALSE( battle.TakesPart(midwinter) );
        }
    }
}

SCENARIO("Battle status reflects which side, if either, has been wiped out")
{
    TMEStep::NewStory();

    GIVEN("a battle with both sides still standing")
    {
        TestableLomBattle battle;
        battle.Friends().push_back(MakeArmy(nullptr, AT_REGIMENT, RA_FREE, 10, 50));
        battle.Foes().push_back(MakeArmy(nullptr, AT_REGIMENT, RA_ENEMY, 10, 50));

        WHEN("battle status is calculated")
        {
            battle.CalculateBattleStatus();

            THEN("the battle continues")
            {
                REQUIRE( battle.Status() == BA_CONTINUES );
            }
        }
    }

    GIVEN("a battle where the foe has been wiped out")
    {
        TestableLomBattle battle;
        battle.Friends().push_back(MakeArmy(nullptr, AT_REGIMENT, RA_FREE, 10, 50));

        WHEN("battle status is calculated")
        {
            battle.CalculateBattleStatus();

            THEN("the friendly side has won")
            {
                REQUIRE( battle.Status() == BA_FRIEND );
            }
        }
    }

    GIVEN("a battle where the friendly side has been wiped out")
    {
        TestableLomBattle battle;
        battle.Foes().push_back(MakeArmy(nullptr, AT_REGIMENT, RA_ENEMY, 10, 50));

        WHEN("battle status is calculated")
        {
            battle.CalculateBattleStatus();

            THEN("the foe has won")
            {
                REQUIRE( battle.Status() == BA_FOE );
            }
        }
    }
}

SCENARIO("A character who continues the battle is flagged as being in it")
{
    TMEStep::NewStory();

    GIVEN("a lord")
    {
        auto lord = GetCharacter(TMEStep::ch_morkin);

        WHEN("they continue the battle")
        {
            TestableLomBattle battle;
            battle.CharacterContinuesBattle(lord);

            THEN("they are flagged as in battle")
            {
                REQUIRE( lord->IsInBattle() );
            }
        }
    }
}

SCENARIO("A character who wins the battle is flagged as having won it")
{
    TMEStep::NewStory();

    GIVEN("a lord")
    {
        auto lord = GetCharacter(TMEStep::ch_morkin);

        WHEN("they win the battle")
        {
            TestableLomBattle battle;
            battle.CharacterWinsBattle(lord);

            THEN("they are flagged as having won")
            {
                REQUIRE( lord->HasWonBattle() );
            }
        }
    }
}

SCENARIO("An enemy stronghold overrun by the friendly side changes sides to the Free")
{
    TMEStep::NewStory();

    GIVEN("an enemy stronghold whose defenders have all been killed")
    {
        auto stronghold = TMEStep::StrongholdAtLocation(k_here, UT_WARRIORS, RA_DOOMGUARD, 500, 100, 1200);
        auto army = MakeArmy(stronghold, AT_STRONGHOLD, RA_ENEMY, 0, 0);
        army->killed = 500;

        TestableLomBattle battle;
        battle.SetStatus(BA_FRIEND);

        WHEN("its army is updated")
        {
            battle.UpdateStrongholdArmy(army);

            THEN("it changes sides to the Free")
            {
                REQUIRE( stronghold->OccupyingRace() == RA_FREE );
            }
        }
    }
}

SCENARIO("A friendly stronghold overrun by the enemy changes sides to the enemy")
{
    TMEStep::NewStory();

    GIVEN("a friendly stronghold whose defenders have all been killed")
    {
        auto stronghold = TMEStep::StrongholdAtLocation(k_here, UT_WARRIORS, RA_FREE, 500, 100, 1200);
        auto army = MakeArmy(stronghold, AT_STRONGHOLD, RA_FREE, 0, 0);
        army->killed = 500;

        TestableLomBattle battle;
        battle.SetStatus(BA_FOE);

        WHEN("its army is updated")
        {
            battle.UpdateStrongholdArmy(army);

            THEN("it changes sides to the enemy")
            {
                REQUIRE( stronghold->OccupyingRace() == RA_DOOMGUARD );
            }
        }
    }
}

SCENARIO("Updating a character's army drains their energy by the standard battle amount")
{
    TMEStep::NewStory();

    GIVEN("a lord whose warriors fought in a battle")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        lord->warriors.Total(100);
        lord->warriors.Energy(100);

        auto army = MakeArmy(lord, AT_CHARACTER, lord->NormalisedLoyalty(), 80, 0);
        army->type = UT_WARRIORS;
        army->killed = 20;

        TestableLomBattle battle;

        WHEN("their army is updated")
        {
            battle.UpdateCharacterArmy(army);

            THEN("their warriors reflect the battle's outcome, with energy drained")
            {
                REQUIRE( lord->warriors.Total() == 80 );
                REQUIRE( lord->warriors.Lost() == 20 );
                REQUIRE( lord->warriors.Killed() == 20 );
                REQUIRE( lord->warriors.Energy() == 100 - (u32) sv_battle_default_energy_drain );
            }
        }
    }
}
