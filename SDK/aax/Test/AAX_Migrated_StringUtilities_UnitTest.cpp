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
 *	\file AAX_Migrated_StringUtilities_UnitTest.cpp
 *
 *	\brief Migrated unit tests for AAX_StringUtilities (originally from UnitTest++ framework)
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX_StringUtilities.h"

//==============================================================================
// Test Fixture
//==============================================================================

class AAX_Migrated_StringUtilities_Test : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

//==============================================================================
// String Utilities Tests
//==============================================================================

TEST_F(AAX_Migrated_StringUtilities_Test, AsStringFourChar)
{
    EXPECT_EQ("(?)(?)(?)(?)", AAX::AsStringFourChar(0x00000000));
    EXPECT_EQ("    ", AAX::AsStringFourChar(0x20202020));
    EXPECT_EQ("~~~~", AAX::AsStringFourChar(0x7E7E7E7E));
    EXPECT_EQ("0123", AAX::AsStringFourChar(0x30313233));
    EXPECT_EQ("0(?)(?)3", AAX::AsStringFourChar(0x30000033));
}

TEST_F(AAX_Migrated_StringUtilities_Test, IsFourCharASCII)
{
    EXPECT_EQ(false, AAX::IsFourCharASCII(0x00000000));
    EXPECT_EQ(true, AAX::IsFourCharASCII(0x20202020));
    EXPECT_EQ(true, AAX::IsFourCharASCII(0x7E7E7E7E));
    EXPECT_EQ(false, AAX::IsFourCharASCII(0x7E7F7E7E));
    EXPECT_EQ(true, AAX::IsFourCharASCII(0x30313233));
    EXPECT_EQ(false, AAX::IsFourCharASCII(0x30000033));
    EXPECT_EQ(false, AAX::IsFourCharASCII(0x2020201F));
}

TEST_F(AAX_Migrated_StringUtilities_Test, AsStringPropertyValue_WithSampleRate)
{
    EXPECT_EQ("00000000", AAX::AsStringPropertyValue(AAX_eProperty_SampleRate, 0));
    EXPECT_EQ("00000001", AAX::AsStringPropertyValue(AAX_eProperty_SampleRate, 1));
    EXPECT_EQ("10000000", AAX::AsStringPropertyValue(AAX_eProperty_SampleRate, 0x80));
    EXPECT_EQ("01000010", AAX::AsStringPropertyValue(AAX_eProperty_SampleRate, 0x42));
    EXPECT_EQ("11111111", AAX::AsStringPropertyValue(AAX_eProperty_SampleRate, 0xff));
}

TEST_F(AAX_Migrated_StringUtilities_Test, AsStringPropertyValue_WithConstraintLocation)
{
    EXPECT_EQ("00000000", AAX::AsStringPropertyValue(AAX_eProperty_Constraint_Location, 0));
    EXPECT_EQ("00000001", AAX::AsStringPropertyValue(AAX_eProperty_Constraint_Location, 1));
    EXPECT_EQ("10000000", AAX::AsStringPropertyValue(AAX_eProperty_Constraint_Location, 0x80));
    EXPECT_EQ("01000010", AAX::AsStringPropertyValue(AAX_eProperty_Constraint_Location, 0x42));
    EXPECT_EQ("11111111", AAX::AsStringPropertyValue(AAX_eProperty_Constraint_Location, 0xff));
}

TEST_F(AAX_Migrated_StringUtilities_Test, AsStringPropertyValue_RemainingValues)
{
    // Four char
    EXPECT_EQ("'    '", AAX::AsStringPropertyValue(AAX_eProperty_GeneralPropsBase, 0x20202020));
    EXPECT_EQ("'~~~~'", AAX::AsStringPropertyValue(AAX_eProperty_GeneralPropsBase, 0x7E7E7E7E));
    EXPECT_EQ("'0123'", AAX::AsStringPropertyValue(AAX_eProperty_GeneralPropsBase, 0x30313233));

    // Should be formatted as hexadecimal
    EXPECT_EQ("0x1000000", AAX::AsStringPropertyValue(AAX_eProperty_GeneralPropsBase, 0x01000000));
    EXPECT_EQ("0x12345678", AAX::AsStringPropertyValue(AAX_eProperty_GeneralPropsBase, 0x12345678));
    EXPECT_EQ("0x9abcdef", AAX::AsStringPropertyValue(AAX_eProperty_GeneralPropsBase, 0x09abcdef));
    EXPECT_EQ("0x80000001", AAX::AsStringPropertyValue(AAX_eProperty_GeneralPropsBase, -2147483647));

    // Should be formatted as decimal numbers
    EXPECT_EQ("0", AAX::AsStringPropertyValue(AAX_eProperty_GeneralPropsBase, 0));
    EXPECT_EQ("16777215", AAX::AsStringPropertyValue(AAX_eProperty_GeneralPropsBase, 0x00FFFFFF));
    EXPECT_EQ("-16777215", AAX::AsStringPropertyValue(AAX_eProperty_GeneralPropsBase, -0x00FFFFFF));
}
