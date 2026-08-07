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

project(DemoMIDI_Sampler_example VERSION 1.0.0)

string(TIMESTAMP CURRENT_YEAR "%Y")

aax_plugin(TARGET_NAME DemoMIDI_Sampler_example
	OUTPUT_NAME DemoMIDI_Sampler_example
	VERSION ${PROJECT_VERSION}
	COPYRIGHT_STRING "Copyright © 1991-${CURRENT_YEAR} Avid Technology, Inc."
	BUNDLE_IDENTIFIER "com.avid.plugin.demomidi_sampler_example"

	RESOURCE_FILES
		Source/DemoSamplerPages.xml
	
	ICON_FILE "${AAX_SDK_SOURCE_DIR}/Utilities/PlugIn.ico"
)

target_sources(DemoMIDI_Sampler_example
	PRIVATE FILE_SET HEADERS FILES
		Source/DemoSampler_Defs.h
		Source/DemoSampler_Describe.h
		Source/DemoSampler_Parameters.h
		Source/DemoSampler_SampleBase.h

	PRIVATE
		Source/DemoSampler_Describe.cpp
		Source/DemoSampler_Parameters.cpp
		Source/DemoSampler_SampleBase.cpp
)

target_link_libraries(DemoMIDI_Sampler_example PRIVATE AAX_SDK::AAX_CMonolithicParameters)

# Define an alias in the AAX_SDK::Example namespace; this is only for example plugins
add_library(AAX_SDK::Example::DemoMIDI_Sampler ALIAS DemoMIDI_Sampler_example)
