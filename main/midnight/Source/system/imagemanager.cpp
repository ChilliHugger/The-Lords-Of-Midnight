//
//  imagemanager.cpp
//  midnight
//

#include "../axmol_sdk.h"
#include "imagemanager.h"

USING_NS_AX;

void imagemanager::registerFiles(const std::vector<std::string>& names, const std::string& scenarioName)
{
    files = names;
    scenario = scenarioName;
    reload();
}

void imagemanager::setLocale(const std::string& newLocale)
{
    locale = newLocale;
    reload();
}

void imagemanager::reload()
{
    images = imagedef::Library();
    auto fileutils = FileUtils::getInstance();

    std::vector<std::string> folders;
    for (auto& base : { std::string("all"), scenario }) {
        if (base.empty())
            continue;
        folders.push_back(base);
        folders.push_back(base + "/en");
        if (locale != "en")
            folders.push_back(base + "/" + locale);
    }

    for (auto& name : files) {
        auto found = false;
        for (auto& folder : folders) {
            auto path = folder + "/" + name + ".xml";
            if (!fileutils->isFileExist(path))
                continue;
            found = true;
            if (!images.loadFromString(fileutils->getStringFromFile(path)))
                AXLOG("imagemanager: failed to load %s: %s", path.c_str(), images.error().c_str());
        }
        if (!found)
            AXLOG("imagemanager: no image file '%s'", name.c_str());
    }
}
