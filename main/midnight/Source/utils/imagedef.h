//
//  imagedef.h
//  midnight
//
//  Data description of composite images (shields, icons...) built from
//  graphics and text elements. Pure data + XML parsing: no engine dependency.
//
//  <images>
//    <image id="lom_shield" size="256,256">
//      <gfx id="art" name="shield"/>
//    </image>
//    <image id="ch_luxor" size="256,256">
//      <gfx id="shield" name="lom_shield" tint="magenta"/>
//      <gfx id="bevel"  name="shield_bevel" alpha="0.3"/>
//      <gfx id="star"   name="shield_star" pos="95,107" anchor="c" tint="yellow"/>
//      <txt id="title"  pos="64,120" anchor="bc" font="midnight" size="10" colour="white"/>
//    </image>
//  </images>
//
//  gfx name  an <image> id in the same file, else an atlas image name. Image ids must differ from atlas
//            names (e.g. image "icon_dawn", atlas "i_dawn"). Tint and alpha on a gfx that is an <image>
//            apply to everything in it. Images cannot contain themselves.
//  align     optional, on an image: moves the visible content (the union of its elements, ignoring the
//            transparent padding of atlas images) to this anchor within the image's own size, e.g.
//            align="bc" bottom-centres a 72x59 icon in its 128x64 cell. Element positions are then
//            relative to that aligned content.
//  txt       a bottom-anchored text is bottom-aligned by its ink, not its box, so pos y=64 puts the lowest
//            ink at 64. 'descent' overrides the estimated overhang (outline + 36% of the size, measured).
//  Element ids are unique within an image. Positions are in base-resolution units
//  (scaled by RES() when built), origin top-left.
//

#ifndef imagedef_h
#define imagedef_h

#include <string>
#include <vector>
#include <map>

namespace imagedef {

struct Colour {
    unsigned char r{255}, g{255}, b{255};
    bool operator==(const Colour& o) const { return r == o.r && g == o.g && b == o.b; }
};

enum class Anchor { TopLeft, TopCentre, TopRight, LeftCentre, Centre, RightCentre, BottomLeft, BottomCentre, BottomRight };

enum class ElementType { Gfx, Txt };

struct Element {
    ElementType type{ElementType::Gfx};
    std::string id;
    std::string name;       // gfx: sprite frame name
    std::string text;       // txt: the text, in the file for its locale; dynamic text is set in code (imagebuilder::setText)
    std::string font;       // txt: font alias
    std::string align;      // txt: left|centre|right
    float x{0}, y{0};
    Anchor anchor{Anchor::TopLeft};
    float scale{1.0f};
    float rotation{0};
    float size{0};          // txt
    float width{0};         // txt: wrap width, 0 = none
    float outline{0};       // txt: black outline size, 0 = none
    float tracking{0};      // txt: extra spacing in 1/1000 of an em
    float descent{-1};      // txt: how far the ink hangs below the label's box (outline, descenders); -1 = estimate
    float alpha{1.0f};
    bool flipX{false}, flipY{false};
    bool enabled{true};
    Colour colour;          // gfx: tint, txt: text colour
};

// a rectangle, origin top-left, y down
struct Box {
    float x0{0}, y0{0}, x1{0}, y1{0};
    float width() const { return x1 - x0; }
    float height() const { return y1 - y0; }
    Box united(const Box& o) const;
};

// how far to move content occupying 'box' so it sits at 'anchor' inside a width x height cell
struct Shift { float x{0}, y{0}; };
Shift alignShift(Anchor anchor, float width, float height, const Box& box);

struct Image {
    std::string id;
    bool aligned{false};    // align the visible content within the image's size
    Anchor align{Anchor::TopLeft};
    float width{0}, height{0};  // optional size of the image (for hosts without a texture)
    std::vector<Element> elements;

    const Element* find(const std::string& elementId) const;
    const Element* firstGfx() const;     // the main art of an icon
};

class Library {
public:
    // parse XML text; returns false (and sets error) if malformed. Merges into the library.
    bool loadFromString(const std::string& xml);
    bool loadFromFile(const std::string& filename);

    const Image* find(const std::string& id) const;
    size_t count() const { return images.size(); }
    const std::string& error() const { return lastError; }

    static bool parseColour(const std::string& text, Colour& out);
    static bool parseAnchor(const std::string& text, Anchor& out);

private:
    std::map<std::string, Image> images;
    std::string lastError;
};

}

#endif /* imagedef_h */
