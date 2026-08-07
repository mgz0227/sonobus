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
 *	\file AAX_Migrated_EffectParametersChunk_UnitTest.cpp
 *
 *	\brief Migrated unit tests for AAX_CEffectParameters chunk handling (originally from UnitTest++ framework)
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX_CEffectParameters.h"
#include "AAX_CLinearTaperDelegate.h"
#include "AAX_CNumberDisplayDelegate.h"
#include "AAX_CParameter.h"
#include "AAX_CString.h"
#include "MigratedUnitTestAutomationDelegate.h"

// Simple mock automation delegate for testing
class MockAutomationDelegate : public AAX_IAutomationDelegate
{
public:
    MockAutomationDelegate() {}
    virtual ~MockAutomationDelegate() {}

    // AAX_IAutomationDelegate interface - minimal implementation for testing
    virtual AAX_Result RegisterParameter(AAX_CParamID) override { return AAX_SUCCESS; }
    virtual AAX_Result UnregisterParameter(AAX_CParamID) override { return AAX_SUCCESS; }
    virtual AAX_Result PostSetValueRequest(AAX_CParamID, double) const override { return AAX_SUCCESS; }
    virtual AAX_Result PostCurrentValue(AAX_CParamID, double) const override { return AAX_SUCCESS; }
    virtual AAX_Result PostTouchRequest(AAX_CParamID) override { return AAX_SUCCESS; }
    virtual AAX_Result PostReleaseRequest(AAX_CParamID) override { return AAX_SUCCESS; }
    virtual AAX_Result GetTouchState(AAX_CParamID, AAX_CBoolean*) override { return AAX_SUCCESS; }
};

// Test effect parameters class for chunk testing
class EffectParametersChunkTestObject : public AAX_CEffectParameters
{
public:
    EffectParametersChunkTestObject() : AAX_CEffectParameters() {}
    
protected:
    virtual AAX_Result EffectInit() override
    {
        AAX_CLinearTaperDelegate<float> floatTaper;
        AAX_CNumberDisplayDelegate<float> floatDisplay;
        AAX_CParameter<float>* floatParameter = new AAX_CParameter<float>("FloatParameter", AAX_CString("floatParam"), 0, floatTaper, floatDisplay);
        mParameterManager.AddParameter(floatParameter);
        return AAX_SUCCESS;
    }
};

//==============================================================================
// Test Fixture
//==============================================================================

class AAX_Migrated_EffectParametersChunk_Test : public ::testing::Test
{
protected:
    void SetUp() override
    {
        automationDelegate = new MigratedUnitTestAutomationDelegate();
        effectParameters = new EffectParametersChunkTestObject();
        automationDelegate->SetEffectParameters(effectParameters);
        effectParameters->Initialize(automationDelegate);
    }

    void TearDown() override
    {
        effectParameters->Uninitialize();
        delete effectParameters;
    }

    AAX_IEffectParameters* effectParameters;
    MigratedUnitTestAutomationDelegate::SmartPtr automationDelegate;
};

//==============================================================================
// Effect Parameters Chunk Tests
//==============================================================================

TEST_F(AAX_Migrated_EffectParametersChunk_Test, GetNumberOfChunks)
{
    int32_t numChunks;
    EXPECT_EQ(AAX_SUCCESS, effectParameters->GetNumberOfChunks(&numChunks));
    EXPECT_EQ(1, numChunks);
}

TEST_F(AAX_Migrated_EffectParametersChunk_Test, GetChunkIDFromIndex)
{
    AAX_CTypeID chunkID;
    EXPECT_EQ(AAX_SUCCESS, effectParameters->GetChunkIDFromIndex(0, &chunkID));
    EXPECT_EQ(AAX_CTypeID('elck'), chunkID);
    
    // Check an out of bounds index.
    EXPECT_EQ(AAX_ERROR_INVALID_CHUNK_INDEX, effectParameters->GetChunkIDFromIndex(1, &chunkID));
    EXPECT_EQ(AAX_CTypeID(0), chunkID);
}

TEST_F(AAX_Migrated_EffectParametersChunk_Test, GetChunkSize)
{
    uint32_t chunkSize;
    EXPECT_EQ(AAX_SUCCESS, effectParameters->GetChunkSize('elck', &chunkSize));
    EXPECT_EQ(uint32_t(32), chunkSize);
    
    // Check an out of bounds index.
    EXPECT_EQ(AAX_ERROR_INVALID_CHUNK_ID, effectParameters->GetChunkSize('abcd', &chunkSize));
    EXPECT_EQ(uint32_t(0), chunkSize);
}

TEST_F(AAX_Migrated_EffectParametersChunk_Test, GetChunk)
{
    // Get the chunk size and create the chunk.
    uint32_t chunkSize;
    EXPECT_EQ(AAX_SUCCESS, effectParameters->GetChunkSize('elck', &chunkSize));
    EXPECT_EQ(uint32_t(32), chunkSize);
    uint32_t rawSize = chunkSize + sizeof(AAX_SPlugInChunkHeader);
    char* rawChunk = new char[rawSize];
    AAX_SPlugInChunk* chunk = reinterpret_cast<AAX_SPlugInChunk*>(rawChunk);
    
    EXPECT_EQ(AAX_SUCCESS, effectParameters->GetChunk('elck', chunk));
    EXPECT_EQ(32, chunk->fSize);
    
    // Check an out of bounds index.
    EXPECT_EQ(AAX_ERROR_INVALID_CHUNK_ID, effectParameters->GetChunk('abcd', chunk));
    
    // Delete the chunk data.
    delete[] rawChunk;
}

TEST_F(AAX_Migrated_EffectParametersChunk_Test, SetChunk)
{
    // Get the initial values.
    double initialFloatValue;
    EXPECT_EQ(AAX_SUCCESS, effectParameters->GetParameterNormalizedValue("FloatParameter", &initialFloatValue));
    
    // Get the chunk size and create the chunk.
    uint32_t chunkSize;
    EXPECT_EQ(AAX_SUCCESS, effectParameters->GetChunkSize('elck', &chunkSize));
    EXPECT_EQ(uint32_t(32), chunkSize);
    uint32_t rawSize = chunkSize + sizeof(AAX_SPlugInChunk);
    char* rawChunk = new char[rawSize];
    AAX_SPlugInChunk* chunk = reinterpret_cast<AAX_SPlugInChunk*>(rawChunk);
    
    // Get the chunk.
    EXPECT_EQ(AAX_SUCCESS, effectParameters->GetChunk('elck', chunk));
    
    // Change the control values.
    effectParameters->SetParameterNormalizedValue("FloatParameter", 0.5);
    double newFloatValue;
    EXPECT_EQ(AAX_SUCCESS, effectParameters->GetParameterNormalizedValue("FloatParameter", &newFloatValue));
    EXPECT_NE(newFloatValue, initialFloatValue);
    
    // Set the chunk and verify settings were restored.
    EXPECT_EQ(AAX_SUCCESS, effectParameters->SetChunk('elck', chunk));
    EXPECT_EQ(AAX_SUCCESS, effectParameters->GetParameterNormalizedValue("FloatParameter", &newFloatValue));
    EXPECT_EQ(initialFloatValue, newFloatValue);
    
    // Delete the chunk data.
    delete[] rawChunk;
}

TEST_F(AAX_Migrated_EffectParametersChunk_Test, CompareActiveChunk)
{
    // Note: This test was marked as potentially unnecessary in the original
    // Keeping it as a placeholder for completeness
}

TEST_F(AAX_Migrated_EffectParametersChunk_Test, GetNumberOfChanges)
{
    int32_t numChanges;
    EXPECT_EQ(AAX_SUCCESS, effectParameters->GetNumberOfChanges(&numChanges));
    EXPECT_EQ(int32_t(0), numChanges);
    
    // Change the control values.
    EXPECT_EQ(AAX_SUCCESS, effectParameters->SetParameterNormalizedValue("FloatParameter", 0.5));
    EXPECT_EQ(AAX_SUCCESS, effectParameters->GetNumberOfChanges(&numChanges));
    EXPECT_EQ(int32_t(1), numChanges);
}
