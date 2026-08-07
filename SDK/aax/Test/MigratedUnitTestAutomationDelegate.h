/*================================================================================================*/
/*
 *	Copyright 2010-2015, 2018-2021, 2023-2024 Avid Technology, Inc.
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
 *  \file   MigratedUnitTestAutomationDelegate.h
 *
 *  \author Dave Tremblay
 *
 */
/*================================================================================================*/

#ifndef MIGRATEDUNITTESTAUTOMATIONDELEGATE_H
#define MIGRATEDUNITTESTAUTOMATIONDELEGATE_H

#include "AAX_IACFEffectParameters.h"
#include "AAX_IACFAutomationDelegate.h"
#include "AAX_Errors.h"
#include "CACFUnknown.h"
#include "acfextras.h"
#include "ACFPtr.h"


class MigratedUnitTestAutomationDelegate :	public AAX_IACFAutomationDelegate, 
											public CACFUnknown
{
public:
	//ACF Stuff
	ACF_DECLARE_STANDARD_UNKNOWN()
	ACFMETHOD(InternalQueryInterface)(const acfIID & riid, void **ppvObjOut) override;
	typedef ACFPtr<MigratedUnitTestAutomationDelegate> SmartPtr;
	
	// constructor/destructor
	MigratedUnitTestAutomationDelegate();
	
	//From IACFAutomationDelegate
	virtual AAX_Result	Initialize(const char* , uint32_t )							{	return AAX_SUCCESS; }
	virtual AAX_Result	Uninitialize()																		{   return AAX_SUCCESS; }
	virtual AAX_Result	RegisterParameter( AAX_CParamID ) override									{   return AAX_SUCCESS; }
	virtual AAX_Result	UnregisterParameter( AAX_CParamID ) override									{   return AAX_SUCCESS; }
	virtual AAX_Result	PostCurrentValue( AAX_CParamID , double ) const override		{	return AAX_SUCCESS; }
	virtual AAX_Result	PostTouchRequest( AAX_CParamID  ) override		{	return AAX_SUCCESS; }
	virtual AAX_Result	PostReleaseRequest( AAX_CParamID  ) override		{	return AAX_SUCCESS; }
	virtual AAX_Result	PostSetValueRequest( AAX_CParamID , double normalizedValue ) const override;
	virtual AAX_Result	GetTouchState ( AAX_CParamID , AAX_CBoolean * outTouched ) override				{ *outTouched = false; return AAX_SUCCESS; }
	
	//This stuff is to be able to shortcut the automation system.
	void	SetEffectParameters(AAX_IACFEffectParameters* params)											{	mEffectParameters = params; }
	
private:
	AAX_IACFEffectParameters*	mEffectParameters;
};

#endif //MIGRATEDUNITTESTAUTOMATIONDELEGATE_H
