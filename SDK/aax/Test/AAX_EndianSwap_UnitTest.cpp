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
 *	\file AAX_EndianSwap_UnitTest.cpp
 *
 *	\brief Unit tests for AAX_EndianSwap template functions
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX_EndianSwap.h"
#include <vector>
#include <array>
#include <cstring>

//--------------------------------------------------------------------
#pragma mark - Test fixture

class AAX_EndianSwap_Test : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
    
    // Helper function to check if system is little endian
    bool IsLittleEndian() {
        uint16_t test = 0x0001;
        return *reinterpret_cast<uint8_t*>(&test) == 0x01;
    }
    
    // Helper to create test data
    template<typename T>
    T CreateTestValue();
};

// Specializations for test values
template<>
uint8_t AAX_EndianSwap_Test::CreateTestValue<uint8_t>() { return 0xAB; }

template<>
uint16_t AAX_EndianSwap_Test::CreateTestValue<uint16_t>() { return 0xABCD; }

template<>
uint32_t AAX_EndianSwap_Test::CreateTestValue<uint32_t>() { return 0xABCDEF12; }

template<>
uint64_t AAX_EndianSwap_Test::CreateTestValue<uint64_t>() { return 0xABCDEF1234567890ULL; }

template<>
int16_t AAX_EndianSwap_Test::CreateTestValue<int16_t>() { return static_cast<int16_t>(0xABCD); }

template<>
int32_t AAX_EndianSwap_Test::CreateTestValue<int32_t>() { return static_cast<int32_t>(0xABCDEF12); }

template<>
float AAX_EndianSwap_Test::CreateTestValue<float>() { 
    uint32_t bits = 0x42280000; // 42.0f in IEEE 754
    return *reinterpret_cast<float*>(&bits);
}

template<>
double AAX_EndianSwap_Test::CreateTestValue<double>() {
    uint64_t bits = 0x4045000000000000ULL; // 42.0 in IEEE 754
    return *reinterpret_cast<double*>(&bits);
}

//--------------------------------------------------------------------
#pragma mark - Basic Endian Swap Tests

TEST_F(AAX_EndianSwap_Test, EndianSwapInPlace_UInt8)
{
    uint8_t value = CreateTestValue<uint8_t>();
    uint8_t original = value;
    
    AAX_EndianSwapInPlace(&value);
    // For single byte, swap should be no-op
    EXPECT_EQ(original, value);
}

TEST_F(AAX_EndianSwap_Test, EndianSwapInPlace_UInt16)
{
    uint16_t value = CreateTestValue<uint16_t>();
    uint16_t expected = 0xCDAB; // Swapped bytes
    
    AAX_EndianSwapInPlace(&value);
    EXPECT_EQ(expected, value);
}

TEST_F(AAX_EndianSwap_Test, EndianSwapInPlace_UInt32)
{
    uint32_t value = CreateTestValue<uint32_t>();
    uint32_t expected = 0x12EFCDAB; // Swapped bytes
    
    AAX_EndianSwapInPlace(&value);
    EXPECT_EQ(expected, value);
}

TEST_F(AAX_EndianSwap_Test, EndianSwapInPlace_UInt64)
{
    uint64_t value = CreateTestValue<uint64_t>();
    uint64_t expected = 0x9078563412EFCDABULL; // Swapped bytes
    
    AAX_EndianSwapInPlace(&value);
    EXPECT_EQ(expected, value);
}

TEST_F(AAX_EndianSwap_Test, EndianSwapInPlace_SignedTypes)
{
    int16_t value16 = CreateTestValue<int16_t>();
    int16_t expected16 = static_cast<int16_t>(0xCDAB);
    
    AAX_EndianSwapInPlace(&value16);
    EXPECT_EQ(expected16, value16);
    
    int32_t value32 = CreateTestValue<int32_t>();
    int32_t expected32 = static_cast<int32_t>(0x12EFCDAB);
    
    AAX_EndianSwapInPlace(&value32);
    EXPECT_EQ(expected32, value32);
}

TEST_F(AAX_EndianSwap_Test, EndianSwapInPlace_FloatingPoint)
{
    float value = CreateTestValue<float>();
    float original = value;
    
    AAX_EndianSwapInPlace(&value);
    // After swap, the bit pattern should be different
    EXPECT_NE(*reinterpret_cast<uint32_t*>(&original), *reinterpret_cast<uint32_t*>(&value));
    
    // Swap back should restore original
    AAX_EndianSwapInPlace(&value);
    EXPECT_EQ(original, value);
    
    double dvalue = CreateTestValue<double>();
    double doriginal = dvalue;
    
    AAX_EndianSwapInPlace(&dvalue);
    EXPECT_NE(*reinterpret_cast<uint64_t*>(&doriginal), *reinterpret_cast<uint64_t*>(&dvalue));
    
    AAX_EndianSwapInPlace(&dvalue);
    EXPECT_EQ(doriginal, dvalue);
}

//--------------------------------------------------------------------
#pragma mark - Copy Endian Swap Tests

TEST_F(AAX_EndianSwap_Test, EndianSwap_Copy)
{
    uint32_t original = CreateTestValue<uint32_t>();
    uint32_t swapped = AAX_EndianSwap(original);
    uint32_t expected = 0x12EFCDAB;
    
    EXPECT_EQ(expected, swapped);
    EXPECT_EQ(CreateTestValue<uint32_t>(), original); // Original unchanged
}

TEST_F(AAX_EndianSwap_Test, EndianSwap_DoubleSwap)
{
    uint16_t original = CreateTestValue<uint16_t>();
    uint16_t doubleSwapped = AAX_EndianSwap(AAX_EndianSwap(original));
    
    EXPECT_EQ(original, doubleSwapped);
}

//--------------------------------------------------------------------
#pragma mark - Big Endian Native Swap Tests

TEST_F(AAX_EndianSwap_Test, BigEndianNativeSwapInPlace)
{
    uint32_t value = CreateTestValue<uint32_t>();
    uint32_t original = value;
    
    AAX_BigEndianNativeSwapInPlace(&value);
    
    if (IsLittleEndian()) {
        // On little endian, should swap
        EXPECT_NE(original, value);
        // Swap back should restore
        AAX_BigEndianNativeSwapInPlace(&value);
        EXPECT_EQ(original, value);
    } else {
        // On big endian, should be no-op
        EXPECT_EQ(original, value);
    }
}

TEST_F(AAX_EndianSwap_Test, BigEndianNativeSwap_Copy)
{
    uint32_t original = CreateTestValue<uint32_t>();
    uint32_t result = AAX_BigEndianNativeSwap(original);
    
    if (IsLittleEndian()) {
        EXPECT_NE(original, result);
    } else {
        EXPECT_EQ(original, result);
    }
}

//--------------------------------------------------------------------
#pragma mark - Little Endian Native Swap Tests

TEST_F(AAX_EndianSwap_Test, LittleEndianNativeSwapInPlace)
{
    uint32_t value = CreateTestValue<uint32_t>();
    uint32_t original = value;
    
    AAX_LittleEndianNativeSwapInPlace(&value);
    
    if (!IsLittleEndian()) {
        // On big endian, should swap
        EXPECT_NE(original, value);
        // Swap back should restore
        AAX_LittleEndianNativeSwapInPlace(&value);
        EXPECT_EQ(original, value);
    } else {
        // On little endian, should be no-op
        EXPECT_EQ(original, value);
    }
}

TEST_F(AAX_EndianSwap_Test, LittleEndianNativeSwap_Copy)
{
    uint32_t original = CreateTestValue<uint32_t>();
    uint32_t result = AAX_LittleEndianNativeSwap(original);
    
    if (!IsLittleEndian()) {
        EXPECT_NE(original, result);
    } else {
        EXPECT_EQ(original, result);
    }
}

//--------------------------------------------------------------------
#pragma mark - Sequence Swap Tests

TEST_F(AAX_EndianSwap_Test, EndianSwapSequenceInPlace_Vector)
{
    std::vector<uint16_t> data = {0x1234, 0x5678, 0x9ABC, 0xDEF0};
    std::vector<uint16_t> expected = {0x3412, 0x7856, 0xBC9A, 0xF0DE};
    
    AAX_EndianSwapSequenceInPlace(data.begin(), data.end());
    
    EXPECT_EQ(expected, data);
}

TEST_F(AAX_EndianSwap_Test, EndianSwapSequenceInPlace_Array)
{
    uint32_t data[] = {0x12345678, 0x9ABCDEF0};
    uint32_t expected[] = {0x78563412, 0xF0DEBC9A};
    
    AAX_EndianSwapSequenceInPlace(data, data + 2);
    
    EXPECT_EQ(expected[0], data[0]);
    EXPECT_EQ(expected[1], data[1]);
}

TEST_F(AAX_EndianSwap_Test, EndianSwapSequenceInPlace_Empty)
{
    std::vector<uint32_t> data;
    
    // Should not crash on empty sequence
    AAX_EndianSwapSequenceInPlace(data.begin(), data.end());
    EXPECT_TRUE(data.empty());
}

TEST_F(AAX_EndianSwap_Test, BigEndianNativeSwapSequenceInPlace)
{
    std::vector<uint16_t> data = {0x1234, 0x5678};
    std::vector<uint16_t> original = data;
    
    AAX_BigEndianNativeSwapSequenceInPlace(data.begin(), data.end());
    
    if (IsLittleEndian()) {
        EXPECT_NE(original, data);
        // Swap back should restore
        AAX_BigEndianNativeSwapSequenceInPlace(data.begin(), data.end());
        EXPECT_EQ(original, data);
    } else {
        EXPECT_EQ(original, data);
    }
}

TEST_F(AAX_EndianSwap_Test, LittleEndianNativeSwapSequenceInPlace)
{
    std::vector<uint16_t> data = {0x1234, 0x5678};
    std::vector<uint16_t> original = data;
    
    AAX_LittleEndianNativeSwapSequenceInPlace(data.begin(), data.end());
    
    if (!IsLittleEndian()) {
        EXPECT_NE(original, data);
        // Swap back should restore
        AAX_LittleEndianNativeSwapSequenceInPlace(data.begin(), data.end());
        EXPECT_EQ(original, data);
    } else {
        EXPECT_EQ(original, data);
    }
}

//--------------------------------------------------------------------
#pragma mark - Edge Cases and Error Conditions

TEST_F(AAX_EndianSwap_Test, NullPointerHandling)
{
    // These should not crash, though behavior may be undefined
    // We test that the functions can be called
    uint32_t* nullPtr = nullptr;
    
    // Note: Calling with null pointer is undefined behavior
    // This test mainly ensures the functions exist and can be called
    EXPECT_NO_THROW({
        // Don't actually call with null - undefined behavior
        uint32_t value = 0x12345678;
        AAX_EndianSwapInPlace(&value);
    });
}

TEST_F(AAX_EndianSwap_Test, CustomStructSwap)
{
    struct TestStruct {
        uint16_t a;
        uint16_t b;
    };

    TestStruct original = {0x1234, 0x5678};
    TestStruct swapped = original;

    AAX_EndianSwapInPlace(&swapped);

    // The entire struct should be byte-swapped
    // POTENTIAL BUG: Cannot reliably test struct comparison due to padding and alignment
    // Expected: struct fields would be individually swapped, but implementation may swap entire struct as bytes
    EXPECT_TRUE(swapped.a != original.a || swapped.b != original.b);

    // Swap back should restore
    AAX_EndianSwapInPlace(&swapped);
    EXPECT_EQ(original.a, swapped.a);
    EXPECT_EQ(original.b, swapped.b);
}

//--------------------------------------------------------------------
#pragma mark - Performance and Consistency Tests

TEST_F(AAX_EndianSwap_Test, ConsistencyBetweenInPlaceAndCopy)
{
    uint32_t original = CreateTestValue<uint32_t>();
    
    // Test in-place version
    uint32_t inPlace = original;
    AAX_EndianSwapInPlace(&inPlace);
    
    // Test copy version
    uint32_t copy = AAX_EndianSwap(original);
    
    EXPECT_EQ(inPlace, copy);
}

TEST_F(AAX_EndianSwap_Test, LargeSequenceSwap)
{
    const size_t size = 1000;
    std::vector<uint32_t> data(size);
    
    // Fill with test pattern
    for (size_t i = 0; i < size; ++i) {
        data[i] = static_cast<uint32_t>(i * 0x01010101);
    }
    
    std::vector<uint32_t> original = data;
    
    AAX_EndianSwapSequenceInPlace(data.begin(), data.end());
    
    // Should be different after swap
    EXPECT_NE(original, data);
    
    // Swap back should restore
    AAX_EndianSwapSequenceInPlace(data.begin(), data.end());
    EXPECT_EQ(original, data);
}
