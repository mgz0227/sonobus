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
 *	\file   DemoScaling_Parameters.h
 *
 *	\brief  DemoScaling_Parameters class declaration.
 */ 
/*================================================================================================*/ 

#pragma once
#ifndef DEMOSCALING_PARAMETERS_H
#define DEMOSCALING_PARAMETERS_H

#include "AAX_CEffectParameters.h"

class DemoScaling_Parameters : public AAX_CEffectParameters
{
public:
	DemoScaling_Parameters(void);
	AAX_DEFAULT_DTOR_OVERRIDE(DemoScaling_Parameters);
	
	// Create callback
	static AAX_CEffectParameters* AAX_CALLBACK Create();

public:
	//Overrides from AAX_CEffectParameters
	AAX_Result EffectInit() AAX_OVERRIDE;
};

#endif
