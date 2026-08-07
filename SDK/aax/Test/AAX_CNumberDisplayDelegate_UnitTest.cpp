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
 *	\file AAX_CNumberDisplayDelegate_UnitTest.cpp
 *
 *	\brief Unit tests for AAX_CNumberDisplayDelegate template class
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX_CNumberDisplayDelegate.h"
#include "AAX_CString.h"

//--------------------------------------------------------------------
#pragma mark - Test fixture

class AAX_CNumberDisplayDelegate_Test : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

//--------------------------------------------------------------------
#pragma mark - Basic Functionality Tests

TEST_F(AAX_CNumberDisplayDelegate_Test, FloatDisplay_DefaultPrecision)
{
    AAX_CNumberDisplayDelegate<float> display; // Default: Precision=2, SpaceAfter=0
    AAX_CString result;
    
    // Test basic value to string conversion
    EXPECT_TRUE(display.ValueToString(3.14159f, &result));
    EXPECT_STREQ("3.14", result.Get()); // Should round to 2 decimal places
    
    EXPECT_TRUE(display.ValueToString(1.0f, &result));
    EXPECT_STREQ("1.00", result.Get()); // Should show 2 decimal places
    
    EXPECT_TRUE(display.ValueToString(0.0f, &result));
    EXPECT_STREQ("0.00", result.Get());
}

TEST_F(AAX_CNumberDisplayDelegate_Test, FloatDisplay_CustomPrecision)
{
    AAX_CNumberDisplayDelegate<float, 3> display; // Precision=3, SpaceAfter=0
    AAX_CString result;
    
    EXPECT_TRUE(display.ValueToString(3.14159f, &result));
    EXPECT_STREQ("3.142", result.Get()); // Should round to 3 decimal places
    
    EXPECT_TRUE(display.ValueToString(1.0f, &result));
    EXPECT_STREQ("1.000", result.Get()); // Should show 3 decimal places
}

TEST_F(AAX_CNumberDisplayDelegate_Test, FloatDisplay_ZeroPrecision)
{
    AAX_CNumberDisplayDelegate<float, 0> display; // Precision=0, SpaceAfter=0
    AAX_CString result;
    
    EXPECT_TRUE(display.ValueToString(3.14159f, &result));
    EXPECT_STREQ("3", result.Get()); // Should show no decimal places
    
    EXPECT_TRUE(display.ValueToString(3.7f, &result));
    EXPECT_STREQ("4", result.Get()); // Should round to nearest integer
}

TEST_F(AAX_CNumberDisplayDelegate_Test, FloatDisplay_WithSpaceAfter)
{
    AAX_CNumberDisplayDelegate<float, 2, 1> display; // Precision=2, SpaceAfter=1
    AAX_CString result;
    
    EXPECT_TRUE(display.ValueToString(3.14f, &result));
    EXPECT_STREQ("3.14 ", result.Get()); // Should have space after number
    
    EXPECT_TRUE(display.ValueToString(100.0f, &result));
    EXPECT_STREQ("100.00 ", result.Get());
}

TEST_F(AAX_CNumberDisplayDelegate_Test, IntDisplay_Basic)
{
    AAX_CNumberDisplayDelegate<int32_t> display; // Default: Precision=2, SpaceAfter=0
    AAX_CString result;

    // Integer display still uses precision template parameter
    EXPECT_TRUE(display.ValueToString(42, &result));
    EXPECT_STREQ("42.00", result.Get()); // Shows decimal places based on precision

    EXPECT_TRUE(display.ValueToString(0, &result));
    EXPECT_STREQ("0.00", result.Get());

    EXPECT_TRUE(display.ValueToString(-15, &result));
    EXPECT_STREQ("-15.00", result.Get());
}

TEST_F(AAX_CNumberDisplayDelegate_Test, ValueToString_WithMaxChars)
{
    AAX_CNumberDisplayDelegate<float, 3> display; // Precision=3
    AAX_CString result;
    
    // Test truncation to max characters
    EXPECT_TRUE(display.ValueToString(123.456f, 5, &result));
    EXPECT_EQ(5U, result.Length());
    EXPECT_STREQ("123.4", result.Get()); // Truncated to 5 characters
    
    // Test edge case where decimal point would be last character
    EXPECT_TRUE(display.ValueToString(123.456f, 4, &result));
    EXPECT_STREQ("123", result.Get()); // Decimal point removed if it would be last
}

TEST_F(AAX_CNumberDisplayDelegate_Test, ValueToString_WithMaxChars_SpaceAfter)
{
    AAX_CNumberDisplayDelegate<float, 2, 1> display; // Precision=2, SpaceAfter=1
    AAX_CString result;
    
    // Test with enough space for number and space
    EXPECT_TRUE(display.ValueToString(12.34f, 10, &result));
    EXPECT_STREQ("12.34 ", result.Get()); // Should include space
    
    // Test with limited space - space should be dropped
    EXPECT_TRUE(display.ValueToString(12.34f, 5, &result));
    EXPECT_STREQ("12.34", result.Get()); // Space dropped due to length limit
}

TEST_F(AAX_CNumberDisplayDelegate_Test, StringToValue_Float)
{
    AAX_CNumberDisplayDelegate<float> display;
    float result;
    
    // Test valid string conversions
    EXPECT_TRUE(display.StringToValue(AAX_CString("3.14"), &result));
    EXPECT_FLOAT_EQ(3.14f, result);
    
    EXPECT_TRUE(display.StringToValue(AAX_CString("0"), &result));
    EXPECT_FLOAT_EQ(0.0f, result);
    
    EXPECT_TRUE(display.StringToValue(AAX_CString("-2.5"), &result));
    EXPECT_FLOAT_EQ(-2.5f, result);
    
    EXPECT_TRUE(display.StringToValue(AAX_CString("100.0"), &result));
    EXPECT_FLOAT_EQ(100.0f, result);
}

TEST_F(AAX_CNumberDisplayDelegate_Test, StringToValue_Int)
{
    AAX_CNumberDisplayDelegate<int32_t> display;
    int32_t result;
    
    // Test valid string conversions
    EXPECT_TRUE(display.StringToValue(AAX_CString("42"), &result));
    EXPECT_EQ(42, result);
    
    EXPECT_TRUE(display.StringToValue(AAX_CString("-15"), &result));
    EXPECT_EQ(-15, result);
    
    // Test float string to int conversion
    EXPECT_TRUE(display.StringToValue(AAX_CString("3.7"), &result));
    EXPECT_EQ(3, result); // Should truncate to integer
}

TEST_F(AAX_CNumberDisplayDelegate_Test, StringToValue_InvalidStrings)
{
    AAX_CNumberDisplayDelegate<float> display;
    float result = 999.0f; // Initialize to non-zero value
    
    // Test invalid string conversions
    EXPECT_FALSE(display.StringToValue(AAX_CString("abc"), &result));
    EXPECT_FLOAT_EQ(0.0f, result); // Should be set to 0 on failure
    
    EXPECT_FALSE(display.StringToValue(AAX_CString(""), &result));
    EXPECT_FLOAT_EQ(0.0f, result);
    
    // String parsing is more lenient - parses up to first invalid character
    EXPECT_TRUE(display.StringToValue(AAX_CString("12.34.56"), &result));
    EXPECT_FLOAT_EQ(12.34f, result); // Parses "12.34" and stops at second decimal point
}

TEST_F(AAX_CNumberDisplayDelegate_Test, Clone)
{
    AAX_CNumberDisplayDelegate<float, 3, 1> original;
    std::unique_ptr<AAX_CNumberDisplayDelegate<float, 3, 1>> clone(original.Clone());
    
    // Test that clone works the same as original
    AAX_CString originalResult, cloneResult;
    float testValue = 12.345f;
    
    EXPECT_TRUE(original.ValueToString(testValue, &originalResult));
    EXPECT_TRUE(clone->ValueToString(testValue, &cloneResult));
    EXPECT_STREQ(originalResult.Get(), cloneResult.Get());
    
    // Test string to value conversion
    float originalValue, cloneValue;
    EXPECT_TRUE(original.StringToValue(AAX_CString("6.789"), &originalValue));
    EXPECT_TRUE(clone->StringToValue(AAX_CString("6.789"), &cloneValue));
    EXPECT_FLOAT_EQ(originalValue, cloneValue);
}

TEST_F(AAX_CNumberDisplayDelegate_Test, RoundTrip_Conversion)
{
    AAX_CNumberDisplayDelegate<float, 2> display;
    
    float originalValues[] = {0.0f, 1.23f, -4.56f, 100.0f, 0.01f};
    
    for (float originalValue : originalValues) {
        AAX_CString stringValue;
        EXPECT_TRUE(display.ValueToString(originalValue, &stringValue));
        
        float convertedValue;
        EXPECT_TRUE(display.StringToValue(stringValue, &convertedValue));
        
        // Should be equal within precision limits (2 decimal places)
        EXPECT_NEAR(originalValue, convertedValue, 0.01f);
    }
}
