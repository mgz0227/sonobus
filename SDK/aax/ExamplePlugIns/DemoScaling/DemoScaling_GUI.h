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

/**  
 *	\file   DemoScaling_GUI.h
 */ 
/*================================================================================================*/
#pragma once
#ifndef DEMOSCALING_GUI_H
#define DEMOSCALING_GUI_H

#include "AAX_CEffectGUI.h"

class DemoScaling_ViewComponent;

//==============================================================================
// Common GUI class (for platform-specific GUI classes to inherit from)
//==============================================================================
class DemoScaling_GUI : public AAX_CEffectGUI
{
public:
	static AAX_IEffectGUI* AAX_CALLBACK Create();

	DemoScaling_GUI() : AAX_CEffectGUI() {};
	~DemoScaling_GUI() override = default;

	DemoScaling_GUI(const DemoScaling_GUI&) = delete;
	DemoScaling_GUI& operator=(const DemoScaling_GUI&) = delete;
};

#endif // DEMOSCALING_GUI_H
