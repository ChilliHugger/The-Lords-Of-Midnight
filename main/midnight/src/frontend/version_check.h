//
//  version_check.h
//  midnight
//
//  Created by Chris Wild on 02/01/2021.
//

#pragma once

//#define _OS_MAC_APPSTORE_

#if defined(_DDR_)
#ifdef _MSC_VER
#if defined(_OS_DESKTOP_)
    #define _USE_VERSION_CHECK_
    #define _VERSION_URL    "https://doomdarksrevenge.com/version/ddr_v2.0_win.cfg"
#endif
#endif

#if defined(_OS_OSX_)
#if !defined(_OS_MAC_APPSTORE_)
#define _USE_VERSION_CHECK_
#define _VERSION_URL    "https://doomdarksrevenge.com/version/ddr_v2.0_osx.cfg"
#endif
#endif

#endif // _DDR_


#if defined(_LOM_)
#ifdef _MSC_VER
#if defined(_OS_DESKTOP_)
    #define _USE_VERSION_CHECK_
    #define _VERSION_URL    "https://thelordsofmidnight.com/version/lom_v2.0_win.cfg"
#endif
#endif

#if defined(_OS_OSX_)
#if !defined(_OS_MAC_APPSTORE_)
    #define _USE_VERSION_CHECK_
    #define _VERSION_URL    "https://thelordsofmidnight.com/version/lom_v2.0_osx.cfg"
#endif
#endif

#endif // _LOM_


