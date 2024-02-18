#/****************************************************************************
# Copyright (c) 2013-2014 cocos2d-x.org
# Copyright (c) 2015-2017 Chukong Technologies Inc.
#
# http://www.cocos2d-x.org
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
# ****************************************************************************/

cmake_minimum_required(VERSION 3.6)

if(TME STREQUAL "LOM")
  set(APP_BUILD _LOM_)
  set(APP_NAME midnight)
  set(LOM ON CACHE BOOL "Enable Lords of Midnight")
  if(WINDOWS)
    add_compile_definitions(ICON_LOM)
  endif()
elseif(TME STREQUAL "DDR")
  set(APP_BUILD _DDR_)
  set(APP_NAME revenge)
  set(DDR ON CACHE BOOL "Enable Doomdark's Revenge")
  if(WINDOWS)
    add_compile_definitions(ICON_DDR)
  endif()
endif()

project(${APP_NAME})

if(XCODE)
    if(NOT DEFINED CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET)
        SET (CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET 12.0)
    endif()
endif()

if(NOT DEFINED BUILD_ENGINE_DONE) # to test midnight into root project
    set(COCOS2DX_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cocos2d)
    set(CMAKE_MODULE_PATH ${COCOS2DX_ROOT_PATH}/cmake/Modules/)

    include(CocosBuildSet)
    add_subdirectory(${COCOS2DX_ROOT_PATH}/cocos ${ENGINE_BINARY_PATH}/cocos/core)
endif()

# record sources, headers, resources...
set(GAME_SOURCE)
set(GAME_HEADER)

set(GAME_RES_FOLDER
    "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
    "${CMAKE_CURRENT_SOURCE_DIR}/Resources-${APP_NAME}"
    )

if(APPLE OR WINDOWS)
    cocos_mark_multi_resources(common_res_files RES_TO "Resources" FOLDERS ${GAME_RES_FOLDER})
endif()

# add cross-platforms source files and header files

include(Source/extensions/CMakeLists.txt)
include(Source/frontend/CMakeLists.txt)
include(Source/landscaping/CMakeLists.txt)
include(Source/library/CMakeLists.txt)
include(Source/models/CMakeLists.txt)
include(Source/panels/CMakeLists.txt)
include(Source/system/CMakeLists.txt)
include(Source/tme/CMakeLists.txt)
include(Source/ui/CMakeLists.txt)
include(Source/utils/CMakeLists.txt)
include(Source/platform/CMakeLists.txt)

set(GAME_HEADER
  Source/cocos.h
  Source/AppDelegate.h
  ${HDR_EXTENSIONS}
  ${HDR_FRONTEND}
  ${HDR_LANDSCAPING}
  ${HDR_LIBRARY}
  ${HDR_MODELS}
  ${HDR_PANELS}
  ${HDR_SYSTEM}
  ${HDR_TME}
  ${HDR_UI}
  ${HDR_UTILS}
  ${HDR_PLATFORM}
  )

set(GAME_SOURCE
  Source/AppDelegate.cpp
  ${SRC_EXTENSIONS}
  ${SRC_FRONTEND}
  ${SRC_LANDSCAPING}
  ${SRC_LIBRARY}
  ${SRC_MODELS}
  ${SRC_PANELS}
  ${SRC_SYSTEM}
  ${SRC_TME}
  ${SRC_UI}
  ${SRC_UTILS}
  ${SRC_PLATFORM}
  )

list(APPEND GAME_SRC ${GAME_HEADER})

if(TESTS)
  include(tests/CMakeLists.txt)
else()
  add_compile_options("$<$<CONFIG:DEBUG>:-DMX_DEBUG>")
endif()

add_compile_options("-D${APP_BUILD}")

if(ANDROID)
    # change APP_NAME to the share library name for Android, it's value depend on AndroidManifest.xml
      list(APPEND GAME_SOURCE proj.android_${APP_NAME}/app/jni/hellocpp/main.cpp )
elseif(WINDOWS)
    add_compile_options(-D_MSC_EXTENSIONS -D_OS_DESKTOP_ -D_OS_WIN32_)
    list(APPEND GAME_HEADER
         proj.win32/main.h
         proj.win32/resource.h)

    list(APPEND GAME_SOURCE
         proj.win32/main.cpp
         ${common_res_files}
         )
    list(APPEND GAME_SOURCE
         proj.win32/${APP_NAME}/game.rc
         )
elseif(APPLE)
    if(IOS)
        add_compile_options(-D_OS_IOS_)
        list(APPEND GAME_HEADER
             proj.ios_mac/ios/AppController.h
             proj.ios_mac/ios/RootViewController.h
             )
        set(APP_UI_RES
            proj.ios_mac/ios/${APP_NAME}/LaunchScreen.storyboard
            proj.ios_mac/ios/${APP_NAME}/LaunchScreenBackground.png
            proj.ios_mac/ios/${APP_NAME}/Images.xcassets
            )
        list(APPEND GAME_SOURCE
             proj.ios_mac/ios/main.m
             proj.ios_mac/ios/AppController.mm
             proj.ios_mac/ios/RootViewController.mm
             proj.ios_mac/ios/Prefix.pch
             ${APP_UI_RES}
             )
    elseif(MACOSX)
        add_compile_options(-D_OS_OSX_ -D_OS_DESKTOP_)
        set(APP_UI_RES
            proj.ios_mac/mac/${APP_NAME}/Images.xcassets
            proj.ios_mac/mac/${APP_NAME}/Icon.icns
            )
        list(APPEND GAME_SOURCE
             proj.ios_mac/mac/main.cpp
             proj.ios_mac/mac/Prefix.pch
             ${APP_UI_RES}
             )
    endif()
    list(APPEND GAME_SOURCE ${common_res_files})
endif()

# mark app complie info and libs info
set(all_code_files
    ${GAME_HEADER}
    ${GAME_SOURCE}
    )
if(NOT ANDROID)
    add_executable(${APP_NAME} ${all_code_files})
else()
    add_library(${APP_NAME} SHARED ${all_code_files})
    add_subdirectory(${COCOS2DX_ROOT_PATH}/cocos/platform/android ${ENGINE_BINARY_PATH}/cocos/platform)
    target_link_libraries(${APP_NAME} -Wl,--whole-archive cpp_android_spec -Wl,--no-whole-archive)
endif()

if(TESTS)
  target_link_libraries(${APP_NAME}
    cocos2d
    Catch2::Catch2
  )
else()
  target_link_libraries(${APP_NAME} cocos2d)
endif()

target_include_directories(${APP_NAME}
        PRIVATE ${COCOS2DX_ROOT_PATH}/cocos/audio/include/
)

# mark app resources
setup_cocos_app_config(${APP_NAME})
if(APPLE)
    set_target_properties(${APP_NAME} PROPERTIES RESOURCE "${APP_UI_RES}")
    set_xcode_property(${APP_NAME} INSTALL_PATH "\$(LOCAL_APPS_DIR)")
    set_target_properties(${APP_NAME} PROPERTIES
           XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "$(inherited)")

    if(MACOSX)
        set_target_properties(${APP_NAME} PROPERTIES
          XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME YES
          MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/proj.ios_mac/mac/${APP_NAME}/Info.plist"
          XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS "${CMAKE_CURRENT_SOURCE_DIR}/proj.ios_mac/mac/midnight.entitlements")
    elseif(IOS)
        set_target_properties(${APP_NAME} PROPERTIES
          MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/proj.ios_mac/ios/${APP_NAME}/Info.plist"
          XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
          )
        set_xcode_property(${APP_NAME} ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon")
        # force x86_64 architecture for iphonesimulator builds
        set_target_properties(${APP_NAME} PROPERTIES
          XCODE_ATTRIBUTE_ARCHS[sdk=iphonesimulator*] x86_64
          XCODE_ATTRIBUTE_VALID_ARCHS[sdk=iphonesimulator*] x86_64
          )

    endif()

    # For code-signing, set the DEVELOPMENT_TEAM:
    set_xcode_property(${APP_NAME} DEVELOPMENT_TEAM "8K6PYTU8T7")
elseif(WINDOWS)
    cocos_copy_target_dll(${APP_NAME})
endif()

if(LINUX OR WINDOWS)
    cocos_get_resource_path(APP_RES_DIR ${APP_NAME})
    cocos_copy_target_res(${APP_NAME} LINK_TO ${APP_RES_DIR} FOLDERS ${GAME_RES_FOLDER})
endif()
