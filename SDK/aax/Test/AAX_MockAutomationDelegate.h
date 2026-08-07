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
 *	\file AAX_MockAutomationDelegate.h
 *
 *	\brief Mock automation delegate for unit testing
 *
 */
/*================================================================================================*/


#ifndef AAX_MOCKAUTOMATIONDELEGATE_H
#define AAX_MOCKAUTOMATIONDELEGATE_H

#include "AAX_IAutomationDelegate.h"
#include <string>

//==============================================================================
// AAX_MockAutomationDelegate
//==============================================================================

/**
 * \brief Mock automation delegate with call tracking for unit testing
 *
 * This class provides a mock implementation of AAX_IAutomationDelegate that
 * tracks method calls for verification in unit tests. It records:
 * - Number of times each method is called
 * - Parameter IDs passed to key methods
 * - Provides stub implementations that return success
 */
class AAX_MockAutomationDelegate : public AAX_IAutomationDelegate
{
public:
    AAX_MockAutomationDelegate() 
        : mRegisterParameterCallCount(0)
        , mUnregisterParameterCallCount(0)
        , mParameterNameChangedCallCount(0)
    {}
    
    // Implementation with call tracking
    AAX_Result RegisterParameter(AAX_CParamID iParameterID) override
    {
        mRegisterParameterCallCount++;
        mLastRegisteredParameterID = iParameterID ? iParameterID : "";
        return AAX_SUCCESS;
    }

    AAX_Result UnregisterParameter(AAX_CParamID iParameterID) override
    {
        mUnregisterParameterCallCount++;
        mLastUnregisteredParameterID = iParameterID ? iParameterID : "";
        return AAX_SUCCESS;
    }

    AAX_Result PostTouchRequest(AAX_CParamID /*iParameterID*/) override { return AAX_SUCCESS; }
    AAX_Result PostReleaseRequest(AAX_CParamID /*iParameterID*/) override { return AAX_SUCCESS; }
    AAX_Result PostSetValueRequest(AAX_CParamID /*iParameterID*/, double /*normalizedValue*/) const override { return AAX_SUCCESS; }
    AAX_Result PostCurrentValue(AAX_CParamID /*iParameterID*/, double /*normalizedValue*/) const override { return AAX_SUCCESS; }
    AAX_Result GetTouchState(AAX_CParamID /*iParameterID*/, AAX_CBoolean* oTouched) override
    {
        if (oTouched) *oTouched = false;
        return AAX_SUCCESS;
    }
    AAX_Result ParameterNameChanged(AAX_CParamID iParameterID) override
    {
        mParameterNameChangedCallCount++;
        mLastParameterNameChangedID = iParameterID ? iParameterID : "";
        return AAX_SUCCESS;
    }
    
    // Test tracking members
    int mRegisterParameterCallCount;
    int mUnregisterParameterCallCount;
    int mParameterNameChangedCallCount;
    std::string mLastRegisteredParameterID;
    std::string mLastUnregisteredParameterID;
    std::string mLastParameterNameChangedID;
};

#endif // AAX_MOCKAUTOMATIONDELEGATE_H
