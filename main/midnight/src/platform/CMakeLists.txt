
if(ANDROID)
elseif(WINDOWS)
      set(HDR_PLATFORM src/platform/win32/DeviceExt-win32.h)
      set(SRC_PLATFORM src/platform/win32/DeviceExt-win32.cpp)
elseif(APPLE)
      set(HDR_PLATFORM src/platform/apple/FileUtilsExt-apple.h)
      set(SRC_PLATFORM src/platform/apple/FileUtilsExt-apple.mm)
    if(MACOSX)
      set(HDR_PLATFORM
          src/platform/apple/FileUtilsExt-apple.h
          src/platform/mac/DeviceExt-mac.h
          )
      set(SRC_PLATFORM
          src/platform/apple/FileUtilsExt-apple.mm
          src/platform/mac/DeviceExt-mac.mm
          )
    elseif(IOS)
    endif()
endif()
