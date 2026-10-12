//
//  imagemanager.h
//  midnight
//
//  The XML image descriptions (icons, shields, lords...): which files there are, the scenario and locale they
//  are loaded for, and the merged library of images from them.
//

#ifndef imagemanager_h
#define imagemanager_h

#include <string>
#include <vector>
#include "../utils/imagedef.h"

class imagemanager
{
public:
    // Image files are registered by name (see moonring's imageDefinitions, like the atlases) and merged into one
    // library, so image ids are unique across all files. Each name is looked for, in this order, in
    //   all/<name>.xml  all/en/<name>.xml  all/<locale>/<name>.xml
    //   <scenario>/<name>.xml  <scenario>/en/<name>.xml  <scenario>/<locale>/<name>.xml
    // and every file found is loaded, a later file adding to or replacing the ids of an earlier one. So a
    // scenario can override the shared images, and a locale can replace the text and layout of the default ("en").
    void registerFiles(const std::vector<std::string>& names, const std::string& scenario);
    void setLocale(const std::string& locale);      // reloads the registered files
    void reload();

    const imagedef::Library& library() const { return images; }
    const imagedef::Image* find(const std::string& id) const { return images.find(id); }

private:
    imagedef::Library images;
    std::vector<std::string> files;
    std::string scenario;
    std::string locale{"en"};
};

#endif /* imagemanager_h */
