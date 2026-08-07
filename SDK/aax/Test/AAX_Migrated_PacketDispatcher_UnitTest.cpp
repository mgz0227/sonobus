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
 *	\file AAX_Migrated_PacketDispatcher_UnitTest.cpp
 *
 *	\brief Migrated unit tests for AAX_CPacketDispatcher (originally from UnitTest++ framework)
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX_CPacketDispatcher.h"

//==============================================================================
// Test Fixture
//==============================================================================

class AAX_Migrated_PacketDispatcher_Test : public ::testing::Test
{
protected:
    void SetUp() override
    {
        packetDispatcher = new AAX_CPacketDispatcher();
    }
    
    void TearDown() override
    {
        delete packetDispatcher;
    }
    
    AAX_CPacketDispatcher* packetDispatcher;
    
    struct CHugeStructure
    {
        uint32_t mVariable[100];
    };
};

//==============================================================================
// Packet Dispatcher Tests
//==============================================================================

TEST_F(AAX_Migrated_PacketDispatcher_Test, CPacketTest_Basic)
{
    AAX_CPacket packet(123);
    // Initial states
    EXPECT_EQ(123, packet.GetID());
    EXPECT_EQ(true, packet.IsDirty());
    EXPECT_EQ(uint32_t(0), packet.GetSize());
    
    // Check upsizing and downsizing the pointer
    uint32_t* ptr1 = packet.GetPtr<uint32_t>();
    EXPECT_EQ(sizeof(*ptr1), packet.GetSize());
    double* ptr2 = packet.GetPtr<double>();
    EXPECT_EQ(sizeof(*ptr2), packet.GetSize());
    uint32_t* ptr3 = packet.GetPtr<uint32_t>();
    EXPECT_EQ(sizeof(*ptr3), packet.GetSize());
    CHugeStructure* ptr4 = packet.GetPtr<CHugeStructure>();
    EXPECT_EQ(sizeof(*ptr4), packet.GetSize());
    uint32_t* ptr5 = packet.GetPtr<uint32_t>();
    EXPECT_EQ(sizeof(*ptr5), packet.GetSize());
    
    // Check setting dirty to various states
    packet.SetDirty(false);
    EXPECT_EQ(false, packet.IsDirty());
    packet.SetDirty(true);
    EXPECT_EQ(true, packet.IsDirty());
}
