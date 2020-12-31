
if(ANDROID)
elseif(WINDOWS)
      set(HDR_PLATFORM src/platform/win32/DeviceExt-win32.h)
      set(SRC_PLATFORM src/platform/win32/DeviceExt-win32.cpp)
elseif(APPLE)
    if(MACOSX)
      set(HDR_PLATFORM
          src/platform/apple/FileUtilsExt-mac.h
          src/platform/mac/DeviceExt-mac.h
          )
      set(SRC_PLATFORM
          src/platform/apple/FileUtilsExt-mac.mm
          src/platform/mac/DeviceExt-mac.mm
          )
    elseif(IOS)
    endif()
endif()
