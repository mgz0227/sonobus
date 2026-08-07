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
 *	\file AAX_CScaledNumberDisplayDelegate_UnitTest.cpp
 *
 *	\brief Unit tests for AAX_CScaledNumberDisplayDelegate template class
 *
 */ 
/*================================================================================================*/

#include "gtest/gtest.h"
#include "AAX_CScaledNumberDisplayDelegate.h"
#include "AAX_CString.h"
#include <cmath>
#include <limits>

//--------------------------------------------------------------------
#pragma mark - Test fixture

class AAX_CScaledNumberDisplayDelegate_Test : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

//--------------------------------------------------------------------
#pragma mark - Basic Functionality Tests

TEST_F(AAX_CScaledNumberDisplayDelegate_Test, FloatDisplay_DefaultScaling)
{
    // Default: Precision=2, SpaceAfter=0, ScaleFactor=1.0
    AAX_CScaledNumberDisplayDelegate<float> display(1.0);
    AAX_CString result;
    
    // Test basic value to string conversion with no scaling (1.0 scale factor)
    EXPECT_TRUE(display.ValueToString(3.14159f, &result));
    EXPECT_STREQ("3.14", result.Get()); // Should round to 2 decimal places
    
    EXPECT_TRUE(display.ValueToString(1.0f, &result));
    EXPECT_STREQ("1.00", result.Get()); // Should show 2 decimal places
    
    EXPECT_TRUE(display.ValueToString(0.0f, &result));
    EXPECT_STREQ("0.00", result.Get());
}

TEST_F(AAX_CScaledNumberDisplayDelegate_Test, FloatDisplay_ScaleFactor100)
{
    // Scale factor of 100 - display values are 100x the logical values
    AAX_CScaledNumberDisplayDelegate<float> display(100.0);
    AAX_CString result;
    
    // Test scaling: logical value 1.0 should display as "100.00"
    EXPECT_TRUE(display.ValueToString(1.0f, &result));
    EXPECT_STREQ("100.00", result.Get());
    
    // Test scaling: logical value 0.01 should display as "1.00"
    EXPECT_TRUE(display.ValueToString(0.01f, &result));
    EXPECT_STREQ("1.00", result.Get());
    
    // Test scaling: logical value 0.5 should display as "50.00"
    EXPECT_TRUE(display.ValueToString(0.5f, &result));
    EXPECT_STREQ("50.00", result.Get());
    
    // Test scaling: logical value 0.0 should display as "0.00"
    EXPECT_TRUE(display.ValueToString(0.0f, &result));
    EXPECT_STREQ("0.00", result.Get());
}

TEST_F(AAX_CScaledNumberDisplayDelegate_Test, FloatDisplay_ScaleFactor001)
{
    // Scale factor of 0.01 - display values are 1/100th of the logical values
    AAX_CScaledNumberDisplayDelegate<float> display(0.01);
    AAX_CString result;
    
    // Test scaling: logical value 100.0 should display as "1.00"
    EXPECT_TRUE(display.ValueToString(100.0f, &result));
    EXPECT_STREQ("1.00", result.Get());
    
    // Test scaling: logical value 1.0 should display as "0.01"
    EXPECT_TRUE(display.ValueToString(1.0f, &result));
    EXPECT_STREQ("0.01", result.Get());
    
    // Test scaling: logical value 50.0 should display as "0.50"
    EXPECT_TRUE(display.ValueToString(50.0f, &result));
    EXPECT_STREQ("0.50", result.Get());
}

TEST_F(AAX_CScaledNumberDisplayDelegate_Test, FloatDisplay_ScaleFactor1000)
{
    // Scale factor of 1000 - display values are 1000x the logical values
    AAX_CScaledNumberDisplayDelegate<float> display(1000.0);
    AAX_CString result;
    
    // Test scaling: logical value 1.0 should display as "1000.00"
    EXPECT_TRUE(display.ValueToString(1.0f, &result));
    EXPECT_STREQ("1000.00", result.Get());
    
    // Test scaling: logical value 0.001 should display as "1.00"
    EXPECT_TRUE(display.ValueToString(0.001f, &result));
    EXPECT_STREQ("1.00", result.Get());
    
    // Test scaling: logical value 0.0015 should display as "1.50"
    EXPECT_TRUE(display.ValueToString(0.0015f, &result));
    EXPECT_STREQ("1.50", result.Get());
}

TEST_F(AAX_CScaledNumberDisplayDelegate_Test, StringToValue_ScaleFactor100)
{
    // Test reverse conversion with scale factor of 100
    AAX_CScaledNumberDisplayDelegate<float> display(100.0);
    float result;
    
    // Display string "100.00" should convert to logical value 1.0
    EXPECT_TRUE(display.StringToValue(AAX_CString("100.00"), &result));
    EXPECT_FLOAT_EQ(1.0f, result);
    
    // Display string "1.00" should convert to logical value 0.01
    EXPECT_TRUE(display.StringToValue(AAX_CString("1.00"), &result));
    EXPECT_FLOAT_EQ(0.01f, result);
    
    // Display string "50.00" should convert to logical value 0.5
    EXPECT_TRUE(display.StringToValue(AAX_CString("50.00"), &result));
    EXPECT_FLOAT_EQ(0.5f, result);
    
    // Display string "0.00" should convert to logical value 0.0
    EXPECT_TRUE(display.StringToValue(AAX_CString("0.00"), &result));
    EXPECT_FLOAT_EQ(0.0f, result);
}

TEST_F(AAX_CScaledNumberDisplayDelegate_Test, StringToValue_ScaleFactor001)
{
    // Test reverse conversion with scale factor of 0.01
    AAX_CScaledNumberDisplayDelegate<float> display(0.01);
    float result;
    
    // Display string "1.00" should convert to logical value 100.0
    EXPECT_TRUE(display.StringToValue(AAX_CString("1.00"), &result));
    EXPECT_FLOAT_EQ(100.0f, result);
    
    // Display string "0.01" should convert to logical value 1.0
    EXPECT_TRUE(display.StringToValue(AAX_CString("0.01"), &result));
    EXPECT_FLOAT_EQ(1.0f, result);
    
    // Display string "0.50" should convert to logical value 50.0
    EXPECT_TRUE(display.StringToValue(AAX_CString("0.50"), &result));
    EXPECT_FLOAT_EQ(50.0f, result);
}

TEST_F(AAX_CScaledNumberDisplayDelegate_Test, RoundTrip_ScaleFactor100)
{
    // Test that values can round-trip through string conversion with scaling
    AAX_CScaledNumberDisplayDelegate<float> display(100.0);
    
    float originalValues[] = {0.0f, 0.01f, 0.5f, 1.0f, 2.5f};
    
    for (float originalValue : originalValues) {
        AAX_CString stringValue;
        EXPECT_TRUE(display.ValueToString(originalValue, &stringValue));
        
        float convertedValue;
        EXPECT_TRUE(display.StringToValue(stringValue, &convertedValue));
        
        // Should be equal within precision limits (2 decimal places) and scaling considerations
        EXPECT_NEAR(originalValue, convertedValue, 0.01f) << "Round-trip failed for value " << originalValue 
                                                           << " (string was: " << stringValue.Get() << ")";
    }
}

TEST_F(AAX_CScaledNumberDisplayDelegate_Test, RoundTrip_ScaleFactor001)
{
    // Test that values can round-trip through string conversion with scaling
    AAX_CScaledNumberDisplayDelegate<float> display(0.01);
    
    float originalValues[] = {0.0f, 1.0f, 50.0f, 100.0f, 250.0f};
    
    for (float originalValue : originalValues) {
        AAX_CString stringValue;
        EXPECT_TRUE(display.ValueToString(originalValue, &stringValue));
        
        float convertedValue;
        EXPECT_TRUE(display.StringToValue(stringValue, &convertedValue));
        
        // Should be equal within precision limits (2 decimal places) and scaling considerations
        EXPECT_NEAR(originalValue, convertedValue, 1.0f) << "Round-trip failed for value " << originalValue 
                                                          << " (string was: " << stringValue.Get() << ")";
    }
}

//--------------------------------------------------------------------
#pragma mark - Precision and SpaceAfter Template Parameter Tests

TEST_F(AAX_CScaledNumberDisplayDelegate_Test, FloatDisplay_CustomPrecision_WithScaling)
{
    // Test precision=3 with scaling factor 100
    AAX_CScaledNumberDisplayDelegate<float, 3> display(100.0);
    AAX_CString result;
    
    EXPECT_TRUE(display.ValueToString(3.14159f, &result));
    EXPECT_STREQ("314.159", result.Get()); // Should round to 3 decimal places after scaling
    
    EXPECT_TRUE(display.ValueToString(1.0f, &result));
    EXPECT_STREQ("100.000", result.Get()); // Should show 3 decimal places
    
    EXPECT_TRUE(display.ValueToString(0.01f, &result));
    EXPECT_STREQ("1.000", result.Get());
}

TEST_F(AAX_CScaledNumberDisplayDelegate_Test, FloatDisplay_ZeroPrecision_WithScaling)
{
    // Test precision=0 with scaling factor 100
    AAX_CScaledNumberDisplayDelegate<float, 0> display(100.0);
    AAX_CString result;
    
    EXPECT_TRUE(display.ValueToString(3.14159f, &result));
    EXPECT_STREQ("314", result.Get()); // Should show no decimal places
    
    EXPECT_TRUE(display.ValueToString(1.0f, &result));
    EXPECT_STREQ("100", result.Get());
    
    EXPECT_TRUE(display.ValueToString(0.037f, &result));
    EXPECT_STREQ("4", result.Get()); // Should round to nearest integer (3.7 -> 4)
}

TEST_F(AAX_CScaledNumberDisplayDelegate_Test, FloatDisplay_WithSpaceAfter_WithScaling)
{
    // Test SpaceAfter=1 with scaling factor 100
    AAX_CScaledNumberDisplayDelegate<float, 2, 1> display(100.0);
    AAX_CString result;
    
    EXPECT_TRUE(display.ValueToString(3.14f, &result));
    EXPECT_STREQ("314.00 ", result.Get()); // Should have space after number
    
    EXPECT_TRUE(display.ValueToString(1.0f, &result));
    EXPECT_STREQ("100.00 ", result.Get());
    
    EXPECT_TRUE(display.ValueToString(0.01f, &result));
    EXPECT_STREQ("1.00 ", result.Get());
}

TEST_F(AAX_CScaledNumberDisplayDelegate_Test, FloatDisplay_HighPrecision_LowScaling)
{
    // Test high precision (5) with low scaling factor (0.001)
    AAX_CScaledNumberDisplayDelegate<float, 5> display(0.001);
    AAX_CString result;
    
    EXPECT_TRUE(display.ValueToString(1000.0f, &result));
    EXPECT_STREQ("1.00000", result.Get()); // Scaled down
    
    EXPECT_TRUE(display.ValueToString(12345.67f, &result));
    EXPECT_STREQ("12.34567", result.Get()); // Show full precision
    
    EXPECT_TRUE(display.ValueToString(1.0f, &result));
    EXPECT_STREQ("0.00100", result.Get());
}

TEST_F(AAX_CScaledNumberDisplayDelegate_Test, StringToValue_CustomPrecision_WithScaling)
{
    // Test reverse conversion with custom precision and scaling
    AAX_CScaledNumberDisplayDelegate<float, 3> display(100.0);
    float result;
    
    // Higher precision strings should still convert correctly
    EXPECT_TRUE(display.StringToValue(AAX_CString("314.159"), &result));
    EXPECT_NEAR(3.14159f, result, 0.001f);
    
    EXPECT_TRUE(display.StringToValue(AAX_CString("100.000"), &result));
    EXPECT_FLOAT_EQ(1.0f, result);
    
    EXPECT_TRUE(display.StringToValue(AAX_CString("1.000"), &result));
    EXPECT_FLOAT_EQ(0.01f, result);
}

//--------------------------------------------------------------------
#pragma mark - Edge Cases and Error Handling Tests

TEST_F(AAX_CScaledNumberDisplayDelegate_Test, StringToValue_InvalidStrings_WithScaling)
{
    AAX_CScaledNumberDisplayDelegate<float> display(100.0);
    float result = 999.0f; // Initialize to non-zero value
    
    // Test invalid string conversions
    EXPECT_FALSE(display.StringToValue(AAX_CString("abc"), &result));
    EXPECT_FLOAT_EQ(0.0f, result); // Should be set to 0 on failure
    
    EXPECT_FALSE(display.StringToValue(AAX_CString(""), &result));
    EXPECT_FLOAT_EQ(0.0f, result);
    
    // String parsing is more lenient - parses up to first invalid character
    EXPECT_TRUE(display.StringToValue(AAX_CString("12.34.56"), &result));
    EXPECT_NEAR(0.1234f, result, 0.001f); // "12.34" scaled down by 100
}

TEST_F(AAX_CScaledNumberDisplayDelegate_Test, NegativeValues_WithScaling)
{
    AAX_CScaledNumberDisplayDelegate<float> display(100.0);
    AAX_CString result;
    
    // Test negative values with scaling
    EXPECT_TRUE(display.ValueToString(-1.0f, &result));
    EXPECT_STREQ("-100.00", result.Get());
    
    EXPECT_TRUE(display.ValueToString(-0.5f, &result));
    EXPECT_STREQ("-50.00", result.Get());
    
    // Test reverse conversion
    float value;
    EXPECT_TRUE(display.StringToValue(AAX_CString("-100.00"), &value));
    EXPECT_FLOAT_EQ(-1.0f, value);
    
    EXPECT_TRUE(display.StringToValue(AAX_CString("-50.00"), &value));
    EXPECT_FLOAT_EQ(-0.5f, value);
}

TEST_F(AAX_CScaledNumberDisplayDelegate_Test, ZeroScaleFactor_EdgeCase)
{
    // NOTE: This test documents the old expected behavior before the zero guard was added
    // With the zero guard in the constructor, zero scale factor gets converted to 1.0
    AAX_CScaledNumberDisplayDelegate<float> display(0.0);
    AAX_CString result;
    
    // With the zero guard, this should behave exactly like scale factor 1.0
    EXPECT_TRUE(display.ValueToString(100.0f, &result));
    EXPECT_STREQ("100.00", result.Get()); // 100.0 * 1.0 = 100.00 (zero guard converts 0.0 to 1.0)
    
    EXPECT_TRUE(display.ValueToString(-50.0f, &result));
    EXPECT_STREQ("-50.00", result.Get()); // -50.0 * 1.0 = -50.00 (zero guard converts 0.0 to 1.0)
}

TEST_F(AAX_CScaledNumberDisplayDelegate_Test, VeryLargeScaleFactor)
{
    // Test with very large scale factor
    AAX_CScaledNumberDisplayDelegate<float> display(1000000.0);
    AAX_CString result;
    
    EXPECT_TRUE(display.ValueToString(0.000001f, &result));
    EXPECT_STREQ("1.00", result.Get());
    
    EXPECT_TRUE(display.ValueToString(0.001f, &result));
    EXPECT_STREQ("1000.00", result.Get());
    
    // Test reverse conversion
    float value;
    EXPECT_TRUE(display.StringToValue(AAX_CString("1000000.00"), &value));
    EXPECT_NEAR(1.0f, value, 0.001f);
}

TEST_F(AAX_CScaledNumberDisplayDelegate_Test, VerySmallScaleFactor)
{
    // Test with very small scale factor
    AAX_CScaledNumberDisplayDelegate<float> display(0.000001);
    AAX_CString result;
    
    EXPECT_TRUE(display.ValueToString(1000000.0f, &result));
    EXPECT_STREQ("1.00", result.Get());
    
    EXPECT_TRUE(display.ValueToString(1000.0f, &result));
    EXPECT_STREQ("0.00", result.Get()); // Very small result rounds to 0.00
    
    // Test reverse conversion
    float value;
    EXPECT_TRUE(display.StringToValue(AAX_CString("1.00"), &value));
    EXPECT_NEAR(1000000.0f, value, 1000.0f);
}

TEST_F(AAX_CScaledNumberDisplayDelegate_Test, Clone_WithScaling)
{
    AAX_CScaledNumberDisplayDelegate<float, 3, 1> original(50.0);
    std::unique_ptr<AAX_CScaledNumberDisplayDelegate<float, 3, 1>> clone(original.Clone());
    
    // Test that clone works the same as original
    AAX_CString originalResult, cloneResult;
    float testValue = 2.0f;
    
    EXPECT_TRUE(original.ValueToString(testValue, &originalResult));
    EXPECT_TRUE(clone->ValueToString(testValue, &cloneResult));
    EXPECT_STREQ(originalResult.Get(), cloneResult.Get());
    
    // Test string to value conversion
    float originalValue, cloneValue;
    EXPECT_TRUE(original.StringToValue(AAX_CString("150.000"), &originalValue));
    EXPECT_TRUE(clone->StringToValue(AAX_CString("150.000"), &cloneValue));
    EXPECT_FLOAT_EQ(originalValue, cloneValue);
    
    // Both should produce "100.000 " (2.0 * 50.0 = 100.0, with space after)
    EXPECT_STREQ("100.000 ", originalResult.Get());
}

//--------------------------------------------------------------------
#pragma mark - MaxNumChars Functionality Tests

TEST_F(AAX_CScaledNumberDisplayDelegate_Test, ValueToString_WithMaxChars_WithScaling)
{
    AAX_CScaledNumberDisplayDelegate<float, 3> display(100.0); // Precision=3, scaling=100
    AAX_CString result;
    
    // Test truncation to max characters with scaling
    EXPECT_TRUE(display.ValueToString(1.23456f, 5, &result));
    EXPECT_EQ(5U, result.Length());
    EXPECT_STREQ("123.4", result.Get()); // Truncated to 5 characters (123.456 -> 123.4)
    
    // Test edge case where decimal point would be last character
    EXPECT_TRUE(display.ValueToString(1.23456f, 4, &result));
    EXPECT_STREQ("123", result.Get()); // Decimal point removed if it would be last
}

TEST_F(AAX_CScaledNumberDisplayDelegate_Test, ValueToString_WithMaxChars_SpaceAfter_WithScaling)
{
    AAX_CScaledNumberDisplayDelegate<float, 2, 1> display(100.0); // Precision=2, SpaceAfter=1, scaling=100
    AAX_CString result;
    
    // Test with enough space for number and space
    EXPECT_TRUE(display.ValueToString(0.1234f, 10, &result));
    EXPECT_STREQ("12.34 ", result.Get()); // Should include space
    
    // Test with limited space - space should be dropped
    EXPECT_TRUE(display.ValueToString(0.1234f, 5, &result));
    EXPECT_STREQ("12.34", result.Get()); // Space dropped due to length limit
    
    // Test with very limited space
    EXPECT_TRUE(display.ValueToString(0.1234f, 3, &result));
    EXPECT_STREQ("12", result.Get()); // Truncated, decimal point removed if it would be last char
}

TEST_F(AAX_CScaledNumberDisplayDelegate_Test, ValueToString_WithMaxChars_LargeNumbers)
{
    AAX_CScaledNumberDisplayDelegate<float, 2> display(1000.0); // Scale by 1000
    AAX_CString result;
    
    // Large scaled number should be properly truncated
    EXPECT_TRUE(display.ValueToString(12.3456f, 6, &result));
    EXPECT_EQ(5U, result.Length());
    EXPECT_STREQ("12345", result.Get()); // "12345.60" truncated to 6 chars, decimal at end removed
    
    EXPECT_TRUE(display.ValueToString(12.3456f, 7, &result));
    EXPECT_STREQ("12345.6", result.Get()); // Full precision fits in 7 chars
}

TEST_F(AAX_CScaledNumberDisplayDelegate_Test, ValueToString_WithMaxChars_ZeroValue)
{
    AAX_CScaledNumberDisplayDelegate<float, 2, 1> display(100.0);
    AAX_CString result;
    
    // Zero value with scaling and space after
    EXPECT_TRUE(display.ValueToString(0.0f, 10, &result));
    EXPECT_STREQ("0.00 ", result.Get()); // Should include space
    
    EXPECT_TRUE(display.ValueToString(0.0f, 4, &result));
    EXPECT_STREQ("0.00", result.Get()); // Space dropped due to length limit
    
    EXPECT_TRUE(display.ValueToString(0.0f, 2, &result));
    EXPECT_STREQ("0", result.Get()); // Truncated, decimal at end removed
}

//--------------------------------------------------------------------
#pragma mark - Different Numeric Types Tests

TEST_F(AAX_CScaledNumberDisplayDelegate_Test, DoubleDisplay_WithScaling)
{
    AAX_CScaledNumberDisplayDelegate<double> display(100.0);
    AAX_CString result;
    
    // Test double precision with scaling
    EXPECT_TRUE(display.ValueToString(3.141592653589793, &result));
    EXPECT_STREQ("314.16", result.Get()); // Should round to 2 decimal places after scaling
    
    EXPECT_TRUE(display.ValueToString(1.0, &result));
    EXPECT_STREQ("100.00", result.Get());
    
    // Test reverse conversion
    double value;
    EXPECT_TRUE(display.StringToValue(AAX_CString("314.16"), &value));
    EXPECT_NEAR(3.1416, value, 0.0001);
}

TEST_F(AAX_CScaledNumberDisplayDelegate_Test, Int32Display_WithScaling)
{
    AAX_CScaledNumberDisplayDelegate<int32_t> display(100.0);
    AAX_CString result;

    // Integer display with scaling still uses precision template parameter
    EXPECT_TRUE(display.ValueToString(42, &result));
    EXPECT_STREQ("4200.00", result.Get()); // 42 * 100 = 4200, shows decimal places based on precision

    EXPECT_TRUE(display.ValueToString(0, &result));
    EXPECT_STREQ("0.00", result.Get());

    EXPECT_TRUE(display.ValueToString(-15, &result));
    EXPECT_STREQ("-1500.00", result.Get()); // -15 * 100 = -1500
    
    // Test reverse conversion
    int32_t value;
    EXPECT_TRUE(display.StringToValue(AAX_CString("4200.00"), &value));
    EXPECT_EQ(42, value);
    
    EXPECT_TRUE(display.StringToValue(AAX_CString("0.00"), &value));
    EXPECT_EQ(0, value);
    
    EXPECT_TRUE(display.StringToValue(AAX_CString("-1500.00"), &value));
    EXPECT_EQ(-15, value);
}

TEST_F(AAX_CScaledNumberDisplayDelegate_Test, Int32Display_FractionalScaling)
{
    // Test integer type with fractional scaling
    AAX_CScaledNumberDisplayDelegate<int32_t> display(0.01);
    AAX_CString result;
    
    EXPECT_TRUE(display.ValueToString(100, &result));
    EXPECT_STREQ("1.00", result.Get()); // 100 * 0.01 = 1.0
    
    EXPECT_TRUE(display.ValueToString(1, &result));
    EXPECT_STREQ("0.01", result.Get()); // 1 * 0.01 = 0.01
    
    // Test reverse conversion
    int32_t value;
    EXPECT_TRUE(display.StringToValue(AAX_CString("1.00"), &value));
    EXPECT_EQ(100, value);
    
    EXPECT_TRUE(display.StringToValue(AAX_CString("0.01"), &value));
    EXPECT_EQ(1, value);
}

TEST_F(AAX_CScaledNumberDisplayDelegate_Test, FloatDisplay_ZeroPrecision_WithScaling_IntegerResult)
{
    // Test that integer-like results work with different types
    AAX_CScaledNumberDisplayDelegate<float, 0> display(10.0); // Scale by 10, no decimal places
    AAX_CString result;
    
    EXPECT_TRUE(display.ValueToString(1.2f, &result));
    EXPECT_STREQ("12", result.Get()); // 1.2 * 10 = 12, no decimals
    
    EXPECT_TRUE(display.ValueToString(0.37f, &result));
    EXPECT_STREQ("4", result.Get()); // 0.37 * 10 = 3.7, rounds to 4
    
    // Test with int32_t for comparison
    AAX_CScaledNumberDisplayDelegate<int32_t, 0> intDisplay(10.0);
    EXPECT_TRUE(intDisplay.ValueToString(1, &result));
    EXPECT_STREQ("10", result.Get()); // 1 * 10 = 10, no decimals
}

TEST_F(AAX_CScaledNumberDisplayDelegate_Test, MixedTypeRoundTrip)
{
    // Test round-trip with different numeric types and scaling
    {
        AAX_CScaledNumberDisplayDelegate<double> doubleDisplay(100.0);
        double originalValues[] = {0.0, 0.01, 0.5, 1.0, 2.5};
        
        for (double originalValue : originalValues) {
            AAX_CString stringValue;
            EXPECT_TRUE(doubleDisplay.ValueToString(originalValue, &stringValue));
            
            double convertedValue;
            EXPECT_TRUE(doubleDisplay.StringToValue(stringValue, &convertedValue));
            
            EXPECT_NEAR(originalValue, convertedValue, 0.01) << "Double round-trip failed for value " << originalValue;
        }
    }
    
    {
        AAX_CScaledNumberDisplayDelegate<int32_t> intDisplay(1.0); // No scaling for simpler test
        int32_t originalValues[] = {0, 1, -5, 42, 100};
        
        for (int32_t originalValue : originalValues) {
            AAX_CString stringValue;
            EXPECT_TRUE(intDisplay.ValueToString(originalValue, &stringValue));
            
            int32_t convertedValue;
            EXPECT_TRUE(intDisplay.StringToValue(stringValue, &convertedValue));
            
            EXPECT_EQ(originalValue, convertedValue) << "Int32 round-trip failed for value " << originalValue;
        }
    }
}

TEST_F(AAX_CScaledNumberDisplayDelegate_Test, EdgeCase_NaN_Values)
{
    AAX_CScaledNumberDisplayDelegate<float> display(100.0);
    AAX_CString result;
    
    // Test NaN input
    float nanValue = std::numeric_limits<float>::quiet_NaN();
    EXPECT_TRUE(display.ValueToString(nanValue, &result));
    // NaN should be handled gracefully - exact string may vary by platform
    // Just ensure the function doesn't crash and returns true
    EXPECT_GT(result.Length(), 0) << "NaN should produce some string output";
    
    // Test signaling NaN if available
    if (std::numeric_limits<float>::has_signaling_NaN) {
        float signalingNan = std::numeric_limits<float>::signaling_NaN();
        EXPECT_TRUE(display.ValueToString(signalingNan, &result));
        EXPECT_GT(result.Length(), 0) << "Signaling NaN should produce some string output";
    }
}

TEST_F(AAX_CScaledNumberDisplayDelegate_Test, EdgeCase_Infinity_Values)
{
    AAX_CScaledNumberDisplayDelegate<float> display(2.0);
    AAX_CString result;
    
    // Test positive infinity
    float posInf = std::numeric_limits<float>::infinity();
    EXPECT_TRUE(display.ValueToString(posInf, &result));
    EXPECT_GT(result.Length(), 0) << "Positive infinity should produce some string output";
    
    // Test negative infinity
    float negInf = -std::numeric_limits<float>::infinity();
    EXPECT_TRUE(display.ValueToString(negInf, &result));
    EXPECT_GT(result.Length(), 0) << "Negative infinity should produce some string output";
}

TEST_F(AAX_CScaledNumberDisplayDelegate_Test, EdgeCase_Extreme_Values)
{
    AAX_CScaledNumberDisplayDelegate<float> display(1.0);
    AAX_CString result;
    
    // Test maximum float value
    float maxFloat = std::numeric_limits<float>::max();
    EXPECT_TRUE(display.ValueToString(maxFloat, &result));
    EXPECT_GT(result.Length(), 0) << "Maximum float should produce string output";
    
    // Test minimum float value (most negative)
    float minFloat = std::numeric_limits<float>::lowest();
    EXPECT_TRUE(display.ValueToString(minFloat, &result));
    EXPECT_GT(result.Length(), 0) << "Minimum float should produce string output";
    
    // Test smallest positive normalized value
    float minPositive = std::numeric_limits<float>::min();
    EXPECT_TRUE(display.ValueToString(minPositive, &result));
    EXPECT_GT(result.Length(), 0) << "Smallest positive float should produce string output";
    
    // Test denormalized number (very small)
    float denormalized = std::numeric_limits<float>::denorm_min();
    EXPECT_TRUE(display.ValueToString(denormalized, &result));
    EXPECT_GT(result.Length(), 0) << "Denormalized float should produce string output";
}

TEST_F(AAX_CScaledNumberDisplayDelegate_Test, EdgeCase_NaN_Infinity_With_Scaling)
{
    // Test how NaN and infinity interact with scaling factors
    AAX_CScaledNumberDisplayDelegate<double> display(100.0);
    AAX_CString result;
    
    // NaN * 100 should still be NaN
    double nanValue = std::numeric_limits<double>::quiet_NaN();
    EXPECT_TRUE(display.ValueToString(nanValue, &result));
    EXPECT_GT(result.Length(), 0);
    
    // Infinity * 100 should still be infinity
    double posInf = std::numeric_limits<double>::infinity();
    EXPECT_TRUE(display.ValueToString(posInf, &result));
    EXPECT_GT(result.Length(), 0);
    
    // Test with very large scaling factor
    AAX_CScaledNumberDisplayDelegate<double> hugeScaleDisplay(1e20);
    EXPECT_TRUE(hugeScaleDisplay.ValueToString(1.0, &result));
    EXPECT_GT(result.Length(), 0) << "Large scaling should produce string output";
}

TEST_F(AAX_CScaledNumberDisplayDelegate_Test, EdgeCase_StringToValue_Invalid_Special)
{
    AAX_CScaledNumberDisplayDelegate<float> display(1.0);
    float result;
    
    // Test various invalid string representations
    EXPECT_FALSE(display.StringToValue(AAX_CString("NaN"), &result));
    EXPECT_EQ(0.0f, result) << "Invalid string should set result to 0";
    
    EXPECT_FALSE(display.StringToValue(AAX_CString("inf"), &result));
    EXPECT_EQ(0.0f, result) << "Invalid string should set result to 0";
    
    EXPECT_FALSE(display.StringToValue(AAX_CString("-inf"), &result));
    EXPECT_EQ(0.0f, result) << "Invalid string should set result to 0";
    
    EXPECT_FALSE(display.StringToValue(AAX_CString("infinity"), &result));
    EXPECT_EQ(0.0f, result) << "Invalid string should set result to 0";
    
    // Test some other invalid formats  
    // Note: "1.2.3" may actually parse as "1.2" on some systems, so use a clearly invalid format
    EXPECT_FALSE(display.StringToValue(AAX_CString("not_a_number"), &result));
    EXPECT_EQ(0.0f, result) << "Invalid string should set result to 0";
    
    EXPECT_FALSE(display.StringToValue(AAX_CString("abc123"), &result));
    EXPECT_EQ(0.0f, result) << "Invalid string should set result to 0";
    
    EXPECT_FALSE(display.StringToValue(AAX_CString(""), &result));
    EXPECT_EQ(0.0f, result) << "Empty string should set result to 0";
}

TEST_F(AAX_CScaledNumberDisplayDelegate_Test, EdgeCase_MaxChars_With_Special_Values)
{
    AAX_CScaledNumberDisplayDelegate<float> display(1.0);
    AAX_CString result;
    
    // Test NaN with character limit
    float nanValue = std::numeric_limits<float>::quiet_NaN();
    EXPECT_TRUE(display.ValueToString(nanValue, 5, &result));
    EXPECT_LE(result.Length(), 5u) << "Result should respect max character limit";
    
    // Test infinity with character limit
    float posInf = std::numeric_limits<float>::infinity();
    EXPECT_TRUE(display.ValueToString(posInf, 3, &result));
    EXPECT_LE(result.Length(), 3u) << "Result should respect max character limit";
    
    // Test very large number with small character limit
    EXPECT_TRUE(display.ValueToString(1234567.89f, 4, &result));
    EXPECT_LE(result.Length(), 4u) << "Result should respect max character limit";
    EXPECT_GT(result.Length(), 0u) << "Should still produce some output";
}

// Test demonstrating partial string parsing behavior (e.g., "1.2.3" -> 1.2)
TEST_F(AAX_CScaledNumberDisplayDelegate_Test, StringParsing_PartialValidStrings)
{
    AAX_CScaledNumberDisplayDelegate<float> display(1.0);
    float result = 0.0f;
    
    // "1.2.3" should parse as 1.2 (stops at first invalid character)
    EXPECT_TRUE(display.StringToValue(AAX_CString("1.2.3"), &result));
    EXPECT_FLOAT_EQ(result, 1.2f) << "Should parse '1.2.3' as 1.2";
    
    // "123.45.67" should parse as 123.45
    EXPECT_TRUE(display.StringToValue(AAX_CString("123.45.67"), &result));
    EXPECT_FLOAT_EQ(result, 123.45f) << "Should parse '123.45.67' as 123.45";
    
    // "42abc" - behavior depends on underlying parsing implementation
    // Some implementations parse partial numbers, others reject them entirely
    bool parseSuccess = display.StringToValue(AAX_CString("42abc"), &result);
    if (parseSuccess) {
        EXPECT_FLOAT_EQ(result, 42.0f) << "If parsing succeeds, should parse '42abc' as 42.0";
    } else {
        EXPECT_EQ(result, 0.0f) << "If parsing fails, result should be 0.0";
    }
    
    // "-3.14xyz" - similar behavior as above
    parseSuccess = display.StringToValue(AAX_CString("-3.14xyz"), &result);
    if (parseSuccess) {
        EXPECT_FLOAT_EQ(result, -3.14f) << "If parsing succeeds, should parse '-3.14xyz' as -3.14";
    } else {
        EXPECT_EQ(result, 0.0f) << "If parsing fails, result should be 0.0";
    }
    
    // "  5.5  " should parse as 5.5 (with whitespace)
    EXPECT_TRUE(display.StringToValue(AAX_CString("  5.5  "), &result));
    EXPECT_FLOAT_EQ(result, 5.5f) << "Should parse '  5.5  ' as 5.5";
}

// Test zero scale factor behavior (should behave as scale factor 1.0)
TEST_F(AAX_CScaledNumberDisplayDelegate_Test, ZeroScaleFactor_BehavesAsOne)
{
    // Zero scale factor should be converted to 1.0 by the constructor guard
    AAX_CScaledNumberDisplayDelegate<float> display(0.0);
    AAX_CString result;
    
    // Test ValueToString - should behave as if scale factor is 1.0
    EXPECT_TRUE(display.ValueToString(42.5f, &result));
    EXPECT_STREQ("42.50", result.Get()) << "Zero scale factor should behave as 1.0";
    
    // Test StringToValue - should behave as if scale factor is 1.0
    float value = 0.0f;
    EXPECT_TRUE(display.StringToValue(AAX_CString("123.45"), &value));
    EXPECT_FLOAT_EQ(value, 123.45f) << "Zero scale factor should behave as 1.0 for parsing";
    
    // Round-trip test
    EXPECT_TRUE(display.ValueToString(123.45f, &result));
    EXPECT_TRUE(display.StringToValue(result, &value));
    EXPECT_FLOAT_EQ(value, 123.45f) << "Round-trip should work with zero scale factor";
}

// Test negative scale factors
TEST_F(AAX_CScaledNumberDisplayDelegate_Test, NegativeScaleFactor)
{
    AAX_CScaledNumberDisplayDelegate<float> display(-2.0);
    AAX_CString result;
    
    // Positive input with negative scale factor
    EXPECT_TRUE(display.ValueToString(5.0f, &result));
    EXPECT_STREQ("-10.00", result.Get()) << "5.0 * -2.0 should display as -10.00";
    
    // Negative input with negative scale factor (should be positive)
    EXPECT_TRUE(display.ValueToString(-3.0f, &result));
    EXPECT_STREQ("6.00", result.Get()) << "-3.0 * -2.0 should display as 6.00";
    
    // Test StringToValue with negative scale factor
    float value = 0.0f;
    EXPECT_TRUE(display.StringToValue(AAX_CString("-20.0"), &value));
    EXPECT_FLOAT_EQ(value, 10.0f) << "-20.0 / -2.0 should equal 10.0";
    
    // Round-trip test with negative scale factor
    float originalValue = 7.5f;
    EXPECT_TRUE(display.ValueToString(originalValue, &result));
    EXPECT_TRUE(display.StringToValue(result, &value));
    EXPECT_FLOAT_EQ(value, originalValue) << "Round-trip should work with negative scale factor";
}

// Test very large scale factors (potential overflow)
TEST_F(AAX_CScaledNumberDisplayDelegate_Test, VeryLargeScaleFactor_Overflow)
{
    AAX_CScaledNumberDisplayDelegate<float> display(1e20);
    AAX_CString result;
    
    // Large scale factor with normal value
    EXPECT_TRUE(display.ValueToString(1.0f, &result));
    EXPECT_GT(result.Length(), 0u) << "Large scale factor should produce some output";
    
    // Test potential overflow case
    EXPECT_TRUE(display.ValueToString(std::numeric_limits<float>::max(), &result));
    EXPECT_GT(result.Length(), 0u) << "Should handle overflow gracefully";
    
    // Test StringToValue with very large displayed value
    float value = 0.0f;
    bool parseSuccess = display.StringToValue(AAX_CString("1e30"), &value);
    if (parseSuccess) {
        // If parsing succeeds, the result should be reasonable
        EXPECT_TRUE(std::isfinite(value) || std::isinf(value)) << "Parsed value should be finite or infinity";
    }
    // Note: StringToValue might fail with very large numbers, which is acceptable
}

// Test very small scale factors (potential underflow)
TEST_F(AAX_CScaledNumberDisplayDelegate_Test, VerySmallScaleFactor_Underflow)
{
    AAX_CScaledNumberDisplayDelegate<float> display(1e-20);
    AAX_CString result;
    
    // Small scale factor with normal value
    EXPECT_TRUE(display.ValueToString(1.0f, &result));
    EXPECT_GT(result.Length(), 0u) << "Small scale factor should produce some output";
    
    // Test potential underflow case
    EXPECT_TRUE(display.ValueToString(std::numeric_limits<float>::min(), &result));
    EXPECT_GT(result.Length(), 0u) << "Should handle underflow gracefully";
    
    // Test StringToValue with very small displayed value
    float value = 0.0f;
    bool parseSuccess = display.StringToValue(AAX_CString("1e-30"), &value);
    if (parseSuccess) {
        // If parsing succeeds, the result should be reasonable
        EXPECT_TRUE(std::isfinite(value) || value == 0.0f) << "Parsed value should be finite or zero";
    }
    // Note: StringToValue might fail with very small numbers, which is acceptable
}

// Test infinity and NaN scale factors
TEST_F(AAX_CScaledNumberDisplayDelegate_Test, InfinityNaN_ScaleFactors)
{
    // Test with infinity scale factor
    {
        AAX_CScaledNumberDisplayDelegate<float> display(std::numeric_limits<double>::infinity());
        AAX_CString result;
        
        EXPECT_TRUE(display.ValueToString(1.0f, &result));
        EXPECT_GT(result.Length(), 0u) << "Infinity scale factor should produce some output";
        
        float value = 0.0f;
        bool parseSuccess = display.StringToValue(AAX_CString("100"), &value);
        // StringToValue might fail or produce 0 due to division by infinity
        if (parseSuccess) {
            EXPECT_TRUE(std::isfinite(value) || value == 0.0f) << "Division by infinity should give finite result or zero";
        }
    }
    
    // Test with NaN scale factor
    {
        AAX_CScaledNumberDisplayDelegate<float> display(std::numeric_limits<double>::quiet_NaN());
        AAX_CString result;
        
        EXPECT_TRUE(display.ValueToString(1.0f, &result));
        EXPECT_GT(result.Length(), 0u) << "NaN scale factor should produce some output";
        
        float value = 0.0f;
        bool parseSuccess = display.StringToValue(AAX_CString("100"), &value);
        // StringToValue might fail or produce NaN
        if (parseSuccess) {
            EXPECT_TRUE(std::isfinite(value) || std::isnan(value)) << "Division by NaN should give finite result or NaN";
        }
    }
}

// Test extreme combinations that could cause numerical issues
TEST_F(AAX_CScaledNumberDisplayDelegate_Test, ExtremeCombinations_NumericalStability)
{
    // Very large scale factor with very large value
    {
        AAX_CScaledNumberDisplayDelegate<float> display(1e10);
        AAX_CString result;
        
        EXPECT_TRUE(display.ValueToString(std::numeric_limits<float>::max() / 1e15f, &result));
        EXPECT_GT(result.Length(), 0u) << "Extreme combination should not crash";
    }
    
    // Very small scale factor with very small value
    {
        AAX_CScaledNumberDisplayDelegate<float> display(1e-10);
        AAX_CString result;
        
        EXPECT_TRUE(display.ValueToString(std::numeric_limits<float>::min() * 1e15f, &result));
        EXPECT_GT(result.Length(), 0u) << "Extreme combination should not crash";
    }
    
    // Test round-trip with extreme scale factors
    {
        AAX_CScaledNumberDisplayDelegate<double> display(1e-6);
        AAX_CString result;
        double originalValue = 123.456;
        
        EXPECT_TRUE(display.ValueToString(originalValue, &result));
        
        double roundTripValue = 0.0;
        bool parseSuccess = display.StringToValue(result, &roundTripValue);
        
        if (parseSuccess && roundTripValue != 0.0) {
            // With extreme scale factors, allow for more precision loss due to string conversion
            // 123.456 * 1e-6 = 0.000123456, then converted back
            EXPECT_NEAR(roundTripValue, originalValue, 1e-3) << "Round-trip with extreme scale factors may have precision loss";
        } else {
            // If parsing fails or results in zero due to very small numbers, that's acceptable behavior
            // Very small scale factors can result in numbers too small to represent accurately in string form
            EXPECT_EQ(roundTripValue, 0.0) << "Very extreme scale factors may result in underflow to zero";
        }
    }
}

// Test precision limits with extreme scale factors
TEST_F(AAX_CScaledNumberDisplayDelegate_Test, PrecisionLimits_ExtremeScaleFactors)
{
    // High precision with large scale factor
    {
        AAX_CScaledNumberDisplayDelegate<double, 8> display(1e6);
        AAX_CString result;
        
        EXPECT_TRUE(display.ValueToString(1.23456789, &result));
        EXPECT_GT(result.Length(), 0u) << "High precision with large scale factor should work";
        // Result should be "1234567.89000000" or similar
    }
    
    // Zero precision with fractional scale factor
    {
        AAX_CScaledNumberDisplayDelegate<int32_t, 0> display(0.001);
        AAX_CString result;
        
        EXPECT_TRUE(display.ValueToString(1000, &result));
        EXPECT_STREQ("1", result.Get()) << "1000 * 0.001 = 1, displayed as '1'";
    }
    
    // Test MaxChars with extreme scale factors
    {
        AAX_CScaledNumberDisplayDelegate<float> display(1e8);
        AAX_CString result;
        
        EXPECT_TRUE(display.ValueToString(1.234f, 6, &result));
        EXPECT_LE(result.Length(), 6u) << "Should respect max character limit even with large scale factors";
        EXPECT_GT(result.Length(), 0u) << "Should still produce some output";
    }
}
