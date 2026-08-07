# Copyright 2024-2025 Avid Technology, Inc.
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

cmake_minimum_required(VERSION 3.10)

# Project name
project(ExamplePlugIns)

# Collect all subdirectories
file(GLOB SUBDIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)

foreach(subdir ${SUBDIRS})
	# Skip "Common" directory - it's added separately as a utility library in the main CMakeLists.txt
	if(subdir STREQUAL "Common")
		continue()
	endif()
	
	if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${subdir} AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${subdir}/CMakeLists.txt)
		message(STATUS "Adding example plugin project: ${subdir}")
		add_subdirectory(${subdir})
	endif()
endforeach()
