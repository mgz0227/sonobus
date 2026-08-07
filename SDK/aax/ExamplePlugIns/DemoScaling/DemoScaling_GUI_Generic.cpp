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
 *
 */
/*================================================================================================*/

// Self Include
#include "DemoScaling_GUI_Generic.h"

// DemoScaling Includes
#include "DemoScaling_Parameters.h"

// AAX Includes
#include "AAX_IController.h"
#include "AAX_Assert.h"
#include "AAX_Enums.h"

AAX_IEffectGUI* AAX_CALLBACK DemoScaling_GUI::Create()
{
	return new DemoScaling_GUI_Generic;
}

DemoScaling_GUI_Generic::DemoScaling_GUI_Generic() : DemoScaling_GUI()
{}

AAX_Result DemoScaling_GUI_Generic::Initialize(IACFUnknown* iController )
{
	AAX_Result result{this->DemoScaling_GUI::Initialize(iController)};
	
	if (AAX_SUCCESS != result) {
		return result;
	}

	return result;
}

AAX_Result DemoScaling_GUI_Generic::Uninitialize()
{
	return DemoScaling_GUI::Uninitialize();
}

AAX_Result DemoScaling_GUI_Generic::UpdateScalingFactor(IACFUnknown* inViewContainer, float inScalingFactor)
{
	return AAX_ERROR_UNIMPLEMENTED;
}

void DemoScaling_GUI_Generic::CreateViewContents()
{
	// N/A
}

void DemoScaling_GUI_Generic::CreateViewContainer()
{
	// N/A
}

void DemoScaling_GUI_Generic::DeleteViewContainer()
{
	// N/A
}
