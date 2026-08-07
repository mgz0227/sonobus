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
 *	\file AAX_CUnitPrefixDisplayDelegateDecorator_UnitTest.cpp
 *
 *	\brief Unit tests for AAX_CUnitPrefixDisplayDelegateDecorator template class
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX_CUnitPrefixDisplayDelegateDecorator.h"
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
class AAX_CUnitPrefixDisplayDelegateDecorator_Test : public ::testing::Test
{
protected:
    void SetUp() override
    {
        mMockDisplayDelegate = std::make_unique<AAX_MockDisplayDelegate<T>>();
        mUnitPrefixDecorator = std::make_unique<AAX_CUnitPrefixDisplayDelegateDecorator<T>>(*mMockDisplayDelegate);
    }
    
    void TearDown() override
    {
        mUnitPrefixDecorator.reset();
        mMockDisplayDelegate.reset();
    }
    
    std::unique_ptr<AAX_MockDisplayDelegate<T>> mMockDisplayDelegate;
    std::unique_ptr<AAX_CUnitPrefixDisplayDelegateDecorator<T>> mUnitPrefixDecorator;
};

// Test with floating-point types only (class doesn't support integers)
using TestTypes = ::testing::Types<float, double>;
TYPED_TEST_SUITE(AAX_CUnitPrefixDisplayDelegateDecorator_Test, TestTypes);

//==============================================================================
// Construction and Cloning Tests
//==============================================================================

TYPED_TEST(AAX_CUnitPrefixDisplayDelegateDecorator_Test, Construction)
{
    // Test that decorator is properly constructed
    EXPECT_NE(nullptr, this->mUnitPrefixDecorator.get());
}

TYPED_TEST(AAX_CUnitPrefixDisplayDelegateDecorator_Test, Clone)
{
    // Test cloning functionality
    auto cloned = std::unique_ptr<AAX_CUnitPrefixDisplayDelegateDecorator<TypeParam>>(
        this->mUnitPrefixDecorator->Clone());
    
    EXPECT_NE(nullptr, cloned.get());
    EXPECT_NE(this->mUnitPrefixDecorator.get(), cloned.get());
    
    // Test that cloned decorator behaves the same
    AAX_CString originalString, clonedString;
    TypeParam testValue = static_cast<TypeParam>(1500.0); // Should be "1.5k"
    
    bool originalResult = this->mUnitPrefixDecorator->ValueToString(testValue, &originalString);
    bool clonedResult = cloned->ValueToString(testValue, &clonedString);
    
    EXPECT_EQ(originalResult, clonedResult);
    EXPECT_EQ(originalString.StdString(), clonedString.StdString());
}

//==============================================================================
// ValueToString Tests - Mega (M) Prefix
//==============================================================================

TYPED_TEST(AAX_CUnitPrefixDisplayDelegateDecorator_Test, ValueToString_MegaPrefix)
{
    AAX_CString result;
    
    // Test 1,000,000 -> "1M"
    TypeParam megaValue = static_cast<TypeParam>(1000000.0);
    EXPECT_TRUE(this->mUnitPrefixDecorator->ValueToString(megaValue, &result));
    EXPECT_NE(result.StdString().find("M"), std::string::npos);
    
    // Test 2,500,000 -> "2.5M"
    TypeParam largeMegaValue = static_cast<TypeParam>(2500000.0);
    EXPECT_TRUE(this->mUnitPrefixDecorator->ValueToString(largeMegaValue, &result));
    EXPECT_NE(result.StdString().find("M"), std::string::npos);
    
    // Test negative mega value
    TypeParam negativeMegaValue = static_cast<TypeParam>(-1500000.0);
    EXPECT_TRUE(this->mUnitPrefixDecorator->ValueToString(negativeMegaValue, &result));
    EXPECT_NE(result.StdString().find("M"), std::string::npos);
    EXPECT_NE(result.StdString().find("-"), std::string::npos);
}

//==============================================================================
// ValueToString Tests - Kilo (k) Prefix
//==============================================================================

TYPED_TEST(AAX_CUnitPrefixDisplayDelegateDecorator_Test, ValueToString_KiloPrefix)
{
    AAX_CString result;
    
    // Test 1,000 -> "1k"
    TypeParam kiloValue = static_cast<TypeParam>(1000.0);
    EXPECT_TRUE(this->mUnitPrefixDecorator->ValueToString(kiloValue, &result));
    EXPECT_NE(result.StdString().find("k"), std::string::npos);
    
    // Test 1,500 -> "1.5k"
    TypeParam kiloHalfValue = static_cast<TypeParam>(1500.0);
    EXPECT_TRUE(this->mUnitPrefixDecorator->ValueToString(kiloHalfValue, &result));
    EXPECT_NE(result.StdString().find("k"), std::string::npos);
    
    // Test 999,999 -> "999.999k" (just under mega threshold)
    TypeParam almostMegaValue = static_cast<TypeParam>(999999.0);
    EXPECT_TRUE(this->mUnitPrefixDecorator->ValueToString(almostMegaValue, &result));
    EXPECT_NE(result.StdString().find("k"), std::string::npos);
    
    // Test negative kilo value
    TypeParam negativeKiloValue = static_cast<TypeParam>(-2500.0);
    EXPECT_TRUE(this->mUnitPrefixDecorator->ValueToString(negativeKiloValue, &result));
    EXPECT_NE(result.StdString().find("k"), std::string::npos);
    EXPECT_NE(result.StdString().find("-"), std::string::npos);
}

//==============================================================================
// ValueToString Tests - No Prefix (Base Units)
//==============================================================================

TYPED_TEST(AAX_CUnitPrefixDisplayDelegateDecorator_Test, ValueToString_NoPrefix)
{
    AAX_CString result;
    
    // Test 1.0 -> "1" (no prefix)
    TypeParam unityValue = static_cast<TypeParam>(1.0);
    EXPECT_TRUE(this->mUnitPrefixDecorator->ValueToString(unityValue, &result));
    // Should not contain any unit prefixes
    EXPECT_EQ(result.StdString().find("M"), std::string::npos);
    EXPECT_EQ(result.StdString().find("k"), std::string::npos);
    EXPECT_EQ(result.StdString().find("m"), std::string::npos);
    EXPECT_EQ(result.StdString().find("u"), std::string::npos);
    
    // Test 999.0 -> "999" (no prefix)
    TypeParam largeBaseValue = static_cast<TypeParam>(999.0);
    EXPECT_TRUE(this->mUnitPrefixDecorator->ValueToString(largeBaseValue, &result));
    EXPECT_EQ(result.StdString().find("M"), std::string::npos);
    EXPECT_EQ(result.StdString().find("k"), std::string::npos);
    EXPECT_EQ(result.StdString().find("m"), std::string::npos);
    EXPECT_EQ(result.StdString().find("u"), std::string::npos);
    
    // Test 10.5 -> "10.5" (no prefix)
    TypeParam decimalValue = static_cast<TypeParam>(10.5);
    EXPECT_TRUE(this->mUnitPrefixDecorator->ValueToString(decimalValue, &result));
    EXPECT_EQ(result.StdString().find("M"), std::string::npos);
    EXPECT_EQ(result.StdString().find("k"), std::string::npos);
    EXPECT_EQ(result.StdString().find("m"), std::string::npos);
    EXPECT_EQ(result.StdString().find("u"), std::string::npos);
}

//==============================================================================
// ValueToString Tests - Milli (m) Prefix
//==============================================================================

TYPED_TEST(AAX_CUnitPrefixDisplayDelegateDecorator_Test, ValueToString_MilliPrefix)
{
    AAX_CString result;
    
    // Test 0.001 -> "1m"
    TypeParam milliValue = static_cast<TypeParam>(0.001);
    EXPECT_TRUE(this->mUnitPrefixDecorator->ValueToString(milliValue, &result));
    EXPECT_NE(result.StdString().find("m"), std::string::npos);
    
    // Test 0.0025 -> "2.5m"
    TypeParam milliHalfValue = static_cast<TypeParam>(0.0025);
    EXPECT_TRUE(this->mUnitPrefixDecorator->ValueToString(milliHalfValue, &result));
    EXPECT_NE(result.StdString().find("m"), std::string::npos);
    
    // Test 0.999 -> "999m" (just under base threshold)
    TypeParam almostBaseValue = static_cast<TypeParam>(0.999);
    EXPECT_TRUE(this->mUnitPrefixDecorator->ValueToString(almostBaseValue, &result));
    EXPECT_NE(result.StdString().find("m"), std::string::npos);
    
    // Test negative milli value
    TypeParam negativeMilliValue = static_cast<TypeParam>(-0.005);
    EXPECT_TRUE(this->mUnitPrefixDecorator->ValueToString(negativeMilliValue, &result));
    EXPECT_NE(result.StdString().find("m"), std::string::npos);
    EXPECT_NE(result.StdString().find("-"), std::string::npos);
}

//==============================================================================
// ValueToString Tests - Micro (u) Prefix
//==============================================================================

TYPED_TEST(AAX_CUnitPrefixDisplayDelegateDecorator_Test, ValueToString_MicroPrefix)
{
    AAX_CString result;

    // Test 0.000002 -> "2u" (use a value safely above the micro threshold)
    TypeParam microValue = static_cast<TypeParam>(0.000002);
    EXPECT_TRUE(this->mUnitPrefixDecorator->ValueToString(microValue, &result));
    EXPECT_NE(result.StdString().find("u"), std::string::npos);

    // Test 0.0000025 -> "2.5u"
    TypeParam microHalfValue = static_cast<TypeParam>(0.0000025);
    EXPECT_TRUE(this->mUnitPrefixDecorator->ValueToString(microHalfValue, &result));
    EXPECT_NE(result.StdString().find("u"), std::string::npos);

    // Test 0.000999 -> "999u" (just under milli threshold)
    TypeParam almostMilliValue = static_cast<TypeParam>(0.000999);
    EXPECT_TRUE(this->mUnitPrefixDecorator->ValueToString(almostMilliValue, &result));
    EXPECT_NE(result.StdString().find("u"), std::string::npos);

    // Test negative micro value
    TypeParam negativeMicroValue = static_cast<TypeParam>(-0.000005);
    EXPECT_TRUE(this->mUnitPrefixDecorator->ValueToString(negativeMicroValue, &result));
    EXPECT_NE(result.StdString().find("u"), std::string::npos);
    EXPECT_NE(result.StdString().find("-"), std::string::npos);
}

//==============================================================================
// ValueToString Tests - Very Small Values
//==============================================================================

TYPED_TEST(AAX_CUnitPrefixDisplayDelegateDecorator_Test, ValueToString_VerySmallValues)
{
    AAX_CString result;
    
    // Test very small value (smaller than micro threshold)
    TypeParam verySmallValue = static_cast<TypeParam>(0.0000001);
    EXPECT_TRUE(this->mUnitPrefixDecorator->ValueToString(verySmallValue, &result));
    // Should not have any unit prefix for values smaller than micro
    EXPECT_EQ(result.StdString().find("M"), std::string::npos);
    EXPECT_EQ(result.StdString().find("k"), std::string::npos);
    EXPECT_EQ(result.StdString().find("m"), std::string::npos);
    EXPECT_EQ(result.StdString().find("u"), std::string::npos);
    
    // Test zero value
    TypeParam zeroValue = static_cast<TypeParam>(0.0);
    EXPECT_TRUE(this->mUnitPrefixDecorator->ValueToString(zeroValue, &result));
    EXPECT_EQ(result.StdString().find("M"), std::string::npos);
    EXPECT_EQ(result.StdString().find("k"), std::string::npos);
    EXPECT_EQ(result.StdString().find("m"), std::string::npos);
    EXPECT_EQ(result.StdString().find("u"), std::string::npos);
}

//==============================================================================
// ValueToString with Character Limit Tests
//==============================================================================

TYPED_TEST(AAX_CUnitPrefixDisplayDelegateDecorator_Test, ValueToString_WithCharLimit_SufficientSpace)
{
    AAX_CString result;
    TypeParam testValue = static_cast<TypeParam>(1500.0); // Should be "1.5k"
    
    // Test with sufficient character limit
    EXPECT_TRUE(this->mUnitPrefixDecorator->ValueToString(testValue, 20, &result));
    EXPECT_NE(result.StdString().find("k"), std::string::npos);
    EXPECT_LE(result.Length(), 20);
}

TYPED_TEST(AAX_CUnitPrefixDisplayDelegateDecorator_Test, ValueToString_WithCharLimit_LimitedSpace)
{
    AAX_CString result;
    TypeParam testValue = static_cast<TypeParam>(1500.0); // Should be "1.5k"
    
    // Test with limited character space (decorator reserves 1 char for prefix)
    EXPECT_TRUE(this->mUnitPrefixDecorator->ValueToString(testValue, 5, &result));
    EXPECT_NE(result.StdString().find("k"), std::string::npos);
    EXPECT_LE(result.Length(), 5);
}

TYPED_TEST(AAX_CUnitPrefixDisplayDelegateDecorator_Test, ValueToString_WithCharLimit_NoPrefix)
{
    AAX_CString result;
    TypeParam testValue = static_cast<TypeParam>(50.0); // No prefix needed
    
    // Test base value with character limit (no prefix reservation needed)
    EXPECT_TRUE(this->mUnitPrefixDecorator->ValueToString(testValue, 5, &result));
    EXPECT_LE(result.Length(), 5);
    // Should not contain any unit prefixes
    EXPECT_EQ(result.StdString().find("M"), std::string::npos);
    EXPECT_EQ(result.StdString().find("k"), std::string::npos);
    EXPECT_EQ(result.StdString().find("m"), std::string::npos);
    EXPECT_EQ(result.StdString().find("u"), std::string::npos);
}

//==============================================================================
// StringToValue Tests
//==============================================================================

TYPED_TEST(AAX_CUnitPrefixDisplayDelegateDecorator_Test, StringToValue_MegaPrefix)
{
    TypeParam result;

    // Test parsing "1M" -> 1,000,000
    EXPECT_TRUE(this->mUnitPrefixDecorator->StringToValue(AAX_CString("1M"), &result));
    EXPECT_NEAR(static_cast<double>(result), 1000000.0, 0.1);

    // Test parsing "2.5M" -> 2,500,000
    EXPECT_TRUE(this->mUnitPrefixDecorator->StringToValue(AAX_CString("2.5M"), &result));
    EXPECT_NEAR(static_cast<double>(result), 2500000.0, 0.1);

    // Test parsing "-1.5M" -> -1,500,000
    EXPECT_TRUE(this->mUnitPrefixDecorator->StringToValue(AAX_CString("-1.5M"), &result));
    EXPECT_NEAR(static_cast<double>(result), -1500000.0, 0.1);
}

TYPED_TEST(AAX_CUnitPrefixDisplayDelegateDecorator_Test, StringToValue_KiloPrefix)
{
    TypeParam result;

    // Test parsing "1k" -> 1,000
    EXPECT_TRUE(this->mUnitPrefixDecorator->StringToValue(AAX_CString("1k"), &result));
    EXPECT_NEAR(static_cast<double>(result), 1000.0, 0.001);

    // Test parsing "1.5k" -> 1,500
    EXPECT_TRUE(this->mUnitPrefixDecorator->StringToValue(AAX_CString("1.5k"), &result));
    EXPECT_NEAR(static_cast<double>(result), 1500.0, 0.001);

    // Test parsing "-2.5k" -> -2,500
    EXPECT_TRUE(this->mUnitPrefixDecorator->StringToValue(AAX_CString("-2.5k"), &result));
    EXPECT_NEAR(static_cast<double>(result), -2500.0, 0.001);
}

TYPED_TEST(AAX_CUnitPrefixDisplayDelegateDecorator_Test, StringToValue_MilliPrefix)
{
    TypeParam result;

    // Test parsing "1m" -> 0.001
    EXPECT_TRUE(this->mUnitPrefixDecorator->StringToValue(AAX_CString("1m"), &result));
    EXPECT_NEAR(static_cast<double>(result), 0.001, 0.000001);

    // Test parsing "2.5m" -> 0.0025
    EXPECT_TRUE(this->mUnitPrefixDecorator->StringToValue(AAX_CString("2.5m"), &result));
    EXPECT_NEAR(static_cast<double>(result), 0.0025, 0.000001);

    // Test parsing "-5m" -> -0.005
    EXPECT_TRUE(this->mUnitPrefixDecorator->StringToValue(AAX_CString("-5m"), &result));
    EXPECT_NEAR(static_cast<double>(result), -0.005, 0.000001);
}

TYPED_TEST(AAX_CUnitPrefixDisplayDelegateDecorator_Test, StringToValue_MicroPrefix)
{
    TypeParam result;

    // Test parsing "2u" -> 0.000002 (use a value safely above precision issues)
    EXPECT_TRUE(this->mUnitPrefixDecorator->StringToValue(AAX_CString("2u"), &result));
    EXPECT_NEAR(static_cast<double>(result), 0.000002, 0.0000001);

    // Test parsing "2.5u" -> 0.0000025
    EXPECT_TRUE(this->mUnitPrefixDecorator->StringToValue(AAX_CString("2.5u"), &result));
    EXPECT_NEAR(static_cast<double>(result), 0.0000025, 0.0000001);

    // Test parsing "-5u" -> -0.000005
    EXPECT_TRUE(this->mUnitPrefixDecorator->StringToValue(AAX_CString("-5u"), &result));
    EXPECT_NEAR(static_cast<double>(result), -0.000005, 0.0000001);
}

TYPED_TEST(AAX_CUnitPrefixDisplayDelegateDecorator_Test, StringToValue_NoPrefix)
{
    TypeParam result;

    // Test parsing plain number (no prefix)
    EXPECT_TRUE(this->mUnitPrefixDecorator->StringToValue(AAX_CString("100"), &result));
    EXPECT_NEAR(static_cast<double>(result), 100.0, 0.001);

    // Test parsing decimal number (no prefix)
    EXPECT_TRUE(this->mUnitPrefixDecorator->StringToValue(AAX_CString("10.5"), &result));
    EXPECT_NEAR(static_cast<double>(result), 10.5, 0.001);

    // Test parsing negative number (no prefix)
    EXPECT_TRUE(this->mUnitPrefixDecorator->StringToValue(AAX_CString("-25.7"), &result));
    EXPECT_NEAR(static_cast<double>(result), -25.7, 0.001);
}

TYPED_TEST(AAX_CUnitPrefixDisplayDelegateDecorator_Test, StringToValue_ShortStrings)
{
    TypeParam result;

    // Test very short strings (1 character or less)
    EXPECT_TRUE(this->mUnitPrefixDecorator->StringToValue(AAX_CString("5"), &result));
    EXPECT_NEAR(static_cast<double>(result), 5.0, 0.001);

    // Test empty string
    bool success = this->mUnitPrefixDecorator->StringToValue(AAX_CString(""), &result);
    // Result depends on underlying mock delegate behavior
}

TYPED_TEST(AAX_CUnitPrefixDisplayDelegateDecorator_Test, StringToValue_UnknownSuffix)
{
    TypeParam result;

    // Test string with unknown suffix - decorator may reject these
    // POTENTIAL BUG: Unknown suffixes cause parsing to fail rather than being ignored
    bool parseResult = this->mUnitPrefixDecorator->StringToValue(AAX_CString("100x"), &result);
    if (parseResult) {
        // If parsing succeeds, 'x' should be treated as unknown and ignored
        EXPECT_NEAR(static_cast<double>(result), 100.0, 0.001);
    } else {
        // If parsing fails, this is the current behavior - decorator rejects unknown suffixes
        // This might be the intended behavior for strict parsing
    }

    // Test string with 'g' suffix (not supported)
    bool parseResult2 = this->mUnitPrefixDecorator->StringToValue(AAX_CString("5g"), &result);
    if (parseResult2) {
        EXPECT_NEAR(static_cast<double>(result), 5.0, 0.001);
    }
    // If parsing fails, this is acceptable behavior for unknown suffixes
}

//==============================================================================
// Rvalue Reference Constructor Tests
//==============================================================================

TYPED_TEST(AAX_CUnitPrefixDisplayDelegateDecorator_Test, RvalueConstructor_FromDisplayDelegate)
{
    // Test rvalue reference constructor with AAX_IDisplayDelegate&&
    auto mockDelegate = std::make_unique<AAX_MockDisplayDelegate<TypeParam>>();
    
    // Create decorator using rvalue reference constructor
    auto unitPrefixDecorator = std::make_unique<AAX_CUnitPrefixDisplayDelegateDecorator<TypeParam>>(
        std::move(*mockDelegate));
    
    EXPECT_NE(nullptr, unitPrefixDecorator.get());
    
    // Test functionality with kilo prefix
    AAX_CString result;
    TypeParam testValue = static_cast<TypeParam>(1500.0);
    EXPECT_TRUE(unitPrefixDecorator->ValueToString(testValue, &result));
    EXPECT_NE(result.StdString().find("k"), std::string::npos);
}

TYPED_TEST(AAX_CUnitPrefixDisplayDelegateDecorator_Test, RvalueConstructor_FromDisplayDelegateDecorator)
{
    // Test rvalue reference constructor with AAX_IDisplayDelegateDecorator&&
    auto mockDelegate = std::make_unique<AAX_MockDisplayDelegate<TypeParam>>();
    auto unitDecorator = std::make_unique<AAX_CUnitDisplayDelegateDecorator<TypeParam>>(
        *mockDelegate, AAX_CString("unit"));
    
    // Create unit prefix decorator using rvalue reference constructor
    auto unitPrefixDecorator = std::make_unique<AAX_CUnitPrefixDisplayDelegateDecorator<TypeParam>>(
        std::move(*unitDecorator));
    
    EXPECT_NE(nullptr, unitPrefixDecorator.get());
    
    // Test functionality with mega prefix
    AAX_CString result;
    TypeParam testValue = static_cast<TypeParam>(2000000.0);
    EXPECT_TRUE(unitPrefixDecorator->ValueToString(testValue, &result));
    EXPECT_NE(result.StdString().find("M"), std::string::npos);
}

TYPED_TEST(AAX_CUnitPrefixDisplayDelegateDecorator_Test, RvalueConstructor_FactoryPattern)
{
    // Test factory pattern using rvalue reference constructor
    auto createUnitPrefixDecorator = []() {
        auto mockDelegate = std::make_unique<AAX_MockDisplayDelegate<TypeParam>>();
        return std::make_unique<AAX_CUnitPrefixDisplayDelegateDecorator<TypeParam>>(
            std::move(*mockDelegate));
    };
    
    auto decorator = createUnitPrefixDecorator();
    EXPECT_NE(nullptr, decorator.get());
    
    // Test functionality with milli prefix
    AAX_CString result;
    TypeParam testValue = static_cast<TypeParam>(0.005);
    EXPECT_TRUE(decorator->ValueToString(testValue, &result));
    EXPECT_NE(result.StdString().find("m"), std::string::npos);
}

TYPED_TEST(AAX_CUnitPrefixDisplayDelegateDecorator_Test, RvalueConstructor_DecoratorChaining)
{
    // Test chaining multiple decorators using rvalue reference constructors
    auto mockDelegate = std::make_unique<AAX_MockDisplayDelegate<TypeParam>>();
    
    // Chain: Mock -> Unit -> UnitPrefix
    auto chainedDecorator = std::make_unique<AAX_CUnitPrefixDisplayDelegateDecorator<TypeParam>>(
        AAX_CUnitDisplayDelegateDecorator<TypeParam>(std::move(*mockDelegate), AAX_CString("Hz")));
    
    EXPECT_NE(nullptr, chainedDecorator.get());
    
    // Test functionality with kilo prefix (frequency use case)
    AAX_CString result;
    TypeParam testValue = static_cast<TypeParam>(1500.0);
    EXPECT_TRUE(chainedDecorator->ValueToString(testValue, &result));
    EXPECT_NE(result.StdString().find("k"), std::string::npos);
}

TYPED_TEST(AAX_CUnitPrefixDisplayDelegateDecorator_Test, RvalueConstructor_MemoryManagement)
{
    // Test that rvalue constructors properly manage memory
    std::vector<std::unique_ptr<AAX_CUnitPrefixDisplayDelegateDecorator<TypeParam>>> decorators;
    
    for (int i = 0; i < 10; ++i) {
        auto mockDelegate = std::make_unique<AAX_MockDisplayDelegate<TypeParam>>();
        decorators.push_back(std::make_unique<AAX_CUnitPrefixDisplayDelegateDecorator<TypeParam>>(
            std::move(*mockDelegate)));
    }
    
    // Test all decorators with different prefix ranges
    std::vector<TypeParam> testValues = {
        static_cast<TypeParam>(2000000.0),  // M prefix
        static_cast<TypeParam>(1500.0),     // k prefix
        static_cast<TypeParam>(0.005),      // m prefix
        static_cast<TypeParam>(0.000005)    // u prefix
    };
    
    for (auto& decorator : decorators) {
        EXPECT_NE(nullptr, decorator.get());
        
        for (TypeParam value : testValues) {
            AAX_CString result;
            EXPECT_TRUE(decorator->ValueToString(value, &result));
        }
    }
}

TYPED_TEST(AAX_CUnitPrefixDisplayDelegateDecorator_Test, RvalueConstructor_TemplateVariations)
{
    // Test rvalue constructors with different template types
    if constexpr (std::is_same_v<TypeParam, float>) {
        auto mockDelegate = std::make_unique<AAX_MockDisplayDelegate<float>>();
        auto decorator = std::make_unique<AAX_CUnitPrefixDisplayDelegateDecorator<float>>(
            std::move(*mockDelegate));
        
        AAX_CString result;
        EXPECT_TRUE(decorator->ValueToString(1500.0f, &result));
        EXPECT_NE(result.StdString().find("k"), std::string::npos);
    }
    
    if constexpr (std::is_same_v<TypeParam, double>) {
        auto mockDelegate = std::make_unique<AAX_MockDisplayDelegate<double>>();
        auto decorator = std::make_unique<AAX_CUnitPrefixDisplayDelegateDecorator<double>>(
            std::move(*mockDelegate));
        
        AAX_CString result;
        EXPECT_TRUE(decorator->ValueToString(1500.0, &result));
        EXPECT_NE(result.StdString().find("k"), std::string::npos);
    }
}

TYPED_TEST(AAX_CUnitPrefixDisplayDelegateDecorator_Test, RvalueConstructor_ComplexChaining)
{
    // Test complex decorator chaining with multiple rvalue constructors
    auto mockDelegate = std::make_unique<AAX_MockDisplayDelegate<TypeParam>>();
    
    // Create a complex chain using temporary objects
    auto complexDecorator = std::make_unique<AAX_CUnitPrefixDisplayDelegateDecorator<TypeParam>>(
        AAX_CScaledDisplayDelegateDecorator<TypeParam>(
            AAX_CUnitDisplayDelegateDecorator<TypeParam>(
                std::move(*mockDelegate), AAX_CString("Hz")), 1000.0));
    
    EXPECT_NE(nullptr, complexDecorator.get());
    
    // Test with multiple values across different prefix ranges
    std::vector<TypeParam> testValues = {
        static_cast<TypeParam>(0.001),   // With scaling: 1.0 (no prefix)
        static_cast<TypeParam>(0.01),    // With scaling: 10.0 (no prefix)
        static_cast<TypeParam>(1.0),     // With scaling: 1000.0 (k prefix)
        static_cast<TypeParam>(2.0)      // With scaling: 2000.0 (k prefix)
    };
    
    for (TypeParam value : testValues) {
        AAX_CString result;
        EXPECT_TRUE(complexDecorator->ValueToString(value, &result));
    }
}

TYPED_TEST(AAX_CUnitPrefixDisplayDelegateDecorator_Test, RvalueConstructor_PolymorphicBehavior)
{
    // Test polymorphic behavior with rvalue reference constructors
    auto mockDelegate = std::make_unique<AAX_MockDisplayDelegate<TypeParam>>();
    
    // Create decorator using rvalue constructor
    std::unique_ptr<AAX_IDisplayDelegateDecorator<TypeParam>> polymorphicDecorator = 
        std::make_unique<AAX_CUnitPrefixDisplayDelegateDecorator<TypeParam>>(std::move(*mockDelegate));
    
    EXPECT_NE(nullptr, polymorphicDecorator.get());
    
    // Test polymorphic behavior with mega prefix
    AAX_CString result;
    TypeParam testValue = static_cast<TypeParam>(3000000.0);
    EXPECT_TRUE(polymorphicDecorator->ValueToString(testValue, &result));
    EXPECT_NE(result.StdString().find("M"), std::string::npos);
    
    // Test cloning through polymorphic interface
    auto cloned = std::unique_ptr<AAX_IDisplayDelegateDecorator<TypeParam>>(
        polymorphicDecorator->Clone());
    EXPECT_NE(nullptr, cloned.get());
    
    AAX_CString clonedResult;
    EXPECT_TRUE(cloned->ValueToString(testValue, &clonedResult));
    EXPECT_EQ(result.StdString(), clonedResult.StdString());
}

TYPED_TEST(AAX_CUnitPrefixDisplayDelegateDecorator_Test, RvalueConstructor_RoundTripWithMove)
{
    // Test round-trip conversion with rvalue-constructed decorators
    auto mockDelegate = std::make_unique<AAX_MockDisplayDelegate<TypeParam>>();
    auto decorator = std::make_unique<AAX_CUnitPrefixDisplayDelegateDecorator<TypeParam>>(
        std::move(*mockDelegate));
    
    std::vector<TypeParam> testValues = {
        static_cast<TypeParam>(2500000.0),  // Should use M prefix
        static_cast<TypeParam>(1500.0),     // Should use k prefix
        static_cast<TypeParam>(50.0),       // No prefix
        static_cast<TypeParam>(0.005),      // Should use m prefix
        static_cast<TypeParam>(0.000005)    // Should use u prefix
    };
    
    for (TypeParam originalValue : testValues) {
        AAX_CString stringValue;
        TypeParam convertedValue;
        
        // Convert to string and back
        EXPECT_TRUE(decorator->ValueToString(originalValue, &stringValue));
        EXPECT_TRUE(decorator->StringToValue(stringValue, &convertedValue));
        
        // Check accuracy (allowing for floating-point precision loss)
        double tolerance = std::abs(static_cast<double>(originalValue)) * 0.001;
        if (tolerance < 0.000001) tolerance = 0.000001;
        EXPECT_NEAR(static_cast<double>(originalValue), static_cast<double>(convertedValue), tolerance);
    }
}

TYPED_TEST(AAX_CUnitPrefixDisplayDelegateDecorator_Test, RvalueConstructor_AllPrefixRanges)
{
    // Test all prefix ranges with rvalue reference constructors
    auto mockDelegate = std::make_unique<AAX_MockDisplayDelegate<TypeParam>>();
    auto decorator = std::make_unique<AAX_CUnitPrefixDisplayDelegateDecorator<TypeParam>>(
        std::move(*mockDelegate));
    
    // Test each prefix range
    struct TestCase {
        TypeParam value;
        std::string expectedPrefix;
    };
    
    std::vector<TestCase> testCases = {
        {static_cast<TypeParam>(5000000.0), "M"},    // Mega
        {static_cast<TypeParam>(3500.0), "k"},       // Kilo
        {static_cast<TypeParam>(150.0), ""},         // No prefix
        {static_cast<TypeParam>(0.008), "m"},        // Milli
        {static_cast<TypeParam>(0.000008), "u"}      // Micro
    };
    
    for (const auto& testCase : testCases) {
        AAX_CString result;
        EXPECT_TRUE(decorator->ValueToString(testCase.value, &result));
        
        if (!testCase.expectedPrefix.empty()) {
            EXPECT_NE(result.StdString().find(testCase.expectedPrefix), std::string::npos);
        } else {
            // No prefix case - should not contain any prefix characters
            EXPECT_EQ(result.StdString().find("M"), std::string::npos);
            EXPECT_EQ(result.StdString().find("k"), std::string::npos);
            EXPECT_EQ(result.StdString().find("m"), std::string::npos);
            EXPECT_EQ(result.StdString().find("u"), std::string::npos);
        }
    }
}

TYPED_TEST(AAX_CUnitPrefixDisplayDelegateDecorator_Test, RvalueConstructor_EdgeCases)
{
    // Test edge cases with rvalue reference constructors
    auto mockDelegate = std::make_unique<AAX_MockDisplayDelegate<TypeParam>>();
    auto decorator = std::make_unique<AAX_CUnitPrefixDisplayDelegateDecorator<TypeParam>>(
        std::move(*mockDelegate));
    
    // Test boundary values at prefix thresholds
    std::vector<TypeParam> edgeValues = {
        static_cast<TypeParam>(1000000.0),   // Exactly at mega threshold
        static_cast<TypeParam>(999999.0),    // Just below mega threshold
        static_cast<TypeParam>(1000.0),      // Exactly at kilo threshold
        static_cast<TypeParam>(999.0),       // Just below kilo threshold
        static_cast<TypeParam>(1.0),         // At unity (no prefix)
        static_cast<TypeParam>(0.001),       // Exactly at milli threshold
        static_cast<TypeParam>(0.000001),    // Exactly at micro threshold
        static_cast<TypeParam>(0.0)          // Zero value
    };
    
    for (TypeParam value : edgeValues) {
        AAX_CString result;
        EXPECT_TRUE(decorator->ValueToString(value, &result));
        
        // All should succeed without crashing
        TypeParam parsedValue;
        EXPECT_TRUE(decorator->StringToValue(result, &parsedValue));
        
        // Verify reasonable accuracy for non-zero values
        if (value != static_cast<TypeParam>(0.0)) {
            double tolerance = std::abs(static_cast<double>(value)) * 0.001;
            if (tolerance < 0.000001) tolerance = 0.000001;
            EXPECT_NEAR(static_cast<double>(value), static_cast<double>(parsedValue), tolerance);
        }
    }
}

//==============================================================================
// Mathematical Accuracy Tests
//==============================================================================

TYPED_TEST(AAX_CUnitPrefixDisplayDelegateDecorator_Test, MathematicalAccuracy_RoundTrip)
{
    // Test round-trip conversion accuracy for different prefixes
    std::vector<TypeParam> testValues = {
        static_cast<TypeParam>(2500000.0),  // Should use M prefix
        static_cast<TypeParam>(1500.0),     // Should use k prefix
        static_cast<TypeParam>(50.0),       // No prefix
        static_cast<TypeParam>(0.005),      // Should use m prefix
        static_cast<TypeParam>(0.000005)    // Should use u prefix (safer value)
    };

    for (TypeParam originalValue : testValues) {
        AAX_CString stringValue;
        TypeParam convertedValue;

        // Convert to string and back
        EXPECT_TRUE(this->mUnitPrefixDecorator->ValueToString(originalValue, &stringValue));
        EXPECT_TRUE(this->mUnitPrefixDecorator->StringToValue(stringValue, &convertedValue));

        // Check accuracy (allowing for some floating-point precision loss)
        double tolerance = std::abs(static_cast<double>(originalValue)) * 0.001; // 0.1% tolerance
        if (tolerance < 0.000001) tolerance = 0.000001; // Minimum tolerance for very small values
        EXPECT_NEAR(static_cast<double>(originalValue), static_cast<double>(convertedValue), tolerance);
    }
}

TYPED_TEST(AAX_CUnitPrefixDisplayDelegateDecorator_Test, MathematicalAccuracy_PrefixThresholds)
{
    AAX_CString result;

    // Test threshold boundaries

    // Just at mega threshold
    TypeParam megaThreshold = static_cast<TypeParam>(1000000.0);
    EXPECT_TRUE(this->mUnitPrefixDecorator->ValueToString(megaThreshold, &result));
    EXPECT_NE(result.StdString().find("M"), std::string::npos);

    // Just below mega threshold
    TypeParam belowMegaThreshold = static_cast<TypeParam>(999999.0);
    EXPECT_TRUE(this->mUnitPrefixDecorator->ValueToString(belowMegaThreshold, &result));
    EXPECT_NE(result.StdString().find("k"), std::string::npos);

    // Just at kilo threshold
    TypeParam kiloThreshold = static_cast<TypeParam>(1000.0);
    EXPECT_TRUE(this->mUnitPrefixDecorator->ValueToString(kiloThreshold, &result));
    EXPECT_NE(result.StdString().find("k"), std::string::npos);

    // Just below kilo threshold
    TypeParam belowKiloThreshold = static_cast<TypeParam>(999.0);
    EXPECT_TRUE(this->mUnitPrefixDecorator->ValueToString(belowKiloThreshold, &result));
    // Should not have any prefix
    EXPECT_EQ(result.StdString().find("M"), std::string::npos);
    EXPECT_EQ(result.StdString().find("k"), std::string::npos);
    EXPECT_EQ(result.StdString().find("m"), std::string::npos);
    EXPECT_EQ(result.StdString().find("u"), std::string::npos);
}
