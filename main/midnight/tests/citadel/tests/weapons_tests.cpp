//
//  weapons_tests.cpp
//  citadel
//
#include "../../steps/tme_steps.h"

SCENARIO("The seven weapons lie where the 1995 game put them")
{
    TMEStep::NewStory();

    // citadel_objects.txt, the export of the 1995 binaries
    const struct { LPCSTR symbol; int x; int y; } weapons[] = {
        { "OB_STORMBLADE",   148, 125 },
        { "OB_BLOODBRINGER",   7,  14 },
        { "OB_WIDOWMAKER",    72, 200 },
        { "OB_ARANATH",       32,  80 },
        { "OB_PERSUADER",     25, 160 },
        { "OB_SKULLCRUSHER", 233, 205 },
        { "OB_SWIFTWING",    241,  10 },   // on Arungor, where it was forged - not 241,210, at sea
    };

    THEN("each is on its square")
    {
        for ( auto& weapon : weapons ) {
            auto object = GetObject(weapon.symbol);
            REQUIRE( object != nullptr );
            REQUIRE( object->Location() == mxgridref(weapon.x, weapon.y) );
        }
    }

    THEN("each is described as the sword or the axe it is")
    {
        REQUIRE( tme::mx->text->DescribeObject(GetObject("OB_SWIFTWING")) == "the sword Swiftwing" );
        REQUIRE( tme::mx->text->DescribeObject(GetObject("OB_WIDOWMAKER")) == "the axe Widowmaker" );
        REQUIRE( tme::mx->text->DescribeObject(GetObject("OB_ARANATH")) == "the sword Aranath" );
    }
}
