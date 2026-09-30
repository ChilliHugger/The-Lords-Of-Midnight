//
//  titles_tests.cpp
//  citadel
//
#include "../../steps/tme_steps.h"

namespace {

    std::string Cook ( const char* text, mxcharacter* character )
    {
        std::string input = text;
        return tme::mx->text->CookText(input, character);
    }
}

SCENARIO("The kings, queens and princes of the Bloodmarch carry the design's titles")
{
    TMEStep::NewStory();

    auto zenethor = GetCharacter("CH_ZENETHOR");
    auto rorthron = GetCharacter("CH_RORTHRON");

    THEN("each ruler holds the title of his realm")
    {
        REQUIRE( zenethor->Title() == "King of the Athelings" );
        REQUIRE( GetCharacter("CH_SHARILA")->Title() == "Princess Regent of the Witherlands" );
        REQUIRE( GetCharacter("CH_MOGRIK")->Title() == "Prince of the Witherlands" );
        REQUIRE( GetCharacter("CH_BOROTH")->Title() == "High King of the Marish" );
    }

    THEN("fifteen lords have a title, and the rest have none")
    {
        int titled = 0;
        for ( auto character : tme::mx->objCharacters ) {
            if ( !character->Title().empty() )
                titled++;
        }
        REQUIRE( titled == 15 );
        REQUIRE( rorthron->Title().empty() );
    }

    THEN("a titled lord's thoughts begin with his title, and an untitled lord's do not")
    {
        REQUIRE( Cook("{char:text:title}", zenethor) == "Zenethor is King of the Athelings. " );
        REQUIRE( Cook("{char:text:title}", rorthron).empty() );
        REQUIRE_THAT( tme::mx->text->CookedSystemString(SS_MESSAGE1, zenethor),
                      Catch::Matchers::StartsWith("Zenethor is King of the Athelings. It is ") );
        REQUIRE_THAT( tme::mx->text->CookedSystemString(SS_MESSAGE1, rorthron),
                      Catch::Matchers::StartsWith("It is ") );
    }

    THEN("the queen and princess of the Eldmark are named as the design writes them")
    {
        REQUIRE( GetCharacter("CH_CARITHILA")->Longname() == "Queen Carithila" );
        REQUIRE( GetCharacter("CH_AREMELA")->Longname() == "Princess Aremela" );
    }
}
