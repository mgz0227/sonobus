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
 *	\file AAX_CScaledDisplayDelegateDecorator.h
 *
 *	\brief A scaled display delegate decorator.
 *
 */ 
/*================================================================================================*/

#ifndef AAX_CSCALEDDISPLAYDELEGATEDECORATOR_H
#define AAX_CSCALEDDISPLAYDELEGATEDECORATOR_H

#include "AAX_IDisplayDelegateDecorator.h"
#include "AAX_CString.h"

/**	\brief A scaled display delegate decorator conforming to AAX_IDisplayDelegateDecorator
	
	\details
	This display delegate decorator applies a constant scaling factor to the numerical value
	that is passed to the wrapped display delegate. The scaling factor is applied during 
	conversion - for example, a scaling factor of 100 will convert a logical value of 1 to 
	the display string "100" and vice versa, or a logical value of 0.01 to the display 
	string "1" and vice versa.
	
	This decorator can wrap any display delegate that implements AAX_IDisplayDelegate<T>.

	\ingroup AAXLibraryFeatures_ParameterManager_DisplayDelegates_Decorators

 */
template <typename T>
class AAX_CScaledDisplayDelegateDecorator : public AAX_IDisplayDelegateDecorator<T>
{
public:
	/**	Constructor
	 *	
	 *	\param[in] displayDelegate
	 *		The display delegate to wrap
	 *	\param[in] scaleFactor
	 *		The scaling factor to apply to values
	 */
	AAX_CScaledDisplayDelegateDecorator(const AAX_IDisplayDelegate<T>& displayDelegate, double scaleFactor);
	
	/**	Constructor (rvalue reference)
	 *	
	 *	\param[in] displayDelegate
	 *		The display delegate to wrap (temporary object)
	 *	\param[in] scaleFactor
	 *		The scaling factor to apply to values
	 */
	AAX_CScaledDisplayDelegateDecorator(AAX_IDisplayDelegate<T>&& displayDelegate, double scaleFactor);

	//Virtual Overrides
	AAX_CScaledDisplayDelegateDecorator<T>*	Clone() const AAX_OVERRIDE;
	bool		ValueToString(T value, AAX_CString* valueString) const AAX_OVERRIDE;
	bool		ValueToString(T value, int32_t maxNumChars, AAX_CString* valueString) const AAX_OVERRIDE;
	bool		StringToValue(const AAX_CString& valueString, T* value) const AAX_OVERRIDE;

private:
	double const mScaleFactor;
};

// Template implementation

template <typename T>
AAX_CScaledDisplayDelegateDecorator<T>::AAX_CScaledDisplayDelegateDecorator(
	const AAX_IDisplayDelegate<T>& displayDelegate, 
	double scaleFactor)
: AAX_IDisplayDelegateDecorator<T>(displayDelegate)
, mScaleFactor(scaleFactor ? scaleFactor : 1.0)  // Guard against zero scale factor
{
}

template <typename T>
AAX_CScaledDisplayDelegateDecorator<T>::AAX_CScaledDisplayDelegateDecorator(
	AAX_IDisplayDelegate<T>&& displayDelegate, 
	double scaleFactor)
: AAX_IDisplayDelegateDecorator<T>(std::move(displayDelegate))
, mScaleFactor(scaleFactor ? scaleFactor : 1.0)  // Guard against zero scale factor
{
}

template <typename T>
AAX_CScaledDisplayDelegateDecorator<T>* AAX_CScaledDisplayDelegateDecorator<T>::Clone() const
{
	return new AAX_CScaledDisplayDelegateDecorator(*this);
}

template <typename T>
bool AAX_CScaledDisplayDelegateDecorator<T>::ValueToString(T value, AAX_CString* valueString) const
{
	// Apply scaling factor to the value before passing to wrapped delegate
	T scaledValue = static_cast<T>(static_cast<double>(value) * mScaleFactor);
	return AAX_IDisplayDelegateDecorator<T>::ValueToString(scaledValue, valueString);
}

template <typename T>
bool AAX_CScaledDisplayDelegateDecorator<T>::ValueToString(T value, int32_t maxNumChars, AAX_CString* valueString) const
{
	// Apply scaling factor to the value before passing to wrapped delegate
	T scaledValue = static_cast<T>(static_cast<double>(value) * mScaleFactor);
	return AAX_IDisplayDelegateDecorator<T>::ValueToString(scaledValue, maxNumChars, valueString);
}

template <typename T>
bool AAX_CScaledDisplayDelegateDecorator<T>::StringToValue(const AAX_CString& valueString, T* value) const
{
	// Validate the string
	double dValue;
	if (!valueString.ToDouble(&dValue))
	{
		*value = T(0);
		return false;
	}
	
	// Delegate to the wrapped delegate
	T parsedValue;
	bool success = AAX_IDisplayDelegateDecorator<T>::StringToValue(valueString, &parsedValue);
	
	if (success)
	{
		// Apply inverse scaling factor
		*value = static_cast<T>(static_cast<double>(parsedValue) / mScaleFactor);
	}
	else
	{
		*value = T(0);
	}
	
	return success;
}

#endif //AAX_CSCALEDDISPLAYDELEGATEDECORATOR_H
