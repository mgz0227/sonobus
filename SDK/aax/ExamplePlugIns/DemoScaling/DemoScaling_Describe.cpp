/*================================================================================================*/
/*
 *	Copyright 2026 Avid Technology, Inc.
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
 *	Or: You may also use this code under the terms of the GPL v3(see
 *	www.gnu.org/licenses).
 *	
 *	THE AAX SDK IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
 *	EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
 *	DISCLAIMED.
 */
/*================================================================================================*/

// DemoScaling Includes
#include "DemoScaling_Describe.h"
#include "DemoScaling_Defs.h"
#include "DemoScaling_Alg.h"
#include "DemoScaling_Parameters.h"
#include "DemoScaling_GUI.h"

// AAX Includes
#include "AAX_ICollection.h"
#include "AAX_IComponentDescriptor.h"
#include "AAX_IEffectDescriptor.h"
#include "AAX_IPropertyMap.h"
#include "AAX_Exception.h"
#include "AAX_Errors.h"
#include "AAX_Assert.h"


// ***************************************************************************
// ROUTINE:	DescribeAlgorithmComponent
// Algorithm component description
// ***************************************************************************
static void DescribeAlgorithmComponent(AAX_IComponentDescriptor* outDesc)
{
	AAX_CheckedResult err;
	
	// Describe algorithm's context structure
	//
	// Subscribe context fields to host-provided services or information
	err = outDesc->AddAudioIn(eAlgFieldID_AudioIn);
	err = outDesc->AddAudioOut(eAlgFieldID_AudioOut);
	err = outDesc->AddAudioBufferLength(eAlgFieldID_BufferSize);
    //
	// Register context fields as communications destinations (i.e. input)
	err = outDesc->AddDataInPort(eAlgPortID_BypassIn, sizeof(int32_t));
	
	// Register processing callbacks
	//
	// Create a property map 
	AAX_IPropertyMap* const properties{outDesc->NewPropertyMap()};

	if(!properties) {
		err = AAX_ERROR_NULL_OBJECT;
	}
	//
	// Generic properties
	err = properties->AddProperty(AAX_eProperty_ManufacturerID, cDemoScaling_ManufactureID);
	err = properties->AddProperty(AAX_eProperty_ProductID, cDemoScaling_ProductID);
	err = properties->AddProperty(AAX_eProperty_CanBypass, true);
	//
	// Stem format -specific properties
	err = properties->AddProperty(AAX_eProperty_InputStemFormat, AAX_eStemFormat_Mono);
	err = properties->AddProperty(AAX_eProperty_OutputStemFormat, AAX_eStemFormat_Mono);
	//
	// ID properties
	err = properties->AddProperty(AAX_eProperty_PlugInID_Native, cDemoScaling_PlugInID_Native);
	err = properties->AddProperty(AAX_eProperty_PlugInID_AudioSuite, cDemoScaling_PlugInID_AudioSuite);	// for offline processing
	
	// Register Native callback
	err = properties->AddPointerProperty(AAX_eProperty_NativeProcessProc, reinterpret_cast<const void*>(&DemoScaling_AlgorithmProcessFunction));

	// Register callbacks for all defined entry points
	err = outDesc->AddProcessProc(properties);
}

// ***************************************************************************
// ROUTINE:	DescribeEffect
// ***************************************************************************
static AAX_Result DescribeEffect(AAX_IEffectDescriptor* outDescriptor)
{
	AAX_CheckedResult err;
	AAX_IComponentDescriptor* const compDesc{outDescriptor->NewComponentDescriptor()};

	if(!compDesc) {
		err = AAX_ERROR_NULL_OBJECT;
	}
	
	// Effect identifiers
	//
	err = outDescriptor->AddName("Demo Scaling Example");
	err = outDescriptor->AddName("Demo Scaling");
	err = outDescriptor->AddName("DemoScaling");
	err = outDescriptor->AddName("DmScal");
	err = outDescriptor->AddName("DScl");
	err = outDescriptor->AddName("DS");
	err = outDescriptor->AddCategory(AAX_ePlugInCategory_Example);
	
	// Effect components
	//
	// Algorithm component
	err = compDesc->Clear();
	DescribeAlgorithmComponent(compDesc);
	err = outDescriptor->AddComponent(compDesc);
	//
	// Data model
	err = outDescriptor->AddProcPtr(reinterpret_cast<void*>(DemoScaling_Parameters::Create), kAAX_ProcPtrID_Create_EffectParameters);
	// 
	// User Interface
	err = outDescriptor->AddProcPtr(reinterpret_cast<void*>(DemoScaling_GUI::Create), kAAX_ProcPtrID_Create_EffectGUI);

	return err;
}

// ***************************************************************************
// ROUTINE:	GetEffectDescriptions
// ***************************************************************************
AAX_Result GetEffectDescriptions(AAX_ICollection* outCollection)
{
	AAX_CheckedResult err;
	AAX_IEffectDescriptor* const plugInDescriptor{outCollection->NewDescriptor()};
	if(plugInDescriptor)
	{
		AAX_SWALLOW_MULT(
			err = DescribeEffect(plugInDescriptor);
			err = outCollection->AddEffect(kEffectID_DemoScaling, plugInDescriptor);
		);
	}
	else {
		err = AAX_ERROR_NULL_OBJECT;
	}
	
	err = outCollection->SetManufacturerName("Avid");
	err = outCollection->AddPackageName("DemoScaling AAX SDK Example Plug-In");
	err = outCollection->AddPackageName("DemoScaling");
	err = outCollection->AddPackageName("DmSc");
	err = outCollection->SetPackageVersion(1);
	
	return err;
}
