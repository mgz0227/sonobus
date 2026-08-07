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
 *	\file AAX_Migrated_EffectParameters_UnitTest.cpp
 *
 *	\brief Migrated unit tests for AAX_CEffectParameters (originally from UnitTest++ framework)
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

// Test effect parameters class
class EffectParametersBasicTestObject : public AAX_CEffectParameters
{
public:
    EffectParametersBasicTestObject() : AAX_CEffectParameters() {}
    
    virtual AAX_Result EffectInit() override
    {
        AAX_CLinearTaperDelegate<float> taper(-100, 100);
        AAX_CNumberDisplayDelegate<float> display;
        AAX_CParameter<float>* param = new AAX_CParameter<float>("1", AAX_CString("LongName"), 0, taper, display);
        param->AddShortenedName(AAX_CString("LongNm"));
        param->AddShortenedName(AAX_CString("LngNm"));
        
        mParameterManager.AddParameter(param);
        return AAX_SUCCESS;
    }
};

//==============================================================================
// Test Fixture
//==============================================================================

class AAX_Migrated_EffectParameters_Test : public ::testing::Test
{
protected:
    void SetUp() override
    {
        automationDelegate = new MigratedUnitTestAutomationDelegate();
        effectParameters = new EffectParametersBasicTestObject();
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
// Effect Parameters Tests
//==============================================================================

TEST_F(AAX_Migrated_EffectParameters_Test, GetParameterNameOfLength)
{
    AAX_CString abbreviatedName;
    const AAX_CParamID paramID = "1";
    
    // Verify parameter ID
    {
        int32_t paramIndex;
        AAX_CString paramIDForIndex;
        EXPECT_EQ(AAX_SUCCESS, effectParameters->GetParameterIndex(paramID, &paramIndex));
        EXPECT_EQ(AAX_SUCCESS, effectParameters->GetParameterIDFromIndex(paramIndex, &paramIDForIndex));
        EXPECT_STREQ(paramID, paramIDForIndex.CString());
        
        AAX_IParameter* param = nullptr;
        EXPECT_EQ(AAX_SUCCESS, effectParameters->GetParameter(paramID, &param));
        EXPECT_NE(nullptr, param);
    }
    
    // Method should never return a string that is longer than the requested length
    {
        EXPECT_EQ(AAX_SUCCESS, effectParameters->GetParameterNameOfLength(paramID, &abbreviatedName, 1023));
        EXPECT_EQ(8U, abbreviatedName.Length());
        EXPECT_STREQ("LongName", abbreviatedName.CString());
        
        EXPECT_EQ(AAX_SUCCESS, effectParameters->GetParameterNameOfLength(paramID, &abbreviatedName, 31));
        EXPECT_EQ(8U, abbreviatedName.Length());
        EXPECT_STREQ("LongName", abbreviatedName.CString());
        
        EXPECT_EQ(AAX_SUCCESS, effectParameters->GetParameterNameOfLength(paramID, &abbreviatedName, 8));
        EXPECT_EQ(8U, abbreviatedName.Length());
        EXPECT_STREQ("LongName", abbreviatedName.CString());
        
        EXPECT_EQ(AAX_SUCCESS, effectParameters->GetParameterNameOfLength(paramID, &abbreviatedName, 7));
        EXPECT_EQ(6U, abbreviatedName.Length());
        EXPECT_STREQ("LongNm", abbreviatedName.CString());
        
        EXPECT_EQ(AAX_SUCCESS, effectParameters->GetParameterNameOfLength(paramID, &abbreviatedName, 6));
        EXPECT_EQ(6U, abbreviatedName.Length());
        EXPECT_STREQ("LongNm", abbreviatedName.CString());
        
        EXPECT_EQ(AAX_SUCCESS, effectParameters->GetParameterNameOfLength(paramID, &abbreviatedName, 5));
        EXPECT_EQ(5U, abbreviatedName.Length());
        EXPECT_STREQ("LngNm", abbreviatedName.CString());
        
        EXPECT_EQ(AAX_SUCCESS, effectParameters->GetParameterNameOfLength(paramID, &abbreviatedName, 4));
        EXPECT_EQ(4U, abbreviatedName.Length());
        EXPECT_STREQ("LngN", abbreviatedName.CString());
        
        EXPECT_EQ(AAX_SUCCESS, effectParameters->GetParameterNameOfLength(paramID, &abbreviatedName, 3));
        EXPECT_EQ(3U, abbreviatedName.Length());
        EXPECT_STREQ("Lng", abbreviatedName.CString());
        
        EXPECT_EQ(AAX_SUCCESS, effectParameters->GetParameterNameOfLength(paramID, &abbreviatedName, 2));
        EXPECT_EQ(2U, abbreviatedName.Length());
        EXPECT_STREQ("Ln", abbreviatedName.CString());
        
        EXPECT_EQ(AAX_SUCCESS, effectParameters->GetParameterNameOfLength(paramID, &abbreviatedName, 1));
        EXPECT_EQ(1U, abbreviatedName.Length());
        EXPECT_STREQ("L", abbreviatedName.CString());
        
        EXPECT_EQ(AAX_SUCCESS, effectParameters->GetParameterNameOfLength(paramID, &abbreviatedName, 0));
        EXPECT_EQ(0U, abbreviatedName.Length());
        EXPECT_STREQ("", abbreviatedName.CString());
    }
    
    // Method should fail if given a NULL string output parameter
    {
        EXPECT_NE(AAX_SUCCESS, effectParameters->GetParameterNameOfLength(paramID, nullptr, 31));
        EXPECT_NE(AAX_SUCCESS, effectParameters->GetParameterNameOfLength(paramID, nullptr, 0));
    }
}
