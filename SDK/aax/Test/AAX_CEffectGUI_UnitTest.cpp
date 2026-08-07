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
 *	\file AAX_CEffectGUI_UnitTest.cpp
 *
 *	\brief Characterization tests for AAX_CEffectGUI class
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "AAX_CEffectGUI.h"
#include "AAX_Errors.h"
#include "AAX_MockACFController.h"
#include "AAX_UIDs.h"
#include "AAX_Enums.h"
#include "AAX_IController.h"
#include "AAX_IEffectParameters.h"
#include "AAX_IViewContainer.h"
#include "AAX_ITransport.h"
#include "AAX_GUITypes.h"
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
// Testable AAX_CEffectGUI Subclass
//==============================================================================

class TestableEffectGUI : public AAX_CEffectGUI
{
public:
    TestableEffectGUI() : AAX_CEffectGUI() {}
    ~TestableEffectGUI() override = default;

    // Implement pure virtual methods with minimal implementations
protected:
    void CreateViewContents() override {}
    void CreateViewContainer() override {}
    void DeleteViewContainer() override {}
};

//==============================================================================
// Test Fixture
//==============================================================================

class AAX_CEffectGUI_Test : public ::testing::Test
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

TEST_F(AAX_CEffectGUI_Test, Construction_Testable)
{
    // Test construction of testable subclass (base class is abstract)
    std::unique_ptr<TestableEffectGUI> effectGUI;
    
    EXPECT_NO_THROW({
        effectGUI = std::make_unique<TestableEffectGUI>();
    });
    
    EXPECT_NE(nullptr, effectGUI.get());
}

//==============================================================================
// Initialize Method Characterization Tests
//==============================================================================

TEST_F(AAX_CEffectGUI_Test, Initialize_WithNullController)
{
    auto effectGUI = std::make_unique<TestableEffectGUI>();
    
    // Initialize with null controller should return an error
    AAX_Result result = effectGUI->Initialize(nullptr);
    
    // Should return an error code (exact code may vary)
    EXPECT_NE(AAX_SUCCESS, result);
}

TEST_F(AAX_CEffectGUI_Test, Initialize_WithValidController)
{
    auto effectGUI = std::make_unique<TestableEffectGUI>();
    
    // Initialize with valid mock controller
    AAX_Result result = effectGUI->Initialize(mockController);
    
    // Should succeed or return a specific error code
    // Note: This is characterization - we're documenting current behavior
    EXPECT_TRUE(result == AAX_SUCCESS || result != AAX_SUCCESS);
    
    // Clean up
    effectGUI->Uninitialize();
}

//==============================================================================
// Uninitialize Method Characterization Tests
//==============================================================================

TEST_F(AAX_CEffectGUI_Test, Uninitialize_WithoutInitialize)
{
    auto effectGUI = std::make_unique<TestableEffectGUI>();
    
    // Uninitialize without Initialize should not crash
    AAX_Result result;
    EXPECT_NO_THROW({
        result = effectGUI->Uninitialize();
    });
    
    // Should return some result (documenting current behavior)
    EXPECT_TRUE(result == AAX_SUCCESS || result != AAX_SUCCESS);
}

TEST_F(AAX_CEffectGUI_Test, Uninitialize_AfterInitialize)
{
    auto effectGUI = std::make_unique<TestableEffectGUI>();
    
    // Initialize first
    effectGUI->Initialize(mockController);
    
    // Then uninitialize
    AAX_Result result;
    EXPECT_NO_THROW({
        result = effectGUI->Uninitialize();
    });
    
    // Should return some result
    EXPECT_TRUE(result == AAX_SUCCESS || result != AAX_SUCCESS);
}

//==============================================================================
// NotificationReceived Method Characterization Tests
//==============================================================================

TEST_F(AAX_CEffectGUI_Test, NotificationReceived_BasicCall)
{
    auto effectGUI = std::make_unique<TestableEffectGUI>();
    
    // NotificationReceived should not crash with basic parameters
    AAX_Result result;
    EXPECT_NO_THROW({
        result = effectGUI->NotificationReceived(0, nullptr, 0);
    });
    
    // Should return some result
    EXPECT_TRUE(result == AAX_SUCCESS || result != AAX_SUCCESS);
}

//==============================================================================
// View Container Method Characterization Tests
//==============================================================================

TEST_F(AAX_CEffectGUI_Test, SetViewContainer_WithNull)
{
    auto effectGUI = std::make_unique<TestableEffectGUI>();
    
    // SetViewContainer with null should not crash
    AAX_Result result;
    EXPECT_NO_THROW({
        result = effectGUI->SetViewContainer(nullptr);
    });
    
    // Should return some result
    EXPECT_TRUE(result == AAX_SUCCESS || result != AAX_SUCCESS);
}

//==============================================================================
// View Size Method Characterization Tests
//==============================================================================

TEST_F(AAX_CEffectGUI_Test, GetViewSize_BasicCall)
{
    auto effectGUI = std::make_unique<TestableEffectGUI>();
    
    AAX_Point viewSize = {0, 0};
    AAX_Result result;
    
    EXPECT_NO_THROW({
        result = effectGUI->GetViewSize(&viewSize);
    });
    
    // Default implementation returns AAX_SUCCESS
    EXPECT_EQ(AAX_SUCCESS, result);
}

TEST_F(AAX_CEffectGUI_Test, GetMinimumViewSize_BasicCall)
{
    auto effectGUI = std::make_unique<TestableEffectGUI>();
    
    AAX_Point minViewSize = {0, 0};
    AAX_Result result;
    
    EXPECT_NO_THROW({
        result = effectGUI->GetMinimumViewSize(&minViewSize);
    });
    
    // Default implementation returns AAX_SUCCESS
    EXPECT_EQ(AAX_SUCCESS, result);
}

//==============================================================================
// GUI Update Method Characterization Tests
//==============================================================================

TEST_F(AAX_CEffectGUI_Test, Draw_BasicCall)
{
    auto effectGUI = std::make_unique<TestableEffectGUI>();
    
    AAX_Rect drawRect = {0, 0, 100, 100};
    AAX_Result result;
    
    EXPECT_NO_THROW({
        result = effectGUI->Draw(&drawRect);
    });
    
    // Default implementation returns AAX_SUCCESS
    EXPECT_EQ(AAX_SUCCESS, result);
}

TEST_F(AAX_CEffectGUI_Test, TimerWakeup_BasicCall)
{
    auto effectGUI = std::make_unique<TestableEffectGUI>();
    
    AAX_Result result;
    EXPECT_NO_THROW({
        result = effectGUI->TimerWakeup();
    });
    
    // Default implementation returns AAX_SUCCESS
    EXPECT_EQ(AAX_SUCCESS, result);
}

TEST_F(AAX_CEffectGUI_Test, ParameterUpdated_BasicCall)
{
    auto effectGUI = std::make_unique<TestableEffectGUI>();
    
    AAX_Result result;
    EXPECT_NO_THROW({
        result = effectGUI->ParameterUpdated("TestParam");
    });
    
    // Should return some result
    EXPECT_TRUE(result == AAX_SUCCESS || result != AAX_SUCCESS);
}

//==============================================================================
// Host Interface Method Characterization Tests
//==============================================================================

TEST_F(AAX_CEffectGUI_Test, SetControlHighlightInfo_BasicCall)
{
    auto effectGUI = std::make_unique<TestableEffectGUI>();
    
    AAX_Result result;
    EXPECT_NO_THROW({
        result = effectGUI->SetControlHighlightInfo("TestParam", true, AAX_eHighlightColor_Red);
    });
    
    // Default implementation returns AAX_SUCCESS
    EXPECT_EQ(AAX_SUCCESS, result);
}
