#/****************************************************************************
# Copyright (c) 2013-2014 cocos2d-x.org
# Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).
#
# https://axmolengine.github.io/
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

cmake_policy(SET CMP0054 NEW)

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

cmake_minimum_required(VERSION 3.31.2)

if(MACOSX)
  set_target_properties(${APP_NAME} PROPERTIES
    CMAKE_OSX_SYSROOT 14.2
    )
  set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Minimum OS X deployment version")
endif()

project(${APP_NAME})

 set(AX_ENABLE_CONSOLE OFF CACHE BOOL "" FORCE)
 set(AX_ENABLE_PHYSICS OFF CACHE BOOL "" FORCE)
 set(AX_ENABLE_3D OFF CACHE BOOL "" FORCE)
 set(AX_ENABLE_EXT_GUI OFF CACHE BOOL "")
 set(AX_ENABLE_EXT_ASSETMANAGER OFF CACHE BOOL "")
 set(AX_ENABLE_EXT_SPINE OFF CACHE BOOL "")
 set(AX_ENABLE_EXT_DRAGONBONES OFF CACHE BOOL "")
 set(AX_ENABLE_EXT_COCOSTUDIO OFF CACHE BOOL "")
 set(AX_ENABLE_EXT_FAIRYGUI OFF CACHE BOOL "")
 set(AX_ENABLE_EXT_LIVE2D OFF CACHE BOOL "")
 set(AX_ENABLE_EXT_EFFEKSEER OFF CACHE BOOL "")
 set(AX_ENABLE_EXT_JSONDEFAULT OFF CACHE BOOL "")
 set(AX_ENABLE_EXT_DRAWNODEEX OFF CACHE BOOL "")
 set(AX_ENABLE_EXT_LUA OFF CACHE BOOL "" FORCE)
 set(AX_ENABLE_MFMEDIA OFF CACHE BOOL "")
 set(AX_ENABLE_MSEDGE_WEBVIEW2 OFF CACHE BOOL "")

 set(AX_ENABLE_EXT_IMGUI OFF CACHE BOOL "")

 set(AX_WITH_WEBP OFF CACHE BOOL "")
 set(AX_WITH_JPEG OFF CACHE BOOL "")

if(NOT DEFINED BUILD_ENGINE_DONE) # to test new into root project
    if(XCODE)
        set(CMAKE_XCODE_GENERATE_TOP_LEVEL_PROJECT_ONLY TRUE)
    endif()

    # config quick starter batch script run.bat for windows
    if(WIN32)
        file(RELATIVE_PATH CMAKE_BUILD_RELATIVE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
        configure_file("${CMAKE_CURRENT_SOURCE_DIR}/run.bat.in" "${CMAKE_CURRENT_SOURCE_DIR}/run.bat" @ONLY)
    endif()

    set(_AX_ROOT "$ENV{AX_ROOT}")

    if(NOT (_AX_ROOT STREQUAL ""))
        file(TO_CMAKE_PATH ${_AX_ROOT} _AX_ROOT)
        message(STATUS "Using system env var _AX_ROOT=${_AX_ROOT}")
    else()
        message(FATAL_ERROR "Please run setup.ps1 add system env var 'AX_ROOT' to specific the engine root")
    endif()

    set(CMAKE_MODULE_PATH ${_AX_ROOT}/cmake/Modules/)

    include(AXBuildSet)
    set(_AX_USE_PREBUILT FALSE)
    if ((WIN32 OR LINUX) AND DEFINED AX_PREBUILT_DIR AND IS_DIRECTORY ${_AX_ROOT}/${AX_PREBUILT_DIR})
        set(_AX_USE_PREBUILT TRUE)
    endif()

    if (NOT _AX_USE_PREBUILT)
        add_subdirectory(${_AX_ROOT}/core ${ENGINE_BINARY_PATH}/axmol/core)
    endif()
endif()

# The common cross-platforms source files and header files
include(Source/extensions/CMakeLists.txt)
include(Source/frontend/CMakeLists.txt)
include(Source/landscaping/CMakeLists.txt)
include(Source/library/CMakeLists.txt)
include(Source/models/CMakeLists.txt)
include(Source/panels/CMakeLists.txt)
include(Source/system/CMakeLists.txt)
include(Source/tinyxml2/CMakeLists.txt)
include(Source/tme/CMakeLists.txt)
include(Source/ui/CMakeLists.txt)
include(Source/utils/CMakeLists.txt)
include(Source/platform/CMakeLists.txt)

set(GAME_HEADER
       Source/axmol_sdk.h
       Source/AppDelegate.h
       ${HDR_EXTENSIONS}
       ${HDR_FRONTEND}
       ${HDR_LANDSCAPING}
       ${HDR_LIBRARY}
       ${HDR_MODELS}
       ${HDR_PANELS}
       ${HDR_SYSTEM}
       ${HDR_TINYXML}
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
       ${SRC_TINYXML}
       ${SRC_TME}
       ${SRC_UI}
       ${SRC_UTILS}
       ${SRC_PLATFORM}
    )

#set(GAME_INC_DIRS
#  "${CMAKE_CURRENT_SOURCE_DIR}/Source"
#)

list(APPEND GAME_SRC ${GAME_HEADER})

set(content_folder
    "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
    "${CMAKE_CURRENT_SOURCE_DIR}/Resources-${APP_NAME}"
    )

if(APPLE)
    ax_mark_multi_resources(common_content_files RES_TO "Resources" FOLDERS ${content_folder})
elseif(WINDOWS)
    ax_mark_multi_resources(common_content_files RES_TO "Content" FOLDERS ${content_folder})
endif()

if(TESTS)
  include(tests/CMakeLists.txt)
else()
  add_compile_options("$<$<CONFIG:DEBUG>:-DMX_DEBUG>")
endif()

add_compile_options("-D${APP_BUILD}")

if(ANDROID)
    # the APP_NAME should match on AndroidManifest.xml
    list(APPEND GAME_SOURCE
         proj.android/app/jni/main.cpp
         )
#elseif(LINUX)
#    list(APPEND GAME_SOURCE
#         proj.linux/main.cpp
#         )
#    list(APPEND GAME_SOURCE ${common_content_files})
#elseif(WASM)
#    list(APPEND GAME_SOURCE
#         proj.wasm/main.cpp
#         )
#    list(APPEND GAME_SOURCE ${common_content_files})
elseif(WINDOWS)
    if(NOT WINRT)
        add_compile_options(-D_OS_DESKTOP_ -D_OS_WIN32_)
        list(APPEND GAME_HEADER
            proj.win32/main.h
            proj.win32/resource.h
            )
        list(APPEND GAME_SOURCE
            proj.win32/main.cpp
            proj.win32/${APP_NAME}/game.rc
            )
#    else()
#        ax_setup_winrt_sources()
    endif()
    list(APPEND GAME_SOURCE ${common_content_files})
elseif(APPLE)
    if(IOS)
        list(APPEND GAME_HEADER
             proj.ios_mac/ios/AppController.h
             proj.ios_mac/ios/RootViewController.h
             )

        if (TVOS)
            set(APP_UI_RES
                proj.ios_mac/ios/targets/tvos/LaunchScreenBackground.png
                proj.ios_mac/ios/targets/tvos/LaunchScreen.storyboard
                proj.ios_mac/ios/targets/tvos/Images.xcassets
                )
        else()
            add_compile_options(-D_OS_IOS_)
            if (LOM)
                   set(APP_UI_RES
                       proj.ios_mac/ios/targets/ios/${APP_NAME}/LaunchScreenBackground.png
                       proj.ios_mac/ios/targets/ios/${APP_NAME}/LaunchScreen.storyboard
                       proj.ios_mac/ios/targets/ios/${APP_NAME}/Images.xcassets
                       )
            elseif(DDR)
                   set(APP_UI_RES
                       proj.ios_mac/ios/targets/ios/${APP_NAME}/LaunchScreenBackground.png
                       proj.ios_mac/ios/targets/ios/${APP_NAME}/LaunchScreenBackground2.png
                       proj.ios_mac/ios/targets/ios/${APP_NAME}/LaunchScreen.storyboard
                       proj.ios_mac/ios/targets/ios/${APP_NAME}/Images.xcassets
                       )
            endif()
        endif()

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
    list(APPEND GAME_SOURCE ${common_content_files})
endif()

# mark app complie info and libs info
set(APP_SOURCES
    ${GAME_HEADER}
    ${GAME_SOURCE}
    )
if(NOT ANDROID)
    add_executable(${APP_NAME} ${APP_SOURCES})
else()
    message(STATUS "Source files: ${APP_NAME} '${APP_SOURCES}'")
    add_library(${APP_NAME} SHARED ${APP_SOURCES})
    # whole archive for jni
    target_link_libraries(${APP_NAME} -Wl,--whole-archive cpp_android_spec -Wl,--no-whole-archive)
endif()

if (NOT _AX_USE_PREBUILT)
    if (TESTS)
      target_link_libraries(${APP_NAME}
        ${_AX_CORE_LIB}
        Catch2::Catch2
        )
    else()
      target_link_libraries(${APP_NAME}
        ${_AX_CORE_LIB}
        )
    endif()
endif()

# The optional thirdparties(not dependent by engine)
if (AX_WITH_YAML_CPP)
    list(APPEND GAME_INC_DIRS "${_AX_ROOT}/thirdparty/yaml-cpp/include")
endif()

target_include_directories(${APP_NAME} PRIVATE ${GAME_INC_DIRS})


# mark app resources, resource will be copy auto after mark
ax_setup_app_config(${APP_NAME})
if(APPLE)
    set_target_properties(${APP_NAME} PROPERTIES RESOURCE "${APP_UI_RES}")
    set_xcode_property(${APP_NAME} INSTALL_PATH "\$(LOCAL_APPS_DIR)")
    set_target_properties(${APP_NAME} PROPERTIES
           XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "$(inherited)")

    if(MACOSX)
        set_target_properties(${APP_NAME} PROPERTIES
              XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME YES
              MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/proj.ios_mac/mac/${APP_NAME}/Info.plist"
              XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS "${CMAKE_CURRENT_SOURCE_DIR}/proj.ios_mac/mac/midnight.entitlements"
        )
#    elseif(TVOS)
#       set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/proj.ios_mac/ios/targets/tvos/Info.plist")
#       set_xcode_property(${APP_NAME} ASSETCATALOG_COMPILER_APPICON_NAME "Brand Assets")
    elseif(IOS)
       set_target_properties(${APP_NAME} PROPERTIES
              MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/proj.ios_mac/ios/targets/ios/${APP_NAME}/Info.plist"
       )
       set_xcode_property(${APP_NAME} ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon")
       set_xcode_property(${APP_NAME} TARGETED_DEVICE_FAMILY "1,2")
       set_target_properties(${APP_NAME} PROPERTIES
              XCODE_ATTRIBUTE_ARCHS[sdk=iphonesimulator*] x86_64
              XCODE_ATTRIBUTE_VALID_ARCHS[sdk=iphonesimulator*] x86_64
        )
    endif()

    # For code-signing, set the DEVELOPMENT_TEAM:
    set_xcode_property(${APP_NAME} DEVELOPMENT_TEAM "8K6PYTU8T7")

    if(MACOSX)
        set_xcode_property(${APP_NAME} CODE_SIGN_IDENTITY "")
        set_xcode_property(${APP_NAME} CODE_SIGNING_ALLOWED "YES")
    endif()


elseif(WINDOWS)
    if(NOT _AX_USE_PREBUILT)
        ax_sync_target_dlls(${APP_NAME})
    endif()
endif()

if (NOT DEFINED AX_PREBUILT_DIR)
    # todo: add "AX_ENABLE_SCRIPT_BINDING=0", now it breaks compilation for android
    target_compile_definitions(${_AX_CORE_LIB} PUBLIC
            AX_USE_PHYSICS=0
            AX_USE_3D_PHYSICS=0
            AX_USE_NAVMESH=0
            AX_ENABLE_CHIPMUNK_INTEGRATION=0
            AX_USE_JPEG=0
            AX_USE_TIFF=0
            AX_USE_WEBP=0)
endif()

if ((NOT APPLE) AND (NOT WINRT))
    ax_get_resource_path(APP_RES_DIR ${APP_NAME})
    if(WINDOWS) # for windows, we need to copy the contents of directories, not link
        ax_sync_target_res(${APP_NAME} LINK_TO ${APP_RES_DIR} FOLDERS ${content_folder})
    else()
        ax_sync_target_res(${APP_NAME} LINK_TO ${APP_RES_DIR} FOLDERS ${content_folder} SYM_LINK 1)
    endif()

    if((WINDOWS AND (NOT (CMAKE_GENERATOR MATCHES "Ninja"))))
        set_property(TARGET ${APP_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${content_folder}")
        if(NOT DEFINED BUILD_ENGINE_DONE)
            set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${APP_NAME})
        endif()
    endif()
elseif(WINRT)
    if(NOT DEFINED BUILD_ENGINE_DONE)
        set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${APP_NAME})
    endif()
endif()

if (_AX_USE_PREBUILT) # support windows and linux
    use_ax_compile_define(${APP_NAME})

    include(AXLinkHelpers)
    ax_link_cxx_prebuilt(${APP_NAME} ${_AX_ROOT} ${AX_PREBUILT_DIR})
endif()

ax_setup_app_props(${APP_NAME})
