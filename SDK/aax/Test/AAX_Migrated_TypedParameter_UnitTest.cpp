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
 *	\file AAX_Migrated_TypedParameter_UnitTest.cpp
 *
 *	\brief Migrated unit tests for AAX_CParameter (originally from UnitTest++ framework)
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX_CParameter.h"
#include "AAX_CBinaryDisplayDelegate.h"
#include "AAX_CLinearTaperDelegate.h"
#include "AAX_CBinaryTaperDelegate.h"
#include "AAX_CNumberDisplayDelegate.h"
#include "AAX_CUnitDisplayDelegateDecorator.h"
#include "AAX_CUnitPrefixDisplayDelegateDecorator.h"
#include "AAX_CStateDisplayDelegate.h"
#include "AAX_CStringDisplayDelegate.h"
#include <map>

//==============================================================================
// Test Fixture
//==============================================================================

class AAX_Migrated_TypedParameter_Test : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // No specific setup needed for typed parameter tests
    }
    
    void TearDown() override
    {
        // No specific cleanup needed for typed parameter tests
    }
};

//==============================================================================
// Basic Parameter Tests
//==============================================================================

TEST_F(AAX_Migrated_TypedParameter_Test, NameAndIdentifier)
{
    AAX_CLinearTaperDelegate<float> taper(-100, 100);
    AAX_CNumberDisplayDelegate<float> display;
    AAX_CParameter<float> param("1", AAX_CString("Linearparameter"), 0, taper, display);
    
    EXPECT_STREQ("Linearparameter", param.Name().CString());
    EXPECT_STREQ("1", param.Identifier());
    
    param.SetName(AAX_CString("NewName"));
    EXPECT_STREQ("NewName", param.Name().CString());
}

TEST_F(AAX_Migrated_TypedParameter_Test, ShortenedName)
{
    AAX_CLinearTaperDelegate<float> taper(-100, 100);
    AAX_CNumberDisplayDelegate<float> display;
    AAX_CParameter<float> param("1", AAX_CString("LongName"), 0, taper, display);
    
    // Before setting shortened names, it should always return long name.
    EXPECT_STREQ("LongName", param.ShortenedName(12).CString());
    EXPECT_STREQ("LongName", param.ShortenedName(8).CString());
    EXPECT_STREQ("LongName", param.ShortenedName(4).CString());
    EXPECT_STREQ("LongName", param.ShortenedName(0).CString());

    // Set a single shortened names...
    param.AddShortenedName(AAX_CString("LongNm"));
    EXPECT_STREQ("LongName", param.ShortenedName(8).CString());
    EXPECT_STREQ("LongNm", param.ShortenedName(7).CString());
    EXPECT_STREQ("LongNm", param.ShortenedName(6).CString());
    EXPECT_STREQ("LongNm", param.ShortenedName(5).CString()); // This should still return the shortest name available... (edge case)
    EXPECT_STREQ("LongNm", param.ShortenedName(0).CString()); // This should still return the shortest name available... (edge case)

    // Set a single shortened names...
    param.AddShortenedName(AAX_CString("LngNm"));
    EXPECT_STREQ("LongName", param.ShortenedName(8).CString());
    EXPECT_STREQ("LongNm", param.ShortenedName(7).CString());
    EXPECT_STREQ("LongNm", param.ShortenedName(6).CString());
    EXPECT_STREQ("LngNm", param.ShortenedName(5).CString());
    EXPECT_STREQ("LngNm", param.ShortenedName(0).CString()); // This should still return the shortest name available... (edge case)
    
    // Set a single shortened names...
    param.AddShortenedName(AAX_CString("L"));
    EXPECT_STREQ("L", param.ShortenedName(0).CString()); // This should still return the shortest name available... (edge case)
    EXPECT_STREQ("L", param.ShortenedName(1).CString());
    EXPECT_STREQ("L", param.ShortenedName(2).CString());
    EXPECT_STREQ("LngNm", param.ShortenedName(5).CString());
    EXPECT_STREQ("LongNm", param.ShortenedName(6).CString());
    EXPECT_STREQ("LongNm", param.ShortenedName(7).CString());
    EXPECT_STREQ("LongName", param.ShortenedName(8).CString());
    EXPECT_STREQ("LongName", param.ShortenedName(9).CString());
    
    param.SetName(AAX_CString("NewName"));
    param.ClearShortenedNames();

    // Before setting shortened names, it should always return long name.
    EXPECT_STREQ("NewName", param.ShortenedName(12).CString());
    EXPECT_STREQ("NewName", param.ShortenedName(8).CString());
    EXPECT_STREQ("NewName", param.ShortenedName(4).CString());
    EXPECT_STREQ("NewName", param.ShortenedName(0).CString());
}

TEST_F(AAX_Migrated_TypedParameter_Test, InitializedValue)
{
    AAX_CLinearTaperDelegate<float> taper(-100, 100);
    AAX_CNumberDisplayDelegate<float> display;
    AAX_CParameter<float> param("1", AAX_CString("Linearparameter"), 10, taper, display);
    
    EXPECT_EQ(10, param.GetValue());
}

TEST_F(AAX_Migrated_TypedParameter_Test, SetValueFunction)
{
    AAX_CLinearTaperDelegate<float> taper(-100, 100);
    AAX_CNumberDisplayDelegate<float> display;
    AAX_CParameter<float> param("1", AAX_CString("Linearparameter"), 10, taper, display);
    
    // Check many valid values within range.
    EXPECT_EQ(10, param.GetValue());
    for (float newValue = -100; newValue < 100; newValue += 0.1f)
    {
        param.SetValue(newValue);
        EXPECT_NEAR(newValue, param.GetValue(), 0.001f);
    }
    
    // Check outside of bounds setting.
    param.SetValue(-1000);
    EXPECT_EQ(-100, param.GetValue());
    param.SetValue(1000);
    EXPECT_EQ(100, param.GetValue());
}

TEST_F(AAX_Migrated_TypedParameter_Test, SetToDefaultValueFunction)
{
    AAX_CLinearTaperDelegate<float> taper(-100, 100);
    AAX_CNumberDisplayDelegate<float> display;
    AAX_CParameter<float> param("1", AAX_CString("Linearparameter"), 10, taper, display);
    
    param.SetValue(25);
    EXPECT_EQ(25, param.GetValue());
    param.SetToDefaultValue();
    EXPECT_EQ(10, param.GetValue());
}

TEST_F(AAX_Migrated_TypedParameter_Test, SetDefaultValueFunction)
{
    AAX_CLinearTaperDelegate<float> taper(-100, 100);
    AAX_CNumberDisplayDelegate<float> display;
    AAX_CParameter<float> param("1", AAX_CString("Linearparameter"), 10, taper, display);
    
    EXPECT_EQ(10, param.GetDefaultValue());
    param.SetValue(25);
    param.SetToDefaultValue();
    EXPECT_EQ(10, param.GetValue());
    param.SetDefaultValue(55);
    param.SetToDefaultValue();
    EXPECT_EQ(55, param.GetValue());
    EXPECT_EQ(55, param.GetDefaultValue());

    // Check negative out of bounds default values.
    param.SetDefaultValue(-200);
    EXPECT_EQ(-100, param.GetDefaultValue()); // default should clamp to the bounds.
    param.SetToDefaultValue();                // This should set to the clamped value.
    EXPECT_EQ(-100, param.GetValue());

    // Check positive out of bounds default values.
    param.SetDefaultValue(200);
    EXPECT_EQ(100, param.GetDefaultValue()); // default should clamp to the bounds.
    param.SetToDefaultValue();               // This should set to the clamped value.
    EXPECT_EQ(100, param.GetValue());
}

TEST_F(AAX_Migrated_TypedParameter_Test, SetNormalizedDefaultValueFunction)
{
    AAX_CLinearTaperDelegate<float> taper(0, 10);
    AAX_CNumberDisplayDelegate<float> display;
    AAX_CParameter<float> typedParam("1", AAX_CString("Linearparameter"), 5, taper, display);
    AAX_IParameter* param = static_cast<AAX_IParameter*>(&typedParam);
    
    EXPECT_EQ(0.5, param->GetNormalizedDefaultValue());
    param->SetNormalizedValue(1.0);
    param->SetToDefaultValue();
    EXPECT_EQ(0.5, param->GetNormalizedValue());
    param->SetNormalizedDefaultValue(0.9);
    param->SetToDefaultValue();
    EXPECT_EQ(0.9, param->GetNormalizedValue());
    EXPECT_EQ(0.9, param->GetNormalizedDefaultValue());

    // Check negative out of bounds default values.
    param->SetNormalizedDefaultValue(-2);
    EXPECT_EQ(0.0, param->GetNormalizedDefaultValue()); // default should clamp to the bounds.
    param->SetToDefaultValue();                         // This should set to the clamped value.
    EXPECT_EQ(0.0, param->GetNormalizedValue());

    // Check positive out of bounds default values.
    param->SetNormalizedDefaultValue(2);
    EXPECT_EQ(1.0, param->GetNormalizedDefaultValue()); // default should clamp to the bounds.
    param->SetToDefaultValue();                         // This should set to the clamped value.
    EXPECT_EQ(1.0, param->GetNormalizedValue());
}

TEST_F(AAX_Migrated_TypedParameter_Test, NumberOfSteps)
{
    AAX_CLinearTaperDelegate<float> taper(-100, 100);
    AAX_CNumberDisplayDelegate<float> display;
    AAX_CParameter<float> param("1", AAX_CString("Linearparameter"), 0, taper, display);

    EXPECT_EQ(uint32_t(AAX_CParameter<float>::eParameterDefaultNumStepsContinuous), param.GetNumberOfSteps());
    param.SetNumberOfSteps(10);
    EXPECT_EQ(uint32_t(10), param.GetNumberOfSteps());
}

TEST_F(AAX_Migrated_TypedParameter_Test, NormalizedValueFunctions)
{
    AAX_CLinearTaperDelegate<float> taper(-100, 100);
    AAX_CNumberDisplayDelegate<float> display;
    AAX_CParameter<float> param("1", AAX_CString("Linearparameter"), 0, taper, display);
    
    EXPECT_EQ(0.5, param.GetNormalizedValue());
    EXPECT_EQ(0, param.GetValue());
    
    param.SetNormalizedValue(0);
    EXPECT_EQ(0, param.GetNormalizedValue());
    EXPECT_EQ(-100, param.GetValue());
    
    param.SetNormalizedValue(1);
    EXPECT_EQ(1, param.GetNormalizedValue());
    EXPECT_EQ(100, param.GetValue());
}

//==============================================================================
// Precision Tests
//==============================================================================

TEST_F(AAX_Migrated_TypedParameter_Test, IntegerPrecision)
{
    AAX_CLinearTaperDelegate<int32_t, 1> taper(-100, 100);
    AAX_CNumberDisplayDelegate<int32_t> display;
    AAX_CParameter<int32_t> param("1", AAX_CString("Linearparameter"), 0, taper, display);
    param.SetValue(24);
    EXPECT_EQ(24, param.GetValue());

    AAX_IParameter* iparam = static_cast<AAX_IParameter*>(&param);
    EXPECT_TRUE(iparam != nullptr);

    int32_t iValue;
    float fValue;
    bool bValue;
    EXPECT_TRUE(iparam->GetValueAsInt32(&iValue));
    EXPECT_EQ(24, iValue);
    EXPECT_TRUE(iparam->SetValueWithInt32(50));
    EXPECT_EQ(50, param.GetValue());
    EXPECT_FALSE(iparam->SetValueWithFloat(1.0f));
    EXPECT_FALSE(iparam->GetValueAsBool(&bValue));
    EXPECT_FALSE(iparam->GetValueAsFloat(&fValue));
}

TEST_F(AAX_Migrated_TypedParameter_Test, FloatPrecision)
{
    AAX_CLinearTaperDelegate<float, 10> taper(-100, 100);
    AAX_CNumberDisplayDelegate<float> display;
    AAX_CParameter<float> param("1", AAX_CString("Linearparameter"), 0, taper, display);
    param.SetValue(24.167f);
    EXPECT_NEAR(24.2f, param.GetValue(), 0.001f);

    AAX_IParameter* iparam = static_cast<AAX_IParameter*>(&param);
    EXPECT_TRUE(iparam != nullptr);

    int32_t iValue;
    float fValue;
    bool bValue;
    EXPECT_TRUE(iparam->GetValueAsFloat(&fValue));
    EXPECT_NEAR(24.2f, fValue, 0.001f);
    EXPECT_TRUE(iparam->SetValueWithFloat(50.0f));
    EXPECT_EQ(50.0f, param.GetValue());
    EXPECT_FALSE(iparam->SetValueWithInt32(50));
    EXPECT_FALSE(iparam->GetValueAsBool(&bValue));
    EXPECT_FALSE(iparam->GetValueAsInt32(&iValue));
}

TEST_F(AAX_Migrated_TypedParameter_Test, BoolFunctions)
{
    AAX_CBinaryTaperDelegate<bool> taper;
    AAX_CBinaryDisplayDelegate<bool> display("false", "true");
    AAX_CParameter<bool> param("1", AAX_CString("BoolParameter"), false, taper, display);
    param.SetValue(true);
    EXPECT_EQ(true, param.GetValue());

    AAX_IParameter* iparam = static_cast<AAX_IParameter*>(&param);
    EXPECT_TRUE(iparam != nullptr);

    int32_t iValue;
    float fValue;
    bool bValue;
    EXPECT_TRUE(iparam->GetValueAsBool(&bValue));
    EXPECT_EQ(true, bValue);
    EXPECT_TRUE(iparam->SetValueWithBool(false));
    EXPECT_EQ(false, param.GetValue());
    EXPECT_FALSE(iparam->SetValueWithInt32(50));
    EXPECT_FALSE(iparam->SetValueWithFloat(1.0f));
    EXPECT_FALSE(iparam->GetValueAsInt32(&iValue));
    EXPECT_FALSE(iparam->GetValueAsFloat(&fValue));
}

//==============================================================================
// String Conversion Tests
//==============================================================================

TEST_F(AAX_Migrated_TypedParameter_Test, FloatToStringAndBack)
{
    AAX_CLinearTaperDelegate<float> taperDelegate(-50, 30);
    AAX_CNumberDisplayDelegate<float, 100> displayDelegate;
    AAX_CParameter<float> param("1", AAX_CString("Linearparameter"), 10, taperDelegate, displayDelegate);

    for (float newValue = -50; newValue < 30; newValue += 0.1f)
    {
        param.SetValue(newValue);
        AAX_CString valueString;
        param.GetValueString(&valueString);
        EXPECT_TRUE(param.SetValueFromString(valueString));
        if (newValue == 0)
            EXPECT_EQ(newValue, param.GetValue());
        else
            EXPECT_NEAR(newValue, param.GetValue(), 0.001f);
    }
}

TEST_F(AAX_Migrated_TypedParameter_Test, FrequencyToStringAndBack)
{
    AAX_CLinearTaperDelegate<float> taperDelegate(-50, 30);
    AAX_CNumberDisplayDelegate<float, 100> displayDelegate;
    AAX_CUnitDisplayDelegateDecorator<float> decoratedDisplayDelegate(displayDelegate, "Hz");
    AAX_CParameter<float> param("1", AAX_CString("Linearparameter"), 10, taperDelegate, decoratedDisplayDelegate);

    for (float newValue = -50; newValue < 30; newValue += 0.1f)
    {
        param.SetValue(newValue);
        AAX_CString valueString;
        param.GetValueString(&valueString);
        EXPECT_TRUE(param.SetValueFromString(valueString));
        if (newValue == 0)
            EXPECT_EQ(newValue, param.GetValue());
        else
            EXPECT_NEAR(newValue, param.GetValue(), 0.001f);
    }
}

TEST_F(AAX_Migrated_TypedParameter_Test, FrequencyLargeRangeToStringAndBack)
{
    AAX_CLinearTaperDelegate<float> taperDelegate(20, 40000);
    AAX_CNumberDisplayDelegate<float, 100> displayDelegate;
    AAX_CUnitPrefixDisplayDelegateDecorator<float> prefixDecorator(displayDelegate);
    AAX_CUnitDisplayDelegateDecorator<float> decoratedDisplayDelegate(prefixDecorator, "Hz");
    AAX_CParameter<float> param("1", AAX_CString("Linearparameter"), 1000, taperDelegate, decoratedDisplayDelegate);

    for (float newValue = 20; newValue < 40000; newValue += 100.0f)
    {
        param.SetValue(newValue);
        AAX_CString valueString;
        param.GetValueString(&valueString);
        EXPECT_TRUE(param.SetValueFromString(valueString));
        EXPECT_NEAR(newValue, param.GetValue(), 0.01f);
    }
}

TEST_F(AAX_Migrated_TypedParameter_Test, IntToStringAndBack)
{
    // Value String Map
    std::map<int32_t, AAX_CString> stringMap;
    stringMap[-5] = "MinusFive";
    stringMap[0] = "Zero";
    stringMap[1] = "One";

    AAX_CStringDisplayDelegate<int32_t> stringDelegate(stringMap);
    AAX_CLinearTaperDelegate<int32_t> taperDelegate(-5, 1);
    AAX_CParameter<int32_t> param("1", AAX_CString("Linearparameter"), 0, taperDelegate, stringDelegate);
    AAX_CString valueString;

    // value to string
    param.SetValue(0);
    EXPECT_TRUE(param.GetValueString(&valueString));
    EXPECT_STREQ("Zero", valueString.CString());
    param.SetValue(1);
    EXPECT_TRUE(param.GetValueString(&valueString));
    EXPECT_STREQ("One", valueString.CString());
    param.SetValue(-5);
    EXPECT_TRUE(param.GetValueString(&valueString));
    EXPECT_STREQ("MinusFive", valueString.CString());

    // This should clamp at the min value of -5.
    param.SetValue(-10);
    EXPECT_TRUE(param.GetValueString(&valueString));
    EXPECT_STREQ("MinusFive", valueString.CString());

    // This should clamp at the max value of 1.
    param.SetValue(10);
    EXPECT_TRUE(param.GetValueString(&valueString));
    EXPECT_STREQ("One", valueString.CString());

    // This should not find a control string for this in range value.
    param.SetValue(-2);
    EXPECT_FALSE(param.GetValueString(&valueString));
    EXPECT_STREQ("String Not Found", valueString.CString());

    // string to value
    EXPECT_TRUE(param.SetValueFromString(AAX_CString("Zero")));
    EXPECT_EQ(0, param.GetValue());
    EXPECT_TRUE(param.SetValueFromString(AAX_CString("One")));
    EXPECT_EQ(1, param.GetValue());
    EXPECT_TRUE(param.SetValueFromString(AAX_CString("MinusFive")));
    EXPECT_EQ(-5, param.GetValue());

    // bad string value
    param.SetValue(-2);
    EXPECT_FALSE(param.SetValueFromString(AAX_CString("MinusOne")));
    EXPECT_EQ(-2, param.GetValue()); // don't change the value if the string was invalid.
}

//==============================================================================
// Delegate Tests
//==============================================================================

TEST_F(AAX_Migrated_TypedParameter_Test, SetTaperDelegate)
{
    AAX_CLinearTaperDelegate<float> taper(-100, 100);
    AAX_CLinearTaperDelegate<float> newTaper(-200, 20);
    AAX_CNumberDisplayDelegate<float> display;
    AAX_CParameter<float> param("1", AAX_CString("Linearparameter"), 10, taper, display);

    EXPECT_EQ(10, param.GetValue());
    param.SetTaperDelegate(newTaper);
    EXPECT_EQ(10, param.GetValue()); // Value should remain the same if it's within the new range.

    param.SetValue(100);
    param.SetTaperDelegate(newTaper);
    EXPECT_EQ(20, param.GetValue()); // Value should be clamped here since it was out of range of the new taper.
}

TEST_F(AAX_Migrated_TypedParameter_Test, SetDisplayDelegate)
{
    AAX_CLinearTaperDelegate<float> taper(-10000, 10000);
    AAX_CNumberDisplayDelegate<float, 0> display;
    AAX_CUnitPrefixDisplayDelegateDecorator<float> newDisplay(display); // no worries about this, display is cloned.
    AAX_CParameter<float> param("1", AAX_CString("Linearparameter"), 1000, taper, display);

    AAX_CString valueString;
    param.GetValueString(&valueString);
    EXPECT_STREQ("1000", valueString.CString());

    param.SetDisplayDelegate(newDisplay);
    param.GetValueString(&valueString);
    EXPECT_STREQ("1k", valueString.CString());

    param.SetValue(2000);
    param.GetValueString(&valueString);
    EXPECT_STREQ("2k", valueString.CString());

    param.SetToDefaultValue();
    param.GetValueString(&valueString);
    EXPECT_STREQ("1k", valueString.CString());
}

TEST_F(AAX_Migrated_TypedParameter_Test, NumSteps)
{
    AAX_CLinearTaperDelegate<float> taper(-100, 100);
    AAX_CNumberDisplayDelegate<float> display;
    AAX_CParameter<float> param("1", AAX_CString("Linearparameter"), 10, taper, display);

    // Check the actual setting and getting of the numSteps.
    uint32_t numSteps = param.GetNumberOfSteps();
    EXPECT_EQ(uint32_t(AAX_CParameter<float>::eParameterDefaultNumStepsContinuous), numSteps); // initialized value
    param.SetNumberOfSteps(10);
    numSteps = param.GetNumberOfSteps();
    EXPECT_EQ(uint32_t(10), numSteps);
    param.SetNumberOfSteps(10000);
    numSteps = param.GetNumberOfSteps();
    EXPECT_EQ(uint32_t(10000), numSteps);
}

TEST_F(AAX_Migrated_TypedParameter_Test, GetNormalizedValueFromString)
{
    const float cParameterMinValue = -50;
    const float cParameterMaxValue = 30;
    AAX_CLinearTaperDelegate<float> taperDelegate(cParameterMinValue, cParameterMaxValue);
    AAX_CNumberDisplayDelegate<float, 100> displayDelegate;
    AAX_CParameter<float> param("1", AAX_CString("Linearparameter"), 10, taperDelegate, displayDelegate);

    double normalizedValue;
    EXPECT_TRUE(param.GetNormalizedValueFromString(AAX_CString("-50"), &normalizedValue));
    EXPECT_EQ(0.0, normalizedValue);

    EXPECT_TRUE(param.GetNormalizedValueFromString(AAX_CString("30"), &normalizedValue));
    EXPECT_EQ(1.0, normalizedValue);

    EXPECT_TRUE(param.GetNormalizedValueFromString(AAX_CString("-10"), &normalizedValue));
    EXPECT_EQ(0.5, normalizedValue);

    // Test invalid string
    EXPECT_FALSE(param.GetNormalizedValueFromString(AAX_CString("InvalidString"), &normalizedValue));
}
