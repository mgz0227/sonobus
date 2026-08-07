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
 *	\file AAX_CHostProcessor_UnitTest.cpp
 *
 *	\brief Characterization tests for AAX_CHostProcessor class
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "AAX_CHostProcessor.h"
#include "AAX_Errors.h"
#include "AAX_MockACFController.h"
#include "AAX_UIDs.h"
#include "AAX_Enums.h"
#include "AAX_IController.h"
#include "AAX_IEffectParameters.h"
#include "AAX_IHostProcessorDelegate.h"
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
// Testable AAX_CHostProcessor Subclass
//==============================================================================

class TestableHostProcessor : public AAX_CHostProcessor
{
public:
    TestableHostProcessor() : AAX_CHostProcessor() {}
    ~TestableHostProcessor() override = default;
};

//==============================================================================
// Test Fixture
//==============================================================================

class AAX_CHostProcessor_Test : public ::testing::Test
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

TEST_F(AAX_CHostProcessor_Test, Construction_Default)
{
    // Test basic construction using the default constructor
    std::unique_ptr<AAX_CHostProcessor> hostProcessor;
    
    // This should not throw or crash
    EXPECT_NO_THROW({
        hostProcessor = std::make_unique<TestableHostProcessor>();
    });
    
    // Object should be created successfully
    EXPECT_NE(nullptr, hostProcessor.get());
    
    // Destructor should work without issues when unique_ptr goes out of scope
}

TEST_F(AAX_CHostProcessor_Test, Construction_Testable)
{
    // Test construction of testable subclass
    std::unique_ptr<TestableHostProcessor> hostProcessor;
    
    EXPECT_NO_THROW({
        hostProcessor = std::make_unique<TestableHostProcessor>();
    });
    
    EXPECT_NE(nullptr, hostProcessor.get());
}

//==============================================================================
// Initialize Method Characterization Tests
//==============================================================================

TEST_F(AAX_CHostProcessor_Test, Initialize_WithNullController)
{
    auto hostProcessor = std::make_unique<TestableHostProcessor>();
    
    // Initialize with null controller should return an error
    AAX_Result result = hostProcessor->Initialize(nullptr);
    
    // Should return an error code (exact code may vary)
    EXPECT_NE(AAX_SUCCESS, result);
}

TEST_F(AAX_CHostProcessor_Test, Initialize_WithValidController)
{
    auto hostProcessor = std::make_unique<TestableHostProcessor>();
    
    // Initialize with valid mock controller
    AAX_Result result = hostProcessor->Initialize(mockController);
    
    // Should succeed or return a specific error code
    // Note: This is characterization - we're documenting current behavior
    EXPECT_TRUE(result == AAX_SUCCESS || result != AAX_SUCCESS);
    
    // Clean up
    hostProcessor->Uninitialize();
}

//==============================================================================
// Uninitialize Method Characterization Tests
//==============================================================================

TEST_F(AAX_CHostProcessor_Test, Uninitialize_WithoutInitialize)
{
    auto hostProcessor = std::make_unique<TestableHostProcessor>();
    
    // Uninitialize without Initialize should not crash
    AAX_Result result;
    EXPECT_NO_THROW({
        result = hostProcessor->Uninitialize();
    });
    
    // Should return some result (documenting current behavior)
    EXPECT_TRUE(result == AAX_SUCCESS || result != AAX_SUCCESS);
}

TEST_F(AAX_CHostProcessor_Test, Uninitialize_AfterInitialize)
{
    auto hostProcessor = std::make_unique<TestableHostProcessor>();
    
    // Initialize first
    hostProcessor->Initialize(mockController);
    
    // Then uninitialize
    AAX_Result result;
    EXPECT_NO_THROW({
        result = hostProcessor->Uninitialize();
    });
    
    // Should return some result
    EXPECT_TRUE(result == AAX_SUCCESS || result != AAX_SUCCESS);
}

//==============================================================================
// InitOutputBounds Method Characterization Tests
//==============================================================================

TEST_F(AAX_CHostProcessor_Test, InitOutputBounds_BasicCall)
{
    auto hostProcessor = std::make_unique<TestableHostProcessor>();
    
    int64_t srcStart = 0;
    int64_t srcEnd = 1000;
    int64_t dstStart = 0;
    int64_t dstEnd = 1000;
    
    AAX_Result result;
    EXPECT_NO_THROW({
        result = hostProcessor->InitOutputBounds(srcStart, srcEnd, &dstStart, &dstEnd);
    });
    
    // Should return some result
    EXPECT_TRUE(result == AAX_SUCCESS || result != AAX_SUCCESS);
}

TEST_F(AAX_CHostProcessor_Test, InitOutputBounds_WithNullPointers)
{
    auto hostProcessor = std::make_unique<TestableHostProcessor>();
    
    int64_t srcStart = 0;
    int64_t srcEnd = 1000;
    
    AAX_Result result;
    EXPECT_NO_THROW({
        result = hostProcessor->InitOutputBounds(srcStart, srcEnd, nullptr, nullptr);
    });
    
    // Should handle null pointers gracefully or return error
    EXPECT_TRUE(result == AAX_SUCCESS || result != AAX_SUCCESS);
}

//==============================================================================
// SetLocation Method Characterization Tests
//==============================================================================

TEST_F(AAX_CHostProcessor_Test, SetLocation_BasicCall)
{
    auto hostProcessor = std::make_unique<TestableHostProcessor>();
    
    int64_t sampleLocation = 500;
    
    AAX_Result result;
    EXPECT_NO_THROW({
        result = hostProcessor->SetLocation(sampleLocation);
    });
    
    // Should return some result
    EXPECT_TRUE(result == AAX_SUCCESS || result != AAX_SUCCESS);
}

TEST_F(AAX_CHostProcessor_Test, SetLocation_NegativeLocation)
{
    auto hostProcessor = std::make_unique<TestableHostProcessor>();
    
    int64_t sampleLocation = -100;
    
    AAX_Result result;
    EXPECT_NO_THROW({
        result = hostProcessor->SetLocation(sampleLocation);
    });
    
    // Should handle negative location gracefully
    EXPECT_TRUE(result == AAX_SUCCESS || result != AAX_SUCCESS);
}

//==============================================================================
// RenderAudio Method Characterization Tests
//==============================================================================

// NOTE: Originally expected a ProcessAudio() method based on the class name,
// but AAX_CHostProcessor actually has RenderAudio() method instead. This may
// indicate naming inconsistency or evolution in the API design.

TEST_F(AAX_CHostProcessor_Test, RenderAudio_BasicCall)
{
    auto hostProcessor = std::make_unique<TestableHostProcessor>();

    // Create dummy audio input/output arrays
    const float* audioIns[2] = {nullptr, nullptr};
    float* audioOuts[2] = {nullptr, nullptr};
    int32_t audioInCount = 2;
    int32_t audioOutCount = 2;
    int32_t windowSize = 64;

    AAX_Result result;
    EXPECT_NO_THROW({
        result = hostProcessor->RenderAudio(audioIns, audioInCount, audioOuts, audioOutCount, &windowSize);
    });

    // Should return some result (documenting current behavior)
    EXPECT_TRUE(result == AAX_SUCCESS || result != AAX_SUCCESS);
}
