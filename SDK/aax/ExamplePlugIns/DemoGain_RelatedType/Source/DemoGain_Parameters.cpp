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
	
//	// UNCOMMENT FOR DEMONSTRATION OF PARAMETER AUTOMATION PERSISTENCE USING MATCHING PARAMETER ID
//	{
//		AAX_CString id = DemoGain_GainID;
//		std::unique_ptr<AAX_IParameter> param (new AAX_CParameter<float>(
//			id, AAX_CString("Gain"), 1.0f,
//			AAX_CLinearTaperDelegate<float>(0.0f, 1.0f),
//			AAX_CNumberDisplayDelegate<float>(), true));
//		param->SetNumberOfSteps(55);
//		param->SetType( AAX_eParameterType_Continuous );
//		mParameterManager.AddParameter(param.release());
//	}
	
	// NEW PARAM: boost
	{
		AAX_CString id = DemoGainRelated_BoostID;
		std::unique_ptr<AAX_IParameter> param (new AAX_CParameter<float>(
			id, AAX_CString("Boost"), 1.0f,
			AAX_CLinearTaperDelegate<float>(0.0f, 2.0f), // new feature: allow greater than unity gain
			AAX_CNumberDisplayDelegate<float>(), true));
		param->SetNumberOfSteps(55);
		param->SetType( AAX_eParameterType_Continuous );
		mParameterManager.AddParameter(param.release());
		mPacketDispatcher.RegisterPacket(id.CString(), eAlgPortID_CoefsGainIn, this, &DemoGain_Parameters::UpdatePacket_Boost);
	}
	
	// NEW PARAM: boost multiplier
	{
		AAX_CString id = DemoGainRelated_BoostMultiplierID;
		std::unique_ptr<AAX_IParameter> param (new AAX_CParameter<float>(
			id, AAX_CString("Boost X"), 1.0f,
			AAX_CLinearTaperDelegate<float>(1.0f, 5.0f),
			AAX_CNumberDisplayDelegate<float>(), true));
		param->SetNumberOfSteps(55);
		param->SetType( AAX_eParameterType_Continuous );
		mParameterManager.AddParameter(param.release());
		mPacketDispatcher.RegisterPacket(id.CString(), eAlgPortID_CoefsGainIn, this, &DemoGain_Parameters::UpdatePacket_Boost);
	}
	
	return AAX_SUCCESS;
}

// *******************************************************************************
// METHOD:	UpdatePacketGain
// *******************************************************************************
AAX_Result DemoGain_Parameters::UpdatePacket_Boost( AAX_CPacket& ioPacket )
{
	AAX_IParameter* const boostParameter = mParameterManager.GetParameterByID( DemoGainRelated_BoostID );
	AAX_IParameter*	const boostXParameter = mParameterManager.GetParameterByID( DemoGainRelated_BoostMultiplierID );
	if (boostParameter && boostXParameter)
	{
		float boostValue;
		float boostXValue;
		if (boostParameter->GetValueAsFloat(&boostValue) && boostXParameter->GetValueAsFloat(&boostXValue))
		{
			SDemoGain_CoefsGain&  packet = *ioPacket.GetPtr<SDemoGain_CoefsGain>();
			
			// Populate packet
			packet.mGain = boostValue * boostXValue;
		}
	}
	return AAX_SUCCESS;
}



// *******************************************************************************
// DATA PERSISTENCE METHODS
// *******************************************************************************

// Chunk from original DemoGain
constexpr AAX_CTypeID cMyCustomChunkID = 'myck';

struct MyCustomChunkData {
	float mGainParamValue{1.f};
};
static_assert(sizeof(MyCustomChunkData) == 4, "MyCustomChunkData has unexpected size");

// Chunk from new and improved DemoGain_Related
constexpr AAX_CTypeID cMyRelatedCustomChunkID = 'mycR';

struct MyRelatedCustomChunkData {
	float mBoostParamValue{1.f};
	float mBoostXParamValue{1.f};
};
static_assert(sizeof(MyRelatedCustomChunkData) == 8, "MyRelatedCustomChunkData has unexpected size");


AAX_Result DemoGain_Parameters::GetNumberOfChunks (int32_t * oNumChunks )  const
{
	if (oNumChunks) {
		*oNumChunks = 1; // we can read the original chunk, but we only write the new chunk out
	}
	return AAX_SUCCESS;
}

AAX_Result DemoGain_Parameters::GetChunkIDFromIndex (int32_t iIndex, AAX_CTypeID * oChunkID )  const
{
	if (iIndex == 0) {
		if (oChunkID) {
			*oChunkID = cMyRelatedCustomChunkID;
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
	if (iChunkID == cMyRelatedCustomChunkID) {
		if (oSize) {
			*oSize = sizeof(MyRelatedCustomChunkData);
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
	
	if (iChunkID == cMyRelatedCustomChunkID)
	{
		AAX_ASSERT(oChunk->fChunkID == iChunkID);
		
		oChunk->fVersion = 1;
		memset(oChunk->fName, 0, 32); // unused
		oChunk->fSize = sizeof(MyRelatedCustomChunkData);
		
		// Fill the chunk data
		MyRelatedCustomChunkData* const chunkDataBuffer = reinterpret_cast<MyRelatedCustomChunkData*>(&oChunk->fData);
		AAX_IParameter const * const boostParam = mParameterManager.GetParameterByID(DemoGainRelated_BoostID);
		AAX_ASSERT(boostParam);
		AAX_IParameter const * const boostXParam = mParameterManager.GetParameterByID(DemoGainRelated_BoostMultiplierID);
		AAX_ASSERT(boostXParam);
		
		bool gotValueSuccess{false};
		gotValueSuccess = boostParam->GetValueAsFloat(&chunkDataBuffer->mBoostParamValue);
		AAX_ASSERT(gotValueSuccess);
		gotValueSuccess = boostXParam->GetValueAsFloat(&chunkDataBuffer->mBoostXParamValue);
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
		
		if (iChunk->fVersion > 1) {
			// This chunk was written only for backwards compatibility. Ignore it
			// and only set data based on cMyRelatedCustomChunkID.
			return AAX_SUCCESS;
		}
		
		// Otherwise, the settings came from the original plugin and do not
		// include cMyRelatedCustomChunkID, so use this chunk's data.
		
		MyCustomChunkData const * const chunkDataBuffer = reinterpret_cast<MyCustomChunkData const *>(&iChunk->fData);
		
		AAX_IParameter * const boostParam = mParameterManager.GetParameterByID(DemoGainRelated_BoostID);
		AAX_ASSERT(boostParam);
		AAX_IParameter * const boostXParam = mParameterManager.GetParameterByID(DemoGainRelated_BoostMultiplierID);
		AAX_ASSERT(boostXParam);
		
		bool setValueSuccess{false};
		
		setValueSuccess = boostParam->SetValueWithFloat(chunkDataBuffer->mGainParamValue);
		AAX_ASSERT(setValueSuccess);
		setValueSuccess = boostXParam->SetValueWithFloat(1.f); // when restoring from the previous plugin, always use gain multiplier of 1x
		AAX_ASSERT(setValueSuccess);
		
		return AAX_SUCCESS;
	}
	
	if (iChunkID == cMyRelatedCustomChunkID)
	{
		if (iChunk->fSize < sizeof(MyRelatedCustomChunkData)) {
			return AAX_ERROR_INCORRECT_CHUNK_SIZE;
		}
		
		MyRelatedCustomChunkData const * const chunkDataBuffer = reinterpret_cast<MyRelatedCustomChunkData const *>(&iChunk->fData);
		
		AAX_IParameter * const boostParam = mParameterManager.GetParameterByID(DemoGainRelated_BoostID);
		AAX_ASSERT(boostParam);
		AAX_IParameter * const boostXParam = mParameterManager.GetParameterByID(DemoGainRelated_BoostMultiplierID);
		AAX_ASSERT(boostXParam);
		
		bool setValueSuccess{false};
		
		setValueSuccess = boostParam->SetValueWithFloat(chunkDataBuffer->mBoostParamValue);
		AAX_ASSERT(setValueSuccess);
		setValueSuccess = boostXParam->SetValueWithFloat(chunkDataBuffer->mBoostXParamValue);
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
		
		if (iChunkP->fVersion > 1) {
			// This chunk was written only for backwards compatibility. Ignore it
			// when comparing chunk data to current state.
			return AAX_SUCCESS;
		}
		
		// Otherwise, the settings came from the original plugin and do not
		// include cMyRelatedCustomChunkID, so use this chunk's data.
		
		// Compare the chunk data to the current plugin state
		MyCustomChunkData const * const chunkDataBuffer = reinterpret_cast<MyCustomChunkData const *>(&iChunkP->fData);
		
		AAX_IParameter const * const boostParam = mParameterManager.GetParameterByID(DemoGainRelated_BoostID);
		AAX_ASSERT(boostParam);
		AAX_IParameter const * const boostXParam = mParameterManager.GetParameterByID(DemoGainRelated_BoostMultiplierID);
		AAX_ASSERT(boostXParam);
		
		float boostParamValue{0.f};
		float boostXParamValue{0.f};
		bool gotValueSuccess{false};
		gotValueSuccess = boostParam->GetValueAsFloat(&boostParamValue);
		AAX_ASSERT(gotValueSuccess);
		gotValueSuccess = boostXParam->GetValueAsFloat(&boostXParamValue);
		AAX_ASSERT(gotValueSuccess);
		
		if (oIsEqual) {
			// Equal if the current state matches the result of SetChunk() for this chunk
			*oIsEqual = (boostParamValue == chunkDataBuffer->mGainParamValue && boostXParamValue == 1.f);
		}
		
		return AAX_SUCCESS;
	}
	
	if (iChunkP->fChunkID == cMyRelatedCustomChunkID)
	{
		if (iChunkP->fSize < sizeof(MyRelatedCustomChunkData)) {
			return AAX_SUCCESS;
		}
		
		// Compare the chunk data to the current plugin state
		MyRelatedCustomChunkData const * const chunkDataBuffer = reinterpret_cast<MyRelatedCustomChunkData const *>(&iChunkP->fData);
		
		AAX_IParameter const * const boostParam = mParameterManager.GetParameterByID(DemoGainRelated_BoostID);
		AAX_ASSERT(boostParam);
		AAX_IParameter const * const boostXParam = mParameterManager.GetParameterByID(DemoGainRelated_BoostMultiplierID);
		AAX_ASSERT(boostXParam);
		
		float boostParamValue{0.f};
		float boostXParamValue{0.f};
		bool gotValueSuccess{false};
		gotValueSuccess = boostParam->GetValueAsFloat(&boostParamValue);
		AAX_ASSERT(gotValueSuccess);
		gotValueSuccess = boostXParam->GetValueAsFloat(&boostXParamValue);
		AAX_ASSERT(gotValueSuccess);
		
		if (oIsEqual) {
			*oIsEqual = (boostParamValue == chunkDataBuffer->mBoostParamValue && boostXParamValue == chunkDataBuffer->mBoostXParamValue);
		}
		
		return AAX_SUCCESS;
	}
	
	return AAX_SUCCESS;
}


