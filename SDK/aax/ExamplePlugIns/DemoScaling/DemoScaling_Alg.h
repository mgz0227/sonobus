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

/**  
 *	\file   DemoScaling_Alg.h
 *
 *	\brief  Algorithm component definitions for DemoScaling.
 *
 *  \author Delta Jankowska
 */ 
/*================================================================================================*/

#pragma once
#ifndef DEMOSCALING_ALG_H
#define DEMOSCALING_ALG_H

// AAX includes
#include "AAX.h"

//==============================================================================
// Component context definitions
//==============================================================================

// Context structure
struct SDemoScaling_Alg_Context
{
	int32_t*	mCtrlBypassP;	// Control message destination
	float**		mInputPP;		// Audio signal destination
	float**		mOutputPP;		// Audio signal source
	int32_t*	mBufferSize;	// Buffer size
};

// Physical addresses within the context
enum EDemoScaling_Alg_PortID
{
	eAlgPortID_BypassIn		= AAX_FIELD_INDEX(SDemoScaling_Alg_Context, mCtrlBypassP),
	eAlgFieldID_AudioIn		= AAX_FIELD_INDEX(SDemoScaling_Alg_Context, mInputPP),
	eAlgFieldID_AudioOut	= AAX_FIELD_INDEX(SDemoScaling_Alg_Context, mOutputPP),
	eAlgFieldID_BufferSize	= AAX_FIELD_INDEX(SDemoScaling_Alg_Context, mBufferSize),
};

//==============================================================================
// Callback declarations
//==============================================================================

void AAX_CALLBACK DemoScaling_AlgorithmProcessFunction(
	SDemoScaling_Alg_Context* const	inInstancesBegin[],
	const void*						inInstancesEnd);

#endif // DEMOSCALING_ALG_H
