
if(ANDROID)
    set(HDR_PLATFORM
      src/platform/Extensions.h
      src/platform/android/Extensions-android.h
    )
    set(SRC_PLATFORM src/platform/android/Extensions-android.cpp)
elseif(WINDOWS)
      set(HDR_PLATFORM
        src/platform/Extensions.h
        src/platform/win32/Extensions-win32.h)
      set(SRC_PLATFORM src/platform/win32/Extensions-win32.cpp)
elseif(APPLE)
    if(MACOSX)
      set(HDR_PLATFORM
          src/platform/Extensions.h
          src/platform/mac/Extensions-mac.h
          src/platform/apple/Extensions-apple.h
          )
      set(SRC_PLATFORM
          src/platform/mac/Extensions-mac.mm
          src/platform/apple/Extensions-apple.mm
          )
    elseif(IOS)
        set(HDR_PLATFORM
            src/platform/Extensions.h
            src/platform/ios/Extensions-ios.h
            src/platform/apple/Extensions-apple.h
            )
        set(SRC_PLATFORM
            src/platform/ios/Extensions-ios.mm
            src/platform/apple/Extensions-apple.mm
            )
    endif()
endif()
