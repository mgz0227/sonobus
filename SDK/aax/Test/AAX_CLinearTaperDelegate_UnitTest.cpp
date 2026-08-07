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
 *	\file AAX_CLinearTaperDelegate_UnitTest.cpp
 *
 *	\brief Unit tests for AAX_CLinearTaperDelegate template class
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX_CLinearTaperDelegate.h"
#include <cmath>

//--------------------------------------------------------------------
#pragma mark - Test fixture

class AAX_CLinearTaperDelegate_Test : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
    
    // Helper function to test floating point equality with tolerance
    void ExpectNear(double expected, double actual, double tolerance = 1e-10) {
        EXPECT_NEAR(expected, actual, tolerance);
    }
};

//--------------------------------------------------------------------
#pragma mark - Basic Functionality Tests

TEST_F(AAX_CLinearTaperDelegate_Test, FloatTaper_BasicConstruction)
{
    AAX_CLinearTaperDelegate<float> taper(0.0f, 10.0f);
    
    // Test basic properties
    EXPECT_FLOAT_EQ(0.0f, taper.GetMinimumValue());
    EXPECT_FLOAT_EQ(10.0f, taper.GetMaximumValue());
}

TEST_F(AAX_CLinearTaperDelegate_Test, FloatTaper_RealToNormalized)
{
    AAX_CLinearTaperDelegate<float> taper(0.0f, 100.0f);
    
    // Test boundary values
    ExpectNear(0.0, taper.RealToNormalized(0.0f));
    ExpectNear(1.0, taper.RealToNormalized(100.0f));
    
    // Test middle values
    ExpectNear(0.5, taper.RealToNormalized(50.0f));
    ExpectNear(0.25, taper.RealToNormalized(25.0f));
    ExpectNear(0.75, taper.RealToNormalized(75.0f));
}

TEST_F(AAX_CLinearTaperDelegate_Test, FloatTaper_NormalizedToReal)
{
    AAX_CLinearTaperDelegate<float> taper(0.0f, 100.0f);
    
    // Test boundary values
    EXPECT_FLOAT_EQ(0.0f, taper.NormalizedToReal(0.0));
    EXPECT_FLOAT_EQ(100.0f, taper.NormalizedToReal(1.0));
    
    // Test middle values
    EXPECT_FLOAT_EQ(50.0f, taper.NormalizedToReal(0.5));
    EXPECT_FLOAT_EQ(25.0f, taper.NormalizedToReal(0.25));
    EXPECT_FLOAT_EQ(75.0f, taper.NormalizedToReal(0.75));
}

TEST_F(AAX_CLinearTaperDelegate_Test, FloatTaper_RoundTrip)
{
    AAX_CLinearTaperDelegate<float> taper(-50.0f, 50.0f);
    
    // Test round-trip conversion: real -> normalized -> real
    float testValues[] = {-50.0f, -25.0f, 0.0f, 25.0f, 50.0f, -12.5f, 37.5f};
    
    for (float testValue : testValues) {
        double normalized = taper.RealToNormalized(testValue);
        float backToReal = taper.NormalizedToReal(normalized);
        EXPECT_FLOAT_EQ(testValue, backToReal);
    }
    
    // Test round-trip conversion: normalized -> real -> normalized
    double testNormalized[] = {0.0, 0.25, 0.5, 0.75, 1.0, 0.1, 0.9};
    
    for (double testValue : testNormalized) {
        float real = taper.NormalizedToReal(testValue);
        double backToNormalized = taper.RealToNormalized(real);
        ExpectNear(testValue, backToNormalized);
    }
}

TEST_F(AAX_CLinearTaperDelegate_Test, FloatTaper_NegativeRange)
{
    AAX_CLinearTaperDelegate<float> taper(-100.0f, -10.0f);
    
    // Test boundary values
    ExpectNear(0.0, taper.RealToNormalized(-100.0f));
    ExpectNear(1.0, taper.RealToNormalized(-10.0f));
    
    EXPECT_FLOAT_EQ(-100.0f, taper.NormalizedToReal(0.0));
    EXPECT_FLOAT_EQ(-10.0f, taper.NormalizedToReal(1.0));
    
    // Test middle value
    ExpectNear(0.5, taper.RealToNormalized(-55.0f));
    EXPECT_FLOAT_EQ(-55.0f, taper.NormalizedToReal(0.5));
}

TEST_F(AAX_CLinearTaperDelegate_Test, IntTaper_BasicFunctionality)
{
    AAX_CLinearTaperDelegate<int32_t> taper(0, 1000);
    
    // Test boundary values
    ExpectNear(0.0, taper.RealToNormalized(0));
    ExpectNear(1.0, taper.RealToNormalized(1000));
    
    EXPECT_EQ(0, taper.NormalizedToReal(0.0));
    EXPECT_EQ(1000, taper.NormalizedToReal(1.0));
    
    // Test middle values
    ExpectNear(0.5, taper.RealToNormalized(500));
    EXPECT_EQ(500, taper.NormalizedToReal(0.5));
    
    ExpectNear(0.25, taper.RealToNormalized(250));
    EXPECT_EQ(250, taper.NormalizedToReal(0.25));
}

TEST_F(AAX_CLinearTaperDelegate_Test, DoubleTaper_HighPrecision)
{
    AAX_CLinearTaperDelegate<double> taper(0.0, 1.0);
    
    // Test high precision values
    double testValue = 0.123456789;
    double normalized = taper.RealToNormalized(testValue);
    double backToReal = taper.NormalizedToReal(normalized);
    
    EXPECT_DOUBLE_EQ(testValue, normalized); // For 0-1 range, real == normalized
    EXPECT_DOUBLE_EQ(testValue, backToReal);
}

TEST_F(AAX_CLinearTaperDelegate_Test, EdgeCases_OutOfRange)
{
    AAX_CLinearTaperDelegate<float> taper(10.0f, 20.0f);

    // Taper delegates correctly clamp values to maintain normalized range [0,1]
    // This is the expected behavior - normalized values must always be in [0,1]

    // Test values outside the range - they get properly clamped
    double normalizedBelow = taper.RealToNormalized(5.0f);
    EXPECT_EQ(0.0, normalizedBelow); // Correctly clamped to 0

    double normalizedAbove = taper.RealToNormalized(25.0f);
    EXPECT_EQ(1.0, normalizedAbove); // Correctly clamped to 1

    // Test normalized values outside 0-1 range - they get properly clamped
    float realBelow = taper.NormalizedToReal(-0.5);
    EXPECT_EQ(10.0f, realBelow); // Correctly clamped to minimum

    float realAbove = taper.NormalizedToReal(1.5);
    EXPECT_EQ(20.0f, realAbove); // Correctly clamped to maximum
}

TEST_F(AAX_CLinearTaperDelegate_Test, ZeroRange_EdgeCase)
{
    AAX_CLinearTaperDelegate<float> taper(5.0f, 5.0f);

    // When min == max, all values should map to the same real value
    EXPECT_FLOAT_EQ(5.0f, taper.NormalizedToReal(0.0));
    EXPECT_FLOAT_EQ(5.0f, taper.NormalizedToReal(0.5));
    EXPECT_FLOAT_EQ(5.0f, taper.NormalizedToReal(1.0));

    // For degenerate case (min == max), RealToNormalized returns 0.5 as middle value
    // This is reasonable behavior for an undefined mathematical operation (division by zero)
    ExpectNear(0.5, taper.RealToNormalized(5.0f)); // Returns middle value for degenerate case
}
