//
//  placement_tests.cpp
//  citadel
//
#include "../../steps/tme_steps.h"

namespace {

    int LordsAt ( mxgridref loc )
    {
        int count = 0;
        for ( auto character : tme::mx->objCharacters ) {
            if ( character->Location() == loc )
                count++;
        }
        return count;
    }
}

SCENARIO("A new story puts the Citadel's lords where the data puts them")
{
    TMEStep::NewStory();

    THEN("the map places each lord on his own square, not the row below it")
    {
        // the TMX character objects are read top-edge, as Lords of Midnight's are (#21)
        REQUIRE( GetCharacter("CH_MORKIN")->Location() == mxgridref(8,12) );
        REQUIRE( GetCharacter("CH_ZENETHOR")->Location() == GetStronghold("SH_CITADEL_MARANOR")->Location() );
    }

    THEN("the hostages and their rescuers are all inside the Dark Citadel")
    {
        REQUIRE( LordsAt(GetStronghold("SH_CITADEL_MARANOR")->Location()) == 18 );
    }

    THEN("Castle Burning holds only its own lord, and the lords once parked there are at home")
    {
        auto burning = GetStronghold("SH_CASTLE_BURNING")->Location();
        REQUIRE( LordsAt(burning) == 1 );
        REQUIRE( GetCharacter("CH_URGORETH")->Location() == burning );
        REQUIRE( GetCharacter("CH_LANKLIN")->Location() == GetStronghold("SH_CASTLE_GOGRUN")->Location() );
    }
}
