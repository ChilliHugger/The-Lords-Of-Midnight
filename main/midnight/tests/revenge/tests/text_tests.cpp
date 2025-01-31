//
//  text_tests.cpp
//  revenge
//
//  Created by Chris Wild on 30/01/2025.
//

#include "../steps/common_steps.h"
#include "../../../Source/tme/scenarios/ddr/ddr_processor_text.h"

string result;


const string SlainByObject = "Tarithel the Fey is dead, slain by Shareth the Heartstealer with the %s.";
const string SlainByNasty = "Tarithel the Fey is dead, slain by %s.";
const string KilledInBattle = "Tarithel the Fey is dead, slain by sword. ";
const string SlainByLord = "Tarithel the Fey is dead, slain by Shareth the Heartstealer. ";

void TarithelSlainByObject(string& object)
{
    auto expected = StringExtensions::Format(SlainByObject.c_str(), object.c_str());
    REQUIRE_THAT(result, Catch::Matchers::StartsWith(expected));
}

void TarithelSlainByNasty(string& object)
{
    auto expected = StringExtensions::Format(SlainByNasty.c_str(), object.c_str());
    REQUIRE_THAT(result, Catch::Matchers::StartsWith(expected));
}

void TarithelSlainByLord()
{
    REQUIRE_THAT(result, Catch::Matchers::StartsWith(SlainByLord));
}

void TarithelKilledInBattle()
{
    REQUIRE_THAT(result, Catch::Matchers::StartsWith(KilledInBattle));
}

SCENARIO("Describe Character Death by special object: SS_KILLED_BY_BATTLE_OBJECT")
{
    string symbol[] = { "OB_HAMMER_IMGARORN", "OB_SWORD_MALISANE", "OB_AXE_HAGILDRORN", "OB_SPEAR_CARORMAND" };
    string type[] = { "hammer of Imgarorn", "sword of Malisane", "axe of Hagildrorn", "spear of Carormand" };
    auto index = GENERATE(0, 1, 2, 3);

    TMEStep::NewStory();
    
    auto text = static_cast<ddr_text*>(tme::mx->text);
    
    
    GIVEN(StringExtensions::Format("Lord killed by battle '%s'", symbol[index].c_str()))
    {
        auto lord = GetDDRCharacter(DDRStep::ch_tarithel);
        auto enemy = GetDDRCharacter(DDRStep::ch_shareth);
        lord->killedbyobject = GetObject(symbol[index]);
        lord->fighting_against = enemy;
        lord->killedby = KB_LORD_OBJECT;
        lord->Flags().Set(cf_inbattle);
    
        WHEN("Death described") {
            result = text->DescribeCharacterDeath(lord);
            
            THEN("Tarithel the Fey is dead, slain by object") {
                TarithelSlainByObject(type[index]);
            }
        }
    }
}

SCENARIO("Describe Character Death by nasty: SS_KILLED_OBJECT")
{
    string symbol[] = { "OB_WOLVES", "OB_DRAGONS", "OB_ICETROLLS", "OB_SKULKRIN" };
    string type[] = { "wolves", "dragons", "ice trolls", "skullkrin" };
    auto index = GENERATE(0, 1, 2, 3);

    TMEStep::NewStory();
    
    auto text = static_cast<ddr_text*>(tme::mx->text);
    
    GIVEN(StringExtensions::Format("Lord killed by battle '%s'", symbol[index].c_str()))
    {
        auto lord = GetDDRCharacter(DDRStep::ch_tarithel);
        lord->killedbyobject = GetObject(symbol[index]);
        lord->killedby = KB_NASTY;

        WHEN("Death described") {
            result = text->DescribeCharacterDeath(lord);
            
            THEN("Tarithel the Fey is dead, slain by nasty") {
                TarithelSlainByNasty(type[index]);
            }
        }
    }
}

SCENARIO("Describe Character Death in battle: SS_KILLED_BATTLE")
{
    TMEStep::NewStory();
    
    auto text = static_cast<ddr_text*>(tme::mx->text);
    
    auto lord = GetDDRCharacter(DDRStep::ch_tarithel);
    auto enemy = GetDDRCharacter(DDRStep::ch_shareth);
    
    GIVEN("Lord killed in battle")
    {
        lord->fighting_against = enemy;
        lord->Flags().Set(cf_inbattle);
        lord->killedby = KB_ARMY;

        WHEN("Death described") {
            result = text->DescribeCharacterDeath(lord);
            
            THEN("She was killed in battle.") {
                TarithelKilledInBattle();
            }
        }
    }
}

SCENARIO("Describe Character Death in battle by lord: SS_KILLED_BATTLE")
{
    TMEStep::NewStory();
    
    auto text = static_cast<ddr_text*>(tme::mx->text);
    
    auto lord = GetDDRCharacter(DDRStep::ch_tarithel);
    auto enemy = GetDDRCharacter(DDRStep::ch_shareth);
    
    GIVEN("Lord killed in battle")
    {
        lord->fighting_against = enemy;
        lord->Flags().Set(cf_inbattle);
        lord->killedby = KB_LORD;

        WHEN("Death described") {
            result = text->DescribeCharacterDeath(lord);
            
            THEN("She was killed in battle.") {
                TarithelSlainByLord();
            }
        }
    }
}
