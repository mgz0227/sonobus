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

// Component includes
#include "DemoScaling_Alg.h"

// AAX includes
#include "AAX.h"

//==============================================================================
// Processing function definition
//==============================================================================

void AAX_CALLBACK DemoScaling_AlgorithmProcessFunction(
	SDemoScaling_Alg_Context* const	inInstancesBegin[],
	const void* 					inInstancesEnd)
{
	// Get a pointer to the beginning of the memory block table
	SDemoScaling_Alg_Context* AAX_RESTRICT instance = inInstancesBegin[0];

	//--------- Iterate over plug-in instances ---------//
	//
	for (SDemoScaling_Alg_Context * const * walk{inInstancesBegin}; walk < inInstancesEnd; ++walk)
	{
		instance = *walk;

		//--------- Retrieve instance-specific information ---------//
		//
		// Memory blocks	
		const int32_t	bypass		= *instance->mCtrlBypassP; // 'int' not 'bool' for optimization
		const int32_t	buffersize	= *instance->mBufferSize;
		// Audio channels
		const float* const AAX_RESTRICT pdI	= instance->mInputPP[0]; // First channel
		float* const AAX_RESTRICT pdO 		= instance->mOutputPP[0]; // First channel

		//--------- Pass-thru  audio samples---------//
		//
		memmove(pdO, pdI, buffersize * sizeof(float));
	} // End instance-iteration loop
}
