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
# AAX_MIDILogging - MIDI logging utility
#
add_library(AAX_MIDILogging STATIC)
add_library(AAX_SDK::AAX_MIDILogging ALIAS AAX_MIDILogging)

target_sources(AAX_MIDILogging
	PRIVATE
		AAX_MIDILogging.cpp
	PUBLIC FILE_SET HEADERS FILES
		AAX_MIDILogging.h
)

target_include_directories(AAX_MIDILogging
	PUBLIC
		.
)

target_link_libraries(AAX_MIDILogging PUBLIC AAX_SDK_Interface)

set_property(TARGET AAX_MIDILogging PROPERTY CXX_STANDARD 17)
