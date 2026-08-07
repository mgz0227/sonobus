# Copyright 2025 Avid Technology, Inc.
# 
# This file is part of the Avid Proprietary Audio SDK.
# 
# The Avid Proprietary Audio SDK is confidential and subject to a commercial license with Avid.
#
# THE AVID PROPRIETARY AUDIO SDK IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
# EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
# DISCLAIMED.
cmake_minimum_required(VERSION 3.13)

if(NOT AAX_BUILD_PTSL_EXAMPLES)
	message(STATUS "Skipping DemoGain_PTSL_example because AAX_BUILD_PTSL_EXAMPLES is OFF")
	return()
endif()

project(DemoGain_PTSL_example VERSION 1.0.0)

string(TIMESTAMP CURRENT_YEAR "%Y")

find_package(AAX_SDK REQUIRED)
if (NOT TARGET PTSLC_CPP::PTSLC_CPP)
	find_package(PTSLC_CPP REQUIRED)
endif()	

aax_plugin(TARGET_NAME DemoGain_PTSL_example
	OUTPUT_NAME DemoGain_PTSL_example
	VERSION ${PROJECT_VERSION}
	COPYRIGHT_STRING "Copyright 2025-${CURRENT_YEAR} Avid Technology, Inc."
	BUNDLE_IDENTIFIER "com.avid.plugin.DemoGain_PTSL_example")

target_sources(DemoGain_PTSL_example
	PRIVATE FILE_SET HEADERS FILES
		DemoGain_Alg.h
		DemoGain_Describe.h
		DemoGain_Parameters.h
		DemoGain_PTSLClient.h

	PRIVATE
		DemoGain_AlgProc.cpp
		DemoGain_Describe.cpp
		DemoGain_Parameters.cpp
		DemoGain_PTSLClient.cpp
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE
		PTSLC_CPP::PTSLC_CPP
		$<IF:$<TARGET_EXISTS:PTSLC_CPP::Dependencies>,PTSLC_CPP::Dependencies,> # For compatibility. The target is no longer provided
)

# Define an alias in the AAX_SDK namespace; this is only for example plugins
add_library(AAX_SDK::DemoGain_PTSL_example ALIAS DemoGain_PTSL_example)
