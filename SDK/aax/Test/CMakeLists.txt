# Copyright 2025 Avid Technology, Inc.
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
cmake_minimum_required (VERSION 3.25 FATAL_ERROR)

project(AAX_SDK_UnitTest VERSION 1.0.0 LANGUAGES CXX)

# Find the AAX SDK package (should be available when built)
find_package(AAX_SDK REQUIRED)


# TODO: Should we use AAX_SDK_SOURCE_DIR instead of defining a new variable?
set (AAX_SDK_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)

# Interface library for mock headers used by unit tests
add_library(AAX_SDK_Tests_Mocks INTERFACE)
target_include_directories(AAX_SDK_Tests_Mocks INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Configure third-party client build system
if(NOT IS_AVID_BUILD)
    message(STATUS "Configuring for third-party client build system")
    
    # Find GTest package (provided by Conan)
    find_package(GTest REQUIRED)
    
    # Define avid_add_gtest_unittest function for third-party builds
    # The AAX SDK targets are already available from find_package(AAX_SDK)
    function(avid_add_gtest_unittest target_name)
        add_executable(${target_name})
        target_link_libraries(${target_name} PRIVATE 
            GTest::gtest 
            GTest::gtest_main 
            GTest::gmock
            AAX_SDK::AAXLibrary
            AAX_SDK_Tests_Mocks
        )
        add_test(NAME ${target_name} COMMAND ${target_name})
        
        # Disable MSVC warning C4068 for unknown pragma
        # Quiets "unknown pragma 'mark'" warnings
        if (MSVC)
            target_compile_options(${target_name} PRIVATE /wd4068)
        endif()
        
        # Set standard properties for unit test targets
        set_target_properties(${target_name} PROPERTIES
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED ON
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
        )
    endfunction()
endif()



######################################################
# AAX_CheckedResult_UnitTest

avid_add_gtest_unittest(AAX_CheckedResult_UnitTest)

target_sources(AAX_CheckedResult_UnitTest PRIVATE
	# Unit test sources
	AAX_CheckedResult_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_CheckedResult_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_CheckedResult_UnitTest PRIVATE AAX_SDK::AAXLibrary)


######################################################
# AAX_CommonConversions_UnitTest

avid_add_gtest_unittest(AAX_CommonConversions_UnitTest)

target_sources(AAX_CommonConversions_UnitTest PRIVATE
    # Unit test sources
    AAX_CommonConversions_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_CommonConversions_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_CommonConversions_UnitTest PRIVATE AAX_SDK::AAXLibrary)


######################################################
# AAX_DataBuffer_UnitTest
# Note: Includes tests for multiple classes:
#   AAX_CValueDataBuffer
#   AAX_CDataBuffer
#   AAX_CArrayDataBuffer
#   AAX_CStringDataBuffer

avid_add_gtest_unittest(AAX_DataBuffer_UnitTest)

target_sources(AAX_DataBuffer_UnitTest PRIVATE
    # Unit test sources
    AAX_DataBuffer_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_DataBuffer_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_DataBuffer_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_Header_UnitTest

avid_add_gtest_unittest(AAX_Header_UnitTest)

target_sources(AAX_Header_UnitTest PRIVATE
    # Unit test sources
    AAX_Header_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_Header_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_Header_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_MIDIUtilities_UnitTest

avid_add_gtest_unittest(AAX_MIDIUtilities_UnitTest)

target_sources(AAX_MIDIUtilities_UnitTest PRIVATE
    # Unit test sources
    AAX_MIDIUtilities_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_MIDIUtilities_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_MIDIUtilities_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_SliderConversions_UnitTest

avid_add_gtest_unittest(AAX_SliderConversions_UnitTest)

target_sources(AAX_SliderConversions_UnitTest PRIVATE
    # Unit test sources
    AAX_SliderConversions_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_SliderConversions_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_SliderConversions_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_StringUtilities_UnitTest

avid_add_gtest_unittest(AAX_StringUtilities_UnitTest)

target_sources(AAX_StringUtilities_UnitTest PRIVATE
    # Unit test sources
    AAX_StringUtilities_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_StringUtilities_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_StringUtilities_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_TaskUtilities_UnitTest

avid_add_gtest_unittest(AAX_TaskUtilities_UnitTest)

target_sources(AAX_TaskUtilities_UnitTest PRIVATE
    # Unit test sources
    AAX_TaskUtilities_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_TaskUtilities_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_TaskUtilities_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_UtilsNative_UnitTest

avid_add_gtest_unittest(AAX_UtilsNative_UnitTest)

target_sources(AAX_UtilsNative_UnitTest PRIVATE
    # Unit test sources
    AAX_UtilsNative_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_UtilsNative_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_UtilsNative_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_CString_UnitTest

avid_add_gtest_unittest(AAX_CString_UnitTest)

target_sources(AAX_CString_UnitTest PRIVATE
    # Unit test sources
    AAX_CString_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_CString_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_CString_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_EndianSwap_UnitTest

avid_add_gtest_unittest(AAX_EndianSwap_UnitTest)

target_sources(AAX_EndianSwap_UnitTest PRIVATE
    # Unit test sources
    AAX_EndianSwap_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_EndianSwap_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_EndianSwap_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_PageTableUtilities_UnitTest

avid_add_gtest_unittest(AAX_PageTableUtilities_UnitTest)

target_sources(AAX_PageTableUtilities_UnitTest PRIVATE
    # Unit test sources
    AAX_PageTableUtilities_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_PageTableUtilities_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_PageTableUtilities_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_VenueUtilities_UnitTest

avid_add_gtest_unittest(AAX_VenueUtilities_UnitTest)

target_sources(AAX_VenueUtilities_UnitTest PRIVATE
    # Unit test sources
    AAX_VenueUtilities_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_VenueUtilities_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_VenueUtilities_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_EnvironmentUtilities_UnitTest

avid_add_gtest_unittest(AAX_EnvironmentUtilities_UnitTest)

target_sources(AAX_EnvironmentUtilities_UnitTest PRIVATE
    # Unit test sources
    AAX_EnvironmentUtilities_UnitTest.cpp
    # Environment-specific test translation units
    AAX_EnvironmentUtilities_UnitTest_TI.cpp
    AAX_EnvironmentUtilities_UnitTest_Windows.cpp
    AAX_EnvironmentUtilities_UnitTest_WindowsLegacy.cpp
    AAX_EnvironmentUtilities_UnitTest_Mac.cpp
    AAX_EnvironmentUtilities_UnitTest_MacLegacy.cpp
    AAX_EnvironmentUtilities_UnitTest_Linux.cpp
    AAX_EnvironmentUtilities_UnitTest_LinuxLegacy.cpp
    AAX_EnvironmentUtilities_UnitTest_Unknown.cpp
    AAX_EnvironmentUtilities_UnitTest_Doxygen.cpp
    AAX_EnvironmentUtilities_UnitTest_LegacyCPP.cpp
    # Common utilities
    AAX_EnvironmentUtilities_UnitTest_UndefineEnvironmentDefs.h
)

if (MSVC)
    target_compile_options(AAX_EnvironmentUtilities_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_EnvironmentUtilities_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_CAtomicQueue_UnitTest

avid_add_gtest_unittest(AAX_CAtomicQueue_UnitTest)

target_sources(AAX_CAtomicQueue_UnitTest PRIVATE
    # Unit test sources
    AAX_CAtomicQueue_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_CAtomicQueue_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_CAtomicQueue_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_CChunkDataParser_UnitTest

avid_add_gtest_unittest(AAX_CChunkDataParser_UnitTest)

target_sources(AAX_CChunkDataParser_UnitTest PRIVATE
    # Unit test sources
    AAX_CChunkDataParser_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_CChunkDataParser_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_CChunkDataParser_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_CMutex_UnitTest

avid_add_gtest_unittest(AAX_CMutex_UnitTest)

target_sources(AAX_CMutex_UnitTest PRIVATE
    # Unit test sources
    AAX_CMutex_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_CMutex_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_CMutex_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_CPieceWiseLinearTaperDelegate_UnitTest

avid_add_gtest_unittest(AAX_CPieceWiseLinearTaperDelegate_UnitTest)

target_sources(AAX_CPieceWiseLinearTaperDelegate_UnitTest PRIVATE
    # Unit test sources
    AAX_CPieceWiseLinearTaperDelegate_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_CPieceWiseLinearTaperDelegate_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_CPieceWiseLinearTaperDelegate_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_CTask_UnitTest

avid_add_gtest_unittest(AAX_CTask_UnitTest)

target_sources(AAX_CTask_UnitTest PRIVATE
    # Unit test sources
    AAX_CTask_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_CTask_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_CTask_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_CTaskAgent_UnitTest

avid_add_gtest_unittest(AAX_CTaskAgent_UnitTest)

target_sources(AAX_CTaskAgent_UnitTest PRIVATE
    # Unit test sources
    AAX_CTaskAgent_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_CTaskAgent_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_CTaskAgent_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_CEffectParameters_UnitTest

avid_add_gtest_unittest(AAX_CEffectParameters_UnitTest)

target_sources(AAX_CEffectParameters_UnitTest PRIVATE
    # Unit test sources
    AAX_CEffectParameters_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_CEffectParameters_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_CEffectParameters_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_CParameter_UnitTest

avid_add_gtest_unittest(AAX_CParameter_UnitTest)

target_sources(AAX_CParameter_UnitTest PRIVATE
    # Unit test sources
    AAX_CParameter_UnitTest.cpp
    AAX_CStatelessParameter_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_CParameter_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_CParameter_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_CLinearTaperDelegate_UnitTest

avid_add_gtest_unittest(AAX_CLinearTaperDelegate_UnitTest)

target_sources(AAX_CLinearTaperDelegate_UnitTest PRIVATE
    # Unit test sources
    AAX_CLinearTaperDelegate_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_CLinearTaperDelegate_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_CLinearTaperDelegate_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_CLogTaperDelegate_UnitTest

avid_add_gtest_unittest(AAX_CLogTaperDelegate_UnitTest)

target_sources(AAX_CLogTaperDelegate_UnitTest PRIVATE
    # Unit test sources
    AAX_CLogTaperDelegate_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_CLogTaperDelegate_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_CLogTaperDelegate_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_CNumberDisplayDelegate_UnitTest

avid_add_gtest_unittest(AAX_CNumberDisplayDelegate_UnitTest)

target_sources(AAX_CNumberDisplayDelegate_UnitTest PRIVATE
    # Unit test sources
    AAX_CNumberDisplayDelegate_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_CNumberDisplayDelegate_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_CNumberDisplayDelegate_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_CScaledNumberDisplayDelegate_UnitTest

avid_add_gtest_unittest(AAX_CScaledNumberDisplayDelegate_UnitTest)

target_sources(AAX_CScaledNumberDisplayDelegate_UnitTest PRIVATE
    # Unit test sources
    AAX_CScaledNumberDisplayDelegate_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_CScaledNumberDisplayDelegate_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_CScaledNumberDisplayDelegate_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_CBinaryTaperDelegate_UnitTest

avid_add_gtest_unittest(AAX_CBinaryTaperDelegate_UnitTest)

target_sources(AAX_CBinaryTaperDelegate_UnitTest PRIVATE
    # Unit test sources
    AAX_CBinaryTaperDelegate_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_CBinaryTaperDelegate_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_CBinaryTaperDelegate_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_CBinaryDisplayDelegate_UnitTest

avid_add_gtest_unittest(AAX_CBinaryDisplayDelegate_UnitTest)

target_sources(AAX_CBinaryDisplayDelegate_UnitTest PRIVATE
    # Unit test sources
    AAX_CBinaryDisplayDelegate_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_CBinaryDisplayDelegate_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_CBinaryDisplayDelegate_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_CStateTaperDelegate_UnitTest

avid_add_gtest_unittest(AAX_CStateTaperDelegate_UnitTest)

target_sources(AAX_CStateTaperDelegate_UnitTest PRIVATE
    # Unit test sources
    AAX_CStateTaperDelegate_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_CStateTaperDelegate_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_CStateTaperDelegate_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_CStateDisplayDelegate_UnitTest

avid_add_gtest_unittest(AAX_CStateDisplayDelegate_UnitTest)

target_sources(AAX_CStateDisplayDelegate_UnitTest PRIVATE
    # Unit test sources
    AAX_CStateDisplayDelegate_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_CStateDisplayDelegate_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_CStateDisplayDelegate_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_CStringDisplayDelegate_UnitTest

avid_add_gtest_unittest(AAX_CStringDisplayDelegate_UnitTest)

target_sources(AAX_CStringDisplayDelegate_UnitTest PRIVATE
    # Unit test sources
    AAX_CStringDisplayDelegate_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_CStringDisplayDelegate_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_CStringDisplayDelegate_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_CRangeTaperDelegate_UnitTest

avid_add_gtest_unittest(AAX_CRangeTaperDelegate_UnitTest)

target_sources(AAX_CRangeTaperDelegate_UnitTest PRIVATE
    # Unit test sources
    AAX_CRangeTaperDelegate_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_CRangeTaperDelegate_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_CRangeTaperDelegate_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_CUnitDisplayDelegateDecorator_UnitTest

avid_add_gtest_unittest(AAX_CUnitDisplayDelegateDecorator_UnitTest)

target_sources(AAX_CUnitDisplayDelegateDecorator_UnitTest PRIVATE
    # Unit test sources
    AAX_CUnitDisplayDelegateDecorator_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_CUnitDisplayDelegateDecorator_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_CUnitDisplayDelegateDecorator_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_CPacketDispatcher_UnitTest

avid_add_gtest_unittest(AAX_CPacketDispatcher_UnitTest)

target_sources(AAX_CPacketDispatcher_UnitTest PRIVATE
    # Unit test sources
    AAX_CPacketDispatcher_UnitTest.cpp
    # Mock class implementations
    AAX_MockController.cpp
    AAX_MockEffectParameters.cpp
    AAX_MockTaperDelegate.cpp
)



if (MSVC)
    target_compile_options(AAX_CPacketDispatcher_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_CPacketDispatcher_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_CParameterManager_UnitTest

avid_add_gtest_unittest(AAX_CParameterManager_UnitTest)

target_sources(AAX_CParameterManager_UnitTest PRIVATE
    # Unit test sources
    AAX_CParameterManager_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_CParameterManager_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_CParameterManager_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_CDecibelDisplayDelegateDecorator_UnitTest

avid_add_gtest_unittest(AAX_CDecibelDisplayDelegateDecorator_UnitTest)

target_sources(AAX_CDecibelDisplayDelegateDecorator_UnitTest PRIVATE
    # Unit test sources
    AAX_CDecibelDisplayDelegateDecorator_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_CDecibelDisplayDelegateDecorator_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_CDecibelDisplayDelegateDecorator_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_CPercentDisplayDelegateDecorator_UnitTest

avid_add_gtest_unittest(AAX_CPercentDisplayDelegateDecorator_UnitTest)

target_sources(AAX_CPercentDisplayDelegateDecorator_UnitTest PRIVATE
    # Unit test sources
    AAX_CPercentDisplayDelegateDecorator_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_CPercentDisplayDelegateDecorator_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_CPercentDisplayDelegateDecorator_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_CUnitPrefixDisplayDelegateDecorator_UnitTest

avid_add_gtest_unittest(AAX_CUnitPrefixDisplayDelegateDecorator_UnitTest)

target_sources(AAX_CUnitPrefixDisplayDelegateDecorator_UnitTest PRIVATE
    # Unit test sources
    AAX_CUnitPrefixDisplayDelegateDecorator_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_CUnitPrefixDisplayDelegateDecorator_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_CUnitPrefixDisplayDelegateDecorator_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_CScaledDisplayDelegateDecorator_UnitTest
######################################################

avid_add_gtest_unittest(AAX_CScaledDisplayDelegateDecorator_UnitTest)

target_sources(AAX_CScaledDisplayDelegateDecorator_UnitTest PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/AAX_CScaledDisplayDelegateDecorator_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_CScaledDisplayDelegateDecorator_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_CScaledDisplayDelegateDecorator_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_DisplayDelegateDecorator_Wrapping_UnitTest

avid_add_gtest_unittest(AAX_DisplayDelegateDecorator_Wrapping_UnitTest)

target_sources(AAX_DisplayDelegateDecorator_Wrapping_UnitTest PRIVATE
    # Unit test sources
    AAX_DisplayDelegateDecorator_Wrapping_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_DisplayDelegateDecorator_Wrapping_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_DisplayDelegateDecorator_Wrapping_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_Migrated_String_UnitTest

avid_add_gtest_unittest(AAX_Migrated_String_UnitTest)

target_sources(AAX_Migrated_String_UnitTest PRIVATE
    # Unit test sources
    AAX_Migrated_String_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_Migrated_String_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_Migrated_String_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_Migrated_TaperDelegate_UnitTest

avid_add_gtest_unittest(AAX_Migrated_TaperDelegate_UnitTest)

target_sources(AAX_Migrated_TaperDelegate_UnitTest PRIVATE
    # Unit test sources
    AAX_Migrated_TaperDelegate_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_Migrated_TaperDelegate_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_Migrated_TaperDelegate_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_Migrated_ParameterManager_UnitTest

avid_add_gtest_unittest(AAX_Migrated_ParameterManager_UnitTest)

target_sources(AAX_Migrated_ParameterManager_UnitTest PRIVATE
    # Unit test sources
    AAX_Migrated_ParameterManager_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_Migrated_ParameterManager_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_Migrated_ParameterManager_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_Migrated_TypedParameter_UnitTest

avid_add_gtest_unittest(AAX_Migrated_TypedParameter_UnitTest)

target_sources(AAX_Migrated_TypedParameter_UnitTest PRIVATE
    # Unit test sources
    AAX_Migrated_TypedParameter_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_Migrated_TypedParameter_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_Migrated_TypedParameter_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_Migrated_EffectParameters_UnitTest

avid_add_gtest_unittest(AAX_Migrated_EffectParameters_UnitTest)

target_sources(AAX_Migrated_EffectParameters_UnitTest PRIVATE
    # Unit test sources
    AAX_Migrated_EffectParameters_UnitTest.cpp
    MigratedUnitTestAutomationDelegate.cpp
    MigratedUnitTestAutomationDelegate.h
)

if (MSVC)
    target_compile_options(AAX_Migrated_EffectParameters_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_Migrated_EffectParameters_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_Migrated_EffectParametersChunk_UnitTest

avid_add_gtest_unittest(AAX_Migrated_EffectParametersChunk_UnitTest)

target_sources(AAX_Migrated_EffectParametersChunk_UnitTest PRIVATE
    # Unit test sources
    AAX_Migrated_EffectParametersChunk_UnitTest.cpp
    MigratedUnitTestAutomationDelegate.cpp
    MigratedUnitTestAutomationDelegate.h
)

if (MSVC)
    target_compile_options(AAX_Migrated_EffectParametersChunk_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_Migrated_EffectParametersChunk_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_Migrated_ChunkDataParser_UnitTest

avid_add_gtest_unittest(AAX_Migrated_ChunkDataParser_UnitTest)

target_sources(AAX_Migrated_ChunkDataParser_UnitTest PRIVATE
    # Unit test sources
    AAX_Migrated_ChunkDataParser_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_Migrated_ChunkDataParser_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_Migrated_ChunkDataParser_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_Migrated_DisplayDelegate_UnitTest

avid_add_gtest_unittest(AAX_Migrated_DisplayDelegate_UnitTest)

target_sources(AAX_Migrated_DisplayDelegate_UnitTest PRIVATE
    # Unit test sources
    AAX_Migrated_DisplayDelegate_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_Migrated_DisplayDelegate_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_Migrated_DisplayDelegate_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_Migrated_PacketDispatcher_UnitTest

avid_add_gtest_unittest(AAX_Migrated_PacketDispatcher_UnitTest)

target_sources(AAX_Migrated_PacketDispatcher_UnitTest PRIVATE
    # Unit test sources
    AAX_Migrated_PacketDispatcher_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_Migrated_PacketDispatcher_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_Migrated_PacketDispatcher_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_Migrated_StringUtilities_UnitTest

avid_add_gtest_unittest(AAX_Migrated_StringUtilities_UnitTest)

target_sources(AAX_Migrated_StringUtilities_UnitTest PRIVATE
    # Unit test sources
    AAX_Migrated_StringUtilities_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_Migrated_StringUtilities_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_Migrated_StringUtilities_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_Migrated_DataConversion_UnitTest

avid_add_gtest_unittest(AAX_Migrated_DataConversion_UnitTest)

target_sources(AAX_Migrated_DataConversion_UnitTest PRIVATE
    # Unit test sources
    AAX_Migrated_DataConversion_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_Migrated_DataConversion_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_Migrated_DataConversion_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_Migrated_Size_UnitTest

avid_add_gtest_unittest(AAX_Migrated_Size_UnitTest)

target_sources(AAX_Migrated_Size_UnitTest PRIVATE
    # Unit test sources
    AAX_Migrated_Size_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_Migrated_Size_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_Migrated_Size_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_Migrated_RandomGen_UnitTest

avid_add_gtest_unittest(AAX_Migrated_RandomGen_UnitTest)

target_sources(AAX_Migrated_RandomGen_UnitTest PRIVATE
    # Unit test sources
    AAX_Migrated_RandomGen_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_Migrated_RandomGen_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_Migrated_RandomGen_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_Migrated_InterfaceSize_UnitTest

avid_add_gtest_unittest(AAX_Migrated_InterfaceSize_UnitTest)

target_sources(AAX_Migrated_InterfaceSize_UnitTest PRIVATE
    # Unit test sources
    AAX_Migrated_InterfaceSize_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_Migrated_InterfaceSize_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_Migrated_InterfaceSize_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_Migrated_AtomicQueue_UnitTest

avid_add_gtest_unittest(AAX_Migrated_AtomicQueue_UnitTest)

target_sources(AAX_Migrated_AtomicQueue_UnitTest PRIVATE
    # Unit test sources
    AAX_Migrated_AtomicQueue_UnitTest.cpp
    MigratedAAX_CTestThread.h
)

if (MSVC)
    target_compile_options(AAX_Migrated_AtomicQueue_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_Migrated_AtomicQueue_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_Migrated_AtomicOperations_UnitTest

avid_add_gtest_unittest(AAX_Migrated_AtomicOperations_UnitTest)

target_sources(AAX_Migrated_AtomicOperations_UnitTest PRIVATE
    # Unit test sources
    AAX_Migrated_AtomicOperations_UnitTest.cpp
    MigratedAAX_CTestThread.h
)

if (MSVC)
    target_compile_options(AAX_Migrated_AtomicOperations_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_Migrated_AtomicOperations_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_CEffectDirectData_UnitTest

avid_add_gtest_unittest(AAX_CEffectDirectData_UnitTest)

target_sources(AAX_CEffectDirectData_UnitTest PRIVATE
    # Unit test sources
    AAX_CEffectDirectData_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_CEffectDirectData_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_CEffectDirectData_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_CEffectGUI_UnitTest

avid_add_gtest_unittest(AAX_CEffectGUI_UnitTest)

target_sources(AAX_CEffectGUI_UnitTest PRIVATE
    # Unit test sources
    AAX_CEffectGUI_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_CEffectGUI_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_CEffectGUI_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_CHostProcessor_UnitTest

avid_add_gtest_unittest(AAX_CHostProcessor_UnitTest)

target_sources(AAX_CHostProcessor_UnitTest PRIVATE
    # Unit test sources
    AAX_CHostProcessor_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_CHostProcessor_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_CHostProcessor_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_CHostServices_UnitTest

avid_add_gtest_unittest(AAX_CHostServices_UnitTest)

target_sources(AAX_CHostServices_UnitTest PRIVATE
    # Unit test sources
    AAX_CHostServices_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_CHostServices_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_CHostServices_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
# AAX_CSessionDocumentClient_UnitTest

avid_add_gtest_unittest(AAX_CSessionDocumentClient_UnitTest)

target_sources(AAX_CSessionDocumentClient_UnitTest PRIVATE
    # Unit test sources
    AAX_CSessionDocumentClient_UnitTest.cpp
)

if (MSVC)
    target_compile_options(AAX_CSessionDocumentClient_UnitTest PRIVATE /bigobj)
endif()

target_link_libraries(AAX_CSessionDocumentClient_UnitTest PRIVATE AAX_SDK::AAXLibrary)

######################################################
######################################################
######################################################

# Include any custom test subdirectories
file(GLOB subdirs RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/*/")
foreach(subdir ${subdirs})
    # Only include directories that:
    # 1. Are not hidden (don't start with .)
    # 2. Are not build directories
    # 3. Are not Scripts directory
    # 4. Have a CMakeLists.txt file
    if(NOT subdir MATCHES "^\\." AND 
       NOT subdir MATCHES "build" AND 
       NOT subdir STREQUAL "Scripts/" AND
       EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${subdir}/CMakeLists.txt")
        string(REGEX REPLACE "/$" "" subdir_name "${subdir}")
        add_subdirectory("${subdir_name}")
    endif()
endforeach()
