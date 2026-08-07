/*================================================================================================*/
/*
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
 *	\file AAX_UtilsNative_UnitTest.cpp
 *
 *	\brief Unit tests for AAX_UtilsNative.h utility functions
 *
 */ 
/*================================================================================================*/


#define _USE_MATH_DEFINES
#include "gtest/gtest.h"
#include "AAX_UtilsNative.h"
#include "AAX_CString.h"
#include <cmath>
#include <limits>

//--------------------------------------------------------------------
#pragma mark - Test fixture

class AAX_UtilsNative : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
    
    // Helper function to check floating point equality with tolerance
    bool IsNearlyEqual(double a, double b, double tolerance = 1e-10)
    {
        return std::abs(a - b) < tolerance;
    }
    
    bool IsNearlyEqual(float a, float b, float tolerance = 1e-6f)
    {
        return std::abs(a - b) < tolerance;
    }
    
    // Helper to create a mock AAX_IString for testing
    class MockAAXString : public AAX_IString
    {
    public:
        MockAAXString(const char* str) : mString(str ? str : "") {}
        const char* Get() const override { return mString.c_str(); }
        uint32_t Length() const override { return static_cast<uint32_t>(mString.length()); }
        uint32_t MaxLength() const override { return static_cast<uint32_t>(mString.capacity()); }
        void Set(const char* iString) override { mString = iString ? iString : ""; }
        AAX_IString& operator=(const AAX_IString& iOther) override { mString = iOther.Get(); return *this; }
        AAX_IString& operator=(const char* iString) override { mString = iString ? iString : ""; return *this; }

    private:
        std::string mString;
    };
};

//--------------------------------------------------------------------
#pragma mark - SafeLog Tests

TEST_F(AAX_UtilsNative, SafeLog)
{
    using namespace AAX;
    
    // Test normal positive values
    EXPECT_TRUE(IsNearlyEqual(0.0, SafeLog(1.0))); // log(1) = 0
    EXPECT_TRUE(IsNearlyEqual(1.0, SafeLog(M_E))); // log(e) = 1
    EXPECT_TRUE(IsNearlyEqual(std::log(2.0), SafeLog(2.0))); // log(2)
    EXPECT_TRUE(IsNearlyEqual(std::log(10.0), SafeLog(10.0))); // log(10)
    
    // Test very small positive values
    EXPECT_TRUE(IsNearlyEqual(std::log(1e-10), SafeLog(1e-10)));
    EXPECT_TRUE(IsNearlyEqual(std::log(std::numeric_limits<double>::min()), SafeLog(std::numeric_limits<double>::min())));
    
    // Test large values
    EXPECT_TRUE(IsNearlyEqual(std::log(1e10), SafeLog(1e10)));
    EXPECT_TRUE(IsNearlyEqual(std::log(std::numeric_limits<double>::max()), SafeLog(std::numeric_limits<double>::max())));
    
    // Test edge cases that should return 0.0
    EXPECT_DOUBLE_EQ(0.0, SafeLog(0.0)); // Zero should return 0.0
    EXPECT_DOUBLE_EQ(0.0, SafeLog(-1.0)); // Negative should return 0.0
    EXPECT_DOUBLE_EQ(0.0, SafeLog(-100.0)); // Large negative should return 0.0
    EXPECT_DOUBLE_EQ(0.0, SafeLog(-std::numeric_limits<double>::infinity())); // Negative infinity should return 0.0
    
    // Test very small negative values
    EXPECT_DOUBLE_EQ(0.0, SafeLog(-1e-10));
    EXPECT_DOUBLE_EQ(0.0, SafeLog(-std::numeric_limits<double>::min()));
}

//--------------------------------------------------------------------
#pragma mark - SafeLogf Tests

TEST_F(AAX_UtilsNative, SafeLogf)
{
    using namespace AAX;
    
    // Test normal positive values
    EXPECT_TRUE(IsNearlyEqual(0.0f, SafeLogf(1.0f))); // logf(1) = 0
    EXPECT_TRUE(IsNearlyEqual(1.0f, SafeLogf(static_cast<float>(M_E)))); // logf(e) = 1
    EXPECT_TRUE(IsNearlyEqual(std::logf(2.0f), SafeLogf(2.0f))); // logf(2)
    EXPECT_TRUE(IsNearlyEqual(std::logf(10.0f), SafeLogf(10.0f))); // logf(10)
    
    // Test very small positive values
    EXPECT_TRUE(IsNearlyEqual(std::logf(1e-6f), SafeLogf(1e-6f)));
    EXPECT_TRUE(IsNearlyEqual(std::logf(std::numeric_limits<float>::min()), SafeLogf(std::numeric_limits<float>::min())));
    
    // Test large values
    EXPECT_TRUE(IsNearlyEqual(std::logf(1e6f), SafeLogf(1e6f)));
    EXPECT_TRUE(IsNearlyEqual(std::logf(std::numeric_limits<float>::max()), SafeLogf(std::numeric_limits<float>::max())));
    
    // Test edge cases that should return 0.0f
    EXPECT_FLOAT_EQ(0.0f, SafeLogf(0.0f)); // Zero should return 0.0f
    EXPECT_FLOAT_EQ(0.0f, SafeLogf(-1.0f)); // Negative should return 0.0f
    EXPECT_FLOAT_EQ(0.0f, SafeLogf(-100.0f)); // Large negative should return 0.0f
    EXPECT_FLOAT_EQ(0.0f, SafeLogf(-std::numeric_limits<float>::infinity())); // Negative infinity should return 0.0f
    
    // Test very small negative values
    EXPECT_FLOAT_EQ(0.0f, SafeLogf(-1e-6f));
    EXPECT_FLOAT_EQ(0.0f, SafeLogf(-std::numeric_limits<float>::min()));
}

//--------------------------------------------------------------------
#pragma mark - IsParameterIDEqual Tests

TEST_F(AAX_UtilsNative, IsParameterIDEqual)
{
    using namespace AAX;
    
    // Test identical strings
    const char* param1 = "gain";
    const char* param2 = "gain";
    EXPECT_TRUE(IsParameterIDEqual(param1, param2));
    
    // Test different strings
    const char* param3 = "gain";
    const char* param4 = "volume";
    EXPECT_FALSE(IsParameterIDEqual(param3, param4));
    
    // Test case sensitivity
    const char* param5 = "Gain";
    const char* param6 = "gain";
    EXPECT_FALSE(IsParameterIDEqual(param5, param6));
    
    // Test empty strings
    const char* empty1 = "";
    const char* empty2 = "";
    EXPECT_TRUE(IsParameterIDEqual(empty1, empty2));
    
    // Test one empty, one non-empty
    const char* empty = "";
    const char* nonEmpty = "gain";
    EXPECT_FALSE(IsParameterIDEqual(empty, nonEmpty));
    EXPECT_FALSE(IsParameterIDEqual(nonEmpty, empty));
    
    // Test null pointers
    EXPECT_TRUE(IsParameterIDEqual(nullptr, nullptr));
    EXPECT_FALSE(IsParameterIDEqual(nullptr, param1));
    EXPECT_FALSE(IsParameterIDEqual(param1, nullptr));
    
    // Test strings that differ only at the end
    const char* param7 = "gain_master";
    const char* param8 = "gain_slave";
    EXPECT_FALSE(IsParameterIDEqual(param7, param8));
    
    // Test strings with maximum length
    char longParam1[kAAX_ParameterIdentifierMaxSize];
    char longParam2[kAAX_ParameterIdentifierMaxSize];
    memset(longParam1, 'a', kAAX_ParameterIdentifierMaxSize - 1);
    memset(longParam2, 'a', kAAX_ParameterIdentifierMaxSize - 1);
    longParam1[kAAX_ParameterIdentifierMaxSize - 1] = '\0';
    longParam2[kAAX_ParameterIdentifierMaxSize - 1] = '\0';
    EXPECT_TRUE(IsParameterIDEqual(longParam1, longParam2));
    
    // Test strings that differ at maximum length boundary
    longParam2[kAAX_ParameterIdentifierMaxSize - 2] = 'b';
    EXPECT_FALSE(IsParameterIDEqual(longParam1, longParam2));
}

//--------------------------------------------------------------------
#pragma mark - IsParameterIDReservedByAvid Tests

TEST_F(AAX_UtilsNative, IsParameterIDReservedByAvid)
{
    using namespace AAX;
    
    // Test reserved parameter IDs (starting with "avid.")
    EXPECT_TRUE(IsParameterIDReservedByAvid("avid.gain"));
    EXPECT_TRUE(IsParameterIDReservedByAvid("avid.volume"));
    EXPECT_TRUE(IsParameterIDReservedByAvid("avid."));
    EXPECT_TRUE(IsParameterIDReservedByAvid("avid.internal.parameter"));
    
    // Test non-reserved parameter IDs
    EXPECT_FALSE(IsParameterIDReservedByAvid("gain"));
    EXPECT_FALSE(IsParameterIDReservedByAvid("volume"));
    EXPECT_FALSE(IsParameterIDReservedByAvid("my.parameter"));
    EXPECT_FALSE(IsParameterIDReservedByAvid("company.gain"));
    
    // Test case sensitivity (should be case-sensitive)
    EXPECT_FALSE(IsParameterIDReservedByAvid("AVID.gain"));
    EXPECT_FALSE(IsParameterIDReservedByAvid("Avid.gain"));
    EXPECT_FALSE(IsParameterIDReservedByAvid("AVID.GAIN"));
    
    // Test partial matches that should not be reserved
    EXPECT_FALSE(IsParameterIDReservedByAvid("avid"));
    EXPECT_FALSE(IsParameterIDReservedByAvid("avi"));
    EXPECT_FALSE(IsParameterIDReservedByAvid("avidgain"));
    EXPECT_FALSE(IsParameterIDReservedByAvid("myavid.gain"));
    
    // Test empty string and null pointer
    EXPECT_FALSE(IsParameterIDReservedByAvid(""));
    EXPECT_FALSE(IsParameterIDReservedByAvid(nullptr));
    
    // Test strings that start with "avid." but have additional content
    EXPECT_TRUE(IsParameterIDReservedByAvid("avid.very.long.parameter.name"));
    EXPECT_TRUE(IsParameterIDReservedByAvid("avid.123"));
    EXPECT_TRUE(IsParameterIDReservedByAvid("avid._internal"));
}

//--------------------------------------------------------------------
#pragma mark - IsEffectIDEqual Tests

TEST_F(AAX_UtilsNative, IsEffectIDEqual)
{
    using namespace AAX;

    // Test identical effect IDs
    MockAAXString effect1("com.company.plugin");
    MockAAXString effect2("com.company.plugin");
    EXPECT_TRUE(IsEffectIDEqual(&effect1, &effect2));

    // Test different effect IDs
    MockAAXString effect3("com.company.plugin1");
    MockAAXString effect4("com.company.plugin2");
    EXPECT_FALSE(IsEffectIDEqual(&effect3, &effect4));

    // Test case sensitivity
    MockAAXString effect5("com.Company.Plugin");
    MockAAXString effect6("com.company.plugin");
    EXPECT_FALSE(IsEffectIDEqual(&effect5, &effect6));

    // Test empty effect IDs
    MockAAXString empty1("");
    MockAAXString empty2("");
    EXPECT_TRUE(IsEffectIDEqual(&empty1, &empty2));

    // Test one empty, one non-empty
    MockAAXString empty("");
    MockAAXString nonEmpty("com.company.plugin");
    EXPECT_FALSE(IsEffectIDEqual(&empty, &nonEmpty));
    EXPECT_FALSE(IsEffectIDEqual(&nonEmpty, &empty));

    // Test null pointers
    EXPECT_TRUE(IsEffectIDEqual(nullptr, nullptr));
    EXPECT_FALSE(IsEffectIDEqual(nullptr, &effect1));
    EXPECT_FALSE(IsEffectIDEqual(&effect1, nullptr));

    // Test long effect IDs
    MockAAXString longEffect1("com.very.long.company.name.with.many.segments.plugin.name.version.1.0");
    MockAAXString longEffect2("com.very.long.company.name.with.many.segments.plugin.name.version.1.0");
    EXPECT_TRUE(IsEffectIDEqual(&longEffect1, &longEffect2));

    // Test similar but different long effect IDs
    MockAAXString longEffect3("com.very.long.company.name.with.many.segments.plugin.name.version.1.1");
    EXPECT_FALSE(IsEffectIDEqual(&longEffect1, &longEffect3));

    // Test effect IDs with special characters
    MockAAXString special1("com.company-name.plugin_v1.0");
    MockAAXString special2("com.company-name.plugin_v1.0");
    EXPECT_TRUE(IsEffectIDEqual(&special1, &special2));

    MockAAXString special3("com.company-name.plugin_v1.1");
    EXPECT_FALSE(IsEffectIDEqual(&special1, &special3));
}

//--------------------------------------------------------------------
#pragma mark - IsAvidNotification Tests

TEST_F(AAX_UtilsNative, IsAvidNotification)
{
    using namespace AAX;

    // Test valid Avid notifications (starting with 'AX')
    AAX_CTypeID avidNotification1 = ('A' << 24) | ('X' << 16) | ('0' << 8) | '1'; // 'AX01'
    EXPECT_TRUE(IsAvidNotification(avidNotification1));

    AAX_CTypeID avidNotification2 = ('A' << 24) | ('X' << 16) | ('T' << 8) | 'S'; // 'AXTS'
    EXPECT_TRUE(IsAvidNotification(avidNotification2));

    AAX_CTypeID avidNotification3 = ('A' << 24) | ('X' << 16) | (0xFF << 8) | 0xFF; // 'AX' + any bytes
    EXPECT_TRUE(IsAvidNotification(avidNotification3));

    // Test special case 'ASPv'
    AAX_CTypeID aspvNotification = ('A' << 24) | ('S' << 16) | ('P' << 8) | 'v'; // 'ASPv'
    EXPECT_TRUE(IsAvidNotification(aspvNotification));

    // Test non-Avid notifications
    AAX_CTypeID nonAvidNotification1 = ('C' << 24) | ('O' << 16) | ('M' << 8) | 'P'; // 'COMP'
    EXPECT_FALSE(IsAvidNotification(nonAvidNotification1));

    AAX_CTypeID nonAvidNotification2 = ('P' << 24) | ('L' << 16) | ('U' << 8) | 'G'; // 'PLUG'
    EXPECT_FALSE(IsAvidNotification(nonAvidNotification2));

    AAX_CTypeID nonAvidNotification3 = ('U' << 24) | ('S' << 16) | ('E' << 8) | 'R'; // 'USER'
    EXPECT_FALSE(IsAvidNotification(nonAvidNotification3));

    // Test notifications that start with 'A' but not 'AX'
    AAX_CTypeID almostAvidNotification1 = ('A' << 24) | ('B' << 16) | ('C' << 8) | 'D'; // 'ABCD'
    EXPECT_FALSE(IsAvidNotification(almostAvidNotification1));

    AAX_CTypeID almostAvidNotification2 = ('A' << 24) | ('V' << 16) | ('I' << 8) | 'D'; // 'AVID'
    EXPECT_FALSE(IsAvidNotification(almostAvidNotification2));

    // Test notifications that have 'X' in second position but not 'A' in first
    AAX_CTypeID notAvidNotification = ('B' << 24) | ('X' << 16) | ('T' << 8) | 'S'; // 'BXTS'
    EXPECT_FALSE(IsAvidNotification(notAvidNotification));

    // Test edge cases
    AAX_CTypeID zeroNotification = 0;
    EXPECT_FALSE(IsAvidNotification(zeroNotification));

    AAX_CTypeID maxNotification = 0xFFFFFFFF;
    EXPECT_FALSE(IsAvidNotification(maxNotification));

    // Test boundary cases for 'AX' pattern
    AAX_CTypeID minAvidNotification = ('A' << 24) | ('X' << 16) | (0 << 8) | 0; // 'AX\0\0'
    EXPECT_TRUE(IsAvidNotification(minAvidNotification));

    AAX_CTypeID maxAvidNotification = ('A' << 24) | ('X' << 16) | (0xFF << 8) | 0xFF; // 'AX\xFF\xFF'
    EXPECT_TRUE(IsAvidNotification(maxAvidNotification));
}

//--------------------------------------------------------------------
#pragma mark - Edge Cases and Boundary Conditions

TEST_F(AAX_UtilsNative, EdgeCasesAndBoundaryConditions)
{
    using namespace AAX;

    // Test SafeLog with special floating point values
    double nanResult = SafeLog(std::numeric_limits<double>::quiet_NaN());
    EXPECT_DOUBLE_EQ(0.0, nanResult); // NaN input should return 0.0 for safety

    double infResult = SafeLog(std::numeric_limits<double>::infinity());
    EXPECT_DOUBLE_EQ(0.0, infResult); // +infinity input should return 0.0 for safety

    // Test SafeLogf with special floating point values
    float nanResultf = SafeLogf(std::numeric_limits<float>::quiet_NaN());
    EXPECT_FLOAT_EQ(0.0f, nanResultf); // NaN input should return 0.0f for safety

    float infResultf = SafeLogf(std::numeric_limits<float>::infinity());
    EXPECT_FLOAT_EQ(0.0f, infResultf); // +infinity input should return 0.0f for safety

    // Test parameter ID comparison with strings containing null characters
    const char paramWithNull[] = {'g', 'a', 'i', 'n', '\0', 'x', '\0'};
    const char paramWithNull2[] = {'g', 'a', 'i', 'n', '\0', 'y', '\0'};
    EXPECT_TRUE(IsParameterIDEqual(paramWithNull, paramWithNull2)); // Should be equal up to first null

    // Test very long parameter IDs (beyond max size)
    std::string veryLongParam1(kAAX_ParameterIdentifierMaxSize + 10, 'a');
    std::string veryLongParam2(kAAX_ParameterIdentifierMaxSize + 10, 'a');
    veryLongParam2[kAAX_ParameterIdentifierMaxSize] = 'b'; // Differ beyond max size
    EXPECT_TRUE(IsParameterIDEqual(veryLongParam1.c_str(), veryLongParam2.c_str())); // Should be equal within max size

    // Test notification IDs with all possible byte combinations for 'AX' pattern
    for (uint8_t byte3 = 0; byte3 < 16; ++byte3) // Test subset to avoid long test
    {
        for (uint8_t byte4 = 0; byte4 < 16; ++byte4)
        {
            AAX_CTypeID testNotification = ('A' << 24) | ('X' << 16) | (byte3 << 8) | byte4;
            EXPECT_TRUE(IsAvidNotification(testNotification))
                << "Failed for AX notification with bytes: " << static_cast<int>(byte3) << ", " << static_cast<int>(byte4);
        }
    }
}
