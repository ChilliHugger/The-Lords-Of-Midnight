//
//  imagedef.cpp
//  midnight
//

#include "imagedef.h"
#include "../tinyxml2/tinyxml2.h"
#include <cstdlib>
#include <cstdio>
#include <algorithm>

using namespace tinyxml2;

namespace imagedef {

// named colours match the game's dark system palette (uihelper.h)
static const struct { const char* name; Colour c; } Palette[] = {
    { "black",   {0x00,0x00,0x00} },
    { "blue",    {0x00,0x00,0xcd} },
    { "red",     {0xcd,0x00,0x00} },
    { "magenta", {0xcd,0x00,0xcd} },
    { "green",   {0x00,0xcd,0x00} },
    { "cyan",    {0x00,0xcd,0xcd} },
    { "yellow",  {0xcd,0xcd,0x00} },
    { "white",   {0xcd,0xcd,0xcd} },
    { "grey",    {0x80,0x80,0x80} },
};

static std::string lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return (char)std::tolower(c); });
    return s;
}

bool Library::parseColour(const std::string& text, Colour& out) {
    if (text.empty()) return false;
    if (text[0] == '#') {
        if (text.size() != 7) return false;
        char* end = nullptr;
        auto v = strtoul(text.c_str() + 1, &end, 16);
        if (*end != 0) return false;
        out = { (unsigned char)(v >> 16), (unsigned char)(v >> 8), (unsigned char)v };
        return true;
    }
    auto key = lower(text);
    for (auto& p : Palette) {
        if (key == p.name) { out = p.c; return true; }
    }
    return false;
}

bool Library::parseAnchor(const std::string& text, Anchor& out) {
    static const struct { const char* n; Anchor a; } map[] = {
        { "tl", Anchor::TopLeft },    { "tc", Anchor::TopCentre },    { "tr", Anchor::TopRight },
        { "cl", Anchor::LeftCentre }, { "c",  Anchor::Centre },       { "cr", Anchor::RightCentre },
        { "bl", Anchor::BottomLeft }, { "bc", Anchor::BottomCentre }, { "br", Anchor::BottomRight },
    };
    auto key = lower(text);
    for (auto& m : map) {
        if (key == m.n) { out = m.a; return true; }
    }
    return false;
}

const Element* Image::find(const std::string& elementId) const {
    for (auto& e : elements) {
        if (e.id == elementId) return &e;
    }
    return nullptr;
}

Box Box::united(const Box& o) const {
    return { std::min(x0, o.x0), std::min(y0, o.y0), std::max(x1, o.x1), std::max(y1, o.y1) };
}

Shift alignShift(Anchor anchor, float width, float height, const Box& box) {
    int col = 0, row = 0;   // 0 = left/top, 1 = centre, 2 = right/bottom
    switch (anchor) {
        case Anchor::TopLeft:       col = 0; row = 0; break;
        case Anchor::TopCentre:     col = 1; row = 0; break;
        case Anchor::TopRight:      col = 2; row = 0; break;
        case Anchor::LeftCentre:    col = 0; row = 1; break;
        case Anchor::Centre:        col = 1; row = 1; break;
        case Anchor::RightCentre:   col = 2; row = 1; break;
        case Anchor::BottomLeft:    col = 0; row = 2; break;
        case Anchor::BottomCentre:  col = 1; row = 2; break;
        case Anchor::BottomRight:   col = 2; row = 2; break;
    }
    Shift s;
    s.x = col == 0 ? -box.x0 : col == 1 ? (width - box.width()) / 2 - box.x0 : width - box.x1;
    s.y = row == 0 ? -box.y0 : row == 1 ? (height - box.height()) / 2 - box.y0 : height - box.y1;
    return s;
}

const Element* Image::firstGfx() const {
    for (auto& e : elements) {
        if (e.type == ElementType::Gfx && e.enabled) return &e;
    }
    return nullptr;
}

static std::string str(const XMLElement* e, const char* name, const char* def = "") {
    auto v = e->Attribute(name);
    return v ? v : def;
}

static bool parsePos(const XMLElement* e, Element& out) {
    auto v = e->Attribute("pos");
    if (!v) return true;
    return sscanf(v, "%f , %f", &out.x, &out.y) == 2;
}

static bool readColour(const XMLElement* e, const char* attr, Colour& out, std::string& err) {
    auto v = e->Attribute(attr);
    if (!v) return true;
    if (!Library::parseColour(v, out)) {
        err = std::string("bad colour '") + v + "'";
        return false;
    }
    return true;
}

static bool parseElement(const XMLElement* e, Element& out, std::string& err) {
    if (!parsePos(e, out)) { err = "bad pos in '" + out.id + "'"; return false; }

    if (auto a = e->Attribute("anchor")) {
        if (!Library::parseAnchor(a, out.anchor)) { err = "bad anchor in '" + out.id + "'"; return false; }
    }
    e->QueryFloatAttribute("scale", &out.scale);
    e->QueryFloatAttribute("rotation", &out.rotation);
    e->QueryFloatAttribute("alpha", &out.alpha);
    e->QueryBoolAttribute("flipx", &out.flipX);
    e->QueryBoolAttribute("flipy", &out.flipY);
    e->QueryBoolAttribute("enabled", &out.enabled);

    if (out.type == ElementType::Circle || out.type == ElementType::Ring) {
        e->QueryFloatAttribute("radius", &out.radius);
        if (out.radius <= 0) { err = "shape '" + out.id + "' without radius"; return false; }
        e->QueryFloatAttribute("thickness", &out.thickness);
        if (out.thickness <= 0) { err = "ring '" + out.id + "' without thickness"; return false; }
        return readColour(e, "colour", out.colour, err);
    }

    if (out.type == ElementType::Gfx) {
        out.name = str(e, "name");
        if (out.name.empty()) { err = "gfx '" + out.id + "' without name"; return false; }
        return readColour(e, "tint", out.colour, err);
    }

    out.text = str(e, "text");
    out.font = str(e, "font", "midnight");
    out.align = str(e, "align", "centre");
    e->QueryFloatAttribute("size", &out.size);
    e->QueryFloatAttribute("width", &out.width);
    e->QueryFloatAttribute("outline", &out.outline);
    e->QueryFloatAttribute("tracking", &out.tracking);
    e->QueryFloatAttribute("descent", &out.descent);
    return readColour(e, "colour", out.colour, err);
}

static bool parseImage(const XMLElement* node, Image& image, std::string& err) {
    for (auto e = node->FirstChildElement(); e; e = e->NextSiblingElement()) {
        std::string kind = e->Name();
        if (kind != "gfx" && kind != "txt" && kind != "circle" && kind != "ring") { err = "unknown element '" + kind + "'"; return false; }

        Element element;
        element.id = str(e, "id");
        if (element.id.empty()) { err = "element without id"; return false; }
        if (image.find(element.id) != nullptr) { err = "duplicate element id '" + element.id + "' in '" + image.id + "'"; return false; }

        element.type = kind == "gfx" ? ElementType::Gfx : kind == "txt" ? ElementType::Txt :
                       kind == "circle" ? ElementType::Circle : ElementType::Ring;
        if (!parseElement(e, element, err)) return false;
        image.elements.push_back(element);
    }
    return true;
}

bool Library::loadFromString(const std::string& xml) {
    lastError.clear();
    XMLDocument doc;
    if (doc.Parse(xml.c_str(), xml.size()) != XML_SUCCESS) {
        lastError = doc.ErrorStr();
        return false;
    }
    auto root = doc.FirstChildElement("images");
    if (!root) { lastError = "missing <images> root"; return false; }

    for (auto n = root->FirstChildElement(); n; n = n->NextSiblingElement()) {
        std::string kind = n->Name();
        if (kind != "image") { lastError = "unknown element '" + kind + "'"; return false; }

        Image image;
        image.id = str(n, "id");
        if (image.id.empty()) { lastError = "image without id"; return false; }

        if (auto a = n->Attribute("align")) {
            if (!parseAnchor(a, image.align)) { lastError = "bad align in '" + image.id + "'"; return false; }
            image.aligned = true;
        }
        if (auto s = n->Attribute("size")) {
            if (sscanf(s, "%f , %f", &image.width, &image.height) != 2) { lastError = "bad size in '" + image.id + "'"; return false; }
        }
        if (!parseImage(n, image, lastError)) return false;

        images[image.id] = image;
    }
    return true;
}

bool Library::loadFromFile(const std::string& filename) {
    FILE* f = fopen(filename.c_str(), "rb");
    if (!f) { lastError = "cannot open " + filename; return false; }
    std::string data;
    char buf[4096];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), f)) > 0) data.append(buf, n);
    fclose(f);
    return loadFromString(data);
}

const Image* Library::find(const std::string& id) const {
    auto it = images.find(id);
    return it == images.end() ? nullptr : &it->second;
}

}
