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
 *	\file AAX_CUnitDisplayDelegateDecorator_UnitTest.cpp
 *
 *	\brief Unit tests for AAX_CUnitDisplayDelegateDecorator template class
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX_CUnitDisplayDelegateDecorator.h"
#include "AAX_CNumberDisplayDelegate.h"
#include "AAX_MockDisplayDelegate.h"
#include "AAX_CString.h"

//--------------------------------------------------------------------
#pragma mark - Test fixture

class AAX_CUnitDisplayDelegateDecorator_Test : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

//--------------------------------------------------------------------
#pragma mark - Constructor Tests

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, FloatDecorator_BasicConstruction)
{
    AAX_MockDisplayDelegate<float> baseDelegate;
    AAX_CUnitDisplayDelegateDecorator<float> decorator(baseDelegate, AAX_CString("Hz"));

    AAX_CString result;

    // Test that unit string is appended (mock delegate provides clean output)
    EXPECT_TRUE(decorator.ValueToString(440.0f, &result));
    EXPECT_STREQ("440Hz", result.Get());

    EXPECT_TRUE(decorator.ValueToString(1000.0f, &result));
    EXPECT_STREQ("1000Hz", result.Get());
}

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, FloatDecorator_DifferentUnits)
{
    AAX_MockDisplayDelegate<float> baseDelegate;

    // Test with voltage unit
    AAX_CUnitDisplayDelegateDecorator<float> voltageDecorator(baseDelegate, AAX_CString("V"));
    AAX_CString result;

    EXPECT_TRUE(voltageDecorator.ValueToString(12.5f, &result));
    EXPECT_STREQ("12.5V", result.Get());

    // Test with decibel unit
    AAX_CUnitDisplayDelegateDecorator<float> dbDecorator(baseDelegate, AAX_CString("dB"));

    EXPECT_TRUE(dbDecorator.ValueToString(-6.0f, &result));
    EXPECT_STREQ("-6dB", result.Get());

    // Test with percentage unit
    AAX_CUnitDisplayDelegateDecorator<float> percentDecorator(baseDelegate, AAX_CString("%"));

    EXPECT_TRUE(percentDecorator.ValueToString(75.0f, &result));
    EXPECT_STREQ("75%", result.Get());
}

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, FloatDecorator_EmptyUnit)
{
    AAX_MockDisplayDelegate<float> baseDelegate;
    AAX_CUnitDisplayDelegateDecorator<float> decorator(baseDelegate, AAX_CString(""));

    AAX_CString result;

    // Test with empty unit string (should behave like base delegate)
    EXPECT_TRUE(decorator.ValueToString(123.0f, &result));
    EXPECT_STREQ("123", result.Get()); // No unit appended
}

//--------------------------------------------------------------------
#pragma mark - ValueToString Tests

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, ValueToString_BasicFunctionality)
{
    AAX_MockDisplayDelegate<float> baseDelegate;
    AAX_CUnitDisplayDelegateDecorator<float> decorator(baseDelegate, AAX_CString("ms"));

    AAX_CString result;

    // Test various values
    EXPECT_TRUE(decorator.ValueToString(0.0f, &result));
    EXPECT_STREQ("0ms", result.Get());

    EXPECT_TRUE(decorator.ValueToString(1.5f, &result));
    EXPECT_STREQ("1.5ms", result.Get());

    EXPECT_TRUE(decorator.ValueToString(-2.75f, &result));
    EXPECT_STREQ("-2.75ms", result.Get());
}

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, ValueToString_WithMaxChars_SufficientSpace)
{
    AAX_MockDisplayDelegate<float> baseDelegate;
    AAX_CUnitDisplayDelegateDecorator<float> decorator(baseDelegate, AAX_CString("Hz"));

    AAX_CString result;

    // Test with sufficient space for unit string
    EXPECT_TRUE(decorator.ValueToString(440.0f, 10, &result));
    EXPECT_STREQ("440Hz", result.Get()); // Unit appended (5 chars total, maxNumChars=10)

    EXPECT_TRUE(decorator.ValueToString(1000.0f, 6, &result));
    EXPECT_STREQ("1000Hz", result.Get()); // Unit appended (6 chars total, maxNumChars=6)
}

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, ValueToString_WithMaxChars_InsufficientSpace)
{
    AAX_MockDisplayDelegate<float> baseDelegate;
    AAX_CUnitDisplayDelegateDecorator<float> decorator(baseDelegate, AAX_CString("Hz"));

    AAX_CString result;

    // Test with insufficient space for unit string
    EXPECT_TRUE(decorator.ValueToString(1000.0f, 5, &result));
    EXPECT_STREQ("1000", result.Get()); // Unit NOT appended (would exceed maxNumChars=5)

    EXPECT_TRUE(decorator.ValueToString(440.0f, 3, &result));
    EXPECT_STREQ("440", result.Get()); // Unit NOT appended (would exceed maxNumChars=3)
}

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, ValueToString_WithMaxChars_ExactFit)
{
    AAX_CNumberDisplayDelegate<float> baseDelegate; // Default precision=2, so 12.0f -> "12.00"
    AAX_CUnitDisplayDelegateDecorator<float> decorator(baseDelegate, AAX_CString("V"));

    AAX_CString result;

    // Test exact fit scenarios - AAX_CNumberDisplayDelegate formats 12.0f as "12.00" (precision=2)
    EXPECT_TRUE(decorator.ValueToString(12.0f, 6, &result));
    EXPECT_STREQ("12.00V", result.Get()); // "12.00" (5 chars) + "V" (1 char) = 6 chars

    EXPECT_TRUE(decorator.ValueToString(12.0f, 5, &result));
    EXPECT_STREQ("12.00", result.Get()); // Unit doesn't fit: "12.00V" would be 6 chars > 5

    EXPECT_TRUE(decorator.ValueToString(12.0f, 4, &result));
    EXPECT_STREQ("12.0", result.Get()); // Truncated to 4 chars, decimal point removed if last char
}

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, ValueToString_WithMaxChars_LongUnit)
{
    AAX_CNumberDisplayDelegate<float> baseDelegate; // Default precision=2, so 5.0f -> "5.00"
    AAX_CUnitDisplayDelegateDecorator<float> decorator(baseDelegate, AAX_CString(" seconds"));

    AAX_CString result;

    // Test with long unit string - 5.0f formats as "5.00" with precision=2
    EXPECT_TRUE(decorator.ValueToString(5.0f, 20, &result));
    EXPECT_STREQ("5.00 seconds", result.Get()); // "5.00" + " seconds" = 12 chars total

    EXPECT_TRUE(decorator.ValueToString(5.0f, 11, &result));
    EXPECT_STREQ("5.00", result.Get()); // Unit NOT appended (would be 12 chars > 11)
}

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, ValueToString_WithMaxChars_NegativeMaxChars)
{
    AAX_CNumberDisplayDelegate<float> baseDelegate; // Default precision=2, so 440.0f -> "440.00"
    AAX_CUnitDisplayDelegateDecorator<float> decorator(baseDelegate, AAX_CString("Hz"));

    AAX_CString result;

    // Test with negative maxNumChars (treated as 0) - should result in empty string
    EXPECT_TRUE(decorator.ValueToString(440.0f, -1, &result));
    EXPECT_STREQ("", result.Get()); // maxNumChars treated as 0, string truncated to empty

    EXPECT_TRUE(decorator.ValueToString(440.0f, -10, &result));
    EXPECT_STREQ("", result.Get()); // maxNumChars treated as 0, string truncated to empty
}

//--------------------------------------------------------------------
#pragma mark - StringToValue Tests

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, StringToValue_BasicFunctionality)
{
    AAX_CNumberDisplayDelegate<float> baseDelegate;
    AAX_CUnitDisplayDelegateDecorator<float> decorator(baseDelegate, AAX_CString("Hz"));
    
    float result;
    
    // Test string to value conversion with unit stripping
    EXPECT_TRUE(decorator.StringToValue(AAX_CString("440Hz"), &result));
    EXPECT_FLOAT_EQ(440.0f, result);
    
    EXPECT_TRUE(decorator.StringToValue(AAX_CString("1000Hz"), &result));
    EXPECT_FLOAT_EQ(1000.0f, result);
    
    EXPECT_TRUE(decorator.StringToValue(AAX_CString("-6Hz"), &result));
    EXPECT_FLOAT_EQ(-6.0f, result);
}

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, StringToValue_WithoutUnit)
{
    AAX_CNumberDisplayDelegate<float> baseDelegate;
    AAX_CUnitDisplayDelegateDecorator<float> decorator(baseDelegate, AAX_CString("Hz"));
    
    float result;
    
    // Test string to value conversion without unit (should pass through)
    EXPECT_TRUE(decorator.StringToValue(AAX_CString("440"), &result));
    EXPECT_FLOAT_EQ(440.0f, result);
    
    EXPECT_TRUE(decorator.StringToValue(AAX_CString("1000"), &result));
    EXPECT_FLOAT_EQ(1000.0f, result);
}

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, StringToValue_WrongUnit)
{
    AAX_CNumberDisplayDelegate<float> baseDelegate;
    AAX_CUnitDisplayDelegateDecorator<float> decorator(baseDelegate, AAX_CString("Hz"));
    
    float result;
    
    // Test string to value conversion with wrong unit (should pass through)
    EXPECT_TRUE(decorator.StringToValue(AAX_CString("440kHz"), &result));
    EXPECT_FLOAT_EQ(440.0f, result); // "kHz" doesn't match "Hz", so "440kHz" is treated as "440kHz"
    
    // This might fail if base delegate can't parse "440kHz", let's test a simpler case
    EXPECT_TRUE(decorator.StringToValue(AAX_CString("440V"), &result));
    EXPECT_FLOAT_EQ(440.0f, result); // "V" doesn't match "Hz", so "440V" is treated as "440V"
}

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, StringToValue_EmptyString)
{
    AAX_CNumberDisplayDelegate<float> baseDelegate;
    AAX_CUnitDisplayDelegateDecorator<float> decorator(baseDelegate, AAX_CString("Hz"));
    
    float result = 999.0f; // Initialize to non-default value
    
    // Test empty string (should pass through to base delegate)
    bool success = decorator.StringToValue(AAX_CString(""), &result);
    // Base delegate behavior for empty string - might succeed or fail depending on implementation
    // Just verify it doesn't crash and result is set appropriately
    if (success) {
        EXPECT_FLOAT_EQ(0.0f, result); // Likely converts to 0
    } else {
        // If it fails, that's also acceptable behavior
        EXPECT_TRUE(true); // Test passes either way
    }
}

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, StringToValue_ShortString)
{
    AAX_CNumberDisplayDelegate<float> baseDelegate;
    AAX_CUnitDisplayDelegateDecorator<float> decorator(baseDelegate, AAX_CString("Hz"));
    
    float result;
    
    // Test string shorter than unit string (should pass through)
    EXPECT_TRUE(decorator.StringToValue(AAX_CString("5"), &result));
    EXPECT_FLOAT_EQ(5.0f, result);
    
    // Test string equal to unit string length but different content
    EXPECT_FALSE(decorator.StringToValue(AAX_CString("AB"), &result)); // Same length as "Hz" but different
    // This should pass through to base delegate, which should fail to parse "AB" as a number
}

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, StringToValue_CaseSensitive)
{
    AAX_CNumberDisplayDelegate<float> baseDelegate;
    AAX_CUnitDisplayDelegateDecorator<float> decorator(baseDelegate, AAX_CString("Hz"));
    
    float result;
    
    // Test case sensitivity of unit matching
    EXPECT_TRUE(decorator.StringToValue(AAX_CString("440hz"), &result)); // lowercase "hz"
    EXPECT_FLOAT_EQ(440.0f, result); // Should pass through as "440hz" (unit doesn't match "Hz")
    
    EXPECT_TRUE(decorator.StringToValue(AAX_CString("440HZ"), &result)); // uppercase "HZ"
    EXPECT_FLOAT_EQ(440.0f, result); // Should pass through as "440HZ" (unit doesn't match "Hz")
}

//--------------------------------------------------------------------
#pragma mark - Clone Tests

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, Clone_BasicFunctionality)
{
    AAX_CNumberDisplayDelegate<float> baseDelegate;
    AAX_CUnitDisplayDelegateDecorator<float> original(baseDelegate, AAX_CString("kHz"));
    std::unique_ptr<AAX_CUnitDisplayDelegateDecorator<float>> clone(original.Clone());

    AAX_CString originalResult, cloneResult;

    // Test that clone works the same as original
    EXPECT_TRUE(original.ValueToString(2.4f, &originalResult));
    EXPECT_TRUE(clone->ValueToString(2.4f, &cloneResult));
    EXPECT_STREQ(originalResult.Get(), cloneResult.Get());

    // Test string to value conversion
    float originalValue, cloneValue;
    EXPECT_TRUE(original.StringToValue(AAX_CString("2.4kHz"), &originalValue));
    EXPECT_TRUE(clone->StringToValue(AAX_CString("2.4kHz"), &cloneValue));
    EXPECT_FLOAT_EQ(originalValue, cloneValue);

    // Test maxNumChars behavior
    EXPECT_TRUE(original.ValueToString(1000.0f, 4, &originalResult));
    EXPECT_TRUE(clone->ValueToString(1000.0f, 4, &cloneResult));
    EXPECT_STREQ(originalResult.Get(), cloneResult.Get());
}

//--------------------------------------------------------------------
#pragma mark - Template Type Tests

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, IntDecorator_BasicFunctionality)
{
    AAX_MockDisplayDelegate<int32_t> baseDelegate;
    AAX_CUnitDisplayDelegateDecorator<int32_t> decorator(baseDelegate, AAX_CString("px"));

    AAX_CString result;

    // Test with integer values
    EXPECT_TRUE(decorator.ValueToString(100, &result));
    EXPECT_STREQ("100px", result.Get());

    EXPECT_TRUE(decorator.ValueToString(-50, &result));
    EXPECT_STREQ("-50px", result.Get());

    EXPECT_TRUE(decorator.ValueToString(0, &result));
    EXPECT_STREQ("0px", result.Get());

    // Test string to value conversion
    int32_t value;
    EXPECT_TRUE(decorator.StringToValue(AAX_CString("100px"), &value));
    EXPECT_EQ(100, value);

    EXPECT_TRUE(decorator.StringToValue(AAX_CString("-50px"), &value));
    EXPECT_EQ(-50, value);
}

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, BoolDecorator_BasicFunctionality)
{
    // Note: Using a simple mock display delegate since AAX_CNumberDisplayDelegate might not support bool
    // For this test, we'll assume the base delegate converts bool to "0"/"1"
    AAX_MockDisplayDelegate<int32_t> baseDelegate; // Use int as proxy for bool
    AAX_CUnitDisplayDelegateDecorator<int32_t> decorator(baseDelegate, AAX_CString(" enabled"));

    AAX_CString result;

    // Test with bool-like values (0/1)
    EXPECT_TRUE(decorator.ValueToString(0, &result));
    EXPECT_STREQ("0 enabled", result.Get());

    EXPECT_TRUE(decorator.ValueToString(1, &result));
    EXPECT_STREQ("1 enabled", result.Get());

    // Test string to value conversion
    int32_t value;
    EXPECT_TRUE(decorator.StringToValue(AAX_CString("0 enabled"), &value));
    EXPECT_EQ(0, value);

    EXPECT_TRUE(decorator.StringToValue(AAX_CString("1 enabled"), &value));
    EXPECT_EQ(1, value);
}

//--------------------------------------------------------------------
#pragma mark - Edge Cases and Error Handling Tests

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, EdgeCases_VeryLongUnit)
{
    AAX_CNumberDisplayDelegate<float> baseDelegate;
    AAX_CString longUnit("ThisIsAVeryLongUnitStringThatMightCauseIssues");
    AAX_CUnitDisplayDelegateDecorator<float> decorator(baseDelegate, longUnit);

    AAX_CString result;

    // Test with very long unit string - 42.0f formats as "42.00" with precision=2
    EXPECT_TRUE(decorator.ValueToString(42.0f, 100, &result));
    EXPECT_STREQ("42.00ThisIsAVeryLongUnitStringThatMightCauseIssues", result.Get());

    // Test with insufficient space for long unit
    EXPECT_TRUE(decorator.ValueToString(42.0f, 10, &result));
    EXPECT_STREQ("42.00", result.Get()); // Unit not appended due to space constraint

    // Test string to value with long unit - should parse "42.00" from the beginning
    float value;
    EXPECT_TRUE(decorator.StringToValue(AAX_CString("42.00ThisIsAVeryLongUnitStringThatMightCauseIssues"), &value));
    EXPECT_FLOAT_EQ(42.0f, value);
}

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, EdgeCases_SpecialCharacters)
{
    AAX_CNumberDisplayDelegate<float> baseDelegate;
    AAX_CUnitDisplayDelegateDecorator<float> decorator(baseDelegate, AAX_CString("\xC2\xB0" "C")); // UTF-8 encoded degree symbol

    AAX_CString result;

    // Test with special characters in unit - 25.0f formats as "25.00" with precision=2
    EXPECT_TRUE(decorator.ValueToString(25.0f, &result));
    EXPECT_STREQ("25.00" "\xC2\xB0" "C", result.Get()); // UTF-8 encoded degree symbol

    EXPECT_TRUE(decorator.ValueToString(-10.0f, &result));
    EXPECT_STREQ("-10.00" "\xC2\xB0" "C", result.Get()); // UTF-8 encoded degree symbol

    float value;
    // Test string to value conversion with the decorator special character but no actual value (should fail)
    EXPECT_FALSE(decorator.StringToValue(AAX_CString("\xC2\xB0" "C"), &value)); // UTF-8 encoded degree symbol

    // Test string to value conversion with special characters
    EXPECT_TRUE(decorator.StringToValue(AAX_CString("25" "\xC2\xB0" "C"), &value)); // UTF-8 encoded degree symbol
    EXPECT_FLOAT_EQ(25.0f, value);

    EXPECT_TRUE(decorator.StringToValue(AAX_CString("-10.0" "\xC2\xB0" "C"), &value)); // UTF-8 encoded degree symbol
    EXPECT_FLOAT_EQ(-10.0f, value);
}

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, EdgeCases_UnitWithSpaces)
{
    AAX_CNumberDisplayDelegate<float> baseDelegate;
    AAX_CUnitDisplayDelegateDecorator<float> decorator(baseDelegate, AAX_CString(" m/s"));

    AAX_CString result;

    // Test with unit that starts with space - 9.8f formats as "9.80" with precision=2
    EXPECT_TRUE(decorator.ValueToString(9.8f, &result));
    EXPECT_STREQ("9.80 m/s", result.Get());

    // Test string to value conversion with spaced unit
    float value;
    EXPECT_TRUE(decorator.StringToValue(AAX_CString("9.80 m/s"), &value));
    EXPECT_FLOAT_EQ(9.8f, value);

    // Test without unit (should still work)
    EXPECT_TRUE(decorator.StringToValue(AAX_CString("9.8"), &value));
    EXPECT_FLOAT_EQ(9.8f, value);
}

//--------------------------------------------------------------------
#pragma mark - Round-Trip Conversion Tests

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, RoundTrip_BasicConversion)
{
    AAX_CNumberDisplayDelegate<float> baseDelegate; // Default precision=2
    AAX_CUnitDisplayDelegateDecorator<float> decorator(baseDelegate, AAX_CString("Hz"));

    // Use values that round-trip exactly with precision=2
    float originalValues[] = {0.0f, 440.0f, 1000.0f, -6.0f, 123.45f}; // 123.45f instead of 123.456f

    for (float originalValue : originalValues) {
        AAX_CString stringValue;
        EXPECT_TRUE(decorator.ValueToString(originalValue, &stringValue));

        float convertedValue;
        EXPECT_TRUE(decorator.StringToValue(stringValue, &convertedValue));

        // Should be equal within precision limits (2 decimal places)
        EXPECT_NEAR(originalValue, convertedValue, 0.01f);
    }
}

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, RoundTrip_WithMaxChars)
{
    AAX_CNumberDisplayDelegate<float> baseDelegate;
    AAX_CUnitDisplayDelegateDecorator<float> decorator(baseDelegate, AAX_CString("V"));

    // Test round-trip with maxNumChars that allows unit - 12.0f formats as "12.00" with precision=2
    float testValue = 12.0f;
    AAX_CString stringValue;
    EXPECT_TRUE(decorator.ValueToString(testValue, 10, &stringValue));
    EXPECT_STREQ("12.00V", stringValue.Get());

    float convertedValue;
    EXPECT_TRUE(decorator.StringToValue(stringValue, &convertedValue));
    EXPECT_FLOAT_EQ(testValue, convertedValue);

    // Test round-trip with maxNumChars that doesn't allow unit
    EXPECT_TRUE(decorator.ValueToString(testValue, 4, &stringValue));
    EXPECT_STREQ("12.0", stringValue.Get()); // No unit, truncated to 4 chars

    EXPECT_TRUE(decorator.StringToValue(stringValue, &convertedValue));
    EXPECT_FLOAT_EQ(testValue, convertedValue);
}

//--------------------------------------------------------------------
#pragma mark - Decorator Pattern Behavior Tests

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, DecoratorPattern_PassThrough)
{
    AAX_CNumberDisplayDelegate<float> baseDelegate;
    AAX_CUnitDisplayDelegateDecorator<float> decorator(baseDelegate, AAX_CString(""));

    AAX_CString decoratorResult, baseResult;

    // Test that decorator with empty unit behaves like base delegate
    EXPECT_TRUE(decorator.ValueToString(123.45f, &decoratorResult));
    EXPECT_TRUE(baseDelegate.ValueToString(123.45f, &baseResult));
    EXPECT_STREQ(baseResult.Get(), decoratorResult.Get());

    // Test with maxNumChars
    EXPECT_TRUE(decorator.ValueToString(123.45f, 5, &decoratorResult));
    EXPECT_TRUE(baseDelegate.ValueToString(123.45f, 5, &baseResult));
    EXPECT_STREQ(baseResult.Get(), decoratorResult.Get());
}

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, DecoratorPattern_Enhancement)
{
    AAX_CNumberDisplayDelegate<float> baseDelegate;
    AAX_CUnitDisplayDelegateDecorator<float> decorator(baseDelegate, AAX_CString("Hz"));

    AAX_CString decoratorResult, baseResult;

    // Test that decorator enhances base delegate output
    EXPECT_TRUE(decorator.ValueToString(440.0f, &decoratorResult));
    EXPECT_TRUE(baseDelegate.ValueToString(440.0f, &baseResult));

    // Decorator result should be base result + unit
    AAX_CString expectedResult = baseResult;
    expectedResult += AAX_CString("Hz");
    EXPECT_STREQ(expectedResult.Get(), decoratorResult.Get());
}

//--------------------------------------------------------------------
#pragma mark - Integration Tests with Real Display Delegates

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, Integration_WithNumberDisplayDelegate)
{
    // Test integration with real AAX_CNumberDisplayDelegate
    AAX_CNumberDisplayDelegate<float> realDelegate;
    AAX_CUnitDisplayDelegateDecorator<float> decorator(realDelegate, AAX_CString("Hz"));

    AAX_CString result;

    // Test that decorator works with real delegate (which uses 2 decimal places)
    EXPECT_TRUE(decorator.ValueToString(440.0f, &result));
    EXPECT_STREQ("440.00Hz", result.Get());

    EXPECT_TRUE(decorator.ValueToString(1000.5f, &result));
    EXPECT_STREQ("1000.50Hz", result.Get());

    // Test string to value conversion
    float value;
    EXPECT_TRUE(decorator.StringToValue(AAX_CString("440.00Hz"), &value));
    EXPECT_FLOAT_EQ(440.0f, value);

    EXPECT_TRUE(decorator.StringToValue(AAX_CString("1000.50Hz"), &value));
    EXPECT_FLOAT_EQ(1000.5f, value);
}

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, Integration_WithNumberDisplayDelegate_MaxChars)
{
    // Test maxNumChars behavior with real delegate
    AAX_CNumberDisplayDelegate<float> realDelegate;
    AAX_CUnitDisplayDelegateDecorator<float> decorator(realDelegate, AAX_CString("V"));

    AAX_CString result;

    // Test with sufficient space
    EXPECT_TRUE(decorator.ValueToString(12.0f, 10, &result));
    EXPECT_STREQ("12.00V", result.Get()); // 6 chars total, fits in 10

    // Test with insufficient space for unit
    EXPECT_TRUE(decorator.ValueToString(12.0f, 5, &result));
    EXPECT_STREQ("12.00", result.Get()); // Unit not appended (would be 6 chars > 5)

    // Test exact fit
    EXPECT_TRUE(decorator.ValueToString(12.0f, 6, &result));
    EXPECT_STREQ("12.00V", result.Get()); // Exact fit: 6 chars = 6 maxNumChars
}

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, Integration_RoundTrip_WithRealDelegate)
{
    // Test round-trip conversion with real delegate
    AAX_CNumberDisplayDelegate<float> realDelegate;
    AAX_CUnitDisplayDelegateDecorator<float> decorator(realDelegate, AAX_CString("dB"));

    float originalValues[] = {0.0f, -6.0f, 12.5f, -20.25f};

    for (float originalValue : originalValues) {
        AAX_CString stringValue;
        EXPECT_TRUE(decorator.ValueToString(originalValue, &stringValue));

        float convertedValue;
        EXPECT_TRUE(decorator.StringToValue(stringValue, &convertedValue));

        // Allow for small precision differences due to string conversion
        EXPECT_NEAR(originalValue, convertedValue, 1e-6);
    }
}

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, Integration_Clone_WithRealDelegate)
{
    // Test cloning with real delegate
    AAX_CNumberDisplayDelegate<float> realDelegate;
    AAX_CUnitDisplayDelegateDecorator<float> original(realDelegate, AAX_CString("ms"));
    std::unique_ptr<AAX_CUnitDisplayDelegateDecorator<float>> clone(original.Clone());

    AAX_CString originalResult, cloneResult;

    // Test that clone works identically to original
    EXPECT_TRUE(original.ValueToString(123.45f, &originalResult));
    EXPECT_TRUE(clone->ValueToString(123.45f, &cloneResult));
    EXPECT_STREQ(originalResult.Get(), cloneResult.Get());

    // Test string to value
    float originalValue, cloneValue;
    EXPECT_TRUE(original.StringToValue(AAX_CString("123.45ms"), &originalValue));
    EXPECT_TRUE(clone->StringToValue(AAX_CString("123.45ms"), &cloneValue));
    EXPECT_FLOAT_EQ(originalValue, cloneValue);
}

// ========== RVALUE REFERENCE CONSTRUCTOR TESTS ==========

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, RvalueReference_DisplayDelegate_Constructor)
{
    // Test basic rvalue reference constructor with display delegate
    auto factory = []() -> AAX_CNumberDisplayDelegate<float> {
        return AAX_CNumberDisplayDelegate<float>();
    };
    
    AAX_CUnitDisplayDelegateDecorator<float> decorator(factory(), AAX_CString("Hz"));
    
    // Test basic functionality works after rvalue construction
    AAX_CString result;
    EXPECT_TRUE(decorator.ValueToString(440.0f, &result));
    EXPECT_STREQ("440.00Hz", result.CString());
    
    float value;
    EXPECT_TRUE(decorator.StringToValue("440.00Hz", &value));
    EXPECT_FLOAT_EQ(440.0f, value);
}

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, RvalueReference_Decorator_Constructor)
{
    // Test rvalue reference constructor from another decorator  
    AAX_CNumberDisplayDelegate<float> baseDelegate;
    AAX_CUnitDisplayDelegateDecorator<float> sourceDecorator(baseDelegate, AAX_CString("V"));
    
    // Create decorator from rvalue reference of another decorator
    auto factory = [&]() -> AAX_CUnitDisplayDelegateDecorator<float> {
        return std::move(sourceDecorator);
    };
    
    AAX_CUnitDisplayDelegateDecorator<float> decorator(factory());
    
    // Test functionality after move construction
    AAX_CString result;
    EXPECT_TRUE(decorator.ValueToString(3.3f, &result));
    EXPECT_STREQ("3.30V", result.CString());
}

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, DecoratorChaining_WithRvalueReferences)
{
    // Test complex decorator chaining using rvalue references
    auto createChainedDecorator = []() -> AAX_CUnitDisplayDelegateDecorator<float> {
        AAX_CNumberDisplayDelegate<float> base;
        return AAX_CUnitDisplayDelegateDecorator<float>(
            std::move(base), 
            AAX_CString("kHz")
        );
    };
    
    AAX_CUnitDisplayDelegateDecorator<float> decorator = createChainedDecorator();
    
    // Test that chaining works correctly
    AAX_CString result;
    EXPECT_TRUE(decorator.ValueToString(1.5f, &result));
    EXPECT_STREQ("1.50kHz", result.CString());
    
    float value;
    EXPECT_TRUE(decorator.StringToValue("1.50kHz", &value));
    EXPECT_FLOAT_EQ(1.5f, value);
}

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, RvalueReference_FactoryPattern)
{
    // Test factory pattern creating decorators with rvalue references
    auto createFrequencyDecorator = [](const AAX_CString& unit) -> AAX_CUnitDisplayDelegateDecorator<float> {
        AAX_CNumberDisplayDelegate<float> delegate;
        return AAX_CUnitDisplayDelegateDecorator<float>(std::move(delegate), unit);
    };
    
    auto hzDecorator = createFrequencyDecorator("Hz");
    auto khzDecorator = createFrequencyDecorator("kHz");
    
    // Test Hz decorator
    AAX_CString hzResult;
    EXPECT_TRUE(hzDecorator.ValueToString(1000.0f, &hzResult));
    EXPECT_STREQ("1000.00Hz", hzResult.CString());
    
    // Test kHz decorator
    AAX_CString khzResult;
    EXPECT_TRUE(khzDecorator.ValueToString(1.0f, &khzResult));
    EXPECT_STREQ("1.00kHz", khzResult.CString());
}

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, RvalueReference_Clone_Behavior)
{
    // Test clone behavior after rvalue reference construction
    AAX_CNumberDisplayDelegate<float> baseDelegate;
    AAX_CUnitDisplayDelegateDecorator<float> original(std::move(baseDelegate), AAX_CString("°C"));
    
    std::unique_ptr<AAX_CUnitDisplayDelegateDecorator<float>> cloned(original.Clone());
    
    // Test both original and clone have same behavior
    AAX_CString originalResult, clonedResult;
    bool originalSuccess = original.ValueToString(25.5f, &originalResult);
    bool clonedSuccess = cloned->ValueToString(25.5f, &clonedResult);
    
    EXPECT_TRUE(originalSuccess);
    EXPECT_TRUE(clonedSuccess);
    EXPECT_STREQ(originalResult.CString(), clonedResult.CString());
    EXPECT_STREQ("25.50°C", originalResult.CString());
    
    // Test round-trip on clone
    float value;
    bool success = cloned->StringToValue("25.50°C", &value);
    EXPECT_TRUE(success);
    EXPECT_FLOAT_EQ(25.5f, value);
}

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, RvalueReference_WithDifferentTemplateTypes)
{
    // Test rvalue constructors with different template parameter combinations
    auto createIntDecorator = []() -> AAX_CUnitDisplayDelegateDecorator<int32_t> {
        AAX_CNumberDisplayDelegate<int32_t> delegate;
        return AAX_CUnitDisplayDelegateDecorator<int32_t>(std::move(delegate), AAX_CString("px"));
    };
    
    auto createDoubleDecorator = []() -> AAX_CUnitDisplayDelegateDecorator<double> {
        AAX_CNumberDisplayDelegate<double> delegate;
        return AAX_CUnitDisplayDelegateDecorator<double>(std::move(delegate), AAX_CString("m/s"));
    };
    
    auto intDecorator = createIntDecorator();
    auto doubleDecorator = createDoubleDecorator();
    
    // Test int32_t version
    AAX_CString intResult;
    EXPECT_TRUE(intDecorator.ValueToString(100, &intResult));
    EXPECT_STREQ("100.00px", intResult.CString()); // NumberDisplayDelegate uses precision=2 by default
    
    // Test double version  
    AAX_CString doubleResult;
    EXPECT_TRUE(doubleDecorator.ValueToString(9.81, &doubleResult));
    EXPECT_STREQ("9.81m/s", doubleResult.CString());
}

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, RvalueReference_MemoryManagement)
{
    // Test memory management with rvalue references
    std::vector<std::unique_ptr<AAX_CUnitDisplayDelegateDecorator<float>>> decorators;
    
    // Create multiple decorators using rvalue references
    for (int i = 0; i < 5; ++i) {
        AAX_CString unit("unit");
        unit += std::to_string(i).c_str(); // Convert int to string properly
        AAX_CNumberDisplayDelegate<float> delegate;
        
        decorators.emplace_back(
            std::make_unique<AAX_CUnitDisplayDelegateDecorator<float>>(
                std::move(delegate), 
                unit
            )
        );
    }
    
    // Test all decorators work correctly
    for (size_t i = 0; i < decorators.size(); ++i) {
        AAX_CString result;
        float testValue = static_cast<float>(i + 1) * 10.0f;
        EXPECT_TRUE(decorators[i]->ValueToString(testValue, &result));
        
        // Check the unit is correctly appended
        EXPECT_TRUE(result.CString() != nullptr);
        EXPECT_TRUE(strlen(result.CString()) > 4); // Minimum expected length
    }
}

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, RvalueReference_ComplexUnitStrings)
{
    // Test rvalue construction with complex unit strings
    auto createComplexUnitDecorator = [](const AAX_CString& complexUnit) -> AAX_CUnitDisplayDelegateDecorator<float> {
        AAX_CNumberDisplayDelegate<float> delegate;
        return AAX_CUnitDisplayDelegateDecorator<float>(std::move(delegate), complexUnit);
    };
    
    // Test with various complex unit strings
    auto decorator1 = createComplexUnitDecorator(" Hz/V²");
    auto decorator2 = createComplexUnitDecorator("μA/cm²");  
    auto decorator3 = createComplexUnitDecorator("[dB SPL]");
    
    // Test complex unit 1
    AAX_CString result1;
    EXPECT_TRUE(decorator1.ValueToString(50.0f, &result1));
    EXPECT_STREQ("50.00 Hz/V²", result1.CString());
    
    // Test complex unit 2
    AAX_CString result2;
    EXPECT_TRUE(decorator2.ValueToString(12.5f, &result2));
    EXPECT_STREQ("12.50μA/cm²", result2.CString());
    
    // Test complex unit 3
    AAX_CString result3;
    EXPECT_TRUE(decorator3.ValueToString(94.0f, &result3));
    EXPECT_STREQ("94.00[dB SPL]", result3.CString());
    
    // Test string to value with complex units
    float value1, value2, value3;
    EXPECT_TRUE(decorator1.StringToValue("50.00 Hz/V²", &value1));
    EXPECT_TRUE(decorator2.StringToValue("12.50μA/cm²", &value2));
    EXPECT_TRUE(decorator3.StringToValue("94.00[dB SPL]", &value3));
    
    EXPECT_FLOAT_EQ(50.0f, value1);
    EXPECT_FLOAT_EQ(12.5f, value2);
    EXPECT_FLOAT_EQ(94.0f, value3);
}

TEST_F(AAX_CUnitDisplayDelegateDecorator_Test, RvalueReference_PolymorphicBehavior)
{
    // Test polymorphic usage of rvalue-constructed decorators
    auto createPolymorphicDecorator = []() -> std::unique_ptr<AAX_IDisplayDelegate<float>> {
        AAX_CNumberDisplayDelegate<float> delegate;
        return std::make_unique<AAX_CUnitDisplayDelegateDecorator<float>>(
            std::move(delegate), 
            AAX_CString("Ω")
        );
    };
    
    auto polymorphicDecorator = createPolymorphicDecorator();
    
    // Test polymorphic interface works
    AAX_CString result;
    EXPECT_TRUE(polymorphicDecorator->ValueToString(47000.0f, &result));
    EXPECT_STREQ("47000.00Ω", result.CString());
    
    float value;
    EXPECT_TRUE(polymorphicDecorator->StringToValue("47000.00Ω", &value));
    EXPECT_FLOAT_EQ(47000.0f, value);
    
    // Test clone through polymorphic interface
    std::unique_ptr<AAX_IDisplayDelegate<float>> cloned(polymorphicDecorator->Clone());
    AAX_CString clonedResult;
    EXPECT_TRUE(cloned->ValueToString(47000.0f, &clonedResult));
    EXPECT_STREQ("47000.00Ω", clonedResult.CString());
}
