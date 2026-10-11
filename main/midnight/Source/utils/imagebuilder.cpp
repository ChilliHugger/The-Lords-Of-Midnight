//
//  imagebuilder.cpp
//  midnight
//

#include "imagebuilder.h"
#include "../system/resolutionmanager.h"
#include "../system/moonring.h"
#include "../ui/uihelper.h"
#include <map>
#include <algorithm>
#include <vector>

USING_NS_AX;
USING_NS_AX_UI;

using namespace imagedef;

static std::string currentLocale = "en";
static std::string currentScenario;
static std::vector<std::string> registeredFiles;
static imagedef::Library imageLibrary;

void imagebuilder::registerFiles(const std::vector<std::string>& names, const std::string& scenario)
{
    registeredFiles = names;
    currentScenario = scenario;
    reload();
}

void imagebuilder::setLocale(const std::string& locale)
{
    currentLocale = locale;
    reload();
}

void imagebuilder::reload()
{
    imageLibrary = imagedef::Library();
    auto fileutils = FileUtils::getInstance();

    std::vector<std::string> folders;
    for (auto& base : { std::string("all"), currentScenario }) {
        if (base.empty())
            continue;
        folders.push_back(base);
        folders.push_back(base + "/en");
        if (currentLocale != "en")
            folders.push_back(base + "/" + currentLocale);
    }

    for (auto& name : registeredFiles) {
        auto found = false;
        for (auto& folder : folders) {
            auto path = folder + "/" + name + ".xml";
            if (!fileutils->isFileExist(path))
                continue;
            found = true;
            if (!imageLibrary.loadFromString(fileutils->getStringFromFile(path)))
                AXLOG("imagebuilder: failed to load %s: %s", path.c_str(), imageLibrary.error().c_str());
        }
        if (!found)
            AXLOG("imagebuilder: no image file '%s'", name.c_str());
    }
}

imagedef::Library& imagebuilder::library()
{
    return imageLibrary;
}

const imagedef::Image* imagebuilder::find(const std::string& id)
{
    return imageLibrary.find(id);
}

static Vec2 toAnchor(Anchor a)
{
    switch (a) {
        case Anchor::TopLeft:       return uihelper::AnchorTopLeft;
        case Anchor::TopCentre:     return uihelper::AnchorTopCenter;
        case Anchor::TopRight:      return uihelper::AnchorTopRight;
        case Anchor::LeftCentre:    return uihelper::AnchorLeftCenter;
        case Anchor::Centre:        return uihelper::AnchorCenter;
        case Anchor::RightCentre:   return uihelper::AnchorRightCenter;
        case Anchor::BottomLeft:    return uihelper::AnchorBottomLeft;
        case Anchor::BottomCentre:  return uihelper::AnchorBottomCenter;
        default:                    return uihelper::AnchorBottomRight;
    }
}

static Color3B toColor(const Colour& c) { return Color3B(c.r, c.g, c.b); }

using Stack = std::vector<std::string>;

// a built element that is not yet attached: its node and the box its visible content occupies
// in the parent's top-left origin space
struct Placed {
    Node* node;
    const Element* element;
    Box box;
    Vec2 offset;    // moves the node so its ink, not its box, sits at the position
};

static std::vector<Placed> placeElements(const Library& lib, const imagedef::Image& image,
                                         const imagebuilder::Options& options, Stack& stack,
                                         const Element* skip = nullptr);

static Shift attachPlaced(Node* parent, std::vector<Placed>& placed, const imagedef::Image& image,
                          Size cell, const Box* extra = nullptr);

// where the visible part of a sprite frame lies inside its (untrimmed) content, top-left origin
static Box frameVisibleBox(SpriteFrame* frame)
{
    auto original = frame->getOriginalSize();
    auto size = frame->getRect().size;
    auto offset = frame->getOffset();   // centre of the visible part relative to the centre of the original, y up
    f32 x0 = original.width / 2 + offset.x - size.width / 2;
    f32 y0 = original.height / 2 - offset.y - size.height / 2;
    return { x0, y0, x0 + size.width, y0 + size.height };
}

// text hangs below its box by its outline and descenders: a bottom-anchored text is lifted by that
static Vec2 inkOffset(const Element& e)
{
    if (e.type != ElementType::Txt)
        return Vec2::ZERO;
    auto a = e.anchor;
    bool bottom = a == Anchor::BottomLeft || a == Anchor::BottomCentre || a == Anchor::BottomRight;
    if (!bottom)
        return Vec2::ZERO;
    // empirical: measured from a screenshot, the glyphs of an outlined label are drawn about a descender lower than the font metrics say
    f32 descent = e.descent >= 0 ? RES(e.descent) : RES(e.outline) + 0.36f * RES(e.size);
    return Vec2(0, descent);    // y is up
}

static Box placedBox(Node* node, const Element& e, const Box& visible)
{
    auto size = node->getContentSize();
    auto a = toAnchor(e.anchor);
    f32 left = RES(e.x) - a.x * size.width * e.scale;
    f32 top = RES(e.y) - (1 - a.y) * size.height * e.scale;
    return { left + visible.x0 * e.scale, top + visible.y0 * e.scale,
             left + visible.x1 * e.scale, top + visible.y1 * e.scale };
}

static Node* createElement(const Library& lib, const Element& e, const imagebuilder::Options& options,
                           Stack& stack, Box& visible)
{
    Node* node = nullptr;

    if (e.type == ElementType::Gfx) {
        // image ids and atlas names are distinct: an <image> of that name, else an atlas image
        auto nested = lib.find(e.name);
        if (nested != nullptr) {
            if (std::find(stack.begin(), stack.end(), e.name) != stack.end()) {
                AXLOG("imagebuilder: image '%s' contains itself", e.name.c_str());
                return nullptr;
            }
            auto container = Node::create();
            container->setIgnoreAnchorPointForPosition(false);
            container->setContentSize(Size(RES(nested->width), RES(nested->height)));
            container->setCascadeColorEnabled(true);
            container->setCascadeOpacityEnabled(true);
            container->setColor(toColor(e.colour));
            stack.push_back(e.name);
            auto nestedPlaced = placeElements(lib, *nested, options, stack);
            attachPlaced(container, nestedPlaced, *nested, container->getContentSize());
            stack.pop_back();
            node = container;
            visible = { 0, 0, container->getContentSize().width, container->getContentSize().height };
        } else {
            auto sprite = Sprite::createWithSpriteFrameName(e.name);
            if (sprite == nullptr)
                return nullptr;
            sprite->setColor(toColor(e.colour));
            sprite->setFlippedX(e.flipX);
            sprite->setFlippedY(e.flipY);
            node = sprite;
            visible = frameVisibleBox(sprite->getSpriteFrame());
        }
    } else {
        TTFConfig config(e.font == "button" ? BUTTON_TEXT_FONT : FONT_FILENAME, RES(e.size));
        auto label = Label::createWithTTF(config, e.text);
        if (label == nullptr)
            return nullptr;
        label->setTextColor(Color4B(toColor(e.colour)));
        if (e.outline > 0)
            label->enableOutline(Color4B(_clrBlack), RES(e.outline));
        if (e.tracking != 0)
            label->setAdditionalKerning(RES(e.size) * e.tracking / 1000.0f);
        if (e.width > 0)
            label->setWidth(RES(e.width));
        label->setAlignment(e.align == "left" ? TextHAlignment::LEFT :
                            e.align == "right" ? TextHAlignment::RIGHT : TextHAlignment::CENTER);
        node = label;
        visible = { 0, 0, label->getContentSize().width, label->getContentSize().height };
    }

    node->setName(e.id);
    node->setScale(e.scale);
    node->setRotation(e.rotation);
    node->setOpacity(ALPHA(e.alpha));
    node->setAnchorPoint(toAnchor(e.anchor));
    return node;
}

static std::vector<Placed> placeElements(const Library& lib, const imagedef::Image& image,
                                         const imagebuilder::Options& options, Stack& stack, const Element* skip)
{
    std::vector<Placed> placed;
    for (auto& e : image.elements) {
        if (&e == skip || !e.enabled || options.disabled.count(e.id) != 0)
            continue;
        Box visible;
        auto node = createElement(lib, e, options, stack, visible);
        if (node == nullptr)
            continue;
        node->retain();     // held until attached
        placed.push_back({ node, &e, placedBox(node, e, visible), inkOffset(e) });
    }
    return placed;
}

// adds the nodes to the parent, moving them by the image's alignment (if any) within 'cell'
static Shift attachPlaced(Node* parent, std::vector<Placed>& placed, const imagedef::Image& image,
                          Size cell, const Box* extra)
{
    Shift shift;
    if (image.aligned && cell.width > 0 && !placed.empty()) {
        Box all = extra != nullptr ? *extra : placed[0].box;
        for (auto& p : placed)
            all = all.united(p.box);
        shift = alignShift(image.align, cell.width, cell.height, all);
    }
    for (auto& p : placed) {
        uihelper::AddTopLeft(parent, p.node, RES(p.element->x), RES(p.element->y));
        p.node->setAnchorPoint(toAnchor(p.element->anchor));   // after positioning, which resets it
        p.node->setPosition(p.node->getPosition() + Vec2(shift.x, -shift.y) + p.offset);
        p.node->release();
    }
    return shift;
}

// The composite is rendered once into a texture, so opacity applies to the picture as a whole:
// translucent text would otherwise show the art through it. The source nodes are kept (as the host's
// user object) so that setText can change a label and render again.
static int flatCount = 0;

// debug: set to true to fill each flattened image's background so its bounds (and any clipping) can be seen
constexpr bool DEBUG_FILL_BACKGROUND = false;

// the source nodes also own their sprite frame: it is removed from the cache when the host (a button or an
// image view, which holds the source as its user object) is destroyed or builds again
class FlatSource : public Node
{
public:
    ~FlatSource() override
    {
        if (Director::getInstance() != nullptr)
            SpriteFrameCache::getInstance()->removeSpriteFrameByName(getName());
    }
};

static Node* createSource(const Size& size, const std::string& id)
{
    auto source = new FlatSource();
    source->init();
    source->autorelease();
    source->setIgnoreAnchorPointForPosition(false);
    source->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    source->setContentSize(size);
    source->setName(StringUtils::format("flat:%s:%d", id.c_str(), ++flatCount));   // the sprite frame's name
    return source;
}

// renders the source into its sprite frame (replacing the last one)
static void renderSource(Node* source)
{
    auto size = source->getContentSize();
    auto renderTexture = RenderTexture::create((int)ceilf(size.width), (int)ceilf(size.height), backend::PixelFormat::RGBA8);

    if (DEBUG_FILL_BACKGROUND)
        renderTexture->beginWithClear(1, 0, 1, 0.35f);
    else
        renderTexture->beginWithClear(0, 0, 0, 0);
    source->visit();
    renderTexture->end();

    auto texture = renderTexture->getSprite()->getTexture();
    auto frame = SpriteFrame::createWithTexture(texture, Rect(0, 0, size.width, size.height));
    SpriteFrameCache::getInstance()->addSpriteFrame(frame, source->getName());
}

// the render texture's pixels are premultiplied
static void showFlattened(Node* renderer)
{
    if (auto sprite = dynamic_cast<Sprite*>(renderer))
        sprite->setBlendFunc(BlendFunc::ALPHA_PREMULTIPLIED);
}

static void releaseSource(Node* host)
{
    host->setUserObject(nullptr);   // the old source removes its frame
}

bool imagebuilder::build(const std::string& id, Node* parent, const Options& options)
{
    auto& lib = library();
    auto image = lib.find(id);
    if (image == nullptr)
        return false;

    releaseSource(parent);
    parent->removeAllChildren();

    Size size(RES(image->width), RES(image->height));
    Stack stack{ id };
    auto placed = placeElements(lib, *image, options, stack);

    // an image view with a size shows the flattened picture
    auto view = dynamic_cast<ImageView*>(parent);
    if (view != nullptr && image->width > 0) {
        auto source = createSource(size, id);
        attachPlaced(source, placed, *image, size);
        renderSource(source);

        view->ignoreContentAdaptWithSize(true);
        view->loadTexture(source->getName(), Widget::TextureResType::PLIST);
        showFlattened(view->getVirtualRenderer());
        view->setUserObject(source);
        return true;
    }

    attachPlaced(parent, placed, *image, size);
    return true;
}

Button* imagebuilder::createIconButton(const imagedef::Image& image, const Options& options)
{
    // the main art is an atlas image; the other elements, e.g. the text, are drawn on top of it
    auto art = image.firstGfx();
    if (art == nullptr)
        return nullptr;
    auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(art->name);
    if (frame == nullptr)
        return nullptr;

    Stack stack{ image.id };
    auto placed = placeElements(library(), image, options, stack, art);

    // align the art and the other elements together within the art's original (untrimmed) size
    auto cell = frame->getOriginalSize();
    auto artBox = frameVisibleBox(frame);

    Shift shift;
    if (image.aligned) {
        Box all = artBox;
        for (auto& p : placed)
            all = all.united(p.box);
        shift = alignShift(image.align, cell.width, cell.height, all);
    }

    auto source = createSource(Size(cell.width, cell.height), image.id);

    auto sprite = Sprite::createWithSpriteFrame(frame);
    sprite->setName(art->id);
    sprite->setColor(toColor(art->colour));
    sprite->setOpacity(ALPHA(art->alpha));
    uihelper::AddTopLeft(source, sprite, shift.x, shift.y);

    for (auto& p : placed) {
        uihelper::AddTopLeft(source, p.node, RES(p.element->x), RES(p.element->y));
        p.node->setAnchorPoint(toAnchor(p.element->anchor));
        p.node->setPosition(p.node->getPosition() + Vec2(shift.x, -shift.y) + p.offset);
        p.node->release();
    }

    renderSource(source);

    auto button = Button::create(source->getName(), "", "", Widget::TextureResType::PLIST);
    showFlattened(button->getRendererNormal());
    button->setUserObject(source);
    return button;
}

bool imagebuilder::setText(Node* parent, const std::string& elementId, const std::string& text)
{
    // a flattened image keeps its source: change the label there and render again
    auto source = dynamic_cast<Node*>(parent->getUserObject());
    if (source != nullptr) {
        auto label = dynamic_cast<Label*>(source->getChildByName(elementId));
        if (label == nullptr)
            return false;
        label->setString(text);
        renderSource(source);

        if (auto button = dynamic_cast<Button*>(parent)) {
            button->loadTextureNormal(source->getName(), Widget::TextureResType::PLIST);
            showFlattened(button->getRendererNormal());
        } else if (auto view = dynamic_cast<ImageView*>(parent)) {
            view->loadTexture(source->getName(), Widget::TextureResType::PLIST);
            showFlattened(view->getVirtualRenderer());
        }
        return true;
    }

    auto label = dynamic_cast<Label*>(parent->getChildByName(elementId));
    if (label == nullptr)
        return false;
    label->setString(text);
    return true;
}

bool imagebuilder::setEnabled(Node* parent, const std::string& elementId, bool enabled)
{
    auto node = parent->getChildByName(elementId);
    if (node == nullptr)
        return false;
    node->setVisible(enabled);
    return true;
}
