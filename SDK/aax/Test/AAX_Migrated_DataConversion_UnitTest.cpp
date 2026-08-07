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
 *	\file AAX_Migrated_DataConversion_UnitTest.cpp
 *
 *	\brief Migrated unit tests for Data Conversion (originally from UnitTest++ framework)
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX_Enums.h"

//==============================================================================
// Test Fixture
//==============================================================================

class AAX_Migrated_DataConversion_Test : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

//==============================================================================
// Data Conversion Tests
//==============================================================================

TEST_F(AAX_Migrated_DataConversion_Test, IntLimits)
{
    EXPECT_EQ(INT32_MIN,  AAX_INT32_MIN);
    EXPECT_EQ(INT32_MAX,  AAX_INT32_MAX);
    EXPECT_EQ(0x0U,       AAX_UINT32_MIN);
    EXPECT_EQ(UINT32_MAX, AAX_UINT32_MAX);
    
    EXPECT_EQ(INT16_MIN,  AAX_INT16_MIN);
    EXPECT_EQ(INT16_MAX,  AAX_INT16_MAX);
    EXPECT_EQ(0x0U,       AAX_UINT16_MIN);
    EXPECT_EQ((unsigned)UINT16_MAX, AAX_UINT16_MAX);
}

TEST_F(AAX_Migrated_DataConversion_Test, StemFormatNum)
{
    // If any stem formats are added in the future, update all Stem Format macro tests.
    EXPECT_EQ(37U, AAX_eStemFormatNum);
}

TEST_F(AAX_Migrated_DataConversion_Test, StemFormatChannelCount)
{
    EXPECT_EQ(1, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_Mono));
    EXPECT_EQ(2, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_Stereo));
    EXPECT_EQ(3, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_LCR));
    EXPECT_EQ(4, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_LCRS));
    EXPECT_EQ(4, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_Quad));
    EXPECT_EQ(5, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_5_0));
    EXPECT_EQ(6, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_5_1));
    EXPECT_EQ(6, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_6_0));
    EXPECT_EQ(7, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_6_1));
    EXPECT_EQ(7, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_7_0_SDDS));
    EXPECT_EQ(8, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_7_1_SDDS));
    EXPECT_EQ(7, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_7_0_DTS));
    EXPECT_EQ(8, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_7_1_DTS));
    EXPECT_EQ(9, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_7_0_2));
    EXPECT_EQ(10, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_7_1_2));
    EXPECT_EQ(4, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_Ambi_1_ACN));
    EXPECT_EQ(9, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_Ambi_2_ACN));
    EXPECT_EQ(16, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_Ambi_3_ACN));
    EXPECT_EQ(25, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_Ambi_4_ACN));
    EXPECT_EQ(36, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_Ambi_5_ACN));
    EXPECT_EQ(49, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_Ambi_6_ACN));
    EXPECT_EQ(64, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_Ambi_7_ACN));
    EXPECT_EQ(7, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_5_0_2));
    EXPECT_EQ(8, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_5_1_2));
    EXPECT_EQ(9, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_5_0_4));
    EXPECT_EQ(10, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_5_1_4));
    EXPECT_EQ(11, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_7_0_4));
    EXPECT_EQ(12, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_7_1_4));
    EXPECT_EQ(13, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_9_0_4));
    EXPECT_EQ(14, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_9_1_4));
    EXPECT_EQ(15, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_9_0_6));
    EXPECT_EQ(16, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_9_1_6));
    EXPECT_EQ(13, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_7_0_6));
    EXPECT_EQ(14, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_7_1_6));
    EXPECT_EQ(0, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_None));
    EXPECT_EQ(0, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_Any));

#if USE_DISCRETE_STEMFORMATS
    EXPECT_EQ(1, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_Discrete_1));
    EXPECT_EQ(2, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_Discrete_2));
    EXPECT_EQ(3, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_Discrete_3));
    EXPECT_EQ(4, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_Discrete_4));
    EXPECT_EQ(5, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_Discrete_5));
    EXPECT_EQ(6, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_Discrete_6));
    EXPECT_EQ(7, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_Discrete_7));
    EXPECT_EQ(8, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_Discrete_8));
    EXPECT_EQ(9, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_Discrete_9));
    EXPECT_EQ(10, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_Discrete_10));
    EXPECT_EQ(11, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_Discrete_11));
    EXPECT_EQ(12, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_Discrete_12));
    EXPECT_EQ(13, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_Discrete_13));
    EXPECT_EQ(14, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_Discrete_14));
    EXPECT_EQ(15, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_Discrete_15));
    EXPECT_EQ(16, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_eStemFormat_Discrete_16));
#endif
    
    // This definition should generate a compiler warning about signed/unsigned
    // comparison, but I don't know how to add a unit test for a compiler warning...
    // EXPECT_EQ(0, AAX_STEM_FORMAT_CHANNEL_COUNT(AAX_STEM_FORMAT(0,-1)));
}

TEST_F(AAX_Migrated_DataConversion_Test, StemFormatIndex)
{
    EXPECT_EQ(0, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_Mono));
    EXPECT_EQ(1, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_Stereo));
    EXPECT_EQ(2, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_LCR));
    EXPECT_EQ(3, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_LCRS));
    EXPECT_EQ(4, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_Quad));
    EXPECT_EQ(5, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_5_0));
    EXPECT_EQ(6, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_5_1));
    EXPECT_EQ(7, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_6_0));
    EXPECT_EQ(8, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_6_1));
    EXPECT_EQ(9, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_7_0_SDDS));
    EXPECT_EQ(10, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_7_1_SDDS));
    EXPECT_EQ(11, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_7_0_DTS));
    EXPECT_EQ(12, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_7_1_DTS));
    EXPECT_EQ(20, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_7_0_2));
    EXPECT_EQ(13, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_7_1_2));
    EXPECT_EQ(14, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_Ambi_1_ACN));
    EXPECT_EQ(18, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_Ambi_2_ACN));
    EXPECT_EQ(19, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_Ambi_3_ACN));
    EXPECT_EQ(31, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_Ambi_4_ACN));
    EXPECT_EQ(32, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_Ambi_5_ACN));
    EXPECT_EQ(33, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_Ambi_6_ACN));
    EXPECT_EQ(34, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_Ambi_7_ACN));
    EXPECT_EQ(21, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_5_0_2));
    EXPECT_EQ(22, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_5_1_2));
    EXPECT_EQ(23, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_5_0_4));
    EXPECT_EQ(24, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_5_1_4));
    EXPECT_EQ(25, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_7_0_4));
    EXPECT_EQ(26, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_7_1_4));
    EXPECT_EQ(27, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_9_0_4));
    EXPECT_EQ(28, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_9_1_4));
    EXPECT_EQ(29, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_9_0_6));
    EXPECT_EQ(30, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_9_1_6));
    EXPECT_EQ(35, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_7_0_6));
    EXPECT_EQ(36, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_7_1_6));

#if USE_DISCRETE_STEMFORMATS
    EXPECT_EQ(-201, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_Discrete_1));
    EXPECT_EQ(-202, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_Discrete_2));
    EXPECT_EQ(-203, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_Discrete_3));
    EXPECT_EQ(-204, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_Discrete_4));
    EXPECT_EQ(-205, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_Discrete_5));
    EXPECT_EQ(-206, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_Discrete_6));
    EXPECT_EQ(-207, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_Discrete_7));
    EXPECT_EQ(-208, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_Discrete_8));
    EXPECT_EQ(-209, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_Discrete_9));
    EXPECT_EQ(-210, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_Discrete_10));
    EXPECT_EQ(-211, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_Discrete_11));
    EXPECT_EQ(-212, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_Discrete_12));
    EXPECT_EQ(-213, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_Discrete_13));
    EXPECT_EQ(-214, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_Discrete_14));
    EXPECT_EQ(-215, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_Discrete_15));
    EXPECT_EQ(-216, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_Discrete_16));
#endif

    EXPECT_EQ(-100, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_None));
    EXPECT_EQ(-1, AAX_STEM_FORMAT_INDEX(AAX_eStemFormat_Any));

    // Test edge cases
    EXPECT_NE(AAX_INT16_MAX, AAX_STEM_FORMAT_INDEX(AAX_STEM_FORMAT(0xFFFF, 0)));
    EXPECT_EQ(-1, AAX_STEM_FORMAT_INDEX(AAX_STEM_FORMAT(0xFFFF, 0)));
    EXPECT_EQ(AAX_INT16_MAX, AAX_STEM_FORMAT_INDEX(AAX_STEM_FORMAT(0x7FFF, 0)));
}

TEST_F(AAX_Migrated_DataConversion_Test, StemFormatValues)
{
    EXPECT_EQ(0x00000001U, AAX_eStemFormat_Mono);
    EXPECT_EQ(0x00010002U, AAX_eStemFormat_Stereo);
    EXPECT_EQ(0x00020003U, AAX_eStemFormat_LCR);
    EXPECT_EQ(0x00030004U, AAX_eStemFormat_LCRS);
    EXPECT_EQ(0x00040004U, AAX_eStemFormat_Quad);
    EXPECT_EQ(0x00050005U, AAX_eStemFormat_5_0);
    EXPECT_EQ(0x00060006U, AAX_eStemFormat_5_1);
    EXPECT_EQ(0x00070006U, AAX_eStemFormat_6_0);
    EXPECT_EQ(0x00080007U, AAX_eStemFormat_6_1);
    EXPECT_EQ(0x00090007U, AAX_eStemFormat_7_0_SDDS);
    EXPECT_EQ(0x000A0008U, AAX_eStemFormat_7_1_SDDS);
    EXPECT_EQ(0x000B0007U, AAX_eStemFormat_7_0_DTS);
    EXPECT_EQ(0x000C0008U, AAX_eStemFormat_7_1_DTS);
    EXPECT_EQ(0x00140009U, AAX_eStemFormat_7_0_2);
    EXPECT_EQ(0x000D000AU, AAX_eStemFormat_7_1_2);
    EXPECT_EQ(0x000E0004U, AAX_eStemFormat_Ambi_1_ACN);
    EXPECT_EQ(0x00120009U, AAX_eStemFormat_Ambi_2_ACN);
    EXPECT_EQ(0x00130010U, AAX_eStemFormat_Ambi_3_ACN);
    EXPECT_EQ(0x001F0019U, AAX_eStemFormat_Ambi_4_ACN);
    EXPECT_EQ(0x00200024U, AAX_eStemFormat_Ambi_5_ACN);
    EXPECT_EQ(0x00210031U, AAX_eStemFormat_Ambi_6_ACN);
    EXPECT_EQ(0x00220040U, AAX_eStemFormat_Ambi_7_ACN);
    EXPECT_EQ(0x00150007U, AAX_eStemFormat_5_0_2);
    EXPECT_EQ(0x00160008U, AAX_eStemFormat_5_1_2);
    EXPECT_EQ(0x00170009U, AAX_eStemFormat_5_0_4);
    EXPECT_EQ(0x0018000AU, AAX_eStemFormat_5_1_4);
    EXPECT_EQ(0x0019000BU, AAX_eStemFormat_7_0_4);
    EXPECT_EQ(0x001A000CU, AAX_eStemFormat_7_1_4);
    EXPECT_EQ(0x001B000DU, AAX_eStemFormat_9_0_4);
    EXPECT_EQ(0x001C000EU, AAX_eStemFormat_9_1_4);
    EXPECT_EQ(0x001D000FU, AAX_eStemFormat_9_0_6);
    EXPECT_EQ(0x001E0010U, AAX_eStemFormat_9_1_6);
    EXPECT_EQ(0x0023000DU, AAX_eStemFormat_7_0_6);
    EXPECT_EQ(0x0024000EU, AAX_eStemFormat_7_1_6);
    EXPECT_EQ(0xFF9C0000U, AAX_eStemFormat_None);
    EXPECT_EQ(0xFFFF0000U, AAX_eStemFormat_Any);

#if USE_DISCRETE_STEMFORMATS
    EXPECT_EQ(0xFF370001U, AAX_eStemFormat_Discrete_1);
    EXPECT_EQ(0xFF360002U, AAX_eStemFormat_Discrete_2);
    EXPECT_EQ(0xFF350003U, AAX_eStemFormat_Discrete_3);
    EXPECT_EQ(0xFF340004U, AAX_eStemFormat_Discrete_4);
    EXPECT_EQ(0xFF330005U, AAX_eStemFormat_Discrete_5);
    EXPECT_EQ(0xFF320006U, AAX_eStemFormat_Discrete_6);
    EXPECT_EQ(0xFF310007U, AAX_eStemFormat_Discrete_7);
    EXPECT_EQ(0xFF300008U, AAX_eStemFormat_Discrete_8);
    EXPECT_EQ(0xFF2F0009U, AAX_eStemFormat_Discrete_9);
    EXPECT_EQ(0xFF2E000AU, AAX_eStemFormat_Discrete_10);
    EXPECT_EQ(0xFF2D000BU, AAX_eStemFormat_Discrete_11);
    EXPECT_EQ(0xFF2C000CU, AAX_eStemFormat_Discrete_12);
    EXPECT_EQ(0xFF2B000DU, AAX_eStemFormat_Discrete_13);
    EXPECT_EQ(0xFF2A000EU, AAX_eStemFormat_Discrete_14);
    EXPECT_EQ(0xFF29000FU, AAX_eStemFormat_Discrete_15);
    EXPECT_EQ(0xFF280010U, AAX_eStemFormat_Discrete_16);
#endif
}
