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
 *	\file AAX_Migrated_ChunkDataParser_UnitTest.cpp
 *
 *	\brief Migrated unit tests for AAX_CChunkDataParser (originally from UnitTest++ framework)
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX_CChunkDataParser.h"
#include "AAX_CString.h"

//==============================================================================
// Test Fixture
//==============================================================================

class AAX_Migrated_ChunkDataParser_Test : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // No specific setup needed for chunk data parser tests
    }
    
    void TearDown() override
    {
        // No specific cleanup needed for chunk data parser tests
    }
};

//==============================================================================
// Chunk Data Parser Tests
//==============================================================================

TEST_F(AAX_Migrated_ChunkDataParser_Test, Constructor)
{
    AAX_CChunkDataParser parser;
    EXPECT_EQ(AAX_ChunkDataParserDefs::HEADER_SIZE, parser.GetChunkDataSize());
}

TEST_F(AAX_Migrated_ChunkDataParser_Test, AddElements)
{
    // Push one of each kind of element, check IsEmpty() and Clear()
    
    AAX_CChunkDataParser parser;
    
    parser.AddFloat("Float for testing", 1.2f);
    EXPECT_FALSE(parser.IsEmpty());
    parser.Clear();
    EXPECT_TRUE(parser.IsEmpty());
    
    parser.AddDouble("Double for testing", 3.4f);
    EXPECT_FALSE(parser.IsEmpty());
    parser.Clear();
    EXPECT_TRUE(parser.IsEmpty());
    
    parser.AddInt32("Int32 for testing", 65000000);
    EXPECT_FALSE(parser.IsEmpty());
    parser.Clear();
    EXPECT_TRUE(parser.IsEmpty());
    
    parser.AddInt16("Int16 for testing", -32000);
    EXPECT_FALSE(parser.IsEmpty());
    parser.Clear();
    EXPECT_TRUE(parser.IsEmpty());
    
    parser.AddString("String for testing", "This is a somewhat longer string for testing purposes.");
    EXPECT_FALSE(parser.IsEmpty());
    parser.Clear();
    EXPECT_TRUE(parser.IsEmpty());
}

TEST_F(AAX_Migrated_ChunkDataParser_Test, FindElements)
{
    // Push a handful of different data elements and find them
    
    int16_t foundInt16;
    int32_t foundInt32;
    double foundDouble;
    float foundFloat;
    AAX_CString foundString;
    
    AAX_CChunkDataParser parser;
    
    parser.AddFloat("Float #1 for testing", 1.2f);
    parser.AddInt16("Int16 #1 for testing", -32000);
    parser.AddString("String #1 for testing", "This is a string for testing purposes.");
    parser.AddDouble("Double #1 for testing", 3.4f);
    parser.AddInt32("Int32 #1 for testing", 65000000);
    
    parser.AddFloat("Float #2 for testing", -3.4f);
    parser.AddInt16("Int16 #2 for testing", 67);
    parser.AddDouble("Double #2 for testing", 5.6e21);
    parser.AddString("String #2 for testing", "This is a much, much longer string than the other one blah blah blah blah blah blah blah blah for testing purposes.");
    parser.AddInt32("Int32 #2 for testing", -65000000);
    
    EXPECT_FALSE(parser.IsEmpty());
    
    EXPECT_TRUE(parser.FindInt16("Int16 #2 for testing", &foundInt16));
    EXPECT_EQ(67, foundInt16);
    
    EXPECT_TRUE(parser.FindInt32("Int32 #2 for testing", &foundInt32));
    EXPECT_EQ(-65000000, foundInt32);
    
    EXPECT_TRUE(parser.FindString("String #1 for testing", &foundString));
    EXPECT_STREQ("This is a string for testing purposes.", foundString.Get());
    
    EXPECT_TRUE(parser.FindString("String #2 for testing", &foundString));
    EXPECT_STREQ("This is a much, much longer string than the other one blah blah blah blah blah blah blah blah for testing purposes.", foundString.Get());
    
    EXPECT_TRUE(parser.FindInt16("Int16 #1 for testing", &foundInt16));
    EXPECT_EQ(-32000, foundInt16);
    
    EXPECT_TRUE(parser.FindInt32("Int32 #1 for testing", &foundInt32));
    EXPECT_EQ(65000000, foundInt32);
    
    EXPECT_TRUE(parser.FindDouble("Double #1 for testing", &foundDouble));
    EXPECT_EQ(3.4f, foundDouble);
    
    EXPECT_TRUE(parser.FindDouble("Double #2 for testing", &foundDouble));
    EXPECT_EQ(5.6e21, foundDouble);
    
    EXPECT_TRUE(parser.FindFloat("Float #1 for testing", &foundFloat));
    EXPECT_EQ(float(1.2), foundFloat);
    
    EXPECT_TRUE(parser.FindFloat("Float #2 for testing", &foundFloat));
    EXPECT_EQ(float(-3.4), foundFloat);
    
    EXPECT_TRUE(parser.ReplaceDouble("Double #2 for testing", 5.6f));
    EXPECT_TRUE(parser.FindDouble("Double #2 for testing", &foundDouble));
    EXPECT_EQ(5.6f, foundDouble);
}

TEST_F(AAX_Migrated_ChunkDataParser_Test, LoadAndSet)
{
    // Load and Set Chunk data including Strings
    
    AAX_Result result;
    
    int16_t foundInt16;
    int32_t foundInt32;
    double foundDouble;
    float foundFloat;
    AAX_CString foundString;
    
    AAX_CChunkDataParser parser;
    
    parser.AddInt32("Int32 #1 for testing", 650000);
    parser.AddDouble("Double #1 for testing", 2.2f);
    parser.AddString("String #1 for testing", "This is a string longer longer longer longer longer longer longer longer long");
    parser.AddInt16("Int16 #1 for testing", -32000);
    parser.AddFloat("Float #1 for testing", 1.2f);
    
    result = parser.GetChunkDataSize();
    EXPECT_EQ(224, result);
    
    char rawChunk[300];
    AAX_SPlugInChunk* chunk = reinterpret_cast<AAX_SPlugInChunk*>(rawChunk);
    
    result = parser.GetChunkData(chunk);
    EXPECT_EQ(AAX_SUCCESS, result);
    
    parser.Clear();
    
    parser.LoadChunk(chunk);
    
    EXPECT_TRUE(parser.FindInt32("Int32 #1 for testing", &foundInt32));
    EXPECT_EQ(650000, foundInt32);
    
    EXPECT_TRUE(parser.FindString("String #1 for testing", &foundString));
    EXPECT_STREQ("This is a string longer longer longer longer longer longer longer longer long", foundString.Get());
    
    EXPECT_TRUE(parser.FindDouble("Double #1 for testing", &foundDouble));
    EXPECT_EQ(double(2.2f), foundDouble);
    
    EXPECT_TRUE(parser.FindFloat("Float #1 for testing", &foundFloat));
    EXPECT_EQ(1.2f, foundFloat);
    
    EXPECT_TRUE(parser.FindInt16("Int16 #1 for testing", &foundInt16));
    EXPECT_EQ(-32000, foundInt16);
}
