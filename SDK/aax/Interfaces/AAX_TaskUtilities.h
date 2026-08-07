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
 *	\file AAX_TaskUtilities.h
 *
 */ 
/*================================================================================================*/

/// @cond ignore
#pragma once
#ifndef AAX_TASKUTILITIES_H
#define AAX_TASKUTILITIES_H
/// @endcond

#include "AAX_ITask.h"
#include "AAX_IACFDataBuffer.h"
#include "AAX_Errors.h"
#include "AAX.h"
#include "ACF/ConstACFPtr.h"
#include <string>
#include <vector>

namespace AAX
{
	namespace Task
	{
		// Base implementation: copy data of type T from the task argument data buffer to outArgumentValue
		template <typename T>
		inline AAX_Result GetArgument(AAX_ITask const & inTask, AAX_CTypeID inArgumentType, T & outArgumentValue)
		{
			AAX_Result err{AAX_SUCCESS};
			ConstACFPtr<AAX_IACFDataBuffer> argBuffer(inTask.GetArgumentOfType(inArgumentType));
			if (argBuffer.isNull()) {
				return AAX_ERROR_INVALID_ARGUMENT;
			}
			AAX_CTypeID dataType{kAAX_TypeID_Undefined};
			if (err = argBuffer->Type(&dataType); AAX_SUCCESS != err) {
				return err;
			}
			if (inArgumentType == dataType) { // data buffer type must match argument type for task arguments
				T const * argDataAddress{nullptr};
				if (err = argBuffer->Data(reinterpret_cast<void const **>(&argDataAddress)); AAX_SUCCESS != err) {
					return err;
				}
				if (argDataAddress) {
					outArgumentValue = *argDataAddress;
				}
			}
			return err;
		}

		// Specialization for copying data to a std::vector - the data buffer must be an array of T
		template <typename T>
		inline AAX_Result GetArgument(AAX_ITask const & inTask, AAX_CTypeID inArgumentType, std::vector<T> & outVector)
		{
			AAX_Result err{AAX_SUCCESS};
			ConstACFPtr<AAX_IACFDataBuffer> argBuffer(inTask.GetArgumentOfType(inArgumentType));
			if (argBuffer.isNull()) {
				return AAX_ERROR_INVALID_ARGUMENT;
			}
			AAX_CTypeID dataType{kAAX_TypeID_Undefined};
			if (err = argBuffer->Type(&dataType); AAX_SUCCESS != err) {
				return err;
			}
			if (inArgumentType == dataType) { // data buffer type must match argument type for task arguments
				T const * argDataAddress{nullptr};
				int32_t numElements{0};
				if (err = argBuffer->Data(reinterpret_cast<void const **>(&argDataAddress)); AAX_SUCCESS != err) {
					return err;
				}
				if (argDataAddress) {
					int32_t numBytes{0};
					err = argBuffer->Size(&numBytes);
					if (AAX_SUCCESS != err) {
						return err;
					}
					numElements = numBytes / sizeof(T);
					outVector.assign(argDataAddress, argDataAddress + numElements);
				}
			}
			return err;
		}

		// Specialization for getting the data buffer address as a C-string
		template <>
		inline AAX_Result GetArgument(AAX_ITask const & inTask, AAX_CTypeID inArgumentType, char const * & outCString)
		{
			AAX_Result err{AAX_SUCCESS};
			ConstACFPtr<AAX_IACFDataBuffer> argBuffer(inTask.GetArgumentOfType(inArgumentType));
			if (argBuffer.isNull()) {
				return AAX_ERROR_INVALID_ARGUMENT;
			}
			AAX_CTypeID dataType{kAAX_TypeID_Undefined};
			if (err = argBuffer->Type(&dataType); AAX_SUCCESS != err) {
				return err;
			}
			if (inArgumentType == dataType) { // data buffer type must match argument type for task arguments
				char const * argDataCStr{nullptr};
				if (err = argBuffer->Data(reinterpret_cast<void const **>(&argDataCStr)); AAX_SUCCESS != err) {
					return err;
				}
				if (argDataCStr) {
					outCString = argDataCStr;
				}
			}
			return err;
		}

		// Specialization for copying C-string data to a std::string
		template <>
		inline AAX_Result GetArgument(AAX_ITask const & inTask, AAX_CTypeID inArgumentType, std::string & outString)
		{
			AAX_Result err{AAX_SUCCESS};
			char const * argDataCStr{nullptr};
			err = GetArgument(inTask, inArgumentType, argDataCStr);
			if (AAX_SUCCESS != err) {
				return err;
			}
			if (argDataCStr) {
				outString = argDataCStr;
			}
			return err;
		}
	}
}

/// @cond ignore
#endif // AAX_TASKUTILITIES_H
/// @endcond
