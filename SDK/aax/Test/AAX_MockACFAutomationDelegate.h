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
 *	\file AAX_MockACFAutomationDelegate.h
 *
 *	\brief Mock ACF automation delegate implementation for unit testing
 *
 */ 
/*================================================================================================*/


#ifndef AAX_MOCKACFAUTOMATIONDELEGATE_H
#define AAX_MOCKACFAUTOMATIONDELEGATE_H

#include "AAX_IACFAutomationDelegate.h"
#include "AAX_UIDs.h"
#include "CACFUnknown.h"

/** \brief Mock ACF automation delegate implementation for unit testing
 *
 *  Provides stubbed implementations of all AAX_IACFAutomationDelegate methods
 */
class AAX_MockACFAutomationDelegate : public AAX_IACFAutomationDelegate, public CACFUnknown
{
public:
    ACF_DECLARE_STANDARD_UNKNOWN()
    ACFMETHOD(InternalQueryInterface)(const acfIID& riid, void** ppvObjOut)
    {
        if (memcmp(&riid, &IID_IAAXAutomationDelegateV1, sizeof(acfIID)) == 0)
        {
            *ppvObjOut = static_cast<AAX_IACFAutomationDelegate*>(this);
            AddRef();
            return ACF_OK;
        }
        return CACFUnknown::InternalQueryInterface(riid, ppvObjOut);
    }

    // AAX_IACFAutomationDelegate interface - stubbed implementations
    AAX_Result RegisterParameter(AAX_CParamID iParameterID) override
    {
        return AAX_SUCCESS; // Allow parameter registration
    }

    AAX_Result UnregisterParameter(AAX_CParamID iParameterID) override
    {
        return AAX_SUCCESS; // Allow parameter unregistration
    }

    AAX_Result PostSetValueRequest(AAX_CParamID iParameterID, double normalizedValue) const override
    {
        return AAX_SUCCESS; // Allow value requests
    }

    AAX_Result PostCurrentValue(AAX_CParamID iParameterID, double normalizedValue) const override
    {
        return AAX_SUCCESS; // Allow current value posting
    }

    AAX_Result PostTouchRequest(AAX_CParamID iParameterID) override
    {
        return AAX_SUCCESS; // Allow touch requests
    }

    AAX_Result PostReleaseRequest(AAX_CParamID iParameterID) override
    {
        return AAX_SUCCESS; // Allow release requests
    }

    AAX_Result GetTouchState(AAX_CParamID iParameterID, AAX_CBoolean* oTouched) override
    {
        if (oTouched) *oTouched = 0; // Default to not touched
        return AAX_SUCCESS;
    }


};

#endif // AAX_MOCKACFAUTOMATIONDELEGATE_H
