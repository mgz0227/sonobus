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
 *	\file AAX_DisplayDelegateDecorator_Wrapping_UnitTest.cpp
 *
 *	\brief Unit tests for display delegate decorator wrapping behavior using the decorator pattern
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX_CNumberDisplayDelegate.h"
#include "AAX_CUnitDisplayDelegateDecorator.h"
#include "AAX_CUnitPrefixDisplayDelegateDecorator.h"
#include "AAX_CPercentDisplayDelegateDecorator.h"
#include "AAX_CDecibelDisplayDelegateDecorator.h"
#include "AAX_CString.h"
#include <memory>
#include <cmath>
#include <iostream>

//==============================================================================
// Test Fixture
//==============================================================================

class AAX_DisplayDelegateDecorator_Wrapping_Test : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Base display delegate with 1 decimal precision and space after number
        mBaseDelegate = std::make_unique<AAX_CNumberDisplayDelegate<double, 1, 1>>();
    }
    
    void TearDown() override
    {
        mBaseDelegate.reset();
    }
    
    std::unique_ptr<AAX_CNumberDisplayDelegate<double, 1, 1>> mBaseDelegate;
};

//==============================================================================
// Single Decorator Tests
//==============================================================================

TEST_F(AAX_DisplayDelegateDecorator_Wrapping_Test, SingleDecorator_UnitDecorator)
{
    // Test: Base -> Unit("s")
    // Expected: 1.5 -> "1.5 s"
    AAX_CUnitDisplayDelegateDecorator<double> unitDecorator(*mBaseDelegate, "s");

    AAX_CString result;
    EXPECT_TRUE(unitDecorator.ValueToString(1.5, &result));

    // Verify presence and order: number should come before unit
    std::string resultStr = result.StdString();
    size_t numberPos = resultStr.find("1.5");
    size_t unitPos = resultStr.find("s");

    EXPECT_NE(numberPos, std::string::npos);
    EXPECT_NE(unitPos, std::string::npos);
    EXPECT_LT(numberPos, unitPos); // Number should come before unit

    // Test round-trip conversion
    double value;
    EXPECT_TRUE(unitDecorator.StringToValue(result, &value));
    EXPECT_NEAR(value, 1.5, 0.001);
}

TEST_F(AAX_DisplayDelegateDecorator_Wrapping_Test, SingleDecorator_UnitPrefixDecorator)
{
    // Test: Base -> UnitPrefix
    // Expected: 1500.0 -> "1.5k"
    AAX_CUnitPrefixDisplayDelegateDecorator<double> prefixDecorator(*mBaseDelegate);

    AAX_CString result;
    EXPECT_TRUE(prefixDecorator.ValueToString(1500.0, &result));

    // Verify presence and order: number should come before prefix
    std::string resultStr = result.StdString();
    size_t numberPos = resultStr.find("1.5");
    size_t prefixPos = resultStr.find("k");

    EXPECT_NE(numberPos, std::string::npos);
    EXPECT_NE(prefixPos, std::string::npos);
    EXPECT_LT(numberPos, prefixPos); // Number should come before prefix

    // Test round-trip conversion
    double value;
    EXPECT_TRUE(prefixDecorator.StringToValue(result, &value));
    EXPECT_NEAR(value, 1500.0, 1.0);
}

TEST_F(AAX_DisplayDelegateDecorator_Wrapping_Test, SingleDecorator_PercentDecorator)
{
    // Test: Base -> Percent
    // Expected: 0.5 -> "50%"
    AAX_CPercentDisplayDelegateDecorator<double> percentDecorator(*mBaseDelegate);

    AAX_CString result;
    EXPECT_TRUE(percentDecorator.ValueToString(0.5, &result));

    // Verify presence and order: number should come before percent symbol
    std::string resultStr = result.StdString();
    size_t numberPos = resultStr.find("50");
    size_t percentPos = resultStr.find("%");

    EXPECT_NE(numberPos, std::string::npos);
    EXPECT_NE(percentPos, std::string::npos);
    EXPECT_LT(numberPos, percentPos); // Number should come before percent symbol

    // Test round-trip conversion
    double value;
    EXPECT_TRUE(percentDecorator.StringToValue(result, &value));
    EXPECT_NEAR(value, 0.5, 0.001);
}

TEST_F(AAX_DisplayDelegateDecorator_Wrapping_Test, SingleDecorator_DecibelDecorator)
{
    // Test: Base -> Decibel
    // Expected: 1.0 -> "0.0 dB" (approximately)
    AAX_CDecibelDisplayDelegateDecorator<double> decibelDecorator(*mBaseDelegate);

    AAX_CString result;
    EXPECT_TRUE(decibelDecorator.ValueToString(1.0, &result));

    // Verify presence and order: number should come before "dB"
    std::string resultStr = result.StdString();
    size_t numberPos = resultStr.find("0.0");
    size_t dbPos = resultStr.find("dB");

    EXPECT_NE(numberPos, std::string::npos);
    EXPECT_NE(dbPos, std::string::npos);
    EXPECT_LT(numberPos, dbPos); // Number should come before "dB"

    // Test round-trip conversion
    double value;
    EXPECT_TRUE(decibelDecorator.StringToValue(result, &value));
    EXPECT_NEAR(value, 1.0, 0.1);
}

//==============================================================================
// Two-Level Decorator Tests
//==============================================================================

TEST_F(AAX_DisplayDelegateDecorator_Wrapping_Test, TwoLevel_UnitPrefix_Then_Unit)
{
    // Test: Base -> UnitPrefix -> Unit("Hz")
    // Expected: 1500.0 -> "1.5kHz"
    AAX_CUnitPrefixDisplayDelegateDecorator<double> prefixDecorator(*mBaseDelegate);
    AAX_CUnitDisplayDelegateDecorator<double> unitDecorator(prefixDecorator, "Hz");

    AAX_CString result;
    EXPECT_TRUE(unitDecorator.ValueToString(1500.0, &result));

    // Verify presence and order: number -> prefix -> unit
    std::string resultStr = result.StdString();
    size_t numberPos = resultStr.find("1.5");
    size_t prefixPos = resultStr.find("k");
    size_t unitPos = resultStr.find("Hz");

    EXPECT_NE(numberPos, std::string::npos);
    EXPECT_NE(prefixPos, std::string::npos);
    EXPECT_NE(unitPos, std::string::npos);
    EXPECT_LT(numberPos, prefixPos); // Number before prefix
    EXPECT_LT(prefixPos, unitPos);   // Prefix before unit

    // Test round-trip conversion
    double value;
    EXPECT_TRUE(unitDecorator.StringToValue(result, &value));
    EXPECT_NEAR(value, 1500.0, 1.0);
}

TEST_F(AAX_DisplayDelegateDecorator_Wrapping_Test, TwoLevel_Percent_Then_Unit)
{
    // Test: Base -> Percent -> Unit("efficiency")
    // Expected: 0.75 -> "75.0 %efficiency"
    AAX_CPercentDisplayDelegateDecorator<double> percentDecorator(*mBaseDelegate);
    AAX_CUnitDisplayDelegateDecorator<double> unitDecorator(percentDecorator, "efficiency");

    AAX_CString result;
    EXPECT_TRUE(unitDecorator.ValueToString(0.75, &result));

    // Verify presence and order: number -> % -> unit
    std::string resultStr = result.StdString();
    size_t numberPos = resultStr.find("75");
    size_t percentPos = resultStr.find("%");
    size_t unitPos = resultStr.find("efficiency");

    EXPECT_NE(numberPos, std::string::npos);
    EXPECT_NE(percentPos, std::string::npos);
    EXPECT_NE(unitPos, std::string::npos);
    EXPECT_LT(numberPos, percentPos); // Number before percent
    EXPECT_LT(percentPos, unitPos);   // Percent before unit

    // Test round-trip conversion
    double value;
    EXPECT_TRUE(unitDecorator.StringToValue(result, &value));
    EXPECT_NEAR(value, 0.75, 0.001);
}

TEST_F(AAX_DisplayDelegateDecorator_Wrapping_Test, TwoLevel_Decibel_Then_Unit)
{
    // Test: Base -> Decibel -> Unit("SPL")
    // Expected: 2.0 -> "6.0 dBSPL" (approximately)
    AAX_CDecibelDisplayDelegateDecorator<double> decibelDecorator(*mBaseDelegate);
    AAX_CUnitDisplayDelegateDecorator<double> unitDecorator(decibelDecorator, "SPL");

    AAX_CString result;
    EXPECT_TRUE(unitDecorator.ValueToString(2.0, &result));

    // Verify presence and order: number -> dB -> unit
    std::string resultStr = result.StdString();
    size_t numberPos = resultStr.find("6.0");
    size_t dbPos = resultStr.find("dB");
    size_t unitPos = resultStr.find("SPL");

    EXPECT_NE(numberPos, std::string::npos);
    EXPECT_NE(dbPos, std::string::npos);
    EXPECT_NE(unitPos, std::string::npos);
    EXPECT_LT(numberPos, dbPos);   // Number before dB
    EXPECT_LT(dbPos, unitPos);     // dB before unit

    // Test round-trip conversion
    double value;
    EXPECT_TRUE(unitDecorator.StringToValue(result, &value));
    EXPECT_NEAR(value, 2.0, 0.1);
}

TEST_F(AAX_DisplayDelegateDecorator_Wrapping_Test, TwoLevel_Unit_Then_UnitPrefix)
{
    // Test: Base -> Unit("V") -> UnitPrefix
    // Expected: 0.001 -> "1.0 mV"
    AAX_CUnitDisplayDelegateDecorator<double> unitDecorator(*mBaseDelegate, "V");
    AAX_CUnitPrefixDisplayDelegateDecorator<double> prefixDecorator(unitDecorator);

    AAX_CString result;
    EXPECT_TRUE(prefixDecorator.ValueToString(0.001, &result));

    // Verify presence and order: when Unit is applied first, then UnitPrefix
    // Expected result: "1.0 Vm" (number -> unit -> prefix)
    std::string resultStr = result.StdString();
    size_t numberPos = resultStr.find("1.0");
    size_t prefixPos = resultStr.find("m");
    size_t unitPos = resultStr.find("V");

    EXPECT_NE(numberPos, std::string::npos);
    EXPECT_NE(prefixPos, std::string::npos);
    EXPECT_NE(unitPos, std::string::npos);

    // Verify order: number -> unit -> prefix (e.g., "1.0 Vm")
    // This is expected behavior when Unit decorator is applied before UnitPrefix
    EXPECT_LT(numberPos, unitPos);   // Number before unit
    EXPECT_LT(unitPos, prefixPos);   // Unit before prefix

    // Test round-trip conversion
    double value;
    EXPECT_TRUE(prefixDecorator.StringToValue(result, &value));
    EXPECT_NEAR(value, 0.001, 0.000001);
}

//==============================================================================
// Three-Level Decorator Tests
//==============================================================================

TEST_F(AAX_DisplayDelegateDecorator_Wrapping_Test, ThreeLevel_UnitPrefix_Unit_Percent)
{
    // Test: Base -> UnitPrefix -> Unit("Hz") -> Percent
    // This is a complex case that might not make practical sense but tests the pattern
    AAX_CUnitPrefixDisplayDelegateDecorator<double> prefixDecorator(*mBaseDelegate);
    AAX_CUnitDisplayDelegateDecorator<double> unitDecorator(prefixDecorator, "Hz");
    AAX_CPercentDisplayDelegateDecorator<double> percentDecorator(unitDecorator);
    
    AAX_CString result;
    EXPECT_TRUE(percentDecorator.ValueToString(0.015, &result)); // 0.015 -> 1.5% -> 15mHz%
    
    // The result should contain all three transformations
    EXPECT_NE(result.StdString().find("%"), std::string::npos);
    // Note: The exact format depends on the order of operations
    
    // Test round-trip conversion
    double value;
    EXPECT_TRUE(percentDecorator.StringToValue(result, &value));
    EXPECT_NEAR(value, 0.015, 0.001);
}

//==============================================================================
// Complex Multi-Level Decorator Tests
//==============================================================================

TEST_F(AAX_DisplayDelegateDecorator_Wrapping_Test, FourLevel_Complex_Wrapping)
{
    // Test: Base -> Percent -> UnitPrefix -> Unit("efficiency") -> Decibel
    // This tests a very complex wrapping scenario
    AAX_CPercentDisplayDelegateDecorator<double> percentDecorator(*mBaseDelegate);
    AAX_CUnitPrefixDisplayDelegateDecorator<double> prefixDecorator(percentDecorator);
    AAX_CUnitDisplayDelegateDecorator<double> unitDecorator(prefixDecorator, "efficiency");
    AAX_CDecibelDisplayDelegateDecorator<double> decibelDecorator(unitDecorator);

    AAX_CString result;
    EXPECT_TRUE(decibelDecorator.ValueToString(0.5, &result));

    // The result should be a complex transformation
    // Note: This may not be practically useful but tests the decorator pattern limits

    // Test round-trip conversion
    double value;
    EXPECT_TRUE(decibelDecorator.StringToValue(result, &value));
    EXPECT_NEAR(value, 0.5, 0.1); // Allow larger tolerance for complex transformations
}

//==============================================================================
// Cloning Tests with Wrapped Decorators
//==============================================================================

TEST_F(AAX_DisplayDelegateDecorator_Wrapping_Test, Clone_SingleWrappedDecorator)
{
    // Test cloning of wrapped decorators
    AAX_CUnitPrefixDisplayDelegateDecorator<double> prefixDecorator(*mBaseDelegate);
    AAX_CUnitDisplayDelegateDecorator<double> unitDecorator(prefixDecorator, "Hz");

    // Clone the wrapped decorator
    auto cloned = std::unique_ptr<AAX_CUnitDisplayDelegateDecorator<double>>(unitDecorator.Clone());

    EXPECT_NE(nullptr, cloned.get());
    EXPECT_NE(&unitDecorator, cloned.get());

    // Test that both original and cloned behave the same
    AAX_CString originalResult, clonedResult;
    double testValue = 2500.0;

    EXPECT_TRUE(unitDecorator.ValueToString(testValue, &originalResult));
    EXPECT_TRUE(cloned->ValueToString(testValue, &clonedResult));

    EXPECT_EQ(originalResult.StdString(), clonedResult.StdString());
}

TEST_F(AAX_DisplayDelegateDecorator_Wrapping_Test, Clone_MultipleWrappedDecorators)
{
    // Test cloning of multiple wrapped decorators
    AAX_CPercentDisplayDelegateDecorator<double> percentDecorator(*mBaseDelegate);
    AAX_CUnitDisplayDelegateDecorator<double> unitDecorator(percentDecorator, "efficiency");

    // Clone the wrapped decorator
    auto cloned = std::unique_ptr<AAX_CUnitDisplayDelegateDecorator<double>>(unitDecorator.Clone());

    EXPECT_NE(nullptr, cloned.get());

    // Test that both original and cloned behave the same
    AAX_CString originalResult, clonedResult;
    double testValue = 0.85;

    EXPECT_TRUE(unitDecorator.ValueToString(testValue, &originalResult));
    EXPECT_TRUE(cloned->ValueToString(testValue, &clonedResult));

    EXPECT_EQ(originalResult.StdString(), clonedResult.StdString());
}

//==============================================================================
// Character Limit Tests with Wrapped Decorators
//==============================================================================

TEST_F(AAX_DisplayDelegateDecorator_Wrapping_Test, CharacterLimit_WrappedDecorators)
{
    // Test character limits with wrapped decorators
    AAX_CUnitPrefixDisplayDelegateDecorator<double> prefixDecorator(*mBaseDelegate);
    AAX_CUnitDisplayDelegateDecorator<double> unitDecorator(prefixDecorator, "Hz");

    AAX_CString result;
    double testValue = 1500.0; // Should be "1.5kHz"

    // Test with sufficient character limit
    EXPECT_TRUE(unitDecorator.ValueToString(testValue, 10, &result));
    EXPECT_LE(result.Length(), 10);
    EXPECT_NE(result.StdString().find("k"), std::string::npos);
    EXPECT_NE(result.StdString().find("Hz"), std::string::npos);

    // Test with very limited character space
    EXPECT_TRUE(unitDecorator.ValueToString(testValue, 5, &result));
    EXPECT_LE(result.Length(), 5);
    // Should still contain essential information, though possibly truncated
}

//==============================================================================
// Edge Cases and Error Handling
//==============================================================================

TEST_F(AAX_DisplayDelegateDecorator_Wrapping_Test, EdgeCase_ZeroValue_WrappedDecorators)
{
    // Test zero values with wrapped decorators
    AAX_CUnitPrefixDisplayDelegateDecorator<double> prefixDecorator(*mBaseDelegate);
    AAX_CUnitDisplayDelegateDecorator<double> unitDecorator(prefixDecorator, "V");

    AAX_CString result;
    EXPECT_TRUE(unitDecorator.ValueToString(0.0, &result));
    EXPECT_NE(result.StdString().find("V"), std::string::npos);

    // Test round-trip conversion
    double value;
    EXPECT_TRUE(unitDecorator.StringToValue(result, &value));
    EXPECT_NEAR(value, 0.0, 0.001);
}

TEST_F(AAX_DisplayDelegateDecorator_Wrapping_Test, EdgeCase_NegativeValue_WrappedDecorators)
{
    // Test negative values with wrapped decorators
    AAX_CPercentDisplayDelegateDecorator<double> percentDecorator(*mBaseDelegate);
    AAX_CUnitDisplayDelegateDecorator<double> unitDecorator(percentDecorator, "change");

    AAX_CString result;
    EXPECT_TRUE(unitDecorator.ValueToString(-0.25, &result));
    EXPECT_NE(result.StdString().find("%"), std::string::npos);
    EXPECT_NE(result.StdString().find("change"), std::string::npos);
    EXPECT_NE(result.StdString().find("-"), std::string::npos);

    // Test round-trip conversion
    double value;
    EXPECT_TRUE(unitDecorator.StringToValue(result, &value));
    EXPECT_NEAR(value, -0.25, 0.001);
}

TEST_F(AAX_DisplayDelegateDecorator_Wrapping_Test, EdgeCase_VeryLargeValue_WrappedDecorators)
{
    // Test very large values with wrapped decorators
    AAX_CUnitPrefixDisplayDelegateDecorator<double> prefixDecorator(*mBaseDelegate);
    AAX_CUnitDisplayDelegateDecorator<double> unitDecorator(prefixDecorator, "Hz");

    AAX_CString result;
    double largeValue = 2500000.0; // Should be "2.5MHz"
    EXPECT_TRUE(unitDecorator.ValueToString(largeValue, &result));

    // Verify presence and order: number -> prefix -> unit
    std::string resultStr = result.StdString();
    size_t numberPos = resultStr.find("2.5");
    size_t prefixPos = resultStr.find("M");
    size_t unitPos = resultStr.find("Hz");

    EXPECT_NE(numberPos, std::string::npos);
    EXPECT_NE(prefixPos, std::string::npos);
    EXPECT_NE(unitPos, std::string::npos);
    EXPECT_LT(numberPos, prefixPos); // Number before prefix
    EXPECT_LT(prefixPos, unitPos);   // Prefix before unit

    // Test round-trip conversion
    double value;
    EXPECT_TRUE(unitDecorator.StringToValue(result, &value));
    EXPECT_NEAR(value, largeValue, 1000.0); // Allow some tolerance for large values
}

TEST_F(AAX_DisplayDelegateDecorator_Wrapping_Test, EdgeCase_VerySmallValue_WrappedDecorators)
{
    // Test very small values with wrapped decorators
    AAX_CUnitPrefixDisplayDelegateDecorator<double> prefixDecorator(*mBaseDelegate);
    AAX_CUnitDisplayDelegateDecorator<double> unitDecorator(prefixDecorator, "V");

    AAX_CString result;
    double smallValue = 0.000005; // Should be "5.0 uV"
    EXPECT_TRUE(unitDecorator.ValueToString(smallValue, &result));

    // Verify presence and order: number -> prefix -> unit
    std::string resultStr = result.StdString();
    size_t numberPos = resultStr.find("5.0");
    size_t prefixPos = resultStr.find("u");
    size_t unitPos = resultStr.find("V");

    EXPECT_NE(numberPos, std::string::npos);
    EXPECT_NE(prefixPos, std::string::npos);
    EXPECT_NE(unitPos, std::string::npos);
    EXPECT_LT(numberPos, prefixPos); // Number before prefix
    EXPECT_LT(prefixPos, unitPos);   // Prefix before unit

    // Test round-trip conversion
    double value;
    EXPECT_TRUE(unitDecorator.StringToValue(result, &value));
    EXPECT_NEAR(value, smallValue, 0.000001);
}

//==============================================================================
// Real-World Practical Examples
//==============================================================================

TEST_F(AAX_DisplayDelegateDecorator_Wrapping_Test, RealWorld_FrequencyDisplay)
{
    // Real-world example: Frequency display with unit prefix and Hz unit
    // Base -> UnitPrefix -> Unit("Hz")
    // Examples: 440.0 -> "440 Hz", 1000.0 -> "1kHz", 44100.0 -> "44.1kHz"
    AAX_CUnitPrefixDisplayDelegateDecorator<double> prefixDecorator(*mBaseDelegate);
    AAX_CUnitDisplayDelegateDecorator<double> freqDecorator(prefixDecorator, "Hz");

    // Test various frequency values
    struct FrequencyTest {
        double input;
        std::string expectedContains[2];
    };

    FrequencyTest tests[] = {
        {440.0, {"440", "Hz"}},      // A4 note
        {1000.0, {"1", "kHz"}},      // 1 kHz
        {44100.0, {"44", "kHz"}},    // CD sample rate
        {0.001, {"1", "mHz"}}        // Very low frequency
    };

    for (const auto& test : tests) {
        AAX_CString result;
        EXPECT_TRUE(freqDecorator.ValueToString(test.input, &result));

        std::string resultStr = result.StdString();

        // Verify presence of expected strings
        for (const auto& expected : test.expectedContains) {
            EXPECT_NE(resultStr.find(expected), std::string::npos)
                << "Expected '" << expected << "' in result '" << result.CString()
                << "' for input " << test.input;
        }

        // Verify order: number should come before unit/prefix, and prefix should come before "Hz"
        size_t hzPos = resultStr.find("Hz");
        EXPECT_NE(hzPos, std::string::npos);

        // Check if there's a prefix (k, m, M)
        size_t prefixPos = std::string::npos;
        if (resultStr.find("k") != std::string::npos) prefixPos = resultStr.find("k");
        else if (resultStr.find("m") != std::string::npos) prefixPos = resultStr.find("m");
        else if (resultStr.find("M") != std::string::npos) prefixPos = resultStr.find("M");

        if (prefixPos != std::string::npos) {
            EXPECT_LT(prefixPos, hzPos) << "Prefix should come before 'Hz' in result '"
                                        << result.CString() << "'";
        }

        // Test round-trip
        double value;
        EXPECT_TRUE(freqDecorator.StringToValue(result, &value));
        EXPECT_NEAR(value, test.input, std::abs(test.input) * 0.01 + 0.001);
    }
}

TEST_F(AAX_DisplayDelegateDecorator_Wrapping_Test, RealWorld_VoltageDisplay)
{
    // Real-world example: Voltage display with unit prefix and V unit
    // Base -> UnitPrefix -> Unit("V")
    AAX_CUnitPrefixDisplayDelegateDecorator<double> prefixDecorator(*mBaseDelegate);
    AAX_CUnitDisplayDelegateDecorator<double> voltageDecorator(prefixDecorator, "V");

    // Test various voltage values
    struct VoltageTest {
        double input;
        std::string expectedPrefix;
    };

    VoltageTest tests[] = {
        {5.0, ""},          // 5V (no prefix)
        {0.001, "m"},       // 1mV
        {0.000005, "u"},    // 5uV (use larger value to avoid rounding to 0.0)
        {1000.0, "k"},      // 1kV
        {1000000.0, "M"}    // 1MV
    };

    for (const auto& test : tests) {
        AAX_CString result;
        EXPECT_TRUE(voltageDecorator.ValueToString(test.input, &result));

        std::string resultStr = result.StdString();

        // Verify presence of "V"
        size_t vPos = resultStr.find("V");
        EXPECT_NE(vPos, std::string::npos);

        if (!test.expectedPrefix.empty()) {
            // Verify presence and order: prefix should come before "V"
            size_t prefixPos = resultStr.find(test.expectedPrefix);
            EXPECT_NE(prefixPos, std::string::npos);
            EXPECT_LT(prefixPos, vPos) << "Prefix '" << test.expectedPrefix
                                       << "' should come before 'V' in result '"
                                       << result.CString() << "'";
        } else {
            // For no prefix case, ensure no prefix characters are present
            EXPECT_EQ(resultStr.find("m"), std::string::npos);
            EXPECT_EQ(resultStr.find("u"), std::string::npos);
            EXPECT_EQ(resultStr.find("k"), std::string::npos);
            EXPECT_EQ(resultStr.find("M"), std::string::npos);
        }

        // Test round-trip
        double value;
        EXPECT_TRUE(voltageDecorator.StringToValue(result, &value));
        EXPECT_NEAR(value, test.input, std::abs(test.input) * 0.01 + 0.000001);
    }
}

TEST_F(AAX_DisplayDelegateDecorator_Wrapping_Test, RealWorld_PercentageEfficiency)
{
    // Real-world example: Efficiency percentage with unit
    // Base -> Percent -> Unit("efficiency")
    AAX_CPercentDisplayDelegateDecorator<double> percentDecorator(*mBaseDelegate);
    AAX_CUnitDisplayDelegateDecorator<double> efficiencyDecorator(percentDecorator, "efficiency");

    // Test various efficiency values
    double efficiencyValues[] = {0.95, 0.80, 0.50, 0.25};

    for (double efficiency : efficiencyValues) {
        AAX_CString result;
        EXPECT_TRUE(efficiencyDecorator.ValueToString(efficiency, &result));

        // Verify presence and order: number -> % -> "efficiency"
        std::string resultStr = result.StdString();
        size_t percentPos = resultStr.find("%");
        size_t efficiencyPos = resultStr.find("efficiency");

        EXPECT_NE(percentPos, std::string::npos);
        EXPECT_NE(efficiencyPos, std::string::npos);
        EXPECT_LT(percentPos, efficiencyPos) << "'%' should come before 'efficiency' in result '"
                                             << result.CString() << "'";

        // Test round-trip
        double value;
        EXPECT_TRUE(efficiencyDecorator.StringToValue(result, &value));
        EXPECT_NEAR(value, efficiency, 0.001);
    }
}

//==============================================================================
// Comprehensive Permutation Tests
//==============================================================================

TEST_F(AAX_DisplayDelegateDecorator_Wrapping_Test, Permutation_AllTwoLevelCombinations)
{
    // Test all meaningful two-level combinations of decorators

    // UnitPrefix -> Unit combinations
    {
        AAX_CUnitPrefixDisplayDelegateDecorator<double> prefixDecorator(*mBaseDelegate);
        AAX_CUnitDisplayDelegateDecorator<double> unitDecorator(prefixDecorator, "Hz");

        AAX_CString result;
        EXPECT_TRUE(unitDecorator.ValueToString(1500.0, &result));

        // Verify order: prefix before unit
        std::string resultStr = result.StdString();
        size_t prefixPos = resultStr.find("k");
        size_t unitPos = resultStr.find("Hz");
        EXPECT_NE(prefixPos, std::string::npos);
        EXPECT_NE(unitPos, std::string::npos);
        EXPECT_LT(prefixPos, unitPos);
    }

    // Percent -> Unit combinations
    {
        AAX_CPercentDisplayDelegateDecorator<double> percentDecorator(*mBaseDelegate);
        AAX_CUnitDisplayDelegateDecorator<double> unitDecorator(percentDecorator, "load");

        AAX_CString result;
        EXPECT_TRUE(unitDecorator.ValueToString(0.75, &result));

        // Verify order: % before unit
        std::string resultStr = result.StdString();
        size_t percentPos = resultStr.find("%");
        size_t unitPos = resultStr.find("load");
        EXPECT_NE(percentPos, std::string::npos);
        EXPECT_NE(unitPos, std::string::npos);
        EXPECT_LT(percentPos, unitPos);
    }

    // Decibel -> Unit combinations
    {
        AAX_CDecibelDisplayDelegateDecorator<double> decibelDecorator(*mBaseDelegate);
        AAX_CUnitDisplayDelegateDecorator<double> unitDecorator(decibelDecorator, "SPL");

        AAX_CString result;
        EXPECT_TRUE(unitDecorator.ValueToString(1.0, &result));

        // Verify order: dB before unit
        std::string resultStr = result.StdString();
        size_t dbPos = resultStr.find("dB");
        size_t unitPos = resultStr.find("SPL");
        EXPECT_NE(dbPos, std::string::npos);
        EXPECT_NE(unitPos, std::string::npos);
        EXPECT_LT(dbPos, unitPos);
    }

    // Unit -> UnitPrefix combinations (reverse order)
    {
        AAX_CUnitDisplayDelegateDecorator<double> unitDecorator(*mBaseDelegate, "V");
        AAX_CUnitPrefixDisplayDelegateDecorator<double> prefixDecorator(unitDecorator);

        AAX_CString result;
        EXPECT_TRUE(prefixDecorator.ValueToString(0.001, &result));

        // Verify presence and order: Unit->UnitPrefix produces "1.0 Vm"
        std::string resultStr = result.StdString();
        size_t numberPos = resultStr.find("1.0");
        size_t prefixPos = resultStr.find("m");
        size_t unitPos = resultStr.find("V");

        EXPECT_NE(numberPos, std::string::npos);
        EXPECT_NE(prefixPos, std::string::npos);
        EXPECT_NE(unitPos, std::string::npos);

        // Verify order: number -> unit -> prefix (decorator application order)
        EXPECT_LT(numberPos, unitPos);   // Number before unit
        EXPECT_LT(unitPos, prefixPos);   // Unit before prefix
    }
}

//==============================================================================
// Performance and Stress Tests
//==============================================================================

TEST_F(AAX_DisplayDelegateDecorator_Wrapping_Test, Stress_DeepNesting)
{
    // Test deep nesting of decorators (stress test)
    // Base -> Unit -> UnitPrefix -> Percent -> Unit -> Decibel
    AAX_CUnitDisplayDelegateDecorator<double> unit1(*mBaseDelegate, "base");
    AAX_CUnitPrefixDisplayDelegateDecorator<double> prefix1(unit1);
    AAX_CPercentDisplayDelegateDecorator<double> percent1(prefix1);
    AAX_CUnitDisplayDelegateDecorator<double> unit2(percent1, "complex");
    AAX_CDecibelDisplayDelegateDecorator<double> decibel1(unit2);

    // This is an extreme case that tests the limits of the decorator pattern
    AAX_CString result;
    EXPECT_TRUE(decibel1.ValueToString(0.5, &result));

    // Test that we can still do round-trip conversion despite deep nesting
    double value;
    EXPECT_TRUE(decibel1.StringToValue(result, &value));
    EXPECT_NEAR(value, 0.5, 0.5); // Allow large tolerance for complex transformations
}

TEST_F(AAX_DisplayDelegateDecorator_Wrapping_Test, Stress_MultipleClones)
{
    // Test multiple clones of wrapped decorators
    AAX_CUnitPrefixDisplayDelegateDecorator<double> prefixDecorator(*mBaseDelegate);
    AAX_CUnitDisplayDelegateDecorator<double> unitDecorator(prefixDecorator, "Hz");

    // Create multiple clones
    std::vector<std::unique_ptr<AAX_CUnitDisplayDelegateDecorator<double>>> clones;
    for (int i = 0; i < 5; ++i) {
        clones.push_back(std::unique_ptr<AAX_CUnitDisplayDelegateDecorator<double>>(
            unitDecorator.Clone()));
    }

    // Test that all clones behave identically
    double testValue = 2500.0;
    AAX_CString originalResult;
    EXPECT_TRUE(unitDecorator.ValueToString(testValue, &originalResult));

    for (const auto& clone : clones) {
        AAX_CString cloneResult;
        EXPECT_TRUE(clone->ValueToString(testValue, &cloneResult));
        EXPECT_EQ(originalResult.StdString(), cloneResult.StdString());
    }
}
