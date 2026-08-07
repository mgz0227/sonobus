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
 *	\file AAX_CStringDisplayDelegate_UnitTest.cpp
 *
 *	\brief Unit tests for AAX_CStringDisplayDelegate template class
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX_CStringDisplayDelegate.h"
#include <map>

//--------------------------------------------------------------------
#pragma mark - Test fixture

class AAX_CStringDisplayDelegate_Test : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

//--------------------------------------------------------------------
#pragma mark - Constructor Tests

TEST_F(AAX_CStringDisplayDelegate_Test, IntDisplay_BasicConstruction)
{
    std::map<int32_t, AAX_CString> stringMap;
    stringMap[0] = AAX_CString("Zero");
    stringMap[1] = AAX_CString("One");
    stringMap[2] = AAX_CString("Two");
    
    AAX_CStringDisplayDelegate<int32_t> display(stringMap);
    
    AAX_CString result;
    
    // Test basic value to string mapping
    EXPECT_TRUE(display.ValueToString(0, &result));
    EXPECT_STREQ("Zero", result.Get());
    
    EXPECT_TRUE(display.ValueToString(1, &result));
    EXPECT_STREQ("One", result.Get());
    
    EXPECT_TRUE(display.ValueToString(2, &result));
    EXPECT_STREQ("Two", result.Get());
    
    // Test unmapped value
    EXPECT_FALSE(display.ValueToString(3, &result));
    EXPECT_STREQ("String Not Found", result.Get());
}

TEST_F(AAX_CStringDisplayDelegate_Test, IntDisplay_EmptyMap)
{
    std::map<int32_t, AAX_CString> emptyMap;
    AAX_CStringDisplayDelegate<int32_t> display(emptyMap);
    
    AAX_CString result;
    
    // Test with empty map
    EXPECT_FALSE(display.ValueToString(0, &result));
    EXPECT_STREQ("String Not Found", result.Get());
    
    EXPECT_FALSE(display.ValueToString(1, &result));
    EXPECT_STREQ("String Not Found", result.Get());
}

TEST_F(AAX_CStringDisplayDelegate_Test, IntDisplay_NonSequentialKeys)
{
    std::map<int32_t, AAX_CString> stringMap;
    stringMap[10] = AAX_CString("Ten");
    stringMap[100] = AAX_CString("Hundred");
    stringMap[-5] = AAX_CString("Negative Five");
    
    AAX_CStringDisplayDelegate<int32_t> display(stringMap);
    
    AAX_CString result;
    
    // Test non-sequential key mapping
    EXPECT_TRUE(display.ValueToString(10, &result));
    EXPECT_STREQ("Ten", result.Get());
    
    EXPECT_TRUE(display.ValueToString(100, &result));
    EXPECT_STREQ("Hundred", result.Get());
    
    EXPECT_TRUE(display.ValueToString(-5, &result));
    EXPECT_STREQ("Negative Five", result.Get());
    
    // Test unmapped values
    EXPECT_FALSE(display.ValueToString(0, &result));
    EXPECT_STREQ("String Not Found", result.Get());
    
    EXPECT_FALSE(display.ValueToString(50, &result));
    EXPECT_STREQ("String Not Found", result.Get());
}

//--------------------------------------------------------------------
#pragma mark - ValueToString Tests

TEST_F(AAX_CStringDisplayDelegate_Test, ValueToString_BasicFunctionality)
{
    std::map<int32_t, AAX_CString> stringMap;
    stringMap[1] = AAX_CString("First");
    stringMap[2] = AAX_CString("Second");
    stringMap[3] = AAX_CString("Third");
    
    AAX_CStringDisplayDelegate<int32_t> display(stringMap);
    
    AAX_CString result;
    
    // Test valid mappings
    EXPECT_TRUE(display.ValueToString(1, &result));
    EXPECT_STREQ("First", result.Get());
    
    EXPECT_TRUE(display.ValueToString(2, &result));
    EXPECT_STREQ("Second", result.Get());
    
    EXPECT_TRUE(display.ValueToString(3, &result));
    EXPECT_STREQ("Third", result.Get());
    
    // Test invalid mapping
    EXPECT_FALSE(display.ValueToString(4, &result));
    EXPECT_STREQ("String Not Found", result.Get());
}

TEST_F(AAX_CStringDisplayDelegate_Test, ValueToString_WithMaxChars)
{
    std::map<int32_t, AAX_CString> stringMap;
    stringMap[1] = AAX_CString("Short");
    stringMap[2] = AAX_CString("VeryLongStringThatExceedsLimit");
    
    AAX_CStringDisplayDelegate<int32_t> display(stringMap);
    
    AAX_CString result;
    
    // Test maxNumChars truncation
    EXPECT_TRUE(display.ValueToString(1, 3, &result));
    EXPECT_STREQ("Sho", result.Get()); // String truncated to 3 characters
    
    EXPECT_TRUE(display.ValueToString(2, 10, &result));
    EXPECT_STREQ("VeryLongSt", result.Get()); // String truncated to 10 characters
    
    // Test invalid mapping with maxNumChars
    EXPECT_FALSE(display.ValueToString(3, 5, &result));
    EXPECT_STREQ("String Not Found", result.Get());
}

//--------------------------------------------------------------------
#pragma mark - StringToValue Tests

TEST_F(AAX_CStringDisplayDelegate_Test, StringToValue_BasicFunctionality)
{
    std::map<int32_t, AAX_CString> stringMap;
    stringMap[10] = AAX_CString("Alpha");
    stringMap[20] = AAX_CString("Beta");
    stringMap[30] = AAX_CString("Gamma");
    
    AAX_CStringDisplayDelegate<int32_t> display(stringMap);
    
    int32_t result;
    
    // Test valid string to value conversions
    EXPECT_TRUE(display.StringToValue(AAX_CString("Alpha"), &result));
    EXPECT_EQ(10, result);
    
    EXPECT_TRUE(display.StringToValue(AAX_CString("Beta"), &result));
    EXPECT_EQ(20, result);
    
    EXPECT_TRUE(display.StringToValue(AAX_CString("Gamma"), &result));
    EXPECT_EQ(30, result);
}

TEST_F(AAX_CStringDisplayDelegate_Test, StringToValue_InvalidStrings)
{
    std::map<int32_t, AAX_CString> stringMap;
    stringMap[1] = AAX_CString("Valid");
    stringMap[2] = AAX_CString("Another");
    
    AAX_CStringDisplayDelegate<int32_t> display(stringMap);
    
    int32_t result = 999; // Initialize to non-default value
    
    // Test invalid string conversions
    EXPECT_FALSE(display.StringToValue(AAX_CString("Invalid"), &result));
    EXPECT_EQ(0, result); // Should be set to 0 on failure (line 140)
    
    result = 999; // Reset
    EXPECT_FALSE(display.StringToValue(AAX_CString(""), &result));
    EXPECT_EQ(0, result); // Should be set to 0 on failure
    
    result = 999; // Reset
    EXPECT_FALSE(display.StringToValue(AAX_CString("valid"), &result)); // Case sensitive
    EXPECT_EQ(0, result); // Should be set to 0 on failure
}

TEST_F(AAX_CStringDisplayDelegate_Test, StringToValue_CaseSensitive)
{
    std::map<int32_t, AAX_CString> stringMap;
    stringMap[1] = AAX_CString("Lower");
    stringMap[2] = AAX_CString("UPPER");
    stringMap[3] = AAX_CString("MiXeD");
    
    AAX_CStringDisplayDelegate<int32_t> display(stringMap);
    
    int32_t result;
    
    // Test case sensitivity
    EXPECT_TRUE(display.StringToValue(AAX_CString("Lower"), &result));
    EXPECT_EQ(1, result);
    
    EXPECT_TRUE(display.StringToValue(AAX_CString("UPPER"), &result));
    EXPECT_EQ(2, result);
    
    EXPECT_TRUE(display.StringToValue(AAX_CString("MiXeD"), &result));
    EXPECT_EQ(3, result);
    
    // Test that case variations don't match
    EXPECT_FALSE(display.StringToValue(AAX_CString("lower"), &result));
    EXPECT_FALSE(display.StringToValue(AAX_CString("upper"), &result));
    EXPECT_FALSE(display.StringToValue(AAX_CString("mixed"), &result));
}

//--------------------------------------------------------------------
#pragma mark - Clone Tests

TEST_F(AAX_CStringDisplayDelegate_Test, Clone_BasicFunctionality)
{
    std::map<int32_t, AAX_CString> stringMap;
    stringMap[5] = AAX_CString("Five");
    stringMap[10] = AAX_CString("Ten");

    AAX_CStringDisplayDelegate<int32_t> original(stringMap);
    std::unique_ptr<AAX_CStringDisplayDelegate<int32_t>> clone(original.Clone());

    AAX_CString originalResult, cloneResult;

    // Test that clone works the same as original
    EXPECT_TRUE(original.ValueToString(5, &originalResult));
    EXPECT_TRUE(clone->ValueToString(5, &cloneResult));
    EXPECT_STREQ(originalResult.Get(), cloneResult.Get());

    EXPECT_TRUE(original.ValueToString(10, &originalResult));
    EXPECT_TRUE(clone->ValueToString(10, &cloneResult));
    EXPECT_STREQ(originalResult.Get(), cloneResult.Get());

    // Test string to value conversion
    int32_t originalValue, cloneValue;
    EXPECT_TRUE(original.StringToValue(AAX_CString("Five"), &originalValue));
    EXPECT_TRUE(clone->StringToValue(AAX_CString("Five"), &cloneValue));
    EXPECT_EQ(originalValue, cloneValue);

    // Test that unmapped values behave the same
    EXPECT_FALSE(original.ValueToString(99, &originalResult));
    EXPECT_FALSE(clone->ValueToString(99, &cloneResult));
    EXPECT_STREQ(originalResult.Get(), cloneResult.Get()); // Both should be "String Not Found"
}

//--------------------------------------------------------------------
#pragma mark - Template Type Tests

TEST_F(AAX_CStringDisplayDelegate_Test, FloatDisplay_BasicFunctionality)
{
    std::map<float, AAX_CString> stringMap;
    stringMap[0.0f] = AAX_CString("Zero");
    stringMap[0.5f] = AAX_CString("Half");
    stringMap[1.0f] = AAX_CString("One");

    AAX_CStringDisplayDelegate<float> display(stringMap);

    AAX_CString result;

    // Test with float values
    EXPECT_TRUE(display.ValueToString(0.0f, &result));
    EXPECT_STREQ("Zero", result.Get());

    EXPECT_TRUE(display.ValueToString(0.5f, &result));
    EXPECT_STREQ("Half", result.Get());

    EXPECT_TRUE(display.ValueToString(1.0f, &result));
    EXPECT_STREQ("One", result.Get());

    // Test string to value conversion
    float value;
    EXPECT_TRUE(display.StringToValue(AAX_CString("Zero"), &value));
    EXPECT_FLOAT_EQ(0.0f, value);

    EXPECT_TRUE(display.StringToValue(AAX_CString("Half"), &value));
    EXPECT_FLOAT_EQ(0.5f, value);

    EXPECT_TRUE(display.StringToValue(AAX_CString("One"), &value));
    EXPECT_FLOAT_EQ(1.0f, value);

    // Test unmapped float value
    EXPECT_FALSE(display.ValueToString(0.25f, &result));
    EXPECT_STREQ("String Not Found", result.Get());
}

TEST_F(AAX_CStringDisplayDelegate_Test, BoolDisplay_BasicFunctionality)
{
    std::map<bool, AAX_CString> stringMap;
    stringMap[false] = AAX_CString("False");
    stringMap[true] = AAX_CString("True");

    AAX_CStringDisplayDelegate<bool> display(stringMap);

    AAX_CString result;

    // Test with bool values
    EXPECT_TRUE(display.ValueToString(false, &result));
    EXPECT_STREQ("False", result.Get());

    EXPECT_TRUE(display.ValueToString(true, &result));
    EXPECT_STREQ("True", result.Get());

    // Test string to value conversion
    bool value;
    EXPECT_TRUE(display.StringToValue(AAX_CString("False"), &value));
    EXPECT_FALSE(value);

    EXPECT_TRUE(display.StringToValue(AAX_CString("True"), &value));
    EXPECT_TRUE(value);
}

//--------------------------------------------------------------------
#pragma mark - Edge Cases and Error Handling Tests

TEST_F(AAX_CStringDisplayDelegate_Test, EdgeCases_DuplicateStrings)
{
    std::map<int32_t, AAX_CString> stringMap;
    stringMap[1] = AAX_CString("Duplicate");
    stringMap[2] = AAX_CString("Duplicate"); // Same string, different key
    stringMap[3] = AAX_CString("Unique");

    AAX_CStringDisplayDelegate<int32_t> display(stringMap);

    AAX_CString result;
    int32_t value;

    // Test value to string (should work for both keys)
    EXPECT_TRUE(display.ValueToString(1, &result));
    EXPECT_STREQ("Duplicate", result.Get());

    EXPECT_TRUE(display.ValueToString(2, &result));
    EXPECT_STREQ("Duplicate", result.Get());

    // NOTE: EDGE CASE WITH UNUSUAL BEHAVIOR
    // String to value with duplicate strings - the inverse map stores one key per string
    // The inverse map insert() doesn't overwrite existing keys, so the first key encountered (key 1) wins
    EXPECT_TRUE(display.StringToValue(AAX_CString("Duplicate"), &value));
    EXPECT_EQ(1, value); // Should return key 1 (the first key inserted for this string)

    // Unique string should work normally
    EXPECT_TRUE(display.StringToValue(AAX_CString("Unique"), &value));
    EXPECT_EQ(3, value);
}

TEST_F(AAX_CStringDisplayDelegate_Test, EdgeCases_EmptyStrings)
{
    std::map<int32_t, AAX_CString> stringMap;
    stringMap[0] = AAX_CString(""); // Empty string
    stringMap[1] = AAX_CString("NonEmpty");

    AAX_CStringDisplayDelegate<int32_t> display(stringMap);

    AAX_CString result;

    // Test with empty string
    EXPECT_TRUE(display.ValueToString(0, &result));
    EXPECT_STREQ("", result.Get());
    EXPECT_EQ(0U, result.Length());

    EXPECT_TRUE(display.ValueToString(1, &result));
    EXPECT_STREQ("NonEmpty", result.Get());

    // Test string to value conversion with empty string
    int32_t value;
    EXPECT_TRUE(display.StringToValue(AAX_CString(""), &value));
    EXPECT_EQ(0, value);

    EXPECT_TRUE(display.StringToValue(AAX_CString("NonEmpty"), &value));
    EXPECT_EQ(1, value);
}

TEST_F(AAX_CStringDisplayDelegate_Test, EdgeCases_LongStrings)
{
    std::map<int32_t, AAX_CString> stringMap;
    AAX_CString longString("This is a very long string that tests the display delegate's ability to handle lengthy text without any issues or truncation problems");
    stringMap[1] = longString;
    stringMap[2] = AAX_CString("Short");

    AAX_CStringDisplayDelegate<int32_t> display(stringMap);

    AAX_CString result;

    // Test with long string
    EXPECT_TRUE(display.ValueToString(1, &result));
    EXPECT_STREQ(longString.Get(), result.Get());

    EXPECT_TRUE(display.ValueToString(2, &result));
    EXPECT_STREQ("Short", result.Get());

    // Test string to value conversion with long string
    int32_t value;
    EXPECT_TRUE(display.StringToValue(longString, &value));
    EXPECT_EQ(1, value);

    EXPECT_TRUE(display.StringToValue(AAX_CString("Short"), &value));
    EXPECT_EQ(2, value);
}

TEST_F(AAX_CStringDisplayDelegate_Test, EdgeCases_SpecialCharacters)
{
    std::map<int32_t, AAX_CString> stringMap;
    stringMap[1] = AAX_CString("Spëcîál Chärs");
    stringMap[2] = AAX_CString("Symbols: !@#$%^&*()");
    stringMap[3] = AAX_CString("Numbers: 123456789");

    AAX_CStringDisplayDelegate<int32_t> display(stringMap);

    AAX_CString result;

    // Test with special characters
    EXPECT_TRUE(display.ValueToString(1, &result));
    EXPECT_STREQ("Spëcîál Chärs", result.Get());

    EXPECT_TRUE(display.ValueToString(2, &result));
    EXPECT_STREQ("Symbols: !@#$%^&*()", result.Get());

    EXPECT_TRUE(display.ValueToString(3, &result));
    EXPECT_STREQ("Numbers: 123456789", result.Get());

    // Test string to value conversion with special characters
    int32_t value;
    EXPECT_TRUE(display.StringToValue(AAX_CString("Spëcîál Chärs"), &value));
    EXPECT_EQ(1, value);

    EXPECT_TRUE(display.StringToValue(AAX_CString("Symbols: !@#$%^&*()"), &value));
    EXPECT_EQ(2, value);

    EXPECT_TRUE(display.StringToValue(AAX_CString("Numbers: 123456789"), &value));
    EXPECT_EQ(3, value);
}

//--------------------------------------------------------------------
#pragma mark - Round-Trip Conversion Tests

TEST_F(AAX_CStringDisplayDelegate_Test, RoundTrip_IntConversion)
{
    std::map<int32_t, AAX_CString> stringMap;
    stringMap[100] = AAX_CString("Hundred");
    stringMap[200] = AAX_CString("TwoHundred");
    stringMap[300] = AAX_CString("ThreeHundred");

    AAX_CStringDisplayDelegate<int32_t> display(stringMap);

    int32_t originalValues[] = {100, 200, 300};

    for (int32_t originalValue : originalValues) {
        AAX_CString stringValue;
        EXPECT_TRUE(display.ValueToString(originalValue, &stringValue));

        int32_t convertedValue;
        EXPECT_TRUE(display.StringToValue(stringValue, &convertedValue));

        EXPECT_EQ(originalValue, convertedValue);
    }
}

TEST_F(AAX_CStringDisplayDelegate_Test, RoundTrip_FloatConversion)
{
    std::map<float, AAX_CString> stringMap;
    stringMap[1.5f] = AAX_CString("OnePointFive");
    stringMap[2.75f] = AAX_CString("TwoPointSevenFive");
    stringMap[-0.5f] = AAX_CString("NegativeHalf");

    AAX_CStringDisplayDelegate<float> display(stringMap);

    float originalValues[] = {1.5f, 2.75f, -0.5f};

    for (float originalValue : originalValues) {
        AAX_CString stringValue;
        EXPECT_TRUE(display.ValueToString(originalValue, &stringValue));

        float convertedValue;
        EXPECT_TRUE(display.StringToValue(stringValue, &convertedValue));

        EXPECT_FLOAT_EQ(originalValue, convertedValue);
    }
}
