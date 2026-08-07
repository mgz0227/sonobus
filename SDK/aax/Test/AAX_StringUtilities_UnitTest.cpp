//  Copyright 2025 Avid Technology, Inc.
//
// This file is part of the Avid AAX SDK.
//
// The AAX SDK is subject to commercial or open-source
// licensing.
//
// By using the AAX SDK, you agree to the terms of both the Avid AAX SDK License
// Agreement and Avid Privacy Policy.
//
// AAX SDK License: https://developer.avid.com/aax
// Privacy Policy: https://www.avid.com/legal/privacy-policy-statement
//
// Or: You may also use this code under the terms of the GPL v3 (see
// www.gnu.org/licenses).
//
// THE AAX SDK IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
// EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
// DISCLAIMED.


#include "gmock/gmock.h"
#include "AAX_StringUtilities.h"
#include "AAX_IString.h"
#include "AAX_CString.h"
#include "AAX.h"

// Use the testing namespace
using namespace testing;

//------------------------------------------------------
#pragma mark - AsStringFourChar

TEST(AAX_StringUtilities, AsStringFourChar) {
    // Test with standard four-character codes
    EXPECT_EQ("ABCD", AAX::AsStringFourChar(0x41424344)); // ASCII for "ABCD"
    EXPECT_EQ("1234", AAX::AsStringFourChar(0x31323334)); // ASCII for "1234"
    EXPECT_EQ("    ", AAX::AsStringFourChar(0x20202020)); // ASCII for spaces
    
    // Test with non-printable characters - intentionally replaced with "(?)".
    // This includes whitespace characters other than a single space character.
    EXPECT_EQ("(?)(?)(?) ", AAX::AsStringFourChar(0x090A0D20)); // Tab, newline, carriage return, space
    
    // Test with non-ASCII characters
    EXPECT_EQ("A(?)CD", AAX::AsStringFourChar(0x41004344)); // Contains a null byte
    EXPECT_EQ("(?)BCD", AAX::AsStringFourChar(0x00424344)); // Starts with a null byte
    
    // Test with non-printable ASCII
    EXPECT_EQ("(?)(?)(?)(?)", AAX::AsStringFourChar(0x01020304)); // Control characters
    
    // Test with special characters
    EXPECT_EQ("!@#$", AAX::AsStringFourChar(0x21402324)); // Special characters
    EXPECT_EQ("~`|\\", AAX::AsStringFourChar(0x7E607C5C)); // More special characters
    
    // Test with mixed ASCII and non-ASCII
    EXPECT_EQ("A(?)Z(?)", AAX::AsStringFourChar(0x41FF5AFF)); // Mix of ASCII and non-ASCII
    
    // Test with all non-printable
    EXPECT_EQ("(?)(?)(?)(?)", AAX::AsStringFourChar(0x00000000)); // All null bytes
    EXPECT_EQ("(?)(?)(?)(?)", AAX::AsStringFourChar(0x1F1F1F1F)); // All control characters
    
    // Test with boundary values
    EXPECT_EQ("~~~~", AAX::AsStringFourChar(0x7E7E7E7E)); // Highest printable ASCII
    EXPECT_EQ("(?)(?)(?)(?)", AAX::AsStringFourChar(0x7F7F7F7F)); // Lowest non-printable ASCII
}

//------------------------------------------------------
#pragma mark - IsFourCharASCII

TEST(AAX_StringUtilities, IsFourCharASCII) {
    // Test with ASCII characters
    EXPECT_TRUE(AAX::IsFourCharASCII(0x41424344)); // "ABCD"
    EXPECT_TRUE(AAX::IsFourCharASCII(0x31323334)); // "1234"
    EXPECT_TRUE(AAX::IsFourCharASCII(0x20202020)); // "    "
    
    // Test with non-ASCII characters
    EXPECT_FALSE(AAX::IsFourCharASCII(0x41004344)); // Contains a null byte
    EXPECT_FALSE(AAX::IsFourCharASCII(0x00424344)); // Starts with a null byte
    EXPECT_FALSE(AAX::IsFourCharASCII(0x41424380)); // Contains a character > 0x7F
    
    // Test with non-printable ASCII
    EXPECT_FALSE(AAX::IsFourCharASCII(0x01020304)); // Control characters
    
    // Test with special characters (still ASCII)
    EXPECT_TRUE(AAX::IsFourCharASCII(0x21402324)); // "!@#$"
    EXPECT_TRUE(AAX::IsFourCharASCII(0x7E607C5C)); // "~`|\"
    
    // Test with boundary values
    EXPECT_TRUE(AAX::IsFourCharASCII(0x20202020)); // Lowest printable ASCII
    EXPECT_TRUE(AAX::IsFourCharASCII(0x7E7E7E7E)); // Highest printable ASCII
    EXPECT_FALSE(AAX::IsFourCharASCII(0x1F1F1F1F)); // Just below lowest printable ASCII
    EXPECT_FALSE(AAX::IsFourCharASCII(0x7F7F7F7F)); // Just above highest printable ASCII
    
    // Test with mixed values
    EXPECT_FALSE(AAX::IsFourCharASCII(0x207E1F7F)); // Mix of printable and non-printable
    EXPECT_FALSE(AAX::IsFourCharASCII(0x41FF5AFF)); // Mix of ASCII and non-ASCII
}

//------------------------------------------------------
#pragma mark - IsASCII

TEST(AAX_StringUtilities, IsASCII) {
    // Test with ASCII characters
    EXPECT_TRUE(AAX::IsASCII('A'));
    EXPECT_TRUE(AAX::IsASCII('1'));
    EXPECT_TRUE(AAX::IsASCII(' '));
    
    // Test with non-ASCII characters
    EXPECT_FALSE(AAX::IsASCII('\0'));
    EXPECT_FALSE(AAX::IsASCII(static_cast<char>(0x80)));
    EXPECT_FALSE(AAX::IsASCII(static_cast<char>(0xFF)));
    
    // Test with non-printable ASCII
    EXPECT_FALSE(AAX::IsASCII('\x01'));
    EXPECT_FALSE(AAX::IsASCII('\x1F'));
    
    // Test with special characters (still ASCII)
    EXPECT_TRUE(AAX::IsASCII('!'));
    EXPECT_TRUE(AAX::IsASCII('@'));
    EXPECT_TRUE(AAX::IsASCII('#'));
    EXPECT_TRUE(AAX::IsASCII('$'));
    EXPECT_TRUE(AAX::IsASCII('~'));
    EXPECT_TRUE(AAX::IsASCII('`'));
    EXPECT_TRUE(AAX::IsASCII('|'));
    EXPECT_TRUE(AAX::IsASCII('\\'));
    
    // Test with boundary values
    EXPECT_TRUE(AAX::IsASCII(' ')); // Lowest printable ASCII (0x20)
    EXPECT_TRUE(AAX::IsASCII('~')); // Highest printable ASCII (0x7E)
    EXPECT_FALSE(AAX::IsASCII('\x1F')); // Just below lowest printable ASCII
    EXPECT_FALSE(AAX::IsASCII('\x7F')); // Just above highest printable ASCII
    
    // Test with signed char values
    EXPECT_FALSE(AAX::IsASCII(static_cast<char>(-1))); // -1 is 0xFF, non-ASCII
    EXPECT_FALSE(AAX::IsASCII(static_cast<char>(-128))); // -128 is 0x80, non-ASCII
}

//------------------------------------------------------
#pragma mark - Binary2String

TEST(AAX_StringUtilities, Binary2String) {
    // Test with various binary values
    EXPECT_EQ("00000000", AAX::Binary2String(0x00, 8));
    EXPECT_EQ("11111111", AAX::Binary2String(0xFF, 8));
    EXPECT_EQ("10101010", AAX::Binary2String(0xAA, 8));
    EXPECT_EQ("01010101", AAX::Binary2String(0x55, 8));
    
    // Test with fewer bits
    EXPECT_EQ("1010", AAX::Binary2String(0x0A, 4));
    EXPECT_EQ("1", AAX::Binary2String(0x01, 1));
    
    // Test with more bits
    EXPECT_EQ("0000000000001111", AAX::Binary2String(0x0F, 16));
    
    // Test with edge cases
    EXPECT_TRUE(AAX::Binary2String(0x00, 0).empty()); // Zero size
	EXPECT_TRUE(AAX::Binary2String(0x10, 0).empty()); // Zero size
	EXPECT_TRUE(AAX::Binary2String(0x00, -1).empty()); // Negative size
	EXPECT_TRUE(AAX::Binary2String(0x10, -1).empty()); // Negative size
    EXPECT_EQ("11111111111111111111111111111111", AAX::Binary2String(0xFFFFFFFF, 32)); // All 32 bits set
    EXPECT_EQ("00000000000000000000000000001111", AAX::Binary2String(0x0F, 33)); // More than 32 bits requested
    
    // Test with large values but fewer bits (truncation)
    EXPECT_EQ("1111", AAX::Binary2String(0xFF, 4)); // Only 4 bits shown from 0xFF
    
    // Test with negative values (treated as unsigned)
    EXPECT_EQ("11111111111111111111111111111111", AAX::Binary2String(-1, 32)); // -1 is all bits set
    EXPECT_EQ("10000000000000000000000000000000", AAX::Binary2String(INT32_MIN, 32)); // Most negative int32
}

//------------------------------------------------------
#pragma mark - String2Binary

TEST(AAX_StringUtilities, String2Binary) {
    // Create test strings
    AAX_CString binaryString1("00000000");
    AAX_CString binaryString2("11111111");
    AAX_CString binaryString3("10101010");
    AAX_CString binaryString4("01010101");
    AAX_CString binaryString5("1010");
    
    // Test conversion
    EXPECT_EQ(0x00, AAX::String2Binary(binaryString1));
    EXPECT_EQ(0xFF, AAX::String2Binary(binaryString2));
    EXPECT_EQ(0xAA, AAX::String2Binary(binaryString3));
    EXPECT_EQ(0x55, AAX::String2Binary(binaryString4));
    EXPECT_EQ(0x0A, AAX::String2Binary(binaryString5));
    
    // Test with non-binary characters - they should be ignored
    AAX_CString invalidString("1x0y1z0");
    EXPECT_EQ(10, AAX::String2Binary(invalidString)); // Should extract "1010" = 10
    
    // Test with edge cases
    AAX_CString emptyString("");
    EXPECT_EQ(0, AAX::String2Binary(emptyString)); // Empty string
    
    AAX_CString longString("1111111111111111111111111111111111111111"); // More than 32 bits
    EXPECT_EQ(0xFFFFFFFF, AAX::String2Binary(longString)); // Should truncate to 32 bits
    
    // Test with mixed valid/invalid characters
    AAX_CString mixedString1("101a");
    EXPECT_EQ(5, AAX::String2Binary(mixedString1)); // Should extract "101" = 5

    AAX_CString mixedString2("1 0 1 0");
    EXPECT_EQ(10, AAX::String2Binary(mixedString2)); // Should extract "1010" = 10

    // Test with special characters
    AAX_CString specialString("1!0@1#0$");
    EXPECT_EQ(10, AAX::String2Binary(specialString)); // Should extract "1010" = 10
}

//------------------------------------------------------
#pragma mark - AsStringPropertyValue

TEST(AAX_StringUtilities, AsStringPropertyValue) {
    // Test with sample rate property
    EXPECT_EQ("00000001", AAX::AsStringPropertyValue(AAX_eProperty_SampleRate, 1));
    EXPECT_EQ("00000010", AAX::AsStringPropertyValue(AAX_eProperty_SampleRate, 2));
    EXPECT_EQ("00000100", AAX::AsStringPropertyValue(AAX_eProperty_SampleRate, 4));
    EXPECT_EQ("00000000", AAX::AsStringPropertyValue(AAX_eProperty_SampleRate, 0));
    EXPECT_EQ("11111111", AAX::AsStringPropertyValue(AAX_eProperty_SampleRate, 0xFF));
    
    // Test with constraint location property
    EXPECT_EQ("00000001", AAX::AsStringPropertyValue(AAX_eProperty_Constraint_Location, 1));
    EXPECT_EQ("00000010", AAX::AsStringPropertyValue(AAX_eProperty_Constraint_Location, 2));
    EXPECT_EQ("10000000", AAX::AsStringPropertyValue(AAX_eProperty_Constraint_Location, 0x80));
    
    // Test with other properties
    EXPECT_EQ("1", AAX::AsStringPropertyValue(AAX_eProperty_TI_InstanceCycleCount, 1));
    EXPECT_EQ("42", AAX::AsStringPropertyValue(AAX_eProperty_TI_InstanceCycleCount, 42));
    EXPECT_EQ("-1", AAX::AsStringPropertyValue(AAX_eProperty_TI_InstanceCycleCount, -1));
    EXPECT_EQ("0", AAX::AsStringPropertyValue(AAX_eProperty_TI_InstanceCycleCount, 0));
    
    // Test with four-char values
    EXPECT_EQ("'ABCD'", AAX::AsStringPropertyValue(AAX_eProperty_ManufacturerID, 0x41424344)); // 'ABCD'
    EXPECT_EQ("'TEST'", AAX::AsStringPropertyValue(AAX_eProperty_ProductID, 0x54455354)); // 'TEST'
    
    // Test with hexadecimal output for large values
    EXPECT_EQ("0x7fffffff", AAX::AsStringPropertyValue(AAX_eProperty_TI_InstanceCycleCount, INT32_MAX));
    EXPECT_EQ("0x80000000", AAX::AsStringPropertyValue(AAX_eProperty_TI_InstanceCycleCount, INT32_MIN));
    EXPECT_EQ("0x1000000", AAX::AsStringPropertyValue(AAX_eProperty_TI_InstanceCycleCount, 0x01000000));
    EXPECT_EQ("0x12345678", AAX::AsStringPropertyValue(AAX_eProperty_TI_InstanceCycleCount, 0x12345678));
}

//------------------------------------------------------
#pragma mark - AsStringInt32

TEST(AAX_StringUtilities, AsStringInt32) {
    // Test with various int32 values
    EXPECT_EQ("0", AAX::AsStringInt32(0));
    EXPECT_EQ("1", AAX::AsStringInt32(1));
    EXPECT_EQ("-1", AAX::AsStringInt32(-1));
    EXPECT_EQ("2147483647", AAX::AsStringInt32(INT32_MAX));
    EXPECT_EQ("-2147483648", AAX::AsStringInt32(INT32_MIN));
    
    // Test with common values
    EXPECT_EQ("42", AAX::AsStringInt32(42));
    EXPECT_EQ("-42", AAX::AsStringInt32(-42));
    EXPECT_EQ("1000000", AAX::AsStringInt32(1000000));
    EXPECT_EQ("-1000000", AAX::AsStringInt32(-1000000));
}

//------------------------------------------------------
#pragma mark - AsStringUInt32

TEST(AAX_StringUtilities, AsStringUInt32) {
    // Test with various uint32 values
    EXPECT_EQ("0", AAX::AsStringUInt32(0));
    EXPECT_EQ("1", AAX::AsStringUInt32(1));
    EXPECT_EQ("4294967295", AAX::AsStringUInt32(UINT32_MAX));
    
    // Test with common values
    EXPECT_EQ("42", AAX::AsStringUInt32(42));
    EXPECT_EQ("1000000", AAX::AsStringUInt32(1000000));
    EXPECT_EQ("2147483648", AAX::AsStringUInt32(2147483648U)); // 2^31, beyond int32 max
    
    // Test with values that would be negative as int32
    EXPECT_EQ("4294967295", AAX::AsStringUInt32(static_cast<uint32_t>(-1))); // -1 as uint32
    EXPECT_EQ("4294967254", AAX::AsStringUInt32(static_cast<uint32_t>(-42))); // -42 as uint32
}

//------------------------------------------------------
#pragma mark - AsStringIDTriad

TEST(AAX_StringUtilities, AsStringIDTriad) {
    // Create test triads
    AAX_SPlugInIdentifierTriad triad1 = { 'AVID', 'DEMO', 'GAIN' };
    AAX_SPlugInIdentifierTriad triad2 = { 'TEST', 'PLUG', 'IN01' };
    
    // Test conversion
    EXPECT_EQ("(man: 'AVID', prod: 'DEMO', type: 'GAIN')", AAX::AsStringIDTriad(triad1));
    EXPECT_EQ("(man: 'TEST', prod: 'PLUG', type: 'IN01')", AAX::AsStringIDTriad(triad2));
    
    // Test with non-ASCII characters
    AAX_SPlugInIdentifierTriad triad3 = { 'A\0CD', 'D\0MO', 'G\0IN' };
    EXPECT_EQ("(man: 'A(?)CD', prod: 'D(?)MO', type: 'G(?)IN')", AAX::AsStringIDTriad(triad3));
    
    // Test with special characters
    AAX_SPlugInIdentifierTriad triad4 = { '!@#$', '%^&*', '()_+' };
    EXPECT_EQ("(man: '!@#$', prod: '%^&*', type: '()_+')", AAX::AsStringIDTriad(triad4));
    
    // Test with non-printable characters
    AAX_SPlugInIdentifierTriad triad5 = { 0x01020304, 0x05060708, 0x090A0B0C };
    EXPECT_EQ("(man: '(?)(?)(?)(?)', prod: '(?)(?)(?)(?)', type: '(?)(?)(?)(?)')", AAX::AsStringIDTriad(triad5));
    
    // Test with zeros
    AAX_SPlugInIdentifierTriad triad6 = { 0, 0, 0 };
    EXPECT_EQ("(man: '(?)(?)(?)(?)', prod: '(?)(?)(?)(?)', type: '(?)(?)(?)(?)')", AAX::AsStringIDTriad(triad6));
}

//------------------------------------------------------
#pragma mark - AsStringStemFormat

TEST(AAX_StringUtilities, AsStringStemFormat) {
    // Test with common stem formats
    EXPECT_EQ("Mono", AAX::AsStringStemFormat(AAX_eStemFormat_Mono));
    EXPECT_EQ("Stereo", AAX::AsStringStemFormat(AAX_eStemFormat_Stereo));
    EXPECT_EQ("LCR", AAX::AsStringStemFormat(AAX_eStemFormat_LCR));
    EXPECT_EQ("5.1", AAX::AsStringStemFormat(AAX_eStemFormat_5_1));
    
    // Test abbreviated formats
    EXPECT_EQ("Mono", AAX::AsStringStemFormat(AAX_eStemFormat_Mono, true));
    EXPECT_EQ("St", AAX::AsStringStemFormat(AAX_eStemFormat_Stereo, true));
    EXPECT_EQ("LCR", AAX::AsStringStemFormat(AAX_eStemFormat_LCR, true));
    EXPECT_EQ("5.1", AAX::AsStringStemFormat(AAX_eStemFormat_5_1, true));
    
    // Test with additional formats
    EXPECT_EQ("LCRS", AAX::AsStringStemFormat(AAX_eStemFormat_LCRS));
    EXPECT_EQ("Quad", AAX::AsStringStemFormat(AAX_eStemFormat_Quad));
    EXPECT_EQ("5.0", AAX::AsStringStemFormat(AAX_eStemFormat_5_0));
    EXPECT_EQ("6.0", AAX::AsStringStemFormat(AAX_eStemFormat_6_0));
    EXPECT_EQ("6.1", AAX::AsStringStemFormat(AAX_eStemFormat_6_1));
    EXPECT_EQ("7.0 SDDS", AAX::AsStringStemFormat(AAX_eStemFormat_7_0_SDDS));
    EXPECT_EQ("7.1 SDDS", AAX::AsStringStemFormat(AAX_eStemFormat_7_1_SDDS));
    EXPECT_EQ("7.0", AAX::AsStringStemFormat(AAX_eStemFormat_7_0_DTS));
    EXPECT_EQ("7.1", AAX::AsStringStemFormat(AAX_eStemFormat_7_1_DTS));
    EXPECT_EQ("7.0.2", AAX::AsStringStemFormat(AAX_eStemFormat_7_0_2));
    EXPECT_EQ("7.1.2", AAX::AsStringStemFormat(AAX_eStemFormat_7_1_2));
    EXPECT_EQ("5.0.2", AAX::AsStringStemFormat(AAX_eStemFormat_5_0_2));
    EXPECT_EQ("5.1.2", AAX::AsStringStemFormat(AAX_eStemFormat_5_1_2));
    EXPECT_EQ("5.0.4", AAX::AsStringStemFormat(AAX_eStemFormat_5_0_4));
    EXPECT_EQ("5.1.4", AAX::AsStringStemFormat(AAX_eStemFormat_5_1_4));
    EXPECT_EQ("7.0.4", AAX::AsStringStemFormat(AAX_eStemFormat_7_0_4));
    EXPECT_EQ("7.1.4", AAX::AsStringStemFormat(AAX_eStemFormat_7_1_4));
    EXPECT_EQ("7.0.6", AAX::AsStringStemFormat(AAX_eStemFormat_7_0_6));
    EXPECT_EQ("7.1.6", AAX::AsStringStemFormat(AAX_eStemFormat_7_1_6));
    EXPECT_EQ("9.0.4", AAX::AsStringStemFormat(AAX_eStemFormat_9_0_4));
    EXPECT_EQ("9.1.4", AAX::AsStringStemFormat(AAX_eStemFormat_9_1_4));
    EXPECT_EQ("9.0.6", AAX::AsStringStemFormat(AAX_eStemFormat_9_0_6));
    EXPECT_EQ("9.1.6", AAX::AsStringStemFormat(AAX_eStemFormat_9_1_6));

    // Test with Ambisonics formats
    EXPECT_EQ("Ambisonics (1st Order)", AAX::AsStringStemFormat(AAX_eStemFormat_Ambi_1_ACN));
    EXPECT_EQ("Ambisonics (2nd Order)", AAX::AsStringStemFormat(AAX_eStemFormat_Ambi_2_ACN));
    EXPECT_EQ("Ambisonics (3rd Order)", AAX::AsStringStemFormat(AAX_eStemFormat_Ambi_3_ACN));
    EXPECT_EQ("Ambisonics (4th Order)", AAX::AsStringStemFormat(AAX_eStemFormat_Ambi_4_ACN));
    EXPECT_EQ("Ambisonics (5th Order)", AAX::AsStringStemFormat(AAX_eStemFormat_Ambi_5_ACN));
    EXPECT_EQ("Ambisonics (6th Order)", AAX::AsStringStemFormat(AAX_eStemFormat_Ambi_6_ACN));
    EXPECT_EQ("Ambisonics (7th Order)", AAX::AsStringStemFormat(AAX_eStemFormat_Ambi_7_ACN));

    // Test with abbreviated formats
    EXPECT_EQ("LCRS", AAX::AsStringStemFormat(AAX_eStemFormat_LCRS, true));
    EXPECT_EQ("Quad", AAX::AsStringStemFormat(AAX_eStemFormat_Quad, true));
    EXPECT_EQ("5.0", AAX::AsStringStemFormat(AAX_eStemFormat_5_0, true));
    EXPECT_EQ("6.0", AAX::AsStringStemFormat(AAX_eStemFormat_6_0, true));
    EXPECT_EQ("6.1", AAX::AsStringStemFormat(AAX_eStemFormat_6_1, true));
    EXPECT_EQ("7.0 S", AAX::AsStringStemFormat(AAX_eStemFormat_7_0_SDDS, true));
    EXPECT_EQ("7.1 S", AAX::AsStringStemFormat(AAX_eStemFormat_7_1_SDDS, true));
    EXPECT_EQ("7.0", AAX::AsStringStemFormat(AAX_eStemFormat_7_0_DTS, true));
    EXPECT_EQ("7.1", AAX::AsStringStemFormat(AAX_eStemFormat_7_1_DTS, true));
    EXPECT_EQ("7.0.2", AAX::AsStringStemFormat(AAX_eStemFormat_7_0_2, true));
    EXPECT_EQ("7.1.2", AAX::AsStringStemFormat(AAX_eStemFormat_7_1_2, true));
    EXPECT_EQ("5.0.2", AAX::AsStringStemFormat(AAX_eStemFormat_5_0_2, true));
    EXPECT_EQ("5.1.2", AAX::AsStringStemFormat(AAX_eStemFormat_5_1_2, true));
    EXPECT_EQ("5.0.4", AAX::AsStringStemFormat(AAX_eStemFormat_5_0_4, true));
    EXPECT_EQ("5.1.4", AAX::AsStringStemFormat(AAX_eStemFormat_5_1_4, true));
    EXPECT_EQ("7.0.4", AAX::AsStringStemFormat(AAX_eStemFormat_7_0_4, true));
    EXPECT_EQ("7.1.4", AAX::AsStringStemFormat(AAX_eStemFormat_7_1_4, true));
    EXPECT_EQ("7.0.6", AAX::AsStringStemFormat(AAX_eStemFormat_7_0_6, true));
    EXPECT_EQ("7.1.6", AAX::AsStringStemFormat(AAX_eStemFormat_7_1_6, true));
    EXPECT_EQ("9.0.4", AAX::AsStringStemFormat(AAX_eStemFormat_9_0_4, true));
    EXPECT_EQ("9.1.4", AAX::AsStringStemFormat(AAX_eStemFormat_9_1_4, true));
    EXPECT_EQ("9.0.6", AAX::AsStringStemFormat(AAX_eStemFormat_9_0_6, true));
    EXPECT_EQ("9.1.6", AAX::AsStringStemFormat(AAX_eStemFormat_9_1_6, true));

    // Test with abbreviated Ambisonics formats
    EXPECT_EQ("Amb1", AAX::AsStringStemFormat(AAX_eStemFormat_Ambi_1_ACN, true));
    EXPECT_EQ("Amb2", AAX::AsStringStemFormat(AAX_eStemFormat_Ambi_2_ACN, true));
    EXPECT_EQ("Amb3", AAX::AsStringStemFormat(AAX_eStemFormat_Ambi_3_ACN, true));
    EXPECT_EQ("Amb4", AAX::AsStringStemFormat(AAX_eStemFormat_Ambi_4_ACN, true));
    EXPECT_EQ("Amb5", AAX::AsStringStemFormat(AAX_eStemFormat_Ambi_5_ACN, true));
    EXPECT_EQ("Amb6", AAX::AsStringStemFormat(AAX_eStemFormat_Ambi_6_ACN, true));
    EXPECT_EQ("Amb7", AAX::AsStringStemFormat(AAX_eStemFormat_Ambi_7_ACN, true));

    // Test with special stem formats
    EXPECT_EQ("None", AAX::AsStringStemFormat(AAX_eStemFormat_None));
    EXPECT_EQ("None", AAX::AsStringStemFormat(AAX_eStemFormat_None, true));
    EXPECT_EQ("Any", AAX::AsStringStemFormat(AAX_eStemFormat_Any));
    EXPECT_EQ("Any", AAX::AsStringStemFormat(AAX_eStemFormat_Any, true));

    // Test with boundary values
    EXPECT_EQ("unknown stem format", AAX::AsStringStemFormat(AAX_eStemFormat_INT32_MAX));
    EXPECT_EQ("unk", AAX::AsStringStemFormat(AAX_eStemFormat_INT32_MAX, true));
    EXPECT_EQ("unknown stem format", AAX::AsStringStemFormat(AAX_eStemFormatNum));
    EXPECT_EQ("unk", AAX::AsStringStemFormat(AAX_eStemFormatNum, true));

    // Test with unknown format
    EXPECT_EQ("unknown stem format", AAX::AsStringStemFormat(static_cast<AAX_EStemFormat>(999)));
    EXPECT_EQ("unk", AAX::AsStringStemFormat(static_cast<AAX_EStemFormat>(999), true));

    // Test with zero
    EXPECT_EQ("unknown stem format", AAX::AsStringStemFormat(static_cast<AAX_EStemFormat>(0)));
    EXPECT_EQ("unk", AAX::AsStringStemFormat(static_cast<AAX_EStemFormat>(0), true));
}

//------------------------------------------------------
#pragma mark - AsStringStemChannel

TEST(AAX_StringUtilities, AsStringStemChannel) {
    // Test Mono format
    EXPECT_EQ("Audio", AAX::AsStringStemChannel(AAX_eStemFormat_Mono, 0, false));
    EXPECT_EQ("M", AAX::AsStringStemChannel(AAX_eStemFormat_Mono, 0, true));
    
    // Test Stereo format
    EXPECT_EQ("Left", AAX::AsStringStemChannel(AAX_eStemFormat_Stereo, 0, false));
    EXPECT_EQ("Right", AAX::AsStringStemChannel(AAX_eStemFormat_Stereo, 1, false));
    EXPECT_EQ("L", AAX::AsStringStemChannel(AAX_eStemFormat_Stereo, 0, true));
    EXPECT_EQ("R", AAX::AsStringStemChannel(AAX_eStemFormat_Stereo, 1, true));
    
    // Test LCR format
    EXPECT_EQ("Left", AAX::AsStringStemChannel(AAX_eStemFormat_LCR, 0, false));
    EXPECT_EQ("Center", AAX::AsStringStemChannel(AAX_eStemFormat_LCR, 1, false));
    EXPECT_EQ("Right", AAX::AsStringStemChannel(AAX_eStemFormat_LCR, 2, false));
    EXPECT_EQ("L", AAX::AsStringStemChannel(AAX_eStemFormat_LCR, 0, true));
    EXPECT_EQ("C", AAX::AsStringStemChannel(AAX_eStemFormat_LCR, 1, true));
    EXPECT_EQ("R", AAX::AsStringStemChannel(AAX_eStemFormat_LCR, 2, true));
    
    // Test LCRS format
    EXPECT_EQ("Left", AAX::AsStringStemChannel(AAX_eStemFormat_LCRS, 0, false));
    EXPECT_EQ("Center", AAX::AsStringStemChannel(AAX_eStemFormat_LCRS, 1, false));
    EXPECT_EQ("Right", AAX::AsStringStemChannel(AAX_eStemFormat_LCRS, 2, false));
    EXPECT_EQ("Surround", AAX::AsStringStemChannel(AAX_eStemFormat_LCRS, 3, false));
    EXPECT_EQ("L", AAX::AsStringStemChannel(AAX_eStemFormat_LCRS, 0, true));
    EXPECT_EQ("C", AAX::AsStringStemChannel(AAX_eStemFormat_LCRS, 1, true));
    EXPECT_EQ("R", AAX::AsStringStemChannel(AAX_eStemFormat_LCRS, 2, true));
    EXPECT_EQ("S", AAX::AsStringStemChannel(AAX_eStemFormat_LCRS, 3, true));
    
    // Test Quad format
    EXPECT_EQ("Left", AAX::AsStringStemChannel(AAX_eStemFormat_Quad, 0, false));
    EXPECT_EQ("Right", AAX::AsStringStemChannel(AAX_eStemFormat_Quad, 1, false));
    EXPECT_EQ("Left Surround", AAX::AsStringStemChannel(AAX_eStemFormat_Quad, 2, false));
    EXPECT_EQ("Right Surround", AAX::AsStringStemChannel(AAX_eStemFormat_Quad, 3, false));
    EXPECT_EQ("L", AAX::AsStringStemChannel(AAX_eStemFormat_Quad, 0, true));
    EXPECT_EQ("R", AAX::AsStringStemChannel(AAX_eStemFormat_Quad, 1, true));
    EXPECT_EQ("Ls", AAX::AsStringStemChannel(AAX_eStemFormat_Quad, 2, true));
    EXPECT_EQ("Rs", AAX::AsStringStemChannel(AAX_eStemFormat_Quad, 3, true));
    
    // Test 5.0 format (L C R Ls Rs)
    EXPECT_EQ("Left", AAX::AsStringStemChannel(AAX_eStemFormat_5_0, 0, false));
    EXPECT_EQ("Center", AAX::AsStringStemChannel(AAX_eStemFormat_5_0, 1, false));
    EXPECT_EQ("Right", AAX::AsStringStemChannel(AAX_eStemFormat_5_0, 2, false));
    EXPECT_EQ("Left Surround", AAX::AsStringStemChannel(AAX_eStemFormat_5_0, 3, false));
    EXPECT_EQ("Right Surround", AAX::AsStringStemChannel(AAX_eStemFormat_5_0, 4, false));
    EXPECT_EQ("L", AAX::AsStringStemChannel(AAX_eStemFormat_5_0, 0, true));
    EXPECT_EQ("C", AAX::AsStringStemChannel(AAX_eStemFormat_5_0, 1, true));
    EXPECT_EQ("R", AAX::AsStringStemChannel(AAX_eStemFormat_5_0, 2, true));
    EXPECT_EQ("Ls", AAX::AsStringStemChannel(AAX_eStemFormat_5_0, 3, true));
    EXPECT_EQ("Rs", AAX::AsStringStemChannel(AAX_eStemFormat_5_0, 4, true));
    
    // Test 5.1 format (L C R Ls Rs LFE)
    EXPECT_EQ("Left", AAX::AsStringStemChannel(AAX_eStemFormat_5_1, 0, false));
    EXPECT_EQ("Center", AAX::AsStringStemChannel(AAX_eStemFormat_5_1, 1, false));
    EXPECT_EQ("Right", AAX::AsStringStemChannel(AAX_eStemFormat_5_1, 2, false));
    EXPECT_EQ("Left Surround", AAX::AsStringStemChannel(AAX_eStemFormat_5_1, 3, false));
    EXPECT_EQ("Right Surround", AAX::AsStringStemChannel(AAX_eStemFormat_5_1, 4, false));
    EXPECT_EQ("LFE", AAX::AsStringStemChannel(AAX_eStemFormat_5_1, 5, false));
    EXPECT_EQ("L", AAX::AsStringStemChannel(AAX_eStemFormat_5_1, 0, true));
    EXPECT_EQ("C", AAX::AsStringStemChannel(AAX_eStemFormat_5_1, 1, true));
    EXPECT_EQ("R", AAX::AsStringStemChannel(AAX_eStemFormat_5_1, 2, true));
    EXPECT_EQ("Ls", AAX::AsStringStemChannel(AAX_eStemFormat_5_1, 3, true));
    EXPECT_EQ("Rs", AAX::AsStringStemChannel(AAX_eStemFormat_5_1, 4, true));
    EXPECT_EQ("LFE", AAX::AsStringStemChannel(AAX_eStemFormat_5_1, 5, true));
    
    // Test 6.0 format (L C R Ls Cs Rs)
    EXPECT_EQ("Left", AAX::AsStringStemChannel(AAX_eStemFormat_6_0, 0, false));
    EXPECT_EQ("Center", AAX::AsStringStemChannel(AAX_eStemFormat_6_0, 1, false));
    EXPECT_EQ("Right", AAX::AsStringStemChannel(AAX_eStemFormat_6_0, 2, false));
    EXPECT_EQ("Left Surround", AAX::AsStringStemChannel(AAX_eStemFormat_6_0, 3, false));
    EXPECT_EQ("Center Surround", AAX::AsStringStemChannel(AAX_eStemFormat_6_0, 4, false));
    EXPECT_EQ("Right Surround", AAX::AsStringStemChannel(AAX_eStemFormat_6_0, 5, false));
    EXPECT_EQ("L", AAX::AsStringStemChannel(AAX_eStemFormat_6_0, 0, true));
    EXPECT_EQ("C", AAX::AsStringStemChannel(AAX_eStemFormat_6_0, 1, true));
    EXPECT_EQ("R", AAX::AsStringStemChannel(AAX_eStemFormat_6_0, 2, true));
    EXPECT_EQ("Ls", AAX::AsStringStemChannel(AAX_eStemFormat_6_0, 3, true));
    EXPECT_EQ("Cs", AAX::AsStringStemChannel(AAX_eStemFormat_6_0, 4, true));
    EXPECT_EQ("Rs", AAX::AsStringStemChannel(AAX_eStemFormat_6_0, 5, true));
    
    // Test 6.1 format (L C R Ls Cs Rs LFE)
    EXPECT_EQ("Left", AAX::AsStringStemChannel(AAX_eStemFormat_6_1, 0, false));
    EXPECT_EQ("Center", AAX::AsStringStemChannel(AAX_eStemFormat_6_1, 1, false));
    EXPECT_EQ("Right", AAX::AsStringStemChannel(AAX_eStemFormat_6_1, 2, false));
    EXPECT_EQ("Left Surround", AAX::AsStringStemChannel(AAX_eStemFormat_6_1, 3, false));
    EXPECT_EQ("Center Surround", AAX::AsStringStemChannel(AAX_eStemFormat_6_1, 4, false));
    EXPECT_EQ("Right Surround", AAX::AsStringStemChannel(AAX_eStemFormat_6_1, 5, false));
    EXPECT_EQ("LFE", AAX::AsStringStemChannel(AAX_eStemFormat_6_1, 6, false));
    EXPECT_EQ("L", AAX::AsStringStemChannel(AAX_eStemFormat_6_1, 0, true));
    EXPECT_EQ("C", AAX::AsStringStemChannel(AAX_eStemFormat_6_1, 1, true));
    EXPECT_EQ("R", AAX::AsStringStemChannel(AAX_eStemFormat_6_1, 2, true));
    EXPECT_EQ("Ls", AAX::AsStringStemChannel(AAX_eStemFormat_6_1, 3, true));
    EXPECT_EQ("Cs", AAX::AsStringStemChannel(AAX_eStemFormat_6_1, 4, true));
    EXPECT_EQ("Rs", AAX::AsStringStemChannel(AAX_eStemFormat_6_1, 5, true));
    EXPECT_EQ("LFE", AAX::AsStringStemChannel(AAX_eStemFormat_6_1, 6, true));
    
    // Test 7.0 SDDS format (L Lc C Rc R Ls Rs)
    EXPECT_EQ("Left", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_SDDS, 0, false));
    EXPECT_EQ("Left Center", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_SDDS, 1, false));
    EXPECT_EQ("Center", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_SDDS, 2, false));
    EXPECT_EQ("Right Center", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_SDDS, 3, false));
    EXPECT_EQ("Right", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_SDDS, 4, false));
    EXPECT_EQ("Left Surround", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_SDDS, 5, false));
    EXPECT_EQ("Right Surround", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_SDDS, 6, false));
    EXPECT_EQ("L", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_SDDS, 0, true));
    EXPECT_EQ("Lc", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_SDDS, 1, true));
    EXPECT_EQ("C", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_SDDS, 2, true));
    EXPECT_EQ("Rc", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_SDDS, 3, true));
    EXPECT_EQ("R", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_SDDS, 4, true));
    EXPECT_EQ("Ls", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_SDDS, 5, true));
    EXPECT_EQ("Rs", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_SDDS, 6, true));
    
    // Test 7.1 SDDS format (L Lc C Rc R Ls Rs LFE)
    EXPECT_EQ("Left", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_SDDS, 0, false));
    EXPECT_EQ("Left Center", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_SDDS, 1, false));
    EXPECT_EQ("Center", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_SDDS, 2, false));
    EXPECT_EQ("Right Center", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_SDDS, 3, false));
    EXPECT_EQ("Right", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_SDDS, 4, false));
    EXPECT_EQ("Left Surround", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_SDDS, 5, false));
    EXPECT_EQ("Right Surround", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_SDDS, 6, false));
    EXPECT_EQ("LFE", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_SDDS, 7, false));
    EXPECT_EQ("L", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_SDDS, 0, true));
    EXPECT_EQ("Lc", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_SDDS, 1, true));
    EXPECT_EQ("C", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_SDDS, 2, true));
    EXPECT_EQ("Rc", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_SDDS, 3, true));
    EXPECT_EQ("R", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_SDDS, 4, true));
    EXPECT_EQ("Ls", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_SDDS, 5, true));
    EXPECT_EQ("Rs", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_SDDS, 6, true));
    EXPECT_EQ("LFE", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_SDDS, 7, true));
    
    // Test 7.0 DTS format (L C R Lss Rss Lsr Rsr)
    EXPECT_EQ("Left", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_DTS, 0, false));
    EXPECT_EQ("Center", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_DTS, 1, false));
    EXPECT_EQ("Right", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_DTS, 2, false));
    EXPECT_EQ("Left Surround Side", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_DTS, 3, false));
    EXPECT_EQ("Right Surround Side", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_DTS, 4, false));
    EXPECT_EQ("Left Surround Rear", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_DTS, 5, false));
    EXPECT_EQ("Right Surround Rear", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_DTS, 6, false));
    EXPECT_EQ("L", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_DTS, 0, true));
    EXPECT_EQ("C", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_DTS, 1, true));
    EXPECT_EQ("R", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_DTS, 2, true));
    EXPECT_EQ("Lss", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_DTS, 3, true));
    EXPECT_EQ("Rss", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_DTS, 4, true));
    EXPECT_EQ("Lsr", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_DTS, 5, true));
    EXPECT_EQ("Rsr", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_DTS, 6, true));
    
    // Test 7.1 DTS format (L C R Lss Rss Lsr Rsr LFE)
    EXPECT_EQ("Left", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_DTS, 0, false));
    EXPECT_EQ("Center", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_DTS, 1, false));
    EXPECT_EQ("Right", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_DTS, 2, false));
    EXPECT_EQ("Left Surround Side", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_DTS, 3, false));
    EXPECT_EQ("Right Surround Side", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_DTS, 4, false));
    EXPECT_EQ("Left Surround Rear", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_DTS, 5, false));
    EXPECT_EQ("Right Surround Rear", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_DTS, 6, false));
    EXPECT_EQ("LFE", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_DTS, 7, false));
    EXPECT_EQ("L", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_DTS, 0, true));
    EXPECT_EQ("C", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_DTS, 1, true));
    EXPECT_EQ("R", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_DTS, 2, true));
    EXPECT_EQ("Lss", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_DTS, 3, true));
    EXPECT_EQ("Rss", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_DTS, 4, true));
    EXPECT_EQ("Lsr", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_DTS, 5, true));
    EXPECT_EQ("Rsr", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_DTS, 6, true));
    EXPECT_EQ("LFE", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_DTS, 7, true));
    
    // Test 7.0.2 format (L C R Lss Rss Lsr Rsr Ltm Rtm)
    EXPECT_EQ("Left", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_2, 0, false));
    EXPECT_EQ("Center", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_2, 1, false));
    EXPECT_EQ("Right", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_2, 2, false));
    EXPECT_EQ("Left Surround Side", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_2, 3, false));
    EXPECT_EQ("Right Surround Side", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_2, 4, false));
    EXPECT_EQ("Left Surround Rear", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_2, 5, false));
    EXPECT_EQ("Right Surround Rear", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_2, 6, false));
    EXPECT_EQ("Left Top Surround", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_2, 7, false));
    EXPECT_EQ("Right Top Surround", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_2, 8, false));
    EXPECT_EQ("L", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_2, 0, true));
    EXPECT_EQ("C", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_2, 1, true));
    EXPECT_EQ("R", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_2, 2, true));
    EXPECT_EQ("Lss", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_2, 3, true));
    EXPECT_EQ("Rss", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_2, 4, true));
    EXPECT_EQ("Lsr", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_2, 5, true));
    EXPECT_EQ("Rsr", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_2, 6, true));
    EXPECT_EQ("LTS", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_2, 7, true));
    EXPECT_EQ("RTS", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_2, 8, true));
    
    // Test 7.1.2 format (L C R Lss Rss Lsr Rsr LFE Ltm Rtm)
    EXPECT_EQ("Left", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_2, 0, false));
    EXPECT_EQ("Center", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_2, 1, false));
    EXPECT_EQ("Right", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_2, 2, false));
    EXPECT_EQ("Left Surround Side", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_2, 3, false));
    EXPECT_EQ("Right Surround Side", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_2, 4, false));
    EXPECT_EQ("Left Surround Rear", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_2, 5, false));
    EXPECT_EQ("Right Surround Rear", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_2, 6, false));
    EXPECT_EQ("LFE", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_2, 7, false));
    EXPECT_EQ("Left Top Surround", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_2, 8, false));
    EXPECT_EQ("Right Top Surround", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_2, 9, false));
    EXPECT_EQ("L", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_2, 0, true));
    EXPECT_EQ("C", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_2, 1, true));
    EXPECT_EQ("R", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_2, 2, true));
    EXPECT_EQ("Lss", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_2, 3, true));
    EXPECT_EQ("Rss", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_2, 4, true));
    EXPECT_EQ("Lsr", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_2, 5, true));
    EXPECT_EQ("Rsr", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_2, 6, true));
    EXPECT_EQ("LFE", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_2, 7, true));
    EXPECT_EQ("LTS", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_2, 8, true));
    EXPECT_EQ("RTS", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_2, 9, true));
    
    // Test 5.0.2 format (L C R Ls Rs Ltm Rtm)
    EXPECT_EQ("Left", AAX::AsStringStemChannel(AAX_eStemFormat_5_0_2, 0, false));
    EXPECT_EQ("Center", AAX::AsStringStemChannel(AAX_eStemFormat_5_0_2, 1, false));
    EXPECT_EQ("Right", AAX::AsStringStemChannel(AAX_eStemFormat_5_0_2, 2, false));
    EXPECT_EQ("Left Surround", AAX::AsStringStemChannel(AAX_eStemFormat_5_0_2, 3, false));
    EXPECT_EQ("Right Surround", AAX::AsStringStemChannel(AAX_eStemFormat_5_0_2, 4, false));
    EXPECT_EQ("Left Top Middle", AAX::AsStringStemChannel(AAX_eStemFormat_5_0_2, 5, false));
    EXPECT_EQ("Right Top Middle", AAX::AsStringStemChannel(AAX_eStemFormat_5_0_2, 6, false));
    EXPECT_EQ("L", AAX::AsStringStemChannel(AAX_eStemFormat_5_0_2, 0, true));
    EXPECT_EQ("C", AAX::AsStringStemChannel(AAX_eStemFormat_5_0_2, 1, true));
    EXPECT_EQ("R", AAX::AsStringStemChannel(AAX_eStemFormat_5_0_2, 2, true));
    EXPECT_EQ("Ls", AAX::AsStringStemChannel(AAX_eStemFormat_5_0_2, 3, true));
    EXPECT_EQ("Rs", AAX::AsStringStemChannel(AAX_eStemFormat_5_0_2, 4, true));
    EXPECT_EQ("Ltm", AAX::AsStringStemChannel(AAX_eStemFormat_5_0_2, 5, true));
    EXPECT_EQ("Rtm", AAX::AsStringStemChannel(AAX_eStemFormat_5_0_2, 6, true));
    
    // Test 5.1.2 format (L C R Ls Rs LFE Ltm Rtm)
    EXPECT_EQ("Left", AAX::AsStringStemChannel(AAX_eStemFormat_5_1_2, 0, false));
    EXPECT_EQ("Center", AAX::AsStringStemChannel(AAX_eStemFormat_5_1_2, 1, false));
    EXPECT_EQ("Right", AAX::AsStringStemChannel(AAX_eStemFormat_5_1_2, 2, false));
    EXPECT_EQ("Left Surround", AAX::AsStringStemChannel(AAX_eStemFormat_5_1_2, 3, false));
    EXPECT_EQ("Right Surround", AAX::AsStringStemChannel(AAX_eStemFormat_5_1_2, 4, false));
    EXPECT_EQ("LFE", AAX::AsStringStemChannel(AAX_eStemFormat_5_1_2, 5, false));
    EXPECT_EQ("Left Top Middle", AAX::AsStringStemChannel(AAX_eStemFormat_5_1_2, 6, false));
    EXPECT_EQ("Right Top Middle", AAX::AsStringStemChannel(AAX_eStemFormat_5_1_2, 7, false));
    EXPECT_EQ("L", AAX::AsStringStemChannel(AAX_eStemFormat_5_1_2, 0, true));
    EXPECT_EQ("C", AAX::AsStringStemChannel(AAX_eStemFormat_5_1_2, 1, true));
    EXPECT_EQ("R", AAX::AsStringStemChannel(AAX_eStemFormat_5_1_2, 2, true));
    EXPECT_EQ("Ls", AAX::AsStringStemChannel(AAX_eStemFormat_5_1_2, 3, true));
    EXPECT_EQ("Rs", AAX::AsStringStemChannel(AAX_eStemFormat_5_1_2, 4, true));
    EXPECT_EQ("LFE", AAX::AsStringStemChannel(AAX_eStemFormat_5_1_2, 5, true));
    EXPECT_EQ("Ltm", AAX::AsStringStemChannel(AAX_eStemFormat_5_1_2, 6, true));
    EXPECT_EQ("Rtm", AAX::AsStringStemChannel(AAX_eStemFormat_5_1_2, 7, true));

    // Test 5.0.4 format (L C R Ls Rs Ltf Rtf Ltr Rtr)
    EXPECT_EQ("Left", AAX::AsStringStemChannel(AAX_eStemFormat_5_0_4, 0, false));
    EXPECT_EQ("Center", AAX::AsStringStemChannel(AAX_eStemFormat_5_0_4, 1, false));
    EXPECT_EQ("Right", AAX::AsStringStemChannel(AAX_eStemFormat_5_0_4, 2, false));
    EXPECT_EQ("Left Surround", AAX::AsStringStemChannel(AAX_eStemFormat_5_0_4, 3, false));
    EXPECT_EQ("Right Surround", AAX::AsStringStemChannel(AAX_eStemFormat_5_0_4, 4, false));
    EXPECT_EQ("Left Top Front", AAX::AsStringStemChannel(AAX_eStemFormat_5_0_4, 5, false));
    EXPECT_EQ("Right Top Front", AAX::AsStringStemChannel(AAX_eStemFormat_5_0_4, 6, false));
    EXPECT_EQ("Left Top Rear", AAX::AsStringStemChannel(AAX_eStemFormat_5_0_4, 7, false));
    EXPECT_EQ("Right Top Rear", AAX::AsStringStemChannel(AAX_eStemFormat_5_0_4, 8, false));
    EXPECT_EQ("L", AAX::AsStringStemChannel(AAX_eStemFormat_5_0_4, 0, true));
    EXPECT_EQ("C", AAX::AsStringStemChannel(AAX_eStemFormat_5_0_4, 1, true));
    EXPECT_EQ("R", AAX::AsStringStemChannel(AAX_eStemFormat_5_0_4, 2, true));
    EXPECT_EQ("Ls", AAX::AsStringStemChannel(AAX_eStemFormat_5_0_4, 3, true));
    EXPECT_EQ("Rs", AAX::AsStringStemChannel(AAX_eStemFormat_5_0_4, 4, true));
    EXPECT_EQ("Ltf", AAX::AsStringStemChannel(AAX_eStemFormat_5_0_4, 5, true));
    EXPECT_EQ("Rtf", AAX::AsStringStemChannel(AAX_eStemFormat_5_0_4, 6, true));
    EXPECT_EQ("Ltr", AAX::AsStringStemChannel(AAX_eStemFormat_5_0_4, 7, true));
    EXPECT_EQ("Rtr", AAX::AsStringStemChannel(AAX_eStemFormat_5_0_4, 8, true));

    // Test 5.1.4 format (L C R Ls Rs LFE Ltf Rtf Ltr Rtr)
    EXPECT_EQ("Left", AAX::AsStringStemChannel(AAX_eStemFormat_5_1_4, 0, false));
    EXPECT_EQ("Center", AAX::AsStringStemChannel(AAX_eStemFormat_5_1_4, 1, false));
    EXPECT_EQ("Right", AAX::AsStringStemChannel(AAX_eStemFormat_5_1_4, 2, false));
    EXPECT_EQ("Left Surround", AAX::AsStringStemChannel(AAX_eStemFormat_5_1_4, 3, false));
    EXPECT_EQ("Right Surround", AAX::AsStringStemChannel(AAX_eStemFormat_5_1_4, 4, false));
    EXPECT_EQ("LFE", AAX::AsStringStemChannel(AAX_eStemFormat_5_1_4, 5, false));
    EXPECT_EQ("Left Top Front", AAX::AsStringStemChannel(AAX_eStemFormat_5_1_4, 6, false));
    EXPECT_EQ("Right Top Front", AAX::AsStringStemChannel(AAX_eStemFormat_5_1_4, 7, false));
    EXPECT_EQ("Left Top Rear", AAX::AsStringStemChannel(AAX_eStemFormat_5_1_4, 8, false));
    EXPECT_EQ("Right Top Rear", AAX::AsStringStemChannel(AAX_eStemFormat_5_1_4, 9, false));
    EXPECT_EQ("L", AAX::AsStringStemChannel(AAX_eStemFormat_5_1_4, 0, true));
    EXPECT_EQ("C", AAX::AsStringStemChannel(AAX_eStemFormat_5_1_4, 1, true));
    EXPECT_EQ("R", AAX::AsStringStemChannel(AAX_eStemFormat_5_1_4, 2, true));
    EXPECT_EQ("Ls", AAX::AsStringStemChannel(AAX_eStemFormat_5_1_4, 3, true));
    EXPECT_EQ("Rs", AAX::AsStringStemChannel(AAX_eStemFormat_5_1_4, 4, true));
    EXPECT_EQ("LFE", AAX::AsStringStemChannel(AAX_eStemFormat_5_1_4, 5, true));
    EXPECT_EQ("Ltf", AAX::AsStringStemChannel(AAX_eStemFormat_5_1_4, 6, true));
    EXPECT_EQ("Rtf", AAX::AsStringStemChannel(AAX_eStemFormat_5_1_4, 7, true));
    EXPECT_EQ("Ltr", AAX::AsStringStemChannel(AAX_eStemFormat_5_1_4, 8, true));
    EXPECT_EQ("Rtr", AAX::AsStringStemChannel(AAX_eStemFormat_5_1_4, 9, true));

    // Test 7.1.4 format (L C R Lss Rss Lsr Rsr LFE Ltf Rtf Ltr Rtr)
    EXPECT_EQ("Left", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_4, 0, false));
    EXPECT_EQ("Center", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_4, 1, false));
    EXPECT_EQ("Right", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_4, 2, false));
    EXPECT_EQ("Left Surround Side", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_4, 3, false));
    EXPECT_EQ("Right Surround Side", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_4, 4, false));
    EXPECT_EQ("Left Surround Rear", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_4, 5, false));
    EXPECT_EQ("Right Surround Rear", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_4, 6, false));
    EXPECT_EQ("LFE", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_4, 7, false));
    EXPECT_EQ("Left Top Front", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_4, 8, false));
    EXPECT_EQ("Right Top Front", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_4, 9, false));
    EXPECT_EQ("Left Top Rear", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_4, 10, false));
    EXPECT_EQ("Right Top Rear", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_4, 11, false));
    EXPECT_EQ("L", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_4, 0, true));
    EXPECT_EQ("C", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_4, 1, true));
    EXPECT_EQ("R", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_4, 2, true));
    EXPECT_EQ("Lss", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_4, 3, true));
    EXPECT_EQ("Rss", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_4, 4, true));
    EXPECT_EQ("Lsr", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_4, 5, true));
    EXPECT_EQ("Rsr", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_4, 6, true));
    EXPECT_EQ("LFE", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_4, 7, true));
    EXPECT_EQ("Ltf", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_4, 8, true));
    EXPECT_EQ("Rtf", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_4, 9, true));
    EXPECT_EQ("Ltr", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_4, 10, true));
    EXPECT_EQ("Rtr", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_4, 11, true));

    // Test 7.0.4 format (L C R Lss Rss Lsr Rsr Ltf Rtf Ltr Rtr)
    EXPECT_EQ("Left", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_4, 0, false));
    EXPECT_EQ("Center", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_4, 1, false));
    EXPECT_EQ("Right", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_4, 2, false));
    EXPECT_EQ("Left Surround Side", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_4, 3, false));
    EXPECT_EQ("Right Surround Side", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_4, 4, false));
    EXPECT_EQ("Left Surround Rear", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_4, 5, false));
    EXPECT_EQ("Right Surround Rear", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_4, 6, false));
    EXPECT_EQ("Left Top Front", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_4, 7, false));
    EXPECT_EQ("Right Top Front", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_4, 8, false));
    EXPECT_EQ("Left Top Rear", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_4, 9, false));
    EXPECT_EQ("Right Top Rear", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_4, 10, false));
    EXPECT_EQ("L", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_4, 0, true));
    EXPECT_EQ("C", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_4, 1, true));
    EXPECT_EQ("R", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_4, 2, true));
    EXPECT_EQ("Lss", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_4, 3, true));
    EXPECT_EQ("Rss", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_4, 4, true));
    EXPECT_EQ("Lsr", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_4, 5, true));
    EXPECT_EQ("Rsr", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_4, 6, true));
    EXPECT_EQ("Ltf", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_4, 7, true));
    EXPECT_EQ("Rtf", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_4, 8, true));
    EXPECT_EQ("Ltr", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_4, 9, true));
    EXPECT_EQ("Rtr", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_4, 10, true));

    // Test 7.0.6 format (L C R Lss Rss Lsr Rsr Ltf Rtf Ltm Rtm Ltr Rtr)
    EXPECT_EQ("Left", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_6, 0, false));
    EXPECT_EQ("Center", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_6, 1, false));
    EXPECT_EQ("Right", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_6, 2, false));
    EXPECT_EQ("Left Surround Side", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_6, 3, false));
    EXPECT_EQ("Right Surround Side", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_6, 4, false));
    EXPECT_EQ("Left Surround Rear", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_6, 5, false));
    EXPECT_EQ("Right Surround Rear", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_6, 6, false));
    EXPECT_EQ("Left Top Front", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_6, 7, false));
    EXPECT_EQ("Right Top Front", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_6, 8, false));
    EXPECT_EQ("Left Top Middle", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_6, 9, false));
    EXPECT_EQ("Right Top Middle", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_6, 10, false));
    EXPECT_EQ("Left Top Rear", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_6, 11, false));
    EXPECT_EQ("Right Top Rear", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_6, 12, false));
    EXPECT_EQ("L", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_6, 0, true));
    EXPECT_EQ("C", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_6, 1, true));
    EXPECT_EQ("R", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_6, 2, true));
    EXPECT_EQ("Lss", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_6, 3, true));
    EXPECT_EQ("Rss", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_6, 4, true));
    EXPECT_EQ("Lsr", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_6, 5, true));
    EXPECT_EQ("Rsr", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_6, 6, true));
    EXPECT_EQ("Ltf", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_6, 7, true));
    EXPECT_EQ("Rtf", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_6, 8, true));
    EXPECT_EQ("Ltm", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_6, 9, true));
    EXPECT_EQ("Rtm", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_6, 10, true));
    EXPECT_EQ("Ltr", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_6, 11, true));
    EXPECT_EQ("Rtr", AAX::AsStringStemChannel(AAX_eStemFormat_7_0_6, 12, true));

    // Test 7.1.6 format (L C R Lss Rss Lsr Rsr LFE Ltf Rtf Ltm Rtm Ltr Rtr)
    EXPECT_EQ("Left", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_6, 0, false));
    EXPECT_EQ("Center", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_6, 1, false));
    EXPECT_EQ("Right", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_6, 2, false));
    EXPECT_EQ("Left Surround Side", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_6, 3, false));
    EXPECT_EQ("Right Surround Side", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_6, 4, false));
    EXPECT_EQ("Left Surround Rear", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_6, 5, false));
    EXPECT_EQ("Right Surround Rear", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_6, 6, false));
    EXPECT_EQ("LFE", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_6, 7, false));
    EXPECT_EQ("Left Top Front", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_6, 8, false));
    EXPECT_EQ("Right Top Front", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_6, 9, false));
    EXPECT_EQ("Left Top Middle", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_6, 10, false));
    EXPECT_EQ("Right Top Middle", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_6, 11, false));
    EXPECT_EQ("Left Top Rear", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_6, 12, false));
    EXPECT_EQ("Right Top Rear", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_6, 13, false));
    EXPECT_EQ("L", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_6, 0, true));
    EXPECT_EQ("C", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_6, 1, true));
    EXPECT_EQ("R", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_6, 2, true));
    EXPECT_EQ("Lss", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_6, 3, true));
    EXPECT_EQ("Rss", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_6, 4, true));
    EXPECT_EQ("Lsr", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_6, 5, true));
    EXPECT_EQ("Rsr", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_6, 6, true));
    EXPECT_EQ("LFE", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_6, 7, true));
    EXPECT_EQ("Ltf", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_6, 8, true));
    EXPECT_EQ("Rtf", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_6, 9, true));
    EXPECT_EQ("Ltm", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_6, 10, true));
    EXPECT_EQ("Rtm", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_6, 11, true));
    EXPECT_EQ("Ltr", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_6, 12, true));
    EXPECT_EQ("Rtr", AAX::AsStringStemChannel(AAX_eStemFormat_7_1_6, 13, true));

    // Test 9.0.4 format (L C R Lw Rw Lss Rss Lsr Rsr Ltf Rtf Ltr Rtr)
    EXPECT_EQ("Left", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_4, 0, false));
    EXPECT_EQ("Center", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_4, 1, false));
    EXPECT_EQ("Right", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_4, 2, false));
    EXPECT_EQ("Left Wide", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_4, 3, false));
    EXPECT_EQ("Right Wide", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_4, 4, false));
    EXPECT_EQ("Left Surround Side", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_4, 5, false));
    EXPECT_EQ("Right Surround Side", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_4, 6, false));
    EXPECT_EQ("Left Surround Rear", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_4, 7, false));
    EXPECT_EQ("Right Surround Rear", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_4, 8, false));
    EXPECT_EQ("Left Top Front", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_4, 9, false));
    EXPECT_EQ("Right Top Front", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_4, 10, false));
    EXPECT_EQ("Left Top Rear", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_4, 11, false));
    EXPECT_EQ("Right Top Rear", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_4, 12, false));
    EXPECT_EQ("L", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_4, 0, true));
    EXPECT_EQ("C", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_4, 1, true));
    EXPECT_EQ("R", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_4, 2, true));
    EXPECT_EQ("Lw", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_4, 3, true));
    EXPECT_EQ("Rw", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_4, 4, true));
    EXPECT_EQ("Lss", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_4, 5, true));
    EXPECT_EQ("Rss", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_4, 6, true));
    EXPECT_EQ("Lsr", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_4, 7, true));
    EXPECT_EQ("Rsr", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_4, 8, true));
    EXPECT_EQ("Ltf", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_4, 9, true));
    EXPECT_EQ("Rtf", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_4, 10, true));
    EXPECT_EQ("Ltr", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_4, 11, true));
    EXPECT_EQ("Rtr", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_4, 12, true));

    // Test 9.1.4 format (L C R Lw Rw Lss Rss Lsr Rsr LFE Ltf Rtf Ltr Rtr)
    EXPECT_EQ("Left", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_4, 0, false));
    EXPECT_EQ("Center", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_4, 1, false));
    EXPECT_EQ("Right", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_4, 2, false));
    EXPECT_EQ("Left Wide", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_4, 3, false));
    EXPECT_EQ("Right Wide", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_4, 4, false));
    EXPECT_EQ("Left Surround Side", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_4, 5, false));
    EXPECT_EQ("Right Surround Side", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_4, 6, false));
    EXPECT_EQ("Left Surround Rear", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_4, 7, false));
    EXPECT_EQ("Right Surround Rear", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_4, 8, false));
    EXPECT_EQ("LFE", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_4, 9, false));
    EXPECT_EQ("Left Top Front", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_4, 10, false));
    EXPECT_EQ("Right Top Front", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_4, 11, false));
    EXPECT_EQ("Left Top Rear", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_4, 12, false));
    EXPECT_EQ("Right Top Rear", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_4, 13, false));
    EXPECT_EQ("L", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_4, 0, true));
    EXPECT_EQ("C", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_4, 1, true));
    EXPECT_EQ("R", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_4, 2, true));
    EXPECT_EQ("Lw", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_4, 3, true));
    EXPECT_EQ("Rw", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_4, 4, true));
    EXPECT_EQ("Lss", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_4, 5, true));
    EXPECT_EQ("Rss", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_4, 6, true));
    EXPECT_EQ("Lsr", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_4, 7, true));
    EXPECT_EQ("Rsr", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_4, 8, true));
    EXPECT_EQ("LFE", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_4, 9, true));
    EXPECT_EQ("Ltf", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_4, 10, true));
    EXPECT_EQ("Rtf", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_4, 11, true));
    EXPECT_EQ("Ltr", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_4, 12, true));
    EXPECT_EQ("Rtr", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_4, 13, true));

    // Test 9.0.6 format (L C R Lw Rw Lss Rss Lsr Rsr Ltf Rtf Ltm Rtm Ltr Rtr)
    EXPECT_EQ("Left", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_6, 0, false));
    EXPECT_EQ("Center", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_6, 1, false));
    EXPECT_EQ("Right", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_6, 2, false));
    EXPECT_EQ("Left Wide", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_6, 3, false));
    EXPECT_EQ("Right Wide", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_6, 4, false));
    EXPECT_EQ("Left Surround Side", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_6, 5, false));
    EXPECT_EQ("Right Surround Side", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_6, 6, false));
    EXPECT_EQ("Left Surround Rear", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_6, 7, false));
    EXPECT_EQ("Right Surround Rear", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_6, 8, false));
    EXPECT_EQ("Left Top Front", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_6, 9, false));
    EXPECT_EQ("Right Top Front", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_6, 10, false));
    EXPECT_EQ("Left Top Middle", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_6, 11, false));
    EXPECT_EQ("Right Top Middle", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_6, 12, false));
    EXPECT_EQ("Left Top Rear", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_6, 13, false));
    EXPECT_EQ("Right Top Rear", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_6, 14, false));
    EXPECT_EQ("L", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_6, 0, true));
    EXPECT_EQ("C", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_6, 1, true));
    EXPECT_EQ("R", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_6, 2, true));
    EXPECT_EQ("Lw", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_6, 3, true));
    EXPECT_EQ("Rw", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_6, 4, true));
    EXPECT_EQ("Lss", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_6, 5, true));
    EXPECT_EQ("Rss", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_6, 6, true));
    EXPECT_EQ("Lsr", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_6, 7, true));
    EXPECT_EQ("Rsr", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_6, 8, true));
    EXPECT_EQ("Ltf", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_6, 9, true));
    EXPECT_EQ("Rtf", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_6, 10, true));
    EXPECT_EQ("Ltm", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_6, 11, true));
    EXPECT_EQ("Rtm", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_6, 12, true));
    EXPECT_EQ("Ltr", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_6, 13, true));
    EXPECT_EQ("Rtr", AAX::AsStringStemChannel(AAX_eStemFormat_9_0_6, 14, true));

    // Test 9.1.6 format (L C R Lw Rw Lss Rss Lsr Rsr LFE Ltf Rtf Ltm Rtm Ltr Rtr)
    EXPECT_EQ("Left", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_6, 0, false));
    EXPECT_EQ("Center", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_6, 1, false));
    EXPECT_EQ("Right", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_6, 2, false));
    EXPECT_EQ("Left Wide", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_6, 3, false));
    EXPECT_EQ("Right Wide", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_6, 4, false));
    EXPECT_EQ("Left Surround Side", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_6, 5, false));
    EXPECT_EQ("Right Surround Side", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_6, 6, false));
    EXPECT_EQ("Left Surround Rear", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_6, 7, false));
    EXPECT_EQ("Right Surround Rear", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_6, 8, false));
    EXPECT_EQ("LFE", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_6, 9, false));
    EXPECT_EQ("Left Top Front", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_6, 10, false));
    EXPECT_EQ("Right Top Front", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_6, 11, false));
    EXPECT_EQ("Left Top Middle", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_6, 12, false));
    EXPECT_EQ("Right Top Middle", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_6, 13, false));
    EXPECT_EQ("Left Top Rear", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_6, 14, false));
    EXPECT_EQ("Right Top Rear", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_6, 15, false));
    EXPECT_EQ("L", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_6, 0, true));
    EXPECT_EQ("C", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_6, 1, true));
    EXPECT_EQ("R", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_6, 2, true));
    EXPECT_EQ("Lw", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_6, 3, true));
    EXPECT_EQ("Rw", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_6, 4, true));
    EXPECT_EQ("Lss", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_6, 5, true));
    EXPECT_EQ("Rss", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_6, 6, true));
    EXPECT_EQ("Lsr", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_6, 7, true));
    EXPECT_EQ("Rsr", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_6, 8, true));
    EXPECT_EQ("LFE", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_6, 9, true));
    EXPECT_EQ("Ltf", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_6, 10, true));
    EXPECT_EQ("Rtf", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_6, 11, true));
    EXPECT_EQ("Ltm", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_6, 12, true));
    EXPECT_EQ("Rtm", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_6, 13, true));
    EXPECT_EQ("Ltr", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_6, 14, true));
    EXPECT_EQ("Rtr", AAX::AsStringStemChannel(AAX_eStemFormat_9_1_6, 15, true));

    // Test Ambisonics 1st Order format (4 channels)
    EXPECT_EQ("1", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_1_ACN, 0, false));
    EXPECT_EQ("2", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_1_ACN, 1, false));
    EXPECT_EQ("3", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_1_ACN, 2, false));
    EXPECT_EQ("4", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_1_ACN, 3, false));
    EXPECT_EQ("1", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_1_ACN, 0, true));
    EXPECT_EQ("2", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_1_ACN, 1, true));
    EXPECT_EQ("3", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_1_ACN, 2, true));
    EXPECT_EQ("4", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_1_ACN, 3, true));

    // Test Ambisonics 2nd Order format (9 channels)
    EXPECT_EQ("1", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_2_ACN, 0, false));
    EXPECT_EQ("2", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_2_ACN, 1, false));
    EXPECT_EQ("3", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_2_ACN, 2, false));
    EXPECT_EQ("4", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_2_ACN, 3, false));
    EXPECT_EQ("5", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_2_ACN, 4, false));
    EXPECT_EQ("6", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_2_ACN, 5, false));
    EXPECT_EQ("7", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_2_ACN, 6, false));
    EXPECT_EQ("8", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_2_ACN, 7, false));
    EXPECT_EQ("9", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_2_ACN, 8, false));
    EXPECT_EQ("1", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_2_ACN, 0, true));
    EXPECT_EQ("2", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_2_ACN, 1, true));
    EXPECT_EQ("3", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_2_ACN, 2, true));
    EXPECT_EQ("4", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_2_ACN, 3, true));
    EXPECT_EQ("5", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_2_ACN, 4, true));
    EXPECT_EQ("6", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_2_ACN, 5, true));
    EXPECT_EQ("7", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_2_ACN, 6, true));
    EXPECT_EQ("8", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_2_ACN, 7, true));
    EXPECT_EQ("9", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_2_ACN, 8, true));

    // Test Ambisonics 3rd Order format (16 channels) - test first few and last few
    EXPECT_EQ("1", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_3_ACN, 0, false));
    EXPECT_EQ("2", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_3_ACN, 1, false));
    EXPECT_EQ("3", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_3_ACN, 2, false));
    EXPECT_EQ("14", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_3_ACN, 13, false));
    EXPECT_EQ("15", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_3_ACN, 14, false));
    EXPECT_EQ("16", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_3_ACN, 15, false));

    // Test Ambisonics 4th Order format (25 channels) - test first few and last few
    EXPECT_EQ("1", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_4_ACN, 0, false));
    EXPECT_EQ("2", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_4_ACN, 1, false));
    EXPECT_EQ("23", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_4_ACN, 22, false));
    EXPECT_EQ("24", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_4_ACN, 23, false));
    EXPECT_EQ("25", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_4_ACN, 24, false));

    // Test Ambisonics 5th Order format (36 channels) - test first few and last few
    EXPECT_EQ("1", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_5_ACN, 0, false));
    EXPECT_EQ("34", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_5_ACN, 33, false));
    EXPECT_EQ("35", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_5_ACN, 34, false));
    EXPECT_EQ("36", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_5_ACN, 35, false));

    // Test Ambisonics 6th Order format (49 channels) - test first few and last few
    EXPECT_EQ("1", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_6_ACN, 0, false));
    EXPECT_EQ("47", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_6_ACN, 46, false));
    EXPECT_EQ("48", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_6_ACN, 47, false));
    EXPECT_EQ("49", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_6_ACN, 48, false));

    // Test Ambisonics 7th Order format (64 channels) - test first few and last few
    EXPECT_EQ("1", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_7_ACN, 0, false));
    EXPECT_EQ("62", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_7_ACN, 61, false));
    EXPECT_EQ("63", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_7_ACN, 62, false));
    EXPECT_EQ("64", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_7_ACN, 63, false));

    // Test with special stem formats
    EXPECT_EQ("unknown", AAX::AsStringStemChannel(AAX_eStemFormat_None, 0, false));
    EXPECT_EQ("?", AAX::AsStringStemChannel(AAX_eStemFormat_None, 0, true));
    EXPECT_EQ("unknown", AAX::AsStringStemChannel(AAX_eStemFormat_Any, 0, false));
    EXPECT_EQ("?", AAX::AsStringStemChannel(AAX_eStemFormat_Any, 0, true));

    // Test with boundary values
    EXPECT_EQ("unknown", AAX::AsStringStemChannel(AAX_eStemFormat_INT32_MAX, 0, false));
    EXPECT_EQ("?", AAX::AsStringStemChannel(AAX_eStemFormat_INT32_MAX, 0, true));
    EXPECT_EQ("unknown", AAX::AsStringStemChannel(AAX_eStemFormatNum, 0, false));
    EXPECT_EQ("?", AAX::AsStringStemChannel(AAX_eStemFormatNum, 0, true));

    // Test with invalid channel index
    EXPECT_EQ("unknown", AAX::AsStringStemChannel(AAX_eStemFormat_Stereo, 2, false));
    EXPECT_EQ("?", AAX::AsStringStemChannel(AAX_eStemFormat_Stereo, 2, true));
    EXPECT_EQ("unknown", AAX::AsStringStemChannel(AAX_eStemFormat_5_1, 6, false));
    EXPECT_EQ("?", AAX::AsStringStemChannel(AAX_eStemFormat_5_1, 6, true));

    // Test with unknown stem format
    EXPECT_EQ("unknown", AAX::AsStringStemChannel(static_cast<AAX_EStemFormat>(999), 0, false));
    EXPECT_EQ("?", AAX::AsStringStemChannel(static_cast<AAX_EStemFormat>(999), 0, true));

    // Test with very large channel index
    EXPECT_EQ("unknown", AAX::AsStringStemChannel(AAX_eStemFormat_Stereo, UINT32_MAX, false));
    EXPECT_EQ("?", AAX::AsStringStemChannel(AAX_eStemFormat_Stereo, UINT32_MAX, true));

    // Test Ambisonics formats with out-of-range channel indices
    EXPECT_EQ("unknown", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_1_ACN, 4, false)); // 1st order has 4 channels (0-3)
    EXPECT_EQ("?", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_1_ACN, 4, true));
    EXPECT_EQ("unknown", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_2_ACN, 9, false)); // 2nd order has 9 channels (0-8)
    EXPECT_EQ("?", AAX::AsStringStemChannel(AAX_eStemFormat_Ambi_2_ACN, 9, true));
}


//------------------------------------------------------
#pragma mark - AsStringResult

TEST(AAX_StringUtilities, AsStringResult) {
    // Test with common result codes
    EXPECT_EQ("AAX_SUCCESS", AAX::AsStringResult(AAX_SUCCESS));
    EXPECT_EQ("AAX_ERROR_INVALID_PARAMETER_ID", AAX::AsStringResult(AAX_ERROR_INVALID_PARAMETER_ID));
    EXPECT_EQ("AAX_ERROR_INVALID_CHUNK_ID", AAX::AsStringResult(AAX_ERROR_INVALID_CHUNK_ID));
    EXPECT_EQ("AAX_ERROR_UNKNOWN_PLUGIN", AAX::AsStringResult(AAX_ERROR_UNKNOWN_PLUGIN));
    
    // Test with plugin-defined error range
    int32_t pluginError = AAX_ERROR_PLUGIN_BEGIN - 1; // Within plugin range
    EXPECT_EQ("plug-in defined error", AAX::AsStringResult(pluginError));

    // Test with boundary values
    EXPECT_EQ("plug-in defined error", AAX::AsStringResult(AAX_ERROR_PLUGIN_BEGIN));
    EXPECT_EQ("<unknown error code>", AAX::AsStringResult(AAX_ERROR_PLUGIN_END)); // END is exclusive
    
    // Test with unknown result code
    EXPECT_EQ("<unknown error code>", AAX::AsStringResult(static_cast<AAX_Result>(999)));
    EXPECT_EQ("<unknown error code>", AAX::AsStringResult(static_cast<AAX_Result>(-999)));
    
    // Test with extreme values
    EXPECT_EQ("<unknown error code>", AAX::AsStringResult(INT32_MAX));
    EXPECT_EQ("<unknown error code>", AAX::AsStringResult(INT32_MIN));
}

//------------------------------------------------------
#pragma mark - AsStringSupportLevel

TEST(AAX_StringUtilities, AsStringSupportLevel) {
    // Test with different support levels
    EXPECT_EQ("AAX_eSupportLevel_Uninitialized", AAX::AsStringSupportLevel(AAX_eSupportLevel_Uninitialized));
    EXPECT_EQ("AAX_eSupportLevel_Unsupported", AAX::AsStringSupportLevel(AAX_eSupportLevel_Unsupported));
    EXPECT_EQ("AAX_eSupportLevel_Supported", AAX::AsStringSupportLevel(AAX_eSupportLevel_Supported));
    EXPECT_EQ("AAX_eSupportLevel_Disabled", AAX::AsStringSupportLevel(AAX_eSupportLevel_Disabled));
    EXPECT_EQ("AAX_eSupportLevel_ByProperty", AAX::AsStringSupportLevel(AAX_eSupportLevel_ByProperty));
    
    // Test with unknown support level
    int unknownLevel = 999;
    EXPECT_EQ("999", AAX::AsStringSupportLevel(static_cast<AAX_ESupportLevel>(unknownLevel)));
}

//------------------------------------------------------
#pragma mark - GetCStringOfLength

TEST(AAX_StringUtilities, GetCStringOfLength) {
    // Test normal case
    char output1[10] = {0};
    AAX::GetCStringOfLength(output1, "Hello", 5);
    EXPECT_STREQ("Hello", output1);
    
    // Test truncation
    char output2[4] = {0};
    AAX::GetCStringOfLength(output2, "Hello", 3);
    EXPECT_STREQ("Hel", output2);
    
    // Test with empty string
    char output3[10] = {0};
    AAX::GetCStringOfLength(output3, "", 5);
    EXPECT_STREQ("", output3);
    
    // Test with null termination
    char output4[10] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'};
    AAX::GetCStringOfLength(output4, "Test", 4);
    EXPECT_STREQ("Test", output4);
}

//------------------------------------------------------
#pragma mark - Caseless_strcmp

TEST(AAX_StringUtilities, Caseless_strcmp) {
    // Test with identical strings
    EXPECT_EQ(0, AAX::Caseless_strcmp("hello", "hello"));
    
    // Test with case differences
    EXPECT_EQ(0, AAX::Caseless_strcmp("Hello", "hello"));
    EXPECT_EQ(0, AAX::Caseless_strcmp("HELLO", "hello"));
    EXPECT_EQ(0, AAX::Caseless_strcmp("HeLLo", "hEllO"));
    
    // Test with standard strcmp behavior
    EXPECT_LT(AAX::Caseless_strcmp("apple", "banana"), 0); // "apple" < "banana"
    EXPECT_GT(AAX::Caseless_strcmp("zebra", "apple"), 0);  // "zebra" > "apple"
    
    // Test with empty strings
    EXPECT_LT(AAX::Caseless_strcmp("", "a"), 0);  // "" < "a"
    EXPECT_GT(AAX::Caseless_strcmp("a", ""), 0);  // "a" > ""
    
    // Test with null pointers
    EXPECT_LT(AAX::Caseless_strcmp(nullptr, "hello"), 0); // nullptr < "hello"
    EXPECT_GT(AAX::Caseless_strcmp("hello", nullptr), 0); // "hello" > nullptr
    EXPECT_EQ(0, AAX::Caseless_strcmp(nullptr, nullptr)); // nullptr == nullptr
    
    // Test with different capitalization schemes
    EXPECT_LT(AAX::Caseless_strcmp("APPLE", "banana"), 0); // "APPLE" < "banana" (case insensitive)
    EXPECT_GT(AAX::Caseless_strcmp("ZEBRA", "apple"), 0);  // "ZEBRA" > "apple" (case insensitive)
    EXPECT_LT(AAX::Caseless_strcmp("Apple", "BANANA"), 0); // "Apple" < "BANANA" (case insensitive)
    EXPECT_GT(AAX::Caseless_strcmp("Zebra", "APPLE"), 0);  // "Zebra" > "APPLE" (case insensitive)
    EXPECT_EQ(0, AAX::Caseless_strcmp("MiXeD", "mixed"));  // "MiXeD" == "mixed" (case insensitive)
    EXPECT_EQ(0, AAX::Caseless_strcmp("CaSe", "CASE"));    // "CaSe" == "CASE" (case insensitive)
    
    // Test with numbers
    EXPECT_EQ(0, AAX::Caseless_strcmp("123", "123"));      // Numbers are equal
    EXPECT_LT(AAX::Caseless_strcmp("123", "124"), 0);      // "123" < "124"
    EXPECT_GT(AAX::Caseless_strcmp("124", "123"), 0);      // "124" > "123"
    EXPECT_LT(AAX::Caseless_strcmp("12", "123"), 0);       // "12" < "123" (shorter)
    EXPECT_GT(AAX::Caseless_strcmp("123", "12"), 0);       // "123" > "12" (longer)
    
    // Test with mixed alphanumeric
    EXPECT_EQ(0, AAX::Caseless_strcmp("Test123", "test123")); // Case insensitive alphanumeric
    EXPECT_EQ(0, AAX::Caseless_strcmp("ABC123", "abc123"));   // Case insensitive alphanumeric
    EXPECT_LT(AAX::Caseless_strcmp("Test123", "Test124"), 0); // "Test123" < "Test124"
    EXPECT_GT(AAX::Caseless_strcmp("Test124", "Test123"), 0); // "Test124" > "Test123"
    
    // Test with symbols and special characters
    EXPECT_EQ(0, AAX::Caseless_strcmp("!@#$", "!@#$"));      // Symbols are equal
    EXPECT_LT(AAX::Caseless_strcmp("!", "~"), 0);            // "!" < "~" (ASCII order)
    EXPECT_GT(AAX::Caseless_strcmp("~", "!"), 0);            // "~" > "!" (ASCII order)
    EXPECT_LT(AAX::Caseless_strcmp("abc!", "abc~"), 0);      // "abc!" < "abc~"
    EXPECT_GT(AAX::Caseless_strcmp("abc~", "abc!"), 0);      // "abc~" > "abc!"
    
    // Test with mixed case and symbols
    EXPECT_EQ(0, AAX::Caseless_strcmp("Hello!", "HELLO!"));  // Case insensitive with symbol
    EXPECT_EQ(0, AAX::Caseless_strcmp("Test@123", "TEST@123")); // Case insensitive with symbol and number
    EXPECT_LT(AAX::Caseless_strcmp("Apple!", "Banana!"), 0); // "Apple!" < "Banana!" (case insensitive)
    EXPECT_GT(AAX::Caseless_strcmp("Zebra!", "Apple!"), 0);  // "Zebra!" > "Apple!" (case insensitive)
    
    // Test with whitespace characters
    EXPECT_EQ(0, AAX::Caseless_strcmp("hello world", "HELLO WORLD")); // Case insensitive with space
    EXPECT_LT(AAX::Caseless_strcmp("hello", "hello "), 0);   // "hello" < "hello " (space at end)
    EXPECT_GT(AAX::Caseless_strcmp("hello ", "hello"), 0);   // "hello " > "hello"
    EXPECT_EQ(0, AAX::Caseless_strcmp("a\tb", "A\tB"));      // Case insensitive with tab
    EXPECT_EQ(0, AAX::Caseless_strcmp("a\nb", "A\nB"));      // Case insensitive with newline
    
    // Test with punctuation
    EXPECT_EQ(0, AAX::Caseless_strcmp("Hello, World!", "HELLO, WORLD!")); // Case insensitive with punctuation
    EXPECT_LT(AAX::Caseless_strcmp("test.", "test?"), 0);     // "test." < "test?" (ASCII order)
    EXPECT_GT(AAX::Caseless_strcmp("test?", "test."), 0);     // "test?" > "test."
    EXPECT_EQ(0, AAX::Caseless_strcmp("C++", "c++"));        // Case insensitive with plus signs
    
    // Test edge cases with different string lengths
    EXPECT_LT(AAX::Caseless_strcmp("A", "AA"), 0);           // "A" < "AA" (shorter)
    EXPECT_GT(AAX::Caseless_strcmp("AA", "A"), 0);           // "AA" > "A" (longer)
    EXPECT_LT(AAX::Caseless_strcmp("test", "testing"), 0);   // "test" < "testing" (prefix)
    EXPECT_GT(AAX::Caseless_strcmp("testing", "test"), 0);   // "testing" > "test"
    
    // Test with Unicode-like characters (high ASCII)
    EXPECT_EQ(0, AAX::Caseless_strcmp("\x80\x81", "\x80\x81")); // High ASCII characters equal
    EXPECT_LT(AAX::Caseless_strcmp("\x80", "\x81"), 0);         // "\x80" < "\x81"
    EXPECT_GT(AAX::Caseless_strcmp("\x81", "\x80"), 0);         // "\x81" > "\x80"
}
