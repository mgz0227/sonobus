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

find_package(AAX_SDK REQUIRED)

project(DemoMIDI_Synth_example VERSION 1.0.0)

string(TIMESTAMP CURRENT_YEAR "%Y")

aax_plugin(TARGET_NAME DemoMIDI_Synth_example
	OUTPUT_NAME DemoMIDI_Synth_example
	VERSION ${PROJECT_VERSION}
	COPYRIGHT_STRING "Copyright © 1991-${CURRENT_YEAR} Avid Technology, Inc."
	BUNDLE_IDENTIFIER "com.avid.plugin.demomidi_synth_example"

	RESOURCE_FILES
		Source/DemoSynthPages.xml
	
	ICON_FILE "${AAX_SDK_SOURCE_DIR}/Utilities/PlugIn.ico"
)

target_sources(DemoMIDI_Synth_example
	PRIVATE FILE_SET HEADERS FILES
		Source/DemoSynth_Defs.h
		Source/DemoSynth_Describe.h
		Source/DemoSynth_Parameters.h

	PRIVATE
		Source/DemoSynth_Describe.cpp
		Source/DemoSynth_Parameters.cpp
)

target_link_libraries(DemoMIDI_Synth_example PRIVATE
	AAX_SDK::AAX_CMonolithicParameters
	AAX_SDK::Example::CSimpleSynth
)

# Define an alias in the AAX_SDK::Example namespace; this is only for example plugins
add_library(AAX_SDK::Example::DemoMIDI_Synth ALIAS DemoMIDI_Synth_example)
