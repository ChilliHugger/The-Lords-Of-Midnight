//
//  savegamemapper_tests.cpp
//  revenge
//
//  Created by Chris Wild on 01/09/2023.
//
#include "catch2/catch.hpp"
#include "../../src/tme/baseinc/tme_internal.h"
#include "../../src/tme/utils/savegamemapping.h"
#include "../steps/tme_steps.h"

using oldflags = tme::utils::OLD_DDR_CHARACTERFLAGS ;
using newflags = tme::flags::CHARACTERFLAGS ;

TEST_CASE("DDR Flags are remapped")
{
    flags32 old_flags(
        oldflags::cf_resting |
        oldflags::cf_tunnel |
        oldflags::cf_approaching
    );
    
    flags32 expected_flags(
        newflags::cf_resting |
        newflags::cf_tunnel |
        newflags::cf_approaching
    );

    flags32 new_flags = tme::utils::UpdateDDRCharacterFlags(old_flags);
    
    REQUIRE (new_flags == expected_flags);
}

TEST_CASE("DDR All Flags are remapped")
{
    flags32 old_flags(
        oldflags::cf_all
    );
    
    flags32 expected_flags(
        newflags::cf_resting |
        newflags::cf_inbattle |
        newflags::cf_wonbattle |
        newflags::cf_tunnel |
        newflags::cf_usedobject |
        newflags::cf_followers |
        newflags::cf_preparesbattle |
        newflags::cf_approaching
     );
    
    flags32 new_flags = tme::utils::UpdateDDRCharacterFlags(old_flags);
    
    REQUIRE (new_flags == expected_flags);
}


TEST_CASE("Other Flags are not moved")
{
    flags32 old_flags(
        MXBIT(0) |
        MXBIT(13) |
        oldflags::cf_tunnel |
        MXBIT(24) |
        MXBIT(31)
    );
    
    flags32 expected_flags(
        MXBIT(0) |
        MXBIT(13) |
        newflags::cf_tunnel |
        MXBIT(24) |
        MXBIT(31)
    );
    
    flags32 new_flags = tme::utils::UpdateDDRCharacterFlags(old_flags);
    
    REQUIRE (new_flags == expected_flags);
}

TEST_CASE("OB_SPEAR_THORTHAK is a spear not a sword")
{
    TMEStep::NewStory();
    
    // 118 | OB_SWORD_THORTHAK > should be a SPEAR ?
    // 124 | OB_HAMMER_TORORTHANE > should be a BOW ?
    auto item = static_cast<ddr_object*>(tme::mx->EntityByName("OB_SPEAR_THORTHAK", IDT_OBJECT));

    REQUIRE( item != nullptr );
    REQUIRE( item->type == OT_SPEAR );

}

TEST_CASE("OB_BOW_TORORTHANE is a bow not a hammer")
{
    TMEStep::NewStory();
    
    // 118 | OB_SWORD_THORTHAK > should be a SPEAR ?
    // 124 | OB_HAMMER_TORORTHANE > should be a BOW ?
    auto item = static_cast<ddr_object*>(tme::mx->EntityByName("OB_BOW_TORORTHANE", IDT_OBJECT));
    
    REQUIRE( item != nullptr );
    REQUIRE( item->type == OT_BOW );
}

TEST_CASE("CH_IMGORARG should be loyal to the dwarves")
{
    TMEStep::NewStory();
    
    auto item = static_cast<mxcharacter*>(tme::mx->EntityByName("CH_IMGORARG", IDT_CHARACTER));
    
    REQUIRE( item != nullptr );
    REQUIRE( item->loyalty == RA_DWARF );
}

TEST_CASE("Special objects can aid recruitment")
{
    auto object = GENERATE(
                    "OB_CROWN_VARENAND",
                    "OB_CROWN_CARUDRIUM",
                    "OB_SPELL_THIGRORN",
                    "OB_RUNES_FINORN",
                    "OB_CROWN_IMIRIEL");
                
    TMEStep::NewStory();
    
    auto item = static_cast<ddr_object*>(tme::mx->EntityByName(object, IDT_OBJECT));
    REQUIRE( item != nullptr );
    REQUIRE( item->CanHelpRecruitment() );
   
    
}
