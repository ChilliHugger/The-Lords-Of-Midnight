//
//  steps.h
//  midnight
//
//  Created by Chris Wild on 04/01/2021.
//
#pragma once


#include "catch2/catch.hpp"
#include "../../Source/system/moonring.h"
#include "../../Source/tme/tme_interface.h"
#include "../../Source/tme/baseinc/tme_internal.h"
#include "../mocks/mocks_entity.h"

#if defined(_LOM_)
    #include "../midnight/steps/common_steps.h"
#endif
#if defined(_DDR_)
    #include "../revenge/steps/common_steps.h"
#endif

#include <stdio.h>

using namespace tme;
using namespace std;
extern mxengine* mx;


#define NEW_TAG "[NEW]"
#define IGNORE_TAG "[ignore]"

class TMEStep
{
public:
    static void NewStory(mxscenarioid scenario, RULEFLAGS rules, mxdifficulty_t difficult);
    static void NewStory(RULEFLAGS rules = RF_DEFAULT, mxdifficulty_t difficulty = DF_NORMAL);
    
    
    static void GivenNormalGame() { NewStory(); }
    static void GivenHardGame() { NewStory(RF_DEFAULT, DF_HARD); }
    
    static void NightFalls();
    static void CurrentLordIsDead();
    static void SetCurrentLord(const string& name);
    static void GameLost();
    static void GameWon();
    
    static void LordCarryingObject(const string& name, mxthing_t thing);
    static void LordCarryingObject(const string& name, const string& object);
    static void LordNotCarryingObject(const string& name);

    static void LordAtLocation(const string& name, const string& location);
    static void LordAtLocation(const string& name, loc_t location);
    static void LordsAtSameLocation(const string& name1, const string& name2);

    // Unlike LordAtLocation, also resets the square (real map terrain there may be impassable) and sets looking/time/energy to a deterministic baseline.
    static mxcharacter* PlaceLordAt(const string& name, loc_t here, mxdir_t looking);
    
    static void LordIsNotRecruited(const string& name);
    static void LordIsRecruited(const string& name);
    static void LordIsRecruitedBy(const string& name, const string& recruiter);

    static void LordPerformsApproach(const string& name1, const string& name2 = "");
    static void LordsIsLookingAt(const string& name1, const string& name2);
    static void LordMovesForward(const string& name);
    static void LordFights(const string& name);
    static void LordIsDead(const string& name);
    static void LordKilledByObject(const string& name, const string& killedby);

    // Grouping
    static void LordHasFollowers(const string& lord, vector<string> names);

    // Regiments
    static mxregiment* RegimentAtLocation(loc_t location, u32 total = 10);

    // Strongholds - relocates a real stronghold matching the given type and
    // occupying race, and gives it the requested troop levels. Returns
    // nullptr if no such stronghold exists in the loaded scenario data.
    static mxstronghold* StrongholdAtLocation(loc_t location, mxunit_t type, mxrace_t occupyingRace,
                                               u32 totalTroops, u32 minTroops, u32 maxTroops);

    
    // mocks
    static void LordShouldDieInFight(const string& lord);
    static void LordShouldNotDieInFight(const string& lord);
    static void LordShouldLoseHorse(const string& lord);
    static void LordShouldNotLoseHorse(const string& lord);
    //
    
    
    //
    static bool LordHasBeenRecruited(const string& name);
    
    //
    static constexpr LPCSTR ch_morkin = "CH_MORKIN";
    static constexpr LPCSTR ch_luxor = "CH_LUXOR";
    static constexpr LPCSTR ch_rorthron = "CH_RORTHRON";
#if defined(_LOM_)
    static constexpr LPCSTR ch_shadows = "CH_SHADOWS";
#endif

    static constexpr LPCSTR ob_wolves = "OB_WOLVES";
};

template <typename T> T* GetEntity(const string& name)
{
    return static_cast<T*>(tme::mx->EntityByName(name));
}

inline mxcharacter* GetCharacter(const string& name)
{
    return GetEntity<mxcharacter>(name);
}

inline mxstronghold* GetStronghold(const string& name)
{
    return GetEntity<mxstronghold>(name);
}

inline mxobject* GetObject(const string& name)
{
    return GetEntity<mxobject>(name);
}

inline mockcharacter* GetMockCharacter(const string& lord)
{
    return static_cast<mockcharacter*>(GetCharacter(lord));
}

#if defined(_DDR_)
inline mockobject* GetMockObject(const string& object)
{
    return static_cast<mockobject*>(GetObject(object));
}

inline mockobject* GetMockThing(mxthing_t thing) {
    return static_cast<mockobject*>(tme::mx->EntityByIdt(MAKE_ID(IDT_OBJECT,thing)));
}

inline ddr_character* GetDDRCharacter(LPCSTR name)
{
    return static_cast<tme::ddr_character*>(GetCharacter(name));
}
#endif

class MockRandom : public randomno
{
public:
    MockRandom()
    {
        Reset();
        data.push_back(0);
    }

    virtual int get ( int number )
    {
        if ( count > data.size() ) {
            count = 0;
        }
        
        return data[count++];
    }

    void Reset() {
        count = 0;
    }

public:
    int count;
    vector<int> data;
};

// Swaps in a MockRandom for the lifetime of the scope, so real formulas that
// call mxrandom() can be exercised deterministically. Restores the original
// generator on destruction, even if a REQUIRE fails mid-scope.
class ScopedRandom
{
public:
    explicit ScopedRandom(vector<int> values)
    {
        saved = randomno::instance;
        mock.Reset();
        mock.data = values;
        randomno::instance = &mock;
    }

    ~ScopedRandom()
    {
        randomno::instance = saved;
    }

private:
    MockRandom mock;
    randomno* saved;
};
