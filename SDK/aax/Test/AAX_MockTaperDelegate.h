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
 *	\file AAX_MockTaperDelegate.h
 *
 *	\brief Simple mock taper delegate for unit testing that provides 1:1 mapping between normalized and real values (no actual taper conversion)
 *
 */
/*================================================================================================*/


#ifndef AAX_MOCKTAPERDELEGATE_H
#define AAX_MOCKTAPERDELEGATE_H

#include "AAX_ITaperDelegate.h"
#include "AAX.h"
#include <cstdint>

/** \brief Simple mock taper delegate for unit testing
 *
 *  This taper delegate provides a 1:1 mapping between normalized and real values,
 *  making it ideal for unit testing where you want predictable parameter behavior
 *  without complex taper mathematics.
 *
 *  - Normalized values (0.0 to 1.0) map directly to real values (minValue to maxValue)
 *  - No precision rounding or complex conversions
 *  - Suitable for testing parameter functionality without taper complexity
 */
template <typename T>
class AAX_MockTaperDelegate : public AAX_ITaperDelegate<T>
{
public:
    /** \brief Constructs a mock taper with specified minimum and maximum values
     *
     *  \param[in] minValue  Minimum real value (maps to normalized 0.0)
     *  \param[in] maxValue  Maximum real value (maps to normalized 1.0)
     */
    AAX_MockTaperDelegate(T minValue, T maxValue);
    
    // Virtual AAX_ITaperDelegate overrides
    AAX_ITaperDelegate<T>* Clone() const override;
    T GetMinimumValue() const override { return mMinValue; }
    T GetMaximumValue() const override { return mMaxValue; }
    T ConstrainRealValue(T value) const override;
    T NormalizedToReal(double normalizedValue) const override;
    double RealToNormalized(T realValue) const override;

private:
    T mMinValue;
    T mMaxValue;
};

// Template implementation

template <typename T>
AAX_MockTaperDelegate<T>::AAX_MockTaperDelegate(T minValue, T maxValue)
    : mMinValue(minValue)
    , mMaxValue(maxValue)
{
}

template <typename T>
AAX_ITaperDelegate<T>* AAX_MockTaperDelegate<T>::Clone() const
{
    return new AAX_MockTaperDelegate<T>(mMinValue, mMaxValue);
}

template <typename T>
T AAX_MockTaperDelegate<T>::ConstrainRealValue(T value) const
{
    if (value < mMinValue) return mMinValue;
    if (value > mMaxValue) return mMaxValue;
    return value;
}

template <typename T>
T AAX_MockTaperDelegate<T>::NormalizedToReal(double normalizedValue) const
{
    // Clamp normalized value to [0.0, 1.0]
    if (normalizedValue < 0.0) normalizedValue = 0.0;
    if (normalizedValue > 1.0) normalizedValue = 1.0;
    
    // Linear interpolation: real = min + (max - min) * normalized
    double range = static_cast<double>(mMaxValue - mMinValue);
    T result = static_cast<T>(mMinValue + range * normalizedValue);
    
    return ConstrainRealValue(result);
}

template <typename T>
double AAX_MockTaperDelegate<T>::RealToNormalized(T realValue) const
{
    // Constrain the real value first
    T constrainedValue = ConstrainRealValue(realValue);
    
    // Handle edge case where min == max
    if (mMaxValue == mMinValue) {
        return 0.0;
    }
    
    // Linear interpolation: normalized = (real - min) / (max - min)
    double range = static_cast<double>(mMaxValue - mMinValue);
    double normalized = static_cast<double>(constrainedValue - mMinValue) / range;
    
    // Clamp result to [0.0, 1.0] (should already be in range, but be safe)
    if (normalized < 0.0) return 0.0;
    if (normalized > 1.0) return 1.0;
    
    return normalized;
}

#endif // AAX_MOCKTAPERDELEGATE_H
