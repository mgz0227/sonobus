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
cmake_minimum_required(VERSION 3.13)

#
# CSimpleDelayLine - Delay line processing class
#
add_library(CSimpleDelayLine STATIC)
add_library(AAX_SDK::Example::CSimpleDelayLine ALIAS CSimpleDelayLine)

target_sources(CSimpleDelayLine
	PRIVATE
		CSimpleDelayLine.cpp
	PUBLIC FILE_SET HEADERS FILES
		CSimpleDelayLine.h
)

target_include_directories(CSimpleDelayLine
	PUBLIC
		.
)

target_link_libraries(CSimpleDelayLine PUBLIC AAX_SDK_Interface)

set_property(TARGET CSimpleDelayLine PROPERTY CXX_STANDARD 17)

#
# CSimpleBiquad - Header-only biquad filter class
#
add_library(CSimpleBiquad INTERFACE)
add_library(AAX_SDK::Example::CSimpleBiquad ALIAS CSimpleBiquad)

target_sources(CSimpleBiquad
	PUBLIC FILE_SET HEADERS FILES
		CSimpleBiquad.h
)

target_include_directories(CSimpleBiquad
	INTERFACE
		.
)

target_link_libraries(CSimpleBiquad INTERFACE AAX_SDK_Interface)

set_property(TARGET CSimpleBiquad PROPERTY CXX_STANDARD 17)

#
# CSimpleSynth - Simple synthesizer processing class
#
add_library(CSimpleSynth STATIC)
add_library(AAX_SDK::Example::CSimpleSynth ALIAS CSimpleSynth)

target_sources(CSimpleSynth
	PRIVATE
		CSimpleSynth.cpp
		CSimpleTone.cpp
	PUBLIC FILE_SET HEADERS FILES
		CSimpleSynth.h
		CSimpleTone.h
)

target_include_directories(CSimpleSynth
	PUBLIC
		.
)

target_link_libraries(CSimpleSynth PUBLIC AAX_SDK_Interface)

set_property(TARGET CSimpleSynth PROPERTY CXX_STANDARD 17)
