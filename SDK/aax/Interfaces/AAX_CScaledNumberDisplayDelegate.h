
/*================================================================================================*/
/*
 *
 *	Copyright 2025 Avid Technology, Inc.
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
 *	\file AAX_CScaledNumberDisplayDelegate.h
 *
 *	\brief A scaled number display delegate.
 *
 */ 
/*================================================================================================*/


#ifndef AAX_CSCALEDNUMBERDISPLAYDELEGATE_H
#define AAX_CSCALEDNUMBERDISPLAYDELEGATE_H

#include "AAX_IDisplayDelegate.h"
#include "AAX_CString.h"
#include <cmath>


/**	\brief A scaled numeric display format conforming to AAX_IDisplayDelegate
	
	\details
	This display delegate converts a parameter value to a numeric string using a specified
	precision and scaling factor. The scaling factor is applied during conversion - for example,
	a scaling factor of 100 will convert a logical value of 1 to the display string "100" and 
	vice versa, or a logical value of 0.01 to the display string "1" and vice versa.

	\sa \ref AAX_CNumberDisplayDelegate for a non-scaled version.

	\ingroup AAXLibraryFeatures_ParameterManager_DisplayDelegates

 */
template <typename T, uint32_t Precision=2, uint32_t SpaceAfter=0>
class AAX_CScaledNumberDisplayDelegate : public AAX_IDisplayDelegate<T>
{
public:
	//Virtual Overrides
	AAX_CScaledNumberDisplayDelegate*	Clone() const AAX_OVERRIDE;
	bool		ValueToString(T value, AAX_CString* valueString) const AAX_OVERRIDE;
	bool		ValueToString(T value, int32_t maxNumChars, AAX_CString* valueString) const AAX_OVERRIDE;
	bool		StringToValue(const AAX_CString& valueString, T* value) const AAX_OVERRIDE;

	// Constructor to set scaling factor
	explicit AAX_CScaledNumberDisplayDelegate(
		double inScaleFactor)
	: mScaleFactor(inScaleFactor ? inScaleFactor : 1.0)
	{}

private:
	double const mScaleFactor;
};




template <typename T, uint32_t Precision, uint32_t SpaceAfter>
AAX_CScaledNumberDisplayDelegate<T,Precision,SpaceAfter>*		AAX_CScaledNumberDisplayDelegate<T,Precision,SpaceAfter>::Clone() const
{
	return new AAX_CScaledNumberDisplayDelegate(*this);
}

template <typename T, uint32_t Precision, uint32_t SpaceAfter>
bool	AAX_CScaledNumberDisplayDelegate<T,Precision,SpaceAfter>::ValueToString(T value, AAX_CString* valueString) const
{
	valueString->Clear();
	
	// Apply scaling factor to the value for display
	double scaledValue = static_cast<double>(value) * mScaleFactor;
	
	valueString->AppendNumber(scaledValue, Precision);

	// Remove negative sign if the displayed value is zero (after rounding)
	if (valueString->Length() > 0 && (*valueString)[0] == '-') {
		// Check if the rest of the string is all zeros or zeros with decimal point
		bool isZero = true;
		for (uint32_t i = 1; i < valueString->Length(); ++i) {
			char c = (*valueString)[i];
			if (c != '0' && c != '.') {
				isZero = false;
				break;
			}
		}
		if (isZero) {
			valueString->Erase(0, 1);
		}
	}

	if (SpaceAfter != 0)
		valueString->Append(" ");		//Added a space after the number for easier display of units.
	return true;
}

template <typename T, uint32_t Precision, uint32_t SpaceAfter>
bool	AAX_CScaledNumberDisplayDelegate<T,Precision,SpaceAfter>::ValueToString(T value, int32_t maxNumChars, AAX_CString* valueString) const
{
	valueString->Clear();
	
	// Apply scaling factor to the value for display
	double scaledValue = static_cast<double>(value) * mScaleFactor;
	
	valueString->AppendNumber(scaledValue, Precision);

	// Remove negative sign if the displayed value is zero (after rounding)
	if (valueString->Length() > 0 && (*valueString)[0] == '-') {
		bool isZero = true;
		for (uint32_t i = 1; i < valueString->Length(); ++i) {
			char c = (*valueString)[i];
			if (c != '0' && c != '.') {
				isZero = false;
				break;
			}
		}
		if (isZero) {
			valueString->Erase(0, 1);
		}
	}

	uint32_t strLen = valueString->Length();
	const uint32_t maxNumCharsUnsigned = (0 <= maxNumChars) ? static_cast<uint32_t>(maxNumChars) : 0;
	if (strLen > maxNumCharsUnsigned)
	{
		valueString->Erase(maxNumCharsUnsigned, strLen-maxNumCharsUnsigned);
		strLen = valueString->Length();
	}
	
	if ( 0 < maxNumCharsUnsigned && strLen == maxNumCharsUnsigned && (*valueString)[maxNumCharsUnsigned-1] == '.')	// Edge case when the decimal point is the last character, we probably shouldn't show it.
	{
		valueString->Erase(maxNumCharsUnsigned-1, 1);
		strLen = valueString->Length();
	}

	if ((SpaceAfter != 0) && (maxNumCharsUnsigned > strLen) && (maxNumCharsUnsigned-strLen > 2))			// Kind of a random threshold for dropping the space after, but seems reasonable for our control surfaces.  (allows dB and Unit prefixes)
		valueString->Append(" ");		//Added a space after the number for easier display of units.
	return true;
}

template <typename T, uint32_t Precision, uint32_t SpaceAfter>
bool	AAX_CScaledNumberDisplayDelegate<T,Precision,SpaceAfter>::StringToValue(const AAX_CString& valueString, T* value) const
{
	double dValue;
	if (valueString.ToDouble(&dValue))
	{
		// Explicitly reject NaN and infinity values for consistent behavior across platforms
		if (!std::isfinite(dValue))
		{
			*value = 0;
			return false;
		}
		
		// Apply inverse scaling factor to convert display value back to logical value
		double scaledValue = dValue / mScaleFactor;
		*value = static_cast<T>(scaledValue);
		return true;	
	}
	*value = 0;
	return false;
}




#endif //AAX_CSCALEDNUMBERDISPLAYDELEGATE_H
