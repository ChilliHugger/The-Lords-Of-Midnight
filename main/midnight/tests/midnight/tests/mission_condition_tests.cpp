//
//  mission_condition_tests.cpp
//  midnight
//

#include "../steps/common_steps.h"
#include "../../steps/map_steps.h"
#include "../../mocks/mocks_entity.h"


namespace {

    // SH_KEEP_OF_FADRATH and SH_KEEP_OF_DROON have no Owner/Occupier set in
    // the TSV data, so MakeChangeSides-driven "fallen" tests aren't
    // entangled with the "owner keeps it free" special case.
    constexpr LPCSTR sh_fadrath = "SH_KEEP_OF_FADRATH";
    constexpr LPCSTR sh_droon = "SH_KEEP_OF_DROON";

    // SH_CITADEL_GLOOM is occupied by CH_GLOOM in the TSV data.
    constexpr LPCSTR sh_gloom = "SH_CITADEL_GLOOM";
    constexpr LPCSTR ch_gloom = "CH_GLOOM";

    // Exposes mxmission's protected fields, so fully synthetic missions can
    // be built to exercise item_victories.cpp's condition logic directly,
    // rather than only through the handful of real conditions the LOM
    // scenario's own missions.tsv happens to use.
    class TestableMission : public mxmission
    {
    public:
        TestableMission()
        {
            priority = 0;
            objective = MO_MAJOR;
            condition = MC_NONE;
            for ( auto& r : reference ) r = 0;
            points = 0;
            scorer = 0;
            action = MA_NONE;
            actionid = 0;
        }

        void SetCondition(m_condition_t c) { condition = c; }
        void SetObjective(m_objective_t o) { objective = o; }
        void SetAny(bool any) { if ( any ) Flags().Set(mf_any); else Flags().Reset(mf_any); }
        void SetAction(m_action_t a, mxid id) { action = a; actionid = id; }

        void SetReferences(std::initializer_list<mxid> refs)
        {
            size_t ii = 0;
            for ( auto r : refs ) reference[ii++] = r;
            for ( ; ii < NUMELE(reference); ii++ ) reference[ii] = 0;
        }
    };

    class TestableVictory : public mxvictory
    {
    public:
        void SetMission(const mxmission* m) { mission = mxentity::SafeIdt(m); }
    };

}


SCENARIO("A mission with no condition never completes")
{
    TMEStep::NewStory();

    GIVEN("a mission with condition MC_NONE")
    {
        TestableMission mission;
        mission.SetCondition(MC_NONE);

        WHEN("it is checked")
        {
            auto result = mission.CheckComplete();

            THEN("it does not complete")
            {
                REQUIRE_FALSE( result );
                REQUIRE_FALSE( mission.IsComplete() );
            }
        }
    }
}

SCENARIO("A stronghold-taken-by-race mission completes when the stronghold matches")
{
    TMEStep::NewStory();

    GIVEN("a mission for a stronghold falling to a given race")
    {
        auto stronghold = GetStronghold(sh_fadrath);
        REQUIRE( stronghold->OccupyingRace() == RA_FREE );

        TestableMission mission;
        mission.SetCondition(MC_STRONGHOLD_TAKEN);
        mission.SetReferences({ mxentity::SafeIdt(stronghold), MAKE_ID(IDT_RACEINFO, RA_FREE) });

        WHEN("it is checked")
        {
            auto result = mission.CheckComplete();

            THEN("it completes")
            {
                REQUIRE( result );
                REQUIRE( mission.IsComplete() );
            }
        }
    }
}

SCENARIO("A stronghold-taken-by-race mission fails when a different race holds it")
{
    TMEStep::NewStory();

    GIVEN("a mission for a stronghold falling to a race that does not hold it")
    {
        auto stronghold = GetStronghold(sh_fadrath);
        REQUIRE( stronghold->OccupyingRace() == RA_FREE );

        TestableMission mission;
        mission.SetCondition(MC_STRONGHOLD_TAKEN);
        mission.SetReferences({ mxentity::SafeIdt(stronghold), MAKE_ID(IDT_RACEINFO, RA_DOOMGUARD) });

        WHEN("it is checked")
        {
            auto result = mission.CheckComplete();

            THEN("it does not complete")
            {
                REQUIRE_FALSE( result );
            }
        }
    }
}

SCENARIO("A stronghold-taken-by-character mission completes when that character occupies it")
{
    TMEStep::NewStory();

    GIVEN("a mission for a stronghold falling to its actual occupier")
    {
        auto stronghold = GetStronghold(sh_gloom);
        auto occupier = GetCharacter(ch_gloom);
        REQUIRE( stronghold->Occupier() == occupier );

        TestableMission mission;
        mission.SetCondition(MC_STRONGHOLD_TAKEN);
        mission.SetReferences({ mxentity::SafeIdt(stronghold), mxentity::SafeIdt(occupier) });

        WHEN("it is checked")
        {
            auto result = mission.CheckComplete();

            THEN("it completes")
            {
                REQUIRE( result );
            }
        }
    }
}

SCENARIO("A stronghold-taken-by-character mission fails when a different character occupies it")
{
    TMEStep::NewStory();

    GIVEN("a mission for a stronghold falling to someone other than its actual occupier")
    {
        auto stronghold = GetStronghold(sh_gloom);
        auto someoneElse = GetCharacter(TMEStep::ch_morkin);
        REQUIRE_FALSE( stronghold->Occupier() == someoneElse );

        TestableMission mission;
        mission.SetCondition(MC_STRONGHOLD_TAKEN);
        mission.SetReferences({ mxentity::SafeIdt(stronghold), mxentity::SafeIdt(someoneElse) });

        WHEN("it is checked")
        {
            auto result = mission.CheckComplete();

            THEN("it does not complete")
            {
                REQUIRE_FALSE( result );
            }
        }
    }
}

SCENARIO("A stronghold-fallen mission requires every referenced stronghold to have fallen")
{
    TMEStep::NewStory();

    GIVEN("two strongholds, only one of which has fallen")
    {
        auto fallen = GetStronghold(sh_fadrath);
        auto notFallen = GetStronghold(sh_droon);
        fallen->MakeChangeSides(RA_DOOMGUARD, nullptr);
        REQUIRE( fallen->HasFallen() );
        REQUIRE_FALSE( notFallen->HasFallen() );

        TestableMission mission;
        mission.SetCondition(MC_STRONGHOLD_FALLEN);
        mission.SetReferences({ mxentity::SafeIdt(fallen), mxentity::SafeIdt(notFallen) });

        WHEN("it is checked, requiring all of them")
        {
            auto result = mission.CheckComplete();

            THEN("it does not complete")
            {
                REQUIRE_FALSE( result );
            }
        }
    }
}

SCENARIO("A stronghold-fallen mission with ANY completes when just one has fallen")
{
    TMEStep::NewStory();

    GIVEN("two strongholds, only one of which has fallen")
    {
        auto fallen = GetStronghold(sh_fadrath);
        auto notFallen = GetStronghold(sh_droon);
        fallen->MakeChangeSides(RA_DOOMGUARD, nullptr);

        TestableMission mission;
        mission.SetCondition(MC_STRONGHOLD_FALLEN);
        mission.SetAny(true);
        mission.SetReferences({ mxentity::SafeIdt(fallen), mxentity::SafeIdt(notFallen) });

        WHEN("it is checked, requiring any of them")
        {
            auto result = mission.CheckComplete();

            THEN("it completes")
            {
                REQUIRE( result );
            }
        }
    }
}

SCENARIO("A character-dead mission requires every referenced character to be dead")
{
    TMEStep::NewStory();

    GIVEN("two lords, only one of whom is dead")
    {
        TMEStep::LordIsDead(TMEStep::ch_morkin);

        TestableMission mission;
        mission.SetCondition(MC_CHARACTER_DEAD);
        mission.SetReferences({
            mxentity::SafeIdt(GetCharacter(TMEStep::ch_morkin)),
            mxentity::SafeIdt(GetCharacter(TMEStep::ch_luxor))
        });

        WHEN("it is checked, requiring all of them")
        {
            auto result = mission.CheckComplete();

            THEN("it does not complete")
            {
                REQUIRE_FALSE( result );
            }
        }
    }
}

SCENARIO("A character-dead mission with ANY completes when just one lord has died")
{
    TMEStep::NewStory();

    GIVEN("two lords, only one of whom is dead")
    {
        TMEStep::LordIsDead(TMEStep::ch_morkin);

        TestableMission mission;
        mission.SetCondition(MC_CHARACTER_DEAD);
        mission.SetAny(true);
        mission.SetReferences({
            mxentity::SafeIdt(GetCharacter(TMEStep::ch_morkin)),
            mxentity::SafeIdt(GetCharacter(TMEStep::ch_luxor))
        });

        WHEN("it is checked, requiring any of them")
        {
            auto result = mission.CheckComplete();

            THEN("it completes")
            {
                REQUIRE( result );
            }
        }
    }
}

SCENARIO("A mission-complete condition requires every referenced mission to be complete")
{
    TMEStep::NewStory();

    GIVEN("two real missions, only one of which has been completed")
    {
        auto done = GetEntity<mxmission>("MI_LUXOR_DEAD");
        auto notDone = GetEntity<mxmission>("MI_MORKIN_DEAD");
        done->Completed();
        REQUIRE( done->IsComplete() );
        REQUIRE_FALSE( notDone->IsComplete() );

        TestableMission mission;
        mission.SetCondition(MC_MISSION_COMPLETE);
        mission.SetReferences({ mxentity::SafeIdt(done), mxentity::SafeIdt(notDone) });

        WHEN("it is checked, requiring all of them")
        {
            auto result = mission.CheckComplete();

            THEN("it does not complete")
            {
                REQUIRE_FALSE( result );
            }
        }
    }
}

SCENARIO("A mission-complete condition with ANY completes when just one referenced mission is done")
{
    TMEStep::NewStory();

    GIVEN("two real missions, only one of which has been completed")
    {
        auto done = GetEntity<mxmission>("MI_LUXOR_DEAD");
        auto notDone = GetEntity<mxmission>("MI_MORKIN_DEAD");
        done->Completed();

        TestableMission mission;
        mission.SetCondition(MC_MISSION_COMPLETE);
        mission.SetAny(true);
        mission.SetReferences({ mxentity::SafeIdt(done), mxentity::SafeIdt(notDone) });

        WHEN("it is checked, requiring any of them")
        {
            auto result = mission.CheckComplete();

            THEN("it completes")
            {
                REQUIRE( result );
            }
        }
    }
}

SCENARIO("A same-location condition completes when a carried object is with its owner")
{
    TMEStep::NewStory();

    GIVEN("a lord carrying the referenced object")
    {
        auto lord = GetCharacter(TMEStep::ch_morkin);
        TMEStep::LordCarryingObject(TMEStep::ch_morkin, OB_WOLFSLAYER);
        REQUIRE( tme::mx->scenario->WhoHasObject(GetObject("OB_WOLFSLAYER")) == lord );

        TestableMission mission;
        mission.SetCondition(MC_SAME_LOCATION);
        mission.SetReferences({ mxentity::SafeIdt(lord), MAKE_ID(IDT_OBJECT, OB_WOLFSLAYER) });

        WHEN("it is checked")
        {
            auto result = mission.CheckComplete();

            THEN("it completes")
            {
                REQUIRE( result );
            }
        }
    }
}

SCENARIO("A same-location condition fails when the referenced object is elsewhere")
{
    TMEStep::NewStory();

    GIVEN("a lord, and the referenced object carried by someone else, elsewhere")
    {
        auto lord = GetCharacter(TMEStep::ch_morkin);
        TMEStep::LordAtLocation(TMEStep::ch_luxor, mxgridref(20, 20));
        TMEStep::LordCarryingObject(TMEStep::ch_luxor, OB_WOLFSLAYER);
        REQUIRE_FALSE( lord->Location() == GetCharacter(TMEStep::ch_luxor)->Location() );

        TestableMission mission;
        mission.SetCondition(MC_SAME_LOCATION);
        mission.SetReferences({ mxentity::SafeIdt(lord), MAKE_ID(IDT_OBJECT, OB_WOLFSLAYER) });

        WHEN("it is checked")
        {
            auto result = mission.CheckComplete();

            THEN("it does not complete")
            {
                REQUIRE_FALSE( result );
            }
        }
    }
}

SCENARIO("Destroy-object is not implemented, and never completes a mission")
{
    TMEStep::NewStory();

    GIVEN("a mission with condition MC_DESTROY_OBJECT")
    {
        TestableMission mission;
        mission.SetCondition(MC_DESTROY_OBJECT);

        WHEN("it is checked")
        {
            auto result = mission.CheckComplete();

            THEN("it does not complete - this condition is a stub")
            {
                REQUIRE_FALSE( result );
            }
        }
    }
}

SCENARIO("A disabled mission never completes, even if its condition holds")
{
    TMEStep::NewStory();

    GIVEN("a disabled mission whose character-dead condition is actually met")
    {
        TMEStep::LordIsDead(TMEStep::ch_morkin);

        TestableMission mission;
        mission.SetCondition(MC_CHARACTER_DEAD);
        mission.SetReferences({ mxentity::SafeIdt(GetCharacter(TMEStep::ch_morkin)) });
        mission.Flags().Set(et_disabled);

        WHEN("it is checked")
        {
            auto result = mission.CheckComplete();

            THEN("it does not complete")
            {
                REQUIRE_FALSE( result );
            }
        }
    }
}

SCENARIO("A completed, non-flag mission is not re-scored")
{
    TMEStep::NewStory();

    GIVEN("a major-objective mission that is already marked complete")
    {
        TestableMission mission;
        mission.SetObjective(MO_MAJOR);
        mission.SetCondition(MC_CHARACTER_DEAD);
        mission.SetReferences({ mxentity::SafeIdt(GetCharacter(TMEStep::ch_morkin)) });
        mission.Flags().Set(mf_complete);

        WHEN("it is checked again")
        {
            auto result = mission.CheckComplete();

            THEN("it reports not-complete, as it has already been scored once")
            {
                REQUIRE_FALSE( result );
            }
        }
    }
}

SCENARIO("A flag-objective mission re-evaluates every time, and can un-mark itself")
{
    TMEStep::NewStory();

    GIVEN("a flag mission that was true, but whose condition is no longer met")
    {
        TMEStep::LordAtLocation(TMEStep::ch_luxor, mxgridref(20, 20));
        TMEStep::LordCarryingObject(TMEStep::ch_luxor, OB_WOLFSLAYER);

        TestableMission mission;
        mission.SetObjective(MO_FLAG);
        mission.SetCondition(MC_SAME_LOCATION);
        mission.SetReferences({ mxentity::SafeIdt(GetCharacter(TMEStep::ch_morkin)), MAKE_ID(IDT_OBJECT, OB_WOLFSLAYER) });
        mission.Flags().Set(mf_complete);

        WHEN("it is checked again")
        {
            auto result = mission.CheckComplete();

            THEN("it is no longer complete")
            {
                REQUIRE_FALSE( result );
                REQUIRE_FALSE( mission.IsComplete() );
            }
        }
    }
}

SCENARIO("Completing a mission with an enable action re-enables the target")
{
    TMEStep::NewStory();

    GIVEN("a disabled character, and a mission that enables them")
    {
        auto target = GetCharacter(TMEStep::ch_rorthron);
        target->Flags().Set(et_disabled);

        TestableMission mission;
        mission.SetAction(MA_ENABLE, mxentity::SafeIdt(target));

        WHEN("the mission completes")
        {
            mission.Completed();

            THEN("the target is no longer disabled")
            {
                REQUIRE_FALSE( target->IsDisabled() );
            }
        }
    }
}

SCENARIO("Completing a mission with a disable action disables the target and strips it if carried")
{
    TMEStep::NewStory();

    GIVEN("a lord carrying an object, and a mission that disables that object")
    {
        auto lord = GetCharacter(TMEStep::ch_morkin);
        TMEStep::LordCarryingObject(TMEStep::ch_morkin, OB_WOLFSLAYER);
        auto object = GetObject("OB_WOLFSLAYER");

        TestableMission mission;
        mission.SetAction(MA_DISABLE, mxentity::SafeIdt(object));

        WHEN("the mission completes")
        {
            mission.Completed();

            THEN("the object is disabled and no longer carried")
            {
                REQUIRE( object->IsDisabled() );
                REQUIRE( lord->Carrying() == nullptr );
            }
        }
    }
}

SCENARIO("A disabled victory never completes, even if its mission is done")
{
    TMEStep::NewStory();

    GIVEN("a disabled victory linked to a completed mission")
    {
        auto realMission = GetEntity<mxmission>("MI_LUXOR_DEAD");
        realMission->Completed();

        TestableVictory victory;
        victory.SetMission(realMission);
        victory.Flags().Set(et_disabled);

        WHEN("it is checked")
        {
            auto result = victory.CheckComplete();

            THEN("it does not complete")
            {
                REQUIRE_FALSE( result );
            }
        }
    }
}

SCENARIO("A victory completes once its linked mission is complete")
{
    TMEStep::NewStory();

    GIVEN("a victory linked to a mission that has just been completed")
    {
        auto realMission = GetEntity<mxmission>("MI_LUXOR_DEAD");
        realMission->Completed();

        TestableVictory victory;
        victory.SetMission(realMission);

        WHEN("it is checked")
        {
            auto result = victory.CheckComplete();

            THEN("it completes")
            {
                REQUIRE( result );
                REQUIRE( victory.IsComplete() );
            }
        }
    }
}

SCENARIO("A victory does not complete while its linked mission remains incomplete")
{
    TMEStep::NewStory();

    GIVEN("a victory linked to a mission that has not been completed")
    {
        auto realMission = GetEntity<mxmission>("MI_LUXOR_DEAD");

        TestableVictory victory;
        victory.SetMission(realMission);

        WHEN("it is checked")
        {
            auto result = victory.CheckComplete();

            THEN("it does not complete")
            {
                REQUIRE_FALSE( result );
            }
        }
    }
}
