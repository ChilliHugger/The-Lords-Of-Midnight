//
//  imagebuilder.h
//  midnight
//
//  Builds engine nodes from an imagedef::Image description.
//

#ifndef imagebuilder_h
#define imagebuilder_h

#include "../axmol_sdk.h"
#include "imagedef.h"
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
    // Image files are registered by name (see moonring's imageDefinitions, like the atlases) and merged into one
    // library, so image ids are unique across all files. Each name is looked for, in this order, in
    //   all/<name>.xml  all/en/<name>.xml  all/<locale>/<name>.xml
    //   <scenario>/<name>.xml  <scenario>/en/<name>.xml  <scenario>/<locale>/<name>.xml
    // and every file found is loaded, a later file adding to or replacing the ids of an earlier one. So a
    // scenario can override the shared images, and a locale can replace the text and layout of the default ("en").
    static void registerFiles(const std::vector<std::string>& names, const std::string& scenario);
    static void setLocale(const std::string& locale);   // reloads the registered files
    static void reload();
    static imagedef::Library& library();
    static const imagedef::Image* find(const std::string& id);

    // clears the parent and adds the image's elements; an image view with a size shows them rendered into one
    // texture (see createIconButton), otherwise they are added as named children; a gfx is another image in the same
    // file (built as a nested image), else an atlas sprite
    static bool build(const std::string& id, ax::Node* parent, const Options& options = Options());

    // a button for an icon: the art (first gfx, an atlas image, with its tint and alpha) and the other elements
    // (e.g. the text) are rendered into one texture, so fading the button fades the picture, not each element.
    // Returns null if the icon has no atlas art. An image with align moves everything together within the
    // art's original size.
    static ax::ui::Button* createIconButton(const imagedef::Image& image, const Options& options = Options());

    // sets the text of a built txt element by id (re-rendering a flattened image); false if there is none
    static bool setText(ax::Node* parent, const std::string& elementId, const std::string& text);

    // show/hide a built element by id
    static bool setEnabled(ax::Node* parent, const std::string& elementId, bool enabled);
};

#endif /* imagebuilder_h */
