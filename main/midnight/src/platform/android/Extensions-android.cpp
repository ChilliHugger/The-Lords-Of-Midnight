//
//  Exensions-android.cpp
//  midnight
//
//  Created by Chris Wild on 17/12/2020.
//
#include "Extensions-android.h"
#include "../../library/chilli.h"
#include <string>

USING_NS_CC;

namespace chilli
{
    namespace extensions
    {
        const std::string utilsClassName = "com.chillihugger.extensions.Utils";

        std::string getBuildNo()
        {
            // Build is xyyyzzz, where xxx=major, yyy=minor, zzz=build
            auto version =  JniHelper::callStaticStringMethod(utilsClassName, "getBuildNo");
            return version.substr(version.size() - 3);
        }
    }
}
