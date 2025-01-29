//
//  stronghold_tests.cpp
//  revenge
//
//  Created by Chris Wild on 04/02/2024.
//

#include "../steps/common_steps.h"
#include "../../../Source/tme/scenarios/ddr/ddr_processor_text.h"


// GIVEN a stronghold has no owner
// AND is not occupied

// GIVEN a stronghold has no owner
// AND is occupied

// GIVEN a stronghold has an owner
// AND is not occupied

// GIVEN a stronghold has an owner
// AND is occupied by the owner

// GIVEN a stronghold has an owner
// AND is occupied by somone other than the owner

class ProxyStronghold : public mxstronghold {
public:
    void SetOwner( mxcharacter* owner ) { this->owner = owner; }
    void SetOccupier( mxcharacter* occupier ) { this->occupier = occupier; }
    void SetTroopType( mxunit_t type ) { this->type = type; }
};
    
static std::string description;

void ShouldShowOwnedByVaratrarg()
{
    REQUIRE_THAT( description, Catch::Matchers::Contains("Fortress is owned by Varatrarg the Barbarian who is loyal to the Barbarians."));
}

void ShouldShowOccupiedByTarithel()
{
    REQUIRE_THAT( description, Catch::Matchers::Contains("Fortress is owned by Tarithel the Fey who is loyal to the Moonprince."));
}

void ShouldShowOccupiedByLuxor()
{
    REQUIRE_THAT( description, Catch::Matchers::Contains("Fortress is owned by Luxor the Moonprince."));
}


void ShouldShowNoOwnerOrOccupier()
{
    REQUIRE_THAT( description, !Catch::Matchers::Contains("Fortress is owned by Varatrarg the Barbarian who is loyal to the Barbarians."));
    REQUIRE_THAT( description, !Catch::Matchers::Contains("Fortress is owned by Tarithel the Fey who is loyal to the Moonprince."));
    REQUIRE_THAT( description, !Catch::Matchers::Contains("Fortress is owned by Luxor the Moonprince."));
}


SCENARIO("Describing a Stronghold", "[new]")
{
    TMEStep::NewStory();
    
    auto text = static_cast<ddr_text*>(tme::mx->text);
    
    auto stronghold = (ProxyStronghold*) GetStronghold("SH_FORTRESS_VARATRARG");
    
    auto owner = GetCharacter("CH_VARATRARG");
    auto occupier = GetCharacter(DDRStep::ch_tarithel);
    auto luxor = GetCharacter(TMEStep::ch_luxor);

    
    auto DescribeStronghold = [&]() {
        description = std::string(text->DescribeStronghold(stronghold));
    };
    
        
    GIVEN("a stronghold has no owner")
    {
        stronghold->SetOwner(nullptr);
    
        AND_GIVEN("is not occupied")
        {
            stronghold->SetOccupier(nullptr);

            WHEN("stronghold is described")
            {
                DescribeStronghold();
                
                THEN("there is no mention of the occupier or owner")
                {
                    ShouldShowNoOwnerOrOccupier();
                }
            }
        }
    }

    GIVEN("a stronghold has an owner")
    {
        stronghold->SetOwner(owner);
            
        AND_GIVEN("is not occupied")
        {
            stronghold->SetOccupier(nullptr);

            WHEN("stronghold is described")
            {
                DescribeStronghold();
                
                THEN("should just mention the owner and their loyalty")
                {
                    ShouldShowOwnedByVaratrarg();
                }
            }
        }

        AND_GIVEN("is occupied by the owner")
        {
            stronghold->SetOwner(owner);
            stronghold->SetOccupier(owner);

            WHEN("stronghold is described")
            {
                DescribeStronghold();
                
                THEN("should just mention the owner and their loyalty")
                {
                    ShouldShowOwnedByVaratrarg();
                }
            }
        }
        
        AND_GIVEN("is occupied by someone other than the owner")
        {
            stronghold->SetOwner(occupier);
            stronghold->SetOccupier(occupier);

            WHEN("stronghold is described")
            {
                DescribeStronghold();
                
                THEN("and should show the occupier and their loyalty")
                {
                    ShouldShowOccupiedByTarithel();
                }
            }
        }
        
        AND_GIVEN("is occupied by luxor")
        {
            stronghold->SetOwner(luxor);
            stronghold->SetOccupier(luxor);

            WHEN("stronghold is described")
            {
                DescribeStronghold();
                
                THEN("and should show occupier without their loyalty")
                {
                    ShouldShowOccupiedByLuxor();
                }
            }
        }
    }
}

// GIVEN the stronghold has 75 warriors
// AND lord can recruit

SCENARIO("Game crashes when lord recruit army (bug #246)", "[new]")
{
    TMEStep::NewStory();
    
    auto stronghold = (ProxyStronghold*) GetStronghold("SH_FORTRESS_VARATRARG");
    auto lord = GetDDRCharacter("CH_VARATRARG");
    
    GIVEN("Lords is at stronghold")
    {
        lord->Location(stronghold->Location());
        
        AND_GIVEN("a stronghold has 75 warriors")
        {
            stronghold->MinTroops(50);
            stronghold->TotalTroops(75);
            stronghold->SetTroopType(UT_WARRIORS);
        
            AND_GIVEN("the lord is able to recruit more men")
            {
                lord->Flags().Set(cf_allowedwarriors);
                lord->Flags().Reset(cf_allowedriders);
                lord->warriors.Total(500);
                lord->riders.Total(0);

                WHEN("lord recruits the soldiers")
                {
                    lord->Cmd_RecruitMen(stronghold, 100);
                    
                    THEN("the lord should have recruited soldiers")
                    {
                        REQUIRE(lord->getArmySize() == 525);
                        REQUIRE(stronghold->TotalTroops() == 50);
                    }
                }
            }
        }
        
        AND_GIVEN("a stronghold has 450 warriors")
        {
            stronghold->MaxTroops(500);
            stronghold->TotalTroops(450);
            stronghold->SetTroopType(UT_WARRIORS);
        
            AND_GIVEN("the lord is able to post more men")
            {
                lord->Flags().Set(cf_allowedwarriors);
                lord->Flags().Reset(cf_allowedriders);
                lord->warriors.Total(500);
                lord->riders.Total(0);

                WHEN("lord posts the soldiers on guard")
                {
                    lord->Cmd_PostMen(stronghold, 100);
                    
                    THEN("the lord should not have placed soldiers on guard")
                    {
                        REQUIRE(lord->getArmySize() == 500);
                        REQUIRE(stronghold->TotalTroops() == 450);
                    }
                }
            }
        }
    }
}
