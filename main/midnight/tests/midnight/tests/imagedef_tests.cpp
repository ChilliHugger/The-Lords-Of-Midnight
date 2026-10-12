//
//  imagedef_tests.cpp
//  midnight
//
#include "catch2/catch.hpp"
#include "../../../Source/utils/imagedef.h"
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

using namespace imagedef;

static const char* xml = R"(
<images>
  <image id="shield" size="256,128">
    <gfx id="art" name="shield_art"/>
  </image>
  <image id="a" size="256, 128">
    <gfx id="body" name="shield" tint="blue" scale="2"/>
    <gfx id="star" name="star" pos="12.5, 7" anchor="c" scale="2" tint="yellow" alpha="0.5" flipx="true" rotation="90"/>
    <txt id="title" text="@name" pos="64,120" anchor="bc" font="button" size="10" outline="2" tracking="-75" colour="#102030" align="left" width="50"/>
  </image>
</images>)";

TEST_CASE("Image library parses gfx elements")
{
    Library lib;
    REQUIRE(lib.loadFromString(xml));
    REQUIRE(lib.count() == 2);

    auto a = lib.find("a");
    REQUIRE(a != nullptr);
    CHECK(a->id == "a");
    CHECK(a->width == 256.0f);
    CHECK(a->height == 128.0f);
    REQUIRE(a->elements.size() == 2 + 1);

    auto body = a->find("body");
    REQUIRE(body != nullptr);
    CHECK(body->name == "shield");      // an atlas image or another <image>: resolved when built
    CHECK(body->anchor == Anchor::TopLeft);
    CHECK(body->colour == Colour{0x00,0x00,0xcd});

    auto star = a->find("star");
    REQUIRE(star != nullptr);
    CHECK(star->type == ElementType::Gfx);
    CHECK(star->x == 12.5f);
    CHECK(star->y == 7.0f);
    CHECK(star->anchor == Anchor::Centre);
    CHECK(star->scale == 2.0f);
    CHECK(star->alpha == 0.5f);
    CHECK(star->rotation == 90.0f);
    CHECK(star->flipX);
    CHECK(star->colour == Colour{0xcd,0xcd,0x00});
}

TEST_CASE("Text elements parse")
{
    Library lib;
    REQUIRE(lib.loadFromString(xml));
    auto title = lib.find("a")->find("title");
    REQUIRE(title != nullptr);
    CHECK(title->type == ElementType::Txt);
    CHECK(title->text == "@name");
    CHECK(title->font == "button");
    CHECK(title->align == "left");
    CHECK(title->colour == Colour{0x10,0x20,0x30});
    CHECK(title->size == 10.0f);
    CHECK(title->width == 50.0f);
    CHECK(title->outline == 2.0f);
    CHECK(title->tracking == -75.0f);
    CHECK(title->anchor == Anchor::BottomCentre);
    CHECK(title->enabled);
}

TEST_CASE("Elements keep their order and disabled elements are still described")
{
    Library lib;
    REQUIRE(lib.loadFromString(
        "<images><image id='a'><gfx id='x' name='n'/><gfx id='y' name='m' enabled='false'/><gfx id='z' name='o'/></image></images>"));
    auto a = lib.find("a");
    REQUIRE(a->elements.size() == 3);
    CHECK(a->elements[0].id == "x");
    CHECK(a->elements[1].id == "y");
    CHECK_FALSE(a->elements[1].enabled);
    CHECK(a->elements[2].id == "z");
}

TEST_CASE("Malformed image definitions are rejected")
{
    Library lib;
    CHECK_FALSE(lib.loadFromString("<images><image id='a'><gfx id='x'/></image></images>"));
    CHECK_FALSE(lib.loadFromString("<images><image id='a'><gfx id='x' name='n' tint='nope'/></image></images>"));
    CHECK_FALSE(lib.loadFromString("<images><image id='a'><gfx id='x' name='n' anchor='zz'/></image></images>"));
    CHECK_FALSE(lib.loadFromString("<images><image id='a'><gfx id='x' name='n' pos='1'/></image></images>"));
    CHECK_FALSE(lib.loadFromString("<images><image id='a'><txt/></image></images>"));
    CHECK_FALSE(lib.loadFromString("<images><image id='a'><gfx name='n'/></image></images>"));
    CHECK_FALSE(lib.loadFromString("<images><image id='a'><gfx id='x' name='n'/><txt id='x' text='t'/></image></images>"));
    CHECK_FALSE(lib.loadFromString("<images><image id='a' size='x'/></images>"));
    CHECK_FALSE(lib.loadFromString("<images><image><gfx id='x' name='n'/></image></images>"));
    CHECK_FALSE(lib.loadFromString("<images><oops/></images>"));
    CHECK_FALSE(lib.loadFromString("<images"));
    CHECK_FALSE(lib.error().empty());
}

TEST_CASE("An icon's main art is its first enabled gfx")
{
    Library lib;
    REQUIRE(lib.loadFromString(
        "<images><image id='icon_think'>"
        "<txt id='t' text='x'/>"
        "<gfx id='off' name='a' enabled='false'/>"
        "<gfx id='art' name='i_think_art'/>"
        "<gfx id='extra' name='b'/>"
        "</image><image id='empty'><txt id='t' text='x'/></image></images>"));
    CHECK(lib.find("icon_think")->firstGfx()->name == "i_think_art");
    CHECK(lib.find("empty")->firstGfx() == nullptr);
}

// the test app is sandboxed, so read the definitions from the copy in its bundle
static std::string resource(const char* file)
{
#ifndef __APPLE__
    return std::string("Resources/") + file;
#else
    char path[4096];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) != 0)
        return file;
    std::string exe = path;     // <app>/Contents/MacOS/<name>
    auto contents = exe.substr(0, exe.rfind("/MacOS/"));
    return contents + "/Resources/" + file;
#endif
}

TEST_CASE("The shipped shield definitions load and use the shared shield")
{
    Library lib;
    bool loaded = lib.loadFromFile(resource("lom/shields.xml"));
    INFO(resource("lom/shields.xml") << ": " << lib.error());
    REQUIRE(loaded);

    auto luxor = lib.find("ch_luxor");
    REQUIRE(luxor != nullptr);
    CHECK(luxor->width == 256.0f);
    CHECK(luxor->find("shield")->name == "lom_shield");     // the shared shield image
    CHECK(luxor->find("shield")->colour == Colour{0xcd,0x00,0xcd});
    CHECK(luxor->find("bevel")->name == "shield_bevel");
    CHECK(luxor->elements.size() == 4);     // shield, bevel and two parts
    REQUIRE(lib.find("lom_shield") != nullptr);
    CHECK(lib.find("lom_shield")->find("art")->name == "shield");
}

TEST_CASE("The shipped icon definitions each name their art and have a label")
{
    Library lib;
    bool loaded = lib.loadFromFile(resource("all/en/icons.xml"));
    INFO(resource("all/en/icons.xml") << ": " << lib.error());
    REQUIRE(loaded);

    auto think = lib.find("icon_think");
    REQUIRE(think != nullptr);
    REQUIRE(think->firstGfx() != nullptr);
    CHECK(think->firstGfx()->name == "i_think");
    auto text = think->find("buttontext");
    REQUIRE(text != nullptr);
    CHECK(text->type == ElementType::Txt);
    CHECK(text->text == "think");
}

TEST_CASE("Content is aligned within a cell")
{
    // a 72x59 icon drawn at (20,3) in a 128x64 cell
    Box art{20, 3, 92, 62};

    auto bc = alignShift(Anchor::BottomCentre, 128, 64, art);
    CHECK(bc.x == 8.0f);                // centred: left edge moves from 20 to (128-72)/2 = 28
    CHECK(bc.y == 2.0f);                // bottom edge at 64

    auto tl = alignShift(Anchor::TopLeft, 128, 64, art);
    CHECK(tl.x == -20.0f);
    CHECK(tl.y == -3.0f);

    auto br = alignShift(Anchor::BottomRight, 128, 64, art);
    CHECK(br.x == 36.0f);
    CHECK(br.y == 2.0f);

    auto c = alignShift(Anchor::Centre, 128, 64, art);
    CHECK(c.x == 8.0f);
    CHECK(c.y == Approx(-0.5f));

    // already in place: no shift
    auto none = alignShift(Anchor::BottomCentre, 128, 64, Box{28, 5, 100, 64});
    CHECK(none.x == 0.0f);
    CHECK(none.y == 0.0f);
}

TEST_CASE("Boxes unite")
{
    auto u = Box{10, 20, 30, 40}.united(Box{5, 25, 20, 60});
    CHECK(u.x0 == 5.0f);
    CHECK(u.y0 == 20.0f);
    CHECK(u.x1 == 30.0f);
    CHECK(u.y1 == 60.0f);
    CHECK(u.width() == 25.0f);
    CHECK(u.height() == 40.0f);
}

TEST_CASE("An image can align its content")
{
    Library lib;
    REQUIRE(lib.loadFromString("<images><image id='a' align='BC' size='128,64'><gfx id='x' name='n'/></image><image id='b'><gfx id='x' name='n'/></image></images>"));
    CHECK(lib.find("a")->aligned);
    CHECK(lib.find("a")->align == Anchor::BottomCentre);
    CHECK_FALSE(lib.find("b")->aligned);
    CHECK_FALSE(lib.loadFromString("<images><image id='a' align='zz'/></images>"));
}

TEST_CASE("A multi-word icon label can be several text items")
{
    Library lib;
    bool loaded = lib.loadFromFile(resource("all/en/icons.xml"));
    INFO(lib.error());
    REQUIRE(loaded);
    auto tunnel = lib.find("icon_entertunnel");
    REQUIRE(tunnel != nullptr);
    REQUIRE(tunnel->find("text1") != nullptr);
    REQUIRE(tunnel->find("text2") != nullptr);
    CHECK(tunnel->find("text1")->text == "enter");
    CHECK(tunnel->find("text2")->text == "tunnel");
    CHECK(tunnel->find("text1")->y < tunnel->find("text2")->y);     // enter above tunnel
}

TEST_CASE("The shipped filter buttons have the icon and both eye badges")
{
    Library lib;
    bool loaded = lib.loadFromFile(resource("all/filters.xml"));
    INFO(lib.error());
    REQUIRE(loaded);

    for (auto id : { "filter_dawn", "filter_night", "filter_battle", "filter_dead",
                     "filter_centre", "filter_critters", "filter_lords", "filter_tunnels" }) {
        INFO(id);
        auto filter = lib.find(id);
        REQUIRE(filter != nullptr);
        REQUIRE(filter->firstGfx() != nullptr);
        REQUIRE(filter->find("visible") != nullptr);
        REQUIRE(filter->find("notvisible") != nullptr);
        CHECK(filter->find("visible")->name == "i_visible");
        CHECK(filter->find("notvisible")->name == "i_not_visible");
    }
    CHECK(lib.find("filter_dawn")->firstGfx()->name == "lord_dawn");
    CHECK(lib.find("filter_dead")->firstGfx()->name == "f_dead!");
}

TEST_CASE("The shipped lord images have circles, a face and status badges that start hidden, but no name")
{
    Library lib;
    bool loaded = lib.loadFromFile(resource("all/lord.xml"));
    INFO(lib.error());
    REQUIRE(loaded);

    auto select = lib.find("lord_select");
    REQUIRE(select != nullptr);
    CHECK(select->width == 128.0f);     // the size of the circles
    REQUIRE(select->find("face") != nullptr);
    CHECK(select->find("title") == nullptr);    // the name is a label on the lord, drawn sharp at any scale
    for (auto part : { "selected", "location", "drop", "battle", "night", "dawn" }) {
        INFO(part);
        REQUIRE(select->find(part) != nullptr);
        CHECK_FALSE(select->find(part)->enabled);
    }
    CHECK(select->find("selected")->name == "lord_select_circle");
    CHECK(select->find("selected")->colour == Colour{0x00,0x00,0xcd});
    CHECK(select->find("location")->name == "map_current_location");
    CHECK(select->find("drop")->name == "lord_drop_circle");
    CHECK(select->find("drop")->colour == Colour{0x00,0x00,0x00});
    CHECK(select->find("drop")->alpha == 0.1f);

    // the circles are behind the face, the badges over it
    CHECK(select->elements.front().id == "selected");
    CHECK(select->elements[3].id == "face");

    auto grouped = lib.find("lord_grouped");
    REQUIRE(grouped != nullptr);
    CHECK(grouped->width == 64.0f);
    CHECK(grouped->find("face") != nullptr);
    CHECK(grouped->find("title") == nullptr);
}

TEST_CASE("Every lord has a shield image the size the lord face is scaled from")
{
    Library lib;
    bool loaded = lib.loadFromFile(resource("lom/shields.xml"));
    INFO(lib.error());
    REQUIRE(loaded);

    for (auto& name : { "ch_luxor", "ch_morkin", "ch_corleth", "ch_rorthron", "ch_xajorkith", "ch_athoril" }) {
        INFO(name);
        auto shield = lib.find(name);
        REQUIRE(shield != nullptr);
        CHECK(shield->width == 256.0f);     // 64 / 256 is the scale of a face
        CHECK(lib.find(std::string("mini_") + name) == nullptr);
    }
}
