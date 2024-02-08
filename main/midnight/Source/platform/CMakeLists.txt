
if(ANDROID)
    set(HDR_PLATFORM
      Source/platform/Extensions.h
      Source/platform/android/Extensions-android.h
    )
    set(SRC_PLATFORM Source/platform/android/Extensions-android.cpp)
elseif(WINDOWS)
      set(HDR_PLATFORM
        Source/platform/Extensions.h
        Source/platform/win32/Extensions-win32.h)
      set(SRC_PLATFORM Source/platform/win32/Extensions-win32.cpp)
elseif(APPLE)
    if(MACOSX)
      set(HDR_PLATFORM
          Source/platform/Extensions.h
          Source/platform/mac/Extensions-mac.h
          Source/platform/apple/Extensions-apple.h
          )
      set(SRC_PLATFORM
          Source/platform/mac/Extensions-mac.mm
          Source/platform/apple/Extensions-apple.mm
          )
    elseif(IOS)
        set(HDR_PLATFORM
            Source/platform/Extensions.h
            Source/platform/ios/Extensions-ios.h
            Source/platform/apple/Extensions-apple.h
            )
        set(SRC_PLATFORM
            Source/platform/ios/Extensions-ios.mm
            Source/platform/apple/Extensions-apple.mm
            )
    endif()
endif()
