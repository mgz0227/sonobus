/*================================================================================================*/
/*
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
 *	\file AAX_MIDIUtilities_UnitTest.cpp
 *
 *	\brief Unit tests for AAX_MIDIUtilities.h utility functions
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX_MIDIUtilities.h"
#include "AAX.h"

//--------------------------------------------------------------------
#pragma mark - Test fixture

class AAX_MIDIUtilities : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
    
    // Helper function to create a MIDI packet with specified data
    AAX_CMidiPacket CreateMIDIPacket(uint8_t status, uint8_t data1 = 0, uint8_t data2 = 0, uint8_t data3 = 0)
    {
        AAX_CMidiPacket packet;
        packet.mTimestamp = 0;
        packet.mLength = 3; // Most MIDI messages are 3 bytes
        packet.mData[0] = status;
        packet.mData[1] = data1;
        packet.mData[2] = data2;
        packet.mData[3] = data3;
        packet.mIsImmediate = false;
        return packet;
    }
    
    // Helper function to create a system message packet (1 byte)
    AAX_CMidiPacket CreateSystemMIDIPacket(uint8_t status)
    {
        AAX_CMidiPacket packet;
        packet.mTimestamp = 0;
        packet.mLength = 1;
        packet.mData[0] = status;
        packet.mData[1] = 0;
        packet.mData[2] = 0;
        packet.mData[3] = 0;
        packet.mIsImmediate = false;
        return packet;
    }
};

//--------------------------------------------------------------------
#pragma mark - IsNoteOn Tests

TEST_F(AAX_MIDIUtilities, IsNoteOn)
{
    using namespace AAX;
    
    // Test valid Note On messages
    AAX_CMidiPacket noteOnPacket = CreateMIDIPacket(0x90, 60, 100); // Note On, C4, velocity 100
    EXPECT_TRUE(IsNoteOn(&noteOnPacket));
    
    AAX_CMidiPacket noteOnCh16 = CreateMIDIPacket(0x9F, 127, 1); // Note On, channel 16, highest note, velocity 1
    EXPECT_TRUE(IsNoteOn(&noteOnCh16));
    
    // Test Note On with velocity 0 (should be false - this is treated as Note Off)
    AAX_CMidiPacket noteOnVel0 = CreateMIDIPacket(0x90, 60, 0); // Note On, C4, velocity 0
    EXPECT_FALSE(IsNoteOn(&noteOnVel0));
    
    // Test Note Off messages
    AAX_CMidiPacket noteOffPacket = CreateMIDIPacket(0x80, 60, 100); // Note Off, C4
    EXPECT_FALSE(IsNoteOn(&noteOffPacket));
    
    // Test other MIDI messages
    AAX_CMidiPacket controlChange = CreateMIDIPacket(0xB0, 7, 100); // Control Change
    EXPECT_FALSE(IsNoteOn(&controlChange));
    
    AAX_CMidiPacket programChange = CreateMIDIPacket(0xC0, 1, 0); // Program Change
    EXPECT_FALSE(IsNoteOn(&programChange));
    
    // Test null pointer
    EXPECT_FALSE(IsNoteOn(nullptr));
}

//--------------------------------------------------------------------
#pragma mark - IsNoteOff Tests

TEST_F(AAX_MIDIUtilities, IsNoteOff)
{
    using namespace AAX;
    
    // Test valid Note Off messages
    AAX_CMidiPacket noteOffPacket = CreateMIDIPacket(0x80, 60, 100); // Note Off, C4
    EXPECT_TRUE(IsNoteOff(&noteOffPacket));
    
    AAX_CMidiPacket noteOffCh16 = CreateMIDIPacket(0x8F, 127, 0); // Note Off, channel 16, highest note
    EXPECT_TRUE(IsNoteOff(&noteOffCh16));
    
    // Test Note On with velocity 0 (should be true - treated as Note Off)
    AAX_CMidiPacket noteOnVel0 = CreateMIDIPacket(0x90, 60, 0); // Note On, C4, velocity 0
    EXPECT_TRUE(IsNoteOff(&noteOnVel0));
    
    // Test valid Note On messages with non-zero velocity
    AAX_CMidiPacket noteOnPacket = CreateMIDIPacket(0x90, 60, 100); // Note On, C4, velocity 100
    EXPECT_FALSE(IsNoteOff(&noteOnPacket));
    
    // Test other MIDI messages
    AAX_CMidiPacket controlChange = CreateMIDIPacket(0xB0, 7, 100); // Control Change
    EXPECT_FALSE(IsNoteOff(&controlChange));
    
    // Test null pointer
    EXPECT_FALSE(IsNoteOff(nullptr));
}

//--------------------------------------------------------------------
#pragma mark - IsAllNotesOff Tests

TEST_F(AAX_MIDIUtilities, IsAllNotesOff)
{
    using namespace AAX;
    
    // Test PolyOff message (always returns true regardless of data2)
    AAX_CMidiPacket polyOff = CreateMIDIPacket(0xB0, eChannelModeData_PolyOff, 0); // Poly Off
    EXPECT_TRUE(IsAllNotesOff(&polyOff));
    
    AAX_CMidiPacket polyOffNonZero = CreateMIDIPacket(0xB0, eChannelModeData_PolyOff, 100); // Poly Off with non-zero data2
    EXPECT_TRUE(IsAllNotesOff(&polyOffNonZero));
    
    // Test All Sound Off with data2 = 0
    AAX_CMidiPacket allSoundOff = CreateMIDIPacket(0xB0, eChannelModeData_AllSoundOff, 0);
    EXPECT_TRUE(IsAllNotesOff(&allSoundOff));
    
    // Test All Notes Off with data2 = 0
    AAX_CMidiPacket allNotesOff = CreateMIDIPacket(0xB0, eChannelModeData_AllNotesOff, 0);
    EXPECT_TRUE(IsAllNotesOff(&allNotesOff));
    
    // Test Omni Off with data2 = 0
    AAX_CMidiPacket omniOff = CreateMIDIPacket(0xB0, eChannelModeData_OmniOff, 0);
    EXPECT_TRUE(IsAllNotesOff(&omniOff));
    
    // Test Omni On with data2 = 0
    AAX_CMidiPacket omniOn = CreateMIDIPacket(0xB0, eChannelModeData_OmniOn, 0);
    EXPECT_TRUE(IsAllNotesOff(&omniOn));
    
    // Test Poly On with data2 = 0
    AAX_CMidiPacket polyOn = CreateMIDIPacket(0xB0, eChannelModeData_PolyOn, 0);
    EXPECT_TRUE(IsAllNotesOff(&polyOn));
    
    // Test channel mode messages with non-zero data2 (should be false)
    AAX_CMidiPacket allSoundOffNonZero = CreateMIDIPacket(0xB0, eChannelModeData_AllSoundOff, 1);
    EXPECT_FALSE(IsAllNotesOff(&allSoundOffNonZero));
    
    // Test non-channel mode messages
    AAX_CMidiPacket controlChange = CreateMIDIPacket(0xB0, 7, 100); // Control Change (CC 7)
    EXPECT_FALSE(IsAllNotesOff(&controlChange));
    
    AAX_CMidiPacket noteOn = CreateMIDIPacket(0x90, 60, 100); // Note On
    EXPECT_FALSE(IsAllNotesOff(&noteOn));
    
    // Test null pointer
    EXPECT_FALSE(IsAllNotesOff(nullptr));
}

//--------------------------------------------------------------------
#pragma mark - Pro Tools Click Tests

TEST_F(AAX_MIDIUtilities, IsAccentedClick)
{
    using namespace AAX;
    
    // Test valid accented click message
    AAX_CMidiPacket accentedClick = CreateMIDIPacket(0x90, eSpecialData_AccentedClick, 0); // Note On, channel 0, accented click
    EXPECT_TRUE(IsAccentedClick(&accentedClick));
    
    // Test unaccented click message
    AAX_CMidiPacket unaccentedClick = CreateMIDIPacket(0x90, eSpecialData_UnaccentedClick, 0); // Note On, channel 0, unaccented click
    EXPECT_FALSE(IsAccentedClick(&unaccentedClick));
    
    // Test Note On with non-zero channel (should be false)
    AAX_CMidiPacket noteOnCh1 = CreateMIDIPacket(0x91, eSpecialData_AccentedClick, 0); // Note On, channel 1
    EXPECT_FALSE(IsAccentedClick(&noteOnCh1));
    
    // Test different status nibble
    AAX_CMidiPacket noteOff = CreateMIDIPacket(0x80, eSpecialData_AccentedClick, 0); // Note Off
    EXPECT_FALSE(IsAccentedClick(&noteOff));
    
    // Test null pointer
    EXPECT_FALSE(IsAccentedClick(nullptr));
}

TEST_F(AAX_MIDIUtilities, IsUnaccentedClick)
{
    using namespace AAX;
    
    // Test valid unaccented click message
    AAX_CMidiPacket unaccentedClick = CreateMIDIPacket(0x90, eSpecialData_UnaccentedClick, 0); // Note On, channel 0, unaccented click
    EXPECT_TRUE(IsUnaccentedClick(&unaccentedClick));
    
    // Test accented click message
    AAX_CMidiPacket accentedClick = CreateMIDIPacket(0x90, eSpecialData_AccentedClick, 0); // Note On, channel 0, accented click
    EXPECT_FALSE(IsUnaccentedClick(&accentedClick));
    
    // Test Note On with non-zero channel (should be false)
    AAX_CMidiPacket noteOnCh1 = CreateMIDIPacket(0x91, eSpecialData_UnaccentedClick, 0); // Note On, channel 1
    EXPECT_FALSE(IsUnaccentedClick(&noteOnCh1));
    
    // Test different status nibble
    AAX_CMidiPacket noteOff = CreateMIDIPacket(0x80, eSpecialData_UnaccentedClick, 0); // Note Off
    EXPECT_FALSE(IsUnaccentedClick(&noteOff));
    
    // Test null pointer
    EXPECT_FALSE(IsUnaccentedClick(nullptr));
}

TEST_F(AAX_MIDIUtilities, IsClick)
{
    using namespace AAX;
    
    // Test accented click
    AAX_CMidiPacket accentedClick = CreateMIDIPacket(0x90, eSpecialData_AccentedClick, 0);
    EXPECT_TRUE(IsClick(&accentedClick));
    
    // Test unaccented click
    AAX_CMidiPacket unaccentedClick = CreateMIDIPacket(0x90, eSpecialData_UnaccentedClick, 0);
    EXPECT_TRUE(IsClick(&unaccentedClick));
    
    // Test non-click message
    AAX_CMidiPacket noteOn = CreateMIDIPacket(0x90, 60, 100);
    EXPECT_FALSE(IsClick(&noteOn));
    
    // Test null pointer
    EXPECT_FALSE(IsClick(nullptr));
}

//--------------------------------------------------------------------
#pragma mark - MIDI Beat Clock Tests

TEST_F(AAX_MIDIUtilities, IsMBCClock)
{
    using namespace AAX;

    // Test valid MIDI Beat Clock message
    AAX_CMidiPacket clockPacket = CreateSystemMIDIPacket(eStatusByte_TimingClock);
    EXPECT_TRUE(IsMBCClock(&clockPacket));

    // Test other system real-time messages
    AAX_CMidiPacket startPacket = CreateSystemMIDIPacket(eStatusByte_Start);
    EXPECT_FALSE(IsMBCClock(&startPacket));

    AAX_CMidiPacket stopPacket = CreateSystemMIDIPacket(eStatusByte_Stop);
    EXPECT_FALSE(IsMBCClock(&stopPacket));

    AAX_CMidiPacket continuePacket = CreateSystemMIDIPacket(eStatusByte_Continue);
    EXPECT_FALSE(IsMBCClock(&continuePacket));

    // Test non-system messages
    AAX_CMidiPacket noteOn = CreateMIDIPacket(0x90, 60, 100);
    EXPECT_FALSE(IsMBCClock(&noteOn));

    // Test null pointer
    EXPECT_FALSE(IsMBCClock(nullptr));
}

TEST_F(AAX_MIDIUtilities, IsMBCStart)
{
    using namespace AAX;

    // Test valid MIDI Beat Clock Start message
    AAX_CMidiPacket startPacket = CreateSystemMIDIPacket(eStatusByte_Start);
    EXPECT_TRUE(IsMBCStart(&startPacket));

    // Test other system real-time messages
    AAX_CMidiPacket clockPacket = CreateSystemMIDIPacket(eStatusByte_TimingClock);
    EXPECT_FALSE(IsMBCStart(&clockPacket));

    AAX_CMidiPacket stopPacket = CreateSystemMIDIPacket(eStatusByte_Stop);
    EXPECT_FALSE(IsMBCStart(&stopPacket));

    AAX_CMidiPacket continuePacket = CreateSystemMIDIPacket(eStatusByte_Continue);
    EXPECT_FALSE(IsMBCStart(&continuePacket));

    // Test non-system messages
    AAX_CMidiPacket noteOn = CreateMIDIPacket(0x90, 60, 100);
    EXPECT_FALSE(IsMBCStart(&noteOn));

    // Test null pointer
    EXPECT_FALSE(IsMBCStart(nullptr));
}

TEST_F(AAX_MIDIUtilities, IsMBCContinue)
{
    using namespace AAX;

    // Test valid MIDI Beat Clock Continue message
    AAX_CMidiPacket continuePacket = CreateSystemMIDIPacket(eStatusByte_Continue);
    EXPECT_TRUE(IsMBCContinue(&continuePacket));

    // Test other system real-time messages
    AAX_CMidiPacket clockPacket = CreateSystemMIDIPacket(eStatusByte_TimingClock);
    EXPECT_FALSE(IsMBCContinue(&clockPacket));

    AAX_CMidiPacket startPacket = CreateSystemMIDIPacket(eStatusByte_Start);
    EXPECT_FALSE(IsMBCContinue(&startPacket));

    AAX_CMidiPacket stopPacket = CreateSystemMIDIPacket(eStatusByte_Stop);
    EXPECT_FALSE(IsMBCContinue(&stopPacket));

    // Test non-system messages
    AAX_CMidiPacket noteOn = CreateMIDIPacket(0x90, 60, 100);
    EXPECT_FALSE(IsMBCContinue(&noteOn));

    // Test null pointer
    EXPECT_FALSE(IsMBCContinue(nullptr));
}

TEST_F(AAX_MIDIUtilities, IsMBCStop)
{
    using namespace AAX;

    // Test valid MIDI Beat Clock Stop message
    AAX_CMidiPacket stopPacket = CreateSystemMIDIPacket(eStatusByte_Stop);
    EXPECT_TRUE(IsMBCStop(&stopPacket));

    // Test other system real-time messages
    AAX_CMidiPacket clockPacket = CreateSystemMIDIPacket(eStatusByte_TimingClock);
    EXPECT_FALSE(IsMBCStop(&clockPacket));

    AAX_CMidiPacket startPacket = CreateSystemMIDIPacket(eStatusByte_Start);
    EXPECT_FALSE(IsMBCStop(&startPacket));

    AAX_CMidiPacket continuePacket = CreateSystemMIDIPacket(eStatusByte_Continue);
    EXPECT_FALSE(IsMBCStop(&continuePacket));

    // Test non-system messages
    AAX_CMidiPacket noteOn = CreateMIDIPacket(0x90, 60, 100);
    EXPECT_FALSE(IsMBCStop(&noteOn));

    // Test null pointer
    EXPECT_FALSE(IsMBCStop(nullptr));
}

//--------------------------------------------------------------------
#pragma mark - Song Position Pointer Tests

TEST_F(AAX_MIDIUtilities, IsSongPositionPointer)
{
    using namespace AAX;

    // Test valid Song Position Pointer message
    AAX_CMidiPacket sppPacket = CreateMIDIPacket(eStatusByte_SongPosition, 0x00, 0x00); // Position 0
    int16_t position = -1;
    EXPECT_TRUE(IsSongPositionPointer(&sppPacket, &position));
    EXPECT_EQ(0, position);

    // Test Song Position Pointer with specific position
    AAX_CMidiPacket sppPacket2 = CreateMIDIPacket(eStatusByte_SongPosition, 0x7F, 0x7F); // Maximum position
    int16_t position2 = -1;
    EXPECT_TRUE(IsSongPositionPointer(&sppPacket2, &position2));
    EXPECT_EQ(16383, position2); // (0x7F << 7) | 0x7F = 16383

    // Test Song Position Pointer with mid-range position
    AAX_CMidiPacket sppPacket3 = CreateMIDIPacket(eStatusByte_SongPosition, 0x40, 0x20); // LSB=0x40, MSB=0x20
    int16_t position3 = -1;
    EXPECT_TRUE(IsSongPositionPointer(&sppPacket3, &position3));
    EXPECT_EQ(4160, position3); // (0x20 << 7) | 0x40 = 4160

    // Test Song Position Pointer without output parameter
    EXPECT_TRUE(IsSongPositionPointer(&sppPacket, nullptr));

    // Test other system messages
    AAX_CMidiPacket clockPacket = CreateSystemMIDIPacket(eStatusByte_TimingClock);
    EXPECT_FALSE(IsSongPositionPointer(&clockPacket, &position));

    AAX_CMidiPacket songSelectPacket = CreateMIDIPacket(eStatusByte_SongSelect, 5, 0);
    EXPECT_FALSE(IsSongPositionPointer(&songSelectPacket, &position));

    // Test non-system messages
    AAX_CMidiPacket noteOn = CreateMIDIPacket(0x90, 60, 100);
    EXPECT_FALSE(IsSongPositionPointer(&noteOn, &position));

    // Test null pointer
    EXPECT_FALSE(IsSongPositionPointer(nullptr, &position));
}

//--------------------------------------------------------------------
#pragma mark - Edge Cases and Boundary Conditions

TEST_F(AAX_MIDIUtilities, EdgeCasesAndBoundaryConditions)
{
    using namespace AAX;

    // Test all MIDI channels for Note On/Off
    for (uint8_t channel = 0; channel < 16; ++channel)
    {
        AAX_CMidiPacket noteOnPacket = CreateMIDIPacket(0x90 | channel, 60, 100);
        EXPECT_TRUE(IsNoteOn(&noteOnPacket)) << "Note On failed for channel " << static_cast<int>(channel);

        AAX_CMidiPacket noteOffPacket = CreateMIDIPacket(0x80 | channel, 60, 100);
        EXPECT_TRUE(IsNoteOff(&noteOffPacket)) << "Note Off failed for channel " << static_cast<int>(channel);
    }

    // Test all note numbers (0-127)
    AAX_CMidiPacket notePacket = CreateMIDIPacket(0x90, 0, 100);
    for (uint8_t note = 0; note <= 127; ++note)
    {
        notePacket.mData[1] = note;
        EXPECT_TRUE(IsNoteOn(&notePacket)) << "Note On failed for note " << static_cast<int>(note);
    }

    // Test all velocity values for Note On
    for (uint8_t velocity = 1; velocity <= 127; ++velocity)
    {
        AAX_CMidiPacket noteOnPacket = CreateMIDIPacket(0x90, 60, velocity);
        EXPECT_TRUE(IsNoteOn(&noteOnPacket)) << "Note On failed for velocity " << static_cast<int>(velocity);
    }

    // Test all channels for Channel Mode messages
    for (uint8_t channel = 0; channel < 16; ++channel)
    {
        AAX_CMidiPacket allNotesOffPacket = CreateMIDIPacket(0xB0 | channel, eChannelModeData_AllNotesOff, 0);
        EXPECT_TRUE(IsAllNotesOff(&allNotesOffPacket)) << "All Notes Off failed for channel " << static_cast<int>(channel);
    }

    // Test Song Position Pointer with various positions
    std::vector<std::pair<uint8_t, uint8_t>> testPositions = {
        {0x00, 0x00}, // Position 0
        {0x01, 0x00}, // Position 1
        {0x7F, 0x00}, // Position 127
        {0x00, 0x01}, // Position 128
        {0x7F, 0x7F}  // Position 16383 (maximum)
    };

    for (const auto& pos : testPositions)
    {
        AAX_CMidiPacket sppPacket = CreateMIDIPacket(eStatusByte_SongPosition, pos.first, pos.second);
        int16_t position = -1;
        EXPECT_TRUE(IsSongPositionPointer(&sppPacket, &position));
        int16_t expectedPosition = (static_cast<int16_t>(pos.second & 0x7F) << 7) | static_cast<int16_t>(pos.first & 0x7F);
        EXPECT_EQ(expectedPosition, position) << "SPP position calculation failed for LSB=" << static_cast<int>(pos.first) << ", MSB=" << static_cast<int>(pos.second);
    }
}
