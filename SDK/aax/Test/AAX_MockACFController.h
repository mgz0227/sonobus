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
 *	\file AAX_MockACFController.h
 *
 *	\brief Mock ACF controller implementation for unit testing
 *
 */ 
/*================================================================================================*/


#ifndef AAX_MOCKACFCONTROLLER_H
#define AAX_MOCKACFCONTROLLER_H

#include "AAX_IACFController.h"
#include "AAX_UIDs.h"
#include "CACFUnknown.h"

/** \brief Mock ACF controller implementation for unit testing
 *
 *  Provides stubbed implementations of all AAX_IACFController methods
 */
class AAX_MockACFController : public AAX_IACFController, public CACFUnknown
{
public:
    ACF_DECLARE_STANDARD_UNKNOWN()
    ACFMETHOD(InternalQueryInterface)(const acfIID& riid, void** ppvObjOut)
    {
        if (memcmp(&riid, &IID_IAAXControllerV1, sizeof(acfIID)) == 0)
        {
            *ppvObjOut = static_cast<AAX_IACFController*>(this);
            AddRef();
            return ACF_OK;
        }
        return CACFUnknown::InternalQueryInterface(riid, ppvObjOut);
    }

    // AAX_IACFController interface - stubbed implementations
    AAX_Result GetEffectID(AAX_IString* outEffectID) const override
    {
        return AAX_ERROR_UNIMPLEMENTED;
    }

    AAX_Result GetSampleRate(AAX_CSampleRate* outSampleRate) const override
    {
        if (outSampleRate) *outSampleRate = 44100.0; // Default sample rate
        return AAX_SUCCESS;
    }

    AAX_Result GetInputStemFormat(AAX_EStemFormat* outStemFormat) const override
    {
        return AAX_ERROR_UNIMPLEMENTED;
    }

    AAX_Result GetOutputStemFormat(AAX_EStemFormat* outStemFormat) const override
    {
        return AAX_ERROR_UNIMPLEMENTED;
    }

    AAX_Result GetSignalLatency(int32_t* outSamples) const override
    {
        return AAX_ERROR_UNIMPLEMENTED;
    }

    AAX_Result GetCycleCount(AAX_EProperty inWhichCycleCount, AAX_CPropertyValue* outNumCycles) const override
    {
        return AAX_ERROR_UNIMPLEMENTED;
    }

    AAX_Result GetTODLocation(AAX_CTimeOfDay* outTODLocation) const override
    {
        return AAX_ERROR_UNIMPLEMENTED;
    }

    AAX_Result SetSignalLatency(int32_t inNumSamples) override
    {
        return AAX_SUCCESS;
    }

    AAX_Result SetCycleCount(AAX_EProperty* inWhichCycleCounts, AAX_CPropertyValue* iValues, int32_t numValues) override
    {
        return AAX_SUCCESS;
    }

    AAX_Result PostPacket(AAX_CFieldIndex inFieldIndex, const void* inPayloadP, uint32_t inPayloadSize) override
    {
        return AAX_SUCCESS; // Allow packet posting to succeed
    }

    AAX_Result GetCurrentMeterValue(AAX_CTypeID inMeterID, float* outMeterValue) const override
    {
        return AAX_ERROR_UNIMPLEMENTED;
    }

    AAX_Result GetMeterPeakValue(AAX_CTypeID inMeterID, float* outMeterPeakValue) const override
    {
        return AAX_ERROR_UNIMPLEMENTED;
    }

    AAX_Result ClearMeterPeakValue(AAX_CTypeID inMeterID) const override
    {
        return AAX_ERROR_UNIMPLEMENTED;
    }

    AAX_Result GetMeterClipped(AAX_CTypeID inMeterID, AAX_CBoolean* outClipped) const override
    {
        return AAX_ERROR_UNIMPLEMENTED;
    }

    AAX_Result ClearMeterClipped(AAX_CTypeID inMeterID) const override
    {
        return AAX_ERROR_UNIMPLEMENTED;
    }

    AAX_Result GetMeterCount(uint32_t* outMeterCount) const override
    {
        return AAX_ERROR_UNIMPLEMENTED;
    }

    AAX_Result GetNextMIDIPacket(AAX_CFieldIndex* outPort, AAX_CMidiPacket* outPacket) override
    {
        return AAX_ERROR_UNIMPLEMENTED;
    }


};

#endif // AAX_MOCKACFCONTROLLER_H
