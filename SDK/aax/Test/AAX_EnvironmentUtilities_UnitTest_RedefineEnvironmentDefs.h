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
 *	\file AAX_EnvironmentUtilities_UnitTest_RedefineEnvironmentDefs.h
 *
 *	\brief Restores original environment-specific preprocessor definitions
 *
 *	This header restores the original macro values that were stored and undefined by
 *	AAX_EnvironmentUtilities_UnitTest_UndefineEnvironmentDefs.h. Use this at the end
 *	of test translation units to restore the original environment.
 */ 
/*================================================================================================*/

#ifndef AAX_ENVIRONMENTUTILITIES_UNITTEST_REDEFINEENVIRONMENTDEFS_H
#define AAX_ENVIRONMENTUTILITIES_UNITTEST_REDEFINEENVIRONMENTDEFS_H

// Restore original values that were stored before undefining

// TI DSP compiler definitions
#ifdef AAX_TEST_ORIGINAL_TMS320C6X_UNDEFINED
// Was originally undefined, leave undefined
#undef AAX_TEST_ORIGINAL_TMS320C6X_UNDEFINED
#else
#define _TMS320C6X AAX_TEST_ORIGINAL_TMS320C6X
#undef AAX_TEST_ORIGINAL_TMS320C6X
#endif

#ifdef AAX_TEST_ORIGINAL_TI_VERSION_UNDEFINED
// Was originally undefined, leave undefined
#undef AAX_TEST_ORIGINAL_TI_VERSION_UNDEFINED
#else
#define TI_VERSION AAX_TEST_ORIGINAL_TI_VERSION
#undef AAX_TEST_ORIGINAL_TI_VERSION
#endif

// C++ version definitions
#ifdef AAX_TEST_ORIGINAL_CPLUSPLUS_UNDEFINED
// Was originally undefined, leave undefined
#undef AAX_TEST_ORIGINAL_CPLUSPLUS_UNDEFINED
#else
#define __cplusplus AAX_TEST_ORIGINAL_CPLUSPLUS
#undef AAX_TEST_ORIGINAL_CPLUSPLUS
#endif

#ifdef AAX_TEST_ORIGINAL_MSVC_LANG_UNDEFINED
// Was originally undefined, leave undefined
#undef AAX_TEST_ORIGINAL_MSVC_LANG_UNDEFINED
#else
#define _MSVC_LANG AAX_TEST_ORIGINAL_MSVC_LANG
#undef AAX_TEST_ORIGINAL_MSVC_LANG
#endif

#ifdef AAX_TEST_ORIGINAL_CPP11_SUPPORT_UNDEFINED
// Was originally undefined, leave undefined
#undef AAX_TEST_ORIGINAL_CPP11_SUPPORT_UNDEFINED
#else
#define AAX_CPP11_SUPPORT AAX_TEST_ORIGINAL_CPP11_SUPPORT
#undef AAX_TEST_ORIGINAL_CPP11_SUPPORT
#endif

// Windows platform definitions
#ifdef AAX_TEST_ORIGINAL_WIN32_UNDEFINED
// Was originally undefined, leave undefined
#undef AAX_TEST_ORIGINAL_WIN32_UNDEFINED
#else
#define _WIN32 AAX_TEST_ORIGINAL_WIN32
#undef AAX_TEST_ORIGINAL_WIN32
#endif

#ifdef AAX_TEST_ORIGINAL_WIN64_UNDEFINED
// Was originally undefined, leave undefined
#undef AAX_TEST_ORIGINAL_WIN64_UNDEFINED
#else
#define _WIN64 AAX_TEST_ORIGINAL_WIN64
#undef AAX_TEST_ORIGINAL_WIN64
#endif

#ifdef AAX_TEST_ORIGINAL_WINDOWS_VERSION_UNDEFINED
// Was originally undefined, leave undefined
#undef AAX_TEST_ORIGINAL_WINDOWS_VERSION_UNDEFINED
#else
#define WINDOWS_VERSION AAX_TEST_ORIGINAL_WINDOWS_VERSION
#undef AAX_TEST_ORIGINAL_WINDOWS_VERSION
#endif

// Mac platform definitions
#ifdef AAX_TEST_ORIGINAL_APPLE_UNDEFINED
// Was originally undefined, leave undefined
#undef AAX_TEST_ORIGINAL_APPLE_UNDEFINED
#else
#define __APPLE__ AAX_TEST_ORIGINAL_APPLE
#undef AAX_TEST_ORIGINAL_APPLE
#endif

#ifdef AAX_TEST_ORIGINAL_MACH_UNDEFINED
// Was originally undefined, leave undefined
#undef AAX_TEST_ORIGINAL_MACH_UNDEFINED
#else
#define __MACH__ AAX_TEST_ORIGINAL_MACH
#undef AAX_TEST_ORIGINAL_MACH
#endif

#ifdef AAX_TEST_ORIGINAL_TARGET_OS_MAC_UNDEFINED
// Was originally undefined, leave undefined
#undef AAX_TEST_ORIGINAL_TARGET_OS_MAC_UNDEFINED
#else
#define TARGET_OS_MAC AAX_TEST_ORIGINAL_TARGET_OS_MAC
#undef AAX_TEST_ORIGINAL_TARGET_OS_MAC
#endif

#ifdef AAX_TEST_ORIGINAL_MAC_VERSION_UNDEFINED
// Was originally undefined, leave undefined
#undef AAX_TEST_ORIGINAL_MAC_VERSION_UNDEFINED
#else
#define MAC_VERSION AAX_TEST_ORIGINAL_MAC_VERSION
#undef AAX_TEST_ORIGINAL_MAC_VERSION
#endif

// Linux platform definitions
#ifdef AAX_TEST_ORIGINAL_LINUX_UNDEFINED
// Was originally undefined, leave undefined
#undef AAX_TEST_ORIGINAL_LINUX_UNDEFINED
#else
#define __linux__ AAX_TEST_ORIGINAL_LINUX
#undef AAX_TEST_ORIGINAL_LINUX
#endif

#ifdef AAX_TEST_ORIGINAL_GNU_LINUX_UNDEFINED
// Was originally undefined, leave undefined
#undef AAX_TEST_ORIGINAL_GNU_LINUX_UNDEFINED
#else
#define __gnu_linux__ AAX_TEST_ORIGINAL_GNU_LINUX
#undef AAX_TEST_ORIGINAL_GNU_LINUX
#endif

#ifdef AAX_TEST_ORIGINAL_LINUX_VERSION_UNDEFINED
// Was originally undefined, leave undefined
#undef AAX_TEST_ORIGINAL_LINUX_VERSION_UNDEFINED
#else
#define LINUX_VERSION AAX_TEST_ORIGINAL_LINUX_VERSION
#undef AAX_TEST_ORIGINAL_LINUX_VERSION
#endif

// Doxygen preprocessor flag
#ifdef AAX_TEST_ORIGINAL_DOXYGEN_PREPROCESSOR_UNDEFINED
// Was originally undefined, leave undefined
#undef AAX_TEST_ORIGINAL_DOXYGEN_PREPROCESSOR_UNDEFINED
#else
#define DOXYGEN_PREPROCESSOR AAX_TEST_ORIGINAL_DOXYGEN_PREPROCESSOR
#undef AAX_TEST_ORIGINAL_DOXYGEN_PREPROCESSOR
#endif

// Glass/internal build flags
#ifdef AAX_TEST_ORIGINAL_GLASS_VERSION_UNDEFINED
// Was originally undefined, leave undefined
#undef AAX_TEST_ORIGINAL_GLASS_VERSION_UNDEFINED
#else
#define GLASS_VERSION AAX_TEST_ORIGINAL_GLASS_VERSION
#undef AAX_TEST_ORIGINAL_GLASS_VERSION
#endif

#ifdef AAX_TEST_ORIGINAL_ALLOW_UNKNOWN_ENVIRONMENT_UNDEFINED
// Was originally undefined, leave undefined
#undef AAX_TEST_ORIGINAL_ALLOW_UNKNOWN_ENVIRONMENT_UNDEFINED
#else
#define AAX_ALLOW_UNKNOWN_ENVIRONMENT AAX_TEST_ORIGINAL_ALLOW_UNKNOWN_ENVIRONMENT
#undef AAX_TEST_ORIGINAL_ALLOW_UNKNOWN_ENVIRONMENT
#endif

// Restore original warning settings
#if defined(__clang__)
	#pragma clang diagnostic pop
#elif defined(_MSC_VER)
	#pragma warning(pop)
#elif defined(__GNUC__)
	#pragma GCC diagnostic pop
#endif

#endif // AAX_ENVIRONMENTUTILITIES_UNITTEST_REDEFINEENVIRONMENTDEFS_H
