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
 *	\file AAX_Migrated_TaperDelegate_UnitTest.cpp
 *
 *	\brief Migrated unit tests for Taper Delegates (originally from UnitTest++ framework)
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX_ITaperDelegate.h"
#include "AAX_CLinearTaperDelegate.h"
#include "AAX_CRangeTaperDelegate.h"
#include "AAX_CBinaryTaperDelegate.h"
#include "AAX_CPieceWiseLinearTaperDelegate.h"
#include <cmath>

//==============================================================================
// Utility Functions
//==============================================================================

double utround(double number)
{
    return number < 0.0 ? ceil(number - 0.5) : floor(number + 0.5);
}

//==============================================================================
// Test Fixture
//==============================================================================

class AAX_Migrated_TaperDelegate_Test : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // No specific setup needed for taper delegate tests
    }
    
    void TearDown() override
    {
        // No specific cleanup needed for taper delegate tests
    }
};

//==============================================================================
// Linear Float Taper Tests
//==============================================================================

TEST_F(AAX_Migrated_TaperDelegate_Test, LinearFloatTaper)
{
    const AAX_CLinearTaperDelegate<float> linearFloatTaper(-23, 18);
    
    EXPECT_EQ(-23, linearFloatTaper.GetMinimumValue());
    EXPECT_EQ(18, linearFloatTaper.GetMaximumValue());
    EXPECT_EQ(-23, linearFloatTaper.ConstrainRealValue(-50));
    EXPECT_EQ(18, linearFloatTaper.ConstrainRealValue(100));
    
    for (float v = -23.0f; v <= 18.0f; v += 0.1f)
    {
        EXPECT_NEAR(v, linearFloatTaper.ConstrainRealValue(v), 0.001);
        double normalizedValue = linearFloatTaper.RealToNormalized(v);
        EXPECT_GE(normalizedValue, 0);
        EXPECT_LE(normalizedValue, 1);
        float outV = linearFloatTaper.NormalizedToReal(normalizedValue);
        EXPECT_NEAR(v, outV, 0.001);
    }
}

TEST_F(AAX_Migrated_TaperDelegate_Test, LinearFloatTaperWithPrecision)
{
    const AAX_CLinearTaperDelegate<float, 10> linearFloatTaper(-23, 18);
    
    EXPECT_EQ(-23, linearFloatTaper.GetMinimumValue());
    EXPECT_EQ(18, linearFloatTaper.GetMaximumValue());
    EXPECT_EQ(-23, linearFloatTaper.ConstrainRealValue(-50));
    EXPECT_EQ(18, linearFloatTaper.ConstrainRealValue(100));
    
    for (float v = -23.0f; v <= 18.0f; v += 0.01f)
    {
        double normalizedValue = linearFloatTaper.RealToNormalized(v);
        EXPECT_GE(normalizedValue, 0);
        EXPECT_LE(normalizedValue, 1);
        float outV = linearFloatTaper.NormalizedToReal(normalizedValue);
        EXPECT_NEAR(utround(v * 10) / 10.0, outV, 0.001);
    }
}

TEST_F(AAX_Migrated_TaperDelegate_Test, LinearFloatTaperEdgeCases)
{
    const AAX_CLinearTaperDelegate<float> linearFloatTaper(-23, 18);
    EXPECT_EQ(-23.f, linearFloatTaper.NormalizedToReal(-10)); // input normalized out of bounds
    EXPECT_EQ(18.f, linearFloatTaper.NormalizedToReal(100));  // input normalized out of bounds
    EXPECT_EQ(1.0, linearFloatTaper.RealToNormalized(1000));  // input real is out of bounds
    EXPECT_EQ(0.0, linearFloatTaper.RealToNormalized(-300));  // input real is out of bounds
    
    // Linear taper delegate with same values for min and max
    const AAX_CLinearTaperDelegate<float> linearFloatTaperEq(0, 0);
    EXPECT_EQ(0.f, linearFloatTaperEq.NormalizedToReal(0.5f));
    EXPECT_EQ(0.f, linearFloatTaperEq.NormalizedToReal(-10));  // input normalized out of bounds
    EXPECT_EQ(0.f, linearFloatTaperEq.NormalizedToReal(100));  // input normalized out of bounds
    EXPECT_EQ(0.5, linearFloatTaperEq.RealToNormalized(1000)); // input real is out of bounds
    EXPECT_EQ(0.5, linearFloatTaperEq.RealToNormalized(-300)); // input real is out of bounds
    EXPECT_EQ(0.5, linearFloatTaperEq.RealToNormalized(0.f));  // input real is out of bounds
}

//==============================================================================
// Linear Double Taper Tests
//==============================================================================

TEST_F(AAX_Migrated_TaperDelegate_Test, LinearDoubleTaper)
{
    const AAX_CLinearTaperDelegate<double> linearDoubleTaper(-23, 18);
    
    EXPECT_EQ(-23, linearDoubleTaper.GetMinimumValue());
    EXPECT_EQ(18, linearDoubleTaper.GetMaximumValue());
    EXPECT_EQ(-23, linearDoubleTaper.ConstrainRealValue(-50));
    EXPECT_EQ(18, linearDoubleTaper.ConstrainRealValue(100));
    
    for (double v = -23; v <= 18; v += 0.1)
    {
        EXPECT_NEAR(v, linearDoubleTaper.ConstrainRealValue(v), 0.001);
        double normalizedValue = linearDoubleTaper.RealToNormalized(v);
        EXPECT_GE(normalizedValue, 0);
        EXPECT_LE(normalizedValue, 1);
        double outV = linearDoubleTaper.NormalizedToReal(normalizedValue);
        EXPECT_NEAR(v, outV, 0.001);
    }
}

TEST_F(AAX_Migrated_TaperDelegate_Test, LinearDoubleTaperEdgeCases)
{
    const AAX_CLinearTaperDelegate<double> linearDoubleTaper(-23, 18);
    EXPECT_EQ(-23, linearDoubleTaper.NormalizedToReal(-10));  // input normalized out of bounds
    EXPECT_EQ(18, linearDoubleTaper.NormalizedToReal(100));   // input normalized out of bounds
    EXPECT_EQ(1.0f, linearDoubleTaper.RealToNormalized(1000)); // input real is out of bounds
    EXPECT_EQ(0.0f, linearDoubleTaper.RealToNormalized(-300)); // input real is out of bounds
}

//==============================================================================
// Linear Integer Taper Tests
//==============================================================================

TEST_F(AAX_Migrated_TaperDelegate_Test, LinearSignedIntegerTaper)
{
    const AAX_CLinearTaperDelegate<int32_t, 1000> linearIntTaper(-23, 18);
    
    EXPECT_EQ(-23, linearIntTaper.GetMinimumValue());
    EXPECT_EQ(18, linearIntTaper.GetMaximumValue());
    EXPECT_EQ(-23, linearIntTaper.ConstrainRealValue(-50));
    EXPECT_EQ(18, linearIntTaper.ConstrainRealValue(100));
    
    for (int32_t v = -23; v <= 18; v += 1)
    {
        EXPECT_EQ(v, linearIntTaper.ConstrainRealValue(v));
        double normalizedValue = linearIntTaper.RealToNormalized(v);
        EXPECT_GE(normalizedValue, 0.);
        EXPECT_LE(normalizedValue, 1.);
        int32_t outV = linearIntTaper.NormalizedToReal(normalizedValue);
        EXPECT_EQ(v, outV);
    }
}

TEST_F(AAX_Migrated_TaperDelegate_Test, LinearSignedIntegerTaperReversed)
{
    const AAX_CLinearTaperDelegate<int32_t, 1000> linearIntRevTaper(18, -23);
    
    EXPECT_EQ(18, linearIntRevTaper.GetMinimumValue());
    EXPECT_EQ(-23, linearIntRevTaper.GetMaximumValue());
    EXPECT_EQ(-23, linearIntRevTaper.ConstrainRealValue(-50));
    EXPECT_EQ(18, linearIntRevTaper.ConstrainRealValue(100));
    
    for (int32_t v = -23; v <= 18; v += 1)
    {
        EXPECT_EQ(v, linearIntRevTaper.ConstrainRealValue(v));
        double normalizedValue = linearIntRevTaper.RealToNormalized(v);
        EXPECT_GE(normalizedValue, 0.);
        EXPECT_LE(normalizedValue, 1.);
        int32_t outV = linearIntRevTaper.NormalizedToReal(normalizedValue);
        EXPECT_EQ(v, outV);
    }
}

TEST_F(AAX_Migrated_TaperDelegate_Test, LinearSignedIntegerTaperEdgeCases)
{
    const AAX_CLinearTaperDelegate<int32_t> linearIntTaper(-23, 18);
    EXPECT_EQ(-23, linearIntTaper.NormalizedToReal(-10));  // input normalized out of bounds
    EXPECT_EQ(18, linearIntTaper.NormalizedToReal(100));   // input normalized out of bounds
    EXPECT_EQ(1.0f, linearIntTaper.RealToNormalized(1000)); // input real is out of bounds
    EXPECT_EQ(0.0f, linearIntTaper.RealToNormalized(-300)); // input real is out of bounds
}

TEST_F(AAX_Migrated_TaperDelegate_Test, LinearUnsignedIntegerTaper)
{
    const AAX_CLinearTaperDelegate<uint32_t> linearUIntTaper(5, 18);
    
    EXPECT_EQ(uint32_t(5), linearUIntTaper.GetMinimumValue());
    EXPECT_EQ(uint32_t(18), linearUIntTaper.GetMaximumValue());
    EXPECT_EQ(uint32_t(5), linearUIntTaper.ConstrainRealValue(2));
    EXPECT_EQ(uint32_t(18), linearUIntTaper.ConstrainRealValue(100));
    
    for (uint32_t v = 5; v <= 18; v += 1)
    {
        EXPECT_EQ(v, linearUIntTaper.ConstrainRealValue(v));
        double normalizedValue = linearUIntTaper.RealToNormalized(v);
        EXPECT_GE(normalizedValue, 0);
        EXPECT_LE(normalizedValue, 1);
        uint32_t outV = linearUIntTaper.NormalizedToReal(normalizedValue);
        EXPECT_EQ(v, outV);
    }
}

TEST_F(AAX_Migrated_TaperDelegate_Test, LinearUnsignedIntegerTaperEdgeCases)
{
    const AAX_CLinearTaperDelegate<uint32_t> linearUIntTaper(5, 18);
    EXPECT_EQ(uint32_t(5), linearUIntTaper.NormalizedToReal(0));    // input normalized out of bounds
    EXPECT_EQ(uint32_t(18), linearUIntTaper.NormalizedToReal(100)); // input normalized out of bounds
    EXPECT_EQ(1.0f, linearUIntTaper.RealToNormalized(1000));        // input real is out of bounds
    EXPECT_EQ(0.0f, linearUIntTaper.RealToNormalized(1));           // input real is out of bounds
}

//==============================================================================
// Linear Float Precision Tests
//==============================================================================

TEST_F(AAX_Migrated_TaperDelegate_Test, LinearFloatIntegerPrecision)
{
    AAX_CLinearTaperDelegate<float, 1> taper(-100, 100);
    double normValue = taper.RealToNormalized(10.1111f);
    float realValue = taper.NormalizedToReal(normValue);
    EXPECT_EQ(10, realValue);
}

TEST_F(AAX_Migrated_TaperDelegate_Test, LinearFloatTensPrecision)
{
    AAX_CLinearTaperDelegate<float, 10> taper(-100, 100);
    double normValue = taper.RealToNormalized(10.14f);
    float realValue = taper.NormalizedToReal(normValue);
    EXPECT_NEAR(10.1, realValue, 0.000001);
}

//==============================================================================
// Binary Taper Tests
//==============================================================================

TEST_F(AAX_Migrated_TaperDelegate_Test, BoolBinaryTaper)
{
    AAX_CBinaryTaperDelegate<bool> taper;
    double normValue = taper.RealToNormalized(true);
    EXPECT_EQ(1, normValue);
    normValue = taper.RealToNormalized(false);
    EXPECT_EQ(0, normValue);
    bool realValue = taper.NormalizedToReal(0);
    EXPECT_EQ(false, realValue);
    realValue = taper.NormalizedToReal(0.1);
    EXPECT_EQ(true, realValue);
    realValue = taper.NormalizedToReal(0.5);
    EXPECT_EQ(true, realValue);
    realValue = taper.NormalizedToReal(1.0);
    EXPECT_EQ(true, realValue);
    realValue = taper.NormalizedToReal(-1);  // out of bounds, but should still work
    EXPECT_EQ(false, realValue);
    realValue = taper.NormalizedToReal(100); // out of bounds, but should still work
    EXPECT_EQ(true, realValue);
}

//==============================================================================
// PieceWise Linear Taper Tests
//==============================================================================

TEST_F(AAX_Migrated_TaperDelegate_Test, FloatSimplePieceWiseLinear)
{
    const double normalized[] = {0.0, 1.0};
    const float real[] = {0.0f, 2.0f};
    AAX_CPieceWiseLinearTaperDelegate<float> taper(normalized, real, 2);

    EXPECT_EQ(0.0f, taper.GetMinimumValue());
    EXPECT_EQ(2.0f, taper.GetMaximumValue());
    EXPECT_EQ(0.0f, taper.ConstrainRealValue(-50));
    EXPECT_EQ(2.0f, taper.ConstrainRealValue(100));

    // Normalized To Real
    for (double v = 0.0f; v <= 1.0f; v += 0.1f)
    {
        float realValue = taper.NormalizedToReal(v);
        EXPECT_NEAR(2.0 * v, realValue, 0.0001);
    }

    // Real to Normalized
    for (float v = 0.0f; v <= 2.0f; v += 0.1f)
    {
        EXPECT_NEAR(v, taper.ConstrainRealValue(v), 0.001);
        double normalizedValue = taper.RealToNormalized(v);
        EXPECT_GE(normalizedValue, 0);
        EXPECT_LE(normalizedValue, 1);
        EXPECT_NEAR(.5f * v, normalizedValue, 0.001);
    }
}

TEST_F(AAX_Migrated_TaperDelegate_Test, FloatComplexPieceWiseLinear)
{
    const double normalized[] = {0.0, 0.25, 0.5, 1.0};
    const float real[] = {-10.0f, 0.0f, 4.0f, 9.0f};
    AAX_CPieceWiseLinearTaperDelegate<float> taper(normalized, real, 4);

    EXPECT_EQ(-10.0f, taper.GetMinimumValue());
    EXPECT_EQ(9.0f, taper.GetMaximumValue());
    EXPECT_EQ(-10.0f, taper.ConstrainRealValue(-50));
    EXPECT_EQ(9.0f, taper.ConstrainRealValue(100));

    // Normalized To Real
    EXPECT_NEAR(-10.0f, taper.NormalizedToReal(-0.5), 0.1);  // outside of range should be clamped
    EXPECT_NEAR(-10.0f, taper.NormalizedToReal(0.0), 0.1);
    EXPECT_NEAR(-5.0f, taper.NormalizedToReal(0.125), 0.1);
    EXPECT_NEAR(2.0f, taper.NormalizedToReal(0.375), 0.1);
    EXPECT_NEAR(6.5f, taper.NormalizedToReal(0.75), 0.1);
    EXPECT_NEAR(9.0f, taper.NormalizedToReal(1.0), 0.1);
    EXPECT_NEAR(9.0f, taper.NormalizedToReal(2.0), 0.1);     // outside of range should be clamped

    // Real to Normalized
    EXPECT_NEAR(0.0, taper.RealToNormalized(-20.0f), 0.0001); // outside of range should be clamped
    EXPECT_NEAR(0.0, taper.RealToNormalized(-10.0f), 0.0001);
    EXPECT_NEAR(0.125, taper.RealToNormalized(-5.0f), 0.0001);
    EXPECT_NEAR(0.375, taper.RealToNormalized(2.0f), 0.0001);
    EXPECT_NEAR(0.75, taper.RealToNormalized(6.5f), 0.0001);
    EXPECT_NEAR(1.0, taper.RealToNormalized(9.0f), 0.0001);
    EXPECT_NEAR(1.0, taper.RealToNormalized(20.0f), 0.0001);  // outside of range should be clamped
}

TEST_F(AAX_Migrated_TaperDelegate_Test, FloatInvertedComplexPieceWiseLinear)
{
    const double normalized[] = {0.0, 0.25, 0.5, 1.0};
    const float real[] = {10.0f, 0.0f, -4.0f, -9.0f};
    AAX_CPieceWiseLinearTaperDelegate<float> taper(normalized, real, 4);

    EXPECT_EQ(-9.0f, taper.GetMinimumValue());
    EXPECT_EQ(10.0f, taper.GetMaximumValue());
    EXPECT_EQ(-9.0f, taper.ConstrainRealValue(-50));
    EXPECT_EQ(10.0f, taper.ConstrainRealValue(100));

    // Normalized To Real
    EXPECT_NEAR(10.0f, taper.NormalizedToReal(-0.5), 0.0001);  // outside of range should be clamped
    EXPECT_NEAR(10.0f, taper.NormalizedToReal(0.0), 0.0001);
    EXPECT_NEAR(5.0f, taper.NormalizedToReal(0.125), 0.0001);
    EXPECT_NEAR(-2.0f, taper.NormalizedToReal(0.375), 0.0001);
    EXPECT_NEAR(-6.5f, taper.NormalizedToReal(0.75), 0.0001);
    EXPECT_NEAR(-9.0f, taper.NormalizedToReal(1.0), 0.0001);
    EXPECT_NEAR(-9.0f, taper.NormalizedToReal(2.0), 0.0001);   // outside of range should be clamped

    // Real to Normalized
    EXPECT_NEAR(0.0, taper.RealToNormalized(20.0f), 0.0001);  // outside of range should be clamped
    EXPECT_NEAR(0.0, taper.RealToNormalized(10.0f), 0.0001);
    EXPECT_NEAR(0.125, taper.RealToNormalized(5.0f), 0.0001);
    EXPECT_NEAR(0.375, taper.RealToNormalized(-2.0f), 0.0001);
    EXPECT_NEAR(0.75, taper.RealToNormalized(-6.5f), 0.0001);
    EXPECT_NEAR(1.0, taper.RealToNormalized(-9.0f), 0.0001);
    EXPECT_NEAR(1.0, taper.RealToNormalized(-20.0f), 0.0001);  // outside of range should be clamped
}

//==============================================================================
// Range Taper Tests
//==============================================================================

TEST_F(AAX_Migrated_TaperDelegate_Test, RangeTaperDelegateFloat)
{
    float rangePoints[] = { 0.0, 1.0, 100.0, 1000.0, 2000.0 };
    double rangeSteps[] = { 0.1, 1.0, 10.0, 25.0 }; // number of steps per range: 10, 99, 90, 40
    const long cNumRanges = sizeof(rangeSteps)/sizeof(rangeSteps[0]);
    EXPECT_EQ(4, cNumRanges);

    double numSteps = 0;
    for (int i = 0; i < cNumRanges; i++)
    {
        numSteps += (rangePoints[i+1] - rangePoints[i]) / rangeSteps[i];
    }
    EXPECT_EQ(239, numSteps); // (10 + 99 + 90 + 40)

    AAX_CRangeTaperDelegate<float> nonLinearTaper(rangePoints, rangeSteps, cNumRanges);

    EXPECT_EQ(0, nonLinearTaper.GetMinimumValue());
    EXPECT_EQ(2000, nonLinearTaper.GetMaximumValue());
    EXPECT_EQ(0, nonLinearTaper.ConstrainRealValue(-50));
    EXPECT_EQ(2000, nonLinearTaper.ConstrainRealValue(2500));

    float controlValue = 21.5;
    double normalized = nonLinearTaper.RealToNormalized(controlValue);
    EXPECT_EQ(0.12761506276150628, normalized);
    float real = nonLinearTaper.NormalizedToReal(normalized);
    EXPECT_EQ(22, real);

    for (float v = 0.0f; v <= 0.6f; v += 0.1f)
    {
        EXPECT_EQ(v, nonLinearTaper.ConstrainRealValue(v));
        double normalizedValue = nonLinearTaper.RealToNormalized(v);
        EXPECT_GE(normalizedValue, 0);
        EXPECT_LE(normalizedValue, 1);
        float outV = nonLinearTaper.NormalizedToReal(normalizedValue);
        EXPECT_EQ(v, outV);
    }

    for (uint32_t v = 5; v <= 18; v += 1)
    {
        EXPECT_EQ(v, nonLinearTaper.ConstrainRealValue(v));
        double normalizedValue = nonLinearTaper.RealToNormalized(v);
        EXPECT_GE(normalizedValue, 0);
        EXPECT_LE(normalizedValue, 1);
        uint32_t outV = static_cast<uint32_t>(nonLinearTaper.NormalizedToReal(normalizedValue));
        EXPECT_EQ(v, outV);
    }

    for (float v = 100.0f; v <= 1000.0f; v += 10.0f)
    {
        EXPECT_EQ(v, nonLinearTaper.ConstrainRealValue(v));
        double normalizedValue = nonLinearTaper.RealToNormalized(v);
        EXPECT_GE(normalizedValue, 0);
        EXPECT_LE(normalizedValue, 1);
        uint32_t outV = static_cast<uint32_t>(nonLinearTaper.NormalizedToReal(normalizedValue));
        EXPECT_EQ(v, outV);
    }
}

TEST_F(AAX_Migrated_TaperDelegate_Test, RangeTaperDelegateInt)
{
    int32_t minValue = 10;
    int32_t middleValue = 1000;
    int32_t maxValue = 2000;
    double lowRangeStepSize = 1.0;
    double highRangeStepSize = 10.0;

    int32_t rangePoints[] = { minValue, middleValue, maxValue };
    double rangeSteps[] = { lowRangeStepSize, highRangeStepSize }; // number of steps per range: 990, 100

    const int32_t cNumRanges = sizeof(rangeSteps)/sizeof(rangeSteps[0]);
    EXPECT_EQ(2, cNumRanges);

    uint32_t numSteps = 0;
    for (int32_t i = 0; i < cNumRanges; i++)
    {
        numSteps += uint32_t((rangePoints[i+1] - rangePoints[i]) / rangeSteps[i]);
    }
    uint32_t expectedNumSteps = 1090;
    EXPECT_EQ(expectedNumSteps, numSteps); // (990 + 100)

    AAX_CRangeTaperDelegate<int32_t, 0> taperDelegate {rangePoints, rangeSteps, cNumRanges};

    EXPECT_EQ(minValue, taperDelegate.GetMinimumValue());
    EXPECT_EQ(maxValue, taperDelegate.GetMaximumValue());
    EXPECT_EQ(minValue, taperDelegate.ConstrainRealValue(-50));
    EXPECT_EQ(maxValue, taperDelegate.ConstrainRealValue(2500));

    int controlValue = 22;
    double normalized = taperDelegate.RealToNormalized(controlValue);
    EXPECT_EQ(0.011009174311926606, normalized);
    float real = taperDelegate.NormalizedToReal(normalized);
    EXPECT_EQ(22, real);

    for (int v = 10; v <= 15; v += 1)
    {
        EXPECT_EQ(v, taperDelegate.ConstrainRealValue(v));
        double normalizedValue = taperDelegate.RealToNormalized(v);
        EXPECT_GE(normalizedValue, 0);
        EXPECT_LE(normalizedValue, 1);
        float outV = taperDelegate.NormalizedToReal(normalizedValue);
        EXPECT_EQ(v, outV);
    }

    for (int v = 997; v <= 1000; v += 1)
    {
        EXPECT_EQ(v, taperDelegate.ConstrainRealValue(v));
        double normalizedValue = taperDelegate.RealToNormalized(v);
        EXPECT_GE(normalizedValue, 0);
        EXPECT_LE(normalizedValue, 1);
        float outV = taperDelegate.NormalizedToReal(normalizedValue);
        EXPECT_EQ(v, outV);
    }

    for (int v = 1010; v <= 1080; v += 10)
    {
        EXPECT_EQ(v, taperDelegate.ConstrainRealValue(v));
        double normalizedValue = taperDelegate.RealToNormalized(v);
        EXPECT_GE(normalizedValue, 0);
        EXPECT_LE(normalizedValue, 1);
        float outV = taperDelegate.NormalizedToReal(normalizedValue);
        EXPECT_EQ(v, outV);
    }

    double normalizedValue2 = taperDelegate.RealToNormalized(1003);
    float outV2 = taperDelegate.NormalizedToReal(normalizedValue2);
    EXPECT_EQ(1000, outV2); // 1003 -> 1000

    double normalizedValue3 = taperDelegate.RealToNormalized(1006);
    float outV3 = taperDelegate.NormalizedToReal(normalizedValue3);
    EXPECT_EQ(1010, outV3); // 1006 -> 1010
}
