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
	enable_language(OBJCXX)
endif()

## Cocoa
# TODO: Create CMake target for Cocoa GUI extension

## Win32
# TODO: Create CMake target for Win32 GUI extension

## VSTGUI
# TODO: Create CMake target for VSTGUI extension

## Juce
if(AAX_BUILD_JUCE_GUI_EXTENSION)
	add_library(AAXJuceGUIExtensions STATIC)
	add_library(AAX_SDK::AAXJuceGUIExtensions ALIAS AAXJuceGUIExtensions)

	target_sources(AAXJuceGUIExtensions
	PRIVATE
		AAX_CEffectGUI_Juce.h
		AAX_CEffectGUI_Juce.cpp
		$<$<PLATFORM_ID:Darwin>:AAX_CEffectGUI_Juce.mm>
		AAX_JuceContentView_Cache.h
		AAX_JuceContentView_Cache.cpp
	)

	target_include_directories(AAXJuceGUIExtensions
	PUBLIC
		.
	)
	target_compile_options(AAXJuceGUIExtensions PRIVATE $<$<C_COMPILER_ID:GNU,Clang,AppleClang>:-Wno-multichar>)

	# Retrieve Juce version
	if (NOT TARGET juce::juce_core)
		find_package(JUCE CONFIG REQUIRED)
		if (NOT JUCE_VERSION)
			message(FATAL_ERROR "JUCE_VERSION is not set by the JUCE package")
		else()
			set(AAX_SDK_JUCE_VERSION ${JUCE_VERSION})
		endif()
	else()
		if (NOT AAX_SDK_JUCE_VERSION)
			message(FATAL_ERROR "Please define AAX_SDK_JUCE_VERSION in the format MAJOR.MINOR.PATCH (e.g. 7.0.1)")
		endif()
	endif()
	# Extract version parts
	string(REPLACE "." ";" VERSION_LIST ${AAX_SDK_JUCE_VERSION})
	list(GET VERSION_LIST 0 AAX_SDK_JUCE_MAJOR_VERSION)
	list(GET VERSION_LIST 1 AAX_SDK_JUCE_MINOR_VERSION)
	list(GET VERSION_LIST 2 AAX_SDK_JUCE_PATCH_VERSION)

	message(STATUS "JUCE version: ${AAX_SDK_JUCE_VERSION} ${AAX_SDK_JUCE_MAJOR_VERSION} ${AAX_SDK_JUCE_MINOR_VERSION} ${AAX_SDK_JUCE_PATCH_VERSION}")
	target_compile_definitions(AAXJuceGUIExtensions
		PUBLIC
		AAX_SDK_JUCE_VERSION=${AAX_SDK_JUCE_VERSION}
		AAX_SDK_JUCE_MAJOR_VERSION=${AAX_SDK_JUCE_MAJOR_VERSION}
		AAX_SDK_JUCE_MINOR_VERSION=${AAX_SDK_JUCE_MINOR_VERSION}
		AAX_SDK_JUCE_PATCH_VERSION=${AAX_SDK_JUCE_PATCH_VERSION}
	)



	target_link_libraries(AAXJuceGUIExtensions
	PUBLIC
		AAX_SDK_Interface
		juce::juce_core # exposed through public header juce.h
		juce::juce_gui_basics
	)

	set_property(TARGET AAXJuceGUIExtensions PROPERTY CXX_STANDARD 14)

	# Export target to build tree, so it may be used by external projects, but do not install it
	export(TARGETS AAXJuceGUIExtensions NAMESPACE AAX_SDK::
		APPEND FILE ${CMAKE_CURRENT_BINARY_DIR}/../../AAX_SDKTargets.cmake)
	# we export also JUCE targets, as they are required by AAXJuceGUIExtensions
	export(TARGETS juce_core juce_gui_basics juce_atomic_wrapper juce_graphics juce_data_structures juce_events
	NAMESPACE AAX_SDK::
	APPEND FILE ${CMAKE_CURRENT_BINARY_DIR}/../../AAX_SDKTargets.cmake)
	
endif()
