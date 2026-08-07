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
 *	\file AAX_CPieceWiseLinearTaperDelegate_UnitTest.cpp
 *
 *	\brief Unit tests for AAX_CPieceWiseLinearTaperDelegate template class
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX_CPieceWiseLinearTaperDelegate.h"
#include <cmath>
#include <memory>

//==============================================================================
// Test Fixture
//==============================================================================

class AAX_CPieceWiseLinearTaperDelegate_Test : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
    
    // Helper function to compare floating point values with tolerance
    bool IsNearlyEqual(double a, double b, double tolerance = 1e-6) const
    {
        return std::abs(a - b) < tolerance;
    }
    
    bool IsNearlyEqual(float a, float b, float tolerance = 1e-5f) const
    {
        return std::abs(a - b) < tolerance;
    }
};

//==============================================================================
// Basic Construction and Destruction Tests
//==============================================================================

TEST_F(AAX_CPieceWiseLinearTaperDelegate_Test, Construction_BasicFloat)
{
    // Test basic construction with float type and default precision
    const double normalizedValues[] = {0.0, 0.5, 1.0};
    const float realValues[] = {10.0f, 50.0f, 100.0f};
    const int32_t numValues = 3;
    
    AAX_CPieceWiseLinearTaperDelegate<float> taper(normalizedValues, realValues, numValues);
    
    EXPECT_EQ(10.0f, taper.GetMinimumValue());
    EXPECT_EQ(100.0f, taper.GetMaximumValue());
}

TEST_F(AAX_CPieceWiseLinearTaperDelegate_Test, Construction_BasicDouble)
{
    // Test basic construction with double type
    const double normalizedValues[] = {0.0, 0.25, 0.75, 1.0};
    const double realValues[] = {1.0, 10.0, 90.0, 100.0};
    const int32_t numValues = 4;
    
    AAX_CPieceWiseLinearTaperDelegate<double, 1000> taper(normalizedValues, realValues, numValues);
    
    EXPECT_EQ(1.0, taper.GetMinimumValue());
    EXPECT_EQ(100.0, taper.GetMaximumValue());
}

TEST_F(AAX_CPieceWiseLinearTaperDelegate_Test, Construction_BasicInt)
{
    // Test basic construction with int type
    const double normalizedValues[] = {0.0, 0.5, 1.0};
    const int realValues[] = {0, 50, 100};
    const int32_t numValues = 3;
    
    AAX_CPieceWiseLinearTaperDelegate<int, 1> taper(normalizedValues, realValues, numValues);
    
    EXPECT_EQ(0, taper.GetMinimumValue());
    EXPECT_EQ(100, taper.GetMaximumValue());
}

TEST_F(AAX_CPieceWiseLinearTaperDelegate_Test, Construction_EmptyArrays)
{
    // Test construction with empty arrays
    const double* normalizedValues = nullptr;
    const float* realValues = nullptr;
    const int32_t numValues = 0;
    
    AAX_CPieceWiseLinearTaperDelegate<float> taper(normalizedValues, realValues, numValues);
    
    EXPECT_EQ(0.0f, taper.GetMinimumValue());
    EXPECT_EQ(0.0f, taper.GetMaximumValue());
}

TEST_F(AAX_CPieceWiseLinearTaperDelegate_Test, Construction_SingleValue)
{
    // Test construction with single value
    const double normalizedValues[] = {0.5};
    const float realValues[] = {42.0f};
    const int32_t numValues = 1;
    
    AAX_CPieceWiseLinearTaperDelegate<float> taper(normalizedValues, realValues, numValues);
    
    EXPECT_EQ(42.0f, taper.GetMinimumValue());
    EXPECT_EQ(42.0f, taper.GetMaximumValue());
}

//==============================================================================
// Copy Constructor Tests
//==============================================================================

TEST_F(AAX_CPieceWiseLinearTaperDelegate_Test, CopyConstructor_Basic)
{
    // Test copy constructor
    const double normalizedValues[] = {0.0, 0.3, 0.7, 1.0};
    const float realValues[] = {5.0f, 15.0f, 85.0f, 95.0f};
    const int32_t numValues = 4;
    
    AAX_CPieceWiseLinearTaperDelegate<float> original(normalizedValues, realValues, numValues);
    AAX_CPieceWiseLinearTaperDelegate<float> copy(original);
    
    EXPECT_EQ(original.GetMinimumValue(), copy.GetMinimumValue());
    EXPECT_EQ(original.GetMaximumValue(), copy.GetMaximumValue());
    
    // Test that they produce the same results
    EXPECT_TRUE(IsNearlyEqual(original.NormalizedToReal(0.5), copy.NormalizedToReal(0.5)));
    EXPECT_TRUE(IsNearlyEqual(original.RealToNormalized(50.0f), copy.RealToNormalized(50.0f)));
}

//==============================================================================
// Clone Tests
//==============================================================================

TEST_F(AAX_CPieceWiseLinearTaperDelegate_Test, Clone_Basic)
{
    // Test Clone method
    const double normalizedValues[] = {0.0, 0.5, 1.0};
    const float realValues[] = {20.0f, 60.0f, 100.0f};
    const int32_t numValues = 3;
    
    AAX_CPieceWiseLinearTaperDelegate<float> original(normalizedValues, realValues, numValues);
    std::unique_ptr<AAX_CPieceWiseLinearTaperDelegate<float>> clone(original.Clone());
    
    EXPECT_NE(&original, clone.get()); // Different objects
    EXPECT_EQ(original.GetMinimumValue(), clone->GetMinimumValue());
    EXPECT_EQ(original.GetMaximumValue(), clone->GetMaximumValue());
    
    // Test that they produce the same results
    EXPECT_TRUE(IsNearlyEqual(original.NormalizedToReal(0.25), clone->NormalizedToReal(0.25)));
    EXPECT_TRUE(IsNearlyEqual(original.RealToNormalized(40.0f), clone->RealToNormalized(40.0f)));
}

//==============================================================================
// ConstrainRealValue Tests
//==============================================================================

TEST_F(AAX_CPieceWiseLinearTaperDelegate_Test, ConstrainRealValue_WithinRange)
{
    const double normalizedValues[] = {0.0, 0.5, 1.0};
    const float realValues[] = {10.0f, 50.0f, 100.0f};
    const int32_t numValues = 3;

    AAX_CPieceWiseLinearTaperDelegate<float> taper(normalizedValues, realValues, numValues);

    // Values within range should be unchanged (after rounding)
    EXPECT_TRUE(IsNearlyEqual(25.0f, taper.ConstrainRealValue(25.0f)));
    EXPECT_TRUE(IsNearlyEqual(75.0f, taper.ConstrainRealValue(75.0f)));
    EXPECT_TRUE(IsNearlyEqual(10.0f, taper.ConstrainRealValue(10.0f)));
    EXPECT_TRUE(IsNearlyEqual(100.0f, taper.ConstrainRealValue(100.0f)));
}

TEST_F(AAX_CPieceWiseLinearTaperDelegate_Test, ConstrainRealValue_OutOfRange)
{
    const double normalizedValues[] = {0.0, 0.5, 1.0};
    const float realValues[] = {10.0f, 50.0f, 100.0f};
    const int32_t numValues = 3;

    AAX_CPieceWiseLinearTaperDelegate<float> taper(normalizedValues, realValues, numValues);

    // Values above max should be clamped to max
    EXPECT_EQ(100.0f, taper.ConstrainRealValue(150.0f));
    EXPECT_EQ(100.0f, taper.ConstrainRealValue(1000.0f));

    // Values below min should be clamped to min
    EXPECT_EQ(10.0f, taper.ConstrainRealValue(5.0f));
    EXPECT_EQ(10.0f, taper.ConstrainRealValue(-50.0f));
}

TEST_F(AAX_CPieceWiseLinearTaperDelegate_Test, ConstrainRealValue_ReverseRange)
{
    // Test with decreasing real values
    const double normalizedValues[] = {0.0, 0.5, 1.0};
    const float realValues[] = {100.0f, 50.0f, 10.0f};
    const int32_t numValues = 3;

    AAX_CPieceWiseLinearTaperDelegate<float> taper(normalizedValues, realValues, numValues);

    EXPECT_EQ(10.0f, taper.GetMinimumValue());
    EXPECT_EQ(100.0f, taper.GetMaximumValue());

    // Values above max should be clamped to max
    EXPECT_EQ(100.0f, taper.ConstrainRealValue(150.0f));

    // Values below min should be clamped to min
    EXPECT_EQ(10.0f, taper.ConstrainRealValue(5.0f));
}

TEST_F(AAX_CPieceWiseLinearTaperDelegate_Test, ConstrainRealValue_SingleValue)
{
    const double normalizedValues[] = {0.5};
    const float realValues[] = {42.0f};
    const int32_t numValues = 1;

    AAX_CPieceWiseLinearTaperDelegate<float> taper(normalizedValues, realValues, numValues);

    // When min == max, should always return that value
    EXPECT_EQ(42.0f, taper.ConstrainRealValue(0.0f));
    EXPECT_EQ(42.0f, taper.ConstrainRealValue(42.0f));
    EXPECT_EQ(42.0f, taper.ConstrainRealValue(100.0f));
}

//==============================================================================
// NormalizedToReal Tests
//==============================================================================

TEST_F(AAX_CPieceWiseLinearTaperDelegate_Test, NormalizedToReal_LinearInterpolation)
{
    const double normalizedValues[] = {0.0, 0.5, 1.0};
    const float realValues[] = {10.0f, 50.0f, 100.0f};
    const int32_t numValues = 3;

    AAX_CPieceWiseLinearTaperDelegate<float> taper(normalizedValues, realValues, numValues);

    // Test exact points
    EXPECT_TRUE(IsNearlyEqual(10.0f, taper.NormalizedToReal(0.0)));
    EXPECT_TRUE(IsNearlyEqual(50.0f, taper.NormalizedToReal(0.5)));
    EXPECT_TRUE(IsNearlyEqual(100.0f, taper.NormalizedToReal(1.0)));

    // Test interpolated points
    EXPECT_TRUE(IsNearlyEqual(30.0f, taper.NormalizedToReal(0.25))); // Between 10 and 50
    EXPECT_TRUE(IsNearlyEqual(75.0f, taper.NormalizedToReal(0.75))); // Between 50 and 100
}

TEST_F(AAX_CPieceWiseLinearTaperDelegate_Test, NormalizedToReal_OutOfRange)
{
    const double normalizedValues[] = {0.0, 0.5, 1.0};
    const float realValues[] = {10.0f, 50.0f, 100.0f};
    const int32_t numValues = 3;

    AAX_CPieceWiseLinearTaperDelegate<float> taper(normalizedValues, realValues, numValues);

    // Values outside [0,1] should be clamped
    EXPECT_TRUE(IsNearlyEqual(10.0f, taper.NormalizedToReal(-0.5)));
    EXPECT_TRUE(IsNearlyEqual(100.0f, taper.NormalizedToReal(1.5)));
}

TEST_F(AAX_CPieceWiseLinearTaperDelegate_Test, NormalizedToReal_NonUniformSpacing)
{
    // Test with non-uniform normalized spacing
    const double normalizedValues[] = {0.0, 0.1, 0.9, 1.0};
    const float realValues[] = {0.0f, 10.0f, 90.0f, 100.0f};
    const int32_t numValues = 4;

    AAX_CPieceWiseLinearTaperDelegate<float> taper(normalizedValues, realValues, numValues);

    // Test exact points
    EXPECT_TRUE(IsNearlyEqual(0.0f, taper.NormalizedToReal(0.0)));
    EXPECT_TRUE(IsNearlyEqual(10.0f, taper.NormalizedToReal(0.1)));
    EXPECT_TRUE(IsNearlyEqual(90.0f, taper.NormalizedToReal(0.9)));
    EXPECT_TRUE(IsNearlyEqual(100.0f, taper.NormalizedToReal(1.0)));

    // Test interpolated points
    EXPECT_TRUE(IsNearlyEqual(5.0f, taper.NormalizedToReal(0.05))); // Between 0 and 10
    EXPECT_TRUE(IsNearlyEqual(50.0f, taper.NormalizedToReal(0.5)));  // Between 10 and 90
}

//==============================================================================
// RealToNormalized Tests
//==============================================================================

TEST_F(AAX_CPieceWiseLinearTaperDelegate_Test, RealToNormalized_LinearInterpolation)
{
    const double normalizedValues[] = {0.0, 0.5, 1.0};
    const float realValues[] = {10.0f, 50.0f, 100.0f};
    const int32_t numValues = 3;

    AAX_CPieceWiseLinearTaperDelegate<float> taper(normalizedValues, realValues, numValues);

    // Test exact points
    EXPECT_TRUE(IsNearlyEqual(0.0, taper.RealToNormalized(10.0f)));
    EXPECT_TRUE(IsNearlyEqual(0.5, taper.RealToNormalized(50.0f)));
    EXPECT_TRUE(IsNearlyEqual(1.0, taper.RealToNormalized(100.0f)));

    // Test interpolated points
    EXPECT_TRUE(IsNearlyEqual(0.25, taper.RealToNormalized(30.0f))); // Between 10 and 50
    EXPECT_TRUE(IsNearlyEqual(0.75, taper.RealToNormalized(75.0f))); // Between 50 and 100
}

TEST_F(AAX_CPieceWiseLinearTaperDelegate_Test, RealToNormalized_OutOfRange)
{
    const double normalizedValues[] = {0.0, 0.5, 1.0};
    const float realValues[] = {10.0f, 50.0f, 100.0f};
    const int32_t numValues = 3;

    AAX_CPieceWiseLinearTaperDelegate<float> taper(normalizedValues, realValues, numValues);

    // Values outside real range should be constrained first
    EXPECT_TRUE(IsNearlyEqual(0.0, taper.RealToNormalized(5.0f)));   // Below min
    EXPECT_TRUE(IsNearlyEqual(1.0, taper.RealToNormalized(150.0f))); // Above max
}

TEST_F(AAX_CPieceWiseLinearTaperDelegate_Test, RealToNormalized_DecreasingValues)
{
    // Test with decreasing real values
    const double normalizedValues[] = {0.0, 0.5, 1.0};
    const float realValues[] = {100.0f, 50.0f, 10.0f};
    const int32_t numValues = 3;

    AAX_CPieceWiseLinearTaperDelegate<float> taper(normalizedValues, realValues, numValues);

    // Test exact points
    EXPECT_TRUE(IsNearlyEqual(0.0, taper.RealToNormalized(100.0f)));
    EXPECT_TRUE(IsNearlyEqual(0.5, taper.RealToNormalized(50.0f)));
    EXPECT_TRUE(IsNearlyEqual(1.0, taper.RealToNormalized(10.0f)));

    // Test interpolated points
    EXPECT_TRUE(IsNearlyEqual(0.25, taper.RealToNormalized(75.0f))); // Between 100 and 50
    EXPECT_TRUE(IsNearlyEqual(0.75, taper.RealToNormalized(30.0f))); // Between 50 and 10
}

TEST_F(AAX_CPieceWiseLinearTaperDelegate_Test, RealToNormalized_FlatSegment)
{
    // Test with a flat segment (same real values)
    const double normalizedValues[] = {0.0, 0.3, 0.7, 1.0};
    const float realValues[] = {10.0f, 50.0f, 50.0f, 90.0f};
    const int32_t numValues = 4;

    AAX_CPieceWiseLinearTaperDelegate<float> taper(normalizedValues, realValues, numValues);

    // Test exact points
    EXPECT_TRUE(IsNearlyEqual(0.0, taper.RealToNormalized(10.0f)));
    EXPECT_TRUE(IsNearlyEqual(1.0, taper.RealToNormalized(90.0f)));

    // For the flat segment, should return middle normalized value (0.5 due to slope calculation)
    double result = taper.RealToNormalized(50.0f);
    EXPECT_TRUE(result >= 0.3 && result <= 0.7); // Should be somewhere in the flat segment
}

//==============================================================================
// Precision Tests
//==============================================================================

TEST_F(AAX_CPieceWiseLinearTaperDelegate_Test, Precision_DefaultPrecision100)
{
    const double normalizedValues[] = {0.0, 1.0};
    const float realValues[] = {0.0f, 1.0f};
    const int32_t numValues = 2;

    AAX_CPieceWiseLinearTaperDelegate<float> taper(normalizedValues, realValues, numValues);

    // With precision 100, should round to nearest 0.01
    float result = taper.ConstrainRealValue(0.123456f);
    EXPECT_TRUE(IsNearlyEqual(0.12f, result, 0.001f)); // Should round to 0.12
}

TEST_F(AAX_CPieceWiseLinearTaperDelegate_Test, Precision_HighPrecision1000)
{
    const double normalizedValues[] = {0.0, 1.0};
    const float realValues[] = {0.0f, 1.0f};
    const int32_t numValues = 2;

    AAX_CPieceWiseLinearTaperDelegate<float, 1000> taper(normalizedValues, realValues, numValues);

    // With precision 1000, should round to nearest 0.001
    float result = taper.ConstrainRealValue(0.123456f);
    EXPECT_TRUE(IsNearlyEqual(0.123f, result, 0.0001f)); // Should round to 0.123
}

TEST_F(AAX_CPieceWiseLinearTaperDelegate_Test, Precision_IntegerPrecision1)
{
    const double normalizedValues[] = {0.0, 1.0};
    const double realValues[] = {0., 100.};
    const int32_t numValues = 2;

    AAX_CPieceWiseLinearTaperDelegate<double, 1> taper(normalizedValues, realValues, numValues);

    // With precision 1, should round to nearest integer
    // Note: The actual rounding behavior may depend on implementation details
    int result = taper.ConstrainRealValue(42.7);
    EXPECT_EQ(43, result);

    result = taper.ConstrainRealValue(42.3);
    EXPECT_EQ(42, result);

    // Test with clearly rounded values
    result = taper.ConstrainRealValue(42.0);
    EXPECT_EQ(42, result);

    result = taper.ConstrainRealValue(43.0);
    EXPECT_EQ(43, result);
}

TEST_F(AAX_CPieceWiseLinearTaperDelegate_Test, Precision_NoPrecision0)
{
    const double normalizedValues[] = {0.0, 1.0};
    const float realValues[] = {0.0f, 1.0f};
    const int32_t numValues = 2;

    AAX_CPieceWiseLinearTaperDelegate<float, 0> taper(normalizedValues, realValues, numValues);

    // With precision 0, should not round
    float result = taper.ConstrainRealValue(0.123456f);
    EXPECT_TRUE(IsNearlyEqual(0.123456f, result, 0.000001f)); // Should not round
}

//==============================================================================
// Roundtrip Tests (NormalizedToReal -> RealToNormalized)
//==============================================================================

TEST_F(AAX_CPieceWiseLinearTaperDelegate_Test, Roundtrip_BasicLinear)
{
    const double normalizedValues[] = {0.0, 0.5, 1.0};
    const float realValues[] = {10.0f, 50.0f, 100.0f};
    const int32_t numValues = 3;

    AAX_CPieceWiseLinearTaperDelegate<float> taper(normalizedValues, realValues, numValues);

    // Test roundtrip conversion
    const double testNormalizedValues[] = {0.0, 0.1, 0.25, 0.5, 0.75, 0.9, 1.0};

    for (double normalizedValue : testNormalizedValues)
    {
        float realValue = taper.NormalizedToReal(normalizedValue);
        double backToNormalized = taper.RealToNormalized(realValue);
        EXPECT_TRUE(IsNearlyEqual(normalizedValue, backToNormalized, 0.001))
            << "Roundtrip failed for normalized value " << normalizedValue
            << " -> real " << realValue << " -> normalized " << backToNormalized;
    }
}

TEST_F(AAX_CPieceWiseLinearTaperDelegate_Test, Roundtrip_NonLinear)
{
    // Test with non-linear curve
    const double normalizedValues[] = {0.0, 0.1, 0.5, 0.9, 1.0};
    const float realValues[] = {1.0f, 2.0f, 50.0f, 98.0f, 100.0f};
    const int32_t numValues = 5;

    AAX_CPieceWiseLinearTaperDelegate<float> taper(normalizedValues, realValues, numValues);

    // Test roundtrip conversion
    const double testNormalizedValues[] = {0.0, 0.05, 0.3, 0.7, 0.95, 1.0};

    for (double normalizedValue : testNormalizedValues)
    {
        float realValue = taper.NormalizedToReal(normalizedValue);
        double backToNormalized = taper.RealToNormalized(realValue);
        EXPECT_TRUE(IsNearlyEqual(normalizedValue, backToNormalized, 0.01))
            << "Roundtrip failed for normalized value " << normalizedValue;
    }
}

//==============================================================================
// Edge Case Tests
//==============================================================================

TEST_F(AAX_CPieceWiseLinearTaperDelegate_Test, EdgeCase_IdenticalRealValues)
{
    // Test with all identical real values
    const double normalizedValues[] = {0.0, 0.5, 1.0};
    const float realValues[] = {42.0f, 42.0f, 42.0f};
    const int32_t numValues = 3;

    AAX_CPieceWiseLinearTaperDelegate<float> taper(normalizedValues, realValues, numValues);

    EXPECT_EQ(42.0f, taper.GetMinimumValue());
    EXPECT_EQ(42.0f, taper.GetMaximumValue());

    // All normalized values should map to the same real value
    EXPECT_EQ(42.0f, taper.NormalizedToReal(0.0));
    EXPECT_EQ(42.0f, taper.NormalizedToReal(0.5));
    EXPECT_EQ(42.0f, taper.NormalizedToReal(1.0));

    // All real values should map to some normalized value (slope handling)
    double result = taper.RealToNormalized(42.0f);
    EXPECT_TRUE(result >= 0.0 && result <= 1.0);
}

TEST_F(AAX_CPieceWiseLinearTaperDelegate_Test, EdgeCase_VerySmallRange)
{
    const double normalizedValues[] = {0.0, 1.0};
    const float realValues[] = {1.0f, 1.001f}; // Very small range
    const int32_t numValues = 2;

    AAX_CPieceWiseLinearTaperDelegate<float, 10000> taper(normalizedValues, realValues, numValues);

    EXPECT_TRUE(IsNearlyEqual(1.0f, taper.GetMinimumValue()));
    EXPECT_TRUE(IsNearlyEqual(1.001f, taper.GetMaximumValue()));

    // Should still interpolate correctly
    float midValue = taper.NormalizedToReal(0.5);
    EXPECT_TRUE(IsNearlyEqual(1.0005f, midValue, 0.0001f));
}

TEST_F(AAX_CPieceWiseLinearTaperDelegate_Test, EdgeCase_NegativeValues)
{
    const double normalizedValues[] = {0.0, 0.5, 1.0};
    const float realValues[] = {-100.0f, 0.0f, 100.0f};
    const int32_t numValues = 3;

    AAX_CPieceWiseLinearTaperDelegate<float> taper(normalizedValues, realValues, numValues);

    EXPECT_EQ(-100.0f, taper.GetMinimumValue());
    EXPECT_EQ(100.0f, taper.GetMaximumValue());

    // Test interpolation with negative values
    EXPECT_TRUE(IsNearlyEqual(-100.0f, taper.NormalizedToReal(0.0)));
    EXPECT_TRUE(IsNearlyEqual(0.0f, taper.NormalizedToReal(0.5)));
    EXPECT_TRUE(IsNearlyEqual(100.0f, taper.NormalizedToReal(1.0)));

    EXPECT_TRUE(IsNearlyEqual(-50.0f, taper.NormalizedToReal(0.25)));
    EXPECT_TRUE(IsNearlyEqual(50.0f, taper.NormalizedToReal(0.75)));
}

//==============================================================================
// Complex Curve Tests
//==============================================================================

TEST_F(AAX_CPieceWiseLinearTaperDelegate_Test, ComplexCurve_LogarithmicApproximation)
{
    // Approximate a logarithmic curve with piecewise linear segments
    const double normalizedValues[] = {0.0, 0.1, 0.2, 0.4, 0.6, 0.8, 1.0};
    const float realValues[] = {1.0f, 2.0f, 4.0f, 16.0f, 64.0f, 256.0f, 1000.0f};
    const int32_t numValues = 7;

    AAX_CPieceWiseLinearTaperDelegate<float> taper(normalizedValues, realValues, numValues);

    EXPECT_EQ(1.0f, taper.GetMinimumValue());
    EXPECT_EQ(1000.0f, taper.GetMaximumValue());

    // Test some interpolated values
    EXPECT_TRUE(taper.NormalizedToReal(0.05) > 1.0f && taper.NormalizedToReal(0.05) < 2.0f);
    EXPECT_TRUE(taper.NormalizedToReal(0.5) > 16.0f && taper.NormalizedToReal(0.5) < 64.0f);
    EXPECT_TRUE(taper.NormalizedToReal(0.9) > 256.0f && taper.NormalizedToReal(0.9) < 1000.0f);
}
