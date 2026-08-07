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
 *	\file AAX_Migrated_ParameterManager_UnitTest.cpp
 *
 *	\brief Migrated unit tests for AAX_CParameterManager (originally from UnitTest++ framework)
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX_CParameterManager.h"
#include "AAX_CParameter.h"
#include "AAX_CLinearTaperDelegate.h"
#include "AAX_CBinaryTaperDelegate.h"
#include "AAX_CNumberDisplayDelegate.h"
#include "AAX_CBinaryDisplayDelegate.h"

//==============================================================================
// Test Fixture
//==============================================================================

class AAX_Migrated_ParameterManager_Test : public ::testing::Test
{
protected:
    void SetUp() override
    {
        parameterManager = new AAX_CParameterManager();
    }
    
    void TearDown() override
    {
        delete parameterManager;
    }
    
    AAX_CParameterManager* parameterManager;
};

//==============================================================================
// Basic Parameter Manager Tests
//==============================================================================

TEST_F(AAX_Migrated_ParameterManager_Test, AddParameter)
{
    // Manager takes ownership of these in the tests. Manager is responsible for deletion.
    AAX_CLinearTaperDelegate<float> floatTaper;
    AAX_CNumberDisplayDelegate<float> floatDisplay;
    AAX_CLinearTaperDelegate<int32_t> intTaper;
    AAX_CNumberDisplayDelegate<int32_t> intDisplay;
    AAX_CLinearTaperDelegate<uint32_t> uintTaper;
    AAX_CNumberDisplayDelegate<uint32_t> uintDisplay;
    AAX_CParameter<float>* floatParameter = new AAX_CParameter<float>("1", AAX_CString("floatParam"), 0, floatTaper, floatDisplay);
    AAX_CParameter<int32_t>* intParameter = new AAX_CParameter<int32_t>("", AAX_CString("intParam"), 0, intTaper, intDisplay);
    AAX_CParameter<uint32_t>* controlStringParameter = new AAX_CParameter<uint32_t>("3", AAX_CString("controlStringParam"), 0, uintTaper, uintDisplay);

    EXPECT_EQ(int32_t(0), parameterManager->NumParameters());
    parameterManager->AddParameter(floatParameter);
    EXPECT_EQ(int32_t(1), parameterManager->NumParameters());
    parameterManager->AddParameter(intParameter);
    EXPECT_EQ(int32_t(2), parameterManager->NumParameters());
    parameterManager->AddParameter(controlStringParameter);
    EXPECT_EQ(int32_t(3), parameterManager->NumParameters());
}

TEST_F(AAX_Migrated_ParameterManager_Test, RemoveParameter)
{
    // Manager takes ownership of these in the tests. Manager is responsible for deletion.
    AAX_CLinearTaperDelegate<float> floatTaper;
    AAX_CNumberDisplayDelegate<float> floatDisplay;
    AAX_CLinearTaperDelegate<int32_t> intTaper;
    AAX_CNumberDisplayDelegate<int32_t> intDisplay;
    AAX_CLinearTaperDelegate<uint32_t> uintTaper;
    AAX_CNumberDisplayDelegate<uint32_t> uintDisplay;
    AAX_CParameter<float>* floatParameter = new AAX_CParameter<float>("1", AAX_CString("floatParam"), 0, floatTaper, floatDisplay);
    AAX_CParameter<int32_t>* intParameter = new AAX_CParameter<int32_t>("", AAX_CString("intParam"), 0, intTaper, intDisplay);
    AAX_CParameter<uint32_t>* controlStringParameter = new AAX_CParameter<uint32_t>("3", AAX_CString("controlStringParam"), 0, uintTaper, uintDisplay);

    EXPECT_EQ(int32_t(0), parameterManager->NumParameters());
    parameterManager->AddParameter(floatParameter);
    parameterManager->AddParameter(intParameter);
    parameterManager->AddParameter(controlStringParameter);
    EXPECT_EQ(int32_t(3), parameterManager->NumParameters());
    parameterManager->RemoveParameter(floatParameter);
    EXPECT_EQ(int32_t(2), parameterManager->NumParameters());
    parameterManager->RemoveParameter(intParameter);
    EXPECT_EQ(int32_t(1), parameterManager->NumParameters());
    parameterManager->RemoveParameter(controlStringParameter);
    EXPECT_EQ(int32_t(0), parameterManager->NumParameters());
}

TEST_F(AAX_Migrated_ParameterManager_Test, RemoveParameterByIdentifier)
{
    // Manager takes ownership of these in the tests. Manager is responsible for deletion.
    AAX_CLinearTaperDelegate<float> floatTaper;
    AAX_CNumberDisplayDelegate<float> floatDisplay;
    AAX_CLinearTaperDelegate<int32_t> intTaper;
    AAX_CNumberDisplayDelegate<int32_t> intDisplay;
    AAX_CLinearTaperDelegate<uint32_t> uintTaper;
    AAX_CNumberDisplayDelegate<uint32_t> uintDisplay;
    AAX_CParameter<float>* floatParameter = new AAX_CParameter<float>("1", AAX_CString("floatParam"), 0, floatTaper, floatDisplay);
    AAX_CParameter<int32_t>* intParameter = new AAX_CParameter<int32_t>("", AAX_CString("intParam"), 0, intTaper, intDisplay);
    AAX_CParameter<uint32_t>* controlStringParameter = new AAX_CParameter<uint32_t>("3", AAX_CString("controlStringParam"), 0, uintTaper, uintDisplay);

    EXPECT_EQ(int32_t(0), parameterManager->NumParameters());
    parameterManager->AddParameter(floatParameter);
    parameterManager->AddParameter(intParameter);
    parameterManager->AddParameter(controlStringParameter);
    EXPECT_EQ(int32_t(3), parameterManager->NumParameters());
    parameterManager->RemoveParameterByID("1");
    EXPECT_EQ(int32_t(2), parameterManager->NumParameters());
    parameterManager->RemoveParameterByID("1"); // try a double removal.
    EXPECT_EQ(int32_t(2), parameterManager->NumParameters());
    parameterManager->RemoveParameterByID("");
    EXPECT_EQ(int32_t(1), parameterManager->NumParameters());
    parameterManager->RemoveParameterByID("3");
    EXPECT_EQ(int32_t(0), parameterManager->NumParameters());
}

TEST_F(AAX_Migrated_ParameterManager_Test, RemoveAllParameters)
{
    // Manager takes ownership of these in the tests. Manager is responsible for deletion.
    AAX_CLinearTaperDelegate<float> floatTaper;
    AAX_CNumberDisplayDelegate<float> floatDisplay;
    AAX_CLinearTaperDelegate<int32_t> intTaper;
    AAX_CNumberDisplayDelegate<int32_t> intDisplay;
    AAX_CLinearTaperDelegate<uint32_t> uintTaper;
    AAX_CNumberDisplayDelegate<uint32_t> uintDisplay;
    AAX_CParameter<float>* floatParameter = new AAX_CParameter<float>("1", AAX_CString("floatParam"), 0, floatTaper, floatDisplay);
    AAX_CParameter<int32_t>* intParameter = new AAX_CParameter<int32_t>("", AAX_CString("intParam"), 0, intTaper, intDisplay);
    AAX_CParameter<uint32_t>* controlStringParameter = new AAX_CParameter<uint32_t>("3", AAX_CString("controlStringParam"), 0, uintTaper, uintDisplay);

    EXPECT_EQ(int32_t(0), parameterManager->NumParameters());
    parameterManager->AddParameter(floatParameter);
    parameterManager->AddParameter(intParameter);
    parameterManager->AddParameter(controlStringParameter);
    EXPECT_EQ(int32_t(3), parameterManager->NumParameters());
    parameterManager->RemoveAllParameters();
    EXPECT_EQ(int32_t(0), parameterManager->NumParameters());
}

//==============================================================================
// Parameter Retrieval Tests
//==============================================================================

TEST_F(AAX_Migrated_ParameterManager_Test, GetParameter)
{
    // Manager takes ownership of these in the tests. Manager is responsible for deletion.
    AAX_CLinearTaperDelegate<float> floatTaper;
    AAX_CNumberDisplayDelegate<float> floatDisplay;
    AAX_CLinearTaperDelegate<int32_t> intTaper;
    AAX_CNumberDisplayDelegate<int32_t> intDisplay;
    AAX_CLinearTaperDelegate<uint32_t> uintTaper;
    AAX_CNumberDisplayDelegate<uint32_t> uintDisplay;
    AAX_CParameter<float>* floatParameter = new AAX_CParameter<float>("1", AAX_CString("floatParam"), 0, floatTaper, floatDisplay);
    AAX_CParameter<int32_t>* intParameter = new AAX_CParameter<int32_t>("", AAX_CString("intParam"), 0, intTaper, intDisplay);
    AAX_CParameter<uint32_t>* controlStringParameter = new AAX_CParameter<uint32_t>("3", AAX_CString("controlStringParam"), 0, uintTaper, uintDisplay);
    
    EXPECT_EQ(int32_t(0), parameterManager->NumParameters());
    parameterManager->AddParameter(floatParameter);
    parameterManager->AddParameter(intParameter);
    parameterManager->AddParameter(controlStringParameter);
    EXPECT_EQ(floatParameter, parameterManager->GetParameter(0));
    EXPECT_EQ(intParameter, parameterManager->GetParameter(1));
    EXPECT_EQ(controlStringParameter, parameterManager->GetParameter(2));
    EXPECT_TRUE(parameterManager->GetParameter(25) == 0);
}

TEST_F(AAX_Migrated_ParameterManager_Test, GetParameterByID)
{
    // Manager takes ownership of these in the tests. Manager is responsible for deletion.
    AAX_CLinearTaperDelegate<float> floatTaper;
    AAX_CNumberDisplayDelegate<float> floatDisplay;
    AAX_CLinearTaperDelegate<int32_t> intTaper;
    AAX_CNumberDisplayDelegate<int32_t> intDisplay;
    AAX_CLinearTaperDelegate<uint32_t> uintTaper;
    AAX_CNumberDisplayDelegate<uint32_t> uintDisplay;
    AAX_CParameter<float>* floatParameter = new AAX_CParameter<float>("1", AAX_CString("floatParam"), 0, floatTaper, floatDisplay);
    AAX_CParameter<int32_t>* intParameter = new AAX_CParameter<int32_t>("", AAX_CString("intParam"), 0, intTaper, intDisplay);
    AAX_CParameter<uint32_t>* controlStringParameter = new AAX_CParameter<uint32_t>("3", AAX_CString("controlStringParam"), 0, uintTaper, uintDisplay);

    EXPECT_EQ(int32_t(0), parameterManager->NumParameters());
    parameterManager->AddParameter(floatParameter);
    parameterManager->AddParameter(intParameter);
    parameterManager->AddParameter(controlStringParameter);
    EXPECT_EQ(floatParameter, parameterManager->GetParameterByID("1"));
    EXPECT_EQ(intParameter, parameterManager->GetParameterByID(""));
    EXPECT_EQ(controlStringParameter, parameterManager->GetParameterByID("3"));
    EXPECT_TRUE(parameterManager->GetParameterByID("25") == 0);
}

TEST_F(AAX_Migrated_ParameterManager_Test, GetParameterByName)
{
    // Manager takes ownership of these in the tests. Manager is responsible for deletion.
    AAX_CLinearTaperDelegate<float> floatTaper;
    AAX_CNumberDisplayDelegate<float> floatDisplay;
    AAX_CLinearTaperDelegate<int32_t> intTaper;
    AAX_CNumberDisplayDelegate<int32_t> intDisplay;
    AAX_CLinearTaperDelegate<uint32_t> uintTaper;
    AAX_CNumberDisplayDelegate<uint32_t> uintDisplay;
    AAX_CParameter<float>* floatParameter = new AAX_CParameter<float>("1", AAX_CString("floatParam"), 0, floatTaper, floatDisplay);
    AAX_CParameter<int32_t>* intParameter = new AAX_CParameter<int32_t>("", AAX_CString("intParam"), 0, intTaper, intDisplay);
    AAX_CParameter<uint32_t>* controlStringParameter = new AAX_CParameter<uint32_t>("3", AAX_CString("controlStringParam"), 0, uintTaper, uintDisplay);

    EXPECT_EQ(int32_t(0), parameterManager->NumParameters());
    parameterManager->AddParameter(floatParameter);
    parameterManager->AddParameter(intParameter);
    parameterManager->AddParameter(controlStringParameter);
    EXPECT_TRUE(nullptr == parameterManager->GetParameterByName("1"));
    EXPECT_EQ(floatParameter, parameterManager->GetParameterByName("floatParam"));
    EXPECT_EQ(intParameter, parameterManager->GetParameterByName("intParam"));
    EXPECT_EQ(controlStringParameter, parameterManager->GetParameterByName("controlStringParam"));

    // ambiguous name resolution
    {
        AAX_CParameter<float>* floatParameter2 = new AAX_CParameter<float>("1.2", AAX_CString("floatParam"), 0, floatTaper, floatDisplay);
        parameterManager->AddParameter(floatParameter2);
        EXPECT_EQ(int32_t(4), parameterManager->NumParameters());
        const AAX_IParameter* const ambiguousFloatParam = parameterManager->GetParameterByName("floatParam");
        EXPECT_TRUE(nullptr != ambiguousFloatParam);
        EXPECT_TRUE((floatParameter == ambiguousFloatParam) || (floatParameter2 == ambiguousFloatParam));
    }
}

TEST_F(AAX_Migrated_ParameterManager_Test, DuplicateParameterIdentifiers)
{
    // Manager takes ownership of these in the tests. Manager is responsible for deletion.
    AAX_CLinearTaperDelegate<float> floatTaper;
    AAX_CNumberDisplayDelegate<float> floatDisplay;
    AAX_CLinearTaperDelegate<int32_t> intTaper;
    AAX_CNumberDisplayDelegate<int32_t> intDisplay;
    AAX_CLinearTaperDelegate<uint32_t> uintTaper;
    AAX_CNumberDisplayDelegate<uint32_t> uintDisplay;
    AAX_CParameter<float>* floatParameter = new AAX_CParameter<float>("1", AAX_CString("floatParam"), 0, floatTaper, floatDisplay);
    AAX_CParameter<int32_t>* intParameter = new AAX_CParameter<int32_t>("", AAX_CString("intParam"), 0, intTaper, intDisplay);
    AAX_CParameter<uint32_t>* controlStringParameter = new AAX_CParameter<uint32_t>("3", AAX_CString("controlStringParam"), 0, uintTaper, uintDisplay);
    AAX_CParameter<float>* floatParameter2 = new AAX_CParameter<float>("1", AAX_CString("floatParam2"), 0, floatTaper, floatDisplay);

    EXPECT_EQ(int32_t(0), parameterManager->NumParameters());
    parameterManager->AddParameter(floatParameter);
    parameterManager->AddParameter(intParameter);
    parameterManager->AddParameter(controlStringParameter);
    EXPECT_EQ(floatParameter, parameterManager->GetParameterByID("1"));
    EXPECT_EQ(intParameter, parameterManager->GetParameterByID(""));
    EXPECT_EQ(controlStringParameter, parameterManager->GetParameterByID("3"));

    // Same ID, so it should fail and leave the first parameter in there.
    parameterManager->AddParameter(floatParameter2);
    EXPECT_EQ(int32_t(3), parameterManager->NumParameters());
    EXPECT_EQ(floatParameter, parameterManager->GetParameterByID("1"));
}

//==============================================================================
// Typed Parameter Tests
//==============================================================================

TEST_F(AAX_Migrated_ParameterManager_Test, AddLinearFloatParameter)
{
    AAX_CParameter<float>* floatParameter = new AAX_CParameter<float>("1", AAX_CString("param1"), 50.0f, AAX_CLinearTaperDelegate<float>(0.0f, 100.0f), AAX_CNumberDisplayDelegate<float>());
    AAX_CParameter<float>* floatParameter2 = new AAX_CParameter<float>("", AAX_CString("param2"), 0.0f, AAX_CLinearTaperDelegate<float>(-100.0f, 100.0f), AAX_CNumberDisplayDelegate<float>());

    parameterManager->AddParameter(floatParameter);
    parameterManager->AddParameter(floatParameter2);
    AAX_IParameter* parameter1 = parameterManager->GetParameterByID("1");
    EXPECT_TRUE(parameter1 != nullptr);
    AAX_IParameter* parameter2 = parameterManager->GetParameterByID("");
    EXPECT_TRUE(parameter2 != nullptr);
    AAX_CParameter<float>* floatParam1 = dynamic_cast<AAX_CParameter<float>*>(parameter1);
    EXPECT_TRUE(floatParam1 != nullptr);
    AAX_CParameter<float>* floatParam2 = dynamic_cast<AAX_CParameter<float>*>(parameter2);
    EXPECT_TRUE(floatParam2 != nullptr);
    if (floatParam1 && floatParam2)
    {
        EXPECT_EQ(50.0f, floatParam1->GetValue());
        EXPECT_EQ(0.0f, floatParam2->GetValue());
        EXPECT_EQ(0.5, floatParam1->GetNormalizedValue());
        EXPECT_EQ(0.5, floatParam2->GetNormalizedValue());
    }
}

TEST_F(AAX_Migrated_ParameterManager_Test, AddLinearIntegerParameter)
{
    AAX_CParameter<int32_t>* intParameter = new AAX_CParameter<int32_t>("1", AAX_CString("param1"), 50, AAX_CLinearTaperDelegate<int32_t>(0, 100), AAX_CNumberDisplayDelegate<int32_t>());
    AAX_CParameter<int32_t>* intParameter2 = new AAX_CParameter<int32_t>("", AAX_CString("param2"), 0, AAX_CLinearTaperDelegate<int32_t>(-100, 100), AAX_CNumberDisplayDelegate<int32_t>());

    parameterManager->AddParameter(intParameter);
    parameterManager->AddParameter(intParameter2);
    AAX_IParameter* parameter1 = parameterManager->GetParameterByID("1");
    EXPECT_TRUE(parameter1 != nullptr);
    AAX_IParameter* parameter2 = parameterManager->GetParameterByID("");
    EXPECT_TRUE(parameter2 != nullptr);
    AAX_CParameter<int32_t>* intParam1 = dynamic_cast<AAX_CParameter<int32_t>*>(parameter1);
    EXPECT_TRUE(intParam1 != nullptr);
    AAX_CParameter<int32_t>* intParam2 = dynamic_cast<AAX_CParameter<int32_t>*>(parameter2);
    EXPECT_TRUE(intParam2 != nullptr);
    if (intParam1 && intParam2 && parameter1 && parameter2)
    {
        EXPECT_EQ(50, intParam1->GetValue());
        EXPECT_EQ(0, intParam2->GetValue());
        EXPECT_EQ(0.5, parameter1->GetNormalizedValue());
        EXPECT_EQ(0.5, parameter2->GetNormalizedValue());
    }
}

TEST_F(AAX_Migrated_ParameterManager_Test, AddBinaryBoolParameters)
{
    AAX_CParameter<bool>* boolParameter = new AAX_CParameter<bool>("1", AAX_CString("param1"), false, AAX_CBinaryTaperDelegate<bool>(), AAX_CBinaryDisplayDelegate<bool>("false", "true"));
    AAX_CParameter<bool>* boolParameter2 = new AAX_CParameter<bool>("", AAX_CString("param2"), true, AAX_CBinaryTaperDelegate<bool>(), AAX_CBinaryDisplayDelegate<bool>("false", "true"));

    parameterManager->AddParameter(boolParameter);
    parameterManager->AddParameter(boolParameter2);
    AAX_IParameter* parameter1 = parameterManager->GetParameterByID("1");
    EXPECT_TRUE(parameter1 != nullptr);
    AAX_IParameter* parameter2 = parameterManager->GetParameterByID("");
    EXPECT_TRUE(parameter2 != nullptr);
    AAX_CParameter<bool>* boolParam1 = dynamic_cast<AAX_CParameter<bool>*>(parameter1);
    EXPECT_TRUE(boolParam1 != nullptr);
    AAX_CParameter<bool>* boolParam2 = dynamic_cast<AAX_CParameter<bool>*>(parameter2);
    EXPECT_TRUE(boolParam2 != nullptr);
    if (boolParam1 && boolParam2 && parameter1 && parameter2)
    {
        EXPECT_EQ(false, boolParam1->GetValue());
        EXPECT_EQ(true, boolParam2->GetValue());
        EXPECT_EQ(0, parameter1->GetNormalizedValue());
        EXPECT_EQ(1, parameter2->GetNormalizedValue());
    }
}
