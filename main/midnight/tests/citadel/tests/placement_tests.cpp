//
//  placement_tests.cpp
//  citadel
//
#include "../../steps/tme_steps.h"

namespace {

    int LordsAt ( const mxgridref& loc )
    {
        int count = 0;
        for ( auto character : tme::mx->objCharacters ) {
            if ( character->Location() == loc )
                count++;
        }
        return count;
    }
}

SCENARIO("Every lord starts on the square the data gives him")
{
    TMEStep::NewStory();

    THEN("the map does not move a lord a row south of his data")
    {
        REQUIRE( GetCharacter("CH_MORKIN")->Location() == mxgridref(8,12) );
    }

    THEN("the rescuers, Luxor, the twelve hostages and three of the Dark Fey are in the Dark Citadel")
    {
        auto maranor = GetStronghold("SH_CITADEL_MARANOR")->Location();
        REQUIRE( LordsAt(maranor) == 18 );
        REQUIRE( GetCharacter("CH_CORLETH")->Location() == maranor );
        REQUIRE( GetCharacter("CH_BOROTH")->Location() == maranor );
    }

    THEN("Castle Burning holds only its own ruler")
    {
        auto burning = GetStronghold("SH_CASTLE_BURNING")->Location();
        REQUIRE( LordsAt(burning) == 1 );
        REQUIRE( GetCharacter("CH_URGORETH")->Location() == burning );
    }

    THEN("the lords who stood at Burning are at the keeps of their own lands")
    {
        REQUIRE( GetCharacter("CH_CADRON")->Location() == GetStronghold("SH_CASTLE_EOMIR")->Location() );
        REQUIRE( GetCharacter("CH_LANKLIN")->Location() == GetStronghold("SH_CASTLE_GOGRUN")->Location() );
    }
}
