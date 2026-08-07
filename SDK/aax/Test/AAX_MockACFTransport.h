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
 *	\file AAX_MockACFTransport.h
 *
 *	\brief Mock ACF transport implementation for unit testing
 *
 */ 
/*================================================================================================*/


#ifndef AAX_MOCKACFTRANSPORT_H
#define AAX_MOCKACFTRANSPORT_H

#include "AAX_IACFTransport.h"
#include "AAX_UIDs.h"
#include "CACFUnknown.h"

/** \brief Mock ACF transport implementation for unit testing
 *
 *  Provides stubbed implementations of all AAX_IACFTransport methods
 */
class AAX_MockACFTransport : public AAX_IACFTransport, public CACFUnknown
{
public:
    ACF_DECLARE_STANDARD_UNKNOWN()
    ACFMETHOD(InternalQueryInterface)(const acfIID& riid, void** ppvObjOut)
    {
        if (memcmp(&riid, &IID_IAAXTransportV1, sizeof(acfIID)) == 0)
        {
            *ppvObjOut = static_cast<AAX_IACFTransport*>(this);
            AddRef();
            return ACF_OK;
        }
        return CACFUnknown::InternalQueryInterface(riid, ppvObjOut);
    }

    // AAX_IACFTransport interface - stubbed implementations
    AAX_Result GetCurrentTempo(double* TempoBPM) const override
    {
        if (TempoBPM) *TempoBPM = 120.0; // Default tempo
        return AAX_SUCCESS;
    }

    AAX_Result GetCurrentMeter(int32_t* MeterNumerator, int32_t* MeterDenominator) const override
    {
        if (MeterNumerator) *MeterNumerator = 4; // Default 4/4 time
        if (MeterDenominator) *MeterDenominator = 4;
        return AAX_SUCCESS;
    }

    AAX_Result IsTransportPlaying(bool* isPlaying) const override
    {
        if (isPlaying) *isPlaying = false; // Default to stopped
        return AAX_SUCCESS;
    }

    AAX_Result GetCurrentTickPosition(int64_t* TickPosition) const override
    {
        if (TickPosition) *TickPosition = 0; // Default position
        return AAX_SUCCESS;
    }

    AAX_Result GetCurrentLoopPosition(bool* bLooping, int64_t* LoopStartTick, int64_t* LoopEndTick) const override
    {
        if (bLooping) *bLooping = false; // Default to not looping
        if (LoopStartTick) *LoopStartTick = 0;
        if (LoopEndTick) *LoopEndTick = 0;
        return AAX_SUCCESS;
    }

    AAX_Result GetCurrentNativeSampleLocation(int64_t* SampleLocation) const override
    {
        if (SampleLocation) *SampleLocation = 0; // Default sample location
        return AAX_SUCCESS;
    }

    AAX_Result GetCustomTickPosition(int64_t* oTickPosition, int64_t iSampleLocation) const override
    {
        if (oTickPosition) *oTickPosition = 0; // Default position
        return AAX_SUCCESS;
    }

    AAX_Result GetBarBeatPosition(int32_t* Bars, int32_t* Beats, int64_t* DisplayTicks, int64_t SampleLocation) const override
    {
        if (Bars) *Bars = 1; // Default bar 1
        if (Beats) *Beats = 1; // Default beat 1
        if (DisplayTicks) *DisplayTicks = 0;
        return AAX_SUCCESS;
    }

    AAX_Result GetTicksPerQuarter(uint32_t* ticks) const override
    {
        if (ticks) *ticks = 960; // Default ticks per quarter
        return AAX_SUCCESS;
    }

    AAX_Result GetCurrentTicksPerBeat(uint32_t* ticks) const override
    {
        if (ticks) *ticks = 960; // Default ticks per beat
        return AAX_SUCCESS;
    }


};

#endif // AAX_MOCKACFTRANSPORT_H
