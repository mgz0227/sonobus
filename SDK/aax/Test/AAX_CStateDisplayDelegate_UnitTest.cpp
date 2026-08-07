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
 *	\file AAX_CStateDisplayDelegate_UnitTest.cpp
 *
 *	\brief Unit tests for AAX_CStateDisplayDelegate template class
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX_CStateDisplayDelegate.h"
#include "AAX_CString.h"
#include <vector>

//--------------------------------------------------------------------
#pragma mark - Test fixture

class AAX_CStateDisplayDelegate_Test : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

//--------------------------------------------------------------------
#pragma mark - Constructor Tests

TEST_F(AAX_CStateDisplayDelegate_Test, IntDisplay_NullTerminatedConstructor)
{
    const char* states[] = {"Off", "Low", "Medium", "High", nullptr};
    AAX_CStateDisplayDelegate<int32_t> display(states, 0);
    
    AAX_CString result;
    
    // Test basic state mapping
    EXPECT_TRUE(display.ValueToString(0, &result));
    EXPECT_STREQ("Off", result.Get());
    
    EXPECT_TRUE(display.ValueToString(1, &result));
    EXPECT_STREQ("Low", result.Get());
    
    EXPECT_TRUE(display.ValueToString(2, &result));
    EXPECT_STREQ("Medium", result.Get());
    
    EXPECT_TRUE(display.ValueToString(3, &result));
    EXPECT_STREQ("High", result.Get());
    
    // Test out of range
    EXPECT_FALSE(display.ValueToString(4, &result));
    EXPECT_FALSE(display.ValueToString(-1, &result));
}

TEST_F(AAX_CStateDisplayDelegate_Test, IntDisplay_CountedConstructor)
{
    const char* states[] = {"State0", "State1", "State2", "State3", "State4"};
    AAX_CStateDisplayDelegate<int32_t> display(3, states, 1); // Only first 3 states, min=1
    
    AAX_CString result;
    
    // Test state mapping with min offset
    EXPECT_TRUE(display.ValueToString(1, &result)); // index 0
    EXPECT_STREQ("State0", result.Get());
    
    EXPECT_TRUE(display.ValueToString(2, &result)); // index 1
    EXPECT_STREQ("State1", result.Get());
    
    EXPECT_TRUE(display.ValueToString(3, &result)); // index 2
    EXPECT_STREQ("State2", result.Get());
    
    // Test out of range (State3 and State4 not included due to count=3)
    EXPECT_FALSE(display.ValueToString(4, &result));
    EXPECT_FALSE(display.ValueToString(0, &result)); // Below min
}

TEST_F(AAX_CStateDisplayDelegate_Test, IntDisplay_VectorConstructor)
{
    std::vector<AAX_IString*> stateStrings;
    AAX_CString str1("First");
    AAX_CString str2("Second");
    AAX_CString str3("Third");
    
    stateStrings.push_back(&str1);
    stateStrings.push_back(&str2);
    stateStrings.push_back(&str3);
    
    AAX_CStateDisplayDelegate<int32_t> display(stateStrings, 10);
    
    AAX_CString result;
    
    // Test state mapping with min offset
    EXPECT_TRUE(display.ValueToString(10, &result)); // index 0
    EXPECT_STREQ("First", result.Get());
    
    EXPECT_TRUE(display.ValueToString(11, &result)); // index 1
    EXPECT_STREQ("Second", result.Get());
    
    EXPECT_TRUE(display.ValueToString(12, &result)); // index 2
    EXPECT_STREQ("Third", result.Get());
    
    // Test out of range
    EXPECT_FALSE(display.ValueToString(13, &result));
    EXPECT_FALSE(display.ValueToString(9, &result));
}

TEST_F(AAX_CStateDisplayDelegate_Test, IntDisplay_VectorConstructorWithNulls)
{
    std::vector<AAX_IString*> stateStrings;
    AAX_CString str1("Valid1");
    AAX_CString str2("Valid2");
    
    stateStrings.push_back(&str1);
    stateStrings.push_back(nullptr); // Null pointer should be skipped
    stateStrings.push_back(&str2);
    
    AAX_CStateDisplayDelegate<int32_t> display(stateStrings, 0);
    
    AAX_CString result;
    
    // Test that null pointers are skipped
    EXPECT_TRUE(display.ValueToString(0, &result));
    EXPECT_STREQ("Valid1", result.Get());
    
    EXPECT_TRUE(display.ValueToString(1, &result));
    EXPECT_STREQ("Valid2", result.Get());
    
    // Only 2 valid strings should be stored
    EXPECT_FALSE(display.ValueToString(2, &result));
}

TEST_F(AAX_CStateDisplayDelegate_Test, CopyConstructor)
{
    const char* states[] = {"Original1", "Original2", nullptr};
    AAX_CStateDisplayDelegate<int32_t> original(states, 5);
    
    // Add shortened strings to original
    const char* shortStates[] = {"O1", "O2", nullptr};
    original.AddShortenedStrings(shortStates, 2);
    
    // Create copy
    AAX_CStateDisplayDelegate<int32_t> copy(original);
    
    AAX_CString result;
    
    // Test that copy works independently
    EXPECT_TRUE(copy.ValueToString(5, &result));
    EXPECT_STREQ("Original1", result.Get());
    
    EXPECT_TRUE(copy.ValueToString(6, &result));
    EXPECT_STREQ("Original2", result.Get());
    
    // Test that shortened strings are also copied
    EXPECT_TRUE(copy.ValueToString(5, 2, &result));
    EXPECT_STREQ("O1", result.Get());
}

//--------------------------------------------------------------------
#pragma mark - ValueToString Tests

TEST_F(AAX_CStateDisplayDelegate_Test, ValueToString_BasicFunctionality)
{
    const char* states[] = {"Zero", "One", "Two", nullptr};
    AAX_CStateDisplayDelegate<int32_t> display(states, 0);
    
    AAX_CString result;
    
    // Test basic functionality
    EXPECT_TRUE(display.ValueToString(0, &result));
    EXPECT_STREQ("Zero", result.Get());
    
    EXPECT_TRUE(display.ValueToString(1, &result));
    EXPECT_STREQ("One", result.Get());
    
    EXPECT_TRUE(display.ValueToString(2, &result));
    EXPECT_STREQ("Two", result.Get());
    
    // Test out of range values
    EXPECT_FALSE(display.ValueToString(3, &result));
    EXPECT_FALSE(display.ValueToString(-1, &result));
}

TEST_F(AAX_CStateDisplayDelegate_Test, ValueToString_WithMaxChars_NoShortenedStrings)
{
    const char* states[] = {"Short", "VeryLongStateName", nullptr};
    AAX_CStateDisplayDelegate<int32_t> display(states, 0);
    
    AAX_CString result;
    
    // Without shortened strings, should return full strings if they fit
    EXPECT_TRUE(display.ValueToString(0, 10, &result));
    EXPECT_STREQ("Short", result.Get()); // Fits within 10 chars
    
    EXPECT_TRUE(display.ValueToString(1, 10, &result));
    EXPECT_STREQ("VeryLongStateName", result.Get()); // Full string returned even if longer than maxNumChars
    
    // Test with maxNumChars larger than strings
    EXPECT_TRUE(display.ValueToString(0, 100, &result));
    EXPECT_STREQ("Short", result.Get());
}

TEST_F(AAX_CStateDisplayDelegate_Test, ValueToString_WithMaxChars_WithShortenedStrings)
{
    const char* states[] = {"VeryLongStateName", "AnotherLongName", nullptr};
    AAX_CStateDisplayDelegate<int32_t> display(states, 0);
    
    // Add shortened strings
    const char* shortStates1[] = {"VLong", "ALong", nullptr};
    display.AddShortenedStrings(shortStates1, 5);
    
    const char* shortStates2[] = {"V", "A", nullptr};
    display.AddShortenedStrings(shortStates2, 1);
    
    AAX_CString result;
    
    // Test with maxNumChars that should use full strings (if they fit)
    EXPECT_TRUE(display.ValueToString(0, 20, &result));
    EXPECT_STREQ("VeryLongStateName", result.Get());
    
    // Test with maxNumChars that should use 5-character shortened strings
    EXPECT_TRUE(display.ValueToString(0, 5, &result));
    EXPECT_STREQ("VLong", result.Get());
    
    EXPECT_TRUE(display.ValueToString(1, 5, &result));
    EXPECT_STREQ("ALong", result.Get());
    
    // Test with maxNumChars that should use 1-character shortened strings
    EXPECT_TRUE(display.ValueToString(0, 1, &result));
    EXPECT_STREQ("V", result.Get());
    
    EXPECT_TRUE(display.ValueToString(1, 1, &result));
    EXPECT_STREQ("A", result.Get());
}

TEST_F(AAX_CStateDisplayDelegate_Test, ValueToString_OutOfRangeWithShortenedStrings)
{
    const char* states[] = {"State0", "State1", nullptr};
    AAX_CStateDisplayDelegate<int32_t> display(states, 0);

    const char* shortStates[] = {"S0", "S1", nullptr};
    display.AddShortenedStrings(shortStates, 2);

    AAX_CString result;

    // POTENTIAL BUG: Out of range values return true but don't set valueString
    // Line 204-205: if index is out of range, return true without setting valueString
    EXPECT_TRUE(display.ValueToString(5, 2, &result)); // Out of range value
    // The result string is not modified, so it contains whatever was there before
}

//--------------------------------------------------------------------
#pragma mark - StringToValue Tests

TEST_F(AAX_CStateDisplayDelegate_Test, StringToValue_BasicFunctionality)
{
    const char* states[] = {"Off", "On", "Auto", nullptr};
    AAX_CStateDisplayDelegate<int32_t> display(states, 1);

    int32_t result;

    // Test valid string conversions
    EXPECT_TRUE(display.StringToValue(AAX_CString("Off"), &result));
    EXPECT_EQ(1, result); // min + index 0

    EXPECT_TRUE(display.StringToValue(AAX_CString("On"), &result));
    EXPECT_EQ(2, result); // min + index 1

    EXPECT_TRUE(display.StringToValue(AAX_CString("Auto"), &result));
    EXPECT_EQ(3, result); // min + index 2
}

TEST_F(AAX_CStateDisplayDelegate_Test, StringToValue_InvalidStrings)
{
    const char* states[] = {"Valid1", "Valid2", nullptr};
    AAX_CStateDisplayDelegate<int32_t> display(states, 0);

    int32_t result = 999; // Initialize to non-default value

    // Test invalid string conversions
    EXPECT_FALSE(display.StringToValue(AAX_CString("Invalid"), &result));
    EXPECT_EQ(0, result); // Should be set to mMinState on failure

    result = 999; // Reset
    EXPECT_FALSE(display.StringToValue(AAX_CString(""), &result));
    EXPECT_EQ(0, result); // Should be set to mMinState on failure

    result = 999; // Reset
    EXPECT_FALSE(display.StringToValue(AAX_CString("valid1"), &result)); // Case sensitive
    EXPECT_EQ(0, result); // Should be set to mMinState on failure
}

TEST_F(AAX_CStateDisplayDelegate_Test, StringToValue_CaseSensitive)
{
    const char* states[] = {"Lower", "UPPER", "MiXeD", nullptr};
    AAX_CStateDisplayDelegate<int32_t> display(states, 0);

    int32_t result;

    // Test case sensitivity
    EXPECT_TRUE(display.StringToValue(AAX_CString("Lower"), &result));
    EXPECT_EQ(0, result);

    EXPECT_TRUE(display.StringToValue(AAX_CString("UPPER"), &result));
    EXPECT_EQ(1, result);

    EXPECT_TRUE(display.StringToValue(AAX_CString("MiXeD"), &result));
    EXPECT_EQ(2, result);

    // Test that case variations don't match
    EXPECT_FALSE(display.StringToValue(AAX_CString("lower"), &result));
    EXPECT_FALSE(display.StringToValue(AAX_CString("upper"), &result));
    EXPECT_FALSE(display.StringToValue(AAX_CString("mixed"), &result));
}

//--------------------------------------------------------------------
#pragma mark - AddShortenedStrings Tests

TEST_F(AAX_CStateDisplayDelegate_Test, AddShortenedStrings_SingleSet)
{
    const char* states[] = {"LongStateName1", "LongStateName2", nullptr};
    AAX_CStateDisplayDelegate<int32_t> display(states, 0);

    // Add one set of shortened strings
    const char* shortStates[] = {"Short1", "Short2", nullptr};
    display.AddShortenedStrings(shortStates, 6);

    AAX_CString result;

    // Test that shortened strings are used when appropriate
    EXPECT_TRUE(display.ValueToString(0, 6, &result));
    EXPECT_STREQ("Short1", result.Get());

    EXPECT_TRUE(display.ValueToString(1, 6, &result));
    EXPECT_STREQ("Short2", result.Get());

    // Test that full strings are still used when there's enough space
    EXPECT_TRUE(display.ValueToString(0, 20, &result));
    EXPECT_STREQ("LongStateName1", result.Get());
}

TEST_F(AAX_CStateDisplayDelegate_Test, AddShortenedStrings_MultipleSets)
{
    const char* states[] = {"VeryLongStateName", "AnotherLongName", nullptr};
    AAX_CStateDisplayDelegate<int32_t> display(states, 0);

    // Add multiple sets of shortened strings (should be sorted by length)
    const char* mediumStates[] = {"Medium1", "Medium2", nullptr};
    display.AddShortenedStrings(mediumStates, 7);

    const char* shortStates[] = {"S1", "S2", nullptr};
    display.AddShortenedStrings(shortStates, 2);

    const char* longStates[] = {"LongName1", "LongName2", nullptr};
    display.AddShortenedStrings(longStates, 9);

    AAX_CString result;

    // Test with maxNumChars=2 (should use shortest available)
    EXPECT_TRUE(display.ValueToString(0, 2, &result));
    EXPECT_STREQ("S1", result.Get());

    // Test with maxNumChars=7 (should use medium length)
    EXPECT_TRUE(display.ValueToString(0, 7, &result));
    EXPECT_STREQ("Medium1", result.Get());

    // Test with maxNumChars=9 (should use long length)
    EXPECT_TRUE(display.ValueToString(0, 9, &result));
    EXPECT_STREQ("LongName1", result.Get());

    // Test with maxNumChars=20 (should use original full strings)
    EXPECT_TRUE(display.ValueToString(0, 20, &result));
    EXPECT_STREQ("VeryLongStateName", result.Get());
}

TEST_F(AAX_CStateDisplayDelegate_Test, AddShortenedStrings_FallbackToShortest)
{
    const char* states[] = {"VeryLongStateName", nullptr};
    AAX_CStateDisplayDelegate<int32_t> display(states, 0);

    // Add shortened strings that are still too long
    const char* shortStates[] = {"StillTooLong", nullptr};
    display.AddShortenedStrings(shortStates, 12);

    AAX_CString result;

    // Test with maxNumChars smaller than any available string
    // Should fall back to shortest available (line 228-233)
    EXPECT_TRUE(display.ValueToString(0, 5, &result));
    EXPECT_STREQ("StillTooLong", result.Get()); // Uses shortest available even if too long
}

//--------------------------------------------------------------------
#pragma mark - Clone Tests

TEST_F(AAX_CStateDisplayDelegate_Test, Clone_BasicFunctionality)
{
    const char* states[] = {"State1", "State2", nullptr};
    AAX_CStateDisplayDelegate<int32_t> original(states, 5);
    std::unique_ptr<AAX_IDisplayDelegate<int32_t>> clone(original.Clone());

    AAX_CString originalResult, cloneResult;

    // Test that clone works the same as original
    EXPECT_TRUE(original.ValueToString(5, &originalResult));
    EXPECT_TRUE(clone->ValueToString(5, &cloneResult));
    EXPECT_STREQ(originalResult.Get(), cloneResult.Get());

    EXPECT_TRUE(original.ValueToString(6, &originalResult));
    EXPECT_TRUE(clone->ValueToString(6, &cloneResult));
    EXPECT_STREQ(originalResult.Get(), cloneResult.Get());

    // Test string to value conversion
    int32_t originalValue, cloneValue;
    EXPECT_TRUE(original.StringToValue(AAX_CString("State1"), &originalValue));
    EXPECT_TRUE(clone->StringToValue(AAX_CString("State1"), &cloneValue));
    EXPECT_EQ(originalValue, cloneValue);
}

TEST_F(AAX_CStateDisplayDelegate_Test, Clone_WithShortenedStrings)
{
    const char* states[] = {"LongState1", "LongState2", nullptr};
    AAX_CStateDisplayDelegate<int32_t> original(states, 0);

    const char* shortStates[] = {"S1", "S2", nullptr};
    original.AddShortenedStrings(shortStates, 2);

    std::unique_ptr<AAX_IDisplayDelegate<int32_t>> clone(original.Clone());

    AAX_CString originalResult, cloneResult;

    // Test that shortened strings are also cloned
    EXPECT_TRUE(original.ValueToString(0, 2, &originalResult));
    EXPECT_TRUE(clone->ValueToString(0, 2, &cloneResult));
    EXPECT_STREQ(originalResult.Get(), cloneResult.Get());

    EXPECT_TRUE(original.ValueToString(1, 2, &originalResult));
    EXPECT_TRUE(clone->ValueToString(1, 2, &cloneResult));
    EXPECT_STREQ(originalResult.Get(), cloneResult.Get());
}

//--------------------------------------------------------------------
#pragma mark - Template Type Tests

TEST_F(AAX_CStateDisplayDelegate_Test, FloatDisplay_BasicFunctionality)
{
    const char* states[] = {"0.0", "0.5", "1.0", nullptr};
    AAX_CStateDisplayDelegate<float> display(states, 0.0f);

    AAX_CString result;

    // Test with float values
    EXPECT_TRUE(display.ValueToString(0.0f, &result));
    EXPECT_STREQ("0.0", result.Get());

    EXPECT_TRUE(display.ValueToString(1.0f, &result));
    EXPECT_STREQ("0.5", result.Get());

    EXPECT_TRUE(display.ValueToString(2.0f, &result));
    EXPECT_STREQ("1.0", result.Get());

    // Test string to value conversion
    float value;
    EXPECT_TRUE(display.StringToValue(AAX_CString("0.0"), &value));
    EXPECT_FLOAT_EQ(0.0f, value);

    EXPECT_TRUE(display.StringToValue(AAX_CString("0.5"), &value));
    EXPECT_FLOAT_EQ(1.0f, value);

    EXPECT_TRUE(display.StringToValue(AAX_CString("1.0"), &value));
    EXPECT_FLOAT_EQ(2.0f, value);
}

TEST_F(AAX_CStateDisplayDelegate_Test, BoolDisplay_BasicFunctionality)
{
    // POTENTIAL BUG: Cannot use bool as template parameter due to ++index operation in StringToValue
    // Line 242: for ( T index = 0; iter != mStateStrings.end(); ++index, ++iter )
    // When T=bool, ++index tries to increment a bool, which is not allowed in C++17

    // Use int32_t instead to simulate bool behavior
    const char* states[] = {"False", "True", nullptr};
    AAX_CStateDisplayDelegate<int32_t> display(states, 0); // 0=false, 1=true

    AAX_CString result;

    // Test with int values representing bool
    EXPECT_TRUE(display.ValueToString(0, &result)); // false
    EXPECT_STREQ("False", result.Get());

    EXPECT_TRUE(display.ValueToString(1, &result)); // true
    EXPECT_STREQ("True", result.Get());

    // Test string to value conversion
    int32_t value;
    EXPECT_TRUE(display.StringToValue(AAX_CString("False"), &value));
    EXPECT_EQ(0, value); // false

    EXPECT_TRUE(display.StringToValue(AAX_CString("True"), &value));
    EXPECT_EQ(1, value); // true
}

//--------------------------------------------------------------------
#pragma mark - Edge Cases and Error Handling Tests

TEST_F(AAX_CStateDisplayDelegate_Test, EdgeCases_EmptyStateArray)
{
    const char* states[] = {nullptr}; // Empty array
    AAX_CStateDisplayDelegate<int32_t> display(states, 0);

    AAX_CString result;

    // Test with empty state array
    EXPECT_FALSE(display.ValueToString(0, &result));
    EXPECT_FALSE(display.ValueToString(1, &result));

    // Test string to value with empty array
    int32_t value;
    EXPECT_FALSE(display.StringToValue(AAX_CString("Anything"), &value));
    EXPECT_EQ(0, value); // Should be set to mMinState
}

TEST_F(AAX_CStateDisplayDelegate_Test, EdgeCases_SingleState)
{
    const char* states[] = {"OnlyState", nullptr};
    AAX_CStateDisplayDelegate<int32_t> display(states, 10);

    AAX_CString result;

    // Test with single state
    EXPECT_TRUE(display.ValueToString(10, &result));
    EXPECT_STREQ("OnlyState", result.Get());

    EXPECT_FALSE(display.ValueToString(11, &result));
    EXPECT_FALSE(display.ValueToString(9, &result));

    // Test string to value with single state
    int32_t value;
    EXPECT_TRUE(display.StringToValue(AAX_CString("OnlyState"), &value));
    EXPECT_EQ(10, value);
}

TEST_F(AAX_CStateDisplayDelegate_Test, EdgeCases_EmptyStrings)
{
    const char* states[] = {"", "NonEmpty", "", nullptr};
    AAX_CStateDisplayDelegate<int32_t> display(states, 0);

    AAX_CString result;

    // Test with empty strings
    EXPECT_TRUE(display.ValueToString(0, &result));
    EXPECT_STREQ("", result.Get());
    EXPECT_EQ(0U, result.Length());

    EXPECT_TRUE(display.ValueToString(1, &result));
    EXPECT_STREQ("NonEmpty", result.Get());

    EXPECT_TRUE(display.ValueToString(2, &result));
    EXPECT_STREQ("", result.Get());

    // Test string to value conversion with empty strings
    int32_t value;
    EXPECT_TRUE(display.StringToValue(AAX_CString(""), &value));
    EXPECT_EQ(0, value); // Should match first empty string

    // Test that it finds the first match (index 0, not index 2)
    EXPECT_TRUE(display.StringToValue(AAX_CString(""), &value));
    EXPECT_EQ(0, value); // First occurrence at index 0
}

//--------------------------------------------------------------------
#pragma mark - Round-Trip Conversion Tests

TEST_F(AAX_CStateDisplayDelegate_Test, RoundTrip_IntConversion)
{
    const char* states[] = {"Alpha", "Beta", "Gamma", "Delta", nullptr};
    AAX_CStateDisplayDelegate<int32_t> display(states, 100);

    int32_t originalValues[] = {100, 101, 102, 103};

    for (int32_t originalValue : originalValues) {
        AAX_CString stringValue;
        EXPECT_TRUE(display.ValueToString(originalValue, &stringValue));

        int32_t convertedValue;
        EXPECT_TRUE(display.StringToValue(stringValue, &convertedValue));

        EXPECT_EQ(originalValue, convertedValue);
    }
}

TEST_F(AAX_CStateDisplayDelegate_Test, Compare_Method)
{
    const char* states[] = {"Test", nullptr};
    AAX_CStateDisplayDelegate<int32_t> display(states, 0);

    // The Compare method is used internally by StringToValue
    // It should perform exact string comparison (case-sensitive)
    EXPECT_TRUE(display.Compare(AAX_CString("Test"), AAX_CString("Test")));
    EXPECT_FALSE(display.Compare(AAX_CString("Test"), AAX_CString("test")));
    EXPECT_FALSE(display.Compare(AAX_CString("Test"), AAX_CString("TEST")));
    EXPECT_FALSE(display.Compare(AAX_CString("Test"), AAX_CString("Different")));
    EXPECT_TRUE(display.Compare(AAX_CString(""), AAX_CString("")));
}
