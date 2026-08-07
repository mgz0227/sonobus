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
 *	\file AAX_DataBuffer_UnitTest.cpp
 *
 *	\brief Unit tests for AAX DataBuffer classes
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX_CValueDataBuffer.h"
#include "AAX_CDataBuffer.h"
#include "AAX_CArrayDataBuffer.h"
#include "AAX_CStringDataBuffer.h"
#include "AAX_Errors.h"
#include <vector>
#include <string>
#include <cstring>
#include <limits>

//--------------------------------------------------------------------
#pragma mark - Test fixture

class AAX_DataBuffer : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
    
    // Test type IDs
    static constexpr AAX_CTypeID kTestTypeID1 = 'TST1';
    static constexpr AAX_CTypeID kTestTypeID2 = 'TST2';
    static constexpr AAX_CTypeID kTestTypeID3 = 'TST3';
    static constexpr AAX_CTypeID kTestTypeID4 = 'TST4';
    
    // Test data structures
    struct TestStruct {
        int32_t value1;
        float value2;
        bool value3;
        
        bool operator==(const TestStruct& other) const {
            return value1 == other.value1 && value2 == other.value2 && value3 == other.value3;
        }
    };
};

//--------------------------------------------------------------------
#pragma mark - AAX_CValueDataBufferOfType Tests

TEST_F(AAX_DataBuffer, ValueDataBufferOfTypeBasicConstruction)
{
    // Test construction with various data types
    int32_t intValue = 42;
    AAX_CValueDataBufferOfType<kTestTypeID1, int32_t> intBuffer(intValue);
    
    float floatValue = 3.14f;
    AAX_CValueDataBufferOfType<kTestTypeID2, float> floatBuffer(floatValue);
    
    TestStruct structValue = {123, 4.56f, true};
    AAX_CValueDataBufferOfType<kTestTypeID3, TestStruct> structBuffer(structValue);
    
    // Test Type() method
    AAX_CTypeID type;
    EXPECT_EQ(AAX_SUCCESS, intBuffer.Type(&type));
    EXPECT_EQ(kTestTypeID1, type);
    
    EXPECT_EQ(AAX_SUCCESS, floatBuffer.Type(&type));
    EXPECT_EQ(kTestTypeID2, type);
    
    EXPECT_EQ(AAX_SUCCESS, structBuffer.Type(&type));
    EXPECT_EQ(kTestTypeID3, type);
}

TEST_F(AAX_DataBuffer, ValueDataBufferOfTypeSize)
{
    int32_t intValue = 42;
    AAX_CValueDataBufferOfType<kTestTypeID1, int32_t> intBuffer(intValue);
    
    TestStruct structValue = {123, 4.56f, true};
    AAX_CValueDataBufferOfType<kTestTypeID3, TestStruct> structBuffer(structValue);
    
    // Test Size() method
    int32_t size;
    EXPECT_EQ(AAX_SUCCESS, intBuffer.Size(&size));
    EXPECT_EQ(sizeof(int32_t), size);
    
    EXPECT_EQ(AAX_SUCCESS, structBuffer.Size(&size));
    EXPECT_EQ(sizeof(TestStruct), size);
}

TEST_F(AAX_DataBuffer, ValueDataBufferOfTypeData)
{
    int32_t intValue = 42;
    AAX_CValueDataBufferOfType<kTestTypeID1, int32_t> intBuffer(intValue);
    
    TestStruct structValue = {123, 4.56f, true};
    AAX_CValueDataBufferOfType<kTestTypeID3, TestStruct> structBuffer(structValue);
    
    // Test Data() method
    void const* data;
    EXPECT_EQ(AAX_SUCCESS, intBuffer.Data(&data));
    EXPECT_NE(nullptr, data);
    EXPECT_EQ(intValue, *static_cast<const int32_t*>(data));
    
    EXPECT_EQ(AAX_SUCCESS, structBuffer.Data(&data));
    EXPECT_NE(nullptr, data);
    EXPECT_EQ(structValue, *static_cast<const TestStruct*>(data));
}

TEST_F(AAX_DataBuffer, ValueDataBufferOfTypeMoveConstruction)
{
    // Test move construction
    std::string stringValue = "test string";
    AAX_CValueDataBufferOfType<kTestTypeID4, std::string> stringBuffer(std::move(stringValue));
    
    void const* data;
    EXPECT_EQ(AAX_SUCCESS, stringBuffer.Data(&data));
    EXPECT_NE(nullptr, data);
    EXPECT_EQ("test string", *static_cast<const std::string*>(data));
}

TEST_F(AAX_DataBuffer, ValueDataBufferOfTypeMultipleInstances)
{
    // Test multiple instances with different data
    int32_t intValue1 = 42;
    int32_t intValue2 = 84;
    AAX_CValueDataBufferOfType<kTestTypeID1, int32_t> buffer1(intValue1);
    AAX_CValueDataBufferOfType<kTestTypeID1, int32_t> buffer2(intValue2);

    // Verify both buffers have different data
    void const* data1;
    void const* data2;
    EXPECT_EQ(AAX_SUCCESS, buffer1.Data(&data1));
    EXPECT_EQ(AAX_SUCCESS, buffer2.Data(&data2));

    EXPECT_EQ(42, *static_cast<const int32_t*>(data1));
    EXPECT_EQ(84, *static_cast<const int32_t*>(data2));
    EXPECT_NE(*static_cast<const int32_t*>(data1), *static_cast<const int32_t*>(data2));
}

TEST_F(AAX_DataBuffer, ValueDataBufferOfTypeErrorHandling)
{
    int32_t intValue = 42;
    AAX_CValueDataBufferOfType<kTestTypeID1, int32_t> buffer(intValue);
    
    // Test null pointer handling
    EXPECT_EQ(AAX_ERROR_NULL_ARGUMENT, buffer.Type(nullptr));
    EXPECT_EQ(AAX_ERROR_NULL_ARGUMENT, buffer.Size(nullptr));
    EXPECT_EQ(AAX_ERROR_NULL_ARGUMENT, buffer.Data(nullptr));
}

//--------------------------------------------------------------------
#pragma mark - AAX_CValueDataBuffer Tests

TEST_F(AAX_DataBuffer, ValueDataBufferBasicConstruction)
{
    // Test construction with runtime type ID
    int32_t intValue = 42;
    AAX_CValueDataBuffer<int32_t> intBuffer(kTestTypeID1, std::move(intValue));
    
    float floatValue = 3.14f;
    AAX_CValueDataBuffer<float> floatBuffer(kTestTypeID2, std::move(floatValue));
    
    // Test Type() method
    AAX_CTypeID type;
    EXPECT_EQ(AAX_SUCCESS, intBuffer.Type(&type));
    EXPECT_EQ(kTestTypeID1, type);
    
    EXPECT_EQ(AAX_SUCCESS, floatBuffer.Type(&type));
    EXPECT_EQ(kTestTypeID2, type);
}

TEST_F(AAX_DataBuffer, ValueDataBufferSizeAndData)
{
    TestStruct structValue = {123, 4.56f, true};
    AAX_CValueDataBuffer<TestStruct> structBuffer(kTestTypeID3, std::move(structValue));
    
    // Test Size() method
    int32_t size;
    EXPECT_EQ(AAX_SUCCESS, structBuffer.Size(&size));
    EXPECT_EQ(sizeof(TestStruct), size);
    
    // Test Data() method
    void const* data;
    EXPECT_EQ(AAX_SUCCESS, structBuffer.Data(&data));
    EXPECT_NE(nullptr, data);
    
    const TestStruct* retrievedStruct = static_cast<const TestStruct*>(data);
    EXPECT_EQ(123, retrievedStruct->value1);
    EXPECT_EQ(4.56f, retrievedStruct->value2);
    EXPECT_EQ(true, retrievedStruct->value3);
}

TEST_F(AAX_DataBuffer, ValueDataBufferMultipleInstances)
{
    int32_t intValue1 = 42;
    int32_t intValue2 = 84;
    AAX_CValueDataBuffer<int32_t> buffer1(kTestTypeID1, std::move(intValue1));
    AAX_CValueDataBuffer<int32_t> buffer2(kTestTypeID2, std::move(intValue2));

    // Verify both buffers have different types and data
    AAX_CTypeID type1, type2;
    EXPECT_EQ(AAX_SUCCESS, buffer1.Type(&type1));
    EXPECT_EQ(AAX_SUCCESS, buffer2.Type(&type2));
    EXPECT_EQ(kTestTypeID1, type1);
    EXPECT_EQ(kTestTypeID2, type2);
    EXPECT_NE(type1, type2);

    void const* data1;
    void const* data2;
    EXPECT_EQ(AAX_SUCCESS, buffer1.Data(&data1));
    EXPECT_EQ(AAX_SUCCESS, buffer2.Data(&data2));

    EXPECT_EQ(42, *static_cast<const int32_t*>(data1));
    EXPECT_EQ(84, *static_cast<const int32_t*>(data2));
}

TEST_F(AAX_DataBuffer, ValueDataBufferErrorHandling)
{
    int32_t intValue = 42;
    AAX_CValueDataBuffer<int32_t> buffer(kTestTypeID1, std::move(intValue));
    
    // Test null pointer handling
    EXPECT_EQ(AAX_ERROR_NULL_ARGUMENT, buffer.Type(nullptr));
    EXPECT_EQ(AAX_ERROR_NULL_ARGUMENT, buffer.Size(nullptr));
    EXPECT_EQ(AAX_ERROR_NULL_ARGUMENT, buffer.Data(nullptr));
}

//--------------------------------------------------------------------
#pragma mark - AAX_CArrayDataBufferOfType Tests

TEST_F(AAX_DataBuffer, ArrayDataBufferOfTypeBasicConstruction)
{
    // Test construction with vector data
    std::vector<int32_t> intVector = {1, 2, 3, 4, 5};
    AAX_CArrayDataBufferOfType<kTestTypeID1, int32_t> intBuffer(intVector);
    
    std::vector<float> floatVector = {1.1f, 2.2f, 3.3f};
    AAX_CArrayDataBufferOfType<kTestTypeID2, float> floatBuffer(floatVector);
    
    // Test Type() method
    AAX_CTypeID type;
    EXPECT_EQ(AAX_SUCCESS, intBuffer.Type(&type));
    EXPECT_EQ(kTestTypeID1, type);
    
    EXPECT_EQ(AAX_SUCCESS, floatBuffer.Type(&type));
    EXPECT_EQ(kTestTypeID2, type);
}

TEST_F(AAX_DataBuffer, ArrayDataBufferOfTypeSize)
{
    std::vector<int32_t> intVector = {1, 2, 3, 4, 5};
    AAX_CArrayDataBufferOfType<kTestTypeID1, int32_t> intBuffer(intVector);
    
    std::vector<TestStruct> structVector = {{1, 1.1f, true}, {2, 2.2f, false}};
    AAX_CArrayDataBufferOfType<kTestTypeID3, TestStruct> structBuffer(structVector);
    
    // Test Size() method
    int32_t size;
    EXPECT_EQ(AAX_SUCCESS, intBuffer.Size(&size));
    EXPECT_EQ(5 * sizeof(int32_t), size);
    
    EXPECT_EQ(AAX_SUCCESS, structBuffer.Size(&size));
    EXPECT_EQ(2 * sizeof(TestStruct), size);
}

TEST_F(AAX_DataBuffer, ArrayDataBufferOfTypeData)
{
    std::vector<int32_t> intVector = {1, 2, 3, 4, 5};
    AAX_CArrayDataBufferOfType<kTestTypeID1, int32_t> intBuffer(intVector);
    
    // Test Data() method
    void const* data;
    EXPECT_EQ(AAX_SUCCESS, intBuffer.Data(&data));
    EXPECT_NE(nullptr, data);
    
    const int32_t* intArray = static_cast<const int32_t*>(data);
    for (size_t i = 0; i < intVector.size(); ++i) {
        EXPECT_EQ(intVector[i], intArray[i]);
    }
}

TEST_F(AAX_DataBuffer, ArrayDataBufferOfTypeEmptyVector)
{
    // Test with empty vector
    std::vector<int32_t> emptyVector;
    AAX_CArrayDataBufferOfType<kTestTypeID1, int32_t> emptyBuffer(emptyVector);
    
    int32_t size;
    EXPECT_EQ(AAX_SUCCESS, emptyBuffer.Size(&size));
    EXPECT_EQ(0, size);
    
    void const* data;
    EXPECT_EQ(AAX_SUCCESS, emptyBuffer.Data(&data));
    // Data pointer may be nullptr for empty vector
}

TEST_F(AAX_DataBuffer, ArrayDataBufferOfTypeMoveConstruction)
{
    // Test move construction
    std::vector<int32_t> intVector = {1, 2, 3, 4, 5};
    AAX_CArrayDataBufferOfType<kTestTypeID1, int32_t> intBuffer(std::move(intVector));

    void const* data;
    EXPECT_EQ(AAX_SUCCESS, intBuffer.Data(&data));
    EXPECT_NE(nullptr, data);

    const int32_t* intArray = static_cast<const int32_t*>(data);
    EXPECT_EQ(1, intArray[0]);
    EXPECT_EQ(5, intArray[4]);
}

TEST_F(AAX_DataBuffer, ArrayDataBufferOfTypeErrorHandling)
{
    std::vector<int32_t> intVector = {1, 2, 3};
    AAX_CArrayDataBufferOfType<kTestTypeID1, int32_t> buffer(intVector);

    // Test null pointer handling
    EXPECT_EQ(AAX_ERROR_NULL_ARGUMENT, buffer.Type(nullptr));
    EXPECT_EQ(AAX_ERROR_NULL_ARGUMENT, buffer.Size(nullptr));
    EXPECT_EQ(AAX_ERROR_NULL_ARGUMENT, buffer.Data(nullptr));
}

//--------------------------------------------------------------------
#pragma mark - AAX_CArrayDataBuffer Tests

TEST_F(AAX_DataBuffer, ArrayDataBufferBasicConstruction)
{
    // Test construction with runtime type ID
    std::vector<int32_t> intVector = {1, 2, 3, 4, 5};
    AAX_CArrayDataBuffer<int32_t> intBuffer(kTestTypeID1, intVector);

    std::vector<float> floatVector = {1.1f, 2.2f, 3.3f};
    AAX_CArrayDataBuffer<float> floatBuffer(kTestTypeID2, floatVector);

    // Test Type() method
    AAX_CTypeID type;
    EXPECT_EQ(AAX_SUCCESS, intBuffer.Type(&type));
    EXPECT_EQ(kTestTypeID1, type);

    EXPECT_EQ(AAX_SUCCESS, floatBuffer.Type(&type));
    EXPECT_EQ(kTestTypeID2, type);
}

TEST_F(AAX_DataBuffer, ArrayDataBufferSizeAndData)
{
    std::vector<TestStruct> structVector = {{1, 1.1f, true}, {2, 2.2f, false}, {3, 3.3f, true}};
    AAX_CArrayDataBuffer<TestStruct> structBuffer(kTestTypeID3, structVector);

    // Test Size() method
    int32_t size;
    EXPECT_EQ(AAX_SUCCESS, structBuffer.Size(&size));
    EXPECT_EQ(3 * sizeof(TestStruct), size);

    // Test Data() method
    void const* data;
    EXPECT_EQ(AAX_SUCCESS, structBuffer.Data(&data));
    EXPECT_NE(nullptr, data);

    const TestStruct* structArray = static_cast<const TestStruct*>(data);
    for (size_t i = 0; i < structVector.size(); ++i) {
        EXPECT_EQ(structVector[i], structArray[i]);
    }
}

TEST_F(AAX_DataBuffer, ArrayDataBufferMoveConstruction)
{
    // Test move construction
    std::vector<std::string> stringVector = {"hello", "world", "test"};
    AAX_CArrayDataBuffer<std::string> stringBuffer(kTestTypeID4, std::move(stringVector));

    void const* data;
    EXPECT_EQ(AAX_SUCCESS, stringBuffer.Data(&data));
    EXPECT_NE(nullptr, data);

    const std::string* stringArray = static_cast<const std::string*>(data);
    EXPECT_EQ("hello", stringArray[0]);
    EXPECT_EQ("world", stringArray[1]);
    EXPECT_EQ("test", stringArray[2]);
}

TEST_F(AAX_DataBuffer, ArrayDataBufferErrorHandling)
{
    std::vector<int32_t> intVector = {1, 2, 3};
    AAX_CArrayDataBuffer<int32_t> buffer(kTestTypeID1, intVector);

    // Test null pointer handling
    EXPECT_EQ(AAX_ERROR_NULL_ARGUMENT, buffer.Type(nullptr));
    EXPECT_EQ(AAX_ERROR_NULL_ARGUMENT, buffer.Size(nullptr));
    EXPECT_EQ(AAX_ERROR_NULL_ARGUMENT, buffer.Data(nullptr));
}

//--------------------------------------------------------------------
#pragma mark - AAX_CStringDataBufferOfType Tests

TEST_F(AAX_DataBuffer, StringDataBufferOfTypeBasicConstruction)
{
    // Test construction with std::string
    std::string testString = "Hello, World!";
    AAX_CStringDataBufferOfType<kTestTypeID1> stringBuffer(testString);

    // Test Type() method
    AAX_CTypeID type;
    EXPECT_EQ(AAX_SUCCESS, stringBuffer.Type(&type));
    EXPECT_EQ(kTestTypeID1, type);
}

TEST_F(AAX_DataBuffer, StringDataBufferOfTypeCStringConstruction)
{
    // Test construction with C-string
    const char* cString = "Test C-string";
    AAX_CStringDataBufferOfType<kTestTypeID2> cStringBuffer(cString);

    // Test with null C-string
    AAX_CStringDataBufferOfType<kTestTypeID3> nullStringBuffer(nullptr);

    AAX_CTypeID type;
    EXPECT_EQ(AAX_SUCCESS, cStringBuffer.Type(&type));
    EXPECT_EQ(kTestTypeID2, type);

    EXPECT_EQ(AAX_SUCCESS, nullStringBuffer.Type(&type));
    EXPECT_EQ(kTestTypeID3, type);
}

TEST_F(AAX_DataBuffer, StringDataBufferOfTypeSize)
{
    std::string testString = "Hello";
    AAX_CStringDataBufferOfType<kTestTypeID1> stringBuffer(testString);

    // Test Size() method - should include null terminator
    int32_t size;
    EXPECT_EQ(AAX_SUCCESS, stringBuffer.Size(&size));
    EXPECT_EQ(6, size); // "Hello" + null terminator

    // Test empty string
    std::string emptyString = "";
    AAX_CStringDataBufferOfType<kTestTypeID2> emptyBuffer(emptyString);
    EXPECT_EQ(AAX_SUCCESS, emptyBuffer.Size(&size));
    EXPECT_EQ(1, size); // Just null terminator
}

TEST_F(AAX_DataBuffer, StringDataBufferOfTypeData)
{
    std::string testString = "Hello, World!";
    AAX_CStringDataBufferOfType<kTestTypeID1> stringBuffer(testString);

    // Test Data() method
    void const* data;
    EXPECT_EQ(AAX_SUCCESS, stringBuffer.Data(&data));
    EXPECT_NE(nullptr, data);

    const char* cString = static_cast<const char*>(data);
    EXPECT_STREQ("Hello, World!", cString);

    // Verify null termination
    EXPECT_EQ('\0', cString[testString.length()]);
}

TEST_F(AAX_DataBuffer, StringDataBufferOfTypeConstruction)
{
    // Test construction with string literal
    AAX_CStringDataBufferOfType<kTestTypeID1> stringBuffer("Test string literal");

    void const* data;
    EXPECT_EQ(AAX_SUCCESS, stringBuffer.Data(&data));
    EXPECT_NE(nullptr, data);

    const char* cString = static_cast<const char*>(data);
    EXPECT_STREQ("Test string literal", cString);
}

TEST_F(AAX_DataBuffer, StringDataBufferOfTypeErrorHandling)
{
    std::string testString = "Test";
    AAX_CStringDataBufferOfType<kTestTypeID1> buffer(testString);

    // Test null pointer handling
    EXPECT_EQ(AAX_ERROR_NULL_ARGUMENT, buffer.Type(nullptr));
    EXPECT_EQ(AAX_ERROR_NULL_ARGUMENT, buffer.Size(nullptr));
    EXPECT_EQ(AAX_ERROR_NULL_ARGUMENT, buffer.Data(nullptr));
}

//--------------------------------------------------------------------
#pragma mark - AAX_CStringDataBuffer Tests

TEST_F(AAX_DataBuffer, StringDataBufferBasicConstruction)
{
    // Test construction with runtime type ID
    std::string testString = "Hello, World!";
    AAX_CStringDataBuffer stringBuffer(kTestTypeID1, testString);

    const char* cString = "Test C-string";
    AAX_CStringDataBuffer cStringBuffer(kTestTypeID2, cString);

    // Test Type() method
    AAX_CTypeID type;
    EXPECT_EQ(AAX_SUCCESS, stringBuffer.Type(&type));
    EXPECT_EQ(kTestTypeID1, type);

    EXPECT_EQ(AAX_SUCCESS, cStringBuffer.Type(&type));
    EXPECT_EQ(kTestTypeID2, type);
}

TEST_F(AAX_DataBuffer, StringDataBufferSizeAndData)
{
    std::string testString = "Test string";
    AAX_CStringDataBuffer stringBuffer(kTestTypeID1, testString);

    // Test Size() method - should include null terminator
    int32_t size;
    EXPECT_EQ(AAX_SUCCESS, stringBuffer.Size(&size));
    EXPECT_EQ(12, size); // "Test string" + null terminator

    // Test Data() method
    void const* data;
    EXPECT_EQ(AAX_SUCCESS, stringBuffer.Data(&data));
    EXPECT_NE(nullptr, data);

    const char* cString = static_cast<const char*>(data);
    EXPECT_STREQ("Test string", cString);
}

TEST_F(AAX_DataBuffer, StringDataBufferConstruction)
{
    // Test construction with string literal
    AAX_CStringDataBuffer stringBuffer(kTestTypeID1, "Test string literal");

    void const* data;
    EXPECT_EQ(AAX_SUCCESS, stringBuffer.Data(&data));
    EXPECT_NE(nullptr, data);

    const char* cString = static_cast<const char*>(data);
    EXPECT_STREQ("Test string literal", cString);
}

TEST_F(AAX_DataBuffer, StringDataBufferNullHandling)
{
    // Test construction with null C-string
    AAX_CStringDataBuffer nullBuffer(kTestTypeID1, static_cast<const char*>(nullptr));

    int32_t size;
    EXPECT_EQ(AAX_SUCCESS, nullBuffer.Size(&size));
    EXPECT_EQ(1, size); // Just null terminator for empty string

    void const* data;
    EXPECT_EQ(AAX_SUCCESS, nullBuffer.Data(&data));
    EXPECT_NE(nullptr, data);

    const char* cString = static_cast<const char*>(data);
    EXPECT_STREQ("", cString);
}

TEST_F(AAX_DataBuffer, StringDataBufferErrorHandling)
{
    std::string testString = "Test";
    AAX_CStringDataBuffer buffer(kTestTypeID1, testString);

    // Test null pointer handling
    EXPECT_EQ(AAX_ERROR_NULL_ARGUMENT, buffer.Type(nullptr));
    EXPECT_EQ(AAX_ERROR_NULL_ARGUMENT, buffer.Size(nullptr));
    EXPECT_EQ(AAX_ERROR_NULL_ARGUMENT, buffer.Data(nullptr));
}

//--------------------------------------------------------------------
#pragma mark - AAX_CDataBufferOfTypeAndSize Tests

TEST_F(AAX_DataBuffer, DataBufferOfTypeAndSizeBasicConstruction)
{
    // Test construction with fixed size
    constexpr size_t bufferSize = 64;
    uint8_t testData[bufferSize];
    for (size_t i = 0; i < bufferSize; ++i) {
        testData[i] = static_cast<uint8_t>(i);
    }

    AAX_CDataBufferOfTypeAndSize<kTestTypeID1, bufferSize> buffer(testData);

    // Test Type() method
    AAX_CTypeID type;
    EXPECT_EQ(AAX_SUCCESS, buffer.Type(&type));
    EXPECT_EQ(kTestTypeID1, type);

    // Test Size() method
    int32_t size;
    EXPECT_EQ(AAX_SUCCESS, buffer.Size(&size));
    EXPECT_EQ(bufferSize, size);
}

TEST_F(AAX_DataBuffer, DataBufferOfTypeAndSizeData)
{
    constexpr size_t bufferSize = 32;
    uint8_t testData[bufferSize];
    for (size_t i = 0; i < bufferSize; ++i) {
        testData[i] = static_cast<uint8_t>(i * 2);
    }

    AAX_CDataBufferOfTypeAndSize<kTestTypeID1, bufferSize> buffer(testData);

    // Test Data() method
    void const* data;
    EXPECT_EQ(AAX_SUCCESS, buffer.Data(&data));
    EXPECT_NE(nullptr, data);

    const uint8_t* retrievedData = static_cast<const uint8_t*>(data);
    for (size_t i = 0; i < bufferSize; ++i) {
        EXPECT_EQ(testData[i], retrievedData[i]);
    }
}

TEST_F(AAX_DataBuffer, DataBufferOfTypeAndSizeNullConstruction)
{
    // Test construction with null data - should zero-initialize
    constexpr size_t bufferSize = 16;
    AAX_CDataBufferOfTypeAndSize<kTestTypeID1, bufferSize> buffer(nullptr);

    void const* data;
    EXPECT_EQ(AAX_SUCCESS, buffer.Data(&data));
    EXPECT_NE(nullptr, data);

    const uint8_t* retrievedData = static_cast<const uint8_t*>(data);
    for (size_t i = 0; i < bufferSize; ++i) {
        EXPECT_EQ(0, retrievedData[i]);
    }
}

TEST_F(AAX_DataBuffer, DataBufferOfTypeAndSizeMultipleInstances)
{
    constexpr size_t bufferSize = 8;
    uint8_t testData1[bufferSize] = {1, 2, 3, 4, 5, 6, 7, 8};
    uint8_t testData2[bufferSize] = {8, 7, 6, 5, 4, 3, 2, 1};

    AAX_CDataBufferOfTypeAndSize<kTestTypeID1, bufferSize> buffer1(testData1);
    AAX_CDataBufferOfTypeAndSize<kTestTypeID1, bufferSize> buffer2(testData2);

    void const* data1;
    void const* data2;
    EXPECT_EQ(AAX_SUCCESS, buffer1.Data(&data1));
    EXPECT_EQ(AAX_SUCCESS, buffer2.Data(&data2));

    // Data should be different pointers with different content
    EXPECT_NE(data1, data2);
    EXPECT_NE(0, std::memcmp(data1, data2, bufferSize));

    // Verify specific content
    const uint8_t* retrievedData1 = static_cast<const uint8_t*>(data1);
    const uint8_t* retrievedData2 = static_cast<const uint8_t*>(data2);
    EXPECT_EQ(1, retrievedData1[0]);
    EXPECT_EQ(8, retrievedData1[7]);
    EXPECT_EQ(8, retrievedData2[0]);
    EXPECT_EQ(1, retrievedData2[7]);
}

//--------------------------------------------------------------------
#pragma mark - AAX_CDataBufferOfType Tests

TEST_F(AAX_DataBuffer, DataBufferOfTypeBasicConstruction)
{
    // Test construction with variable size
    constexpr int32_t bufferSize = 64;
    uint8_t testData[bufferSize];
    for (int32_t i = 0; i < bufferSize; ++i) {
        testData[i] = static_cast<uint8_t>(i);
    }

    AAX_CDataBufferOfType<kTestTypeID1> buffer(testData, bufferSize);

    // Test Type() method
    AAX_CTypeID type;
    EXPECT_EQ(AAX_SUCCESS, buffer.Type(&type));
    EXPECT_EQ(kTestTypeID1, type);

    // Test Size() method
    int32_t size;
    EXPECT_EQ(AAX_SUCCESS, buffer.Size(&size));
    EXPECT_EQ(bufferSize, size);
}

TEST_F(AAX_DataBuffer, DataBufferOfTypeZeroSize)
{
    // Test construction with zero size
    AAX_CDataBufferOfType<kTestTypeID1> buffer(nullptr, 0);

    int32_t size;
    EXPECT_EQ(AAX_SUCCESS, buffer.Size(&size));
    EXPECT_EQ(0, size);

    void const* data;
    EXPECT_EQ(AAX_SUCCESS, buffer.Data(&data));
    // Data pointer may be nullptr for zero size
}

TEST_F(AAX_DataBuffer, DataBufferOfTypeNegativeSize)
{
    // Test construction with negative size
    uint8_t testData[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    AAX_CDataBufferOfType<kTestTypeID1> buffer(testData, -5);

    int32_t size;
    EXPECT_EQ(AAX_SUCCESS, buffer.Size(&size));
    EXPECT_EQ(-5, size); // Should preserve the negative size
}

//--------------------------------------------------------------------
#pragma mark - AAX_CDataBuffer Tests

TEST_F(AAX_DataBuffer, DataBufferBasicConstruction)
{
    // Test construction with runtime type ID and variable size
    constexpr int32_t bufferSize = 32;
    uint8_t testData[bufferSize];
    for (int32_t i = 0; i < bufferSize; ++i) {
        testData[i] = static_cast<uint8_t>(i * 3);
    }

    AAX_CDataBuffer buffer(kTestTypeID1, testData, bufferSize);

    // Test Type() method
    AAX_CTypeID type;
    EXPECT_EQ(AAX_SUCCESS, buffer.Type(&type));
    EXPECT_EQ(kTestTypeID1, type);

    // Test Size() method
    int32_t size;
    EXPECT_EQ(AAX_SUCCESS, buffer.Size(&size));
    EXPECT_EQ(bufferSize, size);
}

TEST_F(AAX_DataBuffer, DataBufferDataVerification)
{
    constexpr int32_t bufferSize = 16;
    uint8_t testData[bufferSize];
    for (int32_t i = 0; i < bufferSize; ++i) {
        testData[i] = static_cast<uint8_t>(255 - i);
    }

    AAX_CDataBuffer buffer(kTestTypeID1, testData, bufferSize);

    // Test Data() method
    void const* data;
    EXPECT_EQ(AAX_SUCCESS, buffer.Data(&data));
    EXPECT_NE(nullptr, data);

    const uint8_t* retrievedData = static_cast<const uint8_t*>(data);
    for (int32_t i = 0; i < bufferSize; ++i) {
        EXPECT_EQ(testData[i], retrievedData[i]);
    }
}

TEST_F(AAX_DataBuffer, DataBufferNullDataConstruction)
{
    // Test construction with null data - should zero-initialize
    constexpr int32_t bufferSize = 8;
    AAX_CDataBuffer buffer(kTestTypeID1, nullptr, bufferSize);

    void const* data;
    EXPECT_EQ(AAX_SUCCESS, buffer.Data(&data));
    EXPECT_NE(nullptr, data);

    const uint8_t* retrievedData = static_cast<const uint8_t*>(data);
    for (int32_t i = 0; i < bufferSize; ++i) {
        EXPECT_EQ(0, retrievedData[i]);
    }
}

TEST_F(AAX_DataBuffer, DataBufferMultipleInstances)
{
    constexpr int32_t bufferSize = 12;
    uint8_t testData1[bufferSize] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120};
    uint8_t testData2[bufferSize] = {120, 110, 100, 90, 80, 70, 60, 50, 40, 30, 20, 10};

    AAX_CDataBuffer buffer1(kTestTypeID1, testData1, bufferSize);
    AAX_CDataBuffer buffer2(kTestTypeID2, testData2, bufferSize);

    // Verify type and size
    AAX_CTypeID type1, type2;
    int32_t size1, size2;

    EXPECT_EQ(AAX_SUCCESS, buffer1.Type(&type1));
    EXPECT_EQ(AAX_SUCCESS, buffer2.Type(&type2));
    EXPECT_EQ(kTestTypeID1, type1);
    EXPECT_EQ(kTestTypeID2, type2);
    EXPECT_NE(type1, type2);

    EXPECT_EQ(AAX_SUCCESS, buffer1.Size(&size1));
    EXPECT_EQ(AAX_SUCCESS, buffer2.Size(&size2));
    EXPECT_EQ(bufferSize, size1);
    EXPECT_EQ(bufferSize, size2);

    // Verify data
    void const* data1;
    void const* data2;
    EXPECT_EQ(AAX_SUCCESS, buffer1.Data(&data1));
    EXPECT_EQ(AAX_SUCCESS, buffer2.Data(&data2));

    EXPECT_NE(data1, data2); // Different pointers
    EXPECT_NE(0, std::memcmp(data1, data2, bufferSize)); // Different content
}

TEST_F(AAX_DataBuffer, DataBufferWithDifferentSizes)
{
    constexpr int32_t bufferSize1 = 8;
    constexpr int32_t bufferSize2 = 16;
    uint8_t testData1[bufferSize1] = {1, 2, 3, 4, 5, 6, 7, 8};
    uint8_t testData2[bufferSize2] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

    AAX_CDataBuffer buffer1(kTestTypeID1, testData1, bufferSize1);
    AAX_CDataBuffer buffer2(kTestTypeID2, testData2, bufferSize2);

    // Verify different sizes
    int32_t size1, size2;
    EXPECT_EQ(AAX_SUCCESS, buffer1.Size(&size1));
    EXPECT_EQ(AAX_SUCCESS, buffer2.Size(&size2));
    EXPECT_EQ(bufferSize1, size1);
    EXPECT_EQ(bufferSize2, size2);
    EXPECT_NE(size1, size2);

    // Verify data content
    void const* data1;
    void const* data2;
    EXPECT_EQ(AAX_SUCCESS, buffer1.Data(&data1));
    EXPECT_EQ(AAX_SUCCESS, buffer2.Data(&data2));

    const uint8_t* retrievedData1 = static_cast<const uint8_t*>(data1);
    const uint8_t* retrievedData2 = static_cast<const uint8_t*>(data2);

    // First 8 bytes should be the same
    for (int32_t i = 0; i < bufferSize1; ++i) {
        EXPECT_EQ(retrievedData1[i], retrievedData2[i]);
    }

    // Buffer2 should have additional data
    EXPECT_EQ(9, retrievedData2[8]);
    EXPECT_EQ(16, retrievedData2[15]);
}

//--------------------------------------------------------------------
#pragma mark - Boundary and Edge Case Tests

TEST_F(AAX_DataBuffer, LargeDataBufferTest)
{
    // Test with large buffer size
    constexpr int32_t largeSize = 1024 * 1024; // 1MB
    std::vector<uint8_t> largeData(largeSize);
    for (int32_t i = 0; i < largeSize; ++i) {
        largeData[i] = static_cast<uint8_t>(i % 256);
    }

    AAX_CDataBuffer largeBuffer(kTestTypeID1, largeData.data(), largeSize);

    int32_t size;
    EXPECT_EQ(AAX_SUCCESS, largeBuffer.Size(&size));
    EXPECT_EQ(largeSize, size);

    void const* data;
    EXPECT_EQ(AAX_SUCCESS, largeBuffer.Data(&data));
    EXPECT_NE(nullptr, data);

    // Verify first and last bytes
    const uint8_t* retrievedData = static_cast<const uint8_t*>(data);
    EXPECT_EQ(largeData[0], retrievedData[0]);
    EXPECT_EQ(largeData[largeSize - 1], retrievedData[largeSize - 1]);
}

TEST_F(AAX_DataBuffer, StringDataBufferSpecialCharacters)
{
    // Test with special characters and Unicode
    std::string specialString = "Hello\nWorld\t\r\0Test\x01\xFF";
    AAX_CStringDataBuffer specialBuffer(kTestTypeID1, specialString);

    int32_t size;
    EXPECT_EQ(AAX_SUCCESS, specialBuffer.Size(&size));
    EXPECT_EQ(specialString.size() + 1, size); // +1 for null terminator

    void const* data;
    EXPECT_EQ(AAX_SUCCESS, specialBuffer.Data(&data));
    EXPECT_NE(nullptr, data);

    const char* retrievedString = static_cast<const char*>(data);
    EXPECT_EQ(0, std::memcmp(specialString.c_str(), retrievedString, specialString.size()));
}

TEST_F(AAX_DataBuffer, ArrayDataBufferLargeVector)
{
    // Test with large vector
    constexpr size_t largeVectorSize = 10000;
    std::vector<int32_t> largeVector(largeVectorSize);
    for (size_t i = 0; i < largeVectorSize; ++i) {
        largeVector[i] = static_cast<int32_t>(i * i);
    }

    AAX_CArrayDataBuffer<int32_t> largeArrayBuffer(kTestTypeID1, largeVector);

    int32_t size;
    EXPECT_EQ(AAX_SUCCESS, largeArrayBuffer.Size(&size));
    EXPECT_EQ(largeVectorSize * sizeof(int32_t), size);

    void const* data;
    EXPECT_EQ(AAX_SUCCESS, largeArrayBuffer.Data(&data));
    EXPECT_NE(nullptr, data);

    const int32_t* retrievedArray = static_cast<const int32_t*>(data);
    EXPECT_EQ(largeVector[0], retrievedArray[0]);
    EXPECT_EQ(largeVector[largeVectorSize - 1], retrievedArray[largeVectorSize - 1]);
}
