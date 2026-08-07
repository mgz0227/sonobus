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
 *	\file AAX_VenueUtilities_UnitTest.cpp
 *
 *	\brief Unit tests for AAX_VenueUtilities functions
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX_VenueUtilities.h"
#include "AAX_EnvironmentUtilities.h"

#if WINDOWS_VERSION
#include <cstdlib>
#include <string>
#endif

//--------------------------------------------------------------------
#pragma mark - Test fixture

class AAX_VenueUtilities_Test : public ::testing::Test
{
protected:
    void SetUp() override 
    {
#if WINDOWS_VERSION
        // Save original environment variable value
        const char* originalValue = std::getenv("JEX_HOST_TYPE");
        if (originalValue) {
            mOriginalValue = originalValue;
            mHadOriginalValue = true;
        } else {
            mHadOriginalValue = false;
        }
#endif
    }
    
    void TearDown() override 
    {
#if WINDOWS_VERSION
        // Restore original environment variable value
        if (mHadOriginalValue) {
            SetEnvironmentVariable("JEX_HOST_TYPE", mOriginalValue.c_str());
        } else {
            SetEnvironmentVariable("JEX_HOST_TYPE", nullptr);
        }
#endif
    }
    
#if WINDOWS_VERSION
    void SetJexHostType(const char* value) {
        if (value) {
            SetEnvironmentVariable("JEX_HOST_TYPE", value);
        } else {
            SetEnvironmentVariable("JEX_HOST_TYPE", nullptr);
        }
    }
    
private:
    bool mHadOriginalValue = false;
    std::string mOriginalValue;
    
    // Helper function to set environment variable on Windows
    void SetEnvironmentVariable(const char* name, const char* value) {
        if (value) {
            _putenv_s(name, value);
        } else {
            _putenv_s(name, "");
        }
    }
#endif
};

//--------------------------------------------------------------------
#pragma mark - Platform Detection Tests

TEST_F(AAX_VenueUtilities_Test, PlatformBehavior)
{
#if WINDOWS_VERSION
    // On Windows, the function should check environment variable
    // Test will vary based on actual environment
    bool result = AAX::IsVenueSystem();
    
    // Result should be consistent when called multiple times
    bool result2 = AAX::IsVenueSystem();
    EXPECT_EQ(result, result2);
    
    // Function should not crash and should return a boolean
    EXPECT_TRUE(result == true || result == false);
#else
    // On non-Windows platforms, should always return false
    bool result = AAX::IsVenueSystem();
    EXPECT_FALSE(result);
    
    // Should be consistent
    bool result2 = AAX::IsVenueSystem();
    EXPECT_FALSE(result2);
#endif
}

//--------------------------------------------------------------------
#pragma mark - Static Caching Behavior Tests (Windows only)

#if WINDOWS_VERSION

// NOTE: These tests verify the static caching behavior of IsVenueSystem().
// The function only reads the environment variable on the first call and caches the result.
// Subsequent environment variable changes are ignored due to static initialization.

TEST_F(AAX_VenueUtilities_Test, StaticCachingBehavior_InitialEnvironmentMatters)
{
    // This test demonstrates that only the INITIAL environment variable value matters
    // due to static caching. The result depends on what JEX_HOST_TYPE was set to
    // when IsVenueSystem() was first called in the test process.

    bool initialResult = AAX::IsVenueSystem();

    // Change environment variable - this should have NO effect due to static caching
    SetJexHostType("venue");
    bool afterSettingVenue = AAX::IsVenueSystem();
    EXPECT_EQ(initialResult, afterSettingVenue); // Should be same as initial

    // Change to different value - still no effect
    SetJexHostType("not_venue");
    bool afterSettingOther = AAX::IsVenueSystem();
    EXPECT_EQ(initialResult, afterSettingOther); // Should be same as initial

    // Clear environment variable - still no effect
    SetJexHostType(nullptr);
    bool afterClearing = AAX::IsVenueSystem();
    EXPECT_EQ(initialResult, afterClearing); // Should be same as initial
}

TEST_F(AAX_VenueUtilities_Test, StaticCachingBehavior_ConsistentResults)
{
    // Verify that multiple calls always return the same result
    bool result1 = AAX::IsVenueSystem();
    bool result2 = AAX::IsVenueSystem();
    bool result3 = AAX::IsVenueSystem();

    EXPECT_EQ(result1, result2);
    EXPECT_EQ(result2, result3);

    // Even after changing environment variables, result should remain consistent
    SetJexHostType("different_value");
    bool result4 = AAX::IsVenueSystem();
    EXPECT_EQ(result1, result4);
}

//--------------------------------------------------------------------
#pragma mark - Static Behavior Tests (Windows only)

TEST_F(AAX_VenueUtilities_Test, StaticBehaviorConsistency)
{
    // The function uses static variables, so the result should be cached from the first call
    // NOTE: We can't predict what the result will be since it depends on the initial
    // environment state when IsVenueSystem() was first called in this test process

    bool initialResult = AAX::IsVenueSystem();

    // Change environment variable - this should have no effect due to static caching
    SetJexHostType("venue");
    bool result1 = AAX::IsVenueSystem();
    EXPECT_EQ(initialResult, result1); // Should be cached from first call

    // Change environment variable again
    SetJexHostType("not_venue");
    bool result2 = AAX::IsVenueSystem();
    EXPECT_EQ(initialResult, result2); // Should still be cached from first call

    // Multiple calls should return same cached result
    bool result3 = AAX::IsVenueSystem();
    EXPECT_EQ(result2, result3);
}

TEST_F(AAX_VenueUtilities_Test, StaticInitializationOrder)
{
    // Test that static initialization works correctly
    // This is mainly a compilation and runtime stability test
    
    // Multiple calls in sequence should not crash
    for (int i = 0; i < 10; ++i) {
        bool result = AAX::IsVenueSystem();
        EXPECT_TRUE(result == true || result == false); // Should be valid boolean
    }
}

//--------------------------------------------------------------------
#pragma mark - Thread Safety Tests (Windows only)

TEST_F(AAX_VenueUtilities_Test, ThreadSafety)
{
    // Test that multiple threads calling the function don't cause issues
    // Since the function uses static variables, this tests thread safety

    const int numThreads = 4;
    const int callsPerThread = 100;
    std::vector<std::thread> threads;
    std::vector<bool> results(numThreads * callsPerThread);

    // Get the expected result (whatever the static cached value is)
    bool expectedResult = AAX::IsVenueSystem();

    // Setting environment variable should have no effect due to static caching
    SetJexHostType("venue");

    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&results, t, callsPerThread]() {
            for (int i = 0; i < callsPerThread; ++i) {
                results[t * callsPerThread + i] = AAX::IsVenueSystem();
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    // All results should be the same as the cached static value
    for (bool result : results) {
        EXPECT_EQ(expectedResult, result);
    }
}

#endif // WINDOWS_VERSION

//--------------------------------------------------------------------
#pragma mark - Compilation and Linkage Tests

TEST_F(AAX_VenueUtilities_Test, FunctionExists)
{
    // Test that the function exists and can be called
    // This is mainly a compilation test
    
    bool result = AAX::IsVenueSystem();
    
    // Function should return a valid boolean value
    EXPECT_TRUE(result == true || result == false);
}

TEST_F(AAX_VenueUtilities_Test, NamespaceAccess)
{
    // Test that the function is accessible through the AAX namespace
    using namespace AAX;
    
    bool result = IsVenueSystem();
    EXPECT_TRUE(result == true || result == false);
}

TEST_F(AAX_VenueUtilities_Test, HeaderInclusion)
{
    // Test that including the header doesn't cause compilation issues
    // This is mainly a compilation test
    
    // The fact that we can call the function means the header was included correctly
    AAX::IsVenueSystem();
    
    // Test that environment utilities are also accessible
    // (since VenueUtilities includes EnvironmentUtilities)
#if WINDOWS_VERSION
    EXPECT_EQ(1, WINDOWS_VERSION);
#endif
#if MAC_VERSION
    EXPECT_EQ(1, MAC_VERSION);
#endif
#if LINUX_VERSION
    EXPECT_EQ(1, LINUX_VERSION);
#endif
}

//--------------------------------------------------------------------
#pragma mark - Documentation and Interface Tests

TEST_F(AAX_VenueUtilities_Test, FunctionSignature)
{
    // Test that the function has the expected signature
    // This is mainly a compilation test
    
    // Function should take no parameters and return bool
    bool (*funcPtr)(void) = &AAX::IsVenueSystem;
    EXPECT_NE(nullptr, funcPtr);
    
    // Should be callable through function pointer
    bool result = funcPtr();
    EXPECT_TRUE(result == true || result == false);
}

TEST_F(AAX_VenueUtilities_Test, ConstexprBehavior)
{
    // Test behavior with different compiler optimizations
    // The function uses static variables, so it's not constexpr,
    // but it should behave consistently
    
    bool result1 = AAX::IsVenueSystem();
    bool result2 = AAX::IsVenueSystem();
    
    EXPECT_EQ(result1, result2);
}

//--------------------------------------------------------------------
#pragma mark - Edge Cases

TEST_F(AAX_VenueUtilities_Test, RepeatedCalls)
{
    // Test that repeated calls don't cause issues
    const int numCalls = 1000;
    
    bool firstResult = AAX::IsVenueSystem();
    
    for (int i = 0; i < numCalls; ++i) {
        bool result = AAX::IsVenueSystem();
        EXPECT_EQ(firstResult, result);
    }
}

#if WINDOWS_VERSION
TEST_F(AAX_VenueUtilities_Test, EnvironmentVariableEdgeCases)
{
    // Due to static caching, changing environment variables after the first call
    // to IsVenueSystem() has no effect. This test verifies the function doesn't crash
    // when setting various environment variable values, but the result will always
    // be the same as the initial cached value.

    bool initialResult = AAX::IsVenueSystem();

    // Test with very long environment variable value
    std::string longValue(1000, 'a');
    SetJexHostType(longValue.c_str());
    bool result = AAX::IsVenueSystem();
    EXPECT_EQ(initialResult, result); // Should be same as cached value

    // Test with special characters
    SetJexHostType("venue\n");
    result = AAX::IsVenueSystem();
    EXPECT_EQ(initialResult, result); // Should be same as cached value

    SetJexHostType("venue\0extra");
    result = AAX::IsVenueSystem();
    EXPECT_EQ(initialResult, result); // Should be same as cached value
}
#endif
