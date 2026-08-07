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
 *	\file AAX_MockDisplayDelegate.h
 *
 *	\brief Mock display delegate for unit testing
 *
 */ 
/*================================================================================================*/


#ifndef AAX_MOCKDISPLAYDELEGATE_H
#define AAX_MOCKDISPLAYDELEGATE_H

#include "AAX_IDisplayDelegate.h"
#include "AAX_CString.h"
#include <sstream>
#include <string>

/** \brief A simple mock display delegate for unit testing
 *
 *  \details
 *  This mock provides predictable, well-defined behavior for testing decorator classes
 *  without dependencies on other display delegate implementations. It converts values
 *  to simple string representations without decimal places or special formatting.
 *
 *  Behavior:
 *  - ValueToString: Converts values to simple string representation (e.g., 123.0f -> "123", 123.45f -> "123.45")
 *  - ValueToString with maxNumChars: Truncates strings that exceed maxNumChars
 *  - StringToValue: Parses simple numeric strings back to values
 *  - Clone: Creates a copy of the mock delegate
 */
template <typename T>
class AAX_MockDisplayDelegate : public AAX_IDisplayDelegate<T>
{
public:
    /** \brief Constructor
     *
     *  \param[in] removeTrailingZeros If true, removes trailing zeros from decimal representations
     */
    explicit AAX_MockDisplayDelegate(bool removeTrailingZeros = true);
    
    // Virtual Overrides
    AAX_MockDisplayDelegate<T>* Clone() const override;
    bool ValueToString(T value, AAX_CString* valueString) const override;
    bool ValueToString(T value, int32_t maxNumChars, AAX_CString* valueString) const override;
    bool StringToValue(const AAX_CString& valueString, T* value) const override;

private:
    bool mRemoveTrailingZeros;
    
    // Helper method to clean up decimal representation
    std::string CleanDecimalString(const std::string& str) const;
};

// Template implementation

template <typename T>
AAX_MockDisplayDelegate<T>::AAX_MockDisplayDelegate(bool removeTrailingZeros)
    : AAX_IDisplayDelegate<T>()
    , mRemoveTrailingZeros(removeTrailingZeros)
{
}

template <typename T>
AAX_MockDisplayDelegate<T>* AAX_MockDisplayDelegate<T>::Clone() const
{
    return new AAX_MockDisplayDelegate<T>(*this);
}

template <typename T>
bool AAX_MockDisplayDelegate<T>::ValueToString(T value, AAX_CString* valueString) const
{
    if (!valueString) return false;
    
    std::ostringstream oss;
    oss << value;
    
    std::string result = oss.str();
    
    // Clean up decimal representation if requested
    if (mRemoveTrailingZeros) {
        result = CleanDecimalString(result);
    }
    
    *valueString = AAX_CString(result.c_str());
    return true;
}

template <typename T>
bool AAX_MockDisplayDelegate<T>::ValueToString(T value, int32_t maxNumChars, AAX_CString* valueString) const
{
    if (!ValueToString(value, valueString)) {
        return false;
    }
    
    // Truncate if necessary
    if (maxNumChars >= 0 && static_cast<int32_t>(valueString->Length()) > maxNumChars) {
        AAX_CString truncated;
        valueString->SubString(0, static_cast<uint32_t>(maxNumChars), &truncated);
        *valueString = truncated;
    }
    
    return true;
}

template <typename T>
bool AAX_MockDisplayDelegate<T>::StringToValue(const AAX_CString& valueString, T* value) const
{
    if (!value) return false;
    
    std::string str(valueString.Get());
    if (str.empty()) {
        *value = T(0);
        return true;
    }
    
    std::istringstream iss(str);
    T result;
    
    if (iss >> result) {
        *value = result;
        return true;
    }
    
    // Failed to parse
    *value = T(0);
    return false;
}

template <typename T>
std::string AAX_MockDisplayDelegate<T>::CleanDecimalString(const std::string& str) const
{
    // Find decimal point
    size_t decimalPos = str.find('.');
    if (decimalPos == std::string::npos) {
        return str; // No decimal point, return as-is
    }
    
    // Remove trailing zeros after decimal point
    std::string result = str;
    while (result.length() > decimalPos + 1 && result.back() == '0') {
        result.pop_back();
    }
    
    // Remove decimal point if no digits after it
    if (result.back() == '.') {
        result.pop_back();
    }
    
    return result;
}

// Specialization for bool to provide more predictable behavior
template <>
class AAX_MockDisplayDelegate<bool> : public AAX_IDisplayDelegate<bool>
{
public:
    AAX_MockDisplayDelegate(bool removeTrailingZeros = true) : AAX_IDisplayDelegate<bool>() {}
    
    AAX_MockDisplayDelegate<bool>* Clone() const override {
        return new AAX_MockDisplayDelegate<bool>(*this);
    }

    bool ValueToString(bool value, AAX_CString* valueString) const override {
        if (!valueString) return false;
        *valueString = AAX_CString(value ? "true" : "false");
        return true;
    }

    bool ValueToString(bool value, int32_t maxNumChars, AAX_CString* valueString) const override {
        if (!ValueToString(value, valueString)) return false;

        if (maxNumChars >= 0 && static_cast<int32_t>(valueString->Length()) > maxNumChars) {
            AAX_CString truncated;
            valueString->SubString(0, static_cast<uint32_t>(maxNumChars), &truncated);
            *valueString = truncated;
        }

        return true;
    }

    bool StringToValue(const AAX_CString& valueString, bool* value) const override {
        if (!value) return false;
        
        std::string str(valueString.Get());
        if (str == "true" || str == "1") {
            *value = true;
            return true;
        } else if (str == "false" || str == "0" || str.empty()) {
            *value = false;
            return true;
        }
        
        *value = false;
        return false;
    }
};

#endif // AAX_MOCKDISPLAYDELEGATE_H
