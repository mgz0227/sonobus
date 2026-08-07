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
 *	\file AAX_CString_UnitTest.cpp
 *
 *	\brief Unit tests for AAX_CString class
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX_CString.h"
#include "AAX_IString.h"
#include <string>
#include <sstream>
#include <limits>

//--------------------------------------------------------------------
#pragma mark - Test fixture

class AAX_CString_Test : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
    
    // Helper function to create test strings
    std::string CreateLongString(size_t length) {
        std::string result;
        result.reserve(length);
        for (size_t i = 0; i < length; ++i) {
            result += static_cast<char>('A' + (i % 26));
        }
        return result;
    }
};

//--------------------------------------------------------------------
#pragma mark - Constructor Tests

TEST_F(AAX_CString_Test, DefaultConstructor)
{
    AAX_CString str;
    EXPECT_EQ(0U, str.Length());
    EXPECT_TRUE(str.Empty());
    EXPECT_STREQ("", str.Get());
    EXPECT_STREQ("", str.CString());
}

TEST_F(AAX_CString_Test, CStringConstructor)
{
    const char* testStr = "Hello World";
    AAX_CString str(testStr);
    
    EXPECT_EQ(strlen(testStr), str.Length());
    EXPECT_FALSE(str.Empty());
    EXPECT_STREQ(testStr, str.Get());
    EXPECT_STREQ(testStr, str.CString());
}

TEST_F(AAX_CString_Test, CStringConstructorNull)
{
    AAX_CString str(nullptr);
    EXPECT_EQ(0U, str.Length());
    EXPECT_TRUE(str.Empty());
    EXPECT_STREQ("", str.Get());
}

TEST_F(AAX_CString_Test, StdStringConstructor)
{
    std::string testStr = "Test String";
    AAX_CString str(testStr);
    
    EXPECT_EQ(testStr.length(), str.Length());
    EXPECT_FALSE(str.Empty());
    EXPECT_STREQ(testStr.c_str(), str.Get());
}

TEST_F(AAX_CString_Test, CopyConstructor)
{
    AAX_CString original("Original String");
    AAX_CString copy(original);
    
    EXPECT_EQ(original.Length(), copy.Length());
    EXPECT_STREQ(original.Get(), copy.Get());
    EXPECT_FALSE(copy.Empty());
}

TEST_F(AAX_CString_Test, IStringConstructor)
{
    AAX_CString original("Interface String");
    AAX_IString& interface = original;
    AAX_CString fromInterface(interface);
    
    EXPECT_EQ(original.Length(), fromInterface.Length());
    EXPECT_STREQ(original.Get(), fromInterface.Get());
}

//--------------------------------------------------------------------
#pragma mark - Assignment Operator Tests

TEST_F(AAX_CString_Test, AssignmentFromCString)
{
    AAX_CString str;
    const char* testStr = "Assigned String";
    
    str = testStr;
    EXPECT_EQ(strlen(testStr), str.Length());
    EXPECT_STREQ(testStr, str.Get());
}

TEST_F(AAX_CString_Test, AssignmentFromStdString)
{
    AAX_CString str;
    std::string testStr = "Standard String";
    
    str = testStr;
    EXPECT_EQ(testStr.length(), str.Length());
    EXPECT_STREQ(testStr.c_str(), str.Get());
}

TEST_F(AAX_CString_Test, AssignmentFromAAXCString)
{
    AAX_CString original("Original");
    AAX_CString target;
    
    target = original;
    EXPECT_EQ(original.Length(), target.Length());
    EXPECT_STREQ(original.Get(), target.Get());
}

TEST_F(AAX_CString_Test, AssignmentFromIString)
{
    AAX_CString original("Interface Assignment");
    AAX_IString& interface = original;
    AAX_CString target;
    
    target = interface;
    EXPECT_EQ(original.Length(), target.Length());
    EXPECT_STREQ(original.Get(), target.Get());
}

//--------------------------------------------------------------------
#pragma mark - String Manipulation Tests

TEST_F(AAX_CString_Test, Clear)
{
    AAX_CString str("Clear Me");
    EXPECT_FALSE(str.Empty());
    
    str.Clear();
    EXPECT_TRUE(str.Empty());
    EXPECT_EQ(0U, str.Length());
    EXPECT_STREQ("", str.Get());
}

TEST_F(AAX_CString_Test, Append)
{
    AAX_CString str("Hello");
    AAX_CString append(" World");
    
    str.Append(append);
    EXPECT_STREQ("Hello World", str.Get());
    
    str.Append(" Again");
    EXPECT_STREQ("Hello World Again", str.Get());
}

TEST_F(AAX_CString_Test, AppendNumber)
{
    AAX_CString str("Value: ");

    str.AppendNumber(42);
    EXPECT_STREQ("Value: 42", str.Get());

    str.Clear();
    str.Append("Pi: ");
    str.AppendNumber(3.14159, 3);
    EXPECT_STREQ("Pi: 3.142", str.Get()); // Should format with 3 decimal places
}

TEST_F(AAX_CString_Test, AppendHex)
{
    AAX_CString str("Hex: ");
    
    str.AppendHex(255, 4);
    EXPECT_TRUE(strstr(str.Get(), "ff") != nullptr || strstr(str.Get(), "FF") != nullptr);
}

TEST_F(AAX_CString_Test, Insert)
{
    AAX_CString str("Hello World");
    AAX_CString insert("Beautiful ");
    
    str.Insert(6, insert);
    EXPECT_STREQ("Hello Beautiful World", str.Get());
    
    str.Insert(0, "Oh ");
    EXPECT_STREQ("Oh Hello Beautiful World", str.Get());
}

TEST_F(AAX_CString_Test, InsertNumber)
{
    AAX_CString str("Value  is good");
    
    str.InsertNumber(6, 123);
    EXPECT_STREQ("Value 123 is good", str.Get());
}

TEST_F(AAX_CString_Test, Replace)
{
    AAX_CString str("Hello World");
    AAX_CString replacement("Universe");
    
    str.Replace(6, 5, replacement);
    EXPECT_STREQ("Hello Universe", str.Get());
}

TEST_F(AAX_CString_Test, Erase)
{
    AAX_CString str("Hello Beautiful World");
    
    str.Erase(6, 10); // Remove "Beautiful "
    EXPECT_STREQ("Hello World", str.Get());
}

//--------------------------------------------------------------------
#pragma mark - Search Tests

TEST_F(AAX_CString_Test, FindFirst)
{
    AAX_CString str("Hello World Hello");

    EXPECT_EQ(0U, str.FindFirst("Hello"));
    EXPECT_EQ(6U, str.FindFirst("World"));
    EXPECT_EQ(AAX_CString::kInvalidIndex, str.FindFirst("NotFound"));

    EXPECT_EQ(4U, str.FindFirst('o'));  // First 'o' in "Hello"
    EXPECT_EQ(AAX_CString::kInvalidIndex, str.FindFirst('z'));
}

TEST_F(AAX_CString_Test, FindLast)
{
    AAX_CString str("Hello World Hello");

    EXPECT_EQ(12U, str.FindLast("Hello"));
    EXPECT_EQ(6U, str.FindLast("World"));
    EXPECT_EQ(AAX_CString::kInvalidIndex, str.FindLast("NotFound"));

    EXPECT_EQ(16U, str.FindLast('o'));  // Last 'o' in the second "Hello"
    EXPECT_EQ(AAX_CString::kInvalidIndex, str.FindLast('z'));
}

//--------------------------------------------------------------------
#pragma mark - Conversion Tests

TEST_F(AAX_CString_Test, ToDouble)
{
    AAX_CString str("3.14159");
    double value;
    
    EXPECT_TRUE(str.ToDouble(&value));
    EXPECT_NEAR(3.14159, value, 0.00001);
    
    str = "not a number";
    EXPECT_FALSE(str.ToDouble(&value));
    
    str = "";
    EXPECT_FALSE(str.ToDouble(&value));
}

TEST_F(AAX_CString_Test, ToInteger)
{
    AAX_CString str("42");
    int32_t value;
    
    EXPECT_TRUE(str.ToInteger(&value));
    EXPECT_EQ(42, value);
    
    str = "-123";
    EXPECT_TRUE(str.ToInteger(&value));
    EXPECT_EQ(-123, value);
    
    str = "not a number";
    EXPECT_FALSE(str.ToInteger(&value));
    
    str = "";
    EXPECT_FALSE(str.ToInteger(&value));
}

//--------------------------------------------------------------------
#pragma mark - Comparison Tests

TEST_F(AAX_CString_Test, EqualityOperators)
{
    AAX_CString str1("Test");
    AAX_CString str2("Test");
    AAX_CString str3("Different");
    
    EXPECT_TRUE(str1 == str2);
    EXPECT_FALSE(str1 == str3);
    EXPECT_FALSE(str1 != str2);
    EXPECT_TRUE(str1 != str3);
    
    EXPECT_TRUE(str1 == "Test");
    EXPECT_FALSE(str1 == "Different");
    EXPECT_FALSE(str1 != "Test");
    EXPECT_TRUE(str1 != "Different");
    
    std::string stdStr = "Test";
    EXPECT_TRUE(str1 == stdStr);
    EXPECT_FALSE(str1 != stdStr);
}

TEST_F(AAX_CString_Test, ComparisonOperators)
{
    AAX_CString str1("Apple");
    AAX_CString str2("Banana");
    
    EXPECT_TRUE(str1 < str2);
    EXPECT_FALSE(str2 < str1);
    EXPECT_TRUE(str2 > str1);
    EXPECT_FALSE(str1 > str2);
}

TEST_F(AAX_CString_Test, EqualsMethod)
{
    AAX_CString str("Test");

    EXPECT_TRUE(str.Equals(AAX_CString("Test")));
    EXPECT_FALSE(str.Equals(AAX_CString("Different")));
    EXPECT_TRUE(str.Equals("Test"));
    EXPECT_FALSE(str.Equals("Different"));
    EXPECT_TRUE(str.Equals(std::string("Test")));
    EXPECT_FALSE(str.Equals(std::string("Different")));
}

//--------------------------------------------------------------------
#pragma mark - Access Tests

TEST_F(AAX_CString_Test, IndexOperator)
{
    AAX_CString str("Hello");

    EXPECT_EQ('H', str[0]);
    EXPECT_EQ('e', str[1]);
    EXPECT_EQ('o', str[4]);
}

TEST_F(AAX_CString_Test, SubString)
{
    AAX_CString str("Hello World");
    AAX_CString result;

    str.SubString(0, 5, &result);
    EXPECT_STREQ("Hello", result.Get());

    str.SubString(6, 5, &result);
    EXPECT_STREQ("World", result.Get());

    str.SubString(6, 100, &result); // Beyond end
    EXPECT_STREQ("World", result.Get());
}

TEST_F(AAX_CString_Test, StdStringAccess)
{
    AAX_CString str("Test String");

    const std::string& constRef = str.StdString();
    EXPECT_EQ("Test String", constRef);

    std::string& ref = str.StdString();
    ref += " Modified";
    EXPECT_STREQ("Test String Modified", str.Get());
}

//--------------------------------------------------------------------
#pragma mark - Stream Operator Tests

TEST_F(AAX_CString_Test, OutputStreamOperator)
{
    AAX_CString str("Stream Test");
    std::ostringstream oss;

    oss << str;
    EXPECT_EQ("Stream Test", oss.str());
}

TEST_F(AAX_CString_Test, InputStreamOperator)
{
    std::istringstream iss("InputTest");
    AAX_CString str;

    iss >> str;
    EXPECT_STREQ("InputTest", str.Get());
}

//--------------------------------------------------------------------
#pragma mark - Edge Case Tests

TEST_F(AAX_CString_Test, EmptyStringOperations)
{
    AAX_CString empty;
    AAX_CString target;

    // Operations on empty string
    EXPECT_EQ(AAX_CString::kInvalidIndex, empty.FindFirst("test"));
    EXPECT_EQ(AAX_CString::kInvalidIndex, empty.FindLast("test"));

    empty.Append("test");
    EXPECT_STREQ("test", empty.Get());

    empty.Clear();
    empty.Insert(0, "inserted");
    EXPECT_STREQ("inserted", empty.Get());
}

TEST_F(AAX_CString_Test, LargeStringHandling)
{
    std::string largeStr = CreateLongString(1000);
    AAX_CString str(largeStr);

    EXPECT_EQ(1000U, str.Length());
    EXPECT_EQ(largeStr, str.StdString());

    // Test operations on large string
    str.Append(" suffix");
    EXPECT_EQ(1007U, str.Length());
    EXPECT_TRUE(str.StdString().find(" suffix") != std::string::npos);
}

TEST_F(AAX_CString_Test, BoundaryConditions)
{
    AAX_CString str("Test");

    // Insert at boundaries
    str.Insert(0, "Start");
    EXPECT_STREQ("StartTest", str.Get());

    str.Insert(str.Length(), "End");
    EXPECT_STREQ("StartTestEnd", str.Get());

    // Erase at boundaries
    str.Erase(0, 5); // Remove "Start"
    EXPECT_STREQ("TestEnd", str.Get());

    str.Erase(4, 100); // Remove beyond end
    EXPECT_STREQ("Test", str.Get());
}

TEST_F(AAX_CString_Test, MaxLengthHandling)
{
    AAX_CString str;
    EXPECT_EQ(AAX_CString::kMaxStringLength, str.MaxLength());
}

//--------------------------------------------------------------------
#pragma mark - Move Semantics Tests

TEST_F(AAX_CString_Test, MoveAssignment)
{
    AAX_CString source("Move Source");
    AAX_CString target;

    target = std::move(source);
    EXPECT_STREQ("Move Source", target.Get());
    // Note: source state after move is implementation-defined
}

//--------------------------------------------------------------------
#pragma mark - Interface Compliance Tests

TEST_F(AAX_CString_Test, IStringInterface)
{
    AAX_CString str("Interface Test");
    AAX_IString* interface = &str;

    EXPECT_EQ(str.Length(), interface->Length());
    EXPECT_EQ(str.MaxLength(), interface->MaxLength());
    EXPECT_STREQ(str.Get(), interface->Get());

    interface->Set("New Value");
    EXPECT_STREQ("New Value", str.Get());

    AAX_CString other("Other");
    *interface = other;
    EXPECT_STREQ("Other", str.Get());

    *interface = "Direct Assignment";
    EXPECT_STREQ("Direct Assignment", str.Get());
}
