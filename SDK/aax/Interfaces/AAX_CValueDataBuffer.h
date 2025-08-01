/*================================================================================================*/
/*
 *
 * Copyright 2025 Avid Technology, Inc.
 * All rights reserved.
 * 
 * This file is part of the Avid AAX SDK.
 * 
 * The AAX SDK is subject to commercial or open-source licensing.
 * 
 * By using the AAX SDK, you agree to the terms of both the Avid AAX SDK License
 * Agreement and Avid Privacy Policy.
 * 
 * AAX SDK License: https://developer.avid.com/aax
 * Privacy Policy: https://www.avid.com/legal/privacy-policy-statement
 * 
 * Or: You may also use this code under the terms of the GPL v3 (see
 * www.gnu.org/licenses).
 * 
 * THE AAX SDK IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
 * EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
 * DISCLAIMED.
 *
 */

/**  
 * \file  AAX_CValueDataBuffer.h
 */ 
/*================================================================================================*/

#pragma once

#ifndef AAX_CValueDataBuffer_H
#define AAX_CValueDataBuffer_H

#include "AAX_IDataBuffer.h"
#include "AAX.h"

#include <limits>
#include <type_traits>


/**
 * \brief A convenience class for single-value data buffers
 * 
 * The data payload is of type \c D
 */
template <AAX_CTypeID T, class D>
class AAX_CValueDataBufferOfType : public AAX_IDataBuffer
{
public:
	explicit AAX_CValueDataBufferOfType (D const & inData) : mData{inData} {}
	explicit AAX_CValueDataBufferOfType (D && inData) : mData{inData} {}

	AAX_CValueDataBufferOfType(AAX_CValueDataBufferOfType const &) = default;
	AAX_CValueDataBufferOfType(AAX_CValueDataBufferOfType &&) = default;

	~AAX_CValueDataBufferOfType (void) AAX_OVERRIDE = default;
	
	AAX_CValueDataBufferOfType& operator= (AAX_CValueDataBufferOfType const & other) = default;
	AAX_CValueDataBufferOfType& operator= (AAX_CValueDataBufferOfType && other) = default;

	AAX_Result Type(AAX_CTypeID * oType) const AAX_OVERRIDE {
		if (!oType) { return AAX_ERROR_NULL_ARGUMENT; }
		*oType = T;
		return AAX_SUCCESS;
	}
	AAX_Result Size(int32_t * oSize) const AAX_OVERRIDE {
		if (!oSize) { return AAX_ERROR_NULL_ARGUMENT; }
		auto const size = sizeof(D);
		static_assert(std::numeric_limits<decltype(size)>::max() >= std::numeric_limits<std::remove_pointer<decltype(oSize)>::type>::max(),
			"size variable may not represent all positive values of oSize");
		if (size > std::numeric_limits<std::remove_pointer<decltype(oSize)>::type>::max()) {
			return AAX_ERROR_SIGNED_INT_OVERFLOW;
		}
		*oSize = static_cast<std::remove_pointer<decltype(oSize)>::type>(size);
		return AAX_SUCCESS;
	}
	AAX_Result Data(void const ** oBuffer) const AAX_OVERRIDE {
		if (!oBuffer) { return AAX_ERROR_NULL_ARGUMENT; }
		*oBuffer = &mData;
		return AAX_SUCCESS;
	}
private:
	D const mData;
};

/**
 * \copydoc AAX_CValueDataBufferOfType
 */
template <class D>
class AAX_CValueDataBuffer : public AAX_IDataBuffer
{
public:
	AAX_CValueDataBuffer (AAX_CTypeID inType, D && inData) : mType{inType}, mData{inData} {}

	AAX_CValueDataBuffer(AAX_CValueDataBuffer const &) = default;
	AAX_CValueDataBuffer(AAX_CValueDataBuffer &&) = default;

	~AAX_CValueDataBuffer (void) AAX_OVERRIDE = default;
	
	AAX_CValueDataBuffer& operator= (AAX_CValueDataBuffer const & other) = default;
	AAX_CValueDataBuffer& operator= (AAX_CValueDataBuffer && other) = default;

	AAX_Result Type(AAX_CTypeID * oType) const AAX_OVERRIDE {
		if (!oType) { return AAX_ERROR_NULL_ARGUMENT; }
		*oType = mType;
		return AAX_SUCCESS;
	}
	AAX_Result Size(int32_t * oSize) const AAX_OVERRIDE {
		if (!oSize) { return AAX_ERROR_NULL_ARGUMENT; }
		auto const size = sizeof(D);
		static_assert(std::numeric_limits<decltype(size)>::max() >= std::numeric_limits<std::remove_pointer<decltype(oSize)>::type>::max(),
			"size variable may not represent all positive values of oSize");
		if (size > std::numeric_limits<std::remove_pointer<decltype(oSize)>::type>::max()) {
			return AAX_ERROR_SIGNED_INT_OVERFLOW;
		}
		*oSize = static_cast<std::remove_pointer<decltype(oSize)>::type>(size);
		return AAX_SUCCESS;
	}
	AAX_Result Data(void const ** oBuffer) const AAX_OVERRIDE {
		if (!oBuffer) { return AAX_ERROR_NULL_ARGUMENT; }
		*oBuffer = &mData;
		return AAX_SUCCESS;
	}
private:
	AAX_CTypeID const mType;
	D const mData;
};

#endif
