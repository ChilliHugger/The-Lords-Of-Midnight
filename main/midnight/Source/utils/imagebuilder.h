//
//  imagebuilder.h
//  midnight
//
//  Builds engine nodes from an imagedef::Image description. One instance, owned by moonring (imagebuilder).
//

#ifndef imagebuilder_h
#define imagebuilder_h

#include "../axmol_sdk.h"
#include "imagedef.h"
#include "../system/imagemanager.h"
#include <functional>
#include <set>
#include <string>
#include <vector>

class imagebuilder
{
public:
    using ImageView = ax::ui::ImageView;
    // element ids that are not built
    struct Options {
        std::set<std::string> disabled;
    };

public:
    // The images come from the files registered with the image manager (see there).
    void registerFiles(const std::vector<std::string>& names, const std::string& scenario) { manager.registerFiles(names, scenario); }
    void setLocale(const std::string& locale) { manager.setLocale(locale); }
    void reload() { manager.reload(); }
    const imagedef::Image* find(const std::string& id) const { return manager.find(id); }

    // debug: fill each flattened image's background so its bounds (and any clipping) can be seen
    void setDebugBackground(bool show) { debugBackground = show; }

    // clears the parent and adds the image's elements; an image view with a size shows them rendered into one
    // texture (see createIconButton), otherwise they are added as named children; a gfx is another image in the same
    // file (built as a nested image), else an atlas sprite
    bool build(const std::string& id, ax::Node* parent, const Options& options = Options());

    // a button for an icon: the art (first gfx, an atlas image, with its tint and alpha) and the other elements
    // (e.g. the text) are rendered into one texture, so fading the button fades the picture, not each element.
    // Returns null if the icon has no atlas art. An image with align moves everything together within the
    // cell, which is the image's size (else the art's original size).
    ax::ui::Button* createIconButton(const imagedef::Image& image, const Options& options = Options());

    // as createIconButton, for a button you already have (e.g. a subclass); false if the icon has no atlas art
    bool setupButton(ax::ui::Button* button, const imagedef::Image& image, const Options& options = Options());

    // renders the image (which needs a size) into a picture - a child sprite of the host node, which becomes the
    // size of the image. Call it again, with a changed copy of the image, to update it: for things like a lord's face
    // and status. 'resolution' multiplies the pixels rendered, for a picture drawn larger than its size
    // (the host can be scaled up without it going soft).
    bool setupPicture(ax::Node* host, const imagedef::Image& image, float resolution = 1.0f, const Options& options = Options());

    // sets the text of a built txt element by id (re-rendering a flattened image); false if there is none
    bool setText(ax::Node* parent, const std::string& elementId, const std::string& text);

    // show/hide a built element by id (re-rendering a flattened image)
    bool setEnabled(ax::Node* parent, const std::string& elementId, bool enabled);

private:
    using Stack = std::vector<std::string>;

    // a built element that is not yet attached: its node and the box its visible content occupies
    // in the parent's top-left origin space
    struct Placed {
        ax::Node* node;
        const imagedef::Element* element;
        imagedef::Box box;
        ax::Vec2 offset;    // moves the node so its ink, not its box, sits at the position
    };

    ax::Vec2 toAnchor(imagedef::Anchor anchor) const;
    ax::Color3B toColor(const imagedef::Colour& colour) const;
    imagedef::Box frameVisibleBox(ax::SpriteFrame* frame) const;
    imagedef::Box placedBox(ax::Node* node, const imagedef::Element& e, const imagedef::Box& visible) const;
    ax::Vec2 inkOffset(const imagedef::Element& e) const;

    ax::Node* createElement(const imagedef::Element& e, const Options& options, Stack& stack, imagedef::Box& visible);
    std::vector<Placed> placeElements(const imagedef::Image& image, const Options& options, Stack& stack);
    imagedef::Shift attachPlaced(ax::Node* parent, std::vector<Placed>& placed, const imagedef::Image& image, ax::Size cell);

    // The composite is rendered once into a texture, so opacity applies to the picture as a whole: translucent text
    // would otherwise show the art through it. The source nodes are kept (as the host's user object) so that
    // setText can change a label and render again.
    ax::Node* createSource(const ax::Size& size, const std::string& id);
    void renderSource(ax::Node* source, float resolution = 1.0f);
    void showFlattened(ax::Node* renderer) const;
    bool changeNode(ax::Node* parent, const std::string& elementId, const std::function<void(ax::Node*)>& change);

private:
    imagemanager manager;
    int flatCount{};
    bool debugBackground{};
};

#endif /* imagebuilder_h */
