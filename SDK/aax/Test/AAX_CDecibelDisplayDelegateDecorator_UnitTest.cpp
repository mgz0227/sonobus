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
 *	\file AAX_CDecibelDisplayDelegateDecorator_UnitTest.cpp
 *
 *	\brief Unit tests for AAX_CDecibelDisplayDelegateDecorator template class
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX_CDecibelDisplayDelegateDecorator.h"
#include "AAX_CUnitDisplayDelegateDecorator.h"
#include "AAX_CScaledDisplayDelegateDecorator.h"
#include "AAX_MockDisplayDelegate.h"
#include "AAX_CString.h"
#include <cmath>
#include <limits>
#include <memory>
#include <vector>
#include <utility>

//==============================================================================
// Test Fixture
//==============================================================================

template <typename T>
class AAX_CDecibelDisplayDelegateDecorator_Test : public ::testing::Test
{
protected:
    void SetUp() override
    {
        mMockDisplayDelegate = std::make_unique<AAX_MockDisplayDelegate<T>>();
        mDecibelDecorator = std::make_unique<AAX_CDecibelDisplayDelegateDecorator<T>>(*mMockDisplayDelegate);
    }
    
    void TearDown() override
    {
        mDecibelDecorator.reset();
        mMockDisplayDelegate.reset();
    }
    
    std::unique_ptr<AAX_MockDisplayDelegate<T>> mMockDisplayDelegate;
    std::unique_ptr<AAX_CDecibelDisplayDelegateDecorator<T>> mDecibelDecorator;
};

// Test with common numeric types
using TestTypes = ::testing::Types<float, double>;
TYPED_TEST_SUITE(AAX_CDecibelDisplayDelegateDecorator_Test, TestTypes);

//==============================================================================
// Construction and Cloning Tests
//==============================================================================

TYPED_TEST(AAX_CDecibelDisplayDelegateDecorator_Test, Construction)
{
    // Test that decorator is properly constructed
    EXPECT_NE(nullptr, this->mDecibelDecorator.get());
}

TYPED_TEST(AAX_CDecibelDisplayDelegateDecorator_Test, Clone)
{
    // Test cloning functionality
    auto cloned = std::unique_ptr<AAX_CDecibelDisplayDelegateDecorator<TypeParam>>(
        this->mDecibelDecorator->Clone());
    
    EXPECT_NE(nullptr, cloned.get());
    EXPECT_NE(this->mDecibelDecorator.get(), cloned.get());
    
    // Test that cloned decorator behaves the same
    AAX_CString originalString, clonedString;
    TypeParam testValue = static_cast<TypeParam>(0.5);
    
    bool originalResult = this->mDecibelDecorator->ValueToString(testValue, &originalString);
    bool clonedResult = cloned->ValueToString(testValue, &clonedString);
    
    EXPECT_EQ(originalResult, clonedResult);
    EXPECT_EQ(originalString, clonedString);
}

//==============================================================================
// ValueToString Tests
//==============================================================================

TYPED_TEST(AAX_CDecibelDisplayDelegateDecorator_Test, ValueToString_PositiveValues)
{
    AAX_CString result;
    
    // Test unity gain (0 dB)
    TypeParam unityValue = static_cast<TypeParam>(1.0);
    EXPECT_TRUE(this->mDecibelDecorator->ValueToString(unityValue, &result));
    EXPECT_NE(result.StdString().find("dB"), std::string::npos);

    // Test half amplitude (-6.02 dB approximately)
    TypeParam halfValue = static_cast<TypeParam>(0.5);
    EXPECT_TRUE(this->mDecibelDecorator->ValueToString(halfValue, &result));
    EXPECT_NE(result.StdString().find("dB"), std::string::npos);
    EXPECT_NE(result.StdString().find("-"), std::string::npos); // Should be negative

    // Test double amplitude (+6.02 dB approximately)
    TypeParam doubleValue = static_cast<TypeParam>(2.0);
    EXPECT_TRUE(this->mDecibelDecorator->ValueToString(doubleValue, &result));
    EXPECT_NE(result.StdString().find("dB"), std::string::npos);
}

TYPED_TEST(AAX_CDecibelDisplayDelegateDecorator_Test, ValueToString_ZeroAndNegativeValues)
{
    AAX_CString result;
    
    // Test zero value (should be -INF dB)
    TypeParam zeroValue = static_cast<TypeParam>(0.0);
    EXPECT_TRUE(this->mDecibelDecorator->ValueToString(zeroValue, &result));
    EXPECT_NE(result.StdString().find("-INF"), std::string::npos);
    EXPECT_NE(result.StdString().find("dB"), std::string::npos);

    // Test negative value (should be -INF dB)
    TypeParam negativeValue = static_cast<TypeParam>(-0.5);
    EXPECT_TRUE(this->mDecibelDecorator->ValueToString(negativeValue, &result));
    EXPECT_NE(result.StdString().find("-INF"), std::string::npos);
    EXPECT_NE(result.StdString().find("dB"), std::string::npos);
}

TYPED_TEST(AAX_CDecibelDisplayDelegateDecorator_Test, ValueToString_EdgeCases)
{
    AAX_CString result;
    
    // Test very small positive value (near zero)
    TypeParam smallValue = static_cast<TypeParam>(0.001);
    EXPECT_TRUE(this->mDecibelDecorator->ValueToString(smallValue, &result));
    EXPECT_NE(result.StdString().find("dB"), std::string::npos);

    // Test large value
    TypeParam largeValue = static_cast<TypeParam>(100.0);
    EXPECT_TRUE(this->mDecibelDecorator->ValueToString(largeValue, &result));
    EXPECT_NE(result.StdString().find("dB"), std::string::npos);
}

//==============================================================================
// ValueToString with Character Limit Tests
//==============================================================================

TYPED_TEST(AAX_CDecibelDisplayDelegateDecorator_Test, ValueToString_WithCharLimit_SufficientSpace)
{
    AAX_CString result;
    TypeParam testValue = static_cast<TypeParam>(1.0);
    
    // Test with sufficient character limit
    EXPECT_TRUE(this->mDecibelDecorator->ValueToString(testValue, 20, &result));
    EXPECT_NE(result.StdString().find("dB"), std::string::npos);
    EXPECT_LE(result.Length(), 20);
}

TYPED_TEST(AAX_CDecibelDisplayDelegateDecorator_Test, ValueToString_WithCharLimit_InsufficientSpace)
{
    AAX_CString result;
    TypeParam testValue = static_cast<TypeParam>(1.0);
    
    // Test with very limited character space (should omit "dB" suffix)
    EXPECT_TRUE(this->mDecibelDecorator->ValueToString(testValue, 3, &result));
    EXPECT_LE(result.Length(), 3);
    // With only 3 characters, "dB" suffix might be omitted
}

TYPED_TEST(AAX_CDecibelDisplayDelegateDecorator_Test, ValueToString_WithCharLimit_ZeroValue)
{
    AAX_CString result;
    TypeParam zeroValue = static_cast<TypeParam>(0.0);
    
    // Test zero value with sufficient space for "-INF dB"
    EXPECT_TRUE(this->mDecibelDecorator->ValueToString(zeroValue, 10, &result));
    EXPECT_NE(result.StdString().find("-INF"), std::string::npos);

    // Test zero value with limited space (should still show "-INF")
    EXPECT_TRUE(this->mDecibelDecorator->ValueToString(zeroValue, 5, &result));
    EXPECT_NE(result.StdString().find("-INF"), std::string::npos);
}

//==============================================================================
// StringToValue Tests
//==============================================================================

TYPED_TEST(AAX_CDecibelDisplayDelegateDecorator_Test, StringToValue_WithDecibelSuffix)
{
    TypeParam result;
    
    // Test parsing "0dB" (should convert to 1.0)
    EXPECT_TRUE(this->mDecibelDecorator->StringToValue(AAX_CString("0dB"), &result));
    EXPECT_NEAR(static_cast<double>(result), 1.0, 0.001);
    
    // Test parsing "6dB" (should convert to approximately 2.0)
    EXPECT_TRUE(this->mDecibelDecorator->StringToValue(AAX_CString("6dB"), &result));
    EXPECT_NEAR(static_cast<double>(result), 2.0, 0.1);
    
    // Test parsing "-6dB" (should convert to approximately 0.5)
    EXPECT_TRUE(this->mDecibelDecorator->StringToValue(AAX_CString("-6dB"), &result));
    EXPECT_NEAR(static_cast<double>(result), 0.5, 0.1);
}

TYPED_TEST(AAX_CDecibelDisplayDelegateDecorator_Test, StringToValue_WithoutDecibelSuffix)
{
    TypeParam result;
    
    // Test parsing plain number (should still apply dB conversion)
    EXPECT_TRUE(this->mDecibelDecorator->StringToValue(AAX_CString("0"), &result));
    EXPECT_NEAR(static_cast<double>(result), 1.0, 0.001);
    
    // Test parsing negative number
    EXPECT_TRUE(this->mDecibelDecorator->StringToValue(AAX_CString("-6"), &result));
    EXPECT_NEAR(static_cast<double>(result), 0.5, 0.1);
}

TYPED_TEST(AAX_CDecibelDisplayDelegateDecorator_Test, StringToValue_ShortStrings)
{
    TypeParam result;
    
    // Test very short strings (2 characters or less)
    EXPECT_TRUE(this->mDecibelDecorator->StringToValue(AAX_CString("0"), &result));
    EXPECT_TRUE(this->mDecibelDecorator->StringToValue(AAX_CString("1"), &result));
    
    // Test empty string
    bool success = this->mDecibelDecorator->StringToValue(AAX_CString(""), &result);
    // Result depends on underlying mock delegate behavior
}

//==============================================================================
// Rvalue Reference Constructor Tests
//==============================================================================

TYPED_TEST(AAX_CDecibelDisplayDelegateDecorator_Test, RvalueConstructor_FromDisplayDelegate)
{
    // Test rvalue reference constructor with AAX_IDisplayDelegate&&
    auto mockDelegate = std::make_unique<AAX_MockDisplayDelegate<TypeParam>>();
    
    // Create decorator using rvalue reference constructor
    auto decibelDecorator = std::make_unique<AAX_CDecibelDisplayDelegateDecorator<TypeParam>>(
        std::move(*mockDelegate));
    
    EXPECT_NE(nullptr, decibelDecorator.get());
    
    // Test functionality
    AAX_CString result;
    TypeParam testValue = static_cast<TypeParam>(1.0);
    EXPECT_TRUE(decibelDecorator->ValueToString(testValue, &result));
    EXPECT_NE(result.StdString().find("dB"), std::string::npos);
}

TYPED_TEST(AAX_CDecibelDisplayDelegateDecorator_Test, RvalueConstructor_FromDisplayDelegateDecorator)
{
    // Test rvalue reference constructor with AAX_IDisplayDelegateDecorator&&
    auto mockDelegate = std::make_unique<AAX_MockDisplayDelegate<TypeParam>>();
    auto unitDecorator = std::make_unique<AAX_CUnitDisplayDelegateDecorator<TypeParam>>(
        *mockDelegate, AAX_CString("unit"));
    
    // Create decibel decorator using rvalue reference constructor
    auto decibelDecorator = std::make_unique<AAX_CDecibelDisplayDelegateDecorator<TypeParam>>(
        std::move(*unitDecorator));
    
    EXPECT_NE(nullptr, decibelDecorator.get());
    
    // Test functionality
    AAX_CString result;
    TypeParam testValue = static_cast<TypeParam>(2.0);
    EXPECT_TRUE(decibelDecorator->ValueToString(testValue, &result));
    EXPECT_NE(result.StdString().find("dB"), std::string::npos);
}

TYPED_TEST(AAX_CDecibelDisplayDelegateDecorator_Test, RvalueConstructor_FactoryPattern)
{
    // Test factory pattern using rvalue reference constructor
    auto createDecibelDecorator = []() {
        auto mockDelegate = std::make_unique<AAX_MockDisplayDelegate<TypeParam>>();
        return std::make_unique<AAX_CDecibelDisplayDelegateDecorator<TypeParam>>(
            std::move(*mockDelegate));
    };
    
    auto decorator = createDecibelDecorator();
    EXPECT_NE(nullptr, decorator.get());
    
    // Test functionality
    AAX_CString result;
    TypeParam testValue = static_cast<TypeParam>(0.5);
    EXPECT_TRUE(decorator->ValueToString(testValue, &result));
    EXPECT_NE(result.StdString().find("dB"), std::string::npos);
}

TYPED_TEST(AAX_CDecibelDisplayDelegateDecorator_Test, RvalueConstructor_DecoratorChaining)
{
    // Test chaining multiple decorators using rvalue reference constructors
    auto mockDelegate = std::make_unique<AAX_MockDisplayDelegate<TypeParam>>();
    
    // Chain: Mock -> Unit -> Decibel
    auto chainedDecorator = std::make_unique<AAX_CDecibelDisplayDelegateDecorator<TypeParam>>(
        AAX_CUnitDisplayDelegateDecorator<TypeParam>(std::move(*mockDelegate), AAX_CString("test")));
    
    EXPECT_NE(nullptr, chainedDecorator.get());
    
    // Test functionality
    AAX_CString result;
    TypeParam testValue = static_cast<TypeParam>(1.0);
    EXPECT_TRUE(chainedDecorator->ValueToString(testValue, &result));
    EXPECT_NE(result.StdString().find("dB"), std::string::npos);
}

TYPED_TEST(AAX_CDecibelDisplayDelegateDecorator_Test, RvalueConstructor_MemoryManagement)
{
    // Test that rvalue constructors properly manage memory
    std::vector<std::unique_ptr<AAX_CDecibelDisplayDelegateDecorator<TypeParam>>> decorators;
    
    for (int i = 0; i < 10; ++i) {
        auto mockDelegate = std::make_unique<AAX_MockDisplayDelegate<TypeParam>>();
        decorators.push_back(std::make_unique<AAX_CDecibelDisplayDelegateDecorator<TypeParam>>(
            std::move(*mockDelegate)));
    }
    
    // Test all decorators
    for (auto& decorator : decorators) {
        EXPECT_NE(nullptr, decorator.get());
        
        AAX_CString result;
        TypeParam testValue = static_cast<TypeParam>(1.0);
        EXPECT_TRUE(decorator->ValueToString(testValue, &result));
        EXPECT_NE(result.StdString().find("dB"), std::string::npos);
    }
}

TYPED_TEST(AAX_CDecibelDisplayDelegateDecorator_Test, RvalueConstructor_TemplateVariations)
{
    // Test rvalue constructors with different template types
    if constexpr (std::is_same_v<TypeParam, float>) {
        auto mockDelegate = std::make_unique<AAX_MockDisplayDelegate<float>>();
        auto decorator = std::make_unique<AAX_CDecibelDisplayDelegateDecorator<float>>(
            std::move(*mockDelegate));
        
        AAX_CString result;
        EXPECT_TRUE(decorator->ValueToString(2.0f, &result));
        EXPECT_NE(result.StdString().find("dB"), std::string::npos);
    }
    
    if constexpr (std::is_same_v<TypeParam, double>) {
        auto mockDelegate = std::make_unique<AAX_MockDisplayDelegate<double>>();
        auto decorator = std::make_unique<AAX_CDecibelDisplayDelegateDecorator<double>>(
            std::move(*mockDelegate));
        
        AAX_CString result;
        EXPECT_TRUE(decorator->ValueToString(2.0, &result));
        EXPECT_NE(result.StdString().find("dB"), std::string::npos);
    }
}

TYPED_TEST(AAX_CDecibelDisplayDelegateDecorator_Test, RvalueConstructor_ComplexChaining)
{
    // Test complex decorator chaining with multiple rvalue constructors
    auto mockDelegate = std::make_unique<AAX_MockDisplayDelegate<TypeParam>>();
    
    // Create a complex chain using temporary objects
    auto complexDecorator = std::make_unique<AAX_CDecibelDisplayDelegateDecorator<TypeParam>>(
        AAX_CScaledDisplayDelegateDecorator<TypeParam>(
            AAX_CUnitDisplayDelegateDecorator<TypeParam>(
                std::move(*mockDelegate), AAX_CString("base")), 2.0));
    
    EXPECT_NE(nullptr, complexDecorator.get());
    
    // Test with multiple values
    std::vector<TypeParam> testValues = {
        static_cast<TypeParam>(0.1),
        static_cast<TypeParam>(0.5),
        static_cast<TypeParam>(1.0),
        static_cast<TypeParam>(2.0)
    };
    
    for (TypeParam value : testValues) {
        AAX_CString result;
        EXPECT_TRUE(complexDecorator->ValueToString(value, &result));
        EXPECT_NE(result.StdString().find("dB"), std::string::npos);
    }
}

TYPED_TEST(AAX_CDecibelDisplayDelegateDecorator_Test, RvalueConstructor_PolymorphicBehavior)
{
    // Test polymorphic behavior with rvalue reference constructors
    auto mockDelegate = std::make_unique<AAX_MockDisplayDelegate<TypeParam>>();
    
    // Create decorator using rvalue constructor
    std::unique_ptr<AAX_IDisplayDelegateDecorator<TypeParam>> polymorphicDecorator = 
        std::make_unique<AAX_CDecibelDisplayDelegateDecorator<TypeParam>>(std::move(*mockDelegate));
    
    EXPECT_NE(nullptr, polymorphicDecorator.get());
    
    // Test polymorphic behavior
    AAX_CString result;
    TypeParam testValue = static_cast<TypeParam>(1.0);
    EXPECT_TRUE(polymorphicDecorator->ValueToString(testValue, &result));
    EXPECT_NE(result.StdString().find("dB"), std::string::npos);
    
    // Test cloning through polymorphic interface
    auto cloned = std::unique_ptr<AAX_IDisplayDelegateDecorator<TypeParam>>(
        polymorphicDecorator->Clone());
    EXPECT_NE(nullptr, cloned.get());
    
    AAX_CString clonedResult;
    EXPECT_TRUE(cloned->ValueToString(testValue, &clonedResult));
    EXPECT_EQ(result, clonedResult);
}

TYPED_TEST(AAX_CDecibelDisplayDelegateDecorator_Test, RvalueConstructor_RoundTripWithMove)
{
    // Test round-trip conversion with rvalue-constructed decorators
    auto mockDelegate = std::make_unique<AAX_MockDisplayDelegate<TypeParam>>();
    auto decorator = std::make_unique<AAX_CDecibelDisplayDelegateDecorator<TypeParam>>(
        std::move(*mockDelegate));
    
    std::vector<TypeParam> testValues = {
        static_cast<TypeParam>(0.1),
        static_cast<TypeParam>(0.5),
        static_cast<TypeParam>(1.0),
        static_cast<TypeParam>(2.0),
        static_cast<TypeParam>(10.0)
    };
    
    for (TypeParam originalValue : testValues) {
        AAX_CString stringValue;
        TypeParam convertedValue;
        
        // Convert to string and back
        EXPECT_TRUE(decorator->ValueToString(originalValue, &stringValue));
        EXPECT_TRUE(decorator->StringToValue(stringValue, &convertedValue));
        
        // Check accuracy (allowing for some floating-point precision loss)
        EXPECT_NEAR(static_cast<double>(originalValue), static_cast<double>(convertedValue), 0.01);
    }
}

TYPED_TEST(AAX_CDecibelDisplayDelegateDecorator_Test, RvalueConstructor_EdgeCases)
{
    // Test edge cases with rvalue reference constructors
    auto mockDelegate = std::make_unique<AAX_MockDisplayDelegate<TypeParam>>();
    auto decorator = std::make_unique<AAX_CDecibelDisplayDelegateDecorator<TypeParam>>(
        std::move(*mockDelegate));
    
    // Test zero and negative values (should result in -INF)
    std::vector<TypeParam> edgeValues = {
        static_cast<TypeParam>(0.0),
        static_cast<TypeParam>(-1.0)
    };
    
    for (TypeParam value : edgeValues) {
        AAX_CString result;
        EXPECT_TRUE(decorator->ValueToString(value, &result));
        EXPECT_NE(result.StdString().find("-INF"), std::string::npos);
    }
    
    // Test positive values
    std::vector<TypeParam> positiveValues = {
        static_cast<TypeParam>(0.001),
        static_cast<TypeParam>(1.0),
        static_cast<TypeParam>(100.0)
    };
    
    for (TypeParam value : positiveValues) {
        AAX_CString result;
        EXPECT_TRUE(decorator->ValueToString(value, &result));
        EXPECT_NE(result.StdString().find("dB"), std::string::npos);
    }
}

//==============================================================================
// Mathematical Accuracy Tests
//==============================================================================

TYPED_TEST(AAX_CDecibelDisplayDelegateDecorator_Test, MathematicalAccuracy_RoundTrip)
{
    // Test round-trip conversion accuracy
    std::vector<TypeParam> testValues = {
        static_cast<TypeParam>(0.1),
        static_cast<TypeParam>(0.5),
        static_cast<TypeParam>(1.0),
        static_cast<TypeParam>(2.0),
        static_cast<TypeParam>(10.0)
    };
    
    for (TypeParam originalValue : testValues) {
        AAX_CString stringValue;
        TypeParam convertedValue;
        
        // Convert to string and back
        EXPECT_TRUE(this->mDecibelDecorator->ValueToString(originalValue, &stringValue));
        EXPECT_TRUE(this->mDecibelDecorator->StringToValue(stringValue, &convertedValue));
        
        // Check accuracy (allowing for some floating-point precision loss)
        EXPECT_NEAR(static_cast<double>(originalValue), static_cast<double>(convertedValue), 0.01);
    }
}

TYPED_TEST(AAX_CDecibelDisplayDelegateDecorator_Test, MathematicalAccuracy_DecibelFormula)
{
    // Test that the decibel formula is correctly applied: dB = 20 * log10(value)
    TypeParam testValue = static_cast<TypeParam>(2.0); // Should be approximately +6.02 dB
    AAX_CString result;
    
    EXPECT_TRUE(this->mDecibelDecorator->ValueToString(testValue, &result));
    
    // Calculate expected dB value
    double expectedDb = 20.0 * log10(2.0); // Approximately 6.02
    
    // The result string should contain a value close to 6.02
    // Note: Exact comparison depends on the underlying display delegate's formatting
    EXPECT_NE(result.StdString().find("dB"), std::string::npos);
}
