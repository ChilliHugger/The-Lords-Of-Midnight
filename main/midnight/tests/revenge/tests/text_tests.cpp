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
const string Guidance = R"(It is dawn. Here in the Forest of Fangrim, Tarithel the Fey has found guidance, "%s)";

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

void GuidanceIs(LPCSTR text) {
    auto expected = StringExtensions::Format(Guidance.c_str(), text);
    REQUIRE_THAT(result, Catch::Matchers::StartsWith(expected));
}

void SharethIsDead() {
    GuidanceIs("Shareth the Heartstealer is dead.");
}

void SharethLocationGiven() {
    GuidanceIs("Shareth the Heartstealer stands in the City of Imgaril.");
}

void DesiredObjectDescribed() {
    GuidanceIs("The Spell of Thigrorn, whose power is in swiftness, can be found");
}

void GuidanceAboutImgormad() {
    auto rnd = new MockRandom();
    randomno::instance = rnd;
    rnd->data[0] = 6; // CH_IMGORMAD
}

void GuidanceGiven(mxcharacter* character)
{
    tme::mx->text->oinfo = tme::mx->ObjectById(OB_GUIDANCE) ;
    tme::mx->scenario->GiveGuidance(character, 0);
    result = TME_LastActionMsg();
};

void WhenLordSees(mxcharacter* character)
{
    result = tme::mx->text->DescribeCharacterSees(character);
};

void ThenLordSees(LPCSTR text) {
    REQUIRE_THAT(result, Catch::Matchers::StartsWith(text));
}

void ThenLordSeesNothing() {
    REQUIRE_THAT(result, Catch::Matchers::Equals(""));
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

SCENARIO("Character finds guidance about a dead lord")
{
    TMEStep::NewStory();
    
    auto text = static_cast<ddr_text*>(tme::mx->text);
    auto character = GetDDRCharacter(DDRStep::ch_tarithel);
 
    GIVEN("The lord will find guidance about a dead lord")
    {
        GuidanceAboutImgormad();
        TMEStep::LordIsDead("CH_IMGORMAD");

        AND_GIVEN("The lord is carrying their desired object")
        {
            TMEStep::LordCarryingObject(DDRStep::ch_tarithel, character->desired_object->Symbol());
            
            AND_GIVEN("Shareth is dead")
            {
                TMEStep::LordIsDead(DDRStep::ch_shareth);
                                
                WHEN("Guidance is given")
                {
                    GuidanceGiven(character);
                    
                    THEN("Shareth will be described as dead")
                    {
                        SharethIsDead();
                    }
                }
            }
            
            AND_GIVEN("Shareth is alive")
            {
                WHEN("Guidance is given")
                {
                    GuidanceGiven(character);
                    
                    THEN("Shareth's location given")
                    {
                        SharethLocationGiven();
                    }
                }
            }

        }

        AND_GIVEN("The lord is not carrying their desired object")
        {
            WHEN("Guidance is given")
            {
                GuidanceGiven(character);
         
                THEN("Desired object should be described")
                {
                    DesiredObjectDescribed();
                }
            }
        }

        AND_GIVEN("The lord has no desired object")
        {
            character->desired_object = nullptr;

            AND_GIVEN("Shareth is dead")
            {
                TMEStep::LordIsDead(DDRStep::ch_shareth);
                                
                WHEN("Guidance is given")
                {
                    GuidanceGiven(character);
                    
                    THEN("Shareth will be described as dead")
                    {
                        SharethIsDead();
                    }
                }
            }
            
            AND_GIVEN("Shareth is alive")
            {
                WHEN("Guidance is given")
                {
                    GuidanceGiven(character);
                    
                    THEN("Shareth's location given")
                    {
                        SharethLocationGiven();
                    }
                }
            }
        }
    }
}

SCENARIO("Character finds guidance about a live lord")
{
    TMEStep::NewStory();
    
    auto text = static_cast<ddr_text*>(tme::mx->text);
    auto character = GetDDRCharacter(DDRStep::ch_tarithel);
 
    GIVEN("The lord should find guidance about a live lord")
    {
        GuidanceAboutImgormad();
                        
        WHEN("Guidance is given")
        {
            GuidanceGiven(character);
            
            THEN("the lords location will be shown")
            {
                GuidanceIs("Imgormad the Giant stands at the Fortress of Imgormad.");
            }
        }
    }
}

SCENARIO("Character Sees something at a location") {
    TMEStep::NewStory();
    
    auto character = GetCharacter(TMEStep::ch_luxor);

    GIVEN("Lord is at a location with a tunnel entrance") {
        DDRStep::LordAtGateOfVarenorn(character->Symbol().c_str());
        
        AND_GIVEN("there is no object to pickup") {
            // no objects on first pass after a new story
            
            WHEN("the lord 'sees'") {
                WhenLordSees(character);
            
                THEN("they will see the tunnel entrance") {
                    ThenLordSees("Luxor sees an underground entrance.");
                }
            }
        }
        
        AND_GIVEN("there is an object to pickup") {
            DDRStep::ObjectAtLocation("OB_CROWN_VARENAND", character->Location());
            
            WHEN("the lord 'sees'") {
                WhenLordSees(character);
                         
                THEN("they will see the tunnel entrance and the object") {
                    ThenLordSees("Luxor sees the Crown of Varenand, whose power is in persuasion and an underground entrance.");
                }
            }
        }
    }
    
    GIVEN("Lord is at a location without a tunnel entrance") {
        auto location = mxgridref(7, 90); // Tower of thirand
        TMEStep::LordAtLocation(character->Symbol().c_str(), location);
        
        AND_GIVEN("there is no object to pickup") {
            // no objects on first pass after a new story
            
            WHEN("the lord 'sees'") {
                WhenLordSees(character);
            
                THEN("they will see nothing") {
                    ThenLordSeesNothing();
                }
            }
        }
        
        AND_GIVEN("there is an object to pickup") {
            DDRStep::ObjectAtLocation("OB_CROWN_VARENAND", location);
            
            WHEN("the lord 'sees'") {
                WhenLordSees(character);
                         
                THEN("they will see the object") {
                    ThenLordSees("Luxor sees the Crown of Varenand, whose power is in persuasion.");
                }
            }
        }
    }
}
