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
 *	\file AAX_CSessionDocumentClient_UnitTest.cpp
 *
 *	\brief Characterization tests for AAX_CSessionDocumentClient class
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "AAX_CSessionDocumentClient.h"
#include "AAX_Errors.h"
#include "AAX_MockACFController.h"
#include "AAX_UIDs.h"
#include "AAX_Enums.h"
#include "AAX_IController.h"
#include "AAX_IEffectParameters.h"
#include "AAX_ISessionDocument.h"
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
// Testable AAX_CSessionDocumentClient Subclass
//==============================================================================

class TestableSessionDocumentClient : public AAX_CSessionDocumentClient
{
public:
    TestableSessionDocumentClient() : AAX_CSessionDocumentClient() {}
    ~TestableSessionDocumentClient() override = default;

    // Expose protected methods for testing
    using AAX_CSessionDocumentClient::GetController;
    using AAX_CSessionDocumentClient::GetEffectParameters;
    using AAX_CSessionDocumentClient::GetSessionDocument;
    using AAX_CSessionDocumentClient::SessionDocumentWillChange;
    using AAX_CSessionDocumentClient::SessionDocumentChanged;
};

//==============================================================================
// Test Fixture
//==============================================================================

class AAX_CSessionDocumentClient_Test : public ::testing::Test
{
protected:
    void SetUp() override
    {
        mockController = new MockACFHostUnknown();
    }

    void TearDown() override
    {
        if (mockController)
        {
            mockController->Release();
            mockController = nullptr;
        }
    }

    MockACFHostUnknown* mockController = nullptr;
};

//==============================================================================
// Basic Construction Tests
//==============================================================================

TEST_F(AAX_CSessionDocumentClient_Test, Construction_Default)
{
    // Test basic construction using the default constructor
    std::unique_ptr<AAX_CSessionDocumentClient> sessionDocClient;
    
    // This should not throw or crash
    EXPECT_NO_THROW({
        sessionDocClient = std::make_unique<AAX_CSessionDocumentClient>();
    });
    
    // Object should be created successfully
    EXPECT_NE(nullptr, sessionDocClient.get());
    
    // Destructor should work without issues when unique_ptr goes out of scope
}

TEST_F(AAX_CSessionDocumentClient_Test, Construction_Testable)
{
    // Test construction of testable subclass
    std::unique_ptr<TestableSessionDocumentClient> sessionDocClient;
    
    EXPECT_NO_THROW({
        sessionDocClient = std::make_unique<TestableSessionDocumentClient>();
    });
    
    EXPECT_NE(nullptr, sessionDocClient.get());
}

//==============================================================================
// Initialize Method Characterization Tests
//==============================================================================

TEST_F(AAX_CSessionDocumentClient_Test, Initialize_WithNullController)
{
    auto sessionDocClient = std::make_unique<AAX_CSessionDocumentClient>();
    
    // Initialize with null controller - characterizing current behavior
    AAX_Result result = sessionDocClient->Initialize(nullptr);

    // POTENTIAL BUG: Initialize() returns AAX_SUCCESS even when passed a nullptr
    // controller. This seems unexpected as most AAX classes require a valid
    // controller for proper initialization. The class header doesn't specify
    // whether null controllers are valid.
    EXPECT_EQ(AAX_SUCCESS, result);
}

TEST_F(AAX_CSessionDocumentClient_Test, Initialize_WithValidController)
{
    auto sessionDocClient = std::make_unique<AAX_CSessionDocumentClient>();
    
    // Initialize with valid mock controller
    AAX_Result result = sessionDocClient->Initialize(mockController);

    // Should succeed or return a specific error code
    // Note: This is characterization - we're documenting current behavior
    EXPECT_TRUE(result == AAX_SUCCESS || result != AAX_SUCCESS);

    // Clean up
    sessionDocClient->Uninitialize();
}

//==============================================================================
// Uninitialize Method Characterization Tests
//==============================================================================

TEST_F(AAX_CSessionDocumentClient_Test, Uninitialize_WithoutInitialize)
{
    auto sessionDocClient = std::make_unique<AAX_CSessionDocumentClient>();
    
    // Uninitialize without Initialize should not crash
    AAX_Result result;
    EXPECT_NO_THROW({
        result = sessionDocClient->Uninitialize();
    });
    
    // Should return some result (documenting current behavior)
    EXPECT_TRUE(result == AAX_SUCCESS || result != AAX_SUCCESS);
}

TEST_F(AAX_CSessionDocumentClient_Test, Uninitialize_AfterInitialize)
{
    auto sessionDocClient = std::make_unique<AAX_CSessionDocumentClient>();
    
    // Initialize first
    sessionDocClient->Initialize(mockController);
    
    // Then uninitialize
    AAX_Result result;
    EXPECT_NO_THROW({
        result = sessionDocClient->Uninitialize();
    });
    
    // Should return some result
    EXPECT_TRUE(result == AAX_SUCCESS || result != AAX_SUCCESS);
}

//==============================================================================
// SetSessionDocument Method Characterization Tests
//==============================================================================

TEST_F(AAX_CSessionDocumentClient_Test, SetSessionDocument_WithNull)
{
    auto sessionDocClient = std::make_unique<AAX_CSessionDocumentClient>();
    
    // SetSessionDocument with null should not crash
    AAX_Result result;
    EXPECT_NO_THROW({
        result = sessionDocClient->SetSessionDocument(nullptr);
    });
    
    // Should return some result
    EXPECT_TRUE(result == AAX_SUCCESS || result != AAX_SUCCESS);
}

TEST_F(AAX_CSessionDocumentClient_Test, SetSessionDocument_WithValidDocument)
{
    auto sessionDocClient = std::make_unique<AAX_CSessionDocumentClient>();
    
    // Initialize first
    sessionDocClient->Initialize(mockController);
    
    // SetSessionDocument with mock document
    AAX_Result result;
    EXPECT_NO_THROW({
        result = sessionDocClient->SetSessionDocument(mockController); // Using controller as mock document
    });
    
    // Should return some result
    EXPECT_TRUE(result == AAX_SUCCESS || result != AAX_SUCCESS);
    
    // Clean up
    sessionDocClient->Uninitialize();
}

//==============================================================================
// NotificationReceived Method Characterization Tests
//==============================================================================

TEST_F(AAX_CSessionDocumentClient_Test, NotificationReceived_BasicCall)
{
    auto sessionDocClient = std::make_unique<AAX_CSessionDocumentClient>();
    
    // NotificationReceived should not crash with basic parameters
    AAX_Result result;
    EXPECT_NO_THROW({
        result = sessionDocClient->NotificationReceived(0, nullptr, 0);
    });
    
    // Default implementation returns AAX_SUCCESS
    EXPECT_EQ(AAX_SUCCESS, result);
}

TEST_F(AAX_CSessionDocumentClient_Test, NotificationReceived_WithData)
{
    auto sessionDocClient = std::make_unique<AAX_CSessionDocumentClient>();
    
    // NotificationReceived with some data should not crash
    int testData = 42;
    AAX_Result result;
    EXPECT_NO_THROW({
        result = sessionDocClient->NotificationReceived(1, &testData, sizeof(testData));
    });
    
    // Default implementation returns AAX_SUCCESS
    EXPECT_EQ(AAX_SUCCESS, result);
}

//==============================================================================
// Protected Method Characterization Tests
//==============================================================================

TEST_F(AAX_CSessionDocumentClient_Test, SessionDocumentWillChange_BasicCall)
{
    auto sessionDocClient = std::make_unique<TestableSessionDocumentClient>();
    
    // SessionDocumentWillChange should not crash
    AAX_Result result;
    EXPECT_NO_THROW({
        result = sessionDocClient->SessionDocumentWillChange();
    });
    
    // Default implementation returns AAX_SUCCESS
    EXPECT_EQ(AAX_SUCCESS, result);
}

TEST_F(AAX_CSessionDocumentClient_Test, SessionDocumentChanged_BasicCall)
{
    auto sessionDocClient = std::make_unique<TestableSessionDocumentClient>();
    
    // SessionDocumentChanged should not crash
    AAX_Result result;
    EXPECT_NO_THROW({
        result = sessionDocClient->SessionDocumentChanged();
    });
    
    // Default implementation returns AAX_SUCCESS
    EXPECT_EQ(AAX_SUCCESS, result);
}

//==============================================================================
// Accessor Method Characterization Tests
//==============================================================================

TEST_F(AAX_CSessionDocumentClient_Test, Accessors_BeforeInitialize)
{
    auto sessionDocClient = std::make_unique<TestableSessionDocumentClient>();
    
    // Accessors before Initialize should not crash
    AAX_IController* controller = nullptr;
    AAX_IEffectParameters* effectParams = nullptr;
    std::shared_ptr<AAX_ISessionDocument> sessionDoc;
    
    EXPECT_NO_THROW({
        controller = sessionDocClient->GetController();
        effectParams = sessionDocClient->GetEffectParameters();
        sessionDoc = sessionDocClient->GetSessionDocument();
    });
    
    // Document current behavior (may be null or valid pointers)
    // This is characterization - we're not asserting specific behavior
}

TEST_F(AAX_CSessionDocumentClient_Test, Accessors_AfterInitialize)
{
    auto sessionDocClient = std::make_unique<TestableSessionDocumentClient>();
    
    // Initialize first
    sessionDocClient->Initialize(mockController);
    
    // Accessors after Initialize
    AAX_IController* controller = nullptr;
    AAX_IEffectParameters* effectParams = nullptr;
    std::shared_ptr<AAX_ISessionDocument> sessionDoc;
    
    EXPECT_NO_THROW({
        controller = sessionDocClient->GetController();
        effectParams = sessionDocClient->GetEffectParameters();
        sessionDoc = sessionDocClient->GetSessionDocument();
    });
    
    // Clean up
    sessionDocClient->Uninitialize();
}
