//
//  Exensions-android.cpp
//  midnight
//
//  Created by Chris Wild on 17/12/2020.
//
#include "Extensions-android.h"
#include "../../library/chilli.h"
#include "../../cocos.h"
#include <string>

USING_NS_CC;

namespace chilli
{
    namespace extensions
    {
        const std::string utilsClassName = "com.chilli.extensions.Utils";

        std::string getBuildNo()
        {
            // Build is xyyyzzz, where xxx=major, yyy=minor, zzz=build
            auto version =  JniHelper::callStaticStringMethod(utilsClassName.c_str(), "getBuildNo");
            if(version.size()>3) {
                return version.substr(version.size() - 3);
            }
            return version;
        }

        std::string getVersion()
        {
            return Application::getInstance()->getVersion();
        }
    }
}
