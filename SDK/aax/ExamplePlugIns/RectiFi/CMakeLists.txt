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

find_package(AAX_SDK REQUIRED)


project(RectiFi_example VERSION 1.0.0)

aax_plugin(TARGET_NAME RectiFi_example
	OUTPUT_NAME RectiFi_example
	VERSION ${PROJECT_VERSION}
	COPYRIGHT_STRING "Copyright © 1991-${CURRENT_YEAR} Avid Technology, Inc."
	BUNDLE_IDENTIFIER "com.AVID.plugin.RectiFi_example"

	RESOURCE_FILES
		RectiFi.xml
	
	FACTORY_PRESETS "Factory Presets"
	ICON_FILE "${AAX_SDK_SOURCE_DIR}/Utilities/PlugIn.ico"
)

target_sources(RectiFi_example
	PRIVATE FILE_SET HEADERS FILES
		RectiFi_Alg.h
		RectiFi_Defs.h
		RectiFi_Describe.h
		RectiFi_EffectParameters.h
		RectiFi_UnitDisplayDecorator.h

	PRIVATE
		RectiFi_AlgProc.cpp
		RectiFi_Describe.cpp
		RectiFi_EffectParameters.cpp
)


# Define an alias in the AAX_SDK namespace; this is only for example plugins
add_library(AAX_SDK::RectiFi_example ALIAS RectiFi_example)
