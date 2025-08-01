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
cmake_minimum_required (VERSION 3.12)

if(APPLE)
project(AAXLibrary LANGUAGES CXX OBJCXX)
else()
project(AAXLibrary LANGUAGES CXX)
endif()


#
# AAXLibrary target values
#

set(AAXLibrary_SOURCES
    source/AAX_CACFUnknown.cpp
    source/AAX_CAutoreleasePool.Win.cpp
    $<$<PLATFORM_ID:Darwin>:source/AAX_CAutoreleasePool.OSX.mm>
    source/AAX_CChunkDataParser.cpp
    source/AAX_CEffectDirectData.cpp
    source/AAX_CEffectGUI.cpp
    source/AAX_CEffectParameters.cpp
    source/AAX_CHostProcessor.cpp
    source/AAX_CHostServices.cpp
#   source/AAX_CInstrumentParameters.cpp
    source/AAX_CMutex.cpp
    source/AAX_CommonConversions.cpp
    source/AAX_CPacketDispatcher.cpp
    source/AAX_CParameter.cpp
    source/AAX_CParameterManager.cpp
    source/AAX_CString.cpp
    source/AAX_CTask.cpp
    source/AAX_CTaskAgent.cpp
    source/AAX_CUIDs.cpp
    source/AAX_IEffectDirectData.cpp
    source/AAX_IEffectGUI.cpp
    source/AAX_IEffectParameters.cpp
    source/AAX_IHostProcessor.cpp
    source/AAX_ITaskAgent.cpp
    source/AAX_Init.cpp
    source/AAX_Properties.cpp
    source/AAX_SliderConversions.cpp
    source/AAX_VAutomationDelegate.cpp
    source/AAX_VCollection.cpp
    source/AAX_VComponentDescriptor.cpp
    source/AAX_VController.cpp
    source/AAX_VDescriptionHost.cpp
    source/AAX_VEffectDescriptor.cpp
    source/AAX_VFeatureInfo.cpp
    source/AAX_VHostProcessorDelegate.cpp
    source/AAX_VHostServices.cpp
    source/AAX_VHostTaskAgent.cpp
    source/AAX_VPageTable.cpp
    source/AAX_VPrivateDataAccess.cpp
    source/AAX_VPropertyMap.cpp
    source/AAX_VTask.cpp
    source/AAX_VTransport.cpp
    source/AAX_VViewContainer.cpp
    source/AAX_VDescriptionHost.cpp

    ../../Interfaces/ACF/CACFClassFactory.cpp
)

set(AAXLibrary_INCLUDE_DIRS
    include
)

set(AAXLibrary_COMPILE_OPTIONS
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wno-multichar>
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wno-undef-prefix>
)

set(AAXLibrary_LINK_LIBRARIES
    AAX_SDK_Interface
)

function(set_aaxlibrary_target_properties target)
    set_property(TARGET ${target} PROPERTY CXX_STANDARD 17)
    set_property(TARGET ${target} PROPERTY POSITION_INDEPENDENT_CODE ON)
endfunction()


# Target: AAXLibrary
#
# This target provides the AAXLibrary as a static library

add_library(${PROJECT_NAME} STATIC)
add_library(AAX_SDK::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_sources(${PROJECT_NAME}
PRIVATE
    ${AAXLibrary_SOURCES}
)

target_include_directories(${PROJECT_NAME}
PRIVATE
    ${AAXLibrary_INCLUDE_DIRS}
)

target_compile_options(${PROJECT_NAME}
PRIVATE
    ${AAXLibrary_COMPILE_OPTIONS}
)

target_link_libraries(${PROJECT_NAME}
PUBLIC
    ${AAXLibrary_LINK_LIBRARIES}
)

set_aaxlibrary_target_properties(${PROJECT_NAME})



#
# AAXLibrary install definition
#

install(TARGETS ${PROJECT_NAME}
    EXPORT AAX_SDKTargets
    DESTINATION ${AAX_SDK_INSTALL_DESTINATIONS}
)
