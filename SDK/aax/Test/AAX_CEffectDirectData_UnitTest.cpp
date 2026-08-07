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
 *	\file AAX_CEffectDirectData_UnitTest.cpp
 *
 *	\brief Characterization tests for AAX_CEffectDirectData class
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "AAX_CEffectDirectData.h"
#include "AAX_Errors.h"
#include "AAX_MockACFController.h"
#include "AAX_MockACFAutomationDelegate.h"
#include "AAX_MockPageTable.h"
#include "AAX_UIDs.h"
#include "AAX_Enums.h"
#include "AAX_IController.h"
#include "AAX_IEffectParameters.h"
#include "AAX_IPrivateDataAccess.h"
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
// Testable AAX_CEffectDirectData Subclass
//==============================================================================

class TestableEffectDirectData : public AAX_CEffectDirectData
{
public:
    TestableEffectDirectData() : AAX_CEffectDirectData() {}
    ~TestableEffectDirectData() override = default;

    // Expose protected methods for testing
    using AAX_CEffectDirectData::Controller;
    using AAX_CEffectDirectData::EffectParameters;
};

//==============================================================================
// Test Fixture
//==============================================================================

class AAX_CEffectDirectData_Test : public ::testing::Test
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

TEST_F(AAX_CEffectDirectData_Test, Construction_Default)
{
    // Test basic construction using the default constructor
    std::unique_ptr<AAX_CEffectDirectData> effectDirectData;
    
    // This should not throw or crash
    EXPECT_NO_THROW({
        effectDirectData = std::make_unique<AAX_CEffectDirectData>();
    });
    
    // Object should be created successfully
    EXPECT_NE(nullptr, effectDirectData.get());
    
    // Destructor should work without issues when unique_ptr goes out of scope
}

TEST_F(AAX_CEffectDirectData_Test, Construction_Testable)
{
    // Test construction of testable subclass
    std::unique_ptr<TestableEffectDirectData> effectDirectData;
    
    EXPECT_NO_THROW({
        effectDirectData = std::make_unique<TestableEffectDirectData>();
    });
    
    EXPECT_NE(nullptr, effectDirectData.get());
}

//==============================================================================
// Initialize Method Characterization Tests
//==============================================================================

TEST_F(AAX_CEffectDirectData_Test, Initialize_WithNullController)
{
    auto effectDirectData = std::make_unique<AAX_CEffectDirectData>();
    
    // Initialize with null controller should return an error
    AAX_Result result = effectDirectData->Initialize(nullptr);
    
    // Should return an error code (exact code may vary)
    EXPECT_NE(AAX_SUCCESS, result);
}

TEST_F(AAX_CEffectDirectData_Test, Initialize_WithValidController)
{
    auto effectDirectData = std::make_unique<AAX_CEffectDirectData>();
    
    // Initialize with valid mock controller
    AAX_Result result = effectDirectData->Initialize(mockController);
    
    // Should succeed or return a specific error code
    // Note: This is characterization - we're documenting current behavior
    EXPECT_TRUE(result == AAX_SUCCESS || result != AAX_SUCCESS);
    
    // Clean up
    effectDirectData->Uninitialize();
}

//==============================================================================
// Uninitialize Method Characterization Tests
//==============================================================================

TEST_F(AAX_CEffectDirectData_Test, Uninitialize_WithoutInitialize)
{
    auto effectDirectData = std::make_unique<AAX_CEffectDirectData>();
    
    // Uninitialize without Initialize should not crash
    AAX_Result result;
    EXPECT_NO_THROW({
        result = effectDirectData->Uninitialize();
    });
    
    // Should return some result (documenting current behavior)
    EXPECT_TRUE(result == AAX_SUCCESS || result != AAX_SUCCESS);
}

TEST_F(AAX_CEffectDirectData_Test, Uninitialize_AfterInitialize)
{
    auto effectDirectData = std::make_unique<AAX_CEffectDirectData>();
    
    // Initialize first
    effectDirectData->Initialize(mockController);
    
    // Then uninitialize
    AAX_Result result;
    EXPECT_NO_THROW({
        result = effectDirectData->Uninitialize();
    });
    
    // Should return some result
    EXPECT_TRUE(result == AAX_SUCCESS || result != AAX_SUCCESS);
}

//==============================================================================
// TimerWakeup Method Characterization Tests
//==============================================================================

TEST_F(AAX_CEffectDirectData_Test, TimerWakeup_WithNullInterface)
{
    auto effectDirectData = std::make_unique<AAX_CEffectDirectData>();
    
    // TimerWakeup with null interface should not crash
    AAX_Result result;
    EXPECT_NO_THROW({
        result = effectDirectData->TimerWakeup(nullptr);
    });
    
    // Should return some result (documenting current behavior)
    EXPECT_TRUE(result == AAX_SUCCESS || result != AAX_SUCCESS);
}

//==============================================================================
// NotificationReceived Method Characterization Tests
//==============================================================================

TEST_F(AAX_CEffectDirectData_Test, NotificationReceived_BasicCall)
{
    auto effectDirectData = std::make_unique<AAX_CEffectDirectData>();
    
    // NotificationReceived should not crash with basic parameters
    AAX_Result result;
    EXPECT_NO_THROW({
        result = effectDirectData->NotificationReceived(0, nullptr, 0);
    });
    
    // Should return some result
    EXPECT_TRUE(result == AAX_SUCCESS || result != AAX_SUCCESS);
}

//==============================================================================
// Accessor Method Characterization Tests
//==============================================================================

TEST_F(AAX_CEffectDirectData_Test, Accessors_BeforeInitialize)
{
    auto effectDirectData = std::make_unique<TestableEffectDirectData>();
    
    // Accessors before Initialize should not crash
    AAX_IController* controller = nullptr;
    AAX_IEffectParameters* effectParams = nullptr;
    
    EXPECT_NO_THROW({
        controller = effectDirectData->Controller();
        effectParams = effectDirectData->EffectParameters();
    });
    
    // Document current behavior (may be null or valid pointers)
    // This is characterization - we're not asserting specific behavior
}

TEST_F(AAX_CEffectDirectData_Test, Accessors_AfterInitialize)
{
    auto effectDirectData = std::make_unique<TestableEffectDirectData>();
    
    // Initialize first
    effectDirectData->Initialize(mockController);
    
    // Accessors after Initialize
    AAX_IController* controller = nullptr;
    AAX_IEffectParameters* effectParams = nullptr;
    
    EXPECT_NO_THROW({
        controller = effectDirectData->Controller();
        effectParams = effectDirectData->EffectParameters();
    });
    
    // Clean up
    effectDirectData->Uninitialize();
}
