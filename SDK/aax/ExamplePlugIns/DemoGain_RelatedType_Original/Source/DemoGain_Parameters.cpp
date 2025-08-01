/*================================================================================================*/
/*
 *	Copyright 2009-2015, 2018, 2023-2025 Avid Technology, Inc.
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
 */
/*================================================================================================*/ 

// Self Include
#include "DemoGain_Parameters.h"

// DemoGain Includes
#include "DemoGain_Defs.h"
#include "DemoGain_Alg.h"

// AAX Includes
#include "AAX_CBinaryTaperDelegate.h"
#include "AAX_CBinaryDisplayDelegate.h"
#include "AAX_CLinearTaperDelegate.h"
#include "AAX_CNumberDisplayDelegate.h"
#include "AAX_Assert.h"

// Standard Includes
#include <memory>



// *******************************************************************************
// ROUTINE:	Create
// *******************************************************************************
AAX_CEffectParameters *AAX_CALLBACK DemoGain_Parameters::Create()
{
	return new DemoGain_Parameters();
}

// *******************************************************************************
// METHOD:	DemoGain_Parameters
// *******************************************************************************
DemoGain_Parameters::DemoGain_Parameters () :
	AAX_CEffectParameters()
{
}

// *******************************************************************************
// METHOD:	EffectInit
// *******************************************************************************
AAX_Result DemoGain_Parameters::EffectInit()
{	
	// bypass
	{
		AAX_CString id = cDefaultMasterBypassID;
		std::unique_ptr<AAX_IParameter> param (new AAX_CParameter<bool>(
			id, AAX_CString("Master Bypass"), false,
			AAX_CBinaryTaperDelegate<bool>(),
			AAX_CBinaryDisplayDelegate<bool>("bypass", "on"), true));
		param->SetNumberOfSteps( 2 );
		param->SetType( AAX_eParameterType_Discrete );
		mParameterManager.AddParameter(param.release());
		mPacketDispatcher.RegisterPacket(id.CString(), eAlgPortID_BypassIn);
	}
	
	// gain
	{
		AAX_CString id = DemoGain_GainID;
		std::unique_ptr<AAX_IParameter> param (new AAX_CParameter<float>(
			id, AAX_CString("Gain"), 1.0f,
			AAX_CLinearTaperDelegate<float>(0.0f, 1.0f),
			AAX_CNumberDisplayDelegate<float>(), true));
		param->SetNumberOfSteps(55);
		param->SetType( AAX_eParameterType_Continuous );
		mParameterManager.AddParameter(param.release());
		mPacketDispatcher.RegisterPacket(id.CString(), eAlgPortID_CoefsGainIn, this, &DemoGain_Parameters::UpdatePacket_Gain);
	}
	
	return AAX_SUCCESS;
}

// *******************************************************************************
// METHOD:	UpdatePacketGain
// *******************************************************************************
AAX_Result DemoGain_Parameters::UpdatePacket_Gain( AAX_CPacket& ioPacket )
{
	AAX_IParameter*	 parameter = mParameterManager.GetParameterByID( DemoGain_GainID );
	if (parameter)
	{
		float floatValue;
		if (parameter->GetValueAsFloat(&floatValue))
		{
			SDemoGain_CoefsGain&  packet = *ioPacket.GetPtr<SDemoGain_CoefsGain>();
			
			// Populate packet
			packet.mGain = floatValue;
		}
	}
	return AAX_SUCCESS;
}



// *******************************************************************************
// DATA PERSISTENCE METHODS
// *******************************************************************************

constexpr AAX_CTypeID cMyCustomChunkID = 'myck';

struct MyCustomChunkData {
	float mGainParamValue{1.f};
};
static_assert(sizeof(MyCustomChunkData) == 4, "MyCustomChunkData has unexpected size");

AAX_Result DemoGain_Parameters::GetNumberOfChunks (int32_t * oNumChunks )  const
{
	if (oNumChunks) {
		*oNumChunks = 1;
	}
	return AAX_SUCCESS;
}

AAX_Result DemoGain_Parameters::GetChunkIDFromIndex (int32_t iIndex, AAX_CTypeID * oChunkID )  const
{
	if (iIndex == 0) {
		if (oChunkID) {
			*oChunkID = cMyCustomChunkID;
		}
		return AAX_SUCCESS;
	}
	
	if (oChunkID) {
		*oChunkID = AAX_CTypeID(0);
	}
	return AAX_ERROR_INVALID_CHUNK_INDEX;
}

AAX_Result DemoGain_Parameters::GetChunkSize (AAX_CTypeID iChunkID, uint32_t * oSize )  const
{
	if (iChunkID == cMyCustomChunkID) {
		if (oSize) {
			*oSize = sizeof(MyCustomChunkData);
		}
		return AAX_SUCCESS;
	}
	
	return AAX_ERROR_INVALID_CHUNK_ID;
}

AAX_Result DemoGain_Parameters::GetChunk (AAX_CTypeID iChunkID, AAX_SPlugInChunk * oChunk ) const
{
	if (!oChunk) {
		return AAX_SUCCESS;
	}
	
	// chunkID, manID, prodID, PlugID are populated already
	
	if (iChunkID == cMyCustomChunkID)
	{
		AAX_ASSERT(oChunk->fChunkID == iChunkID);
		
		oChunk->fVersion = 1;
		memset(oChunk->fName, 0, 32); // unused
		oChunk->fSize = sizeof(MyCustomChunkData);
		
		// Fill the chunk data
		MyCustomChunkData* const chunkDataBuffer = reinterpret_cast<MyCustomChunkData*>(&oChunk->fData);
		AAX_IParameter const * const gainParam = mParameterManager.GetParameterByID(DemoGain_GainID);
		AAX_ASSERT(gainParam);
		bool const gotValueSuccess = gainParam->GetValueAsFloat(&chunkDataBuffer->mGainParamValue);
		AAX_ASSERT(gotValueSuccess);
		
		return AAX_SUCCESS;
	}
	
	return AAX_ERROR_INVALID_CHUNK_ID;
}

AAX_Result DemoGain_Parameters::SetChunk (AAX_CTypeID iChunkID, const AAX_SPlugInChunk * iChunk )
{
	if (!iChunk) {
		return AAX_ERROR_NULL_ARGUMENT;
	}
	
	// Apply the chunk data to the plugin state
	
	if (iChunkID == cMyCustomChunkID)
	{
		if (iChunk->fSize < sizeof(MyCustomChunkData)) {
			return AAX_ERROR_INCORRECT_CHUNK_SIZE;
		}
		
		MyCustomChunkData const * const chunkDataBuffer = reinterpret_cast<MyCustomChunkData const *>(&iChunk->fData);
		AAX_IParameter * const gainParam = mParameterManager.GetParameterByID(DemoGain_GainID);
		AAX_ASSERT(gainParam);
		bool const setValueSuccess = gainParam->SetValueWithFloat(chunkDataBuffer->mGainParamValue);
		AAX_ASSERT(setValueSuccess);
		
		return AAX_SUCCESS;
	}
	
	return AAX_ERROR_INVALID_CHUNK_ID;
}

AAX_Result DemoGain_Parameters::CompareActiveChunk (const AAX_SPlugInChunk * iChunkP, AAX_CBoolean * oIsEqual )  const
{
	// default to not setting the compare light
	if (oIsEqual) {
		*oIsEqual = true;
	}
	if (!iChunkP) {
		return AAX_SUCCESS;
	}
	
	if (iChunkP->fChunkID == cMyCustomChunkID)
	{
		if (iChunkP->fSize < sizeof(MyCustomChunkData)) {
			return AAX_SUCCESS;
		}
		
		// Compare the chunk data to the current plugin state
		MyCustomChunkData const * const chunkDataBuffer = reinterpret_cast<MyCustomChunkData const *>(&iChunkP->fData);
		AAX_IParameter const * const gainParam = mParameterManager.GetParameterByID(DemoGain_GainID);
		AAX_ASSERT(gainParam);
		float gainParamValue{0.f};
		bool const gotValueSuccess = gainParam->GetValueAsFloat(&gainParamValue);
		AAX_ASSERT(gotValueSuccess);
		if (oIsEqual) {
			*oIsEqual = (gainParamValue == chunkDataBuffer->mGainParamValue);
		}
		
		return AAX_SUCCESS;
	}
	
	return AAX_SUCCESS;
}
