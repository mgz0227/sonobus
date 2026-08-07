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
 *	\file AAX_EnvironmentUtilities_UnitTest.cpp
 *
 *	\brief Master unit test file for AAX_EnvironmentUtilities environment detection
 *
 *	This file serves as the main test runner for AAX_EnvironmentUtilities.h.
 *	Individual environment tests are implemented in separate translation units
 *	to properly test preprocessor macro behavior in isolated environments.
 *
 *	Individual test files:
 *	- AAX_EnvironmentUtilities_UnitTest_TI.cpp - TI DSP environment
 *	- AAX_EnvironmentUtilities_UnitTest_Windows.cpp - Windows environment  
 *	- AAX_EnvironmentUtilities_UnitTest_Mac.cpp - Mac environment
 *	- AAX_EnvironmentUtilities_UnitTest_Linux.cpp - Linux environment
 *	- AAX_EnvironmentUtilities_UnitTest_Unknown.cpp - Unknown environment
 *	- AAX_EnvironmentUtilities_UnitTest_Doxygen.cpp - Doxygen environment
 *	- AAX_EnvironmentUtilities_UnitTest_LegacyCPP.cpp - Legacy C++ environment
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX_EnvironmentUtilities.h"

//--------------------------------------------------------------------
#pragma mark - Test fixture

class AAX_EnvironmentUtilities_Test : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

//--------------------------------------------------------------------
#pragma mark - Current Environment Tests

TEST_F(AAX_EnvironmentUtilities_Test, CurrentEnvironmentDetection)
{
    // Test the current compilation environment
    // This verifies that the header works correctly in the actual build environment
    
    // At least one platform should be detected or we should be in an allowed unknown environment
    bool platformDetected = false;
    
#ifdef WINDOWS_VERSION  
    if (WINDOWS_VERSION) {
        platformDetected = true;
        EXPECT_EQ(1, WINDOWS_VERSION);
    }
#endif

#ifdef MAC_VERSION
    if (MAC_VERSION) {
        platformDetected = true;
        EXPECT_EQ(1, MAC_VERSION);
    }
#endif

#ifdef LINUX_VERSION
    if (LINUX_VERSION) {
        platformDetected = true;
        EXPECT_EQ(1, LINUX_VERSION);
    }
#endif

#ifdef AAX_ALLOW_UNKNOWN_ENVIRONMENT
    // Unknown environment is explicitly allowed
    platformDetected = true;
#endif

    EXPECT_TRUE(platformDetected) << "No platform detected and unknown environment not allowed";
}

TEST_F(AAX_EnvironmentUtilities_Test, MacroValuesConsistent)
{
    // Test that macros have consistent values when defined
    
#ifdef TI_VERSION
    EXPECT_TRUE(TI_VERSION == 0 || TI_VERSION == 1);
#endif

#ifdef AAX_CPP11_SUPPORT
    EXPECT_TRUE(AAX_CPP11_SUPPORT == 0 || AAX_CPP11_SUPPORT == 1);
#endif

#ifdef WINDOWS_VERSION
    EXPECT_EQ(1, WINDOWS_VERSION);
#endif

#ifdef MAC_VERSION
    EXPECT_EQ(1, MAC_VERSION);
#endif

#ifdef LINUX_VERSION
    EXPECT_EQ(1, LINUX_VERSION);
#endif
}

TEST_F(AAX_EnvironmentUtilities_Test, PlatformExclusivity)
{
    // Test that only one platform is defined at a time
    int platformCount = 0;
    
#ifdef WINDOWS_VERSION
    if (WINDOWS_VERSION) platformCount++;
#endif

#ifdef MAC_VERSION  
    if (MAC_VERSION) platformCount++;
#endif

#ifdef LINUX_VERSION
    if (LINUX_VERSION) platformCount++;
#endif

    // Should have at most one platform defined
    EXPECT_LE(platformCount, 1) << "Multiple platforms detected";
}

//--------------------------------------------------------------------
#pragma mark - Integration Tests

TEST_F(AAX_EnvironmentUtilities_Test, HeaderInclusionWorks)
{
    // Basic compilation test - if we reach this point, the header was included successfully
    EXPECT_TRUE(true);
    
    // Test that we can use the macros in conditional compilation
#if defined(WINDOWS_VERSION) && WINDOWS_VERSION
    const char* platform = "Windows";
#elif defined(MAC_VERSION) && MAC_VERSION
    const char* platform = "Mac";
#elif defined(LINUX_VERSION) && LINUX_VERSION
    const char* platform = "Linux";
#else
    const char* platform = "Unknown";
#endif
    
    EXPECT_NE(nullptr, platform);
}

//--------------------------------------------------------------------
#pragma mark - Environment-Specific Tests

// Note: The comprehensive environment-specific tests are implemented in separate
// translation units to ensure proper isolation of preprocessor definitions:
//
// - AAX_EnvironmentUtilities_UnitTest_TI.cpp
// - AAX_EnvironmentUtilities_UnitTest_Windows.cpp  
// - AAX_EnvironmentUtilities_UnitTest_Mac.cpp
// - AAX_EnvironmentUtilities_UnitTest_Linux.cpp
// - AAX_EnvironmentUtilities_UnitTest_Unknown.cpp
// - AAX_EnvironmentUtilities_UnitTest_Doxygen.cpp
// - AAX_EnvironmentUtilities_UnitTest_LegacyCPP.cpp
//
// This approach ensures that each environment's preprocessor definitions
// are tested in complete isolation without interference from the actual
// build environment.
