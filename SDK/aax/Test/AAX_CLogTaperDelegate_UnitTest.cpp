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
 *	\file AAX_CLogTaperDelegate_UnitTest.cpp
 *
 *	\brief Unit tests for AAX_CLogTaperDelegate template class
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX_CLogTaperDelegate.h"
#include <cmath>

//--------------------------------------------------------------------
#pragma mark - Test fixture

class AAX_CLogTaperDelegate_Test : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
    
    // Helper function to test floating point equality with tolerance
    void ExpectNear(double expected, double actual, double tolerance = 1e-6) {
        EXPECT_NEAR(expected, actual, tolerance);
    }
};

//--------------------------------------------------------------------
#pragma mark - Basic Functionality Tests

TEST_F(AAX_CLogTaperDelegate_Test, FloatTaper_BasicConstruction)
{
    AAX_CLogTaperDelegate<float> taper(1.0f, 100.0f);
    
    // Test basic properties
    EXPECT_FLOAT_EQ(1.0f, taper.GetMinimumValue());
    EXPECT_FLOAT_EQ(100.0f, taper.GetMaximumValue());
}

TEST_F(AAX_CLogTaperDelegate_Test, FloatTaper_BoundaryValues)
{
    AAX_CLogTaperDelegate<float> taper(1.0f, 1000.0f);
    
    // Test boundary values - normalized values should always be in [0,1]
    ExpectNear(0.0, taper.RealToNormalized(1.0f));
    ExpectNear(1.0, taper.RealToNormalized(1000.0f));
    
    EXPECT_FLOAT_EQ(1.0f, taper.NormalizedToReal(0.0));
    EXPECT_FLOAT_EQ(1000.0f, taper.NormalizedToReal(1.0));
}

TEST_F(AAX_CLogTaperDelegate_Test, FloatTaper_MiddleValues)
{
    AAX_CLogTaperDelegate<float> taper(1.0f, 100.0f);
    
    // For logarithmic taper, middle normalized value (0.5) should map to geometric mean
    // Geometric mean of 1 and 100 is sqrt(1 * 100) = 10
    float middleReal = taper.NormalizedToReal(0.5);
    EXPECT_NEAR(10.0f, middleReal, 0.1f);
    
    // Verify round-trip
    double middleNormalized = taper.RealToNormalized(middleReal);
    ExpectNear(0.5, middleNormalized, 1e-6);
}

TEST_F(AAX_CLogTaperDelegate_Test, FloatTaper_RoundTrip)
{
    AAX_CLogTaperDelegate<float> taper(1.0f, 1000.0f);
    
    // Test round-trip conversion: real -> normalized -> real
    float testValues[] = {1.0f, 3.16f, 10.0f, 31.6f, 100.0f, 316.0f, 1000.0f};
    
    for (float testValue : testValues) {
        double normalized = taper.RealToNormalized(testValue);
        float backToReal = taper.NormalizedToReal(normalized);
        EXPECT_NEAR(testValue, backToReal, 0.01f);
    }
    
    // Test round-trip conversion: normalized -> real -> normalized
    double testNormalized[] = {0.0, 0.25, 0.5, 0.75, 1.0};
    
    for (double testValue : testNormalized) {
        float real = taper.NormalizedToReal(testValue);
        double backToNormalized = taper.RealToNormalized(real);
        // Logarithmic calculations have inherent floating-point precision limitations
        ExpectNear(testValue, backToNormalized, 1e-4);
    }
}

TEST_F(AAX_CLogTaperDelegate_Test, FloatTaper_ConstrainRealValue)
{
    AAX_CLogTaperDelegate<float> taper(10.0f, 100.0f);
    
    // Test constraining values within range
    EXPECT_FLOAT_EQ(50.0f, taper.ConstrainRealValue(50.0f));
    
    // Test constraining values outside range
    EXPECT_FLOAT_EQ(10.0f, taper.ConstrainRealValue(5.0f));   // Below minimum
    EXPECT_FLOAT_EQ(100.0f, taper.ConstrainRealValue(200.0f)); // Above maximum
}

TEST_F(AAX_CLogTaperDelegate_Test, FloatTaper_NormalizedRangeClamping)
{
    AAX_CLogTaperDelegate<float> taper(1.0f, 100.0f);
    
    // Normalized values should always be clamped to [0,1] range
    // Test values outside the real range
    double normalizedBelow = taper.RealToNormalized(0.5f); // Below minimum
    EXPECT_GE(normalizedBelow, 0.0); // Should be >= 0
    EXPECT_LE(normalizedBelow, 1.0); // Should be <= 1
    
    double normalizedAbove = taper.RealToNormalized(200.0f); // Above maximum
    EXPECT_GE(normalizedAbove, 0.0); // Should be >= 0
    EXPECT_LE(normalizedAbove, 1.0); // Should be <= 1
    
    // Test normalized values outside 0-1 range get clamped in NormalizedToReal
    float realBelow = taper.NormalizedToReal(-0.5);
    EXPECT_GE(realBelow, 1.0f); // Should be >= minimum
    EXPECT_LE(realBelow, 100.0f); // Should be <= maximum
    
    float realAbove = taper.NormalizedToReal(1.5);
    EXPECT_GE(realAbove, 1.0f); // Should be >= minimum
    EXPECT_LE(realAbove, 100.0f); // Should be <= maximum
}

TEST_F(AAX_CLogTaperDelegate_Test, IntTaper_BasicFunctionality)
{
    AAX_CLogTaperDelegate<int32_t> taper(1, 1000);
    
    // Test boundary values
    ExpectNear(0.0, taper.RealToNormalized(1));
    ExpectNear(1.0, taper.RealToNormalized(1000));
    
    EXPECT_EQ(1, taper.NormalizedToReal(0.0));
    // Integer taper with precision rounding may not exactly hit 1000
    int32_t maxReal = taper.NormalizedToReal(1.0);
    EXPECT_NEAR(1000, maxReal, 1); // Allow 1 unit tolerance for rounding
    
    // Test middle value (geometric mean of 1 and 1000 is ~31.6, rounds to 32)
    int32_t middleReal = taper.NormalizedToReal(0.5);
    EXPECT_NEAR(32, middleReal, 2); // Allow some rounding tolerance
}

TEST_F(AAX_CLogTaperDelegate_Test, PrecisionTemplate_HighPrecision)
{
    // Test with high precision (rounds to nearest 0.001)
    AAX_CLogTaperDelegate<float, 1000> taper(1.0f, 100.0f);
    
    float testValue = 12.3456f;
    float constrained = taper.ConstrainRealValue(testValue);
    
    // Should be rounded to nearest 0.001
    EXPECT_NEAR(12.346f, constrained, 0.001f);
}

TEST_F(AAX_CLogTaperDelegate_Test, PrecisionTemplate_IntegerPrecision)
{
    // Test with integer precision (rounds to nearest integer)
    AAX_CLogTaperDelegate<float, 1> taper(1.0f, 100.0f);
    
    float testValue = 12.7f;
    float constrained = taper.ConstrainRealValue(testValue);
    
    // Should be rounded to nearest integer
    EXPECT_FLOAT_EQ(13.0f, constrained);
}

TEST_F(AAX_CLogTaperDelegate_Test, Clone)
{
    AAX_CLogTaperDelegate<float> original(5.0f, 50.0f);
    std::unique_ptr<AAX_CLogTaperDelegate<float>> clone(original.Clone());
    
    EXPECT_FLOAT_EQ(5.0f, clone->GetMinimumValue());
    EXPECT_FLOAT_EQ(50.0f, clone->GetMaximumValue());
    
    // Test that clone works the same as original
    float testValue = 15.0f;
    EXPECT_FLOAT_EQ(original.ConstrainRealValue(testValue), clone->ConstrainRealValue(testValue));
    ExpectNear(original.RealToNormalized(testValue), clone->RealToNormalized(testValue));
}

TEST_F(AAX_CLogTaperDelegate_Test, ZeroRange_EdgeCase)
{
    AAX_CLogTaperDelegate<float> taper(10.0f, 10.0f);

    // When min == max, all values should map to the same real value
    EXPECT_FLOAT_EQ(10.0f, taper.NormalizedToReal(0.0));
    EXPECT_FLOAT_EQ(10.0f, taper.NormalizedToReal(0.5));
    EXPECT_FLOAT_EQ(10.0f, taper.NormalizedToReal(1.0));

    // For degenerate case (min == max), RealToNormalized returns 0.5 as middle value
    // This matches the behavior seen in linear taper delegate
    ExpectNear(0.5, taper.RealToNormalized(10.0f));

    // ConstrainRealValue should return the single valid value
    EXPECT_FLOAT_EQ(10.0f, taper.ConstrainRealValue(5.0f));
    EXPECT_FLOAT_EQ(10.0f, taper.ConstrainRealValue(15.0f));
    EXPECT_FLOAT_EQ(10.0f, taper.ConstrainRealValue(10.0f));
}
