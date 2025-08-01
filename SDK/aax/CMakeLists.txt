# Copyright 2023-2025 Avid Technology, Inc.
# 
# This file is part of the Avid AAX SDK.
# 
# The AAX SDK is subject to commercial or open-source licensing.
# 
# By using the AAX SDK, you agree to the terms of both the Avid AAX SDK License
# Agreement and Avid Privacy Policy.
# 
# AAX SDK License: https://developer.avid.com/aax
# Privacy Policy: https://www.avid.com/legal/privacy-policy-statement
# 
# Or: You may also use this code under the terms of the GPL v3 (see
# www.gnu.org/licenses).
# 
# THE AAX SDK IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
# EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
# DISCLAIMED.
cmake_minimum_required(VERSION 3.13)

project(AAX_SDK VERSION 2.9.0)

# Options
option(AAX_BUILD_EXAMPLES "Build example plugins" ON)
option(AAX_BUILD_PTSL_EXAMPLES "Build example plugins that use PTSL" OFF)
option(AAX_BUILD_LIBRARY "Build AAXLibrary" ON)
option(AAX_BUILD_TI_INTERFACE "Build TI interface" ON)
option(AAX_BUILD_JUCE_GUI_EXTENSION "Build Juce GUI extension - please make sure to install a compatible Juce version" OFF)

# Optional: To define a custom output directory for AAX plugins, set -DAAX_PLUGIN_OUTPUT_DIRECTORY=<path>
#
# Examples to send build output to the build system's AAX directory:
#   macOS:   -DAAX_PLUGIN_OUTPUT_DIRECTORY="/Library/Application Support/Avid/Audio/Plug-Ins"
#   Windows: -DAAX_PLUGIN_OUTPUT_DIRECTORY="C:/Program Files/Common Files/Avid/Audio/Plug-Ins"
option(AAX_SDK_PLUGIN_OUTPUT_DIRECTORY "Output directory for AAX plugins" OFF)



# Configure installation path: we override the default installation path.
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX
            "${CMAKE_BINARY_DIR}/INSTALL"
            CACHE PATH "default install path" FORCE)
    set(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT OFF)
endif ()

set(_INSTALL_DESTINATION_ROOT "${PROJECT_NAME}-${PROJECT_VERSION}")
set(AAX_SDK_INSTALL_DESTINATIONS
	RUNTIME DESTINATION "bin"
	LIBRARY DESTINATION "lib"
	ARCHIVE DESTINATION "${_INSTALL_DESTINATION_ROOT}/$<CONFIG>/lib"
	FRAMEWORK DESTINATION "$<CONFIG>/Frameworks"
)
set(INCLUDE_INSTALL_DIR ${_INSTALL_DESTINATION_ROOT}/include)


#
# AAX_SDK interface library
#
add_library(AAX_SDK_Interface INTERFACE)
add_library(AAX_SDK::AAX_SDK_Interface ALIAS AAX_SDK_Interface)

target_include_directories(AAX_SDK_Interface
INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Interfaces>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Interfaces/ACF>
    $<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}/>
    $<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}/ACF>
)

target_compile_options(AAX_SDK_Interface INTERFACE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wno-multichar>
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wno-undef-prefix>)

install(TARGETS AAX_SDK_Interface EXPORT AAX_SDKTargets)

file(GLOB HEADERS Interfaces/*.h Interfaces/*.hpp)
install(FILES ${HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR})
file(GLOB ACF_HEADERS Interfaces/ACF/*.h )
install(FILES ${ACF_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/ACF)

#
# AAX_Exports
#
add_library(AAX_Export OBJECT)
add_library(AAX_SDK::AAX_Export ALIAS AAX_Export)

set_target_properties(AAX_Export PROPERTIES CXX_STANDARD 14)
target_sources(AAX_Export PRIVATE
    Interfaces/AAX_Exports.cpp
)

target_link_libraries(AAX_Export PUBLIC AAX_SDK_Interface)

target_compile_options(AAX_Export PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wno-undef-prefix>)

install(TARGETS AAX_Export EXPORT AAX_SDKTargets DESTINATION ${AAX_SDK_INSTALL_DESTINATIONS})


#
# TI Shell Interface
# TODO: Move to AAX_SDK/TI/CMakeLists.txt
#
if (AAX_BUILD_TI_INTERFACE)
    add_library(TI_Shell_Interface INTERFACE)
    add_library(AAX_SDK::TI_Shell_Interface ALIAS TI_Shell_Interface)
    target_include_directories(TI_Shell_Interface
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/TI>
        $<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}/TI>
    )

    install(TARGETS TI_Shell_Interface EXPORT AAX_SDKTargets)

    file(GLOB HEADERS TI/*.h TI/*.hpp)
    install(FILES ${HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/TI/)
endif()

#
# TI SignalProcessing
# TODO: Move to AAX_SDK/TI/CMakeLists.txt
#
if (AAX_BUILD_TI_INTERFACE)
    add_library(TI_SignalProcessing INTERFACE)
    add_library(AAX_SDK::TI_SignalProcessing ALIAS TI_SignalProcessing)

    target_include_directories(TI_SignalProcessing
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/TI/SignalProcessing/>
        $<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}/TI/SignalProcessing/>
    )

    target_link_libraries(TI_SignalProcessing INTERFACE AAX_SDK_Interface)

    install(TARGETS TI_SignalProcessing EXPORT AAX_SDKTargets)

    file(GLOB HEADERS TI/SignalProcessing/*.h TI/SignalProcessing/*.hpp)
    install(FILES ${HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/TI/SignalProcessing/)
endif()

#
# AAXLibrary
#
if (AAX_BUILD_LIBRARY)
    add_subdirectory(Libs/AAXLibrary)
endif()


include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/AAX_SDKConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/AAX_SDKConfig.cmake.in"
    "${PROJECT_BINARY_DIR}/AAX_SDKConfig.cmake"
    INSTALL_DESTINATION ${_INSTALL_DESTINATION_ROOT}
)
configure_file(${PROJECT_SOURCE_DIR}/cmake/AAX_SDKFunctions.cmake ${PROJECT_BINARY_DIR}/AAX_SDKFunctions.cmake COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/cmake/Info.plist.in ${PROJECT_BINARY_DIR}/Info.plist.in COPYONLY)

install(EXPORT AAX_SDKTargets NAMESPACE AAX_SDK:: DESTINATION ${_INSTALL_DESTINATION_ROOT})
install(FILES "${PROJECT_BINARY_DIR}/AAX_SDKConfigVersion.cmake"
              "${PROJECT_BINARY_DIR}/AAX_SDKConfig.cmake"
              "${PROJECT_BINARY_DIR}/AAX_SDKFunctions.cmake"
              "${PROJECT_BINARY_DIR}/Info.plist.in"
        DESTINATION ${_INSTALL_DESTINATION_ROOT})

export(EXPORT AAX_SDKTargets NAMESPACE AAX_SDK:: FILE AAX_SDKTargets.cmake)

# Set AAX_SDK_DIR to the build directory in the CMake cache.
#
# This allows subsequent targets that use find_package(AAX_SDK) to
# find the package via the cache. Targets that depend on the AAX_SDK
# package without using the cache will need to set
# -DAAX_SDK_DIR=/path/to/aax-sdk-build
set(${PROJECT_NAME}_DIR ${PROJECT_BINARY_DIR} CACHE PATH "${PROJECT_NAME} build directory")

#
# GUI Extensions
#
# Note: Build options are set in this file and checked in Extensions/GUI/CMakeLists.txt:
#   AAX_BUILD_JUCE_GUI_EXTENSION
add_subdirectory(Extensions/GUI)


#
# Examples
#
if (AAX_BUILD_EXAMPLES)
    add_subdirectory(ExamplePlugIns)
endif()

