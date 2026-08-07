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
 *	\file AAX_Migrated_Size_UnitTest.cpp
 *
 *	\brief Migrated unit tests for Size validation (originally from UnitTest++ framework)
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include <stdint.h>
#include "acfbasetypes.h"
#include "AAX.h"
#include "AAX_GUITypes.h"

//==============================================================================
// Basic Type Size Test Fixture
//==============================================================================

class AAX_Migrated_BasicTypeSize_Test : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

//==============================================================================
// Basic Type Size Tests
//==============================================================================

TEST_F(AAX_Migrated_BasicTypeSize_Test, ACFIntTest)
{
    EXPECT_TRUE(sizeof(int32_t) == sizeof(acfSInt32));
    EXPECT_TRUE(sizeof(uint32_t) == sizeof(acfUInt32));
    EXPECT_TRUE(sizeof(int64_t) == sizeof(acfSInt64));
    EXPECT_TRUE(sizeof(uint64_t) == sizeof(acfUInt64));
}

//==============================================================================
// AAX Type Size Test Fixture
//==============================================================================

class AAX_Migrated_AAXTypeSize_Test : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

//==============================================================================
// AAX Type Size Tests
//==============================================================================

TEST_F(AAX_Migrated_AAXTypeSize_Test, Type_Test)
{
    EXPECT_TRUE(sizeof(AAX_CIndex) == 4);
    EXPECT_TRUE(sizeof(AAX_CCount) == 4);
    EXPECT_TRUE(sizeof(AAX_CBoolean) == 1);
    EXPECT_TRUE(sizeof(AAX_CSelector) == 4);
    EXPECT_TRUE(sizeof(AAX_CTimestamp) == 8);
    EXPECT_TRUE(sizeof(AAX_CTimeOfDay) == 8);
    EXPECT_TRUE(sizeof(AAX_CTransportCounter) == 8);
    EXPECT_TRUE(sizeof(AAX_CSampleRate) == 4);
    EXPECT_TRUE(sizeof(AAX_CTypeID) == 4);
    EXPECT_TRUE(sizeof(AAX_CFieldIndex) == 4);
    EXPECT_TRUE(sizeof(AAX_CComponentID) == 4);
    EXPECT_TRUE(sizeof(AAX_CMeterID) == 4);
    EXPECT_TRUE(sizeof(AAX_CParamID) == sizeof(void*));
    EXPECT_TRUE(sizeof(AAX_CEffectID) == sizeof(void*));
}

TEST_F(AAX_Migrated_AAXTypeSize_Test, Chunk_Struct_Test)
{
    // AAX_SPlugInChunk
#if (defined(_WIN64) || defined(__LP64__))
    static_assert(sizeof(AAX_SPlugInChunk) == size_t(60), "AAX_SPlugInChunk must be 60 bytes (8 byte aligned)"); // 57, packed  (8 byte aligned)
#else
    static_assert(sizeof(AAX_SPlugInChunk) == size_t(58), "AAX_SPlugInChunk must be 58 bytes (2 byte aligned)"); // 57, packed   (2 byte aligned)
#endif
    static_assert(offsetof(AAX_SPlugInChunk, fSize) == size_t(0), "AAX_SPlugInChunk fSize offset must be 0");
    static_assert(offsetof(AAX_SPlugInChunk, fVersion) == size_t(4), "AAX_SPlugInChunk fVersion offset must be 4");
    static_assert(offsetof(AAX_SPlugInChunk, fManufacturerID) == size_t(8), "AAX_SPlugInChunk fManufacturerID offset must be 8");
    static_assert(offsetof(AAX_SPlugInChunk, fProductID) == size_t(12), "AAX_SPlugInChunk fProductID offset must be 12");
    static_assert(offsetof(AAX_SPlugInChunk, fPlugInID) == size_t(16), "AAX_SPlugInChunk fPlugInID offset must be 16");
    static_assert(offsetof(AAX_SPlugInChunk, fChunkID) == size_t(20), "AAX_SPlugInChunk fChunkID offset must be 20");
    static_assert(offsetof(AAX_SPlugInChunk, fName) == size_t(24), "AAX_SPlugInChunk fName offset must be 24");
    static_assert(offsetof(AAX_SPlugInChunk, fData) == size_t(56), "AAX_SPlugInChunk fData offset must be 56");
    
    // AAX_SPlugInChunkHeader
    EXPECT_EQ(offsetof(AAX_SPlugInChunk, fSize), offsetof(AAX_SPlugInChunkHeader, fSize));
    EXPECT_EQ(offsetof(AAX_SPlugInChunk, fVersion), offsetof(AAX_SPlugInChunkHeader, fVersion));
    EXPECT_EQ(offsetof(AAX_SPlugInChunk, fManufacturerID), offsetof(AAX_SPlugInChunkHeader, fManufacturerID));
    EXPECT_EQ(offsetof(AAX_SPlugInChunk, fProductID), offsetof(AAX_SPlugInChunkHeader, fProductID));
    EXPECT_EQ(offsetof(AAX_SPlugInChunk, fPlugInID), offsetof(AAX_SPlugInChunkHeader, fPlugInID));
    EXPECT_EQ(offsetof(AAX_SPlugInChunk, fChunkID), offsetof(AAX_SPlugInChunkHeader, fChunkID));
    EXPECT_EQ(offsetof(AAX_SPlugInChunk, fName), offsetof(AAX_SPlugInChunkHeader, fName));
}

TEST_F(AAX_Migrated_AAXTypeSize_Test, AAX_SPlugInIdentifierTriad_TEST)
{
    static_assert(sizeof(AAX_SPlugInIdentifierTriad) == size_t(12), "AAX_SPlugInIdentifierTriad must be 12 bytes");
    static_assert(offsetof(AAX_SPlugInIdentifierTriad, mManufacturerID) == size_t(0), "mManufacturerID offset must be 0");
    static_assert(offsetof(AAX_SPlugInIdentifierTriad, mProductID) == size_t(4), "mProductID offset must be 4");
    static_assert(offsetof(AAX_SPlugInIdentifierTriad, mPlugInID) == size_t(8), "mPlugInID offset must be 8");
}

TEST_F(AAX_Migrated_AAXTypeSize_Test, Midi_Struct_Test)
{
#if (defined(_WIN64) || defined(__LP64__))
    //64 bit windows and 64 bit macOS  (THESE ARE CHANGED TO BE 8 BYTE ALIGNED)
    static_assert(sizeof(AAX_CMidiPacket) == size_t(16), "AAX_CMidiPacket must be 16 bytes (8 byte aligned)");  //13, packed.  Alg requires 8 byte aligned.
    static_assert(offsetof(AAX_CMidiPacket, mTimestamp) == size_t(0), "mTimestamp offset must be 0");
    static_assert(offsetof(AAX_CMidiPacket, mLength) == size_t(4), "mLength offset must be 4");
    static_assert(offsetof(AAX_CMidiPacket, mData) == size_t(8), "mData offset must be 8");
    static_assert(offsetof(AAX_CMidiPacket, mIsImmediate) == size_t(12), "mIsImmediate offset must be 12");

    static_assert(sizeof(AAX_CMidiStream) == size_t(16), "AAX_CMidiStream must be 16 bytes (8 byte aligned)");  //12, packed.  Alg requires 8 byte aligned.
    static_assert(offsetof(AAX_CMidiStream, mBufferSize) == size_t(0), "mBufferSize offset must be 0");
    static_assert(offsetof(AAX_CMidiStream, mBuffer) == size_t(8), "mBuffer offset must be 8");
#else
   //<DMT> In 32 bit, instead of 8 byte alignement values, I believe that this is what is locked down for PT10 compatibility.
#ifdef _WIN32
    static_assert(sizeof(AAX_CMidiPacket) == size_t(14), "AAX_CMidiPacket must be 14 bytes (32-bit Windows)");  //13, packed.
#else	//GNU
    static_assert(sizeof(AAX_CMidiPacket) == size_t(16), "AAX_CMidiPacket must be 16 bytes (32-bit macOS)");  //13, packed. (for some reason, macOS is different here)
#endif
    static_assert(offsetof(AAX_CMidiPacket, mTimestamp) == size_t(0), "mTimestamp offset must be 0");
    static_assert(offsetof(AAX_CMidiPacket, mLength) == size_t(4), "mLength offset must be 4");
    static_assert(offsetof(AAX_CMidiPacket, mData) == size_t(8), "mData offset must be 8");
    static_assert(offsetof(AAX_CMidiPacket, mIsImmediate) == size_t(12), "mIsImmediate offset must be 12");

    static_assert(sizeof(AAX_CMidiStream) == size_t(8), "AAX_CMidiStream must be 8 bytes (32-bit)");  //8, packed.  Original unit tests claim this should be 5 bytes packed, but that looks wrong to me.
    static_assert(offsetof(AAX_CMidiStream, mBufferSize) == size_t(0), "mBufferSize offset must be 0");
    static_assert(offsetof(AAX_CMidiStream, mBuffer) == size_t(4), "mBuffer offset must be 4");
#endif
}

TEST_F(AAX_Migrated_AAXTypeSize_Test, GUITypes_Test)
{
    EXPECT_TRUE(sizeof(AAX_Point) == 8);
    EXPECT_TRUE(sizeof(AAX_Rect) == 16);
}
