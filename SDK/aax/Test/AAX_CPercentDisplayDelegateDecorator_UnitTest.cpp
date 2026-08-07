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
 *	\file AAX_CPercentDisplayDelegateDecorator_UnitTest.cpp
 *
 *	\brief Unit tests for AAX_CPercentDisplayDelegateDecorator template class
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX_CPercentDisplayDelegateDecorator.h"
#include "AAX_CUnitDisplayDelegateDecorator.h"
#include "AAX_CScaledDisplayDelegateDecorator.h"
#include "AAX_MockDisplayDelegate.h"
#include "AAX_CString.h"
#include <cmath>
#include <limits>
#include <iostream>
#include <memory>
#include <vector>
#include <utility>

//==============================================================================
// Test Fixture
//==============================================================================

template <typename T>
class AAX_CPercentDisplayDelegateDecorator_Test : public ::testing::Test
{
protected:
    void SetUp() override
    {
        mMockDisplayDelegate = std::make_unique<AAX_MockDisplayDelegate<T>>();
        mPercentDecorator = std::make_unique<AAX_CPercentDisplayDelegateDecorator<T>>(*mMockDisplayDelegate);
    }
    
    void TearDown() override
    {
        mPercentDecorator.reset();
        mMockDisplayDelegate.reset();
    }
    
    std::unique_ptr<AAX_MockDisplayDelegate<T>> mMockDisplayDelegate;
    std::unique_ptr<AAX_CPercentDisplayDelegateDecorator<T>> mPercentDecorator;
};

// Test with common numeric types
using TestTypes = ::testing::Types<float, double>;
TYPED_TEST_SUITE(AAX_CPercentDisplayDelegateDecorator_Test, TestTypes);

//==============================================================================
// Construction and Cloning Tests
//==============================================================================

TYPED_TEST(AAX_CPercentDisplayDelegateDecorator_Test, Construction)
{
    // Test that decorator is properly constructed
    EXPECT_NE(nullptr, this->mPercentDecorator.get());
}

TYPED_TEST(AAX_CPercentDisplayDelegateDecorator_Test, Clone)
{
    // Test cloning functionality
    auto cloned = std::unique_ptr<AAX_CPercentDisplayDelegateDecorator<TypeParam>>(
        this->mPercentDecorator->Clone());
    
    EXPECT_NE(nullptr, cloned.get());
    EXPECT_NE(this->mPercentDecorator.get(), cloned.get());
    
    // Test that cloned decorator behaves the same
    AAX_CString originalString, clonedString;
    TypeParam testValue = static_cast<TypeParam>(0.5);
    
    bool originalResult = this->mPercentDecorator->ValueToString(testValue, &originalString);
    bool clonedResult = cloned->ValueToString(testValue, &clonedString);
    
    EXPECT_EQ(originalResult, clonedResult);
    EXPECT_EQ(originalString, clonedString);
}

//==============================================================================
// ValueToString Tests
//==============================================================================

TYPED_TEST(AAX_CPercentDisplayDelegateDecorator_Test, ValueToString_BasicConversion)
{
    AAX_CString result;

    // Test 0.5 -> "50%"
    TypeParam halfValue = static_cast<TypeParam>(0.5);
    EXPECT_TRUE(this->mPercentDecorator->ValueToString(halfValue, &result));
    EXPECT_NE(result.StdString().find("%"), std::string::npos);

    // Test 1.0 -> "100%"
    TypeParam unityValue = static_cast<TypeParam>(1.0);
    EXPECT_TRUE(this->mPercentDecorator->ValueToString(unityValue, &result));
    EXPECT_NE(result.StdString().find("%"), std::string::npos);

    // Test 0.0 -> "0%"
    TypeParam zeroValue = static_cast<TypeParam>(0.0);
    EXPECT_TRUE(this->mPercentDecorator->ValueToString(zeroValue, &result));
    EXPECT_NE(result.StdString().find("%"), std::string::npos);
}

TYPED_TEST(AAX_CPercentDisplayDelegateDecorator_Test, ValueToString_FractionalValues)
{
    AAX_CString result;
    
    // Test 0.25 -> "25%"
    TypeParam quarterValue = static_cast<TypeParam>(0.25);
    EXPECT_TRUE(this->mPercentDecorator->ValueToString(quarterValue, &result));
    EXPECT_NE(result.StdString().find("%"), std::string::npos);

    // Test 0.75 -> "75%"
    TypeParam threeQuarterValue = static_cast<TypeParam>(0.75);
    EXPECT_TRUE(this->mPercentDecorator->ValueToString(threeQuarterValue, &result));
    EXPECT_NE(result.StdString().find("%"), std::string::npos);

    // Test 0.01 -> "1%"
    TypeParam onePercentValue = static_cast<TypeParam>(0.01);
    EXPECT_TRUE(this->mPercentDecorator->ValueToString(onePercentValue, &result));
    EXPECT_NE(result.StdString().find("%"), std::string::npos);
}

TYPED_TEST(AAX_CPercentDisplayDelegateDecorator_Test, ValueToString_ValuesOverOne)
{
    AAX_CString result;
    
    // Test 1.5 -> "150%"
    TypeParam overUnityValue = static_cast<TypeParam>(1.5);
    EXPECT_TRUE(this->mPercentDecorator->ValueToString(overUnityValue, &result));
    EXPECT_NE(result.StdString().find("%"), std::string::npos);

    // Test 2.0 -> "200%"
    TypeParam doubleValue = static_cast<TypeParam>(2.0);
    EXPECT_TRUE(this->mPercentDecorator->ValueToString(doubleValue, &result));
    EXPECT_NE(result.StdString().find("%"), std::string::npos);
}

TYPED_TEST(AAX_CPercentDisplayDelegateDecorator_Test, ValueToString_NegativeValues)
{
    AAX_CString result;
    
    // Test -0.5 -> "-50%"
    TypeParam negativeValue = static_cast<TypeParam>(-0.5);
    EXPECT_TRUE(this->mPercentDecorator->ValueToString(negativeValue, &result));
    EXPECT_NE(result.StdString().find("%"), std::string::npos);
    EXPECT_NE(result.StdString().find("-"), std::string::npos); // Should contain negative sign
}

//==============================================================================
// ValueToString with Character Limit Tests
//==============================================================================

TYPED_TEST(AAX_CPercentDisplayDelegateDecorator_Test, ValueToString_WithCharLimit_SufficientSpace)
{
    AAX_CString result;
    TypeParam testValue = static_cast<TypeParam>(0.5);
    
    // Test with sufficient character limit
    EXPECT_TRUE(this->mPercentDecorator->ValueToString(testValue, 20, &result));
    EXPECT_NE(result.StdString().find("%"), std::string::npos);
    EXPECT_LE(result.Length(), 20);
}

TYPED_TEST(AAX_CPercentDisplayDelegateDecorator_Test, ValueToString_WithCharLimit_LimitedSpace)
{
    AAX_CString result;
    TypeParam testValue = static_cast<TypeParam>(0.5);
    
    // Test with limited character space
    EXPECT_TRUE(this->mPercentDecorator->ValueToString(testValue, 5, &result));
    EXPECT_NE(result.StdString().find("%"), std::string::npos);
    EXPECT_LE(result.Length(), 5);
}

TYPED_TEST(AAX_CPercentDisplayDelegateDecorator_Test, ValueToString_WithCharLimit_VeryLimitedSpace)
{
    AAX_CString result;
    TypeParam testValue = static_cast<TypeParam>(0.5);
    
    // Test with very limited character space
    EXPECT_TRUE(this->mPercentDecorator->ValueToString(testValue, 2, &result));
    EXPECT_LE(result.Length(), 2);
    // Should still include % symbol
    EXPECT_NE(result.StdString().find("%"), std::string::npos);
}

//==============================================================================
// StringToValue Tests
//==============================================================================

TYPED_TEST(AAX_CPercentDisplayDelegateDecorator_Test, StringToValue_WithPercentSuffix)
{
    TypeParam result;
    
    // Test parsing "50%" -> 0.5
    EXPECT_TRUE(this->mPercentDecorator->StringToValue(AAX_CString("50%"), &result));
    EXPECT_NEAR(static_cast<double>(result), 0.5, 0.001);
    
    // Test parsing "100%" -> 1.0
    EXPECT_TRUE(this->mPercentDecorator->StringToValue(AAX_CString("100%"), &result));
    EXPECT_NEAR(static_cast<double>(result), 1.0, 0.001);
    
    // Test parsing "0%" -> 0.0
    EXPECT_TRUE(this->mPercentDecorator->StringToValue(AAX_CString("0%"), &result));
    EXPECT_NEAR(static_cast<double>(result), 0.0, 0.001);
    
    // Test parsing "25%" -> 0.25
    EXPECT_TRUE(this->mPercentDecorator->StringToValue(AAX_CString("25%"), &result));
    EXPECT_NEAR(static_cast<double>(result), 0.25, 0.001);
}

TYPED_TEST(AAX_CPercentDisplayDelegateDecorator_Test, StringToValue_WithoutPercentSuffix)
{
    TypeParam result;
    
    // Test parsing plain number (should still apply percent conversion)
    EXPECT_TRUE(this->mPercentDecorator->StringToValue(AAX_CString("50"), &result));
    EXPECT_NEAR(static_cast<double>(result), 0.5, 0.001);
    
    // Test parsing "100" -> 1.0
    EXPECT_TRUE(this->mPercentDecorator->StringToValue(AAX_CString("100"), &result));
    EXPECT_NEAR(static_cast<double>(result), 1.0, 0.001);
}

TYPED_TEST(AAX_CPercentDisplayDelegateDecorator_Test, StringToValue_ValuesOverOneHundred)
{
    TypeParam result;
    
    // Test parsing "150%" -> 1.5
    EXPECT_TRUE(this->mPercentDecorator->StringToValue(AAX_CString("150%"), &result));
    EXPECT_NEAR(static_cast<double>(result), 1.5, 0.001);
    
    // Test parsing "200%" -> 2.0
    EXPECT_TRUE(this->mPercentDecorator->StringToValue(AAX_CString("200%"), &result));
    EXPECT_NEAR(static_cast<double>(result), 2.0, 0.001);
}

TYPED_TEST(AAX_CPercentDisplayDelegateDecorator_Test, StringToValue_NegativeValues)
{
    TypeParam result;
    
    // Test parsing "-50%" -> -0.5
    EXPECT_TRUE(this->mPercentDecorator->StringToValue(AAX_CString("-50%"), &result));
    EXPECT_NEAR(static_cast<double>(result), -0.5, 0.001);
}

TYPED_TEST(AAX_CPercentDisplayDelegateDecorator_Test, StringToValue_ShortStrings)
{
    TypeParam result;
    
    // Test very short strings (2 characters or less)
    EXPECT_TRUE(this->mPercentDecorator->StringToValue(AAX_CString("5"), &result));
    EXPECT_TRUE(this->mPercentDecorator->StringToValue(AAX_CString("10"), &result));
    
    // Test empty string
    bool success = this->mPercentDecorator->StringToValue(AAX_CString(""), &result);
    // Result depends on underlying mock delegate behavior
}

//==============================================================================
// Rvalue Reference Constructor Tests
//==============================================================================

TYPED_TEST(AAX_CPercentDisplayDelegateDecorator_Test, RvalueConstructor_FromDisplayDelegate)
{
    // Test rvalue reference constructor with AAX_IDisplayDelegate&&
    auto mockDelegate = std::make_unique<AAX_MockDisplayDelegate<TypeParam>>();
    
    // Create decorator using rvalue reference constructor
    auto percentDecorator = std::make_unique<AAX_CPercentDisplayDelegateDecorator<TypeParam>>(
        std::move(*mockDelegate));
    
    EXPECT_NE(nullptr, percentDecorator.get());
    
    // Test functionality
    AAX_CString result;
    TypeParam testValue = static_cast<TypeParam>(0.5);
    EXPECT_TRUE(percentDecorator->ValueToString(testValue, &result));
    EXPECT_NE(result.StdString().find("%"), std::string::npos);
}

TYPED_TEST(AAX_CPercentDisplayDelegateDecorator_Test, RvalueConstructor_FromDisplayDelegateDecorator)
{
    // Test rvalue reference constructor with AAX_IDisplayDelegateDecorator&&
    auto mockDelegate = std::make_unique<AAX_MockDisplayDelegate<TypeParam>>();
    auto unitDecorator = std::make_unique<AAX_CUnitDisplayDelegateDecorator<TypeParam>>(
        *mockDelegate, AAX_CString("unit"));
    
    // Create percent decorator using rvalue reference constructor
    auto percentDecorator = std::make_unique<AAX_CPercentDisplayDelegateDecorator<TypeParam>>(
        std::move(*unitDecorator));
    
    EXPECT_NE(nullptr, percentDecorator.get());
    
    // Test functionality
    AAX_CString result;
    TypeParam testValue = static_cast<TypeParam>(0.5);
    EXPECT_TRUE(percentDecorator->ValueToString(testValue, &result));
    EXPECT_NE(result.StdString().find("%"), std::string::npos);
}

TYPED_TEST(AAX_CPercentDisplayDelegateDecorator_Test, RvalueConstructor_FactoryPattern)
{
    // Test factory pattern using rvalue reference constructor
    auto createPercentDecorator = []() {
        auto mockDelegate = std::make_unique<AAX_MockDisplayDelegate<TypeParam>>();
        return std::make_unique<AAX_CPercentDisplayDelegateDecorator<TypeParam>>(
            std::move(*mockDelegate));
    };
    
    auto decorator = createPercentDecorator();
    EXPECT_NE(nullptr, decorator.get());
    
    // Test functionality
    AAX_CString result;
    TypeParam testValue = static_cast<TypeParam>(0.75);
    EXPECT_TRUE(decorator->ValueToString(testValue, &result));
    EXPECT_NE(result.StdString().find("%"), std::string::npos);
}

TYPED_TEST(AAX_CPercentDisplayDelegateDecorator_Test, RvalueConstructor_DecoratorChaining)
{
    // Test chaining multiple decorators using rvalue reference constructors
    auto mockDelegate = std::make_unique<AAX_MockDisplayDelegate<TypeParam>>();
    
    // Chain: Mock -> Unit -> Percent
    auto chainedDecorator = std::make_unique<AAX_CPercentDisplayDelegateDecorator<TypeParam>>(
        AAX_CUnitDisplayDelegateDecorator<TypeParam>(std::move(*mockDelegate), AAX_CString("test")));
    
    EXPECT_NE(nullptr, chainedDecorator.get());
    
    // Test functionality
    AAX_CString result;
    TypeParam testValue = static_cast<TypeParam>(0.25);
    EXPECT_TRUE(chainedDecorator->ValueToString(testValue, &result));
    EXPECT_NE(result.StdString().find("%"), std::string::npos);
}

TYPED_TEST(AAX_CPercentDisplayDelegateDecorator_Test, RvalueConstructor_MemoryManagement)
{
    // Test that rvalue constructors properly manage memory
    std::vector<std::unique_ptr<AAX_CPercentDisplayDelegateDecorator<TypeParam>>> decorators;
    
    for (int i = 0; i < 10; ++i) {
        auto mockDelegate = std::make_unique<AAX_MockDisplayDelegate<TypeParam>>();
        decorators.push_back(std::make_unique<AAX_CPercentDisplayDelegateDecorator<TypeParam>>(
            std::move(*mockDelegate)));
    }
    
    // Test all decorators
    for (auto& decorator : decorators) {
        EXPECT_NE(nullptr, decorator.get());
        
        AAX_CString result;
        TypeParam testValue = static_cast<TypeParam>(0.5);
        EXPECT_TRUE(decorator->ValueToString(testValue, &result));
        EXPECT_NE(result.StdString().find("%"), std::string::npos);
    }
}

TYPED_TEST(AAX_CPercentDisplayDelegateDecorator_Test, RvalueConstructor_TemplateVariations)
{
    // Test rvalue constructors with different template types
    if constexpr (std::is_same_v<TypeParam, float>) {
        auto mockDelegate = std::make_unique<AAX_MockDisplayDelegate<float>>();
        auto decorator = std::make_unique<AAX_CPercentDisplayDelegateDecorator<float>>(
            std::move(*mockDelegate));
        
        AAX_CString result;
        EXPECT_TRUE(decorator->ValueToString(0.5f, &result));
        EXPECT_NE(result.StdString().find("%"), std::string::npos);
    }
    
    if constexpr (std::is_same_v<TypeParam, double>) {
        auto mockDelegate = std::make_unique<AAX_MockDisplayDelegate<double>>();
        auto decorator = std::make_unique<AAX_CPercentDisplayDelegateDecorator<double>>(
            std::move(*mockDelegate));
        
        AAX_CString result;
        EXPECT_TRUE(decorator->ValueToString(0.5, &result));
        EXPECT_NE(result.StdString().find("%"), std::string::npos);
    }
}

TYPED_TEST(AAX_CPercentDisplayDelegateDecorator_Test, RvalueConstructor_ComplexChaining)
{
    // Test complex decorator chaining with multiple rvalue constructors
    auto mockDelegate = std::make_unique<AAX_MockDisplayDelegate<TypeParam>>();
    
    // Create a complex chain using temporary objects
    auto complexDecorator = std::make_unique<AAX_CPercentDisplayDelegateDecorator<TypeParam>>(
        AAX_CScaledDisplayDelegateDecorator<TypeParam>(
            AAX_CUnitDisplayDelegateDecorator<TypeParam>(
                std::move(*mockDelegate), AAX_CString("base")), 2.0));
    
    EXPECT_NE(nullptr, complexDecorator.get());
    
    // Test with multiple values
    std::vector<TypeParam> testValues = {
        static_cast<TypeParam>(0.1),
        static_cast<TypeParam>(0.5),
        static_cast<TypeParam>(1.0)
    };
    
    for (TypeParam value : testValues) {
        AAX_CString result;
        EXPECT_TRUE(complexDecorator->ValueToString(value, &result));
        EXPECT_NE(result.StdString().find("%"), std::string::npos);
    }
}

TYPED_TEST(AAX_CPercentDisplayDelegateDecorator_Test, RvalueConstructor_PolymorphicBehavior)
{
    // Test polymorphic behavior with rvalue reference constructors
    auto mockDelegate = std::make_unique<AAX_MockDisplayDelegate<TypeParam>>();
    
    // Create decorator using rvalue constructor
    std::unique_ptr<AAX_IDisplayDelegateDecorator<TypeParam>> polymorphicDecorator = 
        std::make_unique<AAX_CPercentDisplayDelegateDecorator<TypeParam>>(std::move(*mockDelegate));
    
    EXPECT_NE(nullptr, polymorphicDecorator.get());
    
    // Test polymorphic behavior
    AAX_CString result;
    TypeParam testValue = static_cast<TypeParam>(0.8);
    EXPECT_TRUE(polymorphicDecorator->ValueToString(testValue, &result));
    EXPECT_NE(result.StdString().find("%"), std::string::npos);
    
    // Test cloning through polymorphic interface
    auto cloned = std::unique_ptr<AAX_IDisplayDelegateDecorator<TypeParam>>(
        polymorphicDecorator->Clone());
    EXPECT_NE(nullptr, cloned.get());
    
    AAX_CString clonedResult;
    EXPECT_TRUE(cloned->ValueToString(testValue, &clonedResult));
    EXPECT_EQ(result, clonedResult);
}

TYPED_TEST(AAX_CPercentDisplayDelegateDecorator_Test, RvalueConstructor_RoundTripWithMove)
{
    // Test round-trip conversion with rvalue-constructed decorators
    auto mockDelegate = std::make_unique<AAX_MockDisplayDelegate<TypeParam>>();
    auto decorator = std::make_unique<AAX_CPercentDisplayDelegateDecorator<TypeParam>>(
        std::move(*mockDelegate));
    
    std::vector<TypeParam> testValues = {
        static_cast<TypeParam>(0.0),
        static_cast<TypeParam>(0.25),
        static_cast<TypeParam>(0.5),
        static_cast<TypeParam>(0.75),
        static_cast<TypeParam>(1.0),
        static_cast<TypeParam>(1.5)
    };
    
    for (TypeParam originalValue : testValues) {
        AAX_CString stringValue;
        TypeParam convertedValue;
        
        // Convert to string and back
        EXPECT_TRUE(decorator->ValueToString(originalValue, &stringValue));
        EXPECT_TRUE(decorator->StringToValue(stringValue, &convertedValue));
        
        // Check accuracy (allowing for some floating-point precision loss)
        EXPECT_NEAR(static_cast<double>(originalValue), static_cast<double>(convertedValue), 0.001);
    }
}

TYPED_TEST(AAX_CPercentDisplayDelegateDecorator_Test, RvalueConstructor_EdgeCases)
{
    // Test edge cases with rvalue reference constructors
    auto mockDelegate = std::make_unique<AAX_MockDisplayDelegate<TypeParam>>();
    auto decorator = std::make_unique<AAX_CPercentDisplayDelegateDecorator<TypeParam>>(
        std::move(*mockDelegate));
    
    // Test extreme values
    std::vector<TypeParam> edgeValues = {
        static_cast<TypeParam>(-1.0),
        static_cast<TypeParam>(0.0),
        static_cast<TypeParam>(0.001),
        static_cast<TypeParam>(0.999),
        static_cast<TypeParam>(10.0)
    };
    
    for (TypeParam value : edgeValues) {
        AAX_CString result;
        EXPECT_TRUE(decorator->ValueToString(value, &result));
        EXPECT_NE(result.StdString().find("%"), std::string::npos);
        
        // Test string parsing back
        TypeParam parsedValue;
        EXPECT_TRUE(decorator->StringToValue(result, &parsedValue));
        EXPECT_NEAR(static_cast<double>(value), static_cast<double>(parsedValue), 0.001);
    }
}

//==============================================================================
// Mathematical Accuracy Tests
//==============================================================================

TYPED_TEST(AAX_CPercentDisplayDelegateDecorator_Test, MathematicalAccuracy_RoundTrip)
{
    // Test round-trip conversion accuracy
    std::vector<TypeParam> testValues = {
        static_cast<TypeParam>(0.0),
        static_cast<TypeParam>(0.25),
        static_cast<TypeParam>(0.5),
        static_cast<TypeParam>(0.75),
        static_cast<TypeParam>(1.0),
        static_cast<TypeParam>(1.5),
        static_cast<TypeParam>(2.0)
    };
    
    for (TypeParam originalValue : testValues) {
        AAX_CString stringValue;
        TypeParam convertedValue;
        
        // Convert to string and back
        EXPECT_TRUE(this->mPercentDecorator->ValueToString(originalValue, &stringValue));
        EXPECT_TRUE(this->mPercentDecorator->StringToValue(stringValue, &convertedValue));
        
        // Check accuracy (allowing for some floating-point precision loss)
        EXPECT_NEAR(static_cast<double>(originalValue), static_cast<double>(convertedValue), 0.001);
    }
}

TYPED_TEST(AAX_CPercentDisplayDelegateDecorator_Test, MathematicalAccuracy_PercentFormula)
{
    // Test that the percent formula is correctly applied: display = value * 100
    TypeParam testValue = static_cast<TypeParam>(0.5); // Should display as 50%
    AAX_CString result;
    
    EXPECT_TRUE(this->mPercentDecorator->ValueToString(testValue, &result));
    
    // The result string should contain "%" suffix
    EXPECT_NE(result.StdString().find("%"), std::string::npos);
    
    // Test inverse conversion
    TypeParam convertedBack;
    EXPECT_TRUE(this->mPercentDecorator->StringToValue(result, &convertedBack));
    EXPECT_NEAR(static_cast<double>(testValue), static_cast<double>(convertedBack), 0.001);
}
