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
 * \file  AAX_CDataBuffer.h
 */ 
/*================================================================================================*/

#pragma once

#ifndef AAX_CDataBuffer_H
#define AAX_CDataBuffer_H

#include "AAX_IDataBuffer.h"
#include "AAX.h"

#include <memory>
#include <array>


/**
 * \brief A convenience class for raw data buffers
 */
template <AAX_CTypeID T, size_t S>
class AAX_CDataBufferOfTypeAndSize : public AAX_IDataBuffer
{
public:
	explicit AAX_CDataBufferOfTypeAndSize (void const * inData)
	: mData{}
	{
		this->Init(inData);
	}

	AAX_CDataBufferOfTypeAndSize(AAX_CDataBufferOfTypeAndSize const & other)
	: mData{}
	{
		this->Init(other.mData.data());
	}

	AAX_CDataBufferOfTypeAndSize(AAX_CDataBufferOfTypeAndSize && other)
	{
		mData = std::move(other.mData);
	}

	~AAX_CDataBufferOfTypeAndSize (void) AAX_OVERRIDE = default;
	
	AAX_CDataBufferOfTypeAndSize& operator= (AAX_CDataBufferOfTypeAndSize const & other)
	{
		if (this != &other)
		{
			this->Init(other.mData.data());
		}
		return *this;
	}

	AAX_CDataBufferOfTypeAndSize& operator= (AAX_CDataBufferOfTypeAndSize && other)
	{
		if (this != &other)
		{
			mData = std::move(other.mData);
		}
		return *this;
	}

	AAX_Result Type(AAX_CTypeID * oType) const AAX_OVERRIDE {
		if (!oType) { return AAX_ERROR_NULL_ARGUMENT; }
		*oType = T;
		return AAX_SUCCESS;
	}

	AAX_Result Size(int32_t * oSize) const AAX_OVERRIDE {
		if (!oSize) { return AAX_ERROR_NULL_ARGUMENT; }
		*oSize = static_cast<int32_t>(S);
		return AAX_SUCCESS;
	}

	AAX_Result Data(void const ** oBuffer) const AAX_OVERRIDE {
		if (!oBuffer) { return AAX_ERROR_NULL_ARGUMENT; }
		*oBuffer = mData.data();
		return AAX_SUCCESS;
	}

private:
	void Init(void const * inData)
	{
		if (inData)
		{
			std::memcpy(mData.data(), inData, S);
		}
		else
		{
			mData.fill(0);
		}
	}

	std::array<uint8_t, S> mData;
};


/**
 * \copydoc AAX_CDataBufferOfTypeAndSize
 */
template <AAX_CTypeID T>
class AAX_CDataBufferOfType : public AAX_IDataBuffer
{
public:
	explicit AAX_CDataBufferOfType (void const * inData, int32_t inSize)
	: mSize{inSize}
	, mData{nullptr}
	{
		this->Init(inData);
	}

	AAX_CDataBufferOfType(AAX_CDataBufferOfType const & other)
	: mSize{other.mSize}
	, mData{nullptr}
	{
		this->Init(other.mData.get());
	}

	AAX_CDataBufferOfType(AAX_CDataBufferOfType && other)
	{
		mSize = other.mSize;
		mData = std::move(other.mData);
	}

	~AAX_CDataBufferOfType (void) AAX_OVERRIDE = default;
	
	AAX_CDataBufferOfType& operator= (AAX_CDataBufferOfType const & other)
	{
		if (this != &other)
		{
			mSize = other.mSize;
			this->Init(other.mData.get());
		}
		return *this;
	}

	AAX_CDataBufferOfType& operator= (AAX_CDataBufferOfType && other)
	{
		if (this != &other)
		{
			mSize = other.mSize;
			mData = std::move(other.mData);
		}
		return *this;
	}

	AAX_Result Type(AAX_CTypeID * oType) const AAX_OVERRIDE {
		if (!oType) { return AAX_ERROR_NULL_ARGUMENT; }
		*oType = T;
		return AAX_SUCCESS;
	}

	AAX_Result Size(int32_t * oSize) const AAX_OVERRIDE {
		if (!oSize) { return AAX_ERROR_NULL_ARGUMENT; }
		*oSize = mSize;
		return AAX_SUCCESS;
	}

	AAX_Result Data(void const ** oBuffer) const AAX_OVERRIDE {
		if (!oBuffer) { return AAX_ERROR_NULL_ARGUMENT; }
		*oBuffer = mData.get();
		return AAX_SUCCESS;
	}

private:
	void Init(void const * inData)
	{
		mData.reset();
		if (mSize > 0)
		{
			mData = std::make_unique<uint8_t[]>(mSize);
			if (inData)
			{
				std::memcpy(mData.get(), inData, mSize);
			}
			else
			{
				std::memset(mData.get(), 0, mSize);
			}
		}
	}

	int32_t mSize;
	std::unique_ptr<uint8_t[]> mData;
};

/**
 * \copydoc AAX_CDataBufferOfTypeAndSize
 */
template <size_t S>
class AAX_CDataBufferOfSize : public AAX_IDataBuffer
{
public:
	explicit AAX_CDataBufferOfSize (AAX_CTypeID inType, void const * inData)
	: mType{inType}
	, mData{}
	{
		this->Init(inData);
	}

	AAX_CDataBufferOfSize(AAX_CDataBufferOfSize const & other)
	: mType{other.mType}
	, mData{}
	{
		this->Init(other.mData.data());
	}

	AAX_CDataBufferOfSize(AAX_CDataBufferOfSize && other)
	{
		mType = other.mType;
		mData = std::move(other.mData);
	}

	~AAX_CDataBufferOfSize (void) AAX_OVERRIDE = default;
	
	AAX_CDataBufferOfSize& operator= (AAX_CDataBufferOfSize const & other)
	{
		if (this != &other)
		{
			mType = other.mType;
			this->Init(other.mData.data());
		}
		return *this;
	}

	AAX_CDataBufferOfSize& operator= (AAX_CDataBufferOfSize && other)
	{
		if (this != &other)
		{
			mType = other.mType;
			mData = std::move(other.mData);
		}
		return *this;
	}

	AAX_Result Type(AAX_CTypeID * oType) const AAX_OVERRIDE {
		if (!oType) { return AAX_ERROR_NULL_ARGUMENT; }
		*oType = mType;
		return AAX_SUCCESS;
	}

	AAX_Result Size(int32_t * oSize) const AAX_OVERRIDE {
		if (!oSize) { return AAX_ERROR_NULL_ARGUMENT; }
		*oSize = static_cast<int32_t>(S);
		return AAX_SUCCESS;
	}

	AAX_Result Data(void const ** oBuffer) const AAX_OVERRIDE {
		if (!oBuffer) { return AAX_ERROR_NULL_ARGUMENT; }
		*oBuffer = mData.data();
		return AAX_SUCCESS;
	}

private:
	void Init(void const * inData)
	{
		if (inData)
		{
			std::memcpy(mData.data(), inData, S);
		}
		else
		{
			mData.fill(0);
		}
	}

	AAX_CTypeID mType;
	std::array<uint8_t, S> mData;
};

/**
 * \copydoc AAX_CDataBufferOfTypeAndSize
 */
class AAX_CDataBuffer : public AAX_IDataBuffer
{
public:
	explicit AAX_CDataBuffer (AAX_CTypeID inType, void const * inData, int32_t inSize)
	: mType{inType}
	, mSize{inSize}
	, mData{nullptr}
	{
		this->Init(inData);
	}

	AAX_CDataBuffer(AAX_CDataBuffer const & other)
	: mType{other.mType}
	, mSize{other.mSize}
	, mData{nullptr}
	{
		this->Init(other.mData.get());
	}

	AAX_CDataBuffer(AAX_CDataBuffer && other)
	{
		mType = other.mType;
		mSize = other.mSize;
		mData = std::move(other.mData);
	}

	~AAX_CDataBuffer (void) AAX_OVERRIDE = default;
	
	AAX_CDataBuffer& operator= (AAX_CDataBuffer const & other)
	{
		if (this != &other)
		{
			mType = other.mType;
			mSize = other.mSize;
			this->Init(other.mData.get());
		}
		return *this;
	}

	AAX_CDataBuffer& operator= (AAX_CDataBuffer && other)
	{
		if (this != &other)
		{
			mType = other.mType;
			mSize = other.mSize;
			mData = std::move(other.mData);
		}
		return *this;
	}

	AAX_Result Type(AAX_CTypeID * oType) const AAX_OVERRIDE {
		if (!oType) { return AAX_ERROR_NULL_ARGUMENT; }
		*oType = mType;
		return AAX_SUCCESS;
	}

	AAX_Result Size(int32_t * oSize) const AAX_OVERRIDE {
		if (!oSize) { return AAX_ERROR_NULL_ARGUMENT; }
		*oSize = mSize;
		return AAX_SUCCESS;
	}

	AAX_Result Data(void const ** oBuffer) const AAX_OVERRIDE {
		if (!oBuffer) { return AAX_ERROR_NULL_ARGUMENT; }
		*oBuffer = mData.get();
		return AAX_SUCCESS;
	}

private:
	void Init(void const * inData)
	{
		mData.reset();
		if (mSize > 0)
		{
			mData = std::make_unique<uint8_t[]>(mSize);
			if (inData)
			{
				std::memcpy(mData.get(), inData, mSize);
			}
			else
			{
				std::memset(mData.get(), 0, mSize);
			}
		}
	}

	AAX_CTypeID mType;
	int32_t mSize;
	std::unique_ptr<uint8_t[]> mData;
};


#endif
