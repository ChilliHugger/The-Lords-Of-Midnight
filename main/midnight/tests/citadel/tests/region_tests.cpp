//
//  region_tests.cpp
//  citadel
//
#include "../../steps/tme_steps.h"

namespace {

    citadel_area* Region ( LPCSTR symbol )
    {
        auto area = GetEntity<citadel_area>(symbol);
        REQUIRE( area != nullptr );
        return area;
    }
}

SCENARIO("The regions know their neighbours")
{
    TMEStep::NewStory();

    THEN("as many as the design counts")
    {
        REQUIRE( Region("AR_IMILVIR")->neighbours.size() == 25 );
        REQUIRE( Region("AR_MELIBOR")->neighbours.size() == 10 );
        REQUIRE( Region("AR_IMMIEL")->neighbours.size() == 1 );
        REQUIRE( Region("AR_OBLIVION_MISTS")->neighbours.empty() );
    }

    THEN("Corelay borders the river and the Plains of Last, and no more")
    {
        auto corelay = Region("AR_CORELAY");
        REQUIRE( corelay->neighbours.size() == 2 );
        REQUIRE( corelay->Borders(Region("AR_IMILVIR")) );
        REQUIRE( corelay->Borders(Region("AR_LAST")) );
        REQUIRE_FALSE( corelay->Borders(Region("AR_MARANOR")) );
    }

    THEN("every border runs both ways, and no region borders itself")
    {
        u32 borders = 0;
        for ( auto info : tme::mx->objAreaInfos ) {
            auto area = static_cast<citadel_area*>(info);
            REQUIRE_FALSE( area->Borders(area) );
            for ( auto id : area->neighbours ) {
                CAPTURE( area->Symbol(), id );
                REQUIRE( static_cast<citadel_area*>(tme::mx->AreaById(id))->Borders(area) );
                borders++;
            }
        }
        REQUIRE( borders == 2 * 403 );
    }
}

SCENARIO("The neighbours survive the database cache")
{
    // the second story in a process is read back from the binary cache, not the .tsv
    TMEStep::NewStory();
    TMEStep::NewStory();

    REQUIRE( Region("AR_IMILVIR")->neighbours.size() == 25 );
    REQUIRE( Region("AR_CORELAY")->Borders(Region("AR_LAST")) );
}
