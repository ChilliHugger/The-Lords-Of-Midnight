//
//  Extensions.h
//  midnight
//
//  Created by Chris Wild on 17/12/2020.
//
#pragma once

#if defined(_OS_WIN32_)
    #include "win32/Extensions-win32.h"
#elif defined(_OS_OSX_)
    #include "apple/Extensions-apple.h"
    #include "mac/Extensions-mac.h"
#elif defined(_OS_IOS_) || defined(_OS_TVOS_)
    #include "apple/Extensions-apple.h"
    #include "ios/Extensions-ios.h"
#elif AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
    #include "android/Extensions-android.h"
#endif
