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
#include "DemoScaling_GUI_Win32.h"

// DemoScaling Includes
#include "DemoScaling_Parameters.h"

// AAX Includes
#include "AAX_IController.h"
#include "AAX_IViewContainer.h"
#include "AAX_VViewContainer.h"
#include "AAX_Assert.h"
#include "AAX_Enums.h"

namespace
{

constexpr float cDefaultWidth{500.0f};
constexpr float cDefaultHeight{400.0f};

HINSTANCE sInstance{nullptr};

LRESULT CALLBACK PluginWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_NCCREATE:
		{
			CREATESTRUCT* create{reinterpret_cast<CREATESTRUCT*>(lParam)};
			DemoScaling_GUI_Win32* plugin{reinterpret_cast<DemoScaling_GUI_Win32*>(create->lpCreateParams)};

			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(plugin));

			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
		case WM_PAINT:
		{
			DemoScaling_GUI_Win32* plugin{reinterpret_cast<DemoScaling_GUI_Win32*>(GetWindowLongPtr(hwnd, GWLP_USERDATA))};

			PAINTSTRUCT ps;
			HDC hdc{BeginPaint(hwnd, &ps)};
			RECT rect;

			GetClientRect(hwnd, &rect);
			FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));

			if(!plugin)
			{
				return 0;
			}

			// Draw text
			SetBkMode(hdc, TRANSPARENT);
			SetTextColor(hdc, RGB(0, 0, 0));

			const std::string text{
				plugin->GetScaleFactorString() + "\r\n" +
				plugin->GetPhysicalSizeString() + "\r\n" +
				plugin->GetLogicalSizeString()};

			// Select the custom font
			HFONT oldFont{nullptr};

			if(plugin->GetFont()) {
				oldFont = reinterpret_cast<HFONT>(SelectObject(hdc, plugin->GetFont()));
			}

			DrawText(hdc, text.c_str(), -1, &rect, DT_CENTER | DT_VCENTER);

			// Restore the old font
			if(oldFont) {
				SelectObject(hdc, oldFont);
			}

			EndPaint(hwnd, &ps);
			
			return 0;
		}

		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

void RegisterPluginWindowClass()
{
	WNDCLASS wc{0};

	wc.lpfnWndProc		= PluginWindowProc;
    wc.hInstance		= sInstance;
	wc.lpszClassName	= TEXT("DEMOSCALING_PLUGIN");
	wc.cbWndExtra 		= sizeof(void*);  

	RegisterClass(&wc);
}

void UnregisterPluginWindowClass() {
	UnregisterClass(TEXT("DEMOSCALING_PLUGIN"), sInstance);
}

} // namespace

extern "C" BOOL WINAPI DllMain(HINSTANCE iInstance, DWORD iSelector, LPVOID iReserved)
{
	try
	{
		if(iSelector == DLL_PROCESS_ATTACH)
		{
			sInstance = iInstance;
			RegisterPluginWindowClass();
		}
		else if(iSelector == DLL_PROCESS_DETACH)
		{
			UnregisterPluginWindowClass();
			sInstance = nullptr;
		}
	}
	catch(...)
	{
		return false;
	}

	return true;
}

AAX_IEffectGUI* AAX_CALLBACK DemoScaling_GUI::Create() {
	return new DemoScaling_GUI_Win32;
}

DemoScaling_GUI_Win32::DemoScaling_GUI_Win32()
:	DemoScaling_GUI(),
	mScalingFactor(1.0f),
	mParentHwnd(nullptr),
	mPluginHwnd(nullptr),
	mFont(nullptr),
	mCurrentViewContainer(nullptr)
{
	mLogicalSize.horz = cDefaultWidth;
	mLogicalSize.vert = cDefaultHeight;
	this->OnScaleChanged(eOnScaleChangedFlags_UpdateFont);
}

AAX_Result DemoScaling_GUI_Win32::Initialize(IACFUnknown * iController)
{
	AAX_Result result{this->DemoScaling_GUI::Initialize(iController)};
	
	if (AAX_SUCCESS != result){
		return result;
	}

	return result;
}

AAX_Result DemoScaling_GUI_Win32::Uninitialize()
{
	return DemoScaling_GUI::Uninitialize();
}

AAX_Result DemoScaling_GUI_Win32::SetViewContainer(IACFUnknown* inViewContainer)
{
	mCurrentViewContainer = inViewContainer;

	return DemoScaling_GUI::SetViewContainer(inViewContainer);
}

AAX_Result DemoScaling_GUI_Win32::UpdateScalingFactor(IACFUnknown* inViewContainer, float inScalingFactor)
{
	if(!inViewContainer) {
		return AAX_ERROR_INVALID_ARGUMENT;
	}

	if(mCurrentViewContainer != inViewContainer) {
		return AAX_ERROR_INVALID_ARGUMENT;
	}

	AAX_Result result{AAX_SUCCESS};

	if(AAX_IViewContainer* container{GetViewContainer()})
	{
		// Store the new scaling factor
		mScalingFactor = inScalingFactor;
			
		// Update the font and resize the plugin window
		this->OnScaleChanged(eOnScaleChangedFlags_UpdateFont | eOnScaleChangedFlags_ResizeWindow);

		// Inform the host about the new physical size
		result = container->SetViewSize(mPhysicalSize);
	}
	else {
		result = AAX_ERROR_NULL_COMPONENT;
	}

	return result;
}

AAX_Result DemoScaling_GUI_Win32::GetViewSize(AAX_Point* oEffectViewSize) const
{
	if(!oEffectViewSize) {
		return AAX_ERROR_INVALID_ARGUMENT;
	}

	// Inform the host about the physical size of the plugin
	*oEffectViewSize = mPhysicalSize;

	return AAX_SUCCESS;
}

const std::string& DemoScaling_GUI_Win32::GetLogicalSizeString() const
{
	return mLogicalSizeString;
}

const std::string& DemoScaling_GUI_Win32::GetPhysicalSizeString() const
{
	return mPhysicalSizeString;
}

const std::string& DemoScaling_GUI_Win32::GetScaleFactorString() const
{
	return mScaleFactorString;
}

HFONT DemoScaling_GUI_Win32::GetFont() const
{
	return mFont;
}

void DemoScaling_GUI_Win32::CreateViewContents()
{
}

void DemoScaling_GUI_Win32::CreateViewContainer()
{
	// Get the parent window handle to add the plugin's one as a child
	if(this->GetViewContainerType() == AAX_eViewContainer_Type_HWND) {
		mParentHwnd = reinterpret_cast<HWND>(this->GetViewContainerPtr());
	}

	// Retrieve the data about the scaling and set calculate size
	if(AAX_IViewContainer* container{this->GetViewContainer()})
	{
		float scalingFactor{1.0f};
		AAX_Result result{container->GetScalingFactor(&scalingFactor)};

		if(result == AAX_SUCCESS)
		{
			mScalingFactor = scalingFactor;

			this->OnScaleChanged(eOnScaleChangedFlags_UpdateFont);
		}
	}

	// Create the window if we have a parent
	if(mParentHwnd)
	{
		mPluginHwnd = CreateWindowEx(
			0,                              // Extended window style
			TEXT("DEMOSCALING_PLUGIN"),     // Window class name
			TEXT(""),     					// Window title
			WS_CHILD | WS_VISIBLE,          // Window style
			0,                              // X position
			0,                              // Y position
			static_cast<int>(mPhysicalSize.horz),  // Width
			static_cast<int>(mPhysicalSize.vert),  // Height
			mParentHwnd,                    // Parent window
			nullptr,                        // Menu
			sInstance,                      // Instance handle
			this                         	// Additional application data
		);
	}
}

void DemoScaling_GUI_Win32::DeleteViewContainer()
{
	// Destroy the window handle
	if(mPluginHwnd)
	{
		DestroyWindow(mPluginHwnd);
		mPluginHwnd = nullptr;
	}

	// Delete the font
	if(mFont)
	{
		DeleteObject(mFont);
		mFont = nullptr;
	}

	mParentHwnd = nullptr;
}

void DemoScaling_GUI_Win32::OnScaleChanged(unsigned int flags)
{
	// Recalculate the physical size and re-create the strings 
	mPhysicalSize.horz = mLogicalSize.horz * mScalingFactor;
	mPhysicalSize.vert = mLogicalSize.vert * mScalingFactor;
	const std::string logicalHorzString{std::to_string(static_cast<int>(mLogicalSize.horz))};
	const std::string logicalVertString{std::to_string(static_cast<int>(mLogicalSize.vert))};
	const std::string physicalHorzString{std::to_string(static_cast<int>(mPhysicalSize.horz))};
	const std::string physicalVertString{std::to_string(static_cast<int>(mPhysicalSize.vert))};
	const std::string scalingFactorString{std::to_string(mScalingFactor)};
	mScaleFactorString = "Scale factor: " + scalingFactorString;
	mPhysicalSizeString = "Physical size: " + physicalHorzString + "x" + physicalVertString;
	mLogicalSizeString = "Logical size: " + logicalHorzString + "x" + logicalVertString;

	// Resize the plugin view if it exists
	if((flags & eOnScaleChangedFlags_UpdateFont) != 0) // Create or recreate the font
	{
		if(mFont)
		{
			DeleteObject(mFont);
			mFont = nullptr;
		}

		constexpr int baseFontSize{16};
		const int scaledFontSize{static_cast<int>(baseFontSize * mScalingFactor)};

		mFont = CreateFont(
			-scaledFontSize,               // Height (negative = character height)
			0,                             // Width (0 = default aspect ratio)
			0,                             // Escapement
			0,                             // Orientation
			FW_NORMAL,                     // Weight
			FALSE,                         // Italic
			FALSE,                         // Underline
			FALSE,                         // Strikeout
			DEFAULT_CHARSET,               // Character set
			OUT_DEFAULT_PRECIS,            // Output precision
			CLIP_DEFAULT_PRECIS,           // Clipping precision
			DEFAULT_QUALITY,               // Quality
			DEFAULT_PITCH | FF_DONTCARE,   // Pitch and family
			TEXT("Segoe UI")               // Font name
		);
	}

	if(mPluginHwnd && (flags & eOnScaleChangedFlags_ResizeWindow) != 0)
	{
		MoveWindow(mPluginHwnd, 0, 0, mPhysicalSize.horz, mPhysicalSize.vert, false);
		InvalidateRect(mPluginHwnd, nullptr, false);
	}
}
