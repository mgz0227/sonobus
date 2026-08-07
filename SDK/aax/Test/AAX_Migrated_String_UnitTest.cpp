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
 *	\file AAX_Migrated_String_UnitTest.cpp
 *
 *	\brief Migrated unit tests for AAX_CString (originally from UnitTest++ framework)
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX_CString.h"
#include <cstring>
#include <string>

//==============================================================================
// Test Fixture
//==============================================================================

class AAX_Migrated_String_Test : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // No specific setup needed for string tests
    }
    
    void TearDown() override
    {
        // No specific cleanup needed for string tests
    }
};

//==============================================================================
// Basic String Tests
//==============================================================================

TEST_F(AAX_Migrated_String_Test, UninitializedStringValue)
{
    AAX_CString inputString;
    EXPECT_STREQ("", inputString.CString());
}

TEST_F(AAX_Migrated_String_Test, InitializedStringValue)
{
    AAX_CString inputString("Howdy");
    EXPECT_STREQ("Howdy", inputString.CString());
    AAX_CString copyString(inputString);
    EXPECT_STREQ("Howdy", copyString.CString());
}

TEST_F(AAX_Migrated_String_Test, EqualityOperator)
{
    AAX_CString inputString("There");
    EXPECT_STREQ("There", inputString.CString());
    inputString.Erase(4, 1);
    EXPECT_NE(0, strcmp("There", inputString.CString()));
}

TEST_F(AAX_Migrated_String_Test, InequalityOperator)
{
    AAX_CString inputString("There");
    EXPECT_TRUE(AAX_CString("Here") != inputString);
    EXPECT_FALSE(AAX_CString("There") != inputString);
}

TEST_F(AAX_Migrated_String_Test, AssignmentOperator)
{
    AAX_CString inputString;
    inputString = "cString";
    EXPECT_STREQ("cString", inputString.CString());
    inputString = AAX_CString("string");
    EXPECT_STREQ("string", inputString.CString());
}

TEST_F(AAX_Migrated_String_Test, Length)
{
    AAX_CString inputString("Howdy");
    EXPECT_EQ(uint32_t(5), inputString.Length());
    inputString = "";
    EXPECT_EQ(uint32_t(0), inputString.Length());
}

TEST_F(AAX_Migrated_String_Test, MaxLength)
{
    AAX_CString inputString("HelloWorld");
    EXPECT_EQ(static_cast<uint32_t>(-2), inputString.MaxLength());
}

TEST_F(AAX_Migrated_String_Test, Clear)
{
    AAX_CString inputString("HelloWorld");
    EXPECT_EQ(uint32_t(10), inputString.Length());
    inputString.Clear();
    EXPECT_EQ(uint32_t(0), inputString.Length());
    EXPECT_STREQ("", inputString.CString());
}

TEST_F(AAX_Migrated_String_Test, Empty)
{
    AAX_CString inputString;
    EXPECT_TRUE(inputString.Empty());
    inputString = "A";
    EXPECT_FALSE(inputString.Empty());
    inputString.Clear();
    EXPECT_TRUE(inputString.Empty());
}

//==============================================================================
// String Manipulation Tests
//==============================================================================

TEST_F(AAX_Migrated_String_Test, Append)
{
    AAX_CString inputString;
    inputString.Append(AAX_CString("AB"));
    EXPECT_STREQ("AB", inputString.CString());
    inputString.Append("CD");
    EXPECT_STREQ("ABCD", inputString.CString());
    inputString.AppendNumber(10);
    EXPECT_STREQ("ABCD10", inputString.CString());
    inputString.AppendNumber(5.5, 1);
    EXPECT_STREQ("ABCD105.5", inputString.CString());
}

TEST_F(AAX_Migrated_String_Test, Insert)
{
    AAX_CString inputString("AAAA");
    inputString.Insert(1, AAX_CString("B"));
    EXPECT_STREQ("ABAAA", inputString.CString());
    inputString.Insert(3, "C");
    EXPECT_STREQ("ABACAA", inputString.CString());
    inputString.InsertNumber(5, 5);
    EXPECT_STREQ("ABACA5A", inputString.CString());
    inputString.InsertNumber(7, 5.5, 1);
    EXPECT_STREQ("ABACA5A5.5", inputString.CString());
}

TEST_F(AAX_Migrated_String_Test, Replace)
{
    AAX_CString inputString("ABABA");
    inputString.Replace(2, 2, AAX_CString("CC"));
    EXPECT_STREQ("ABCCA", inputString.CString());
    inputString.Replace(3, 2, "DD");
    EXPECT_STREQ("ABCDD", inputString.CString());
    inputString.Replace(4, 1, AAX_CString("E"));
    EXPECT_STREQ("ABCDE", inputString.CString());
}

TEST_F(AAX_Migrated_String_Test, Erase)
{
    AAX_CString inputString("0123456789");
    inputString.Erase(5, 3);
    EXPECT_EQ(uint32_t(7), inputString.Length());
    EXPECT_STREQ("0123489", inputString.CString());
    
    AAX_CString shortString("01234");
    shortString.Erase(0, 10);
    EXPECT_STREQ("", shortString.CString());
    
    AAX_CString shortString2("012345");
    shortString2.Erase(3, 10);
    EXPECT_STREQ("012", shortString2.CString());
}

//==============================================================================
// Comparison Operator Tests
//==============================================================================

TEST_F(AAX_Migrated_String_Test, LessThanOperator)
{
    AAX_CString str1("A");
    AAX_CString str2("B");
    AAX_CString str3("C");
    EXPECT_TRUE(str1 < str2);
    EXPECT_TRUE(str2 < str3);
    EXPECT_TRUE(AAX_CString() < str1);
}

TEST_F(AAX_Migrated_String_Test, GreaterThanOperator)
{
    AAX_CString str1("A");
    AAX_CString str2("B");
    AAX_CString str3("C");
    EXPECT_TRUE(str3 > str2);
    EXPECT_TRUE(str2 > str1);
    EXPECT_TRUE(str1 > AAX_CString());
}

//==============================================================================
// Advanced String Operations
//==============================================================================

TEST_F(AAX_Migrated_String_Test, SubStr)
{
    AAX_CString inputString("HowdyThere");
    AAX_CString howdy;
    inputString.SubString(0, 5, &howdy);
    AAX_CString there;
    inputString.SubString(5, 5, &there);
    EXPECT_STREQ("Howdy", howdy.CString());
    EXPECT_STREQ("There", there.CString());
}

TEST_F(AAX_Migrated_String_Test, PlusEqualOperator)
{
    AAX_CString howdy("Howdy");
    AAX_CString there("There");
    EXPECT_STREQ("Howdy", howdy.CString());
    EXPECT_STREQ("There", there.CString());
    
    AAX_CString howdyThere = howdy;
    EXPECT_STREQ("Howdy", howdyThere.CString());
    howdyThere += there;
    EXPECT_STREQ("HowdyThere", howdyThere.CString());
    
    howdyThere.Clear();
    howdyThere = howdy;
    howdyThere += "There"; // const char* version
    EXPECT_STREQ("HowdyThere", howdyThere.CString());
}

TEST_F(AAX_Migrated_String_Test, StdString)
{
    std::string stdAStr("AString");
    std::string stdBStr("BString");
    
    AAX_CString aStr(stdAStr);
    AAX_CString bStr(stdBStr);
    EXPECT_EQ(aStr.StdString(), stdAStr);
    EXPECT_EQ(bStr.StdString(), stdBStr);
    
    std::string a = aStr.StdString();
    const std::string b = bStr.StdString();
    EXPECT_EQ(a, stdAStr);
    EXPECT_EQ(b, stdBStr);
    
    bStr = aStr;
    EXPECT_EQ(aStr.StdString(), bStr.StdString());
}
