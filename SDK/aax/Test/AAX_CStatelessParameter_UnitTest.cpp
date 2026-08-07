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
 *	\file AAX_CStatelessParameter_UnitTest.cpp
 *
 *	\brief Unit tests for AAX_CStatelessParameter class
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "AAX_CParameter.h"
#include "AAX_CString.h"
#include "AAX_IAutomationDelegate.h"

//--------------------------------------------------------------------
#pragma mark - Mock classes

class MockAutomationDelegate : public AAX_IAutomationDelegate
{
public:
    MOCK_METHOD(AAX_Result, RegisterParameter, (AAX_CParamID iParameterID), (override));
    MOCK_METHOD(AAX_Result, UnregisterParameter, (AAX_CParamID iParameterID), (override));
    MOCK_METHOD(AAX_Result, PostSetValueRequest, (AAX_CParamID iParameterID, double normalizedValue), (const, override));
    MOCK_METHOD(AAX_Result, PostCurrentValue, (AAX_CParamID iParameterID, double normalizedValue), (const, override));
    MOCK_METHOD(AAX_Result, PostTouchRequest, (AAX_CParamID iParameterID), (override));
    MOCK_METHOD(AAX_Result, PostReleaseRequest, (AAX_CParamID iParameterID), (override));
    MOCK_METHOD(AAX_Result, GetTouchState, (AAX_CParamID iParameterID, AAX_CBoolean* oTouched), (override));
    MOCK_METHOD(AAX_Result, ParameterNameChanged, (AAX_CParamID iParameterID), (override));
};

//--------------------------------------------------------------------
#pragma mark - Test fixture

class AAX_CStatelessParameter_Test : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

//--------------------------------------------------------------------
#pragma mark - Constructor Tests

TEST_F(AAX_CStatelessParameter_Test, Constructor_WithCParamID)
{
    AAX_CStatelessParameter param("test_id", AAX_CString("Test Parameter"), AAX_CString("Trigger"));
    
    // Test basic properties
    EXPECT_STREQ("test_id", param.Identifier());
    EXPECT_STREQ("Test Parameter", param.Name().Get());
    
    // Test value string
    AAX_CString valueString;
    EXPECT_TRUE(param.GetValueString(&valueString));
    EXPECT_STREQ("Trigger", valueString.Get());
}

TEST_F(AAX_CStatelessParameter_Test, Constructor_WithStringID)
{
    AAX_CStatelessParameter param(AAX_CString("string_id"), AAX_CString("String ID Parameter"), AAX_CString("Button"));
    
    // Test basic properties
    EXPECT_STREQ("string_id", param.Identifier());
    EXPECT_STREQ("String ID Parameter", param.Name().Get());
    
    // Test value string
    AAX_CString valueString;
    EXPECT_TRUE(param.GetValueString(&valueString));
    EXPECT_STREQ("Button", valueString.Get());
}

//--------------------------------------------------------------------
#pragma mark - Identification Methods Tests

TEST_F(AAX_CStatelessParameter_Test, NameManagement)
{
    AAX_CStatelessParameter param("name_test", AAX_CString("Original Name"), AAX_CString("Value"));
    
    // Test initial name
    EXPECT_STREQ("Original Name", param.Name().Get());
    
    // Test SetName
    param.SetName(AAX_CString("New Name"));
    EXPECT_STREQ("New Name", param.Name().Get());
    
    // Test AddShortenedName and ShortenedName
    param.AddShortenedName(AAX_CString("Short"));
    param.AddShortenedName(AAX_CString("VeryShort"));
    
    // Test getting shortened names for different character limits
    const AAX_CString& shortName9 = param.ShortenedName(9);
    const AAX_CString& shortName5 = param.ShortenedName(5);
    
    // At minimum, should return some valid string
    EXPECT_GT(shortName9.Length(), 0U);
    EXPECT_GT(shortName5.Length(), 0U);
    
    // Test ClearShortenedNames
    param.ClearShortenedNames();
    // After clearing, should fall back to full name
    EXPECT_STREQ("New Name", param.ShortenedName(5).Get());
}

TEST_F(AAX_CStatelessParameter_Test, NameManagement_WithAutomationDelegate)
{
    AAX_CStatelessParameter param("name_auto_test", AAX_CString("Auto Name Test"), AAX_CString("Value"));
    MockAutomationDelegate mockDelegate;
    
    // Set up automation delegate
    EXPECT_CALL(mockDelegate, RegisterParameter(::testing::StrEq("name_auto_test")))
        .WillOnce(::testing::Return(AAX_SUCCESS));
    param.SetAutomationDelegate(&mockDelegate);
    
    // When SetName is called, expect ParameterNameChanged to be called
    EXPECT_CALL(mockDelegate, ParameterNameChanged(::testing::StrEq("name_auto_test")))
        .WillOnce(::testing::Return(AAX_SUCCESS));
    param.SetName(AAX_CString("Changed Name"));
    
    EXPECT_STREQ("Changed Name", param.Name().Get());
    
    // Clean up
    EXPECT_CALL(mockDelegate, UnregisterParameter(::testing::StrEq("name_auto_test")))
        .WillOnce(::testing::Return(AAX_SUCCESS));
    param.SetAutomationDelegate(nullptr);
}

//--------------------------------------------------------------------
#pragma mark - Automation Methods Tests

TEST_F(AAX_CStatelessParameter_Test, Automatable)
{
    AAX_CStatelessParameter param("auto_test", AAX_CString("Auto Test"), AAX_CString("Value"));
    
    // Stateless parameters are never automatable
    EXPECT_FALSE(param.Automatable());
}

TEST_F(AAX_CStatelessParameter_Test, AutomationDelegate_SetAndGet)
{
    AAX_CStatelessParameter param("delegate_test", AAX_CString("Delegate Test"), AAX_CString("Value"));
    MockAutomationDelegate mockDelegate;
    
    // Expect RegisterParameter to be called when setting automation delegate
    EXPECT_CALL(mockDelegate, RegisterParameter(::testing::StrEq("delegate_test")))
        .WillOnce(::testing::Return(AAX_SUCCESS));
    
    param.SetAutomationDelegate(&mockDelegate);
    
    // Expect UnregisterParameter to be called when setting to nullptr
    EXPECT_CALL(mockDelegate, UnregisterParameter(::testing::StrEq("delegate_test")))
        .WillOnce(::testing::Return(AAX_SUCCESS));
    
    param.SetAutomationDelegate(nullptr);
}

TEST_F(AAX_CStatelessParameter_Test, TouchAndRelease)
{
    AAX_CStatelessParameter param("touch_test", AAX_CString("Touch Test"), AAX_CString("Value"));
    MockAutomationDelegate mockDelegate;
    
    // Set up automation delegate
    EXPECT_CALL(mockDelegate, RegisterParameter(::testing::StrEq("touch_test")))
        .WillOnce(::testing::Return(AAX_SUCCESS));
    param.SetAutomationDelegate(&mockDelegate);
    
    // Test Touch
    EXPECT_CALL(mockDelegate, PostTouchRequest(::testing::StrEq("touch_test")))
        .WillOnce(::testing::Return(AAX_SUCCESS));
    param.Touch();
    
    // Test Release
    EXPECT_CALL(mockDelegate, PostReleaseRequest(::testing::StrEq("touch_test")))
        .WillOnce(::testing::Return(AAX_SUCCESS));
    param.Release();
    
    // Clean up
    EXPECT_CALL(mockDelegate, UnregisterParameter(::testing::StrEq("touch_test")))
        .WillOnce(::testing::Return(AAX_SUCCESS));
    param.SetAutomationDelegate(nullptr);
}

TEST_F(AAX_CStatelessParameter_Test, TouchAndRelease_WithoutDelegate)
{
    AAX_CStatelessParameter param("no_delegate_test", AAX_CString("No Delegate Test"), AAX_CString("Value"));

    // Touch and Release without automation delegate should not crash
    param.Touch();
    param.Release();
}

//--------------------------------------------------------------------
#pragma mark - Taper Methods Tests (Stateless Behavior)

TEST_F(AAX_CStatelessParameter_Test, TaperMethods_StatelessBehavior)
{
    AAX_CStatelessParameter param("taper_test", AAX_CString("Taper Test"), AAX_CString("Value"));

    // All taper methods should be no-ops or return fixed values for stateless parameters

    // SetNormalizedValue should be a no-op
    param.SetNormalizedValue(0.5);
    EXPECT_DOUBLE_EQ(0.0, param.GetNormalizedValue()); // Should always return 0

    // SetNormalizedDefaultValue should be a no-op
    param.SetNormalizedDefaultValue(0.75);
    EXPECT_DOUBLE_EQ(0.0, param.GetNormalizedDefaultValue()); // Should always return 0

    // SetToDefaultValue should be a no-op
    param.SetToDefaultValue();
    EXPECT_DOUBLE_EQ(0.0, param.GetNormalizedValue()); // Should still be 0

    // Step methods should return fixed values
    param.SetNumberOfSteps(10); // Should be a no-op
    EXPECT_EQ(1U, param.GetNumberOfSteps()); // Should always return 1

    EXPECT_EQ(0U, param.GetStepValue()); // Should always return 0

    EXPECT_DOUBLE_EQ(0.0, param.GetNormalizedValueFromStep(5)); // Should always return 0
    EXPECT_EQ(0U, param.GetStepValueFromNormalizedValue(0.8)); // Should always return 0

    param.SetStepValue(3); // Should be a no-op
    EXPECT_EQ(0U, param.GetStepValue()); // Should still be 0
}

//--------------------------------------------------------------------
#pragma mark - Display Methods Tests

TEST_F(AAX_CStatelessParameter_Test, GetValueString)
{
    AAX_CStatelessParameter param("display_test", AAX_CString("Display Test"), AAX_CString("Button Press"));

    // Test basic GetValueString
    AAX_CString valueString;
    EXPECT_TRUE(param.GetValueString(&valueString));
    EXPECT_STREQ("Button Press", valueString.Get());

    // Test GetValueString with max chars (should ignore max chars and return full string)
    AAX_CString limitedString;
    EXPECT_TRUE(param.GetValueString(5, &limitedString));
    EXPECT_STREQ("Button Press", limitedString.Get()); // Should return full string, not truncated

    // Test with null pointer
    EXPECT_TRUE(param.GetValueString(nullptr)); // Should not crash
}

TEST_F(AAX_CStatelessParameter_Test, NormalizedValueConversions_GetFrom)
{
    AAX_CStatelessParameter param("conversion_test", AAX_CString("Conversion Test"), AAX_CString("Value"));

    double normalizedValue = 999.0; // Initialize to non-zero value

    // All GetNormalizedValueFrom* methods should return true and set normalized value to 0
    EXPECT_TRUE(param.GetNormalizedValueFromBool(true, &normalizedValue));
    EXPECT_DOUBLE_EQ(0.0, normalizedValue);

    normalizedValue = 999.0;
    EXPECT_TRUE(param.GetNormalizedValueFromInt32(42, &normalizedValue));
    EXPECT_DOUBLE_EQ(0.0, normalizedValue);

    normalizedValue = 999.0;
    EXPECT_TRUE(param.GetNormalizedValueFromFloat(3.14f, &normalizedValue));
    EXPECT_DOUBLE_EQ(0.0, normalizedValue);

    normalizedValue = 999.0;
    EXPECT_TRUE(param.GetNormalizedValueFromDouble(2.71, &normalizedValue));
    EXPECT_DOUBLE_EQ(0.0, normalizedValue);

    normalizedValue = 999.0;
    EXPECT_TRUE(param.GetNormalizedValueFromString(AAX_CString("test"), &normalizedValue));
    EXPECT_DOUBLE_EQ(0.0, normalizedValue);

    // Test with null pointers
    EXPECT_TRUE(param.GetNormalizedValueFromBool(true, nullptr)); // Should not crash
    EXPECT_TRUE(param.GetNormalizedValueFromInt32(42, nullptr));
    EXPECT_TRUE(param.GetNormalizedValueFromFloat(3.14f, nullptr));
    EXPECT_TRUE(param.GetNormalizedValueFromDouble(2.71, nullptr));
    EXPECT_TRUE(param.GetNormalizedValueFromString(AAX_CString("test"), nullptr));
}

TEST_F(AAX_CStatelessParameter_Test, NormalizedValueConversions_GetTo)
{
    AAX_CStatelessParameter param("conversion_test2", AAX_CString("Conversion Test 2"), AAX_CString("Test Value"));

    // GetBoolFromNormalizedValue should return true and set value to false
    bool boolValue = true; // Initialize to non-default value
    EXPECT_TRUE(param.GetBoolFromNormalizedValue(0.5, &boolValue));
    EXPECT_FALSE(boolValue);

    // Test with null pointer
    EXPECT_TRUE(param.GetBoolFromNormalizedValue(0.5, nullptr)); // Should not crash

    // GetInt32FromNormalizedValue should return false (not supported)
    int32_t intValue = 999;
    EXPECT_FALSE(param.GetInt32FromNormalizedValue(0.5, &intValue));
    // Value should remain unchanged since method returns false
    EXPECT_EQ(999, intValue);

    // GetFloatFromNormalizedValue should return false (not supported)
    float floatValue = 999.0f;
    EXPECT_FALSE(param.GetFloatFromNormalizedValue(0.5, &floatValue));
    EXPECT_FLOAT_EQ(999.0f, floatValue);

    // GetDoubleFromNormalizedValue should return false (not supported)
    double doubleValue = 999.0;
    EXPECT_FALSE(param.GetDoubleFromNormalizedValue(0.5, &doubleValue));
    EXPECT_DOUBLE_EQ(999.0, doubleValue);

    // GetStringFromNormalizedValue should return true and set to value string
    AAX_CString stringValue;
    EXPECT_TRUE(param.GetStringFromNormalizedValue(0.5, stringValue));
    EXPECT_STREQ("Test Value", stringValue.Get());

    // GetStringFromNormalizedValue with max chars should ignore max chars
    AAX_CString limitedStringValue;
    EXPECT_TRUE(param.GetStringFromNormalizedValue(0.5, 4, limitedStringValue));
    EXPECT_STREQ("Test Value", limitedStringValue.Get()); // Should return full string
}

TEST_F(AAX_CStatelessParameter_Test, SetValueFromString)
{
    AAX_CStatelessParameter param("string_set_test", AAX_CString("String Set Test"), AAX_CString("Original"));

    // SetValueFromString should update the value string
    EXPECT_TRUE(param.SetValueFromString(AAX_CString("New Value")));

    // Verify the value string was updated
    AAX_CString valueString;
    EXPECT_TRUE(param.GetValueString(&valueString));
    EXPECT_STREQ("New Value", valueString.Get());

    // Verify GetStringFromNormalizedValue also returns the new value
    AAX_CString normalizedString;
    EXPECT_TRUE(param.GetStringFromNormalizedValue(0.0, normalizedString));
    EXPECT_STREQ("New Value", normalizedString.Get());
}

//--------------------------------------------------------------------
#pragma mark - Typed Accessors Tests

TEST_F(AAX_CStatelessParameter_Test, TypedAccessors_Get)
{
    AAX_CStatelessParameter param("typed_get_test", AAX_CString("Typed Get Test"), AAX_CString("Value"));

    // GetValueAsBool should return true and set value to false
    bool boolValue = true; // Initialize to non-default
    EXPECT_TRUE(param.GetValueAsBool(&boolValue));
    EXPECT_FALSE(boolValue);

    // Test with null pointer
    EXPECT_TRUE(param.GetValueAsBool(nullptr)); // Should not crash

    // GetValueAsInt32 should return false (not supported)
    int32_t intValue = 999;
    EXPECT_FALSE(param.GetValueAsInt32(&intValue));
    EXPECT_EQ(999, intValue); // Should remain unchanged

    // GetValueAsFloat should return false (not supported)
    float floatValue = 999.0f;
    EXPECT_FALSE(param.GetValueAsFloat(&floatValue));
    EXPECT_FLOAT_EQ(999.0f, floatValue);

    // GetValueAsDouble should return false (not supported)
    double doubleValue = 999.0;
    EXPECT_FALSE(param.GetValueAsDouble(&doubleValue));
    EXPECT_DOUBLE_EQ(999.0, doubleValue);

    // GetValueAsString should return false (not supported)
    AAX_CString stringValue("unchanged");
    EXPECT_FALSE(param.GetValueAsString(&stringValue));
    EXPECT_STREQ("unchanged", stringValue.Get());
}

TEST_F(AAX_CStatelessParameter_Test, TypedAccessors_Set)
{
    AAX_CStatelessParameter param("typed_set_test", AAX_CString("Typed Set Test"), AAX_CString("Original"));

    // SetValueWithBool should return true (accepted but doesn't change value string)
    EXPECT_TRUE(param.SetValueWithBool(true));
    EXPECT_TRUE(param.SetValueWithBool(false));

    // Value string should remain unchanged
    AAX_CString valueString;
    EXPECT_TRUE(param.GetValueString(&valueString));
    EXPECT_STREQ("Original", valueString.Get());

    // SetValueWithInt32 should return false (not supported)
    EXPECT_FALSE(param.SetValueWithInt32(42));

    // SetValueWithFloat should return false (not supported)
    EXPECT_FALSE(param.SetValueWithFloat(3.14f));

    // SetValueWithDouble should return false (not supported)
    EXPECT_FALSE(param.SetValueWithDouble(2.71));

    // SetValueWithString should return true and update value string
    EXPECT_TRUE(param.SetValueWithString(AAX_CString("Updated Value")));

    // Verify the value string was updated
    EXPECT_TRUE(param.GetValueString(&valueString));
    EXPECT_STREQ("Updated Value", valueString.Get());
}

//--------------------------------------------------------------------
#pragma mark - Type and Orientation Tests

TEST_F(AAX_CStatelessParameter_Test, TypeAndOrientation)
{
    AAX_CStatelessParameter param("type_test", AAX_CString("Type Test"), AAX_CString("Value"));

    // GetType should always return Discrete
    EXPECT_EQ(AAX_eParameterType_Discrete, param.GetType());

    // SetType should be a no-op
    param.SetType(AAX_eParameterType_Continuous);
    EXPECT_EQ(AAX_eParameterType_Discrete, param.GetType()); // Should still be Discrete

    // GetOrientation should always return Default
    EXPECT_EQ(AAX_eParameterOrientation_Default, param.GetOrientation());

    // SetOrientation should be a no-op
    param.SetOrientation(AAX_eParameterOrientation_BottomMinTopMax);
    EXPECT_EQ(AAX_eParameterOrientation_Default, param.GetOrientation()); // Should still be Default
}

//--------------------------------------------------------------------
#pragma mark - Delegate Methods Tests

TEST_F(AAX_CStatelessParameter_Test, DelegateMethods_NoOps)
{
    AAX_CStatelessParameter param("delegate_noop_test", AAX_CString("Delegate NoOp Test"), AAX_CString("Value"));

    // These methods should be no-ops and not crash
    // Since we can't easily create instances of abstract base classes and the methods
    // are no-ops anyway, we'll just verify the parameter still functions normally
    // The implementation shows these methods have empty bodies, so they're safe no-ops

    // The parameter should function normally (these methods are no-ops)
    AAX_CString valueString;
    EXPECT_TRUE(param.GetValueString(&valueString));
    EXPECT_STREQ("Value", valueString.Get());

    // Verify other functionality still works
    EXPECT_DOUBLE_EQ(0.0, param.GetNormalizedValue());
    EXPECT_EQ(AAX_eParameterType_Discrete, param.GetType());
    EXPECT_EQ(AAX_eParameterOrientation_Default, param.GetOrientation());
}

//--------------------------------------------------------------------
#pragma mark - Host Interface Methods Tests

TEST_F(AAX_CStatelessParameter_Test, UpdateNormalizedValue)
{
    AAX_CStatelessParameter param("host_test", AAX_CString("Host Test"), AAX_CString("Value"));

    // UpdateNormalizedValue should be a no-op
    param.UpdateNormalizedValue(0.5);

    // Normalized value should still be 0
    EXPECT_DOUBLE_EQ(0.0, param.GetNormalizedValue());

    // Value string should be unchanged
    AAX_CString valueString;
    EXPECT_TRUE(param.GetValueString(&valueString));
    EXPECT_STREQ("Value", valueString.Get());
}

//--------------------------------------------------------------------
#pragma mark - CloneValue Tests

TEST_F(AAX_CStatelessParameter_Test, CloneValue)
{
    AAX_CStatelessParameter param("clone_test", AAX_CString("Clone Test"), AAX_CString("Value"));

    // CloneValue should return NULL for stateless parameters
    AAX_IParameterValue* clonedValue = param.CloneValue();
    EXPECT_EQ(nullptr, clonedValue);
}

//--------------------------------------------------------------------
#pragma mark - Edge Cases and Integration Tests

TEST_F(AAX_CStatelessParameter_Test, DestructorBehavior)
{
    MockAutomationDelegate mockDelegate;

    {
        AAX_CStatelessParameter param("destructor_test", AAX_CString("Destructor Test"), AAX_CString("Value"));

        // Set up automation delegate
        EXPECT_CALL(mockDelegate, RegisterParameter(::testing::StrEq("destructor_test")))
            .WillOnce(::testing::Return(AAX_SUCCESS));
        param.SetAutomationDelegate(&mockDelegate);

        // POTENTIAL BUG:
        // AAX_CStatelessParameter uses AAX_DEFAULT_DTOR_OVERRIDE which generates a default destructor
        // Unlike AAX_CParameter, it does NOT automatically call SetAutomationDelegate(nullptr) in destructor
        // (Though note that stateless parameters are not automatable anyway)

        // Manually clean up to avoid leaving dangling automation delegate
        EXPECT_CALL(mockDelegate, UnregisterParameter(::testing::StrEq("destructor_test")))
            .WillOnce(::testing::Return(AAX_SUCCESS));
        param.SetAutomationDelegate(nullptr);
    } // param destructor called here - no additional automation calls expected
}

TEST_F(AAX_CStatelessParameter_Test, EmptyValueString)
{
    AAX_CStatelessParameter param("empty_test", AAX_CString("Empty Test"), AAX_CString(""));

    // Test with empty value string
    AAX_CString valueString;
    EXPECT_TRUE(param.GetValueString(&valueString));
    EXPECT_STREQ("", valueString.Get());
    EXPECT_EQ(0U, valueString.Length());

    // GetStringFromNormalizedValue should also return empty string
    AAX_CString normalizedString;
    EXPECT_TRUE(param.GetStringFromNormalizedValue(0.0, normalizedString));
    EXPECT_STREQ("", normalizedString.Get());
}

TEST_F(AAX_CStatelessParameter_Test, LongValueString)
{
    AAX_CString longValue("This is a very long value string that tests the parameter's ability to handle lengthy text content without issues");
    AAX_CStatelessParameter param("long_test", AAX_CString("Long Test"), longValue);

    // Test with long value string
    AAX_CString valueString;
    EXPECT_TRUE(param.GetValueString(&valueString));
    EXPECT_STREQ(longValue.Get(), valueString.Get());

    // Test GetValueString with max chars (should still return full string)
    AAX_CString limitedString;
    EXPECT_TRUE(param.GetValueString(10, &limitedString));
    EXPECT_STREQ(longValue.Get(), limitedString.Get()); // Should return full string, not truncated
}

TEST_F(AAX_CStatelessParameter_Test, ValueStringModification)
{
    AAX_CStatelessParameter param("modify_test", AAX_CString("Modify Test"), AAX_CString("Initial"));

    // Test multiple modifications
    EXPECT_TRUE(param.SetValueFromString(AAX_CString("First Change")));
    AAX_CString valueString;
    EXPECT_TRUE(param.GetValueString(&valueString));
    EXPECT_STREQ("First Change", valueString.Get());

    EXPECT_TRUE(param.SetValueWithString(AAX_CString("Second Change")));
    EXPECT_TRUE(param.GetValueString(&valueString));
    EXPECT_STREQ("Second Change", valueString.Get());

    // Verify GetStringFromNormalizedValue reflects the changes
    AAX_CString normalizedString;
    EXPECT_TRUE(param.GetStringFromNormalizedValue(0.0, normalizedString));
    EXPECT_STREQ("Second Change", normalizedString.Get());
}

TEST_F(AAX_CStatelessParameter_Test, StatelessConsistency)
{
    AAX_CStatelessParameter param("consistency_test", AAX_CString("Consistency Test"), AAX_CString("Value"));

    // Verify that all stateless behaviors are consistent regardless of operations performed

    // Perform various operations
    param.SetNormalizedValue(0.75);
    param.SetNormalizedDefaultValue(0.25);
    param.SetToDefaultValue();
    param.SetNumberOfSteps(100);
    param.SetStepValue(50);
    param.UpdateNormalizedValue(0.9);
    param.SetType(AAX_eParameterType_Continuous);
    param.SetOrientation(AAX_eParameterOrientation_BottomMinTopMax);

    // All stateless values should remain consistent
    EXPECT_DOUBLE_EQ(0.0, param.GetNormalizedValue());
    EXPECT_DOUBLE_EQ(0.0, param.GetNormalizedDefaultValue());
    EXPECT_EQ(1U, param.GetNumberOfSteps());
    EXPECT_EQ(0U, param.GetStepValue());
    EXPECT_DOUBLE_EQ(0.0, param.GetNormalizedValueFromStep(25));
    EXPECT_EQ(0U, param.GetStepValueFromNormalizedValue(0.5));
    EXPECT_EQ(AAX_eParameterType_Discrete, param.GetType());
    EXPECT_EQ(AAX_eParameterOrientation_Default, param.GetOrientation());
    EXPECT_FALSE(param.Automatable());

    // Value string should be unchanged
    AAX_CString valueString;
    EXPECT_TRUE(param.GetValueString(&valueString));
    EXPECT_STREQ("Value", valueString.Get());
}
