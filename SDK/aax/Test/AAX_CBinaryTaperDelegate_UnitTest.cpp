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
 *	\file AAX_CBinaryTaperDelegate_UnitTest.cpp
 *
 *	\brief Unit tests for AAX_CBinaryTaperDelegate template class
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX_CBinaryTaperDelegate.h"
#include <cmath>
#include <limits>

//--------------------------------------------------------------------
#pragma mark - Test fixture

class AAX_CBinaryTaperDelegate_Test : public ::testing::Test
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

TEST_F(AAX_CBinaryTaperDelegate_Test, BoolTaper_BasicConstruction)
{
    AAX_CBinaryTaperDelegate<bool> taper;
    
    // Test basic properties - binary taper always has min=false, max=true
    EXPECT_FALSE(taper.GetMinimumValue());
    EXPECT_TRUE(taper.GetMaximumValue());
}

TEST_F(AAX_CBinaryTaperDelegate_Test, BoolTaper_RealToNormalized)
{
    AAX_CBinaryTaperDelegate<bool> taper;
    
    // Test positive values map to 1.0
    ExpectNear(1.0, taper.RealToNormalized(true));
    
    // Test negative/zero values map to 0.0
    ExpectNear(0.0, taper.RealToNormalized(false));
}

TEST_F(AAX_CBinaryTaperDelegate_Test, BoolTaper_NormalizedToReal)
{
    AAX_CBinaryTaperDelegate<bool> taper;
    
    // Test positive normalized values map to true
    EXPECT_TRUE(taper.NormalizedToReal(1.0));
    EXPECT_TRUE(taper.NormalizedToReal(0.5));
    EXPECT_TRUE(taper.NormalizedToReal(0.1));
    
    // Test zero normalized value maps to false
    EXPECT_FALSE(taper.NormalizedToReal(0.0));
    
    // Test negative normalized values map to false
    EXPECT_FALSE(taper.NormalizedToReal(-0.5));
}

TEST_F(AAX_CBinaryTaperDelegate_Test, BoolTaper_RoundTrip)
{
    AAX_CBinaryTaperDelegate<bool> taper;
    
    // Test round-trip conversion: real -> normalized -> real
    bool testValues[] = {true, false};
    
    for (bool testValue : testValues) {
        double normalized = taper.RealToNormalized(testValue);
        bool backToReal = taper.NormalizedToReal(normalized);
        EXPECT_EQ(testValue, backToReal);
    }
}

TEST_F(AAX_CBinaryTaperDelegate_Test, BoolTaper_ConstrainRealValue)
{
    AAX_CBinaryTaperDelegate<bool> taper;
    
    // ConstrainRealValue should return the value unchanged (no constraining for binary)
    EXPECT_TRUE(taper.ConstrainRealValue(true));
    EXPECT_FALSE(taper.ConstrainRealValue(false));
}

TEST_F(AAX_CBinaryTaperDelegate_Test, FloatTaper_BasicFunctionality)
{
    AAX_CBinaryTaperDelegate<float> taper;
    
    // Test basic properties - for float, min/max are still false/true (0/1)
    EXPECT_FLOAT_EQ(0.0f, taper.GetMinimumValue()); // false cast to float
    EXPECT_FLOAT_EQ(1.0f, taper.GetMaximumValue()); // true cast to float
    
    // Test positive values map to 1.0 normalized
    ExpectNear(1.0, taper.RealToNormalized(1.0f));
    ExpectNear(1.0, taper.RealToNormalized(0.5f));
    ExpectNear(1.0, taper.RealToNormalized(100.0f));
    
    // Test negative/zero values map to 0.0 normalized
    ExpectNear(0.0, taper.RealToNormalized(0.0f));
    ExpectNear(0.0, taper.RealToNormalized(-0.5f));
    ExpectNear(0.0, taper.RealToNormalized(-100.0f));
    
    // Test normalized to real conversion
    EXPECT_FLOAT_EQ(1.0f, taper.NormalizedToReal(1.0));
    EXPECT_FLOAT_EQ(1.0f, taper.NormalizedToReal(0.5));
    EXPECT_FLOAT_EQ(1.0f, taper.NormalizedToReal(0.1));
    EXPECT_FLOAT_EQ(0.0f, taper.NormalizedToReal(0.0));
    EXPECT_FLOAT_EQ(0.0f, taper.NormalizedToReal(-0.5));
}

TEST_F(AAX_CBinaryTaperDelegate_Test, IntTaper_BasicFunctionality)
{
    AAX_CBinaryTaperDelegate<int32_t> taper;
    
    // Test basic properties
    EXPECT_EQ(0, taper.GetMinimumValue()); // false cast to int
    EXPECT_EQ(1, taper.GetMaximumValue()); // true cast to int
    
    // Test positive values map to 1.0 normalized
    ExpectNear(1.0, taper.RealToNormalized(1));
    ExpectNear(1.0, taper.RealToNormalized(42));
    ExpectNear(1.0, taper.RealToNormalized(1000));
    
    // Test negative/zero values map to 0.0 normalized
    ExpectNear(0.0, taper.RealToNormalized(0));
    ExpectNear(0.0, taper.RealToNormalized(-1));
    ExpectNear(0.0, taper.RealToNormalized(-42));
    
    // Test normalized to real conversion
    EXPECT_EQ(1, taper.NormalizedToReal(1.0));
    EXPECT_EQ(1, taper.NormalizedToReal(0.5));
    EXPECT_EQ(1, taper.NormalizedToReal(0.1));
    EXPECT_EQ(0, taper.NormalizedToReal(0.0));
    EXPECT_EQ(0, taper.NormalizedToReal(-0.5));
}

TEST_F(AAX_CBinaryTaperDelegate_Test, DoubleTaper_HighPrecision)
{
    AAX_CBinaryTaperDelegate<double> taper;
    
    // Test basic properties
    EXPECT_DOUBLE_EQ(0.0, taper.GetMinimumValue());
    EXPECT_DOUBLE_EQ(1.0, taper.GetMaximumValue());
    
    // Test high precision values
    ExpectNear(1.0, taper.RealToNormalized(0.000001));
    ExpectNear(0.0, taper.RealToNormalized(-0.000001));
    
    // Test normalized to real conversion
    EXPECT_DOUBLE_EQ(1.0, taper.NormalizedToReal(0.999999));
    EXPECT_DOUBLE_EQ(0.0, taper.NormalizedToReal(0.0));
}

TEST_F(AAX_CBinaryTaperDelegate_Test, Clone)
{
    AAX_CBinaryTaperDelegate<float> original;
    std::unique_ptr<AAX_ITaperDelegate<float>> clone(original.Clone());

    EXPECT_FLOAT_EQ(0.0f, clone->GetMinimumValue());
    EXPECT_FLOAT_EQ(1.0f, clone->GetMaximumValue());

    // Test that clone works the same as original
    float testValue = 5.0f;
    EXPECT_FLOAT_EQ(original.ConstrainRealValue(testValue), clone->ConstrainRealValue(testValue));
    ExpectNear(original.RealToNormalized(testValue), clone->RealToNormalized(testValue));
    EXPECT_FLOAT_EQ(original.NormalizedToReal(0.5), clone->NormalizedToReal(0.5));
}

TEST_F(AAX_CBinaryTaperDelegate_Test, EdgeCases_BoundaryValues)
{
    AAX_CBinaryTaperDelegate<float> taper;
    
    // Test exactly zero
    ExpectNear(0.0, taper.RealToNormalized(0.0f));
    
    // Test very small positive value
    ExpectNear(1.0, taper.RealToNormalized(std::numeric_limits<float>::min()));
    
    // Test very small negative value
    ExpectNear(0.0, taper.RealToNormalized(-std::numeric_limits<float>::min()));
    
    // Test infinity
    ExpectNear(1.0, taper.RealToNormalized(std::numeric_limits<float>::infinity()));
    ExpectNear(0.0, taper.RealToNormalized(-std::numeric_limits<float>::infinity()));
}

TEST_F(AAX_CBinaryTaperDelegate_Test, BinaryBehavior_Consistency)
{
    AAX_CBinaryTaperDelegate<bool> boolTaper;
    AAX_CBinaryTaperDelegate<float> floatTaper;
    AAX_CBinaryTaperDelegate<int32_t> intTaper;
    
    // All tapers should have consistent binary behavior
    // Positive values always map to 1.0 normalized
    ExpectNear(1.0, boolTaper.RealToNormalized(true));
    ExpectNear(1.0, floatTaper.RealToNormalized(1.0f));
    ExpectNear(1.0, intTaper.RealToNormalized(1));
    
    // Zero/negative values always map to 0.0 normalized
    ExpectNear(0.0, boolTaper.RealToNormalized(false));
    ExpectNear(0.0, floatTaper.RealToNormalized(0.0f));
    ExpectNear(0.0, intTaper.RealToNormalized(0));
    
    // Positive normalized values always map to "true" equivalent
    EXPECT_TRUE(boolTaper.NormalizedToReal(0.5));
    EXPECT_FLOAT_EQ(1.0f, floatTaper.NormalizedToReal(0.5));
    EXPECT_EQ(1, intTaper.NormalizedToReal(0.5));
    
    // Zero normalized value always maps to "false" equivalent
    EXPECT_FALSE(boolTaper.NormalizedToReal(0.0));
    EXPECT_FLOAT_EQ(0.0f, floatTaper.NormalizedToReal(0.0));
    EXPECT_EQ(0, intTaper.NormalizedToReal(0.0));
}
