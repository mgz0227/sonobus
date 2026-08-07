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
 *	\file AAX_EnvironmentUtilities_UnitTest_Unknown.cpp
 *
 *	\brief Unit tests for AAX_EnvironmentUtilities.h unknown environment detection
 *
 */ 
/*================================================================================================*/

// Create a clean preprocessor environment for testing
#include "AAX_EnvironmentUtilities_UnitTest_UndefineEnvironmentDefs.h"

// Mock unknown environment definitions (no platform-specific macros, no C++11)
#define __cplusplus 199711L  // C++98 support only

// Note: We explicitly define AAX_ALLOW_UNKNOWN_ENVIRONMENT to suppress the warning
// during testing, otherwise the compilation would fail due to the warning being
// treated as an error in many build configurations
#define AAX_ALLOW_UNKNOWN_ENVIRONMENT 1

// If the compiler does not allow redefining these macros, skip the test
#if (defined(__cplusplus) && (__cplusplus == 199711L))

#include "AAX_EnvironmentUtilities.h"

//--------------------------------------------------------------------
#pragma mark - Unknown Environment Preprocessor Tests

// No platform versions should be defined in unknown environment
#ifdef WINDOWS_VERSION
#error "WINDOWS_VERSION should not be defined in unknown environment"
#endif

#ifdef MAC_VERSION
#error "MAC_VERSION should not be defined in unknown environment"
#endif

#ifdef LINUX_VERSION
#error "LINUX_VERSION should not be defined in unknown environment"
#endif

// TI_VERSION should not be defined in unknown environment
#ifdef TI_VERSION
#error "TI_VERSION should not be defined in unknown environment"
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

// Compilation success indicates all tests passed

#endif // Mock environment check

// Restore original environment definitions
#include "AAX_EnvironmentUtilities_UnitTest_RedefineEnvironmentDefs.h"
