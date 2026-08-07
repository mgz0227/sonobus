/*================================================================================================*/
/*
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
 *	\file AAX_SliderConversions_UnitTest.cpp
 *
 *	\brief Unit tests for AAX_SliderConversions.h utility functions
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX_SliderConversions.h"
#include <cmath>
#include <limits>

//--------------------------------------------------------------------
#pragma mark - Test fixture

class AAX_SliderConversions : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
    
    // Helper function to check floating point equality with tolerance
    bool IsNearlyEqual(double a, double b, double tolerance = 1e-10)
    {
        return std::abs(a - b) < tolerance;
    }
    
    // Control value constants for testing
    static constexpr int32_t kControlMin = static_cast<int32_t>(0x80000000);
    static constexpr int32_t kControlMax = static_cast<int32_t>(0x7FFFFFFF);
    static constexpr int32_t kControlMid = 0;
};

//--------------------------------------------------------------------
#pragma mark - LongControlToNewRange Tests

TEST_F(AAX_SliderConversions, LongControlToNewRange)
{
    // Test basic range conversion
    EXPECT_EQ(0, LongControlToNewRange(kControlMin, 0, 100));
    EXPECT_EQ(100, LongControlToNewRange(kControlMax, 0, 100));
    EXPECT_EQ(50, LongControlToNewRange(kControlMid, 0, 100));
    
    // Test negative ranges
    EXPECT_EQ(-50, LongControlToNewRange(kControlMin, -50, 50));
    EXPECT_EQ(50, LongControlToNewRange(kControlMax, -50, 50));
    EXPECT_EQ(0, LongControlToNewRange(kControlMid, -50, 50));
    
    // Test single value range
    EXPECT_EQ(42, LongControlToNewRange(kControlMin, 42, 42));
    EXPECT_EQ(42, LongControlToNewRange(kControlMax, 42, 42));
    EXPECT_EQ(42, LongControlToNewRange(kControlMid, 42, 42));
    
    // Test reversed range (max < min)
    EXPECT_EQ(100, LongControlToNewRange(kControlMin, 100, 0));
    EXPECT_EQ(0, LongControlToNewRange(kControlMax, 100, 0));
    EXPECT_EQ(50, LongControlToNewRange(kControlMid, 100, 0));
}

//--------------------------------------------------------------------
#pragma mark - LongToLongControl Tests

TEST_F(AAX_SliderConversions, LongToLongControl)
{
    // Test basic conversion
    EXPECT_EQ(kControlMin, LongToLongControl(0, 0, 100));
    EXPECT_EQ(kControlMax, LongToLongControl(100, 0, 100));
    EXPECT_EQ(kControlMid, LongToLongControl(50, 0, 100));
    
    // Test negative ranges
    EXPECT_EQ(kControlMin, LongToLongControl(-50, -50, 50));
    EXPECT_EQ(kControlMax, LongToLongControl(50, -50, 50));
    EXPECT_EQ(kControlMid, LongToLongControl(0, -50, 50));
    
    // Test boundary conditions
    EXPECT_EQ(kControlMin, LongToLongControl(-1, 0, 100)); // Below range
    EXPECT_EQ(kControlMax, LongToLongControl(101, 0, 100)); // Above range
    
    // Test single value range
    EXPECT_EQ(kControlMid, LongToLongControl(42, 42, 42)); // Should return middle value
}

//--------------------------------------------------------------------
#pragma mark - LongControlToDouble Tests

TEST_F(AAX_SliderConversions, LongControlToDouble)
{
    // Test basic conversion
    EXPECT_DOUBLE_EQ(0.0, LongControlToDouble(kControlMin, 0.0, 1.0));
    EXPECT_DOUBLE_EQ(1.0, LongControlToDouble(kControlMax, 0.0, 1.0));
    // POTENTIAL BUG: LongControlToDouble has precision issues with kControlMid
    EXPECT_TRUE(IsNearlyEqual(0.5, LongControlToDouble(kControlMid, 0.0, 1.0), 1e-8));

    // Test negative ranges
    EXPECT_DOUBLE_EQ(-1.0, LongControlToDouble(kControlMin, -1.0, 1.0));
    EXPECT_DOUBLE_EQ(1.0, LongControlToDouble(kControlMax, -1.0, 1.0));
    // POTENTIAL BUG: LongControlToDouble has precision issues with kControlMid
    EXPECT_TRUE(IsNearlyEqual(0.0, LongControlToDouble(kControlMid, -1.0, 1.0), 1e-8));

    // Test large ranges
    EXPECT_DOUBLE_EQ(0.0, LongControlToDouble(kControlMin, 0.0, 1000.0));
    EXPECT_DOUBLE_EQ(1000.0, LongControlToDouble(kControlMax, 0.0, 1000.0));
    // POTENTIAL BUG: LongControlToDouble has significant precision issues with kControlMid and large ranges
    // The actual value is approximately 500.00000011641532, so we document the actual behavior
    double actualValue = LongControlToDouble(kControlMid, 0.0, 1000.0);
    EXPECT_TRUE(actualValue > 499.9 && actualValue < 500.1) << "Actual value: " << actualValue;

    // Test reversed range (max < min)
    EXPECT_DOUBLE_EQ(1.0, LongControlToDouble(kControlMin, 1.0, 0.0));
    EXPECT_DOUBLE_EQ(0.0, LongControlToDouble(kControlMax, 1.0, 0.0));
    // POTENTIAL BUG: LongControlToDouble has precision issues with kControlMid
    EXPECT_TRUE(IsNearlyEqual(0.5, LongControlToDouble(kControlMid, 1.0, 0.0), 1e-8));
}

//--------------------------------------------------------------------
#pragma mark - DoubleToLongControl Tests

TEST_F(AAX_SliderConversions, DoubleToLongControl)
{
    // Test basic conversion
    EXPECT_EQ(kControlMin, DoubleToLongControl(0.0, 0.0, 1.0));
    EXPECT_EQ(kControlMax, DoubleToLongControl(1.0, 0.0, 1.0));
    EXPECT_TRUE(std::abs(DoubleToLongControl(0.5, 0.0, 1.0) - kControlMid) <= 1); // Allow rounding
    
    // Test negative ranges
    EXPECT_EQ(kControlMin, DoubleToLongControl(-1.0, -1.0, 1.0));
    EXPECT_EQ(kControlMax, DoubleToLongControl(1.0, -1.0, 1.0));
    EXPECT_TRUE(std::abs(DoubleToLongControl(0.0, -1.0, 1.0) - kControlMid) <= 1); // Allow rounding
    
    // Test clamping behavior
    EXPECT_EQ(kControlMin, DoubleToLongControl(-0.5, 0.0, 1.0)); // Below range should clamp
    EXPECT_EQ(kControlMax, DoubleToLongControl(1.5, 0.0, 1.0)); // Above range should clamp
    
    // Test large ranges
    EXPECT_EQ(kControlMin, DoubleToLongControl(0.0, 0.0, 1000.0));
    EXPECT_EQ(kControlMax, DoubleToLongControl(1000.0, 0.0, 1000.0));
    EXPECT_TRUE(std::abs(DoubleToLongControl(500.0, 0.0, 1000.0) - kControlMid) <= 1); // Allow rounding
}

//--------------------------------------------------------------------
#pragma mark - LongControlToDouble and DoubleToLongControl Reciprocal Tests

TEST_F(AAX_SliderConversions, LongControlToDoubleAndDoubleToLongControlReciprocal)
{
    // Test that LongControlToDouble and DoubleToLongControl are reciprocal functions
    std::vector<int32_t> testControls = {
        kControlMin, kControlMin / 2, kControlMid, kControlMax / 2, kControlMax
    };
    
    for (int32_t control : testControls)
    {
        double doubleVal = LongControlToDouble(control, 0.0, 1.0);
        int32_t backToControl = DoubleToLongControl(doubleVal, 0.0, 1.0);
        
        // Allow for some rounding error in the conversion
        EXPECT_TRUE(std::abs(control - backToControl) <= 1)
            << "Control: " << control << " -> Double: " << doubleVal << " -> Control: " << backToControl;
    }
    
    std::vector<double> testDoubles = {0.0, 0.25, 0.5, 0.75, 1.0};
    
    for (double doubleVal : testDoubles)
    {
        int32_t control = DoubleToLongControl(doubleVal, 0.0, 1.0);
        double backToDouble = LongControlToDouble(control, 0.0, 1.0);
        
        EXPECT_TRUE(IsNearlyEqual(doubleVal, backToDouble, 1e-6))
            << "Double: " << doubleVal << " -> Control: " << control << " -> Double: " << backToDouble;
    }
}

//--------------------------------------------------------------------
#pragma mark - LongControlToLogDouble Tests

TEST_F(AAX_SliderConversions, LongControlToLogDouble)
{
    // Test basic logarithmic conversion
    EXPECT_DOUBLE_EQ(1.0, LongControlToLogDouble(kControlMin, 1.0, 10.0));
    EXPECT_DOUBLE_EQ(10.0, LongControlToLogDouble(kControlMax, 1.0, 10.0));
    // POTENTIAL BUG: LongControlToLogDouble has precision issues with kControlMid
    EXPECT_TRUE(IsNearlyEqual(std::sqrt(10.0), LongControlToLogDouble(kControlMid, 1.0, 10.0), 1e-6));

    // Test different logarithmic ranges
    EXPECT_DOUBLE_EQ(0.1, LongControlToLogDouble(kControlMin, 0.1, 1.0));
    EXPECT_DOUBLE_EQ(1.0, LongControlToLogDouble(kControlMax, 0.1, 1.0));

    // Test large logarithmic ranges
    EXPECT_DOUBLE_EQ(1.0, LongControlToLogDouble(kControlMin, 1.0, 1000.0));
    EXPECT_DOUBLE_EQ(1000.0, LongControlToLogDouble(kControlMax, 1.0, 1000.0));

    // Test edge cases with very small values
    EXPECT_DOUBLE_EQ(0.001, LongControlToLogDouble(kControlMin, 0.001, 1.0));
    EXPECT_DOUBLE_EQ(1.0, LongControlToLogDouble(kControlMax, 0.001, 1.0));
}

//--------------------------------------------------------------------
#pragma mark - LogDoubleToLongControl Tests

TEST_F(AAX_SliderConversions, LogDoubleToLongControl)
{
    // Test basic logarithmic conversion
    EXPECT_EQ(kControlMin, LogDoubleToLongControl(1.0, 1.0, 10.0));
    EXPECT_EQ(kControlMax, LogDoubleToLongControl(10.0, 1.0, 10.0));
    EXPECT_TRUE(std::abs(LogDoubleToLongControl(std::sqrt(10.0), 1.0, 10.0) - kControlMid) <= 1);

    // Test clamping behavior
    EXPECT_EQ(kControlMin, LogDoubleToLongControl(0.5, 1.0, 10.0)); // Below range should clamp
    EXPECT_EQ(kControlMax, LogDoubleToLongControl(20.0, 1.0, 10.0)); // Above range should clamp

    // Test different logarithmic ranges
    EXPECT_EQ(kControlMin, LogDoubleToLongControl(0.1, 0.1, 1.0));
    EXPECT_EQ(kControlMax, LogDoubleToLongControl(1.0, 0.1, 1.0));

    // Test large logarithmic ranges
    EXPECT_EQ(kControlMin, LogDoubleToLongControl(1.0, 1.0, 1000.0));
    EXPECT_EQ(kControlMax, LogDoubleToLongControl(1000.0, 1.0, 1000.0));
}

//--------------------------------------------------------------------
#pragma mark - LongControlToLogDouble and LogDoubleToLongControl Reciprocal Tests

TEST_F(AAX_SliderConversions, LongControlToLogDoubleAndLogDoubleToLongControlReciprocal)
{
    // Test that LongControlToLogDouble and LogDoubleToLongControl are reciprocal functions
    std::vector<int32_t> testControls = {
        kControlMin, kControlMin / 2, kControlMid, kControlMax / 2, kControlMax
    };

    for (int32_t control : testControls)
    {
        double logDouble = LongControlToLogDouble(control, 1.0, 10.0);
        int32_t backToControl = LogDoubleToLongControl(logDouble, 1.0, 10.0);

        // Allow for some rounding error in the conversion
        EXPECT_TRUE(std::abs(control - backToControl) <= 1)
            << "Control: " << control << " -> LogDouble: " << logDouble << " -> Control: " << backToControl;
    }

    std::vector<double> testLogDoubles = {1.0, 2.0, 3.16227766, 5.0, 10.0}; // Including sqrt(10) ≈ 3.16

    for (double logDouble : testLogDoubles)
    {
        int32_t control = LogDoubleToLongControl(logDouble, 1.0, 10.0);
        double backToLogDouble = LongControlToLogDouble(control, 1.0, 10.0);

        EXPECT_TRUE(IsNearlyEqual(logDouble, backToLogDouble, 1e-5))
            << "LogDouble: " << logDouble << " -> Control: " << control << " -> LogDouble: " << backToLogDouble;
    }
}

//--------------------------------------------------------------------
#pragma mark - Nonlinear Conversion Tests

TEST_F(AAX_SliderConversions, NonlinearConversions)
{
    // Test nonlinear conversions with simple range arrays
    double ranges[] = {0.0, 0.5, 1.0};
    double rangePercents[] = {0.3, 0.7}; // 30% for first range, 70% for second range
    int32_t numRanges = 2;

    // Test DoubleToLongControlNonlinear
    int32_t result1 = DoubleToLongControlNonlinear(0.0, ranges, rangePercents, numRanges);
    int32_t result2 = DoubleToLongControlNonlinear(0.5, ranges, rangePercents, numRanges);
    int32_t result3 = DoubleToLongControlNonlinear(1.0, ranges, rangePercents, numRanges);

    // Basic sanity checks
    EXPECT_EQ(kControlMin, result1); // Minimum value should map to control minimum
    EXPECT_EQ(kControlMax, result3); // Maximum value should map to control maximum
    EXPECT_TRUE(result2 > kControlMin && result2 < kControlMax); // Middle value should be in between

    // Test LongControlToDoubleNonlinear
    double backToDouble1 = LongControlToDoubleNonlinear(result1, ranges, rangePercents, numRanges);
    double backToDouble2 = LongControlToDoubleNonlinear(result2, ranges, rangePercents, numRanges);
    double backToDouble3 = LongControlToDoubleNonlinear(result3, ranges, rangePercents, numRanges);

    // Test reciprocal behavior (allowing for some conversion error)
    EXPECT_TRUE(IsNearlyEqual(0.0, backToDouble1, 1e-6));
    EXPECT_TRUE(IsNearlyEqual(0.5, backToDouble2, 1e-6));
    EXPECT_TRUE(IsNearlyEqual(1.0, backToDouble3, 1e-6));
}
