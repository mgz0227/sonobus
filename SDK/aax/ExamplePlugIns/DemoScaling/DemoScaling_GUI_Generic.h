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

/**  
 *	\file   DemoScaling_GUI_Generic.h
 */ 
/*================================================================================================*/
#pragma once
#ifndef DEMOSCALING_GUI_GENERIC_H
#define DEMOSCALING_GUI_GENERIC_H

#include "DemoScaling_GUI.h"

//==============================================================================
// Generic GUI class (for platforms that do not support scaling)
//==============================================================================
class DemoScaling_GUI_Generic : public DemoScaling_GUI
{
public:
	DemoScaling_GUI_Generic();
	~DemoScaling_GUI_Generic() override = default;

	DemoScaling_GUI_Generic(const DemoScaling_GUI_Generic&) = delete;
	DemoScaling_GUI_Generic& operator=(const DemoScaling_GUI_Generic&) = delete;

	AAX_Result Initialize(IACFUnknown* iController ) AAX_OVERRIDE;
	AAX_Result Uninitialize() AAX_OVERRIDE;
	AAX_Result UpdateScalingFactor(IACFUnknown* inViewContainer, float inScalingFactor) AAX_OVERRIDE;
	void CreateViewContents() AAX_OVERRIDE;
	void CreateViewContainer() AAX_OVERRIDE;
	void DeleteViewContainer() AAX_OVERRIDE;

private:

};

#endif // DEMOSCALING_GUI_GENERIC_H
