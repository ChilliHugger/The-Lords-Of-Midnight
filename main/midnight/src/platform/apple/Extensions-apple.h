//
//  DeviceExt-mac.h
//  midnight
//
//  Created by Chris Wild on 17/12/2020.
//

#pragma once

#include "cocos2d.h"

namespace chilli
{
    namespace extensions
    {
        void getDesktopSize(int& width, int& height);
        std::string getApplicationSupportPath();
        std::string getVersion();
        std::string getBuildNo();
    }
}

