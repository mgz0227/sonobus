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
 *	\file AAX_EnvironmentUtilities_UnitTest_MacLegacy.cpp
 *
 *	\brief Preprocessor tests for AAX_EnvironmentUtilities.h Mac legacy environment
 *
 */ 
/*================================================================================================*/

// The MAC_VERSION macro includes a framework header in AAX_EnvironmentUtilities.h, so we cannot mock it on other platforms
#if (defined(__APPLE__))

// Create a clean preprocessor environment for testing
#include "AAX_EnvironmentUtilities_UnitTest_UndefineEnvironmentDefs.h"

// Mock Mac legacy environment definitions (no C++11 support)
#define __APPLE__ 1
#define __MACH__ 1
#define __cplusplus 199711L  // C++98 support only
#define TARGET_OS_MAC 1

// If the compiler does not allow redefining these macros, skip the test
#if (defined(__APPLE__) && (__APPLE__ == 1)) && (defined(__MACH__) && (__MACH__ == 1)) && (defined(__cplusplus) && (__cplusplus == 199711L)) && (defined(TARGET_OS_MAC) && (TARGET_OS_MAC == 1))

#include "AAX_EnvironmentUtilities.h"

//--------------------------------------------------------------------
#pragma mark - Mac Legacy Environment Preprocessor Tests

// MAC_VERSION should be defined as 1 when __APPLE__ and __MACH__ are defined
#ifndef MAC_VERSION
#error "MAC_VERSION should be defined when __APPLE__ and __MACH__ are defined"
#endif

#if MAC_VERSION != 1
#error "MAC_VERSION should be 1 when __APPLE__ and __MACH__ are defined"
#endif

// AAX_CPP11_SUPPORT should not be defined with C++98
#ifdef AAX_CPP11_SUPPORT
#if AAX_CPP11_SUPPORT != 0
// _MSVC_LANG cannot be undefined and is also used for AAX_CPP11_SUPPORT
#if (!(defined _MSVC_LANG) || (_MSVC_LANG < 201402))
#error "AAX_CPP11_SUPPORT should be 0 with C++98"
#endif
#endif
#endif

// TI_VERSION should not be defined in Mac environment
#ifdef TI_VERSION
#error "TI_VERSION should not be defined in Mac environment"
#endif

// Other platform versions should not be defined
#ifdef WINDOWS_VERSION
#error "WINDOWS_VERSION should not be defined in Mac environment"
#endif

#ifdef LINUX_VERSION
#error "LINUX_VERSION should not be defined in Mac environment"
#endif

// Compilation success indicates all tests passed

#endif // Mock environment check

// Restore original environment definitions
#include "AAX_EnvironmentUtilities_UnitTest_RedefineEnvironmentDefs.h"

#endif // __APPLE__
