//
//  imagebuilder.cpp
//  midnight
//

#include "imagebuilder.h"
#include "../system/resolutionmanager.h"
#include "../system/moonring.h"
#include "../ui/uihelper.h"
#include <map>
#include <functional>
#include <algorithm>
#include <vector>

USING_NS_AX;
USING_NS_AX_UI;

using namespace imagedef;

Vec2 imagebuilder::toAnchor(Anchor anchor) const
{
    switch (anchor) {
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

Color3B imagebuilder::toColor(const Colour& c) const { return Color3B(c.r, c.g, c.b); }

// where the visible part of a sprite frame lies inside its (untrimmed) content, top-left origin
Box imagebuilder::frameVisibleBox(SpriteFrame* frame) const
{
    auto original = frame->getOriginalSize();
    auto size = frame->getRect().size;
    auto offset = frame->getOffset();   // centre of the visible part relative to the centre of the original, y up
    f32 x0 = original.width / 2 + offset.x - size.width / 2;
    f32 y0 = original.height / 2 - offset.y - size.height / 2;
    return { x0, y0, x0 + size.width, y0 + size.height };
}

// text hangs below its box by its outline and descenders: a bottom-anchored text is lifted by that
Vec2 imagebuilder::inkOffset(const Element& e) const
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

Box imagebuilder::placedBox(Node* node, const Element& e, const Box& visible) const
{
    auto size = node->getContentSize();
    auto a = toAnchor(e.anchor);
    f32 left = RES(e.x) - a.x * size.width * e.scale;
    f32 top = RES(e.y) - (1 - a.y) * size.height * e.scale;
    return { left + visible.x0 * e.scale, top + visible.y0 * e.scale,
             left + visible.x1 * e.scale, top + visible.y1 * e.scale };
}

Node* imagebuilder::createElement(const Element& e, const Options& options, Stack& stack, Box& visible)
{
    Node* node = nullptr;

    if (e.type == ElementType::Gfx) {
        // image ids and atlas names are distinct: an <image> of that name, else an atlas image
        auto nested = manager.find(e.name);
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
            auto nestedPlaced = placeElements(*nested, options, stack);
            attachPlaced(container, nestedPlaced, *nested, container->getContentSize());
            stack.pop_back();
            node = container;
            visible = { 0, 0, container->getContentSize().width, container->getContentSize().height };
        } else {
            // an atlas image, else an image file
            auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(e.name);
            auto sprite = frame != nullptr ? Sprite::createWithSpriteFrame(frame) : Sprite::create(e.name);
            if (sprite == nullptr)
                return nullptr;
            sprite->setColor(toColor(e.colour));
            sprite->setFlippedX(e.flipX);
            sprite->setFlippedY(e.flipY);
            node = sprite;
            auto size = sprite->getContentSize();
            visible = frame != nullptr ? frameVisibleBox(frame) : Box{ 0, 0, size.width, size.height };
        }
    } else if (e.type == ElementType::Gradient) {
        // colours premultiplied by their alpha, as the picture's pixels are
        auto premultiplied = [](const Colour& c, f32 alpha) {
            return Color4B((u8)(c.r * alpha), (u8)(c.g * alpha), (u8)(c.b * alpha), (u8)(alpha * 255));
        };
        auto gradient = LayerGradient::create(premultiplied(e.colour, e.alpha), premultiplied(e.colour2, e.alpha2));
        gradient->setContentSize(Size(RES(e.width), RES(e.height)));
        gradient->setBlendFunc(BlendFunc::ALPHA_PREMULTIPLIED);
        node = gradient;
        visible = { 0, 0, RES(e.width), RES(e.height) };
    } else if (e.type == ElementType::Circle || e.type == ElementType::Ring) {
        // drawn into the picture at whatever resolution it is rendered at. The colour is premultiplied by the
        // alpha (as the picture's pixels are), so a faint shape is not squared into nothing.
        const f32 radius = RES(e.radius);
        const unsigned int segments = 96;
        Color4F colour(e.colour.r / 255.0f * e.alpha, e.colour.g / 255.0f * e.alpha, e.colour.b / 255.0f * e.alpha, e.alpha);
        auto shape = DrawNode::create();
        shape->setContentSize(Size(radius * 2, radius * 2));
        shape->setBlendFunc(BlendFunc::ALPHA_PREMULTIPLIED);
        if (e.type == ElementType::Circle) {
            shape->drawSolidCircle(Vec2(radius, radius), radius, 0, segments, colour);
        } else {
            // the line is centred on the circle drawn: keep its outer edge on the radius
            auto thickness = RES(e.thickness);
            shape->drawCircle(Vec2(radius, radius), radius - thickness / 2, 0, segments, false, colour, thickness);
        }
        node = shape;
        visible = { 0, 0, radius * 2, radius * 2 };
    } else {
        TTFConfig config(e.font == "button" ? BUTTON_TEXT_FONT : FONT_FILENAME, RES(e.size));
        // as the font configs in uihelper: plain bitmap glyphs, not the engine's default of distance fields (soft at small sizes)
        config.glyphs = GlyphCollection::DYNAMIC;
        config.outlineSize = 0;
        config.distanceFieldEnabled = false;
        auto label = Label::createWithTTF(config, e.text);
        if (label == nullptr)
            return nullptr;
        label->getFontAtlas()->setAntiAliasTexParameters();     // as the other labels: smooth, not nearest-neighbour
        label->setTextColor(Color4B(toColor(e.colour)));
        if (e.outline > 0)
            label->enableOutline(Color4B(_clrBlack), uihelper::outlineSize(e.outline));
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
    node->setOpacity((e.type == ElementType::Circle || e.type == ElementType::Ring || e.type == ElementType::Gradient) ? 255 : ALPHA(e.alpha));   // shapes have the alpha in their colour
    node->setAnchorPoint(toAnchor(e.anchor));
    return node;
}

std::vector<imagebuilder::Placed> imagebuilder::placeElements(const imagedef::Image& image, const Options& options, Stack& stack)
{
    std::vector<Placed> placed;
    for (auto& e : image.elements) {
        if (!e.enabled || options.disabled.count(e.id) != 0)
            continue;
        Box visible;
        auto node = createElement(e, options, stack, visible);
        if (node == nullptr)
            continue;
        node->retain();     // held until attached
        placed.push_back({ node, &e, placedBox(node, e, visible), inkOffset(e) });
    }
    return placed;
}

// adds the nodes to the parent, moving them by the image's alignment (if any) within 'cell'
Shift imagebuilder::attachPlaced(Node* parent, std::vector<Placed>& placed, const imagedef::Image& image, Size cell)
{
    Shift shift;
    if (image.aligned && cell.width > 0 && !placed.empty()) {
        Box all = placed[0].box;
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

Node* imagebuilder::createSource(const Size& size, const std::string& id)
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
void imagebuilder::renderSource(Node* source, f32 resolution)
{
    // resolution > 1 renders more pixels than the cell is big, for pictures that are drawn larger than their size
    auto size = source->getContentSize();
    auto renderTexture = RenderTexture::create((int)ceilf(size.width * resolution), (int)ceilf(size.height * resolution), backend::PixelFormat::RGBA8);
    source->setScale(resolution);

    if (debugBackground)
        renderTexture->beginWithClear(1, 0, 1, 0.35f);
    else
        renderTexture->beginWithClear(0, 0, 0, 0);
    source->visit();
    renderTexture->end();
    source->setScale(1);

    auto texture = renderTexture->getSprite()->getTexture();
    texture->setAntiAliasTexParameters();   // smooth when drawn larger or smaller than it was rendered
    auto frame = SpriteFrame::createWithTexture(texture, Rect(0, 0, size.width * resolution, size.height * resolution));
    SpriteFrameCache::getInstance()->addSpriteFrame(frame, source->getName());
}

// the render texture's pixels are premultiplied
void imagebuilder::showFlattened(Node* renderer) const
{
    if (auto sprite = dynamic_cast<Sprite*>(renderer)) {
        sprite->setBlendFunc(BlendFunc::ALPHA_PREMULTIPLIED);
#if defined(AX_USE_GL)
        // an OpenGL render target is stored bottom-up. RenderTexture flips its own sprite to
        // compensate, but we draw from the texture through a sprite frame, so do it here
        sprite->setFlippedY(true);
#endif
    }
}

bool imagebuilder::build(const std::string& id, Node* parent, const Options& options)
{
    auto image = manager.find(id);
    if (image == nullptr)
        return false;

    parent->setUserObject(nullptr);   // the old source removes its frame
    parent->removeAllChildren();

    Size size(RES(image->width), RES(image->height));
    Stack stack{ id };
    auto placed = placeElements(*image, options, stack);

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

bool imagebuilder::setupButton(Button* button, const imagedef::Image& image, const Options& options)
{
    // an icon needs atlas art (its first gfx); the cell is the image's size, else the art's original size
    auto art = image.firstGfx();
    if (art == nullptr)
        return false;
    auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(art->name);
    if (frame == nullptr)
        return false;

    Size cell = image.width > 0 ? Size(RES(image.width), RES(image.height)) : Size(frame->getOriginalSize());

    Stack stack{ image.id };
    auto placed = placeElements(image, options, stack);

    // everything is drawn into one texture, aligned together within the cell if the image asks
    auto source = createSource(cell, image.id);
    attachPlaced(source, placed, image, cell);
    renderSource(source);

    button->loadTextureNormal(source->getName(), Widget::TextureResType::PLIST);
    showFlattened(button->getRendererNormal());
    button->setUserObject(source);
    return true;
}

bool imagebuilder::setupPicture(Node* host, const imagedef::Image& image, f32 resolution, const Options& options)
{
    // the image needs a size: it is the host's cell
    if (image.width <= 0)
        return false;

    Size cell(RES(image.width), RES(image.height));
    Stack stack{ image.id };
    auto placed = placeElements(image, options, stack);

    auto source = createSource(cell, image.id);
    attachPlaced(source, placed, image, cell);
    renderSource(source, resolution);

    // the picture is a child of the host, drawn smaller than its texture so it is cell sized, sharp when the
    // host is scaled up. The host is plain, so scaling and fading it work as for any node.
    auto picture = dynamic_cast<Sprite*>(host->getChildByName("picture"));
    if (picture == nullptr) {
        picture = Sprite::create();
        picture->setName("picture");
        picture->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
        picture->setPosition(Vec2::ZERO);
        host->addChild(picture);
        host->setIgnoreAnchorPointForPosition(false);
        host->setCascadeOpacityEnabled(true);
    }
    host->setContentSize(cell);
    picture->setSpriteFrame(source->getName());
    picture->setScale(1.0f / resolution);
    showFlattened(picture);
    host->setUserObject(source);     // replaces (and so removes the frame of) the previous picture
    return true;
}

Button* imagebuilder::createIconButton(const imagedef::Image& image, const Options& options)
{
    auto button = Button::create();
    return setupButton(button, image, options) ? button : nullptr;
}

// changes a node of a flattened image and renders it again; a plain (unflattened) image just changes the node
bool imagebuilder::changeNode(Node* parent, const std::string& elementId, const std::function<void(Node*)>& change)
{
    auto source = dynamic_cast<Node*>(parent->getUserObject());
    auto node = (source != nullptr ? source : parent)->getChildByName(elementId);
    if (node == nullptr)
        return false;
    change(node);

    if (source == nullptr)
        return true;

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

bool imagebuilder::setText(Node* parent, const std::string& elementId, const std::string& text)
{
    return changeNode(parent, elementId, [&](Node* node) {
        if (auto label = dynamic_cast<Label*>(node))
            label->setString(text);
    });
}

bool imagebuilder::setEnabled(Node* parent, const std::string& elementId, bool enabled)
{
    return changeNode(parent, elementId, [&](Node* node) { node->setVisible(enabled); });
}
