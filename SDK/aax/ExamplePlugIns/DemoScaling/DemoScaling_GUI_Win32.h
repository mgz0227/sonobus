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
 *	\file   DemoScaling_GUI_Win32.h
 */ 
/*================================================================================================*/
#pragma once
#ifndef DEMOSCALING_GUI_WIN32_H
#define DEMOSCALING_GUI_WIN32_H

#include "DemoScaling_GUI.h"
#include <windows.h>
#include <string>

//==============================================================================
// Windows GUI class
//==============================================================================
class DemoScaling_GUI_Win32 : public DemoScaling_GUI
{
public:
	DemoScaling_GUI_Win32();
	~DemoScaling_GUI_Win32() override = default;

	DemoScaling_GUI_Win32(const DemoScaling_GUI_Win32&) = delete;
	DemoScaling_GUI_Win32& operator=(const DemoScaling_GUI_Win32&) = delete;
	
	AAX_Result Initialize(IACFUnknown* iController ) AAX_OVERRIDE;
	AAX_Result Uninitialize() AAX_OVERRIDE;
	AAX_Result SetViewContainer(IACFUnknown* inViewContainer) AAX_OVERRIDE;
	AAX_Result UpdateScalingFactor(IACFUnknown* inViewContainer, float inScalingFactor) AAX_OVERRIDE;
	AAX_Result GetViewSize(AAX_Point* oEffectViewSize) const AAX_OVERRIDE;
	const std::string& GetLogicalSizeString() const;
	const std::string& GetPhysicalSizeString() const;
	const std::string& GetScaleFactorString() const;
	HFONT GetFont() const;
	void CreateViewContents() AAX_OVERRIDE;
	void CreateViewContainer() AAX_OVERRIDE;
	void DeleteViewContainer() AAX_OVERRIDE;

private:
	enum EOnScaleChangedFlags
	{
		eOnScaleChangedFlags_None = 0,
		eOnScaleChangedFlags_ResizeWindow = 1 << 0,
		eOnScaleChangedFlags_UpdateFont = 1 << 1,
	};

	void OnScaleChanged(unsigned int flags = eOnScaleChangedFlags_None);

	std::string mScaleFactorString;
	std::string mLogicalSizeString;
	std::string mPhysicalSizeString;
	HWND mParentHwnd;
	HWND mPluginHwnd;
	HFONT mFont;
	float mScalingFactor;
	AAX_Point mPhysicalSize;
	AAX_Point mLogicalSize;
	void* mCurrentViewContainer;
};

#endif // DEMOSCALING_GUI_WIN32_H
