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
 *	\file AAX_EnvironmentUtilities_UnitTest_UndefineEnvironmentDefs.h
 *
 *	\brief Undefines environment-specific preprocessor definitions and stores original values
 *
 *	This header stores original macro values and undefines them to create a clean 
 *	preprocessor environment for testing. Use AAX_EnvironmentUtilities_UnitTest_RedefineEnvironmentDefs.h
 *	to restore the original values.
 */ 
/*================================================================================================*/

#ifndef AAX_ENVIRONMENTUTILITIES_UNITTEST_UNDEFINEENVIRONMENTDEFS_H
#define AAX_ENVIRONMENTUTILITIES_UNITTEST_UNDEFINEENVIRONMENTDEFS_H

// Suppress expected warnings during environment testing
#if defined(__clang__)
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wmacro-redefined"
	#pragma clang diagnostic ignored "-W#warnings"
#elif defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable: 4005) // macro redefinition
	#pragma warning(disable: 4005) // #warning directive (if applicable)
#elif defined(__GNUC__)
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wmacro-redefined"
	#pragma GCC diagnostic ignored "-Wcpp"
#endif

// Store original values before undefining them

// TI DSP compiler definitions
#ifdef _TMS320C6X
#define AAX_TEST_ORIGINAL_TMS320C6X _TMS320C6X
#undef _TMS320C6X
#else
#define AAX_TEST_ORIGINAL_TMS320C6X_UNDEFINED
#endif

#ifdef TI_VERSION
#define AAX_TEST_ORIGINAL_TI_VERSION TI_VERSION
#undef TI_VERSION
#else
#define AAX_TEST_ORIGINAL_TI_VERSION_UNDEFINED
#endif

// C++ version definitions
#ifdef __cplusplus
#define AAX_TEST_ORIGINAL_CPLUSPLUS __cplusplus
#undef __cplusplus
#else
#define AAX_TEST_ORIGINAL_CPLUSPLUS_UNDEFINED
#endif

#ifdef _MSVC_LANG
#define AAX_TEST_ORIGINAL_MSVC_LANG _MSVC_LANG
#undef _MSVC_LANG
#else
#define AAX_TEST_ORIGINAL_MSVC_LANG_UNDEFINED
#endif

#ifdef AAX_CPP11_SUPPORT
#define AAX_TEST_ORIGINAL_CPP11_SUPPORT AAX_CPP11_SUPPORT
#undef AAX_CPP11_SUPPORT
#else
#define AAX_TEST_ORIGINAL_CPP11_SUPPORT_UNDEFINED
#endif

// Windows platform definitions
#ifdef _WIN32
#define AAX_TEST_ORIGINAL_WIN32 _WIN32
#undef _WIN32
#else
#define AAX_TEST_ORIGINAL_WIN32_UNDEFINED
#endif

#ifdef _WIN64
#define AAX_TEST_ORIGINAL_WIN64 _WIN64
#undef _WIN64
#else
#define AAX_TEST_ORIGINAL_WIN64_UNDEFINED
#endif

#ifdef WINDOWS_VERSION
#define AAX_TEST_ORIGINAL_WINDOWS_VERSION WINDOWS_VERSION
#undef WINDOWS_VERSION
#else
#define AAX_TEST_ORIGINAL_WINDOWS_VERSION_UNDEFINED
#endif

// Mac platform definitions
#ifdef __APPLE__
#define AAX_TEST_ORIGINAL_APPLE __APPLE__
#undef __APPLE__
#else
#define AAX_TEST_ORIGINAL_APPLE_UNDEFINED
#endif

#ifdef __MACH__
#define AAX_TEST_ORIGINAL_MACH __MACH__
#undef __MACH__
#else
#define AAX_TEST_ORIGINAL_MACH_UNDEFINED
#endif

#ifdef TARGET_OS_MAC
#define AAX_TEST_ORIGINAL_TARGET_OS_MAC TARGET_OS_MAC
#undef TARGET_OS_MAC
#else
#define AAX_TEST_ORIGINAL_TARGET_OS_MAC_UNDEFINED
#endif

#ifdef MAC_VERSION
#define AAX_TEST_ORIGINAL_MAC_VERSION MAC_VERSION
#undef MAC_VERSION
#else
#define AAX_TEST_ORIGINAL_MAC_VERSION_UNDEFINED
#endif

// Linux platform definitions
#ifdef __linux__
#define AAX_TEST_ORIGINAL_LINUX __linux__
#undef __linux__
#else
#define AAX_TEST_ORIGINAL_LINUX_UNDEFINED
#endif

#ifdef __gnu_linux__
#define AAX_TEST_ORIGINAL_GNU_LINUX __gnu_linux__
#undef __gnu_linux__
#else
#define AAX_TEST_ORIGINAL_GNU_LINUX_UNDEFINED
#endif

#ifdef LINUX_VERSION
#define AAX_TEST_ORIGINAL_LINUX_VERSION LINUX_VERSION
#undef LINUX_VERSION
#else
#define AAX_TEST_ORIGINAL_LINUX_VERSION_UNDEFINED
#endif

// Doxygen preprocessor flag
#ifdef DOXYGEN_PREPROCESSOR
#define AAX_TEST_ORIGINAL_DOXYGEN_PREPROCESSOR DOXYGEN_PREPROCESSOR
#undef DOXYGEN_PREPROCESSOR
#else
#define AAX_TEST_ORIGINAL_DOXYGEN_PREPROCESSOR_UNDEFINED
#endif

// Glass/internal build flags
#ifdef GLASS_VERSION
#define AAX_TEST_ORIGINAL_GLASS_VERSION GLASS_VERSION
#undef GLASS_VERSION
#else
#define AAX_TEST_ORIGINAL_GLASS_VERSION_UNDEFINED
#endif

#ifdef AAX_ALLOW_UNKNOWN_ENVIRONMENT
#define AAX_TEST_ORIGINAL_ALLOW_UNKNOWN_ENVIRONMENT AAX_ALLOW_UNKNOWN_ENVIRONMENT
#undef AAX_ALLOW_UNKNOWN_ENVIRONMENT
#else
#define AAX_TEST_ORIGINAL_ALLOW_UNKNOWN_ENVIRONMENT_UNDEFINED
#endif

#endif // AAX_ENVIRONMENTUTILITIES_UNITTEST_UNDEFINEENVIRONMENTDEFS_H
