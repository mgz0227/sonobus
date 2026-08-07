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
 *	\file AAX_MockEffectParameters.h
 *
 *	\brief Mock effect parameters implementation for unit testing
 *
 */ 
/*================================================================================================*/


#ifndef AAX_MOCKEFFECTPARAMETERS_H
#define AAX_MOCKEFFECTPARAMETERS_H

#include "AAX_IEffectParameters.h"
#include "AAX.h"
#include <cstdint>

/** \brief A mock effect parameters implementation for unit testing
 *
 *  Specific unit tests may override this class to provide test-specific behavior
 */
class AAX_MockEffectParameters : public AAX_IEffectParameters
{
public:
    AAX_MockEffectParameters() = default;
    ~AAX_MockEffectParameters() override = default;

    // From AAX_IACFEffectParameters_V1
    AAX_Result Initialize(IACFUnknown* iController) override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result Uninitialize() override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result NotificationReceived(AAX_CTypeID inNotificationType, const void* inNotificationData, uint32_t inNotificationDataSize) override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetNumberOfParameters(int32_t* oNumControls) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetMasterBypassParameter(AAX_IString* oIDString) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetParameterIsAutomatable(AAX_CParamID iParameterID, AAX_CBoolean* oAutomatable) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetParameterNumberOfSteps(AAX_CParamID iParameterID, int32_t* oNumSteps) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetParameterName(AAX_CParamID iParameterID, AAX_IString* oName) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetParameterNameOfLength(AAX_CParamID iParameterID, AAX_IString* oName, int32_t iNameLength) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetParameterDefaultNormalizedValue(AAX_CParamID iParameterID, double* oValue) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result SetParameterDefaultNormalizedValue(AAX_CParamID iParameterID, double iValue) override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetParameterType(AAX_CParamID iParameterID, AAX_EParameterType* oParameterType) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetParameterOrientation(AAX_CParamID iParameterID, AAX_EParameterOrientation* oParameterOrientation) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetParameter(AAX_CParamID iParameterID, AAX_IParameter** oParameter) override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetParameterIndex(AAX_CParamID iParameterID, int32_t* oControlIndex) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetParameterIDFromIndex(int32_t iControlIndex, AAX_IString* oParameterIDString) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetParameterValueInfo(AAX_CParamID iParameterID, int32_t iSelector, int32_t* oValue) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetParameterValueFromString(AAX_CParamID iParameterID, double* oValue, const AAX_IString& iValueString) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetParameterStringFromValue(AAX_CParamID iParameterID, double iValue, AAX_IString* oValueString, int32_t iMaxLength) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetParameterValueString(AAX_CParamID iParameterID, AAX_IString* oValueString, int32_t iMaxLength) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result SetParameterNormalizedRelative(AAX_CParamID iParameterID, double iValue) override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result TouchParameter(AAX_CParamID iParameterID) override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result ReleaseParameter(AAX_CParamID iParameterID) override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result UpdateParameterTouch(AAX_CParamID iParameterID, AAX_CBoolean iTouchState) override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result UpdateParameterNormalizedRelative(AAX_CParamID iParameterID, double iValue) override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GenerateCoefficients() override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result ResetFieldData(AAX_CFieldIndex inFieldIndex, void* oData, uint32_t inDataSize) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetNumberOfChunks(int32_t* oNumChunks) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetChunkIDFromIndex(int32_t iIndex, AAX_CTypeID* oChunkID) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetChunkSize(AAX_CTypeID iChunkID, uint32_t* oSize) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetChunk(AAX_CTypeID iChunkID, AAX_SPlugInChunk* oChunk) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result SetChunk(AAX_CTypeID iChunkID, const AAX_SPlugInChunk* iChunk) override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result CompareActiveChunk(const AAX_SPlugInChunk* iChunkP, AAX_CBoolean* oIsEqual) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetNumberOfChanges(int32_t* oNumChanges) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result TimerWakeup() override { return AAX_ERROR_UNIMPLEMENTED; }

    // From AAX_IACFEffectParameters_V2
    AAX_Result GetCurveData(AAX_CTypeID iCurveType, const float* iValues, uint32_t iNumValues, float* oValues) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetCustomData(AAX_CTypeID iDataBlockID, uint32_t inDataSize, void* oData, uint32_t* oDataWritten) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result SetCustomData(AAX_CTypeID iDataBlockID, uint32_t inDataSize, const void* iData) override { return AAX_ERROR_UNIMPLEMENTED; }

    // From AAX_IACFEffectParameters_V3
    AAX_Result DoMIDITransfers() override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result RenderAudio_Hybrid(AAX_SHybridRenderInfo* ioRenderInfo) override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result UpdateMIDINodes(AAX_CFieldIndex inFieldIndex, AAX_CMidiPacket& iPacket) override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result UpdateControlMIDINodes(AAX_CTypeID nodeID, AAX_CMidiPacket& iPacket) override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetCurveDataMeterIds(AAX_CTypeID iCurveType, uint32_t* oXMeterId, uint32_t* oYMeterId) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetCurveDataDisplayRange(AAX_CTypeID iCurveType, float* oXMin, float* oXMax, float* oYMin, float* oYMax) const override { return AAX_ERROR_UNIMPLEMENTED; }

    // From AAX_IACFEffectParameters_V4
    AAX_Result UpdatePageTable(uint32_t inTableType, int32_t inTablePageSize, IACFUnknown* iHostUnknown, IACFUnknown* ioPageTableUnknown) const override { return AAX_ERROR_UNIMPLEMENTED; }

    // From AAX_IACFEffectParameters_V5
    AAX_Result GetParameterAffectsState(AAX_CParamID iParameterID, AAX_CTypeID iStateType, void const* iQueryData, AAX_CBoolean* oAffectsState) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetParameterWithRole(AAX_CTypeID iParameterRole, void const* iQueryData, AAX_CBoolean* oHasParameter, AAX_IString* oParameterIDString) const override { return AAX_ERROR_UNIMPLEMENTED; }

    // From AAX_IACFEffectParameters_V6
	AAX_Result HandleQueryMessage(
		AAX_CTypeID iMessageType,
		uint32_t iMessageDataSize,
		void const * iMessageData,
		uint32_t iResponseDataSize,
		void * oResponseData,
		uint32_t * oResponseDataWritten) const override { return AAX_ERROR_UNIMPLEMENTED; }

    AAX_Result GetParameterDefaultAutomationEnabledState(AAX_CParamID iParameterID, int32_t* oAutomationEnabledState) const override { return AAX_ERROR_UNIMPLEMENTED; }
};





#endif // AAX_MOCKEFFECTPARAMETERS_H
