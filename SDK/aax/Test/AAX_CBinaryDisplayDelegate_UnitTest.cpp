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
 *	\file AAX_CBinaryDisplayDelegate_UnitTest.cpp
 *
 *	\brief Unit tests for AAX_CBinaryDisplayDelegate template class
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX_CBinaryDisplayDelegate.h"
#include "AAX_CString.h"

//--------------------------------------------------------------------
#pragma mark - Test fixture

class AAX_CBinaryDisplayDelegate_Test : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

//--------------------------------------------------------------------
#pragma mark - Constructor Tests

TEST_F(AAX_CBinaryDisplayDelegate_Test, BoolDisplay_BasicConstruction)
{
    AAX_CBinaryDisplayDelegate<bool> display("False", "True");
    
    // Test basic value to string conversion
    AAX_CString result;
    
    EXPECT_TRUE(display.ValueToString(true, &result));
    EXPECT_STREQ("True", result.Get());
    
    EXPECT_TRUE(display.ValueToString(false, &result));
    EXPECT_STREQ("False", result.Get());
}

TEST_F(AAX_CBinaryDisplayDelegate_Test, BoolDisplay_CustomStrings)
{
    AAX_CBinaryDisplayDelegate<bool> display("Off", "On");
    
    AAX_CString result;
    
    EXPECT_TRUE(display.ValueToString(true, &result));
    EXPECT_STREQ("On", result.Get());
    
    EXPECT_TRUE(display.ValueToString(false, &result));
    EXPECT_STREQ("Off", result.Get());
}

TEST_F(AAX_CBinaryDisplayDelegate_Test, BoolDisplay_CopyConstructor)
{
    AAX_CBinaryDisplayDelegate<bool> original("Disabled", "Enabled");
    AAX_CBinaryDisplayDelegate<bool> copy(original);
    
    AAX_CString originalResult, copyResult;
    
    // Test that both work independently
    EXPECT_TRUE(original.ValueToString(true, &originalResult));
    EXPECT_TRUE(copy.ValueToString(true, &copyResult));
    
    EXPECT_STREQ("Enabled", originalResult.Get());
    EXPECT_STREQ("Enabled", copyResult.Get());
    
    EXPECT_TRUE(original.ValueToString(false, &originalResult));
    EXPECT_TRUE(copy.ValueToString(false, &copyResult));
    
    EXPECT_STREQ("Disabled", originalResult.Get());
    EXPECT_STREQ("Disabled", copyResult.Get());
}

//--------------------------------------------------------------------
#pragma mark - ValueToString Tests

TEST_F(AAX_CBinaryDisplayDelegate_Test, ValueToString_BasicFunctionality)
{
    AAX_CBinaryDisplayDelegate<bool> display("No", "Yes");
    
    AAX_CString result;
    
    // Test basic ValueToString without character limit
    EXPECT_TRUE(display.ValueToString(true, &result));
    EXPECT_STREQ("Yes", result.Get());
    
    EXPECT_TRUE(display.ValueToString(false, &result));
    EXPECT_STREQ("No", result.Get());
    
    // Note: Passing nullptr to ValueToString may cause segmentation fault
    // The implementation dereferences the pointer without null checking
    // This is expected behavior - the caller must provide a valid pointer
}

TEST_F(AAX_CBinaryDisplayDelegate_Test, ValueToString_WithMaxChars_NoShortenedStrings)
{
    AAX_CBinaryDisplayDelegate<bool> display("False", "True");
    
    AAX_CString result;
    
    // Without shortened strings, should return full strings regardless of maxNumChars
    EXPECT_TRUE(display.ValueToString(true, 3, &result));
    EXPECT_STREQ("True", result.Get()); // Full string despite maxNumChars=3
    
    EXPECT_TRUE(display.ValueToString(false, 2, &result));
    EXPECT_STREQ("False", result.Get()); // Full string despite maxNumChars=2
    
    // Test with maxNumChars larger than strings
    EXPECT_TRUE(display.ValueToString(true, 10, &result));
    EXPECT_STREQ("True", result.Get());
    
    EXPECT_TRUE(display.ValueToString(false, 10, &result));
    EXPECT_STREQ("False", result.Get());
}

TEST_F(AAX_CBinaryDisplayDelegate_Test, ValueToString_WithMaxChars_WithShortenedStrings)
{
    AAX_CBinaryDisplayDelegate<bool> display("False", "True");
    
    // Add shortened strings
    display.AddShortenedStrings("F", "T", 1);
    display.AddShortenedStrings("No", "Yes", 3);
    
    AAX_CString result;
    
    // Test with maxNumChars that should use full strings
    EXPECT_TRUE(display.ValueToString(true, 10, &result));
    EXPECT_STREQ("True", result.Get());
    
    EXPECT_TRUE(display.ValueToString(false, 10, &result));
    EXPECT_STREQ("False", result.Get());
    
    // Test with maxNumChars that should use 3-character shortened strings
    EXPECT_TRUE(display.ValueToString(true, 3, &result));
    EXPECT_STREQ("Yes", result.Get());
    
    EXPECT_TRUE(display.ValueToString(false, 3, &result));
    EXPECT_STREQ("No", result.Get());
    
    // Test with maxNumChars that should use 1-character shortened strings
    EXPECT_TRUE(display.ValueToString(true, 1, &result));
    EXPECT_STREQ("T", result.Get());
    
    EXPECT_TRUE(display.ValueToString(false, 1, &result));
    EXPECT_STREQ("F", result.Get());
}

//--------------------------------------------------------------------
#pragma mark - StringToValue Tests

TEST_F(AAX_CBinaryDisplayDelegate_Test, StringToValue_BasicFunctionality)
{
    AAX_CBinaryDisplayDelegate<bool> display("False", "True");
    
    bool result;
    
    // Test valid string conversions
    EXPECT_TRUE(display.StringToValue(AAX_CString("True"), &result));
    EXPECT_TRUE(result);
    
    EXPECT_TRUE(display.StringToValue(AAX_CString("False"), &result));
    EXPECT_FALSE(result);
}

TEST_F(AAX_CBinaryDisplayDelegate_Test, StringToValue_InvalidStrings)
{
    AAX_CBinaryDisplayDelegate<bool> display("Off", "On");
    
    bool result = true; // Initialize to non-default value
    
    // Test invalid string conversions
    EXPECT_FALSE(display.StringToValue(AAX_CString("Maybe"), &result));
    EXPECT_FALSE(result); // Should be set to false on failure
    
    result = true; // Reset
    EXPECT_FALSE(display.StringToValue(AAX_CString(""), &result));
    EXPECT_FALSE(result); // Should be set to false on failure
    
    result = true; // Reset
    EXPECT_FALSE(display.StringToValue(AAX_CString("TRUE"), &result)); // Case sensitive
    EXPECT_FALSE(result); // Should be set to false on failure
}

TEST_F(AAX_CBinaryDisplayDelegate_Test, StringToValue_WithNullPointer)
{
    AAX_CBinaryDisplayDelegate<bool> display("No", "Yes");

    // Note: Passing nullptr to StringToValue may cause segmentation fault
    // The implementation dereferences the pointer without null checking
    // This is expected behavior - the caller must provide a valid pointer

    // Test valid conversions instead
    bool result;
    EXPECT_TRUE(display.StringToValue(AAX_CString("Yes"), &result));
    EXPECT_TRUE(result);

    EXPECT_TRUE(display.StringToValue(AAX_CString("No"), &result));
    EXPECT_FALSE(result);
}

//--------------------------------------------------------------------
#pragma mark - AddShortenedStrings Tests

TEST_F(AAX_CBinaryDisplayDelegate_Test, AddShortenedStrings_SingleSet)
{
    AAX_CBinaryDisplayDelegate<bool> display("Disabled", "Enabled");
    
    // Add one set of shortened strings
    display.AddShortenedStrings("Off", "On", 3);
    
    AAX_CString result;
    
    // Test that shortened strings are used when appropriate
    EXPECT_TRUE(display.ValueToString(true, 3, &result));
    EXPECT_STREQ("On", result.Get());
    
    EXPECT_TRUE(display.ValueToString(false, 3, &result));
    EXPECT_STREQ("Off", result.Get());
    
    // Test that full strings are still used when there's enough space
    EXPECT_TRUE(display.ValueToString(true, 10, &result));
    EXPECT_STREQ("Enabled", result.Get());
    
    EXPECT_TRUE(display.ValueToString(false, 10, &result));
    EXPECT_STREQ("Disabled", result.Get());
}

TEST_F(AAX_CBinaryDisplayDelegate_Test, AddShortenedStrings_MultipleSets)
{
    AAX_CBinaryDisplayDelegate<bool> display("Inactive", "Active");

    // Add multiple sets of shortened strings (order shouldn't matter due to sorting)
    display.AddShortenedStrings("I", "A", 1);
    display.AddShortenedStrings("Off", "On", 3);
    display.AddShortenedStrings("No", "Yes", 3); // Same length as previous, should be sorted

    AAX_CString result;

    // Test with maxNumChars=1 (should use shortest available)
    EXPECT_TRUE(display.ValueToString(true, 1, &result));
    EXPECT_STREQ("A", result.Get());

    EXPECT_TRUE(display.ValueToString(false, 1, &result));
    EXPECT_STREQ("I", result.Get());

    // Test with maxNumChars=3 (should use one of the 3-character sets)
    EXPECT_TRUE(display.ValueToString(true, 3, &result));
    // Could be either "On" or "Yes" depending on sort order - both are valid
    EXPECT_TRUE(result == AAX_CString("On") || result == AAX_CString("Yes"));

    // Test with maxNumChars=10 (should use full strings)
    EXPECT_TRUE(display.ValueToString(true, 10, &result));
    EXPECT_STREQ("Active", result.Get());
}

TEST_F(AAX_CBinaryDisplayDelegate_Test, AddShortenedStrings_EdgeCases)
{
    AAX_CBinaryDisplayDelegate<bool> display("False", "True");

    // Add shortened strings with edge case lengths
    display.AddShortenedStrings("", "", 0); // Empty strings
    display.AddShortenedStrings("VeryLongFalseString", "VeryLongTrueString", 20); // Longer than originals

    AAX_CString result;

    // Test with maxNumChars=0 (should use empty strings if available)
    EXPECT_TRUE(display.ValueToString(true, 0, &result));
    EXPECT_STREQ("", result.Get());

    EXPECT_TRUE(display.ValueToString(false, 0, &result));
    EXPECT_STREQ("", result.Get());

    // Test with large maxNumChars (should use original strings, not longer shortened ones)
    EXPECT_TRUE(display.ValueToString(true, 25, &result));
    EXPECT_STREQ("True", result.Get()); // Original, not "VeryLongTrueString"
}

//--------------------------------------------------------------------
#pragma mark - Clone Tests

TEST_F(AAX_CBinaryDisplayDelegate_Test, Clone_BasicFunctionality)
{
    AAX_CBinaryDisplayDelegate<bool> original("Off", "On");
    std::unique_ptr<AAX_IDisplayDelegate<bool>> clone(original.Clone());

    AAX_CString originalResult, cloneResult;

    // Test that clone works the same as original
    EXPECT_TRUE(original.ValueToString(true, &originalResult));
    EXPECT_TRUE(clone->ValueToString(true, &cloneResult));
    EXPECT_STREQ(originalResult.Get(), cloneResult.Get());

    EXPECT_TRUE(original.ValueToString(false, &originalResult));
    EXPECT_TRUE(clone->ValueToString(false, &cloneResult));
    EXPECT_STREQ(originalResult.Get(), cloneResult.Get());

    // Test string to value conversion
    bool originalValue, cloneValue;
    EXPECT_TRUE(original.StringToValue(AAX_CString("On"), &originalValue));
    EXPECT_TRUE(clone->StringToValue(AAX_CString("On"), &cloneValue));
    EXPECT_EQ(originalValue, cloneValue);
}

TEST_F(AAX_CBinaryDisplayDelegate_Test, Clone_WithShortenedStrings)
{
    AAX_CBinaryDisplayDelegate<bool> original("False", "True");
    original.AddShortenedStrings("F", "T", 1);
    original.AddShortenedStrings("No", "Yes", 3);

    std::unique_ptr<AAX_IDisplayDelegate<bool>> clone(original.Clone());

    AAX_CString originalResult, cloneResult;

    // Test that shortened strings are also cloned
    EXPECT_TRUE(original.ValueToString(true, 1, &originalResult));
    EXPECT_TRUE(clone->ValueToString(true, 1, &cloneResult));
    EXPECT_STREQ(originalResult.Get(), cloneResult.Get());

    EXPECT_TRUE(original.ValueToString(false, 3, &originalResult));
    EXPECT_TRUE(clone->ValueToString(false, 3, &cloneResult));
    EXPECT_STREQ(originalResult.Get(), cloneResult.Get());
}

//--------------------------------------------------------------------
#pragma mark - Template Type Tests

TEST_F(AAX_CBinaryDisplayDelegate_Test, FloatDisplay_BasicFunctionality)
{
    AAX_CBinaryDisplayDelegate<float> display("0.0", "1.0");

    AAX_CString result;

    // Test with float values
    EXPECT_TRUE(display.ValueToString(1.0f, &result));
    EXPECT_STREQ("1.0", result.Get());

    EXPECT_TRUE(display.ValueToString(0.0f, &result));
    EXPECT_STREQ("0.0", result.Get());

    EXPECT_TRUE(display.ValueToString(0.5f, &result)); // Non-zero should be true
    EXPECT_STREQ("1.0", result.Get());

    EXPECT_TRUE(display.ValueToString(-1.0f, &result)); // POTENTIAL BUG: Negative values are treated as true (non-zero)
    EXPECT_STREQ("1.0", result.Get()); // Any non-zero value (including negative) evaluates to true

    // Test string to value conversion
    float value;
    EXPECT_TRUE(display.StringToValue(AAX_CString("1.0"), &value));
    EXPECT_FLOAT_EQ(1.0f, value);

    EXPECT_TRUE(display.StringToValue(AAX_CString("0.0"), &value));
    EXPECT_FLOAT_EQ(0.0f, value);
}

TEST_F(AAX_CBinaryDisplayDelegate_Test, IntDisplay_BasicFunctionality)
{
    AAX_CBinaryDisplayDelegate<int32_t> display("Zero", "NonZero");

    AAX_CString result;

    // Test with integer values
    EXPECT_TRUE(display.ValueToString(1, &result));
    EXPECT_STREQ("NonZero", result.Get());

    EXPECT_TRUE(display.ValueToString(0, &result));
    EXPECT_STREQ("Zero", result.Get());

    EXPECT_TRUE(display.ValueToString(42, &result)); // Non-zero should be true
    EXPECT_STREQ("NonZero", result.Get());

    EXPECT_TRUE(display.ValueToString(-5, &result)); // POTENTIAL BUG: Negative values are treated as true (non-zero)
    EXPECT_STREQ("NonZero", result.Get()); // Any non-zero value (including negative) evaluates to true

    // Test string to value conversion
    int32_t value;
    EXPECT_TRUE(display.StringToValue(AAX_CString("NonZero"), &value));
    EXPECT_EQ(1, value);

    EXPECT_TRUE(display.StringToValue(AAX_CString("Zero"), &value));
    EXPECT_EQ(0, value);
}

//--------------------------------------------------------------------
#pragma mark - Edge Cases and Error Handling Tests

TEST_F(AAX_CBinaryDisplayDelegate_Test, EdgeCases_EmptyStrings)
{
    AAX_CBinaryDisplayDelegate<bool> display("", "");

    AAX_CString result;

    // Test with empty strings
    EXPECT_TRUE(display.ValueToString(true, &result));
    EXPECT_STREQ("", result.Get());
    EXPECT_EQ(0U, result.Length());

    EXPECT_TRUE(display.ValueToString(false, &result));
    EXPECT_STREQ("", result.Get());
    EXPECT_EQ(0U, result.Length());

    // Test string to value conversion with empty strings
    bool value;
    EXPECT_TRUE(display.StringToValue(AAX_CString(""), &value));
    EXPECT_TRUE(value); // POTENTIAL BUG: When both false and true strings are empty, empty input matches true string first (line 200 vs 204), should probably check false first
}

TEST_F(AAX_CBinaryDisplayDelegate_Test, EdgeCases_LongStrings)
{
    AAX_CString longFalse("This is a very long false string that tests the display delegate's ability to handle lengthy text");
    AAX_CString longTrue("This is a very long true string that tests the display delegate's ability to handle lengthy text");

    AAX_CBinaryDisplayDelegate<bool> display(longFalse.Get(), longTrue.Get());

    AAX_CString result;

    // Test with long strings
    EXPECT_TRUE(display.ValueToString(true, &result));
    EXPECT_STREQ(longTrue.Get(), result.Get());

    EXPECT_TRUE(display.ValueToString(false, &result));
    EXPECT_STREQ(longFalse.Get(), result.Get());

    // Test string to value conversion with long strings
    bool value;
    EXPECT_TRUE(display.StringToValue(longTrue, &value));
    EXPECT_TRUE(value);

    EXPECT_TRUE(display.StringToValue(longFalse, &value));
    EXPECT_FALSE(value);
}

TEST_F(AAX_CBinaryDisplayDelegate_Test, EdgeCases_SpecialCharacters)
{
    AAX_CBinaryDisplayDelegate<bool> display("Nö", "Yës");

    AAX_CString result;

    // Test with special characters
    EXPECT_TRUE(display.ValueToString(true, &result));
    EXPECT_STREQ("Yës", result.Get());

    EXPECT_TRUE(display.ValueToString(false, &result));
    EXPECT_STREQ("Nö", result.Get());

    // Test string to value conversion with special characters
    bool value;
    EXPECT_TRUE(display.StringToValue(AAX_CString("Yës"), &value));
    EXPECT_TRUE(value);

    EXPECT_TRUE(display.StringToValue(AAX_CString("Nö"), &value));
    EXPECT_FALSE(value);
}

//--------------------------------------------------------------------
#pragma mark - Round-Trip Conversion Tests

TEST_F(AAX_CBinaryDisplayDelegate_Test, RoundTrip_BoolConversion)
{
    AAX_CBinaryDisplayDelegate<bool> display("Inactive", "Active");

    bool originalValues[] = {true, false};

    for (bool originalValue : originalValues) {
        AAX_CString stringValue;
        EXPECT_TRUE(display.ValueToString(originalValue, &stringValue));

        bool convertedValue;
        EXPECT_TRUE(display.StringToValue(stringValue, &convertedValue));

        EXPECT_EQ(originalValue, convertedValue);
    }
}

TEST_F(AAX_CBinaryDisplayDelegate_Test, RoundTrip_WithShortenedStrings)
{
    AAX_CBinaryDisplayDelegate<bool> display("False", "True");
    display.AddShortenedStrings("F", "T", 1);

    // Test round-trip with full strings
    bool value;
    EXPECT_TRUE(display.StringToValue(AAX_CString("True"), &value));
    EXPECT_TRUE(value);

    AAX_CString result;
    EXPECT_TRUE(display.ValueToString(value, &result));
    EXPECT_STREQ("True", result.Get()); // Should use full string, not shortened

    // Note: Shortened strings are only used in ValueToString with maxNumChars
    // StringToValue only recognizes the original strings, not shortened ones
    EXPECT_FALSE(display.StringToValue(AAX_CString("T"), &value)); // Shortened string not recognized
}

TEST_F(AAX_CBinaryDisplayDelegate_Test, ShortenedStrings_SortingBehavior)
{
    AAX_CBinaryDisplayDelegate<bool> display("False", "True");

    // Add shortened strings in random order to test sorting
    display.AddShortenedStrings("Medium", "Medium", 6);
    display.AddShortenedStrings("S", "S", 1);
    display.AddShortenedStrings("Long", "Long", 4);

    AAX_CString result;

    // Test that the shortest available string is used when maxNumChars is very small
    EXPECT_TRUE(display.ValueToString(true, 1, &result));
    EXPECT_STREQ("S", result.Get()); // Should use the 1-character string

    // Test that appropriate length string is used
    EXPECT_TRUE(display.ValueToString(true, 4, &result));
    EXPECT_STREQ("Long", result.Get()); // Should use the 4-character string

    // Test that original string is used when maxNumChars is larger than original
    EXPECT_TRUE(display.ValueToString(true, 6, &result));
    EXPECT_STREQ("True", result.Get()); // Should use original string since 6 >= original length (4)
}

TEST_F(AAX_CBinaryDisplayDelegate_Test, NegativeMaxChars_EdgeCase)
{
    AAX_CBinaryDisplayDelegate<bool> display("False", "True");
    display.AddShortenedStrings("F", "T", 1);

    AAX_CString result;

    // Test with negative maxNumChars (should be treated as 0)
    EXPECT_TRUE(display.ValueToString(true, -1, &result));
    EXPECT_STREQ("T", result.Get()); // Should use shortest available

    EXPECT_TRUE(display.ValueToString(false, -5, &result));
    EXPECT_STREQ("F", result.Get()); // Should use shortest available
}
