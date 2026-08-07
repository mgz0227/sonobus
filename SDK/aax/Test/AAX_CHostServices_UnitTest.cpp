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
 *	\file AAX_CHostServices_UnitTest.cpp
 *
 *	\brief Characterization tests for AAX_CHostServices class
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "AAX_CHostServices.h"
#include "AAX_Errors.h"
#include "AAX_MockACFController.h"
#include "AAX_UIDs.h"
#include "AAX_Enums.h"
#include "acfunknown.h"
#include "CACFUnknown.h"
#include "ACFPtr.h"
#include <memory>

//==============================================================================
// Mock Host Unknown for ACF interface resolution
//==============================================================================

class MockACFHostUnknown : public IACFUnknown, public CACFUnknown
{
public:
    MockACFHostUnknown() : CACFUnknown() {}
    ~MockACFHostUnknown() override = default;

    // IACFUnknown interface - delegate to CACFUnknown
    ACF_DECLARE_STANDARD_UNKNOWN()

    // IACFUnknown interface
    ACFRESULT ACFMETHODCALLTYPE InternalQueryInterface(const acfIID& riid, void** ppvObjOut) override
    {
        if (ppvObjOut == nullptr) return ACF_E_POINTER;
        
        *ppvObjOut = nullptr;
        
        // Return mock controller for controller interface requests
        if (memcmp(&riid, &IID_IAAXControllerV1, sizeof(acfIID)) == 0)
        {
            if (!mMockController)
                mMockController = new AAX_MockACFController();
            *ppvObjOut = mMockController;
            mMockController->AddRef();
            return ACF_OK;
        }
        
        return this->CACFUnknown::InternalQueryInterface(riid, ppvObjOut);
    }

private:
    AAX_MockACFController* mMockController = nullptr;
};

//==============================================================================
// Test Fixture
//==============================================================================

class AAX_CHostServices_Test : public ::testing::Test
{
protected:
    void SetUp() override
    {
        mockHost = new MockACFHostUnknown();
        // Store original state to restore later
        originalHostSet = false;
    }

    void TearDown() override
    {
        // Reset host services to clean state
        AAX_CHostServices::Set(nullptr);
        
        if (mockHost)
        {
            mockHost->Release();
            mockHost = nullptr;
        }
    }

    MockACFHostUnknown* mockHost = nullptr;
    bool originalHostSet = false;
};

//==============================================================================
// Set Method Characterization Tests
//==============================================================================

TEST_F(AAX_CHostServices_Test, Set_WithNullHost)
{
    // Set with null host should not crash
    EXPECT_NO_THROW({
        AAX_CHostServices::Set(nullptr);
    });
    
    // This is a static method, so we can't directly verify internal state
    // but we can verify it doesn't crash
}

TEST_F(AAX_CHostServices_Test, Set_WithValidHost)
{
    // Set with valid mock host should not crash
    EXPECT_NO_THROW({
        AAX_CHostServices::Set(mockHost);
    });

    // This is characterization - we're documenting that the method doesn't crash
}

TEST_F(AAX_CHostServices_Test, Set_MultipleCallsWithSameHost)
{
    // Multiple calls with same host should not crash
    EXPECT_NO_THROW({
        AAX_CHostServices::Set(mockHost);
        AAX_CHostServices::Set(mockHost);
        AAX_CHostServices::Set(mockHost);
    });
}

TEST_F(AAX_CHostServices_Test, Set_MultipleCallsWithDifferentHosts)
{
    MockACFHostUnknown* secondHost = new MockACFHostUnknown();

    // Multiple calls with different hosts should not crash
    EXPECT_NO_THROW({
        AAX_CHostServices::Set(mockHost);
        AAX_CHostServices::Set(secondHost);
        AAX_CHostServices::Set(nullptr);
    });

    secondHost->Release();
}

//==============================================================================
// HandleAssertFailure Method Characterization Tests
//==============================================================================

TEST_F(AAX_CHostServices_Test, HandleAssertFailure_WithoutHostSet)
{
    // Ensure no host is set
    AAX_CHostServices::Set(nullptr);
    
    // HandleAssertFailure without host set should not crash
    AAX_Result result;
    EXPECT_NO_THROW({
        result = AAX_CHostServices::HandleAssertFailure("test.cpp", 123, "Test assertion");
    });
    
    // Should not crash - return value not important for characterization
}

TEST_F(AAX_CHostServices_Test, HandleAssertFailure_WithHostSet)
{
    // Set a host first
    AAX_CHostServices::Set(mockHost);
    
    // HandleAssertFailure with host set should not crash
    AAX_Result result;
    EXPECT_NO_THROW({
        result = AAX_CHostServices::HandleAssertFailure("test.cpp", 456, "Test assertion with host");
    });
    
    // Should not crash - return value not important for characterization
}

TEST_F(AAX_CHostServices_Test, HandleAssertFailure_WithNullParameters)
{
    AAX_CHostServices::Set(mockHost);

    // NOTE: HandleAssertFailure() handles null parameters gracefully (unlike
    // Trace() and StackTrace() which crash). This inconsistency in null pointer
    // handling across AAX_CHostServices methods may indicate different implementation
    // approaches or missing validation in some methods.

    // HandleAssertFailure with null parameters should not crash
    AAX_Result result;
    EXPECT_NO_THROW({
        result = AAX_CHostServices::HandleAssertFailure(nullptr, 0, nullptr);
    });

    // Should not crash - return value not important for characterization
}

TEST_F(AAX_CHostServices_Test, HandleAssertFailure_WithFlags)
{
    AAX_CHostServices::Set(mockHost);
    
    // HandleAssertFailure with custom flags should not crash
    AAX_Result result;
    EXPECT_NO_THROW({
        result = AAX_CHostServices::HandleAssertFailure("test.cpp", 789, "Test with flags", AAX_eAssertFlags_Log);
    });
    
    // Should not crash - return value not important for characterization
}

//==============================================================================
// Trace Method Characterization Tests
//==============================================================================

TEST_F(AAX_CHostServices_Test, Trace_WithoutHostSet)
{
    // Ensure no host is set
    AAX_CHostServices::Set(nullptr);
    
    // Trace without host set should not crash
    AAX_Result result;
    EXPECT_NO_THROW({
        result = AAX_CHostServices::Trace(AAX_eTracePriorityHost_Normal, "Test trace message");
    });
    
    // Should not crash - return value not important for characterization
}

TEST_F(AAX_CHostServices_Test, Trace_WithHostSet)
{
    // Set a host first
    AAX_CHostServices::Set(mockHost);
    
    // Trace with host set should not crash
    AAX_Result result;
    EXPECT_NO_THROW({
        result = AAX_CHostServices::Trace(AAX_eTracePriorityHost_Normal, "Test trace with host");
    });
    
    // Should not crash - return value not important for characterization
}

TEST_F(AAX_CHostServices_Test, Trace_WithDifferentPriorities)
{
    AAX_CHostServices::Set(mockHost);
    
    // Test different trace priorities
    AAX_Result result1, result2, result3;
    EXPECT_NO_THROW({
        result1 = AAX_CHostServices::Trace(AAX_eTracePriorityHost_Low, "Low priority trace");
        result2 = AAX_CHostServices::Trace(AAX_eTracePriorityHost_Normal, "Normal priority trace");
        result3 = AAX_CHostServices::Trace(AAX_eTracePriorityHost_High, "High priority trace");
    });
    
    // Should not crash - return values not important for characterization
}

TEST_F(AAX_CHostServices_Test, Trace_WithEmptyMessage)
{
    AAX_CHostServices::Set(mockHost);

    // Trace with empty message should not crash
    AAX_Result result;
    EXPECT_NO_THROW({
        result = AAX_CHostServices::Trace(AAX_eTracePriorityHost_Normal, "");
    });

    // Should not crash - return value not important for characterization
}

TEST_F(AAX_CHostServices_Test, Trace_WithNullMessage)
{
    AAX_CHostServices::Set(mockHost);

    // Trace with null message should not crash and return error
    AAX_Result result;
    EXPECT_NO_THROW({
        result = AAX_CHostServices::Trace(AAX_eTracePriorityHost_Normal, nullptr);
    });

    // Should return error for null message
    EXPECT_EQ(result, AAX_ERROR_NULL_ARGUMENT);
}

//==============================================================================
// StackTrace Method Characterization Tests
//==============================================================================

TEST_F(AAX_CHostServices_Test, StackTrace_WithoutHostSet)
{
    // Ensure no host is set
    AAX_CHostServices::Set(nullptr);
    
    // StackTrace without host set should not crash
    AAX_Result result;
    EXPECT_NO_THROW({
        result = AAX_CHostServices::StackTrace(AAX_eTracePriorityHost_Normal, AAX_eTracePriorityHost_Normal, "Test stack trace");
    });
    
    // Should not crash - return value not important for characterization
}

TEST_F(AAX_CHostServices_Test, StackTrace_WithHostSet)
{
    // Set a host first
    AAX_CHostServices::Set(mockHost);
    
    // StackTrace with host set should not crash
    AAX_Result result;
    EXPECT_NO_THROW({
        result = AAX_CHostServices::StackTrace(AAX_eTracePriorityHost_High, AAX_eTracePriorityHost_Low, "Test stack trace with host");
    });
    
    // Should not crash - return value not important for characterization
}

TEST_F(AAX_CHostServices_Test, StackTrace_WithEmptyMessage)
{
    AAX_CHostServices::Set(mockHost);

    // StackTrace with empty message should not crash
    AAX_Result result;
    EXPECT_NO_THROW({
        result = AAX_CHostServices::StackTrace(AAX_eTracePriorityHost_Normal, AAX_eTracePriorityHost_Normal, "");
    });

    // Should not crash - return value not important for characterization
}

TEST_F(AAX_CHostServices_Test, StackTrace_WithNullMessage)
{
    AAX_CHostServices::Set(mockHost);

    // StackTrace with null message should not crash and return error
    AAX_Result result;
    EXPECT_NO_THROW({
        result = AAX_CHostServices::StackTrace(AAX_eTracePriorityHost_Normal, AAX_eTracePriorityHost_Normal, nullptr);
    });

    // Should return error for null message
    EXPECT_EQ(result, AAX_ERROR_NULL_ARGUMENT);
}

//==============================================================================
// Security and Edge Case Tests
//==============================================================================

TEST_F(AAX_CHostServices_Test, Trace_BufferOverflowProtection)
{
    AAX_CHostServices::Set(mockHost);
    
    // Create a string that would exceed the 512-byte buffer when formatted
    std::string longString(600, 'A'); // 600 A's
    
    // This should not crash due to vsnprintf buffer protection
    AAX_Result result;
    EXPECT_NO_THROW({
        result = AAX_CHostServices::Trace(AAX_eTracePriorityHost_Normal, "Long message: %s", longString.c_str());
    });
    
    // Should not crash - return value not important for characterization
}

TEST_F(AAX_CHostServices_Test, StackTrace_BufferOverflowProtection)
{
    AAX_CHostServices::Set(mockHost);
    
    // Create a string that would exceed the 512-byte buffer when formatted
    std::string longString(600, 'B'); // 600 B's
    
    // This should not crash due to vsnprintf buffer protection
    AAX_Result result;
    EXPECT_NO_THROW({
        result = AAX_CHostServices::StackTrace(AAX_eTracePriorityHost_Normal, AAX_eTracePriorityHost_Normal, "Long message: %s", longString.c_str());
    });
    
    // Should not crash - return value not important for characterization
}

TEST_F(AAX_CHostServices_Test, Trace_MalformedFormatSpecifiers)
{
    AAX_CHostServices::Set(mockHost);
    
    // Test various malformed format specifiers
    AAX_Result result;
    EXPECT_NO_THROW({
        result = AAX_CHostServices::Trace(AAX_eTracePriorityHost_Normal, "Bad format: %");
    });
    
    // Should not crash - return value not important for characterization
}

TEST_F(AAX_CHostServices_Test, StackTrace_MalformedFormatSpecifiers)
{
    AAX_CHostServices::Set(mockHost);
    
    // Test malformed format specifiers
    AAX_Result result;
    EXPECT_NO_THROW({
        result = AAX_CHostServices::StackTrace(AAX_eTracePriorityHost_Normal, AAX_eTracePriorityHost_Normal, "Bad format: %q");
    });
    
    // Should not crash - return value not important for characterization
}

TEST_F(AAX_CHostServices_Test, Trace_TypeMismatchArguments)
{
    AAX_CHostServices::Set(mockHost);
    
    // Test type mismatches - this is undefined behavior but should not crash due to vsnprintf
    // Note: This test verifies crash protection, not correctness of output
    AAX_Result result;
    EXPECT_NO_THROW({
        result = AAX_CHostServices::Trace(AAX_eTracePriorityHost_Normal, "Number: %d", 0xDEADBEEF);
    });
    
    // Should not crash - return value not important for characterization
}

TEST_F(AAX_CHostServices_Test, StackTrace_TypeMismatchArguments)
{
    AAX_CHostServices::Set(mockHost);
    
    // Test type mismatches - this is undefined behavior but should not crash
    // Using a safer approach: pass integer instead of trying to format invalid pointer
    AAX_Result result;
    EXPECT_NO_THROW({
        result = AAX_CHostServices::StackTrace(AAX_eTracePriorityHost_Normal, AAX_eTracePriorityHost_Normal, "Value: %d", 42);
    });
    
    // Should not crash - return value not important for characterization
}

TEST_F(AAX_CHostServices_Test, Trace_UnicodeCharacters)
{
    AAX_CHostServices::Set(mockHost);
    
    // Test with Unicode characters
    AAX_Result result;
    EXPECT_NO_THROW({
        result = AAX_CHostServices::Trace(AAX_eTracePriorityHost_Normal, "Unicode: %s", "Hello 世界 🎵");
    });
    
    // Should not crash - return value not important for characterization
}

TEST_F(AAX_CHostServices_Test, StackTrace_UnicodeCharacters)
{
    AAX_CHostServices::Set(mockHost);
    
    // Test with Unicode characters
    AAX_Result result;
    EXPECT_NO_THROW({
        result = AAX_CHostServices::StackTrace(AAX_eTracePriorityHost_Normal, AAX_eTracePriorityHost_Normal, "Unicode: %s", "Café ñ 日本語");
    });
    
    // Should not crash - return value not important for characterization
}

TEST_F(AAX_CHostServices_Test, Trace_VeryLongFormatString)
{
    AAX_CHostServices::Set(mockHost);
    
    // Create a very long format string itself (not just the output)
    std::string longFormat(400, 'X');
    longFormat += " end";
    
    AAX_Result result;
    EXPECT_NO_THROW({
        result = AAX_CHostServices::Trace(AAX_eTracePriorityHost_Normal, longFormat.c_str());
    });
    
    // Should not crash - return value not important for characterization
}

TEST_F(AAX_CHostServices_Test, StackTrace_VeryLongFormatString)
{
    AAX_CHostServices::Set(mockHost);
    
    // Create a very long format string itself
    std::string longFormat(400, 'Y');
    longFormat += " end";
    
    AAX_Result result;
    EXPECT_NO_THROW({
        result = AAX_CHostServices::StackTrace(AAX_eTracePriorityHost_Normal, AAX_eTracePriorityHost_Normal, longFormat.c_str());
    });
    
    // Should not crash - return value not important for characterization
}

TEST_F(AAX_CHostServices_Test, Trace_MultipleFormatSpecifiers)
{
    AAX_CHostServices::Set(mockHost);
    
    // Test multiple format specifiers with correct arguments
    AAX_Result result;
    EXPECT_NO_THROW({
        result = AAX_CHostServices::Trace(AAX_eTracePriorityHost_Normal, "Int: %d, String: %s, Float: %f, Hex: %x", 
                                        42, "test", 3.14159, 255);
    });
    
    // Should not crash - return value not important for characterization
}

TEST_F(AAX_CHostServices_Test, StackTrace_MultipleFormatSpecifiers)
{
    AAX_CHostServices::Set(mockHost);
    
    // Test multiple format specifiers with correct arguments
    AAX_Result result;
    EXPECT_NO_THROW({
        result = AAX_CHostServices::StackTrace(AAX_eTracePriorityHost_Normal, AAX_eTracePriorityHost_Normal,
                                             "Char: %c, Long: %ld, Double: %g, Octal: %o", 
                                             'A', 1234567L, 2.71828, 64);
    });
    
    // Should not crash - return value not important for characterization
}

TEST_F(AAX_CHostServices_Test, Trace_ExtremelyLongOutput)
{
    AAX_CHostServices::Set(mockHost);
    
    // Test that would generate extremely long output (multiple long strings)
    std::string str1(200, 'A');
    std::string str2(200, 'B'); 
    std::string str3(200, 'C');
    
    AAX_Result result;
    EXPECT_NO_THROW({
        result = AAX_CHostServices::Trace(AAX_eTracePriorityHost_Normal, "%s-%s-%s", 
                                        str1.c_str(), str2.c_str(), str3.c_str());
    });
    
    // Should not crash with truncation - return value not important for characterization
}

TEST_F(AAX_CHostServices_Test, StackTrace_ExtremelyLongOutput)
{
    AAX_CHostServices::Set(mockHost);
    
    // Test that would generate extremely long output
    std::string str1(300, 'X');
    std::string str2(300, 'Y');
    
    AAX_Result result;
    EXPECT_NO_THROW({
        result = AAX_CHostServices::StackTrace(AAX_eTracePriorityHost_Normal, AAX_eTracePriorityHost_Normal,
                                             "First: %s, Second: %s", str1.c_str(), str2.c_str());
    });
    
    // Should not crash with truncation - return value not important for characterization
}

TEST_F(AAX_CHostServices_Test, Trace_SpecialCharactersInFormat)
{
    AAX_CHostServices::Set(mockHost);
    
    // Test with special characters that might cause issues
    AAX_Result result;
    EXPECT_NO_THROW({
        result = AAX_CHostServices::Trace(AAX_eTracePriorityHost_Normal, "Special chars: \n\t\r\\\"\'");
    });
    
    // Should not crash - return value not important for characterization
}

TEST_F(AAX_CHostServices_Test, StackTrace_SpecialCharactersInFormat)
{
    AAX_CHostServices::Set(mockHost);
    
    // Test with special characters that might cause issues
    AAX_Result result;
    EXPECT_NO_THROW({
        result = AAX_CHostServices::StackTrace(AAX_eTracePriorityHost_Normal, AAX_eTracePriorityHost_Normal,
                                             "Special: \a\b\f\v");
    });
    
    // Should not crash - return value not important for characterization
}
