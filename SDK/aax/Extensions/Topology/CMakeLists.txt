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
# AAX_CMonolithicParameters - Monolithic parameters extension
#
add_library(AAX_CMonolithicParameters STATIC)
add_library(AAX_SDK::AAX_CMonolithicParameters ALIAS AAX_CMonolithicParameters)

target_sources(AAX_CMonolithicParameters
	PRIVATE
		AAX_CMonolithicParameters.cpp
	PUBLIC FILE_SET HEADERS FILES
		AAX_CMonolithicParameters.h
)

target_include_directories(AAX_CMonolithicParameters
	PUBLIC
		.
)

target_link_libraries(AAX_CMonolithicParameters PUBLIC AAX_SDK_Interface)

set_property(TARGET AAX_CMonolithicParameters PROPERTY CXX_STANDARD 17)
