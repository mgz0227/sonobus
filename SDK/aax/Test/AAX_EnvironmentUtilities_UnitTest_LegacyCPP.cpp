/*================================================================================================*/
/*
 *
 *	Copyright 2025 Avid Technology, Inc.
 *	All rights reserved.
 *	
 *	This file is part of the Avid AAX SDK.
 *	
 *	The AAX SDK is subject to commercial or open-source licensing.
 *	
 *	By using the AAX SDK, you agree to the terms of both the Avid AAX SDK License
 *	Agreement and Avid Privacy Policy.
 *	
 *	AAX SDK License: https://developer.avid.com/aax
 *	Privacy Policy: https://www.avid.com/legal/privacy-policy-statement
 *	
 *	Or: You may also use this code under the terms of the GPL v3 (see
 *	www.gnu.org/licenses).
 *	
 *	THE AAX SDK IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
 *	EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
 *	DISCLAIMED.
 *
 */

/**  
 *	\file AAX_EnvironmentUtilities_UnitTest_LegacyCPP.cpp
 *
 *	\brief Unit tests for AAX_EnvironmentUtilities.h legacy C++ environment detection
 *
 */ 
/*================================================================================================*/

// Create a clean preprocessor environment for testing
#include "AAX_EnvironmentUtilities_UnitTest_UndefineEnvironmentDefs.h"

// Mock legacy C++ environment definitions (Windows with C++98)
#define _WIN32 1
#define __cplusplus 199711L  // C++98 support (no C++11)
#define _MSVC_LANG 201103L   // MSVC C++11 but below C++14 threshold (201402L)

// If the compiler does not allow redefining these macros, skip the test
#if (defined(_WIN32) && (_WIN32 == 1)) && (defined(__cplusplus) && (__cplusplus == 199711L)) && (defined(_MSVC_LANG) && (_MSVC_LANG == 201103L))

#include "AAX_EnvironmentUtilities.h"

//--------------------------------------------------------------------
#pragma mark - Legacy C++ Environment Preprocessor Tests

// WINDOWS_VERSION should be defined as 1 when _WIN32 is defined
#ifndef WINDOWS_VERSION
#error "WINDOWS_VERSION should be defined when _WIN32 is defined"
#endif

#if WINDOWS_VERSION != 1
#error "WINDOWS_VERSION should be 1 when _WIN32 is defined"
#endif

// AAX_CPP11_SUPPORT should not be defined with C++98 and _MSVC_LANG < 201402
#ifdef AAX_CPP11_SUPPORT
#if AAX_CPP11_SUPPORT != 0
#error "AAX_CPP11_SUPPORT should be 0 with C++98 and _MSVC_LANG < 201402"
#endif
#endif

// TI_VERSION should not be defined in Windows environment
#ifdef TI_VERSION
#error "TI_VERSION should not be defined in Windows environment"
#endif

// Other platform versions should not be defined
#ifdef MAC_VERSION
#error "MAC_VERSION should not be defined in Windows environment"
#endif

#ifdef LINUX_VERSION
#error "LINUX_VERSION should not be defined in Windows environment"
#endif

// Compilation success indicates all tests passed

#endif // Mock environment check

// Restore original environment definitions
#include "AAX_EnvironmentUtilities_UnitTest_RedefineEnvironmentDefs.h"
