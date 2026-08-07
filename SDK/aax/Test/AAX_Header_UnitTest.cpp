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
 *	\file AAX_Header_UnitTest.cpp
 *
 *	\brief Unit tests for AAX.h header definitions and utilities
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX.h"
#include <algorithm>
#include <limits>
#include <cstddef>

//--------------------------------------------------------------------
#pragma mark - Test fixture

class AAX_Header : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
    
    // Test structure for AAX_FIELD_INDEX testing
    struct TestContext {
        void* field1;
        void* field2;
        void* field3;
        int32_t intField;
        void* field4;
    };
};

//--------------------------------------------------------------------
#pragma mark - Type Definition Tests

TEST_F(AAX_Header, TypeDefinitionSizes)
{
    // Test that basic types have expected sizes
    static_assert(sizeof(AAX_CIndex) == sizeof(int32_t), "AAX_CIndex must be int32_t sized");
    static_assert(sizeof(AAX_CCount) == sizeof(AAX_CIndex), "AAX_CCount must be AAX_CIndex sized");
    static_assert(sizeof(AAX_CBoolean) == sizeof(uint8_t), "AAX_CBoolean must be uint8_t sized");
    static_assert(sizeof(AAX_CSelector) == sizeof(uint32_t), "AAX_CSelector must be uint32_t sized");
    static_assert(sizeof(AAX_CTimestamp) == sizeof(int64_t), "AAX_CTimestamp must be int64_t sized");
    static_assert(sizeof(AAX_CTypeID) == sizeof(uint32_t), "AAX_CTypeID must be uint32_t sized");
    static_assert(sizeof(AAX_Result) == sizeof(int32_t), "AAX_Result must be int32_t sized");
    static_assert(sizeof(AAX_CPropertyValue) == sizeof(int32_t), "AAX_CPropertyValue must be int32_t sized");
    static_assert(sizeof(AAX_CPropertyValue64) == sizeof(int64_t), "AAX_CPropertyValue64 must be int64_t sized");
}

TEST_F(AAX_Header, TypeDefinitionAliases)
{
    // Test that type aliases are correctly defined
    static_assert(std::is_same<AAX_CIndex, AAX_CFieldIndex>::value, "AAX_CFieldIndex should be AAX_CIndex");
    static_assert(std::is_same<AAX_CSelector, AAX_CComponentID>::value, "AAX_CComponentID should be AAX_CSelector");
    static_assert(std::is_same<AAX_CSelector, AAX_CMeterID>::value, "AAX_CMeterID should be AAX_CSelector");
    static_assert(std::is_same<const char*, AAX_CParamID>::value, "AAX_CParamID should be const char*");
}

TEST_F(AAX_Header, ConstantValues)
{
    // Test constant values
    static_assert(kAAX_TypeID_Undefined == 0U, "kAAX_TypeID_Undefined must be 0");
    static_assert(kAAX_InstanceGroupID_Undefined == 0U, "kAAX_InstanceGroupID_Undefined must be 0");
    static_assert(kAAX_TaskID_Undefined == 0ULL, "kAAX_TaskID_Undefined must be 0");
    static_assert(kAAX_ParameterIdentifierMaxSize == 32U, "kAAX_ParameterIdentifierMaxSize must be 32");
    static_assert(kAAX_Never == static_cast<AAX_CTimestamp>(~0ULL), "kAAX_Never must be max timestamp");
}

//--------------------------------------------------------------------
#pragma mark - AAX_FIELD_INDEX Macro Tests

TEST_F(AAX_Header, FieldIndexMacroBasic)
{
    // Test AAX_FIELD_INDEX macro with basic fields
    AAX_CFieldIndex index0 = AAX_FIELD_INDEX(TestContext, field1);
    AAX_CFieldIndex index1 = AAX_FIELD_INDEX(TestContext, field2);
    AAX_CFieldIndex index2 = AAX_FIELD_INDEX(TestContext, field3);
    
    EXPECT_EQ(0, index0);
    EXPECT_EQ(1, index1);
    EXPECT_EQ(2, index2);
}

TEST_F(AAX_Header, FieldIndexMacroNonPointerField)
{
    // Test AAX_FIELD_INDEX with non-pointer field
    AAX_CFieldIndex intIndex = AAX_FIELD_INDEX(TestContext, intField);
    
    // intField should be at offset 3 * sizeof(void*) / sizeof(void*) = 3
    EXPECT_EQ(3, intIndex);
}

TEST_F(AAX_Header, FieldIndexMacroLastField)
{
    // Test AAX_FIELD_INDEX with last field
    AAX_CFieldIndex lastIndex = AAX_FIELD_INDEX(TestContext, field4);

    // field4 should be at offset after intField, but we need to account for alignment
    // The actual offset depends on struct packing, so we'll just verify it's greater than intField index
    AAX_CFieldIndex intIndex = AAX_FIELD_INDEX(TestContext, intField);
    EXPECT_GT(lastIndex, intIndex);
}

//--------------------------------------------------------------------
#pragma mark - Audio Port Type Tests

TEST_F(AAX_Header, AudioPortTypes)
{
    // Test audio port type definitions
    static_assert(std::is_same<const float * const *, AAX_CAudioInPort>::value, 
                  "AAX_CAudioInPort should be const float * const *");
    static_assert(std::is_same<float * const *, AAX_CAudioOutPort>::value, 
                  "AAX_CAudioOutPort should be float * const *");
    static_assert(std::is_same<float * const, AAX_CMeterPort>::value, 
                  "AAX_CMeterPort should be float * const");
}

//--------------------------------------------------------------------
#pragma mark - MIDI Structure Tests

TEST_F(AAX_Header, MidiPacketStructure)
{
    // Test AAX_CMidiPacket structure
    AAX_CMidiPacket packet;
    packet.mTimestamp = 12345;
    packet.mData[0] = 0x90; // Note on
    packet.mData[1] = 0x40; // Note number
    packet.mData[2] = 0x7F; // Velocity
    packet.mData[3] = 0x00; // Unused
    packet.mLength = 3;
    packet.mIsImmediate = 1;
    
    EXPECT_EQ(12345, packet.mTimestamp);
    EXPECT_EQ(0x90, packet.mData[0]);
    EXPECT_EQ(0x40, packet.mData[1]);
    EXPECT_EQ(0x7F, packet.mData[2]);
    EXPECT_EQ(3, packet.mLength);
    EXPECT_EQ(1, packet.mIsImmediate);
}

TEST_F(AAX_Header, MidiStreamStructure)
{
    // Test AAX_CMidiStream structure
    AAX_CMidiPacket packets[2];
    AAX_CMidiStream stream;
    stream.mBufferSize = 2;
    stream.mBuffer = packets;
    
    EXPECT_EQ(2U, stream.mBufferSize);
    EXPECT_EQ(packets, stream.mBuffer);
}

//--------------------------------------------------------------------
#pragma mark - Plugin Identifier Tests

TEST_F(AAX_Header, PluginIdentifierTriad)
{
    // Test AAX_SPlugInIdentifierTriad structure
    AAX_SPlugInIdentifierTriad triad;
    triad.mManufacturerID = 'AVID';
    triad.mProductID = 'TEST';
    triad.mPlugInID = 'PLG1';

    EXPECT_EQ('AVID', triad.mManufacturerID);
    EXPECT_EQ('TEST', triad.mProductID);
    EXPECT_EQ('PLG1', triad.mPlugInID);
}

//--------------------------------------------------------------------
#pragma mark - Plugin Identifier Member Function Tests

TEST_F(AAX_Header, PluginIdentifierTriadIsUndefined)
{
    // Test IsUndefined() member function
    AAX_SPlugInIdentifierTriad undefinedTriad;
    undefinedTriad.mManufacturerID = kAAX_TypeID_Undefined;
    undefinedTriad.mProductID = kAAX_TypeID_Undefined;
    undefinedTriad.mPlugInID = kAAX_TypeID_Undefined;

    EXPECT_TRUE(undefinedTriad.IsUndefined());

    // Test partially undefined (should return false)
    AAX_SPlugInIdentifierTriad partiallyUndefined;
    partiallyUndefined.mManufacturerID = 'AVID';
    partiallyUndefined.mProductID = kAAX_TypeID_Undefined;
    partiallyUndefined.mPlugInID = kAAX_TypeID_Undefined;

    EXPECT_FALSE(partiallyUndefined.IsUndefined());

    // Test fully defined (should return false)
    AAX_SPlugInIdentifierTriad definedTriad;
    definedTriad.mManufacturerID = 'AVID';
    definedTriad.mProductID = 'TEST';
    definedTriad.mPlugInID = 'PLG1';

    EXPECT_FALSE(definedTriad.IsUndefined());
}

TEST_F(AAX_Header, PluginIdentifierTriadIsSameProduct)
{
    // Test IsSameProduct() member function
    AAX_SPlugInIdentifierTriad triad1;
    triad1.mManufacturerID = 'AVID';
    triad1.mProductID = 'TEST';
    triad1.mPlugInID = 'PLG1';

    AAX_SPlugInIdentifierTriad triad2;
    triad2.mManufacturerID = 'AVID';
    triad2.mProductID = 'TEST';
    triad2.mPlugInID = 'PLG2'; // Different plugin ID

    // Same manufacturer and product should return true
    EXPECT_TRUE(triad1.IsSameProduct(triad2));
    EXPECT_TRUE(triad2.IsSameProduct(triad1));

    // Different manufacturer should return false
    AAX_SPlugInIdentifierTriad triad3;
    triad3.mManufacturerID = 'OTHR';
    triad3.mProductID = 'TEST';
    triad3.mPlugInID = 'PLG1';

    EXPECT_FALSE(triad1.IsSameProduct(triad3));
    EXPECT_FALSE(triad3.IsSameProduct(triad1));

    // Different product should return false
    AAX_SPlugInIdentifierTriad triad4;
    triad4.mManufacturerID = 'AVID';
    triad4.mProductID = 'DIFF';
    triad4.mPlugInID = 'PLG1';

    EXPECT_FALSE(triad1.IsSameProduct(triad4));
    EXPECT_FALSE(triad4.IsSameProduct(triad1));

    // Same triad should return true
    EXPECT_TRUE(triad1.IsSameProduct(triad1));
}

//--------------------------------------------------------------------
#pragma mark - Plugin Identifier Comparison Operator Tests

TEST_F(AAX_Header, PluginIdentifierTriadEqualityOperator)
{
    // Test operator== and operator!=
    AAX_SPlugInIdentifierTriad triad1;
    triad1.mManufacturerID = 'AVID';
    triad1.mProductID = 'TEST';
    triad1.mPlugInID = 'PLG1';

    AAX_SPlugInIdentifierTriad triad2;
    triad2.mManufacturerID = 'AVID';
    triad2.mProductID = 'TEST';
    triad2.mPlugInID = 'PLG1';

    // Identical triads should be equal
    EXPECT_TRUE(triad1 == triad2);
    EXPECT_FALSE(triad1 != triad2);

    // Same triad should be equal to itself
    EXPECT_TRUE(triad1 == triad1);
    EXPECT_FALSE(triad1 != triad1);

    // Different plugin ID should not be equal
    AAX_SPlugInIdentifierTriad triad3;
    triad3.mManufacturerID = 'AVID';
    triad3.mProductID = 'TEST';
    triad3.mPlugInID = 'PLG2';

    EXPECT_FALSE(triad1 == triad3);
    EXPECT_TRUE(triad1 != triad3);

    // Different product ID should not be equal
    AAX_SPlugInIdentifierTriad triad4;
    triad4.mManufacturerID = 'AVID';
    triad4.mProductID = 'DIFF';
    triad4.mPlugInID = 'PLG1';

    EXPECT_FALSE(triad1 == triad4);
    EXPECT_TRUE(triad1 != triad4);

    // Different manufacturer ID should not be equal
    AAX_SPlugInIdentifierTriad triad5;
    triad5.mManufacturerID = 'OTHR';
    triad5.mProductID = 'TEST';
    triad5.mPlugInID = 'PLG1';

    EXPECT_FALSE(triad1 == triad5);
    EXPECT_TRUE(triad1 != triad5);
}

TEST_F(AAX_Header, PluginIdentifierTriadLessThanOperator)
{
    // Test operator< with manufacturer ID differences
    AAX_SPlugInIdentifierTriad triad1;
    triad1.mManufacturerID = 'AVID';
    triad1.mProductID = 'TEST';
    triad1.mPlugInID = 'PLG1';

    AAX_SPlugInIdentifierTriad triad2;
    triad2.mManufacturerID = 'ZVID'; // Greater manufacturer ID
    triad2.mProductID = 'TEST';
    triad2.mPlugInID = 'PLG1';

    EXPECT_TRUE(triad1 < triad2);
    EXPECT_FALSE(triad2 < triad1);

    // Test with product ID differences (same manufacturer)
    AAX_SPlugInIdentifierTriad triad3;
    triad3.mManufacturerID = 'AVID';
    triad3.mProductID = 'ZEST'; // Greater product ID
    triad3.mPlugInID = 'PLG1';

    EXPECT_TRUE(triad1 < triad3);
    EXPECT_FALSE(triad3 < triad1);

    // Test with plugin ID differences (same manufacturer and product)
    AAX_SPlugInIdentifierTriad triad4;
    triad4.mManufacturerID = 'AVID';
    triad4.mProductID = 'TEST';
    triad4.mPlugInID = 'ZLG1'; // Greater plugin ID

    EXPECT_TRUE(triad1 < triad4);
    EXPECT_FALSE(triad4 < triad1);

    // Test identical triads (should not be less than each other)
    AAX_SPlugInIdentifierTriad triad5;
    triad5.mManufacturerID = 'AVID';
    triad5.mProductID = 'TEST';
    triad5.mPlugInID = 'PLG1';

    EXPECT_FALSE(triad1 < triad5);
    EXPECT_FALSE(triad5 < triad1);
}

TEST_F(AAX_Header, PluginIdentifierTriadGreaterThanOperator)
{
    // Test operator>
    AAX_SPlugInIdentifierTriad triad1;
    triad1.mManufacturerID = 'ZVID';
    triad1.mProductID = 'TEST';
    triad1.mPlugInID = 'PLG1';

    AAX_SPlugInIdentifierTriad triad2;
    triad2.mManufacturerID = 'AVID'; // Lesser manufacturer ID
    triad2.mProductID = 'TEST';
    triad2.mPlugInID = 'PLG1';

    EXPECT_TRUE(triad1 > triad2);
    EXPECT_FALSE(triad2 > triad1);

    // Test identical triads (should not be greater than each other)
    AAX_SPlugInIdentifierTriad triad3;
    triad3.mManufacturerID = 'ZVID';
    triad3.mProductID = 'TEST';
    triad3.mPlugInID = 'PLG1';

    EXPECT_FALSE(triad1 > triad3);
    EXPECT_FALSE(triad3 > triad1);
}

TEST_F(AAX_Header, PluginIdentifierTriadGreaterEqualOperator)
{
    // Test operator>=
    AAX_SPlugInIdentifierTriad triad1;
    triad1.mManufacturerID = 'ZVID';
    triad1.mProductID = 'TEST';
    triad1.mPlugInID = 'PLG1';

    AAX_SPlugInIdentifierTriad triad2;
    triad2.mManufacturerID = 'AVID'; // Lesser manufacturer ID
    triad2.mProductID = 'TEST';
    triad2.mPlugInID = 'PLG1';

    EXPECT_TRUE(triad1 >= triad2);
    EXPECT_FALSE(triad2 >= triad1);

    // Test identical triads (should be greater than or equal)
    AAX_SPlugInIdentifierTriad triad3;
    triad3.mManufacturerID = 'ZVID';
    triad3.mProductID = 'TEST';
    triad3.mPlugInID = 'PLG1';

    EXPECT_TRUE(triad1 >= triad3);
    EXPECT_TRUE(triad3 >= triad1);
}

TEST_F(AAX_Header, PluginIdentifierTriadLessEqualOperator)
{
    // Test operator<=
    AAX_SPlugInIdentifierTriad triad1;
    triad1.mManufacturerID = 'AVID';
    triad1.mProductID = 'TEST';
    triad1.mPlugInID = 'PLG1';

    AAX_SPlugInIdentifierTriad triad2;
    triad2.mManufacturerID = 'ZVID'; // Greater manufacturer ID
    triad2.mProductID = 'TEST';
    triad2.mPlugInID = 'PLG1';

    EXPECT_TRUE(triad1 <= triad2);
    EXPECT_FALSE(triad2 <= triad1);

    // Test identical triads (should be less than or equal)
    AAX_SPlugInIdentifierTriad triad3;
    triad3.mManufacturerID = 'AVID';
    triad3.mProductID = 'TEST';
    triad3.mPlugInID = 'PLG1';

    EXPECT_TRUE(triad1 <= triad3);
    EXPECT_TRUE(triad3 <= triad1);
}

//--------------------------------------------------------------------
#pragma mark - Plugin Identifier Hash Function Tests

TEST_F(AAX_Header, PluginIdentifierTriadHash)
{
    // Test AAX_SPlugInIdentifierHash hash functor
    AAX_SPlugInIdentifierHash hasher;

    AAX_SPlugInIdentifierTriad triad1;
    triad1.mManufacturerID = 'AVID';
    triad1.mProductID = 'TEST';
    triad1.mPlugInID = 'PLG1';

    AAX_SPlugInIdentifierTriad triad2;
    triad2.mManufacturerID = 'AVID';
    triad2.mProductID = 'TEST';
    triad2.mPlugInID = 'PLG1';

    // Identical triads should produce identical hashes
    size_t hash1 = hasher(triad1);
    size_t hash2 = hasher(triad2);
    EXPECT_EQ(hash1, hash2);

    // Different triads should produce different hashes (usually)
    AAX_SPlugInIdentifierTriad triad3;
    triad3.mManufacturerID = 'AVID';
    triad3.mProductID = 'TEST';
    triad3.mPlugInID = 'PLG2'; // Different plugin ID

    size_t hash3 = hasher(triad3);
    EXPECT_NE(hash1, hash3); // Different plugin ID should produce different hash

    // Test with completely different triad
    AAX_SPlugInIdentifierTriad triad4;
    triad4.mManufacturerID = 'OTHR';
    triad4.mProductID = 'DIFF';
    triad4.mPlugInID = 'PLG3';

    size_t hash4 = hasher(triad4);
    EXPECT_NE(hash1, hash4);
}

TEST_F(AAX_Header, PluginIdentifierTriadProductHash)
{
    // Test AAX_SPlugInIdentifierTriadProductHash hash functor
    AAX_SPlugInIdentifierTriadProductHash hasher;

    AAX_SPlugInIdentifierTriad triad1;
    triad1.mManufacturerID = 'AVID';
    triad1.mProductID = 'TEST';
    triad1.mPlugInID = 'PLG1';

    AAX_SPlugInIdentifierTriad triad2;
    triad2.mManufacturerID = 'AVID';
    triad2.mProductID = 'TEST';
    triad2.mPlugInID = 'PLG2'; // Different plugin ID

    // Test that hash functor works
    size_t hash1 = hasher(triad1);
    size_t hash2 = hasher(triad2);

    // Same manufacturer and product should produce same hash (ignores plugin ID)
    EXPECT_EQ(hash1, hash2);

    // Different manufacturer should produce different hash
    AAX_SPlugInIdentifierTriad triad3;
    triad3.mManufacturerID = 'OTHR';
    triad3.mProductID = 'TEST';
    triad3.mPlugInID = 'PLG1';

    size_t hash3 = hasher(triad3);
    EXPECT_NE(hash1, hash3);

    // Different product should produce different hash
    AAX_SPlugInIdentifierTriad triad4;
    triad4.mManufacturerID = 'AVID';
    triad4.mProductID = 'DIFF';
    triad4.mPlugInID = 'PLG1';

    size_t hash4 = hasher(triad4);
    EXPECT_NE(hash1, hash4);
}

TEST_F(AAX_Header, PluginIdentifierTriadProductEqual)
{
    // Test AAX_SPlugInIdentifierTriadProductEqual functor
    AAX_SPlugInIdentifierTriadProductEqual equalizer;

    AAX_SPlugInIdentifierTriad triad1;
    triad1.mManufacturerID = 'AVID';
    triad1.mProductID = 'TEST';
    triad1.mPlugInID = 'PLG1';

    AAX_SPlugInIdentifierTriad triad2;
    triad2.mManufacturerID = 'AVID';
    triad2.mProductID = 'TEST';
    triad2.mPlugInID = 'PLG2'; // Different plugin ID

    // Same manufacturer and product should be equal (ignores plugin ID)
    EXPECT_TRUE(equalizer(triad1, triad2));
    EXPECT_TRUE(equalizer(triad2, triad1));

    // Different manufacturer should not be equal
    AAX_SPlugInIdentifierTriad triad3;
    triad3.mManufacturerID = 'OTHR';
    triad3.mProductID = 'TEST';
    triad3.mPlugInID = 'PLG1';

    EXPECT_FALSE(equalizer(triad1, triad3));
    EXPECT_FALSE(equalizer(triad3, triad1));

    // Different product should not be equal
    AAX_SPlugInIdentifierTriad triad4;
    triad4.mManufacturerID = 'AVID';
    triad4.mProductID = 'DIFF';
    triad4.mPlugInID = 'PLG1';

    EXPECT_FALSE(equalizer(triad1, triad4));
    EXPECT_FALSE(equalizer(triad4, triad1));

    // Same triad should be equal to itself
    EXPECT_TRUE(equalizer(triad1, triad1));
}

//--------------------------------------------------------------------
#pragma mark - Plugin Identifier Edge Case Tests

TEST_F(AAX_Header, PluginIdentifierTriadZeroValues)
{
    // Test with zero values
    AAX_SPlugInIdentifierTriad zeroTriad;
    zeroTriad.mManufacturerID = 0;
    zeroTriad.mProductID = 0;
    zeroTriad.mPlugInID = 0;

    // Zero values should be considered undefined
    EXPECT_TRUE(zeroTriad.IsUndefined());

    // Test comparison with zero values
    AAX_SPlugInIdentifierTriad nonZeroTriad;
    nonZeroTriad.mManufacturerID = 'AVID';
    nonZeroTriad.mProductID = 'TEST';
    nonZeroTriad.mPlugInID = 'PLG1';

    EXPECT_FALSE(zeroTriad == nonZeroTriad);
    EXPECT_TRUE(zeroTriad != nonZeroTriad);
    EXPECT_TRUE(zeroTriad < nonZeroTriad);
    EXPECT_FALSE(zeroTriad > nonZeroTriad);
}

TEST_F(AAX_Header, PluginIdentifierTriadMaxValues)
{
    // Test with maximum uint32_t values
    AAX_SPlugInIdentifierTriad maxTriad;
    maxTriad.mManufacturerID = std::numeric_limits<uint32_t>::max();
    maxTriad.mProductID = std::numeric_limits<uint32_t>::max();
    maxTriad.mPlugInID = std::numeric_limits<uint32_t>::max();

    // Max values should not be undefined
    EXPECT_FALSE(maxTriad.IsUndefined());

    // Test comparison with max values
    AAX_SPlugInIdentifierTriad normalTriad;
    normalTriad.mManufacturerID = 'AVID';
    normalTriad.mProductID = 'TEST';
    normalTriad.mPlugInID = 'PLG1';

    EXPECT_FALSE(maxTriad == normalTriad);
    EXPECT_TRUE(maxTriad != normalTriad);
    EXPECT_FALSE(maxTriad < normalTriad);
    EXPECT_TRUE(maxTriad > normalTriad);
}

TEST_F(AAX_Header, PluginIdentifierTriadMixedUndefinedValues)
{
    // Test various combinations of undefined values
    AAX_SPlugInIdentifierTriad triad1;
    triad1.mManufacturerID = kAAX_TypeID_Undefined;
    triad1.mProductID = 'TEST';
    triad1.mPlugInID = 'PLG1';
    EXPECT_FALSE(triad1.IsUndefined());

    AAX_SPlugInIdentifierTriad triad2;
    triad2.mManufacturerID = 'AVID';
    triad2.mProductID = kAAX_TypeID_Undefined;
    triad2.mPlugInID = 'PLG1';
    EXPECT_FALSE(triad2.IsUndefined());

    AAX_SPlugInIdentifierTriad triad3;
    triad3.mManufacturerID = 'AVID';
    triad3.mProductID = 'TEST';
    triad3.mPlugInID = kAAX_TypeID_Undefined;
    EXPECT_FALSE(triad3.IsUndefined());

    AAX_SPlugInIdentifierTriad triad4;
    triad4.mManufacturerID = kAAX_TypeID_Undefined;
    triad4.mProductID = kAAX_TypeID_Undefined;
    triad4.mPlugInID = 'PLG1';
    EXPECT_FALSE(triad4.IsUndefined());
}

TEST_F(AAX_Header, PluginIdentifierTriadSameProductEdgeCases)
{
    // Test IsSameProduct with edge cases
    AAX_SPlugInIdentifierTriad triad1;
    triad1.mManufacturerID = kAAX_TypeID_Undefined;
    triad1.mProductID = kAAX_TypeID_Undefined;
    triad1.mPlugInID = 'PLG1';

    AAX_SPlugInIdentifierTriad triad2;
    triad2.mManufacturerID = kAAX_TypeID_Undefined;
    triad2.mProductID = kAAX_TypeID_Undefined;
    triad2.mPlugInID = 'PLG2';

    // Same undefined manufacturer and product should be considered same product
    EXPECT_TRUE(triad1.IsSameProduct(triad2));

    // Test with max values
    AAX_SPlugInIdentifierTriad triad3;
    triad3.mManufacturerID = std::numeric_limits<uint32_t>::max();
    triad3.mProductID = std::numeric_limits<uint32_t>::max();
    triad3.mPlugInID = 'PLG1';

    AAX_SPlugInIdentifierTriad triad4;
    triad4.mManufacturerID = std::numeric_limits<uint32_t>::max();
    triad4.mProductID = std::numeric_limits<uint32_t>::max();
    triad4.mPlugInID = 'PLG2';

    EXPECT_TRUE(triad3.IsSameProduct(triad4));
}

TEST_F(AAX_Header, PluginIdentifierTriadComparisonConsistency)
{
    // Test that comparison operators are consistent with each other
    AAX_SPlugInIdentifierTriad triad1;
    triad1.mManufacturerID = 'AVID';
    triad1.mProductID = 'TEST';
    triad1.mPlugInID = 'PLG1';

    AAX_SPlugInIdentifierTriad triad2;
    triad2.mManufacturerID = 'ZVID';
    triad2.mProductID = 'TEST';
    triad2.mPlugInID = 'PLG1';

    // If triad1 < triad2, then triad2 > triad1
    EXPECT_TRUE(triad1 < triad2);
    EXPECT_TRUE(triad2 > triad1);

    // If triad1 < triad2, then triad1 <= triad2
    EXPECT_TRUE(triad1 <= triad2);

    // If triad2 > triad1, then triad2 >= triad1
    EXPECT_TRUE(triad2 >= triad1);

    // If triad1 < triad2, then !(triad1 >= triad2)
    EXPECT_FALSE(triad1 >= triad2);

    // If triad2 > triad1, then !(triad2 <= triad1)
    EXPECT_FALSE(triad2 <= triad1);

    // If triad1 != triad2, then !(triad1 == triad2)
    EXPECT_TRUE(triad1 != triad2);
    EXPECT_FALSE(triad1 == triad2);
}

TEST_F(AAX_Header, PluginIdentifierTriadHashConsistency)
{
    // Test that hash functions are consistent
    AAX_SPlugInIdentifierHash fullHasher;
    AAX_SPlugInIdentifierTriadProductHash productHasher;

    AAX_SPlugInIdentifierTriad triad;
    triad.mManufacturerID = 'AVID';
    triad.mProductID = 'TEST';
    triad.mPlugInID = 'PLG1';

    // Hash should be consistent across multiple calls
    size_t fullHash1 = fullHasher(triad);
    size_t fullHash2 = fullHasher(triad);
    EXPECT_EQ(fullHash1, fullHash2);

    size_t productHash1 = productHasher(triad);
    size_t productHash2 = productHasher(triad);
    EXPECT_EQ(productHash1, productHash2);

    // Test with zero values
    AAX_SPlugInIdentifierTriad zeroTriad;
    zeroTriad.mManufacturerID = 0;
    zeroTriad.mProductID = 0;
    zeroTriad.mPlugInID = 0;

    size_t zeroFullHash = fullHasher(zeroTriad);
    size_t zeroProductHash = productHasher(zeroTriad);

    // Zero hash should be different from non-zero hash
    EXPECT_NE(fullHash1, zeroFullHash);
    EXPECT_NE(productHash1, zeroProductHash);
}

TEST_F(AAX_Header, PluginIdentifierTriadOrderingTransitivity)
{
    // Test transitivity: if a < b and b < c, then a < c
    AAX_SPlugInIdentifierTriad triadA;
    triadA.mManufacturerID = 'AVID';
    triadA.mProductID = 'TEST';
    triadA.mPlugInID = 'PLG1';

    AAX_SPlugInIdentifierTriad triadB;
    triadB.mManufacturerID = 'BVID';
    triadB.mProductID = 'TEST';
    triadB.mPlugInID = 'PLG1';

    AAX_SPlugInIdentifierTriad triadC;
    triadC.mManufacturerID = 'CVID';
    triadC.mProductID = 'TEST';
    triadC.mPlugInID = 'PLG1';

    // Verify ordering
    EXPECT_TRUE(triadA < triadB);
    EXPECT_TRUE(triadB < triadC);
    EXPECT_TRUE(triadA < triadC); // Transitivity

    // Test with product ID ordering
    AAX_SPlugInIdentifierTriad triadD;
    triadD.mManufacturerID = 'AVID';
    triadD.mProductID = 'AEST';
    triadD.mPlugInID = 'PLG1';

    AAX_SPlugInIdentifierTriad triadE;
    triadE.mManufacturerID = 'AVID';
    triadE.mProductID = 'BEST';
    triadE.mPlugInID = 'PLG1';

    AAX_SPlugInIdentifierTriad triadF;
    triadF.mManufacturerID = 'AVID';
    triadF.mProductID = 'CEST';
    triadF.mPlugInID = 'PLG1';

    EXPECT_TRUE(triadD < triadE);
    EXPECT_TRUE(triadE < triadF);
    EXPECT_TRUE(triadD < triadF); // Transitivity
}

TEST_F(AAX_Header, PluginIdentifierTriadComplexComparisons)
{
    // Test complex comparison scenarios
    std::vector<AAX_SPlugInIdentifierTriad> triads;

    // Create various triads for sorting
    AAX_SPlugInIdentifierTriad triad1;
    triad1.mManufacturerID = 'AVID';
    triad1.mProductID = 'TEST';
    triad1.mPlugInID = 'PLG1';
    triads.push_back(triad1);

    AAX_SPlugInIdentifierTriad triad2;
    triad2.mManufacturerID = 'AVID';
    triad2.mProductID = 'TEST';
    triad2.mPlugInID = 'PLG2';
    triads.push_back(triad2);

    AAX_SPlugInIdentifierTriad triad3;
    triad3.mManufacturerID = 'AVID';
    triad3.mProductID = 'ZEST';
    triad3.mPlugInID = 'PLG1';
    triads.push_back(triad3);

    AAX_SPlugInIdentifierTriad triad4;
    triad4.mManufacturerID = 'ZVID';
    triad4.mProductID = 'AEST';
    triad4.mPlugInID = 'PLG1';
    triads.push_back(triad4);

    // Sort the triads
    std::sort(triads.begin(), triads.end());

    // Verify sorted order
    EXPECT_TRUE(triads[0] == triad1); // AVID, TEST, PLG1
    EXPECT_TRUE(triads[1] == triad2); // AVID, TEST, PLG2
    EXPECT_TRUE(triads[2] == triad3); // AVID, ZEST, PLG1
    EXPECT_TRUE(triads[3] == triad4); // ZVID, AEST, PLG1

    // Verify all are in ascending order
    for (size_t i = 0; i < triads.size() - 1; ++i) {
        EXPECT_TRUE(triads[i] < triads[i + 1] || triads[i] == triads[i + 1]);
    }
}

//--------------------------------------------------------------------
#pragma mark - Boolean Type Tests

TEST_F(AAX_Header, BooleanTypeValues)
{
    // Test AAX_CBoolean values
    AAX_CBoolean falseValue = 0;
    AAX_CBoolean trueValue = 1;
    AAX_CBoolean nonZeroValue = 255;
    
    EXPECT_EQ(0, falseValue);
    EXPECT_EQ(1, trueValue);
    EXPECT_EQ(255, nonZeroValue);
    
    // Test that AAX_CBoolean can hold 0-255
    EXPECT_LE(falseValue, std::numeric_limits<AAX_CBoolean>::max());
    EXPECT_LE(trueValue, std::numeric_limits<AAX_CBoolean>::max());
    EXPECT_LE(nonZeroValue, std::numeric_limits<AAX_CBoolean>::max());
}

//--------------------------------------------------------------------
#pragma mark - Timestamp and ID Tests

TEST_F(AAX_Header, TimestampValues)
{
    // Test timestamp values
    AAX_CTimestamp currentTime = 1000000;
    AAX_CTimestamp futureTime = 2000000;
    AAX_CTimestamp neverTime = kAAX_Never;

    EXPECT_LT(currentTime, futureTime);
    // kAAX_Never is the maximum value, but when cast to signed it becomes -1
    // So we test that it's the maximum unsigned value when cast properly
    EXPECT_EQ(static_cast<AAX_CTimestamp>(~0ULL), neverTime);
    EXPECT_EQ(static_cast<uint64_t>(~0ULL), static_cast<uint64_t>(neverTime));
}

TEST_F(AAX_Header, TaskIDValues)
{
    // Test task ID values
    AAX_CTaskID validTaskID = 12345;
    AAX_CTaskID undefinedTaskID = kAAX_TaskID_Undefined;
    
    EXPECT_NE(validTaskID, undefinedTaskID);
    EXPECT_EQ(0ULL, undefinedTaskID);
    EXPECT_GT(validTaskID, undefinedTaskID);
}

TEST_F(AAX_Header, InstanceGroupIDValues)
{
    // Test instance group ID values
    AAX_CInstanceGroupID validGroupID = 100;
    AAX_CInstanceGroupID undefinedGroupID = kAAX_InstanceGroupID_Undefined;
    
    EXPECT_NE(validGroupID, undefinedGroupID);
    EXPECT_EQ(0U, undefinedGroupID);
    EXPECT_GT(validGroupID, undefinedGroupID);
}

//--------------------------------------------------------------------
#pragma mark - Property Value Tests

TEST_F(AAX_Header, PropertyValues)
{
    // Test 32-bit property values
    AAX_CPropertyValue prop32 = 42;
    EXPECT_EQ(42, prop32);
    static_assert(sizeof(prop32) == sizeof(int32_t), "AAX_CPropertyValue must be int32_t sized");
    
    // Test 64-bit property values
    AAX_CPropertyValue64 prop64 = 0x123456789ABCDEFLL;
    EXPECT_EQ(0x123456789ABCDEFLL, prop64);
    static_assert(sizeof(prop64) == sizeof(int64_t), "AAX_CPropertyValue64 must be int64_t sized");
}

//--------------------------------------------------------------------
#pragma mark - Type ID Tests

TEST_F(AAX_Header, TypeIDValues)
{
    // Test type ID values
    AAX_CTypeID validTypeID = 'TEST';
    AAX_CTypeID undefinedTypeID = kAAX_TypeID_Undefined;
    
    EXPECT_NE(validTypeID, undefinedTypeID);
    EXPECT_EQ(0U, undefinedTypeID);
    EXPECT_GT(validTypeID, undefinedTypeID);
    
    // Test that type IDs can hold 4-character codes
    AAX_CTypeID fourCharCode = 'ABCD';
    EXPECT_EQ('ABCD', fourCharCode);
}

//--------------------------------------------------------------------
#pragma mark - Parameter ID Tests

TEST_F(AAX_Header, ParameterIDConstraints)
{
    // Test parameter ID constraints
    const char* validParamID = "test.parameter";
    const char* maxLengthParamID = "this.is.exactly.thirty.one.char"; // 31 characters
    const char* tooLongParamID = "this.parameter.id.is.too.long.for.aax"; // > 31 characters
    
    AAX_CParamID param1 = validParamID;
    AAX_CParamID param2 = maxLengthParamID;
    AAX_CParamID param3 = tooLongParamID;
    
    EXPECT_STREQ(validParamID, param1);
    EXPECT_STREQ(maxLengthParamID, param2);
    EXPECT_STREQ(tooLongParamID, param3);
    
    // Test length constraints
    EXPECT_LT(strlen(validParamID), kAAX_ParameterIdentifierMaxSize);
    EXPECT_EQ(strlen(maxLengthParamID), kAAX_ParameterIdentifierMaxSize - 1); // -1 for null terminator
    EXPECT_GT(strlen(tooLongParamID), kAAX_ParameterIdentifierMaxSize - 1);
}

//--------------------------------------------------------------------
#pragma mark - C++ Keyword Macro Tests

TEST_F(AAX_Header, CPPKeywordMacros)
{
    // Test that C++ keyword macros are defined
    // Note: We can't easily test the actual macro expansion in unit tests,
    // but we can verify they compile and are defined

    // Test class using AAX macros
    class BaseTestClass {
    public:
        virtual ~BaseTestClass() {}
        virtual void virtualMethod() {}
        virtual void finalMethod() {}
    };

    class TestClass : public BaseTestClass {
    public:
        AAX_DEFAULT_CTOR(TestClass);
        virtual ~TestClass() {}

        virtual void virtualMethod() AAX_OVERRIDE {}
        virtual void finalMethod() AAX_FINAL AAX_OVERRIDE {}
    };

    // If we get here, the macros compiled successfully
    EXPECT_TRUE(true);
}

TEST_F(AAX_Header, ConstexprMacro)
{
    // Test AAX_CONSTEXPR macro
    AAX_CONSTEXPR int testConstexpr = 42;
    EXPECT_EQ(42, testConstexpr);

    // Test with existing AAX constants
    AAX_CONSTEXPR AAX_CTypeID testTypeID = kAAX_TypeID_Undefined;
    EXPECT_EQ(0U, testTypeID);
}

//--------------------------------------------------------------------
#pragma mark - Alignment Macro Tests

TEST_F(AAX_Header, AlignmentMacros)
{
    // Test that alignment macros are defined as strings
    EXPECT_NE(nullptr, AAX_ALIGN_FILE_HOST);
    EXPECT_NE(nullptr, AAX_ALIGN_FILE_ALG);
    EXPECT_NE(nullptr, AAX_ALIGN_FILE_RESET);
    EXPECT_NE(nullptr, AAX_ALIGN_FILE_BEGIN);
    EXPECT_NE(nullptr, AAX_ALIGN_FILE_END);

    // Test that they contain expected file names
    EXPECT_NE(nullptr, strstr(AAX_ALIGN_FILE_RESET, "Pop"));
    EXPECT_NE(nullptr, strstr(AAX_ALIGN_FILE_BEGIN, "Pre"));
    EXPECT_NE(nullptr, strstr(AAX_ALIGN_FILE_END, "Post"));
}

//--------------------------------------------------------------------
#pragma mark - Callback Macro Tests

TEST_F(AAX_Header, CallbackMacro)
{
    // Test that AAX_CALLBACK is defined
    // We can't easily test the actual calling convention, but we can
    // verify it compiles in a function declaration

    typedef void (AAX_CALLBACK *TestCallbackType)(int);
    TestCallbackType callback = nullptr;

    EXPECT_EQ(nullptr, callback);
}

//--------------------------------------------------------------------
#pragma mark - Aligned Macro Tests

TEST_F(AAX_Header, AlignedMacro)
{
    // Test AAX_ALIGNED macro
    struct AAX_ALIGNED(8) AlignedStruct {
        char data[16];
    };

    AlignedStruct alignedData;

    // Test that the struct can be instantiated
    alignedData.data[0] = 'A';
    EXPECT_EQ('A', alignedData.data[0]);

    // Test alignment (this is platform-specific, so we just check it compiles)
    static_assert(sizeof(alignedData.data) == 16, "Aligned data array must be 16 bytes");
}

//--------------------------------------------------------------------
#pragma mark - MIDI Structure Boundary Tests

TEST_F(AAX_Header, MidiPacketBoundaryValues)
{
    // Test MIDI packet with boundary values
    AAX_CMidiPacket packet;

    // Test maximum timestamp (mTimestamp is uint32_t, not AAX_CTimestamp)
    packet.mTimestamp = std::numeric_limits<uint32_t>::max();
    EXPECT_EQ(std::numeric_limits<uint32_t>::max(), packet.mTimestamp);

    // Test maximum length (mLength is uint32_t, not uint8_t)
    packet.mLength = std::numeric_limits<uint32_t>::max();
    EXPECT_EQ(std::numeric_limits<uint32_t>::max(), packet.mLength);

    // Test boolean values
    packet.mIsImmediate = 0;
    EXPECT_EQ(0, packet.mIsImmediate);
    packet.mIsImmediate = 1;
    EXPECT_EQ(1, packet.mIsImmediate);
    packet.mIsImmediate = 255;
    EXPECT_EQ(255, packet.mIsImmediate);
}

TEST_F(AAX_Header, MidiStreamBoundaryValues)
{
    // Test MIDI stream with boundary values
    AAX_CMidiStream stream;

    // Test zero buffer size
    stream.mBufferSize = 0;
    stream.mBuffer = nullptr;
    EXPECT_EQ(0U, stream.mBufferSize);
    EXPECT_EQ(nullptr, stream.mBuffer);

    // Test maximum buffer size
    stream.mBufferSize = std::numeric_limits<uint32_t>::max();
    EXPECT_EQ(std::numeric_limits<uint32_t>::max(), stream.mBufferSize);
}

//--------------------------------------------------------------------
#pragma mark - Plugin Identifier Boundary Tests

TEST_F(AAX_Header, PluginIdentifierBoundaryValues)
{
    // Test plugin identifier with boundary values
    AAX_SPlugInIdentifierTriad triad;

    // Test zero values
    triad.mManufacturerID = 0;
    triad.mProductID = 0;
    triad.mPlugInID = 0;
    EXPECT_EQ(0U, triad.mManufacturerID);
    EXPECT_EQ(0U, triad.mProductID);
    EXPECT_EQ(0U, triad.mPlugInID);

    // Test maximum values
    triad.mManufacturerID = std::numeric_limits<uint32_t>::max();
    triad.mProductID = std::numeric_limits<uint32_t>::max();
    triad.mPlugInID = std::numeric_limits<uint32_t>::max();
    EXPECT_EQ(std::numeric_limits<uint32_t>::max(), triad.mManufacturerID);
    EXPECT_EQ(std::numeric_limits<uint32_t>::max(), triad.mProductID);
    EXPECT_EQ(std::numeric_limits<uint32_t>::max(), triad.mPlugInID);
}

//--------------------------------------------------------------------
#pragma mark - Integration Tests

TEST_F(AAX_Header, FieldIndexWithRealStructure)
{
    // Test AAX_FIELD_INDEX with a more realistic structure
    struct AudioContext {
        AAX_CAudioInPort inputPort;
        AAX_CAudioOutPort outputPort;
        AAX_CMeterPort meterPort;
        AAX_CPropertyValue sampleRate;
        AAX_CTimestamp timestamp;
    };

    AAX_CFieldIndex inputIndex = AAX_FIELD_INDEX(AudioContext, inputPort);
    AAX_CFieldIndex outputIndex = AAX_FIELD_INDEX(AudioContext, outputPort);
    AAX_CFieldIndex meterIndex = AAX_FIELD_INDEX(AudioContext, meterPort);
    AAX_CFieldIndex sampleRateIndex = AAX_FIELD_INDEX(AudioContext, sampleRate);
    AAX_CFieldIndex timestampIndex = AAX_FIELD_INDEX(AudioContext, timestamp);

    // Verify indices are sequential and make sense
    EXPECT_EQ(0, inputIndex);
    EXPECT_EQ(1, outputIndex);
    EXPECT_EQ(2, meterIndex);
    EXPECT_LT(meterIndex, sampleRateIndex);
    EXPECT_LT(sampleRateIndex, timestampIndex);
}

TEST_F(AAX_Header, TypeCompatibilityChecks)
{
    // Test that types are compatible as expected
    AAX_CIndex index = 42;
    AAX_CCount count = index; // Should be compatible
    EXPECT_EQ(index, count);

    AAX_CSelector selector = 'TEST';
    AAX_CComponentID componentID = selector; // Should be compatible
    AAX_CMeterID meterID = selector; // Should be compatible
    EXPECT_EQ(selector, componentID);
    EXPECT_EQ(selector, meterID);

    AAX_CPropertyValue prop32 = 100;
    AAX_Result result = prop32; // Should be compatible (both int32_t)
    EXPECT_EQ(prop32, result);
}
