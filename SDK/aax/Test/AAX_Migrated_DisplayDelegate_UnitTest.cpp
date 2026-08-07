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
 *	\file AAX_Migrated_DisplayDelegate_UnitTest.cpp
 *
 *	\brief Migrated unit tests for Display Delegates (originally from UnitTest++ framework)
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX_CNumberDisplayDelegate.h"
#include "AAX_CStringDisplayDelegate.h"
#include "AAX_CUnitDisplayDelegateDecorator.h"
#include "AAX_CUnitPrefixDisplayDelegateDecorator.h"
#include "AAX_CDecibelDisplayDelegateDecorator.h"
#include "AAX_CPercentDisplayDelegateDecorator.h"
#include <cstring>
#include <map>

//==============================================================================
// Number Display Delegate Tests
//==============================================================================

class AAX_Migrated_NumberDisplayDelegate_Test : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(AAX_Migrated_NumberDisplayDelegate_Test, FloatToStringSingleFractional)
{
    AAX_CNumberDisplayDelegate<float, 1> numberDisplayDelegate;
    AAX_CString valueString;
    
    // Test positive string equality
    numberDisplayDelegate.ValueToString(1.1f, &valueString);
    EXPECT_STREQ("1.1", valueString.CString());
    
    // Test rounding positive numbers to precision
    numberDisplayDelegate.ValueToString(100.255f, &valueString);
    EXPECT_STREQ("100.3", valueString.CString());
    
    // Test negative string equality
    numberDisplayDelegate.ValueToString(-5.0f, &valueString);
    EXPECT_STREQ("-5.0", valueString.CString());
    
    // Test rounding negative numbers to precision
    numberDisplayDelegate.ValueToString(-100.29f, &valueString);
    EXPECT_STREQ("-100.3", valueString.CString());
    
    // Test that positive median numbers round up correctly, rather than down
    numberDisplayDelegate.ValueToString(0.55f, &valueString);
    EXPECT_STREQ("0.6", valueString.CString());
    
    // Test that negative median numbers round up correctly, rather than down
    numberDisplayDelegate.ValueToString(-0.55f, &valueString);
    EXPECT_STREQ("-0.6", valueString.CString());
}

TEST_F(AAX_Migrated_NumberDisplayDelegate_Test, FloatToStringDoubleFractional)
{
    AAX_CNumberDisplayDelegate<float, 2> numberDisplayDelegate;
    AAX_CString valueString;
    
    numberDisplayDelegate.ValueToString(1.1f, &valueString);
    EXPECT_STREQ("1.10", valueString.CString());
    
    numberDisplayDelegate.ValueToString(100.256f, &valueString);
    EXPECT_STREQ("100.26", valueString.CString());
    
    numberDisplayDelegate.ValueToString(-5.0f, &valueString);
    EXPECT_STREQ("-5.00", valueString.CString());
    
    numberDisplayDelegate.ValueToString(-100.29f, &valueString);
    EXPECT_STREQ("-100.29", valueString.CString());
    
    // Test that positive median numbers round up correctly, rather than down
    numberDisplayDelegate.ValueToString(0.555f, &valueString);
    EXPECT_STREQ("0.56", valueString.CString());
    
    // Test that negative median numbers round up correctly, rather than down
    numberDisplayDelegate.ValueToString(-0.555f, &valueString);
    EXPECT_STREQ("-0.56", valueString.CString());
}

TEST_F(AAX_Migrated_NumberDisplayDelegate_Test, IntToString)
{
    AAX_CNumberDisplayDelegate<int32_t, 0> numberDisplayDelegate;
    AAX_CString valueString;
    
    numberDisplayDelegate.ValueToString(1, &valueString);
    EXPECT_STREQ("1", valueString.CString());
    
    numberDisplayDelegate.ValueToString(100, &valueString);
    EXPECT_STREQ("100", valueString.CString());
    
    numberDisplayDelegate.ValueToString(-5, &valueString);
    EXPECT_STREQ("-5", valueString.CString());
    
    numberDisplayDelegate.ValueToString(-100, &valueString);
    EXPECT_STREQ("-100", valueString.CString());
}

TEST_F(AAX_Migrated_NumberDisplayDelegate_Test, SingleStringToFloat)
{
    AAX_CNumberDisplayDelegate<float, 1> numberDisplayDelegate;
    AAX_CString valueString("1.1");
    float convertedValue = 0;
    
    EXPECT_TRUE(numberDisplayDelegate.StringToValue(valueString, &convertedValue));
    EXPECT_NEAR(1.1f, convertedValue, 0.001f);
}

TEST_F(AAX_Migrated_NumberDisplayDelegate_Test, DoubleStringToFloat)
{
    AAX_CNumberDisplayDelegate<float, 1> numberDisplayDelegate;
    AAX_CString valueString("-100.09");
    float convertedValue = 0;
    
    EXPECT_TRUE(numberDisplayDelegate.StringToValue(valueString, &convertedValue));
    EXPECT_NEAR(-100.09f, convertedValue, 0.001f);
}

TEST_F(AAX_Migrated_NumberDisplayDelegate_Test, StringToPositiveInt)
{
    AAX_CNumberDisplayDelegate<int32_t> numberDisplayDelegate;
    AAX_CString valueString("150");
    int32_t convertedValue = 0;
    
    EXPECT_TRUE(numberDisplayDelegate.StringToValue(valueString, &convertedValue));
    EXPECT_EQ(150, convertedValue);
}

TEST_F(AAX_Migrated_NumberDisplayDelegate_Test, StringToNegativeInt)
{
    AAX_CNumberDisplayDelegate<int32_t> numberDisplayDelegate;
    AAX_CString valueString("-1");
    int32_t convertedValue = 0;
    
    EXPECT_TRUE(numberDisplayDelegate.StringToValue(valueString, &convertedValue));
    EXPECT_EQ(-1, convertedValue);
}

TEST_F(AAX_Migrated_NumberDisplayDelegate_Test, FloatStringToInt)
{
    AAX_CNumberDisplayDelegate<int32_t> numberDisplayDelegate;
    AAX_CString valueString("-50.4");
    int32_t convertedValue = 0;
    
    EXPECT_TRUE(numberDisplayDelegate.StringToValue(valueString, &convertedValue));
    EXPECT_EQ(-50, convertedValue);
}

TEST_F(AAX_Migrated_NumberDisplayDelegate_Test, SuffixStringToInt)
{
    AAX_CNumberDisplayDelegate<int32_t> numberDisplayDelegate;
    AAX_CString valueString("-1024 Hz");
    int32_t convertedValue = 0;
    
    EXPECT_TRUE(numberDisplayDelegate.StringToValue(valueString, &convertedValue));
    EXPECT_EQ(-1024, convertedValue);
}

TEST_F(AAX_Migrated_NumberDisplayDelegate_Test, BadStringToInt)
{
    AAX_CNumberDisplayDelegate<int32_t> numberDisplayDelegate;
    AAX_CString valueString("abc-1");
    int32_t convertedValue = -1;
    
    EXPECT_FALSE(numberDisplayDelegate.StringToValue(valueString, &convertedValue));
    EXPECT_EQ(0, convertedValue);
}

TEST_F(AAX_Migrated_NumberDisplayDelegate_Test, BadStringToFloat)
{
    AAX_CNumberDisplayDelegate<float, 1> numberDisplayDelegate;
    AAX_CString valueString("BadString");
    float convertedValue = -1;
    
    EXPECT_FALSE(numberDisplayDelegate.StringToValue(valueString, &convertedValue));
    EXPECT_EQ(0, convertedValue);
}

//==============================================================================
// String Display Delegate Tests
//==============================================================================

class AAX_Migrated_StringDisplayDelegate_Test : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(AAX_Migrated_StringDisplayDelegate_Test, IntToStringAndBack)
{
    // Value String Map
    std::map<int32_t, AAX_CString> stringMap;
    stringMap[-5] = "MinusFive";
    stringMap[0] = "Zero";
    stringMap[1] = "One";
    
    AAX_CStringDisplayDelegate<int32_t> stringDelegate(stringMap);
    AAX_CString valueString;
    
    // value to string
    EXPECT_TRUE(stringDelegate.ValueToString(0, &valueString));
    EXPECT_STREQ("Zero", valueString.CString());
    EXPECT_TRUE(stringDelegate.ValueToString(1, &valueString));
    EXPECT_STREQ("One", valueString.CString());
    EXPECT_TRUE(stringDelegate.ValueToString(-5, &valueString));
    EXPECT_STREQ("MinusFive", valueString.CString());
    
    // This should not find a control string for this value.
    EXPECT_FALSE(stringDelegate.ValueToString(-2, &valueString));
    EXPECT_STREQ("String Not Found", valueString.CString());
    
    // string to value
    int32_t convertedValue;
    EXPECT_TRUE(stringDelegate.StringToValue(AAX_CString("Zero"), &convertedValue));
    EXPECT_EQ(0, convertedValue);
    EXPECT_TRUE(stringDelegate.StringToValue(AAX_CString("One"), &convertedValue));
    EXPECT_EQ(1, convertedValue);
    EXPECT_TRUE(stringDelegate.StringToValue(AAX_CString("MinusFive"), &convertedValue));
    EXPECT_EQ(-5, convertedValue);
    
    // bad string value
    EXPECT_FALSE(stringDelegate.StringToValue(AAX_CString("MinusOne"), &convertedValue));
    EXPECT_EQ(0, convertedValue); // StringToValue sets to 0 on failure
}

//==============================================================================
// Standard Unit Display Delegate Decorator Tests
//==============================================================================

class AAX_Migrated_StandardUnitDisplayDelegateDecorator_Test : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(AAX_Migrated_StandardUnitDisplayDelegateDecorator_Test, FloatToStringSingleFractional)
{
    AAX_CNumberDisplayDelegate<float, 1, 1> numberDisplayDelegate;
    AAX_CUnitDisplayDelegateDecorator<float> decoratedDisplay(numberDisplayDelegate, AAX_CString("Hz"));
    AAX_CString valueString;

    // Test positive string equality
    decoratedDisplay.ValueToString(1.1f, &valueString);
    EXPECT_STREQ("1.1 Hz", valueString.CString());

    // Test rounding positive numbers to precision
    decoratedDisplay.ValueToString(100.255f, &valueString);
    EXPECT_STREQ("100.3 Hz", valueString.CString());

    // Test negative string equality
    decoratedDisplay.ValueToString(-5.0f, &valueString);
    EXPECT_STREQ("-5.0 Hz", valueString.CString());

    // Test rounding negative numbers to precision
    decoratedDisplay.ValueToString(-100.29f, &valueString);
    EXPECT_STREQ("-100.3 Hz", valueString.CString());

    // Test that positive median numbers round up correctly, rather than down
    decoratedDisplay.ValueToString(0.55f, &valueString);
    EXPECT_STREQ("0.6 Hz", valueString.CString());

    // Test that negative median numbers round up correctly, rather than down
    decoratedDisplay.ValueToString(-0.55f, &valueString);
    EXPECT_STREQ("-0.6 Hz", valueString.CString());
}

TEST_F(AAX_Migrated_StandardUnitDisplayDelegateDecorator_Test, FloatToStringDoubleFractional)
{
    AAX_CNumberDisplayDelegate<float, 2, 1> numberDisplayDelegate;
    AAX_CUnitDisplayDelegateDecorator<float> decoratedDisplay(numberDisplayDelegate, AAX_CString("Hz"));
    AAX_CString valueString;

    decoratedDisplay.ValueToString(1.1f, &valueString);
    EXPECT_STREQ("1.10 Hz", valueString.CString());

    decoratedDisplay.ValueToString(100.256f, &valueString);
    EXPECT_STREQ("100.26 Hz", valueString.CString());

    decoratedDisplay.ValueToString(-5.0f, &valueString);
    EXPECT_STREQ("-5.00 Hz", valueString.CString());

    decoratedDisplay.ValueToString(-100.29f, &valueString);
    EXPECT_STREQ("-100.29 Hz", valueString.CString());

    // Test that positive median numbers round up correctly, rather than down
    decoratedDisplay.ValueToString(0.555f, &valueString);
    EXPECT_STREQ("0.56 Hz", valueString.CString());

    // Test that negative median numbers round up correctly, rather than down
    decoratedDisplay.ValueToString(-0.555f, &valueString);
    EXPECT_STREQ("-0.56 Hz", valueString.CString());
}

TEST_F(AAX_Migrated_StandardUnitDisplayDelegateDecorator_Test, IntToString)
{
    AAX_CNumberDisplayDelegate<int32_t, 0, 1> numberDisplayDelegate;
    AAX_CUnitDisplayDelegateDecorator<int32_t> decoratedDisplay(numberDisplayDelegate, AAX_CString("Hz"));
    AAX_CString valueString;

    decoratedDisplay.ValueToString(1, &valueString);
    EXPECT_STREQ("1 Hz", valueString.CString());

    decoratedDisplay.ValueToString(100, &valueString);
    EXPECT_STREQ("100 Hz", valueString.CString());

    decoratedDisplay.ValueToString(-5, &valueString);
    EXPECT_STREQ("-5 Hz", valueString.CString());

    decoratedDisplay.ValueToString(-100, &valueString);
    EXPECT_STREQ("-100 Hz", valueString.CString());
}

TEST_F(AAX_Migrated_StandardUnitDisplayDelegateDecorator_Test, SingleStringToFloat)
{
    AAX_CNumberDisplayDelegate<float, 1, 1> numberDisplayDelegate;
    AAX_CUnitDisplayDelegateDecorator<float> decoratedDisplay(numberDisplayDelegate, AAX_CString("Hz"));
    AAX_CString valueString("1.1 Hz");
    float convertedValue = 0;

    EXPECT_TRUE(decoratedDisplay.StringToValue(valueString, &convertedValue));
    EXPECT_NEAR(1.1f, convertedValue, 0.001f);
}

TEST_F(AAX_Migrated_StandardUnitDisplayDelegateDecorator_Test, ProblemStringToFloat)
{
    AAX_CNumberDisplayDelegate<float, 1, 1> numberDisplayDelegate;
    AAX_CUnitDisplayDelegateDecorator<float> decoratedDisplay(numberDisplayDelegate, AAX_CString("miles")); // problem because miles starts with m, which is also a prefix for milli.
    AAX_CString valueString("-1.1 miles");
    float convertedValue = 0;

    EXPECT_TRUE(decoratedDisplay.StringToValue(valueString, &convertedValue));
    EXPECT_NEAR(-1.1f, convertedValue, 0.001f);
}

TEST_F(AAX_Migrated_StandardUnitDisplayDelegateDecorator_Test, ProblemSpacedStringToFloat)
{
    AAX_CNumberDisplayDelegate<float, 1, 1> numberDisplayDelegate;
    AAX_CUnitDisplayDelegateDecorator<float> decoratedDisplay(numberDisplayDelegate, AAX_CString("miles")); // problem because miles starts with m, which is also a prefix for milli.
    AAX_CString valueString("-1.1miles"); // problem because there is no space between the number and the units.
    float convertedValue = 0;

    EXPECT_TRUE(decoratedDisplay.StringToValue(valueString, &convertedValue));
    EXPECT_NEAR(-1.1f, convertedValue, 0.001f);
}

TEST_F(AAX_Migrated_StandardUnitDisplayDelegateDecorator_Test, DoubleStringToFloat)
{
    AAX_CNumberDisplayDelegate<float, 1, 1> numberDisplayDelegate;
    AAX_CUnitDisplayDelegateDecorator<float> decoratedDisplay(numberDisplayDelegate, AAX_CString("Hz"));
    AAX_CString valueString("-100.09 Hz");
    float convertedValue = 0;

    EXPECT_TRUE(decoratedDisplay.StringToValue(valueString, &convertedValue));
    EXPECT_NEAR(-100.09f, convertedValue, 0.001f);
}

TEST_F(AAX_Migrated_StandardUnitDisplayDelegateDecorator_Test, StringToPositiveInt)
{
    AAX_CNumberDisplayDelegate<int32_t, 0, 1> numberDisplayDelegate;
    AAX_CUnitDisplayDelegateDecorator<int32_t> decoratedDisplay(numberDisplayDelegate, AAX_CString("Hz"));
    AAX_CString valueString("150 Hz");
    int32_t convertedValue = 0;

    EXPECT_TRUE(decoratedDisplay.StringToValue(valueString, &convertedValue));
    EXPECT_EQ(150, convertedValue);
}

TEST_F(AAX_Migrated_StandardUnitDisplayDelegateDecorator_Test, StringToNegativeInt)
{
    AAX_CNumberDisplayDelegate<int32_t, 0, 1> numberDisplayDelegate;
    AAX_CUnitDisplayDelegateDecorator<int32_t> decoratedDisplay(numberDisplayDelegate, AAX_CString("Hz"));
    AAX_CString valueString("-1 Hz");
    int32_t convertedValue = 0;

    EXPECT_TRUE(decoratedDisplay.StringToValue(valueString, &convertedValue));
    EXPECT_EQ(-1, convertedValue);
}

TEST_F(AAX_Migrated_StandardUnitDisplayDelegateDecorator_Test, FloatStringToInt)
{
    AAX_CNumberDisplayDelegate<int32_t, 0, 1> numberDisplayDelegate;
    AAX_CUnitDisplayDelegateDecorator<int32_t> decoratedDisplay(numberDisplayDelegate, AAX_CString("Hz"));
    AAX_CString valueString("-50.4 Hz");
    int32_t convertedValue = 0;

    EXPECT_TRUE(decoratedDisplay.StringToValue(valueString, &convertedValue));
    EXPECT_EQ(-50, convertedValue);
}

TEST_F(AAX_Migrated_StandardUnitDisplayDelegateDecorator_Test, SuffixStringToInt)
{
    AAX_CNumberDisplayDelegate<int32_t, 0, 1> numberDisplayDelegate;
    AAX_CUnitDisplayDelegateDecorator<int32_t> decoratedDisplay(numberDisplayDelegate, AAX_CString("Hz"));
    AAX_CString valueString("-1024 Hz");
    int32_t convertedValue = 0;

    EXPECT_TRUE(decoratedDisplay.StringToValue(valueString, &convertedValue));
    EXPECT_EQ(-1024, convertedValue);
}

TEST_F(AAX_Migrated_StandardUnitDisplayDelegateDecorator_Test, BadStringToInt)
{
    AAX_CNumberDisplayDelegate<int32_t, 0, 1> numberDisplayDelegate;
    AAX_CUnitDisplayDelegateDecorator<int32_t> decoratedDisplay(numberDisplayDelegate, AAX_CString("Hz"));
    AAX_CString valueString("abc-1 Hz");
    int32_t convertedValue = -1;

    EXPECT_FALSE(decoratedDisplay.StringToValue(valueString, &convertedValue));
    EXPECT_EQ(0, convertedValue);
}

TEST_F(AAX_Migrated_StandardUnitDisplayDelegateDecorator_Test, BadStringToFloat)
{
    AAX_CNumberDisplayDelegate<float, 1, 1> numberDisplayDelegate;
    AAX_CUnitDisplayDelegateDecorator<float> decoratedDisplay(numberDisplayDelegate, AAX_CString("Hz"));
    AAX_CString valueString("BadString Hz");
    float convertedValue = -1;

    EXPECT_FALSE(decoratedDisplay.StringToValue(valueString, &convertedValue));
    EXPECT_EQ(0, convertedValue);
}

TEST_F(AAX_Migrated_StandardUnitDisplayDelegateDecorator_Test, NullStringConversion)
{
    AAX_CNumberDisplayDelegate<int32_t> numberDisplayDelegate;
    AAX_CUnitDisplayDelegateDecorator<int32_t> decoratedDisplay(numberDisplayDelegate, AAX_CString("C"));
    AAX_CString valueString("");
    int32_t convertedValue = -1;

    EXPECT_FALSE(decoratedDisplay.StringToValue(valueString, &convertedValue));
    EXPECT_EQ(0, convertedValue);
}

//==============================================================================
// Unit Prefix Display Delegate Decorator Tests
//==============================================================================

class AAX_Migrated_UnitPrefixDisplayDelegateDecorator_Test : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(AAX_Migrated_UnitPrefixDisplayDelegateDecorator_Test, FloatToStringSingleFractional)
{
    AAX_CNumberDisplayDelegate<float, 1, 1> numberDisplayDelegate;
    AAX_CUnitPrefixDisplayDelegateDecorator<float> prefixDecorator(numberDisplayDelegate);
    AAX_CUnitDisplayDelegateDecorator<float> decoratedDisplay(prefixDecorator, AAX_CString("Hz"));
    AAX_CString valueString;

    decoratedDisplay.ValueToString(1.1f, &valueString);
    EXPECT_STREQ("1.1 Hz", valueString.CString());

    decoratedDisplay.ValueToString(100.255f, &valueString);
    EXPECT_STREQ("100.3 Hz", valueString.CString());

    decoratedDisplay.ValueToString(1000.0f, &valueString);
    EXPECT_STREQ("1.0 kHz", valueString.CString());

    decoratedDisplay.ValueToString(1000000.0f, &valueString);
    EXPECT_STREQ("1.0 MHz", valueString.CString());

    decoratedDisplay.ValueToString(0.050f, &valueString);
    EXPECT_STREQ("50.0 mHz", valueString.CString());

    decoratedDisplay.ValueToString(0.0001f, &valueString);
    EXPECT_STREQ("100.0 uHz", valueString.CString());
}

TEST_F(AAX_Migrated_UnitPrefixDisplayDelegateDecorator_Test, IntToString)
{
    AAX_CNumberDisplayDelegate<int32_t, 0, 1> numberDisplayDelegate;
    AAX_CUnitDisplayDelegateDecorator<int32_t> decoratedDisplay(numberDisplayDelegate, AAX_CString("Cents"));
    AAX_CString valueString;

    decoratedDisplay.ValueToString(1, &valueString);
    EXPECT_STREQ("1 Cents", valueString.CString());

    decoratedDisplay.ValueToString(1000, &valueString);
    EXPECT_STREQ("1000 Cents", valueString.CString());
}

TEST_F(AAX_Migrated_UnitPrefixDisplayDelegateDecorator_Test, SingleStringToFloat)
{
    AAX_CNumberDisplayDelegate<float, 1, 1> numberDisplayDelegate;
    AAX_CUnitPrefixDisplayDelegateDecorator<float> prefixDecorator(numberDisplayDelegate);
    AAX_CUnitDisplayDelegateDecorator<float> decoratedDisplay(prefixDecorator, AAX_CString("miles"));
    AAX_CString valueString("1.1 miles");
    float convertedValue = 0;

    EXPECT_TRUE(decoratedDisplay.StringToValue(valueString, &convertedValue));
    EXPECT_NEAR(1.1f, convertedValue, 0.001f);
}

TEST_F(AAX_Migrated_UnitPrefixDisplayDelegateDecorator_Test, DoubleStringToFloat)
{
    AAX_CNumberDisplayDelegate<float, 1, 1> numberDisplayDelegate;
    AAX_CUnitPrefixDisplayDelegateDecorator<float> prefixDecorator(numberDisplayDelegate);
    AAX_CUnitDisplayDelegateDecorator<float> decoratedDisplay(prefixDecorator, AAX_CString("miles"));
    AAX_CString valueString("-1.1 miles");
    float convertedValue = 0;

    EXPECT_TRUE(decoratedDisplay.StringToValue(valueString, &convertedValue));
    EXPECT_NEAR(-1.1f, convertedValue, 0.001f);
}

TEST_F(AAX_Migrated_UnitPrefixDisplayDelegateDecorator_Test, ProblemStringToFloat)
{
    AAX_CNumberDisplayDelegate<float, 1, 1> numberDisplayDelegate;
    AAX_CUnitPrefixDisplayDelegateDecorator<float> prefixDecorator(numberDisplayDelegate);
    AAX_CUnitDisplayDelegateDecorator<float> decoratedDisplay(prefixDecorator, AAX_CString("miles"));
    AAX_CString valueString("-1.1 miles");
    float convertedValue = 0;

    EXPECT_TRUE(decoratedDisplay.StringToValue(valueString, &convertedValue));
    EXPECT_NEAR(-1.1f, convertedValue, 0.001f);
}

TEST_F(AAX_Migrated_UnitPrefixDisplayDelegateDecorator_Test, ProblemSpacedStringToFloat)
{
    AAX_CNumberDisplayDelegate<float, 1, 1> numberDisplayDelegate;
    AAX_CUnitPrefixDisplayDelegateDecorator<float> prefixDecorator(numberDisplayDelegate);
    AAX_CUnitDisplayDelegateDecorator<float> decoratedDisplay(prefixDecorator, AAX_CString("miles"));
    AAX_CString valueString("-1.1miles"); // problem because there is no space between the number and the units.
    float convertedValue = 0;

    EXPECT_TRUE(decoratedDisplay.StringToValue(valueString, &convertedValue));
    EXPECT_NEAR(-1.1f, convertedValue, 0.001f);
}

TEST_F(AAX_Migrated_UnitPrefixDisplayDelegateDecorator_Test, StringToPositiveInt)
{
    AAX_CNumberDisplayDelegate<int32_t> numberDisplayDelegate;
    AAX_CUnitDisplayDelegateDecorator<int32_t> decoratedDisplay(numberDisplayDelegate, AAX_CString("Cents"));
    AAX_CString valueString("150 Cents");
    int32_t convertedValue = 0;

    EXPECT_TRUE(decoratedDisplay.StringToValue(valueString, &convertedValue));
    EXPECT_EQ(150, convertedValue);
}

TEST_F(AAX_Migrated_UnitPrefixDisplayDelegateDecorator_Test, StringWithoutUnitsToNegativeInt)
{
    // What if a user doesn't enter the units, it should detect this and still work.
    AAX_CNumberDisplayDelegate<int32_t> numberDisplayDelegate;
    AAX_CUnitDisplayDelegateDecorator<int32_t> decoratedDisplay(numberDisplayDelegate, AAX_CString("C"));
    AAX_CString valueString("150");
    int32_t convertedValue = 0;

    EXPECT_TRUE(decoratedDisplay.StringToValue(valueString, &convertedValue));
    EXPECT_EQ(150, convertedValue);
}

TEST_F(AAX_Migrated_UnitPrefixDisplayDelegateDecorator_Test, BadStringToInt)
{
    AAX_CNumberDisplayDelegate<int32_t> numberDisplayDelegate;
    AAX_CUnitDisplayDelegateDecorator<int32_t> decoratedDisplay(numberDisplayDelegate, AAX_CString("C"));
    AAX_CString valueString("abc-1 C");
    int32_t convertedValue = -1;

    EXPECT_FALSE(decoratedDisplay.StringToValue(valueString, &convertedValue));
    EXPECT_EQ(0, convertedValue);
}

TEST_F(AAX_Migrated_UnitPrefixDisplayDelegateDecorator_Test, BadStringToFloat)
{
    AAX_CNumberDisplayDelegate<int32_t> numberDisplayDelegate;
    AAX_CUnitDisplayDelegateDecorator<int32_t> decoratedDisplay(numberDisplayDelegate, AAX_CString("C"));
    AAX_CString valueString("BadString");
    int32_t convertedValue = -1;

    EXPECT_FALSE(decoratedDisplay.StringToValue(valueString, &convertedValue));
    EXPECT_EQ(0, convertedValue);
}

TEST_F(AAX_Migrated_UnitPrefixDisplayDelegateDecorator_Test, NullStringConversion)
{
    AAX_CNumberDisplayDelegate<int32_t> numberDisplayDelegate;
    AAX_CUnitDisplayDelegateDecorator<int32_t> decoratedDisplay(numberDisplayDelegate, AAX_CString("C"));
    AAX_CString valueString("");
    int32_t convertedValue = -1;

    EXPECT_FALSE(decoratedDisplay.StringToValue(valueString, &convertedValue));
    EXPECT_EQ(0, convertedValue);
}

//==============================================================================
// Decibel Display Delegate Decorator Tests
//==============================================================================

class AAX_Migrated_DecibelDisplayDelegateDecorator_Test : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(AAX_Migrated_DecibelDisplayDelegateDecorator_Test, FloatToStringSingleFractional)
{
    AAX_CNumberDisplayDelegate<float, 1, 1> numberDisplayDelegate;
    AAX_CDecibelDisplayDelegateDecorator<float> decoratedDisplay(numberDisplayDelegate);
    AAX_CString valueString;

    decoratedDisplay.ValueToString(1.0f, &valueString);
    EXPECT_STREQ("0.0 dB", valueString.CString());

    decoratedDisplay.ValueToString(0, &valueString);
    EXPECT_STREQ("-INF dB", valueString.CString());

    decoratedDisplay.ValueToString(2, &valueString);
    EXPECT_STREQ("6.0 dB", valueString.CString());

    decoratedDisplay.ValueToString(10, &valueString);
    EXPECT_STREQ("20.0 dB", valueString.CString());

    decoratedDisplay.ValueToString(1000, &valueString);
    EXPECT_STREQ("60.0 dB", valueString.CString());

    decoratedDisplay.ValueToString(0.01f, &valueString);
    EXPECT_STREQ("-40.0 dB", valueString.CString());
}

TEST_F(AAX_Migrated_DecibelDisplayDelegateDecorator_Test, SingleStringToFloat)
{
    AAX_CNumberDisplayDelegate<float, 1, 1> numberDisplayDelegate;
    AAX_CDecibelDisplayDelegateDecorator<float> decoratedDisplay(numberDisplayDelegate);
    float convertedValue = 0;

    AAX_CString valueString("0.0 dB");
    EXPECT_TRUE(decoratedDisplay.StringToValue(valueString, &convertedValue));
    EXPECT_NEAR(1, convertedValue, 0.001f);

    valueString = AAX_CString("6.0 dB");
    convertedValue = 0;
    EXPECT_TRUE(decoratedDisplay.StringToValue(valueString, &convertedValue));
    EXPECT_NEAR(1.995f, convertedValue, 0.001f);

    valueString = AAX_CString("60.0 dB");
    convertedValue = 0;
    EXPECT_TRUE(decoratedDisplay.StringToValue(valueString, &convertedValue));
    EXPECT_NEAR(1000, convertedValue, 0.001f);

    valueString = AAX_CString("20.0 dB");
    convertedValue = 0;
    EXPECT_TRUE(decoratedDisplay.StringToValue(valueString, &convertedValue));
    EXPECT_NEAR(10, convertedValue, 0.001f);

    valueString = AAX_CString("-120 dB");
    convertedValue = 0;
    EXPECT_TRUE(decoratedDisplay.StringToValue(valueString, &convertedValue));
    EXPECT_NEAR(0.000001, convertedValue, 0.001f);

    valueString = AAX_CString("-40.0 dB");
    convertedValue = 0;
    EXPECT_TRUE(decoratedDisplay.StringToValue(valueString, &convertedValue));
    EXPECT_NEAR(0.01, convertedValue, 0.001f);
}

TEST_F(AAX_Migrated_DecibelDisplayDelegateDecorator_Test, ProblemSpacedStringToFloat)
{
    AAX_CNumberDisplayDelegate<float, 1, 1> numberDisplayDelegate;
    AAX_CDecibelDisplayDelegateDecorator<float> decoratedDisplay(numberDisplayDelegate);
    AAX_CString valueString("0.0dB"); // problem because there is no space between the number and the units.
    float convertedValue = 0;

    EXPECT_TRUE(decoratedDisplay.StringToValue(valueString, &convertedValue));
    EXPECT_NEAR(1, convertedValue, 0.001f);
}

TEST_F(AAX_Migrated_DecibelDisplayDelegateDecorator_Test, BadStringToFloat)
{
    AAX_CNumberDisplayDelegate<float, 1> numberDisplayDelegate;
    AAX_CDecibelDisplayDelegateDecorator<float> decoratedDisplay(numberDisplayDelegate);
    AAX_CString valueString("BadString");
    float convertedValue = 0;

    EXPECT_FALSE(decoratedDisplay.StringToValue(valueString, &convertedValue));
    EXPECT_EQ(1, convertedValue);
}

TEST_F(AAX_Migrated_DecibelDisplayDelegateDecorator_Test, NullStringConversion)
{
    AAX_CNumberDisplayDelegate<float, 1> numberDisplayDelegate;
    AAX_CDecibelDisplayDelegateDecorator<float> decoratedDisplay(numberDisplayDelegate);
    AAX_CString valueString("");
    float convertedValue = 0;

    EXPECT_FALSE(decoratedDisplay.StringToValue(valueString, &convertedValue));
    EXPECT_EQ(1, convertedValue);
}

//==============================================================================
// Shortened Decibel Display Tests
//==============================================================================

class AAX_Migrated_ShortenedDecibelDisplay_Test : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(AAX_Migrated_ShortenedDecibelDisplay_Test, ShortenedDecibelLong)
{
    AAX_CNumberDisplayDelegate<float, 1, 1> numberDisplayDelegate;
    AAX_CDecibelDisplayDelegateDecorator<float> decoratedDisplay(numberDisplayDelegate);
    AAX_CString valueString;

    decoratedDisplay.ValueToString(1.0f, 10, &valueString);
    EXPECT_STREQ("0.0 dB", valueString.CString());

    decoratedDisplay.ValueToString(0, 10, &valueString);
    EXPECT_STREQ("-INF dB", valueString.CString());

    decoratedDisplay.ValueToString(2, 10, &valueString);
    EXPECT_STREQ("6.0 dB", valueString.CString());

    decoratedDisplay.ValueToString(10, 10, &valueString);
    EXPECT_STREQ("20.0 dB", valueString.CString());

    decoratedDisplay.ValueToString(1000, 10, &valueString);
    EXPECT_STREQ("60.0 dB", valueString.CString());

    decoratedDisplay.ValueToString(0.01f, 10, &valueString);
    EXPECT_STREQ("-40.0 dB", valueString.CString());
}

TEST_F(AAX_Migrated_ShortenedDecibelDisplay_Test, ShortenedDecibelMedium)
{
    AAX_CNumberDisplayDelegate<float, 1, 1> numberDisplayDelegate;
    AAX_CDecibelDisplayDelegateDecorator<float> decoratedDisplay(numberDisplayDelegate);
    AAX_CString valueString;

    decoratedDisplay.ValueToString(1.0f, 6, &valueString);
    EXPECT_STREQ("0.0 dB", valueString.CString());

    decoratedDisplay.ValueToString(0, 6, &valueString);
    EXPECT_STREQ("-INF", valueString.CString());

    decoratedDisplay.ValueToString(2, 6, &valueString);
    EXPECT_STREQ("6.0 dB", valueString.CString());

    decoratedDisplay.ValueToString(10, 6, &valueString);
    EXPECT_STREQ("20.0dB", valueString.CString());

    decoratedDisplay.ValueToString(1000, 6, &valueString);
    EXPECT_STREQ("60.0dB", valueString.CString());

    decoratedDisplay.ValueToString(0.01f, 6, &valueString);
    EXPECT_STREQ("-40.0", valueString.CString());
}

TEST_F(AAX_Migrated_ShortenedDecibelDisplay_Test, ShortenedDecibelShort)
{
    AAX_CNumberDisplayDelegate<float, 1, 1> numberDisplayDelegate;
    AAX_CDecibelDisplayDelegateDecorator<float> decoratedDisplay(numberDisplayDelegate);
    AAX_CString valueString;

    decoratedDisplay.ValueToString(1.0f, 4, &valueString);
    EXPECT_STREQ("0.0", valueString.CString());

    decoratedDisplay.ValueToString(0, 4, &valueString);
    EXPECT_STREQ("-INF", valueString.CString());

    decoratedDisplay.ValueToString(2, 4, &valueString);
    EXPECT_STREQ("6.0", valueString.CString());

    decoratedDisplay.ValueToString(10, 4, &valueString);
    EXPECT_STREQ("20.0", valueString.CString());

    decoratedDisplay.ValueToString(1000, 4, &valueString);
    EXPECT_STREQ("60.0", valueString.CString());

    decoratedDisplay.ValueToString(0.01f, 4, &valueString);
    EXPECT_STREQ("-40", valueString.CString());
}

TEST_F(AAX_Migrated_ShortenedDecibelDisplay_Test, ShortenedDecibelMediumHigherPrecision)
{
    AAX_CNumberDisplayDelegate<float, 2, 1> numberDisplayDelegate;
    AAX_CDecibelDisplayDelegateDecorator<float> decoratedDisplay(numberDisplayDelegate);
    AAX_CString valueString;

    decoratedDisplay.ValueToString(1.0f, 6, &valueString);
    EXPECT_STREQ("0.00dB", valueString.CString());

    decoratedDisplay.ValueToString(0, 6, &valueString);
    EXPECT_STREQ("-INF", valueString.CString());

    decoratedDisplay.ValueToString(2, 6, &valueString);
    EXPECT_STREQ("6.02dB", valueString.CString());

    decoratedDisplay.ValueToString(10, 6, &valueString);
    EXPECT_STREQ("20.00", valueString.CString());

    decoratedDisplay.ValueToString(1000, 6, &valueString);
    EXPECT_STREQ("60.00", valueString.CString());

    decoratedDisplay.ValueToString(0.01f, 6, &valueString);
    EXPECT_STREQ("-40.00", valueString.CString());
}

TEST_F(AAX_Migrated_ShortenedDecibelDisplay_Test, ShortenedDecibelShortHigherPrecision)
{
    AAX_CNumberDisplayDelegate<float, 2, 1> numberDisplayDelegate;
    AAX_CDecibelDisplayDelegateDecorator<float> decoratedDisplay(numberDisplayDelegate);
    AAX_CString valueString;

    decoratedDisplay.ValueToString(1.0f, 4, &valueString);
    EXPECT_STREQ("0.00", valueString.CString());

    decoratedDisplay.ValueToString(0, 4, &valueString);
    EXPECT_STREQ("-INF", valueString.CString());

    decoratedDisplay.ValueToString(2, 4, &valueString);
    EXPECT_STREQ("6.02", valueString.CString());

    decoratedDisplay.ValueToString(10, 4, &valueString);
    EXPECT_STREQ("20.0", valueString.CString());

    decoratedDisplay.ValueToString(1000, 4, &valueString);
    EXPECT_STREQ("60.0", valueString.CString());

    decoratedDisplay.ValueToString(0.01f, 4, &valueString);
    EXPECT_STREQ("-40", valueString.CString());
}

//==============================================================================
// Shortened kHz Tests
//==============================================================================

class AAX_Migrated_ShortenedkHz_Test : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(AAX_Migrated_ShortenedkHz_Test, ShortenedkHzLong)
{
    AAX_CNumberDisplayDelegate<float, 1, 1> numberDisplayDelegate;
    AAX_CUnitPrefixDisplayDelegateDecorator<float> prefixDecoratedDisplay(numberDisplayDelegate);
    AAX_CUnitDisplayDelegateDecorator<float> decoratedDisplay(prefixDecoratedDisplay, AAX_CString("Hz"));
    AAX_CString valueString;
    const int32_t cNumChars = 10;

    decoratedDisplay.ValueToString(1.0f, cNumChars, &valueString);
    EXPECT_STREQ("1.0 Hz", valueString.CString());

    decoratedDisplay.ValueToString(0, cNumChars, &valueString);
    EXPECT_STREQ("0.0 Hz", valueString.CString());

    decoratedDisplay.ValueToString(500, cNumChars, &valueString);
    EXPECT_STREQ("500.0 Hz", valueString.CString());

    decoratedDisplay.ValueToString(1000, cNumChars, &valueString);
    EXPECT_STREQ("1.0 kHz", valueString.CString());

    decoratedDisplay.ValueToString(5000, cNumChars, &valueString);
    EXPECT_STREQ("5.0 kHz", valueString.CString());

    decoratedDisplay.ValueToString(10000, cNumChars, &valueString);
    EXPECT_STREQ("10.0 kHz", valueString.CString());
}

TEST_F(AAX_Migrated_ShortenedkHz_Test, ShortenedkHzMedium)
{
    AAX_CNumberDisplayDelegate<float, 1, 1> numberDisplayDelegate;
    AAX_CUnitPrefixDisplayDelegateDecorator<float> prefixDecoratedDisplay(numberDisplayDelegate);
    AAX_CUnitDisplayDelegateDecorator<float> decoratedDisplay(prefixDecoratedDisplay, AAX_CString("Hz"));
    AAX_CString valueString;
    const int32_t cNumChars = 6;

    decoratedDisplay.ValueToString(1.0f, cNumChars, &valueString);
    EXPECT_STREQ("1.0 Hz", valueString.CString());

    decoratedDisplay.ValueToString(0, cNumChars, &valueString);
    EXPECT_STREQ("0.0 Hz", valueString.CString());

    decoratedDisplay.ValueToString(500, cNumChars, &valueString);
    EXPECT_STREQ("500.0", valueString.CString());

    decoratedDisplay.ValueToString(1000, cNumChars, &valueString);
    EXPECT_STREQ("1.0kHz", valueString.CString());

    decoratedDisplay.ValueToString(5000, cNumChars, &valueString);
    EXPECT_STREQ("5.0kHz", valueString.CString());

    decoratedDisplay.ValueToString(10000, cNumChars, &valueString);
    EXPECT_STREQ("10.0k", valueString.CString());
}

TEST_F(AAX_Migrated_ShortenedkHz_Test, ShortenedkHzShort)
{
    AAX_CNumberDisplayDelegate<float, 1, 1> numberDisplayDelegate;
    AAX_CUnitPrefixDisplayDelegateDecorator<float> prefixDecoratedDisplay(numberDisplayDelegate);
    AAX_CUnitDisplayDelegateDecorator<float> decoratedDisplay(prefixDecoratedDisplay, AAX_CString("Hz"));
    AAX_CString valueString;
    const int32_t cNumChars = 4;

    decoratedDisplay.ValueToString(1.0f, cNumChars, &valueString);
    EXPECT_STREQ("1.0", valueString.CString());

    decoratedDisplay.ValueToString(0, cNumChars, &valueString);
    EXPECT_STREQ("0.0", valueString.CString());

    decoratedDisplay.ValueToString(500, cNumChars, &valueString);
    EXPECT_STREQ("500", valueString.CString());

    decoratedDisplay.ValueToString(1000, cNumChars, &valueString);
    EXPECT_STREQ("1.0k", valueString.CString());

    decoratedDisplay.ValueToString(5000, cNumChars, &valueString);
    EXPECT_STREQ("5.0k", valueString.CString());

    decoratedDisplay.ValueToString(10000, cNumChars, &valueString);
    EXPECT_STREQ("10k", valueString.CString());
}

TEST_F(AAX_Migrated_ShortenedkHz_Test, HighPrecisionShortenedkHzLong)
{
    AAX_CNumberDisplayDelegate<float, 2, 1> numberDisplayDelegate;
    AAX_CUnitPrefixDisplayDelegateDecorator<float> prefixDecoratedDisplay(numberDisplayDelegate);
    AAX_CUnitDisplayDelegateDecorator<float> decoratedDisplay(prefixDecoratedDisplay, AAX_CString("Hz"));
    AAX_CString valueString;
    const int32_t cNumChars = 10;

    decoratedDisplay.ValueToString(1.0f, cNumChars, &valueString);
    EXPECT_STREQ("1.00 Hz", valueString.CString());

    decoratedDisplay.ValueToString(0, cNumChars, &valueString);
    EXPECT_STREQ("0.00 Hz", valueString.CString());

    decoratedDisplay.ValueToString(500, cNumChars, &valueString);
    EXPECT_STREQ("500.00 Hz", valueString.CString());

    decoratedDisplay.ValueToString(1000, cNumChars, &valueString);
    EXPECT_STREQ("1.00 kHz", valueString.CString());

    decoratedDisplay.ValueToString(5000, cNumChars, &valueString);
    EXPECT_STREQ("5.00 kHz", valueString.CString());

    decoratedDisplay.ValueToString(10000, cNumChars, &valueString);
    EXPECT_STREQ("10.00 kHz", valueString.CString());
}

TEST_F(AAX_Migrated_ShortenedkHz_Test, HighPrecisionShortenedkHzMedium)
{
    AAX_CNumberDisplayDelegate<float, 2, 1> numberDisplayDelegate;
    AAX_CUnitPrefixDisplayDelegateDecorator<float> prefixDecoratedDisplay(numberDisplayDelegate);
    AAX_CUnitDisplayDelegateDecorator<float> decoratedDisplay(prefixDecoratedDisplay, AAX_CString("Hz"));
    AAX_CString valueString;
    const int32_t cNumChars = 6;

    decoratedDisplay.ValueToString(1.0f, cNumChars, &valueString);
    EXPECT_STREQ("1.00Hz", valueString.CString());

    decoratedDisplay.ValueToString(0, cNumChars, &valueString);
    EXPECT_STREQ("0.00Hz", valueString.CString());

    decoratedDisplay.ValueToString(500, cNumChars, &valueString);
    EXPECT_STREQ("500.00", valueString.CString());

    decoratedDisplay.ValueToString(1000, cNumChars, &valueString);
    EXPECT_STREQ("1.00k", valueString.CString());

    decoratedDisplay.ValueToString(5000, cNumChars, &valueString);
    EXPECT_STREQ("5.00k", valueString.CString());

    decoratedDisplay.ValueToString(10000, cNumChars, &valueString);
    EXPECT_STREQ("10.00k", valueString.CString());
}

TEST_F(AAX_Migrated_ShortenedkHz_Test, HighPrecisionShortenedkHzShort)
{
    AAX_CNumberDisplayDelegate<float, 2, 1> numberDisplayDelegate;
    AAX_CUnitPrefixDisplayDelegateDecorator<float> prefixDecoratedDisplay(numberDisplayDelegate);
    AAX_CUnitDisplayDelegateDecorator<float> decoratedDisplay(prefixDecoratedDisplay, AAX_CString("Hz"));
    AAX_CString valueString;
    const int32_t cNumChars = 4;

    decoratedDisplay.ValueToString(1.0f, cNumChars, &valueString);
    EXPECT_STREQ("1.00", valueString.CString());

    decoratedDisplay.ValueToString(0, cNumChars, &valueString);
    EXPECT_STREQ("0.00", valueString.CString());

    decoratedDisplay.ValueToString(500, cNumChars, &valueString);
    EXPECT_STREQ("500", valueString.CString());

    decoratedDisplay.ValueToString(1000, cNumChars, &valueString);
    EXPECT_STREQ("1.0k", valueString.CString());

    decoratedDisplay.ValueToString(5000, cNumChars, &valueString);
    EXPECT_STREQ("5.0k", valueString.CString());

    decoratedDisplay.ValueToString(10000, cNumChars, &valueString);
    EXPECT_STREQ("10k", valueString.CString());
}

//==============================================================================
// Shortened Percentage Tests
//==============================================================================

class AAX_Migrated_ShortenedPercentage_Test : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(AAX_Migrated_ShortenedPercentage_Test, ShortenedPercentageLong)
{
    AAX_CNumberDisplayDelegate<float, 2, 0> numberDisplayDelegate;
    AAX_CPercentDisplayDelegateDecorator<float> decoratedDisplay(numberDisplayDelegate);
    AAX_CString valueString;
    const int32_t cNumChars = 8;

    decoratedDisplay.ValueToString(1.0f, cNumChars, &valueString);
    EXPECT_STREQ("100.00%", valueString.CString());

    decoratedDisplay.ValueToString(0, cNumChars, &valueString);
    EXPECT_STREQ("0.00%", valueString.CString());

    decoratedDisplay.ValueToString(5, cNumChars, &valueString);
    EXPECT_STREQ("500.00%", valueString.CString());

    decoratedDisplay.ValueToString(-.5f, cNumChars, &valueString);
    EXPECT_STREQ("-50.00%", valueString.CString());
}

TEST_F(AAX_Migrated_ShortenedPercentage_Test, ShortenedPercentageMedium)
{
    AAX_CNumberDisplayDelegate<float, 2, 0> numberDisplayDelegate;
    AAX_CPercentDisplayDelegateDecorator<float> decoratedDisplay(numberDisplayDelegate);
    AAX_CString valueString;
    const int32_t cNumChars = 6;

    decoratedDisplay.ValueToString(1.0f, cNumChars, &valueString);
    EXPECT_STREQ("100.0%", valueString.CString());

    decoratedDisplay.ValueToString(0, cNumChars, &valueString);
    EXPECT_STREQ("0.00%", valueString.CString());

    decoratedDisplay.ValueToString(5, cNumChars, &valueString);
    EXPECT_STREQ("500.0%", valueString.CString());

    decoratedDisplay.ValueToString(-.5f, cNumChars, &valueString);
    EXPECT_STREQ("-50.0%", valueString.CString());
}

TEST_F(AAX_Migrated_ShortenedPercentage_Test, ShortenedPercentageShort)
{
    AAX_CNumberDisplayDelegate<float, 2, 0> numberDisplayDelegate;
    AAX_CPercentDisplayDelegateDecorator<float> decoratedDisplay(numberDisplayDelegate);
    AAX_CString valueString;
    const int32_t cNumChars = 4;

    decoratedDisplay.ValueToString(1.0f, cNumChars, &valueString);
    EXPECT_STREQ("100%", valueString.CString());

    decoratedDisplay.ValueToString(0, cNumChars, &valueString);
    EXPECT_STREQ("0.0%", valueString.CString());

    decoratedDisplay.ValueToString(5, cNumChars, &valueString);
    EXPECT_STREQ("500%", valueString.CString());

    decoratedDisplay.ValueToString(-.5f, cNumChars, &valueString);
    EXPECT_STREQ("-50%", valueString.CString());
}
