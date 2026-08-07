/*================================================================================================*/
/*
 *	Copyright 2026 Avid Technology, Inc.
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
#include "DemoScaling_Parameters.h"

// DemoScaling Includes
#include "DemoScaling_Defs.h"
#include "DemoScaling_Alg.h"

// AAX Includes
#include "AAX_CBinaryTaperDelegate.h"
#include "AAX_CBinaryDisplayDelegate.h"

// Standard Includes
#include <memory>

// *******************************************************************************
// ROUTINE:	Create
// *******************************************************************************
AAX_CEffectParameters *AAX_CALLBACK DemoScaling_Parameters::Create()
{
	return new DemoScaling_Parameters();
}

// *******************************************************************************
// METHOD:	DemoScaling_Parameters
// *******************************************************************************
DemoScaling_Parameters::DemoScaling_Parameters () :
	AAX_CEffectParameters()
{
}

// *******************************************************************************
// METHOD:	EffectInit
// *******************************************************************************
AAX_Result DemoScaling_Parameters::EffectInit()
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
	
	return AAX_SUCCESS;
}
