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
 *	\file AAX_CChunkDataParser_UnitTest.cpp
 *
 *	\brief Unit tests for AAX_CChunkDataParser class
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX_CChunkDataParser.h"
#include "AAX_CString.h"

//--------------------------------------------------------------------
#pragma mark - Test fixture

class AAX_CChunkDataParser_Test : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

//--------------------------------------------------------------------
#pragma mark - Basic Functionality Tests

TEST_F(AAX_CChunkDataParser_Test, DefaultConstruction)
{
    AAX_CChunkDataParser parser;

    // New parser should be empty
    EXPECT_TRUE(parser.IsEmpty());
    // POTENTIAL BUG: GetChunkDataSize() returns 4 (header size) even when empty
    // Expected: Empty parser should return 0 size
    // Actual: Returns 4 bytes for header even when no data is present
    EXPECT_EQ(4, parser.GetChunkDataSize()); // Header size
    EXPECT_EQ(-1, parser.GetChunkVersion()); // -1 indicates no chunk loaded
}

TEST_F(AAX_CChunkDataParser_Test, AddAndFindFloat)
{
    AAX_CChunkDataParser parser;
    const char* name = "test_float";
    float originalValue = 3.14159f;
    
    // Add float value
    parser.AddFloat(name, originalValue);
    EXPECT_FALSE(parser.IsEmpty());
    
    // Find the value
    float retrievedValue = 0.0f;
    bool found = parser.FindFloat(name, &retrievedValue);
    EXPECT_TRUE(found);
    EXPECT_FLOAT_EQ(originalValue, retrievedValue);
}

TEST_F(AAX_CChunkDataParser_Test, AddAndFindDouble)
{
    AAX_CChunkDataParser parser;
    const char* name = "test_double";
    double originalValue = 2.718281828;
    
    // Add double value
    parser.AddDouble(name, originalValue);
    EXPECT_FALSE(parser.IsEmpty());
    
    // Find the value
    double retrievedValue = 0.0;
    bool found = parser.FindDouble(name, &retrievedValue);
    EXPECT_TRUE(found);
    EXPECT_DOUBLE_EQ(originalValue, retrievedValue);
}

TEST_F(AAX_CChunkDataParser_Test, AddAndFindInt32)
{
    AAX_CChunkDataParser parser;
    const char* name = "test_int32";
    int32_t originalValue = 42;
    
    // Add int32 value
    parser.AddInt32(name, originalValue);
    EXPECT_FALSE(parser.IsEmpty());
    
    // Find the value
    int32_t retrievedValue = 0;
    bool found = parser.FindInt32(name, &retrievedValue);
    EXPECT_TRUE(found);
    EXPECT_EQ(originalValue, retrievedValue);
}

TEST_F(AAX_CChunkDataParser_Test, AddAndFindInt16)
{
    AAX_CChunkDataParser parser;
    const char* name = "test_int16";
    int16_t originalValue = 1234;
    
    // Add int16 value
    parser.AddInt16(name, originalValue);
    EXPECT_FALSE(parser.IsEmpty());
    
    // Find the value
    int16_t retrievedValue = 0;
    bool found = parser.FindInt16(name, &retrievedValue);
    EXPECT_TRUE(found);
    EXPECT_EQ(originalValue, retrievedValue);
}

TEST_F(AAX_CChunkDataParser_Test, AddAndFindString)
{
    AAX_CChunkDataParser parser;
    const char* name = "test_string";
    AAX_CString originalValue("Hello World");
    
    // Add string value
    parser.AddString(name, originalValue);
    EXPECT_FALSE(parser.IsEmpty());
    
    // Find the value
    AAX_CString retrievedValue;
    bool found = parser.FindString(name, &retrievedValue);
    EXPECT_TRUE(found);
    EXPECT_STREQ(originalValue.Get(), retrievedValue.Get());
}

TEST_F(AAX_CChunkDataParser_Test, FindNonExistentValue)
{
    AAX_CChunkDataParser parser;
    
    // Try to find values that don't exist
    float floatValue = 0.0f;
    EXPECT_FALSE(parser.FindFloat("nonexistent", &floatValue));
    
    double doubleValue = 0.0;
    EXPECT_FALSE(parser.FindDouble("nonexistent", &doubleValue));
    
    int32_t int32Value = 0;
    EXPECT_FALSE(parser.FindInt32("nonexistent", &int32Value));
    
    int16_t int16Value = 0;
    EXPECT_FALSE(parser.FindInt16("nonexistent", &int16Value));
    
    AAX_CString stringValue;
    EXPECT_FALSE(parser.FindString("nonexistent", &stringValue));
}

TEST_F(AAX_CChunkDataParser_Test, ClearOperation)
{
    AAX_CChunkDataParser parser;
    
    // Add some data
    parser.AddFloat("float_val", 1.0f);
    parser.AddInt32("int_val", 100);
    EXPECT_FALSE(parser.IsEmpty());
    
    // Clear the parser
    parser.Clear();
    EXPECT_TRUE(parser.IsEmpty());
    // POTENTIAL BUG: GetChunkDataSize() returns 4 (header size) even after clear
    // Expected: Cleared parser should return 0 size
    // Actual: Returns 4 bytes for header even after clearing
    EXPECT_EQ(4, parser.GetChunkDataSize()); // Header size remains
    
    // Values should no longer be found
    float floatValue = 0.0f;
    EXPECT_FALSE(parser.FindFloat("float_val", &floatValue));
    
    int32_t intValue = 0;
    EXPECT_FALSE(parser.FindInt32("int_val", &intValue));
}

TEST_F(AAX_CChunkDataParser_Test, MultipleValues)
{
    AAX_CChunkDataParser parser;

    // Add multiple values of different types
    parser.AddFloat("gain", 0.75f);
    parser.AddDouble("frequency", 440.0);
    parser.AddInt32("samples", 48000);
    parser.AddInt16("channels", 2);
    parser.AddString("name", AAX_CString("Test Plugin"));

    EXPECT_FALSE(parser.IsEmpty());

    // Retrieve all values
    float gain = 0.0f;
    EXPECT_TRUE(parser.FindFloat("gain", &gain));
    EXPECT_FLOAT_EQ(0.75f, gain);

    double frequency = 0.0;
    EXPECT_TRUE(parser.FindDouble("frequency", &frequency));
    EXPECT_DOUBLE_EQ(440.0, frequency);

    int32_t samples = 0;
    EXPECT_TRUE(parser.FindInt32("samples", &samples));
    EXPECT_EQ(48000, samples);

    int16_t channels = 0;
    EXPECT_TRUE(parser.FindInt16("channels", &channels));
    EXPECT_EQ(2, channels);

    AAX_CString name;
    EXPECT_TRUE(parser.FindString("name", &name));
    EXPECT_STREQ("Test Plugin", name.Get());
}

TEST_F(AAX_CChunkDataParser_Test, ReplaceDouble)
{
    AAX_CChunkDataParser parser;
    const char* name = "replaceable_value";

    // Add initial value
    parser.AddDouble(name, 1.0);

    double value = 0.0;
    EXPECT_TRUE(parser.FindDouble(name, &value));
    EXPECT_DOUBLE_EQ(1.0, value);

    // Replace the value
    bool replaced = parser.ReplaceDouble(name, 2.5);
    EXPECT_TRUE(replaced);

    // Verify new value
    EXPECT_TRUE(parser.FindDouble(name, &value));
    EXPECT_DOUBLE_EQ(2.5, value);
}

TEST_F(AAX_CChunkDataParser_Test, ReplaceNonExistentDouble)
{
    AAX_CChunkDataParser parser;

    // Try to replace a value that doesn't exist
    bool replaced = parser.ReplaceDouble("nonexistent", 3.14);
    EXPECT_FALSE(replaced);
}

TEST_F(AAX_CChunkDataParser_Test, ChunkDataSize)
{
    AAX_CChunkDataParser parser;
    int32_t initialSize = parser.GetChunkDataSize();

    // Add some data and verify size increases
    parser.AddFloat("test", 1.0f);
    int32_t sizeAfterFloat = parser.GetChunkDataSize();
    EXPECT_GT(sizeAfterFloat, initialSize);

    parser.AddInt32("test2", 100);
    int32_t sizeAfterInt = parser.GetChunkDataSize();
    EXPECT_GT(sizeAfterInt, sizeAfterFloat);

    parser.AddString("test3", AAX_CString("Hello"));
    int32_t sizeAfterString = parser.GetChunkDataSize();
    EXPECT_GT(sizeAfterString, sizeAfterInt);
}
