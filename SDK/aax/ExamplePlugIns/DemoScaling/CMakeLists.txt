# Copyright 2026 Avid Technology, Inc.
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

project(DemoScaling_example VERSION 1.0.0)

string(TIMESTAMP CURRENT_YEAR "%Y")

aax_plugin(TARGET_NAME ${PROJECT_NAME}
	OUTPUT_NAME ${PROJECT_NAME}
	VERSION ${PROJECT_VERSION}
	COPYRIGHT_STRING "Copyright 2026-${CURRENT_YEAR} Avid Technology, Inc."
	BUNDLE_IDENTIFIER "com.avid.plugin.demoscaling_example")

if(WIN32)
	SET(GUI_HEADER_FILE DemoScaling_GUI_Win32.h)
	SET(GUI_SOURCE_FILE DemoScaling_GUI_Win32.cpp)
else()
	SET(GUI_HEADER_FILE DemoScaling_GUI_Generic.h)
	SET(GUI_SOURCE_FILE DemoScaling_GUI_Generic.cpp)
endif()

target_sources(${PROJECT_NAME}
	PRIVATE FILE_SET HEADERS FILES
		DemoScaling_Alg.h
		DemoScaling_Describe.h
		DemoScaling_Defs.h
		DemoScaling_Parameters.h
		DemoScaling_GUI.h
		${GUI_HEADER_FILE}
	PRIVATE
		DemoScaling_AlgProc.cpp
		DemoScaling_Describe.cpp
		DemoScaling_Parameters.cpp
		${GUI_SOURCE_FILE}
)

# Define an alias in the AAX_SDK namespace; this is only for example plugins
add_library(AAX_SDK::DemoScaling_example ALIAS ${PROJECT_NAME})
