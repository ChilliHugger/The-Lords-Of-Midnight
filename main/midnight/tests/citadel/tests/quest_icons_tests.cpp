//
//  quest_icons_tests.cpp
//  citadel
//
#include "catch2/catch.hpp"
#include "../../../Source/utils/imagedef.h"
#include "../../../Source/axmol_sdk.h"
#include "../../../Source/platform/Extensions.h"

using namespace imagedef;

// the test app is sandboxed, so read the definitions from the copy in its bundle
static std::string resource(const char* file)
{
    return chilli::extensions::getBundleResourcePath() + "/" + file;
}

TEST_CASE("The citadel quest icons have a cell wider than their art and their text")
{
    Library lib;
    bool loaded = lib.loadFromFile(resource("citadel/quest.xml"));
    INFO(resource("citadel/quest.xml") << ": " << lib.error());
    REQUIRE(loaded);

    auto recruit = lib.find("icon_quest_recruit");
    REQUIRE(recruit != nullptr);
    CHECK(recruit->width == 144.0f);
    CHECK(recruit->firstGfx()->name == "quest_recruit");
    REQUIRE(recruit->find("buttontext") != nullptr);
    CHECK(recruit->find("buttontext")->text == "recruit");

    REQUIRE(lib.find("icon_quest") != nullptr);
    CHECK(lib.find("icon_quest")->firstGfx()->name == "i_quest");
}

TEST_CASE("Every citadel quest choice has an icon with art and text")
{
    Library lib;
    REQUIRE(lib.loadFromFile(resource("citadel/quest.xml")));

    for (auto id : { "icon_quest_recruit", "icon_quest_join", "icon_quest_kill", "icon_quest_rescue",
                     "icon_quest_follow", "icon_quest_goto", "icon_quest_guard", "icon_quest_seize",
                     "icon_quest_find", "icon_quest_take", "icon_quest_destroy", "icon_quest_rest" }) {
        INFO(id);
        auto icon = lib.find(id);
        REQUIRE(icon != nullptr);
        REQUIRE(icon->firstGfx() != nullptr);
        auto text = icon->find("buttontext");
        REQUIRE(text != nullptr);
        CHECK_FALSE(text->text.empty());
    }
}
