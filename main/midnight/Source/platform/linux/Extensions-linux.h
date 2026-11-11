//
//  Extensions-linux.h
//  midnight
//
#pragma once

#include "axmol.h"

namespace chilli
{
    namespace extensions
    {
        void getDesktopSize(int& width, int& height);
        std::string getVersion();
        std::string getBuildNo();
        std::string getBundleResourcePath();
    }
}
