/*================================================================================================*/
/*
 *	Copyright 2013-2017, 2019, 2023-2024 Avid Technology, Inc.
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
 *	\file  AAX_MockPageTable.h
 *
 *	\brief Mock implementation of AAX_IPageTable for unit testing
 *
 */
/*================================================================================================*/


#ifndef AAX_MOCKPAGETABLE_H
#define AAX_MOCKPAGETABLE_H

#include "AAX_IPageTable.h"

//==============================================================================
// Simple Mock Page Table for testing
//==============================================================================

/**
 * \brief Mock implementation of AAX_IPageTable for unit testing
 * 
 * This class provides a simple stub implementation of all AAX_IPageTable
 * methods for use in unit tests. All methods return success or sensible
 * default values without performing any actual page table operations.
 */
class AAX_MockPageTable : public AAX_IPageTable
{
public:
    AAX_MockPageTable() = default;
    ~AAX_MockPageTable() override = default;

    // Implement all pure virtual methods with simple stubs
    AAX_Result Clear() override { return AAX_SUCCESS; }
    
    AAX_Result Empty(AAX_CBoolean& oEmpty) const override { 
        oEmpty = AAX_CBoolean(1); // Always report as empty
        return AAX_SUCCESS; 
    }
    
    AAX_Result GetNumPages(int32_t& oNumPages) const override { 
        oNumPages = 0; // Always report zero pages
        return AAX_SUCCESS; 
    }
    
    AAX_Result InsertPage(int32_t /*iPage*/) override { 
        return AAX_SUCCESS; 
    }
    
    AAX_Result RemovePage(int32_t /*iPage*/) override { 
        return AAX_SUCCESS; 
    }
    
    AAX_Result GetNumMappedParameterIDs(int32_t /*iPage*/, int32_t& oNumParameterIdentifiers) const override { 
        oNumParameterIdentifiers = 0; // Always report zero mapped parameters
        return AAX_SUCCESS; 
    }
    
    AAX_Result ClearMappedParameter(int32_t /*iPage*/, int32_t /*iIndex*/) override { 
        return AAX_SUCCESS; 
    }
    
    AAX_Result GetMappedParameterID(int32_t /*iPage*/, int32_t /*iIndex*/, AAX_IString& /*oParameterIdentifier*/) const override { 
        return AAX_ERROR_UNIMPLEMENTED; // No parameters mapped
    }
    
    AAX_Result MapParameterID(AAX_CPageTableParamID /*iParameterIdentifier*/, int32_t /*iPage*/, int32_t /*iIndex*/) override { 
        return AAX_SUCCESS; 
    }
    
    AAX_Result GetNumParametersWithNameVariations(int32_t& oNumParameterIdentifiers) const override { 
        oNumParameterIdentifiers = 0; // No name variations
        return AAX_SUCCESS; 
    }
    
    AAX_Result GetNameVariationParameterIDAtIndex(int32_t /*iIndex*/, AAX_IString& /*oParameterIdentifier*/) const override { 
        return AAX_ERROR_UNIMPLEMENTED; // No name variations
    }
    
    AAX_Result GetNumNameVariationsForParameter(AAX_CPageTableParamID /*iParameterIdentifier*/, int32_t& oNumVariations) const override { 
        oNumVariations = 0; // No variations for any parameter
        return AAX_SUCCESS; 
    }
    
    AAX_Result GetParameterNameVariationAtIndex(AAX_CPageTableParamID /*iParameterIdentifier*/, int32_t /*iIndex*/, AAX_IString& /*oNameVariation*/, int32_t& oLength) const override {
        oLength = 0;
        return AAX_ERROR_UNIMPLEMENTED; // No variations available
    }

    AAX_Result GetParameterNameVariationOfLength(AAX_CPageTableParamID /*iParameterIdentifier*/, int32_t /*iLength*/, AAX_IString& /*oNameVariation*/) const override {
        return AAX_ERROR_UNIMPLEMENTED; // No variations available
    }

    AAX_Result ClearParameterNameVariations() override {
        return AAX_SUCCESS;
    }

    AAX_Result ClearNameVariationsForParameter(AAX_CPageTableParamID /*iParameterIdentifier*/) override {
        return AAX_SUCCESS;
    }

    AAX_Result SetParameterNameVariation(AAX_CPageTableParamID /*iParameterIdentifier*/, const AAX_IString& /*iNameVariation*/, int32_t /*iLength*/) override {
        return AAX_SUCCESS;
    }
};

#endif // AAX_MOCKPAGETABLE_H
