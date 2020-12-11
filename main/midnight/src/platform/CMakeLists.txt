
if(ANDROID)
elseif(WINDOWS)

elseif(APPLE)
    set(HDR_PLATFORM
        src/platform/apple/FileUtilsExt-apple.h
        )
    set(SRC_PLATFORM
        src/platform/apple/FileUtilsExt-apple.mm
        )
    if(MACOSX)
    elseif(IOS)
    endif()
endif()
