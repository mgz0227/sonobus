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
 *	\file AAX_EnvironmentUtilities_UnitTest_Doxygen.cpp
 *
 *	\brief Unit tests for AAX_EnvironmentUtilities.h Doxygen environment detection
 *
 */ 
/*================================================================================================*/

// Create a clean preprocessor environment for testing
#include "AAX_EnvironmentUtilities_UnitTest_UndefineEnvironmentDefs.h"

// Mock Doxygen environment definitions
#define DOXYGEN_PREPROCESSOR 1

// If the compiler does not allow redefining these macros, skip the test
#if (defined(DOXYGEN_PREPROCESSOR) && (DOXYGEN_PREPROCESSOR == 1))

#include "AAX_EnvironmentUtilities.h"

//--------------------------------------------------------------------
#pragma mark - Doxygen Environment Preprocessor Tests

// TI_VERSION should be defined as 0 for Doxygen
#ifndef TI_VERSION
#error "TI_VERSION should be defined for Doxygen"
#endif

#if TI_VERSION != 0
#error "TI_VERSION should be 0 for Doxygen"
#endif

// AAX_CPP11_SUPPORT should be defined as 1 for Doxygen
#ifndef AAX_CPP11_SUPPORT
#error "AAX_CPP11_SUPPORT should be defined for Doxygen"
#endif

#if AAX_CPP11_SUPPORT != 1
#error "AAX_CPP11_SUPPORT should be 1 for Doxygen"
#endif

// Platform versions should not be defined in Doxygen environment
#ifdef WINDOWS_VERSION
#error "WINDOWS_VERSION should not be defined in Doxygen environment"
#endif

#ifdef MAC_VERSION
#error "MAC_VERSION should not be defined in Doxygen environment"
#endif

#ifdef LINUX_VERSION
#error "LINUX_VERSION should not be defined in Doxygen environment"
#endif

// Compilation success indicates all tests passed

#endif // Mock environment check

// Restore original environment definitions
#include "AAX_EnvironmentUtilities_UnitTest_RedefineEnvironmentDefs.h"
