/*================================================================================================*/
/*
 *	Copyright 2010-2015, 2023-2025 Avid Technology, Inc.
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

#include "AAX_CEffectGUI_Win32.h"
#include "AAX_IParameter.h"

HINSTANCE AAX_CEffectGUI_Win32::sInstance = 0;

#define AAX_ORIG_WINDOW_PROC_PROP	"AAX_OrigWinProc_Prop"
#define AAX_EFFECTGUI_PROP			"AAX_EffectGUI_Prop"

// *******************************************************************************
// ROUTINE:	DllMain
// *******************************************************************************
extern "C" BOOL WINAPI DllMain ( HINSTANCE iInstance, DWORD iSelector, LPVOID iReserved )
{
	try
	{
		if ( iSelector == DLL_PROCESS_ATTACH )
		{
			AAX_CEffectGUI_Win32::SetInstance( iInstance );
		}	
	}
	catch(...)
	{
		return false;
	}

	return true;
}

// *******************************************************************************
// METHOD:	AAX_CEffectGUI_Win32
// *******************************************************************************
AAX_CEffectGUI_Win32::AAX_CEffectGUI_Win32()
{
	mHWND = 0;
	mPlugInHWND = 0;
	mLastTextResult = AAX_eEventResult_PassEventToSystem;
	mLogicalViewSize = AAX_Point{};
	mRelativeViewScaleFactor = 1.f;
	mLastViewScaleFactor = 0.f;
}

// *******************************************************************************
// METHOD:	~AAX_CEffectGUI_Win32
// *******************************************************************************
AAX_CEffectGUI_Win32::~AAX_CEffectGUI_Win32()
{
	this->DeleteViewContainer();
}

// *******************************************************************************
// METHOD:	GetViewSize
// *******************************************************************************
AAX_Result AAX_CEffectGUI_Win32::GetViewSize ( AAX_Point* oEffectViewSize ) const
{
	// Use the logical size, not the physical size, for GetViewSize
	*oEffectViewSize = mLogicalViewSize;
	return AAX_SUCCESS;
}

// *******************************************************************************
// METHOD:	GetViewScaleFactor
// *******************************************************************************
AAX_Result AAX_CEffectGUI_Win32::GetViewScaleFactor(float* oViewScaleFactor) const
{
	constexpr float cDefaultWindowsDPI = 96.f;
	AAX_Result result = AAX_SUCCESS;
	if (oViewScaleFactor)
	{
		if (mPlugInHWND)
		{
			UINT const dpi = ::GetDpiForWindow( mPlugInHWND );
			if (0 == dpi)
			{
				result = AAX_ERROR_INVALID_VIEW_SIZE; // TODO: Define a proper error code
			}
			*oViewScaleFactor = static_cast<float>(dpi) / cDefaultWindowsDPI;
		}
		else
		{
			*oViewScaleFactor = 1.f;
			result = AAX_ERROR_INVALID_VIEW_SIZE; // TODO: Define a proper error code
		}
	}

	return result;
}

// *******************************************************************************
// ROUTINE:	CreateViewContents
// *******************************************************************************
void AAX_CEffectGUI_Win32::CreateViewContents ( )
{
	// Create the view contents if the plug-in HWND does not yet exist
	if ( ( NULL != GetParentHWND () ) && ( NULL == GetPlugInHWND () ) )
	{
		const WORD plugInHWNDResourceID = GetPlugInHWNDResourceID ();
		CreatePlugInHWND ( plugInHWNDResourceID );
		InitPlugInHWNDContents ();
	}
}

// *******************************************************************************
// ROUTINE:	CreateViewContainer
// *******************************************************************************
void AAX_CEffectGUI_Win32::CreateViewContainer ( )
{	
	if ( this->GetViewContainerType () == AAX_eViewContainer_Type_HWND )
	{
		// Set the provided HWND as the parent HWND
		SetParentHWND( (HWND) this->GetViewContainerPtr () );
		if ( NULL != GetParentHWND () )
		{
			// Create the view contents if the plug-in HWND does not yet exist
			if ( NULL == GetPlugInHWND () )
			{
				const WORD plugInHWNDResourceID = GetPlugInHWNDResourceID ();
				CreatePlugInHWND ( plugInHWNDResourceID );
				InitPlugInHWNDContents ();
			}
		
			// Set the view contents on the parent
			if ( NULL != GetPlugInHWND () )
			{
				::SetParent( GetPlugInHWND (), GetParentHWND () );
				::ShowWindow ( GetPlugInHWND (), SW_SHOW );
			}
		}
	}
}

// *******************************************************************************
// ROUTINE:	DeleteViewContainer
// *******************************************************************************
void AAX_CEffectGUI_Win32::DeleteViewContainer ()
{	
	if ( mPlugInHWND )
	{
		DestroyWindow( mPlugInHWND );
		mPlugInHWND = 0;
	}
}	

// *******************************************************************************
// METHOD:	DoTextWindowProc
// *******************************************************************************
AAX_EEventResult AAX_CEffectGUI_Win32::DoTextWindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	AAX_EEventResult	textResult = AAX_eEventResult_PassEventToSystem;

	if ( hwnd == GetFocus() )
	{
		if ( uMsg == WM_KEYDOWN )
		{
			mLastTextResult = this->HandleKeyDown( hwnd, wParam );
			textResult = mLastTextResult;
			if ( mLastTextResult == AAX_eEventResult_CancelFocus ) 
				this->ForceParameterUpdateForHWND( hwnd );
		}
		else if ( uMsg == WM_CHAR )
		{
			textResult = mLastTextResult;
			mLastTextResult = AAX_eEventResult_PassEventToSystem;

			if ( textResult == AAX_eEventResult_ClearFocus ||
				textResult == AAX_eEventResult_CancelFocus )
			{
				SetFocus( 0 );
			}
		}
//		else if ( uMsg == WM_KEYUP )
//		{
//			if ( wParam == VK_RETURN || wParam == VK_ESCAPE )
//				return true;
//		}
	}

	return textResult;
}

// *******************************************************************************
// METHOD:	TextWindowProc
// *******************************************************************************
LRESULT CALLBACK AAX_CEffectGUI_Win32::TextWindowProc( __in HWND hwnd, __in UINT uMsg, __in WPARAM wParam, __in LPARAM lParam )
{
	AAX_EEventResult textResult = AAX_eEventResult_PassEventToSystem;

	AAX_CEffectGUI_Win32 * effectGUI = (AAX_CEffectGUI_Win32 *) GetProp( hwnd, AAX_EFFECTGUI_PROP );
	if ( effectGUI )
	{
		textResult = effectGUI->DoTextWindowProc( hwnd, uMsg, wParam, lParam );
		if ( textResult == AAX_eEventResult_PassEventToClient )
			SendMessage( effectGUI->GetParentHWND(), uMsg, wParam, lParam );
	}

	if ( textResult == AAX_eEventResult_PassEventToSystem )
	{
		WNDPROC origWindowProc = (WNDPROC) GetProp( hwnd, AAX_ORIG_WINDOW_PROC_PROP );
		if ( origWindowProc )
			return CallWindowProc( origWindowProc, hwnd, uMsg, wParam, lParam );
	}

	return 1;
}

// *******************************************************************************
// METHOD:	SetupForKeyboardInput
// *******************************************************************************
void AAX_CEffectGUI_Win32::SetupForKeyboardInput ( HWND iEditText )
{
	if ( iEditText )
	{
		WNDPROC oldTextWindowProc = (WNDPROC) GetWindowLongPtr( iEditText, GWLP_WNDPROC );
		SetProp( iEditText, AAX_ORIG_WINDOW_PROC_PROP, oldTextWindowProc );
		SetProp( iEditText, AAX_EFFECTGUI_PROP, this );
		SetWindowLongPtr( iEditText, GWLP_WNDPROC, (LONG_PTR) TextWindowProc );
	}
}

// *******************************************************************************
// METHOD:	DoMouseWindowProc
// *******************************************************************************
AAX_EEventResult AAX_CEffectGUI_Win32::DoMouseWindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	return AAX_eEventResult_PassEventToSystem;
}

// *******************************************************************************
// METHOD:	MouseInputWindowProc
// *******************************************************************************
LRESULT CALLBACK AAX_CEffectGUI_Win32::MouseInputWindowProc( __in HWND hwnd, __in UINT uMsg, __in WPARAM wParam, __in LPARAM lParam )
{
	LRESULT result = 1;
	AAX_EEventResult mouseResult = AAX_eEventResult_PassEventToSystem;

	AAX_CEffectGUI_Win32 * effectGUI = (AAX_CEffectGUI_Win32 *) GetProp( hwnd, AAX_EFFECTGUI_PROP );
	if ( effectGUI )
	{
		switch (uMsg)
		{
			case WM_RBUTTONDOWN:
			case WM_MBUTTONDOWN:
			case WM_LBUTTONDOWN:
			case WM_MOUSEMOVE:
			case WM_LBUTTONUP:
			case WM_RBUTTONUP:
			case WM_MBUTTONUP:
#if (_WIN32_WINNT >= 0x0500)
			case WM_XBUTTONDOWN:
			case WM_XBUTTONUP:
#endif
			{
				mouseResult = effectGUI->DoMouseWindowProc( hwnd, uMsg, wParam, lParam );
				break;
			}
		}

		if (mouseResult == AAX_eEventResult_PassEventToClient)
		{
			SendMessage( effectGUI->GetParentHWND(), uMsg, wParam, lParam );
		}
	}

	if (mouseResult == AAX_eEventResult_PassEventToSystem)
	{
		WNDPROC origWindowProc = (WNDPROC) GetProp( hwnd, AAX_ORIG_WINDOW_PROC_PROP );
		if ( origWindowProc )
		{
			result = CallWindowProc( origWindowProc, hwnd, uMsg, wParam, lParam );
		}
	}

	return result;
}

// *******************************************************************************
// METHOD:	SetupForMouseInput
// *******************************************************************************
void AAX_CEffectGUI_Win32::SetupForMouseInput ( HWND iWindow )
{
	if ( iWindow )
	{
		WNDPROC oldWindowProc = (WNDPROC) GetWindowLongPtr( iWindow, GWLP_WNDPROC );
		SetProp( iWindow, AAX_ORIG_WINDOW_PROC_PROP, oldWindowProc );
		SetProp( iWindow, AAX_EFFECTGUI_PROP, this );
		SetWindowLongPtr( iWindow, GWLP_WNDPROC, (LONG_PTR) MouseInputWindowProc );
	}
}
 
// *******************************************************************************
// METHOD:	DialogProc
// *******************************************************************************
INT_PTR CALLBACK AAX_CEffectGUI_Win32::DialogProc( __in  HWND hwnd, __in  UINT uMsg, __in  WPARAM wParam, __in  LPARAM lParam )
{
	AAX_CEffectGUI_Win32 * effectGUI = (AAX_CEffectGUI_Win32 *) GetProp( hwnd, AAX_EFFECTGUI_PROP );
	if ( effectGUI )
		return effectGUI->HandleMessage( uMsg, wParam, lParam );

	return false;
}

// *******************************************************************************
// METHOD:	CreatePlugInHWND
// *******************************************************************************
HWND AAX_CEffectGUI_Win32::CreatePlugInHWND ( WORD iResourceID )
{
	HWND plugInHWND = mPlugInHWND;
	
	if ((NULL != this->GetParentHWND()) && (NULL == plugInHWND))
	{
		plugInHWND = ::CreateDialog( this->GetInstance(), MAKEINTRESOURCE( iResourceID ), this->GetParentHWND(), DialogProc );
		if ( NULL != plugInHWND )
		{
			RECT plugInHWNDRect{};
			::GetWindowRect(plugInHWND, &plugInHWNDRect);
			mLogicalViewSize.horz = plugInHWNDRect.right - plugInHWNDRect.left;
			mLogicalViewSize.vert = plugInHWNDRect.bottom - plugInHWNDRect.top;

			SetProp( plugInHWND, AAX_EFFECTGUI_PROP, this );
			this->SetPlugInHWND( plugInHWND );
		}
	}

	return plugInHWND;
}

// *******************************************************************************
// METHOD:	UpdateViewContents
// *******************************************************************************
AAX_Result AAX_CEffectGUI_Win32::UpdateViewContents()
{
	AAX_Result err = this->UpdateViewScale();
	return err;
}

// *******************************************************************************
// METHOD:	UpdateViewScale
// *******************************************************************************

namespace
{
	template <typename F>
	bool AreNearlyEqual(F a, F b, F epsilon = 1e-5f)
	{
		return std::fabs(a - b) < epsilon;
	}
}

AAX_Result AAX_CEffectGUI_Win32::UpdateViewScale()
{
	AAX_Result result = AAX_SUCCESS;

	// Update the scale factor value
	{
		float curScaleFactor{ 0.f };
		result = this->GetViewScaleFactor(&curScaleFactor);
		if (AAX_SUCCESS != result || 0 >= curScaleFactor)
		{
			return result; // TODO: Provide proper error code
		}

		if (AreNearlyEqual(mLastViewScaleFactor, curScaleFactor))
		{
			return AAX_SUCCESS;
		}

		// set the relative scale to use when updating individual elements
		mRelativeViewScaleFactor = mLastViewScaleFactor ? curScaleFactor / mLastViewScaleFactor : curScaleFactor;
		mLastViewScaleFactor = curScaleFactor;
	}

	// Callback function for EnumChildWindows
	auto enumFunc = [](HWND hChild, LPARAM lParam) -> BOOL
	{
		AAX_CEffectGUI_Win32* const self = reinterpret_cast<AAX_CEffectGUI_Win32*>(lParam);
		self->UpdateViewElementScale(hChild);
		return TRUE; // Continue enumeration
	};

	// Scale all child windows
	HWND const parent = this->GetParentHWND();
	if (parent)
	{
		EnumChildWindows(parent, enumFunc, reinterpret_cast<LPARAM>(this));
	}

	mRelativeViewScaleFactor = 1.f;
	return result;
}

// *******************************************************************************
// METHOD:	UpdateViewElementScale
// *******************************************************************************
void AAX_CEffectGUI_Win32::UpdateViewElementScale(HWND iElement)
{
	HWND const parentView = this->GetParentHWND();
	if (!iElement || !parentView)
	{
		return;
	}

	// Scale and position the element
	{
		RECT rect;
		::GetWindowRect(iElement, &rect);
		::MapWindowPoints(HWND_DESKTOP, parentView, (LPPOINT)&rect, 2);

		int newX = static_cast<int>(rect.left * mRelativeViewScaleFactor);
		int newY = static_cast<int>(rect.top * mRelativeViewScaleFactor);
		int newWidth = static_cast<int>((rect.right - rect.left) * mRelativeViewScaleFactor);
		int newHeight = static_cast<int>((rect.bottom - rect.top) * mRelativeViewScaleFactor);

		::SetWindowPos(iElement, nullptr, newX, newY, newWidth, newHeight, SWP_NOZORDER);
	}

	// Scale the font, if any
	{
		HFONT hFont = (HFONT)::SendMessage(iElement, WM_GETFONT, 0, 0);
		if (hFont)
		{
			LOGFONT logFont{};
			::GetObject(hFont, sizeof(LOGFONT), &logFont);
			auto const origFontSizeInsertResult = mOriginalFontSize.emplace(iElement, logFont.lfHeight);
			LONG const origFontSize = origFontSizeInsertResult.first->second;
			logFont.lfHeight = static_cast<int>(origFontSize * mRelativeViewScaleFactor);
			HFONT hNewFont = CreateFontIndirect(&logFont);
			::SendMessage(iElement, WM_SETFONT, (WPARAM)hNewFont, TRUE);
		}
	}
}
