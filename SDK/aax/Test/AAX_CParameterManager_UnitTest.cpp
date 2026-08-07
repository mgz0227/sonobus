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
 *	\file AAX_CParameterManager_UnitTest.cpp
 *
 *	\brief Comprehensive unit tests for AAX_CParameterManager class
 *
 */
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX_CParameterManager.h"
#include "AAX_CParameter.h"
#include "AAX_MockTaperDelegate.h"
#include "AAX_MockAutomationDelegate.h"
#include "AAX_CString.h"
#include "AAX_IAutomationDelegate.h"
#include "AAX_CBinaryTaperDelegate.h"
#include "AAX_CLinearTaperDelegate.h"
#include "AAX_CBinaryDisplayDelegate.h"
#include "AAX_CNumberDisplayDelegate.h"
#include <memory>
#include <string>
#include <vector>



//==============================================================================
// Basic Tests
//==============================================================================

// Test basic construction and initialization
TEST(AAX_CParameterManager_BasicTest, Construction)
{
    // Automation delegate must outlive parameter manager
    AAX_MockAutomationDelegate mockDelegate;

    {
        AAX_CParameterManager manager;

        // Test initialization
        manager.Initialize(&mockDelegate);

        // Test initial state
        EXPECT_EQ(0, manager.NumParameters());
    }
}

// Test null parameter handling
TEST(AAX_CParameterManager_BasicTest, NullParameterHandling)
{
    // Automation delegate must outlive parameter manager
    AAX_MockAutomationDelegate mockDelegate;

    {
        AAX_CParameterManager manager;
        manager.Initialize(&mockDelegate);

        // Test adding null parameter (should be handled gracefully)
        manager.AddParameter(nullptr);
        EXPECT_EQ(0, manager.NumParameters());

        // Test retrieving non-existent parameters
        EXPECT_EQ(nullptr, manager.GetParameterByID("NonExistent"));
        EXPECT_EQ(nullptr, manager.GetParameterByName("NonExistent"));
        EXPECT_EQ(nullptr, manager.GetParameter(0));
        EXPECT_EQ(-1, manager.GetParameterIndex("NonExistent"));

        // Test with null parameter ID
        EXPECT_EQ(nullptr, manager.GetParameterByID(nullptr));
        EXPECT_EQ(nullptr, manager.GetParameterByName(nullptr));
        EXPECT_EQ(-1, manager.GetParameterIndex(nullptr));
    }
}

//==============================================================================
// Test Fixture
//==============================================================================

class AAX_CParameterManager_Test : public ::testing::Test
{
protected:
    void SetUp() override
    {
        mMockAutomationDelegate = std::make_unique<AAX_MockAutomationDelegate>();
        mParameterManager = std::make_unique<AAX_CParameterManager>();

        // Initialize the parameter manager
        mParameterManager->Initialize(mMockAutomationDelegate.get());
    }
    
    void TearDown() override
    {
        // Cleanup in correct order: parameter manager first, then automation delegate
        mParameterManager.reset();
        mMockAutomationDelegate.reset();
    }

    // Helper method to create a boolean parameter (like bypass)
    AAX_IParameter* CreateBoolParameter(const std::string& id, const std::string& name, bool defaultValue, bool automatable = true)
    {
        AAX_CString paramID(id.c_str());
        AAX_CString paramName(name.c_str());

        return new AAX_CParameter<bool>(
            paramID, paramName, defaultValue,
            AAX_CBinaryTaperDelegate<bool>(),
            AAX_CBinaryDisplayDelegate<bool>("off", "on"),
            automatable);
    }

    // Helper method to create a float parameter (like gain)
    AAX_IParameter* CreateFloatParameter(const std::string& id, const std::string& name, float defaultValue,
                                       float minValue = 0.0f, float maxValue = 1.0f, bool automatable = true)
    {
        AAX_CString paramID(id.c_str());
        AAX_CString paramName(name.c_str());

        return new AAX_CParameter<float>(
            paramID, paramName, defaultValue,
            AAX_CLinearTaperDelegate<float>(minValue, maxValue),
            AAX_CNumberDisplayDelegate<float>(),
            automatable);
    }

    // Helper method to create a double parameter using mock taper delegate
    AAX_IParameter* CreateDoubleParameter(const std::string& id, const std::string& name, double defaultValue,
                                        double minValue = 0.0, double maxValue = 1.0, bool automatable = true)
    {
        AAX_CString paramID(id.c_str());
        AAX_CString paramName(name.c_str());

        auto param = new AAX_CParameter<double>(paramID, paramName, automatable);

        // Use mock taper delegate
        auto mockTaper = new AAX_MockTaperDelegate<double>(minValue, maxValue);
        param->SetTaperDelegate(*mockTaper, false);
        param->SetValue(defaultValue);

        return param;
    }

    // Helper method to create an int32_t parameter using mock taper delegate
    AAX_IParameter* CreateIntParameter(const std::string& id, const std::string& name, int32_t defaultValue,
                                     int32_t minValue = 0, int32_t maxValue = 100, bool automatable = true)
    {
        AAX_CString paramID(id.c_str());
        AAX_CString paramName(name.c_str());

        auto param = new AAX_CParameter<int32_t>(paramID, paramName, automatable);

        // Use mock taper delegate
        auto mockTaper = new AAX_MockTaperDelegate<int32_t>(minValue, maxValue);
        param->SetTaperDelegate(*mockTaper, false);
        param->SetValue(defaultValue);

        return param;
    }

    std::unique_ptr<AAX_MockAutomationDelegate> mMockAutomationDelegate;
    std::unique_ptr<AAX_CParameterManager> mParameterManager;
};

//==============================================================================
// Basic Functionality Tests
//==============================================================================

// Simple standalone test without complex setup
TEST(AAX_CParameterManager_SimpleTest, BasicOperations)
{
    // Automation delegate must outlive parameter manager
    AAX_MockAutomationDelegate mockDelegate;

    {
        // Create a simple parameter manager
        AAX_CParameterManager manager;
        manager.Initialize(&mockDelegate);

        EXPECT_EQ(0, manager.NumParameters());

        // Create a simple parameter and add it
        AAX_CString paramID("TestParam");
        AAX_CString paramName("Test Parameter");
        AAX_IParameter* param = new AAX_CParameter<bool>(
            paramID, paramName, false,
            AAX_CBinaryTaperDelegate<bool>(),
            AAX_CBinaryDisplayDelegate<bool>("off", "on"),
            true);
        manager.AddParameter(param);

        EXPECT_EQ(1, manager.NumParameters());

        // Test retrieval
        AAX_IParameter* retrieved = manager.GetParameterByID("TestParam");
        EXPECT_EQ(param, retrieved);

        // Test non-existent parameter
        AAX_IParameter* nonExistent = manager.GetParameterByID("NonExistent");
        EXPECT_EQ(nullptr, nonExistent);

        // Parameter will be deleted by manager destructor
    }
}

//==============================================================================
// Basic Functionality Tests
//==============================================================================

TEST_F(AAX_CParameterManager_Test, BasicConstruction)
{
    // Test that parameter manager is properly constructed and initialized
    EXPECT_EQ(0, mParameterManager->NumParameters());
}

TEST_F(AAX_CParameterManager_Test, AddParameter_SingleParameter)
{
    // Create and add a single parameter
    AAX_IParameter* param = CreateBoolParameter("bypass", "Master Bypass", false);
    mParameterManager->AddParameter(param);

    EXPECT_EQ(1, mParameterManager->NumParameters());

    // Verify automation delegate was set
    EXPECT_EQ(1, mMockAutomationDelegate->mRegisterParameterCallCount);
    EXPECT_EQ("bypass", mMockAutomationDelegate->mLastRegisteredParameterID);
}

TEST_F(AAX_CParameterManager_Test, AddParameter_MultipleParameters)
{
    // Create and add multiple parameters of different types
    AAX_IParameter* bypassParam = CreateBoolParameter("bypass", "Master Bypass", false);
    AAX_IParameter* gainParam = CreateFloatParameter("gain", "Gain", 0.5f, 0.0f, 1.0f);
    AAX_IParameter* freqParam = CreateDoubleParameter("freq", "Frequency", 440.0, 20.0, 20000.0);
    AAX_IParameter* stepsParam = CreateIntParameter("steps", "Steps", 50, 0, 100);

    mParameterManager->AddParameter(bypassParam);
    mParameterManager->AddParameter(gainParam);
    mParameterManager->AddParameter(freqParam);
    mParameterManager->AddParameter(stepsParam);

    EXPECT_EQ(4, mParameterManager->NumParameters());

    // Verify all parameters were registered with automation delegate
    EXPECT_EQ(4, mMockAutomationDelegate->mRegisterParameterCallCount);
}

TEST_F(AAX_CParameterManager_Test, AddParameter_NullParameter)
{
    // Test adding null parameter (should be handled gracefully)
    int32_t initialCount = mParameterManager->NumParameters();

    mParameterManager->AddParameter(nullptr);

    // Should not crash and should not change parameter count
    EXPECT_EQ(initialCount, mParameterManager->NumParameters());
    EXPECT_EQ(0, mMockAutomationDelegate->mRegisterParameterCallCount);
}

TEST_F(AAX_CParameterManager_Test, AddParameter_DuplicateID)
{
    // Add a parameter
    AAX_IParameter* param1 = CreateBoolParameter("duplicate", "First Parameter", false);
    mParameterManager->AddParameter(param1);
    EXPECT_EQ(1, mParameterManager->NumParameters());

    // Try to add another parameter with the same ID (should be rejected)
    AAX_IParameter* param2 = CreateFloatParameter("duplicate", "Second Parameter", 0.5f);
    mParameterManager->AddParameter(param2);

    // Should still have only 1 parameter, and param2 should be deleted by AddParameter
    EXPECT_EQ(1, mParameterManager->NumParameters());
    EXPECT_EQ(1, mMockAutomationDelegate->mRegisterParameterCallCount);

    // The first parameter should still be accessible
    AAX_IParameter* retrieved = mParameterManager->GetParameterByID("duplicate");
    EXPECT_EQ(param1, retrieved);
    EXPECT_EQ("First Parameter", std::string(retrieved->Name().CString()));
}

//==============================================================================
// Parameter Retrieval Tests
//==============================================================================

TEST_F(AAX_CParameterManager_Test, GetParameterByID)
{
    // Create and add test parameters
    AAX_IParameter* bypassParam = CreateBoolParameter("bypass", "Master Bypass", false);
    AAX_IParameter* gainParam = CreateFloatParameter("gain", "Gain", 0.5f);
    AAX_IParameter* freqParam = CreateDoubleParameter("freq", "Frequency", 440.0);

    mParameterManager->AddParameter(bypassParam);
    mParameterManager->AddParameter(gainParam);
    mParameterManager->AddParameter(freqParam);

    // Test retrieving parameters by ID
    AAX_IParameter* retrievedBypass = mParameterManager->GetParameterByID("bypass");
    AAX_IParameter* retrievedGain = mParameterManager->GetParameterByID("gain");
    AAX_IParameter* retrievedFreq = mParameterManager->GetParameterByID("freq");

    EXPECT_EQ(bypassParam, retrievedBypass);
    EXPECT_EQ(gainParam, retrievedGain);
    EXPECT_EQ(freqParam, retrievedFreq);

    // Test retrieving non-existent parameter
    AAX_IParameter* nonExistent = mParameterManager->GetParameterByID("NonExistent");
    EXPECT_EQ(nullptr, nonExistent);

    // Test with null parameter ID
    AAX_IParameter* nullID = mParameterManager->GetParameterByID(nullptr);
    EXPECT_EQ(nullptr, nullID);
}

TEST_F(AAX_CParameterManager_Test, GetParameterByID_Const)
{
    // Create and add test parameters
    AAX_IParameter* bypassParam = CreateBoolParameter("bypass", "Master Bypass", false);
    AAX_IParameter* gainParam = CreateFloatParameter("gain", "Gain", 0.5f);

    mParameterManager->AddParameter(bypassParam);
    mParameterManager->AddParameter(gainParam);

    // Test const version of GetParameterByID
    const AAX_CParameterManager* constManager = mParameterManager.get();
    const AAX_IParameter* retrievedBypass = constManager->GetParameterByID("bypass");
    const AAX_IParameter* retrievedGain = constManager->GetParameterByID("gain");

    EXPECT_EQ(bypassParam, retrievedBypass);
    EXPECT_EQ(gainParam, retrievedGain);

    // Test retrieving non-existent parameter
    const AAX_IParameter* nonExistent = constManager->GetParameterByID("NonExistent");
    EXPECT_EQ(nullptr, nonExistent);
}

TEST_F(AAX_CParameterManager_Test, GetParameterByName)
{
    // Create and add test parameters
    AAX_IParameter* bypassParam = CreateBoolParameter("bypass", "Master Bypass", false);
    AAX_IParameter* gainParam = CreateFloatParameter("gain", "Gain Control", 0.5f);
    AAX_IParameter* freqParam = CreateDoubleParameter("freq", "Frequency", 440.0);

    mParameterManager->AddParameter(bypassParam);
    mParameterManager->AddParameter(gainParam);
    mParameterManager->AddParameter(freqParam);

    // Test retrieving parameters by name
    AAX_IParameter* retrievedBypass = mParameterManager->GetParameterByName("Master Bypass");
    AAX_IParameter* retrievedGain = mParameterManager->GetParameterByName("Gain Control");
    AAX_IParameter* retrievedFreq = mParameterManager->GetParameterByName("Frequency");

    EXPECT_EQ(bypassParam, retrievedBypass);
    EXPECT_EQ(gainParam, retrievedGain);
    EXPECT_EQ(freqParam, retrievedFreq);

    // Test retrieving non-existent parameter
    AAX_IParameter* nonExistent = mParameterManager->GetParameterByName("NonExistent");
    EXPECT_EQ(nullptr, nonExistent);

    // Test with null name
    AAX_IParameter* nullName = mParameterManager->GetParameterByName(nullptr);
    EXPECT_EQ(nullptr, nullName);
}

TEST_F(AAX_CParameterManager_Test, GetParameterByName_Const)
{
    // Create and add test parameters
    AAX_IParameter* bypassParam = CreateBoolParameter("bypass", "Master Bypass", false);
    AAX_IParameter* gainParam = CreateFloatParameter("gain", "Gain Control", 0.5f);

    mParameterManager->AddParameter(bypassParam);
    mParameterManager->AddParameter(gainParam);

    // Test const version of GetParameterByName
    const AAX_CParameterManager* constManager = mParameterManager.get();
    const AAX_IParameter* retrievedBypass = constManager->GetParameterByName("Master Bypass");
    const AAX_IParameter* retrievedGain = constManager->GetParameterByName("Gain Control");

    EXPECT_EQ(bypassParam, retrievedBypass);
    EXPECT_EQ(gainParam, retrievedGain);

    // Test retrieving non-existent parameter
    const AAX_IParameter* nonExistent = constManager->GetParameterByName("NonExistent");
    EXPECT_EQ(nullptr, nonExistent);
}

TEST_F(AAX_CParameterManager_Test, GetParameterByIndex)
{
    // Create and add test parameters
    AAX_IParameter* bypassParam = CreateBoolParameter("bypass", "Master Bypass", false);
    AAX_IParameter* gainParam = CreateFloatParameter("gain", "Gain", 0.5f);
    AAX_IParameter* freqParam = CreateDoubleParameter("freq", "Frequency", 440.0);

    mParameterManager->AddParameter(bypassParam);
    mParameterManager->AddParameter(gainParam);
    mParameterManager->AddParameter(freqParam);

    // Test retrieving parameters by index
    AAX_IParameter* param0 = mParameterManager->GetParameter(0);
    AAX_IParameter* param1 = mParameterManager->GetParameter(1);
    AAX_IParameter* param2 = mParameterManager->GetParameter(2);

    EXPECT_EQ(bypassParam, param0);
    EXPECT_EQ(gainParam, param1);
    EXPECT_EQ(freqParam, param2);

    // Test out-of-bounds indices
    AAX_IParameter* outOfBounds1 = mParameterManager->GetParameter(-1);
    AAX_IParameter* outOfBounds2 = mParameterManager->GetParameter(3);

    EXPECT_EQ(nullptr, outOfBounds1);
    EXPECT_EQ(nullptr, outOfBounds2);
}

TEST_F(AAX_CParameterManager_Test, GetParameterByIndex_Const)
{
    // Create and add test parameters
    AAX_IParameter* bypassParam = CreateBoolParameter("bypass", "Master Bypass", false);
    AAX_IParameter* gainParam = CreateFloatParameter("gain", "Gain", 0.5f);

    mParameterManager->AddParameter(bypassParam);
    mParameterManager->AddParameter(gainParam);

    // Test const version of GetParameter
    const AAX_CParameterManager* constManager = mParameterManager.get();
    const AAX_IParameter* param0 = constManager->GetParameter(0);
    const AAX_IParameter* param1 = constManager->GetParameter(1);

    EXPECT_EQ(bypassParam, param0);
    EXPECT_EQ(gainParam, param1);

    // Test out-of-bounds index
    const AAX_IParameter* outOfBounds = constManager->GetParameter(2);
    EXPECT_EQ(nullptr, outOfBounds);
}

TEST_F(AAX_CParameterManager_Test, GetParameterIndex)
{
    // Create and add test parameters
    AAX_IParameter* bypassParam = CreateBoolParameter("bypass", "Master Bypass", false);
    AAX_IParameter* gainParam = CreateFloatParameter("gain", "Gain", 0.5f);
    AAX_IParameter* freqParam = CreateDoubleParameter("freq", "Frequency", 440.0);

    mParameterManager->AddParameter(bypassParam);
    mParameterManager->AddParameter(gainParam);
    mParameterManager->AddParameter(freqParam);

    // Test getting parameter indices
    int32_t bypassIndex = mParameterManager->GetParameterIndex("bypass");
    int32_t gainIndex = mParameterManager->GetParameterIndex("gain");
    int32_t freqIndex = mParameterManager->GetParameterIndex("freq");

    EXPECT_EQ(0, bypassIndex);
    EXPECT_EQ(1, gainIndex);
    EXPECT_EQ(2, freqIndex);

    // Test non-existent parameter
    int32_t nonExistentIndex = mParameterManager->GetParameterIndex("NonExistent");
    EXPECT_EQ(-1, nonExistentIndex);

    // Test null parameter ID
    int32_t nullIndex = mParameterManager->GetParameterIndex(nullptr);
    EXPECT_EQ(-1, nullIndex);
}

//==============================================================================
// Parameter Removal Tests
//==============================================================================

TEST_F(AAX_CParameterManager_Test, RemoveParameter)
{
    // Create and add test parameters
    AAX_IParameter* bypassParam = CreateBoolParameter("bypass", "Master Bypass", false);
    AAX_IParameter* gainParam = CreateFloatParameter("gain", "Gain", 0.5f);
    AAX_IParameter* freqParam = CreateDoubleParameter("freq", "Frequency", 440.0);

    mParameterManager->AddParameter(bypassParam);
    mParameterManager->AddParameter(gainParam);
    mParameterManager->AddParameter(freqParam);

    EXPECT_EQ(3, mParameterManager->NumParameters());

    // Remove middle parameter
    mParameterManager->RemoveParameter(gainParam);

    EXPECT_EQ(2, mParameterManager->NumParameters());

    // Verify remaining parameters
    AAX_IParameter* param0 = mParameterManager->GetParameter(0);
    AAX_IParameter* param1 = mParameterManager->GetParameter(1);

    EXPECT_EQ(bypassParam, param0);
    EXPECT_EQ(freqParam, param1);

    // Verify removed parameter is no longer accessible by ID
    AAX_IParameter* removedParam = mParameterManager->GetParameterByID("gain");
    EXPECT_EQ(nullptr, removedParam);

    // Verify automation delegate was notified
    EXPECT_EQ(1, mMockAutomationDelegate->mUnregisterParameterCallCount);
    EXPECT_EQ("gain", mMockAutomationDelegate->mLastUnregisteredParameterID);
}

TEST_F(AAX_CParameterManager_Test, RemoveParameterByID)
{
    // Create and add test parameters
    AAX_IParameter* bypassParam = CreateBoolParameter("bypass", "Master Bypass", false);
    AAX_IParameter* gainParam = CreateFloatParameter("gain", "Gain", 0.5f);
    AAX_IParameter* freqParam = CreateDoubleParameter("freq", "Frequency", 440.0);

    mParameterManager->AddParameter(bypassParam);
    mParameterManager->AddParameter(gainParam);
    mParameterManager->AddParameter(freqParam);

    EXPECT_EQ(3, mParameterManager->NumParameters());

    // Remove parameter by ID
    mParameterManager->RemoveParameterByID("gain");

    EXPECT_EQ(2, mParameterManager->NumParameters());

    // Verify removed parameter is no longer accessible
    AAX_IParameter* removedParam = mParameterManager->GetParameterByID("gain");
    EXPECT_EQ(nullptr, removedParam);

    // Verify remaining parameters are still accessible
    AAX_IParameter* retrievedBypass = mParameterManager->GetParameterByID("bypass");
    AAX_IParameter* retrievedFreq = mParameterManager->GetParameterByID("freq");

    EXPECT_EQ(bypassParam, retrievedBypass);
    EXPECT_EQ(freqParam, retrievedFreq);

    // Verify automation delegate was notified
    EXPECT_EQ(1, mMockAutomationDelegate->mUnregisterParameterCallCount);
    EXPECT_EQ("gain", mMockAutomationDelegate->mLastUnregisteredParameterID);
}

//==============================================================================
// Edge Cases and Error Handling Tests
//==============================================================================

TEST_F(AAX_CParameterManager_Test, NullParameterHandling)
{
    // Test adding null parameter (should be handled gracefully)
    int32_t initialCount = mParameterManager->NumParameters();
    mParameterManager->AddParameter(nullptr);
    EXPECT_EQ(initialCount, mParameterManager->NumParameters());

    // Test removing null parameter (should be handled gracefully)
    AAX_IParameter* param = CreateBoolParameter("test", "Test", false);
    mParameterManager->AddParameter(param);
    initialCount = mParameterManager->NumParameters();
    mParameterManager->RemoveParameter(nullptr);
    EXPECT_EQ(initialCount, mParameterManager->NumParameters());

    // Test retrieving with null IDs
    EXPECT_EQ(nullptr, mParameterManager->GetParameterByID(nullptr));
    EXPECT_EQ(nullptr, mParameterManager->GetParameterByName(nullptr));
    EXPECT_EQ(-1, mParameterManager->GetParameterIndex(nullptr));

    // Test out-of-bounds access
    EXPECT_EQ(nullptr, mParameterManager->GetParameter(-1));
    EXPECT_EQ(nullptr, mParameterManager->GetParameter(100));
}

TEST_F(AAX_CParameterManager_Test, ParameterTypeIntegration)
{
    // Test with different parameter types to ensure they work correctly
    AAX_IParameter* boolParam = CreateBoolParameter("bypass", "Master Bypass", false);
    AAX_IParameter* floatParam = CreateFloatParameter("gain", "Gain", 0.5f, 0.0f, 2.0f);
    AAX_IParameter* doubleParam = CreateDoubleParameter("freq", "Frequency", 440.0, 20.0, 20000.0);
    AAX_IParameter* intParam = CreateIntParameter("steps", "Steps", 50, 0, 100);

    mParameterManager->AddParameter(boolParam);
    mParameterManager->AddParameter(floatParam);
    mParameterManager->AddParameter(doubleParam);
    mParameterManager->AddParameter(intParam);

    EXPECT_EQ(4, mParameterManager->NumParameters());

    // Verify all parameters are accessible and have correct properties
    EXPECT_EQ(boolParam, mParameterManager->GetParameterByID("bypass"));
    EXPECT_EQ(floatParam, mParameterManager->GetParameterByID("gain"));
    EXPECT_EQ(doubleParam, mParameterManager->GetParameterByID("freq"));
    EXPECT_EQ(intParam, mParameterManager->GetParameterByID("steps"));

    // Verify parameter names
    EXPECT_EQ("Master Bypass", std::string(boolParam->Name().CString()));
    EXPECT_EQ("Gain", std::string(floatParam->Name().CString()));
    EXPECT_EQ("Frequency", std::string(doubleParam->Name().CString()));
    EXPECT_EQ("Steps", std::string(intParam->Name().CString()));

    // Verify automation delegate was set for all parameters
    EXPECT_EQ(4, mMockAutomationDelegate->mRegisterParameterCallCount);
}

TEST_F(AAX_CParameterManager_Test, RemoveAllParameters)
{
    // Create and add test parameters
    AAX_IParameter* bypassParam = CreateBoolParameter("bypass", "Master Bypass", false);
    AAX_IParameter* gainParam = CreateFloatParameter("gain", "Gain", 0.5f);
    AAX_IParameter* freqParam = CreateDoubleParameter("freq", "Frequency", 440.0);

    mParameterManager->AddParameter(bypassParam);
    mParameterManager->AddParameter(gainParam);
    mParameterManager->AddParameter(freqParam);

    EXPECT_EQ(3, mParameterManager->NumParameters());

    // Remove all parameters
    mParameterManager->RemoveAllParameters();

    EXPECT_EQ(0, mParameterManager->NumParameters());

    // Verify no parameters are accessible
    EXPECT_EQ(nullptr, mParameterManager->GetParameterByID("bypass"));
    EXPECT_EQ(nullptr, mParameterManager->GetParameterByID("gain"));
    EXPECT_EQ(nullptr, mParameterManager->GetParameterByID("freq"));

    // Verify automation delegate was notified of all removals
    EXPECT_EQ(3, mMockAutomationDelegate->mUnregisterParameterCallCount);
}
