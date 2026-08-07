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

project(DemoGain_GUIExtensions_example VERSION 1.0.0)

string(TIMESTAMP CURRENT_YEAR "%Y")


# This example requires JUCE GUI extensions
if(AAX_BUILD_JUCE_GUI_EXTENSION)

aax_plugin(TARGET_NAME DemoGain_Juce_example
	OUTPUT_NAME DemoGain_Juce_example
	VERSION ${PROJECT_VERSION}
	COPYRIGHT_STRING "Copyright © 1991-${CURRENT_YEAR} Avid Technology, Inc."
	BUNDLE_IDENTIFIER "com.avid.plugin.demogain_guiextensions_example"

	RESOURCE_FILES
		Source/DemoGainPages.xml
		Source/GUI/DemoGain_Background.png
	
	ICON_FILE "${AAX_SDK_SOURCE_DIR}/Utilities/PlugIn.ico"
)

target_sources(DemoGain_Juce_example
	PRIVATE FILE_SET HEADERS FILES
		Source/DemoGain_Alg.h
		Source/DemoGain_Defs.h
		Source/DemoGain_Describe.h
		Source/DemoGain_Parameters.h
		Source/GUI/DemoGain_GUI.h
		Source/GUI/DemoGain_GUI_Juce.h
		Source/GUI/DemoGain_SliderJuce.h
		Source/GUI/DemoGain_ViewComponent.h

	PRIVATE
		Source/DemoGain_AlgProc.cpp
		Source/DemoGain_Describe.cpp
		Source/DemoGain_Parameters.cpp
		Source/GUI/DemoGain_GUI_Juce.cpp
		Source/GUI/DemoGain_SliderJuce.cpp
		Source/GUI/DemoGain_ViewComponent.cpp
)

# Add Source directory to include path for headers
target_include_directories(DemoGain_Juce_example PRIVATE Source Source/GUI)

# Define PLUGGUI=2 for Juce GUI
target_compile_definitions(DemoGain_Juce_example PRIVATE PLUGGUI=2)

# OBJCXX is required by AAX_SDK::AAXJuceGUIExtensions
if(APPLE)
	enable_language(OBJCXX)
endif()

# Link JUCE GUI extensions
target_link_libraries(DemoGain_Juce_example
	PRIVATE
		AAX_SDK::AAXJuceGUIExtensions
)

# Define an alias in the AAX_SDK::Example namespace; this is only for example plugins
add_library(AAX_SDK::Example::DemoGain_GUIExtensions ALIAS DemoGain_Juce_example)

else()
	message(STATUS "Skipping DemoGain_Juce_example - requires AAX_BUILD_JUCE_GUI_EXTENSION=ON")
endif()
