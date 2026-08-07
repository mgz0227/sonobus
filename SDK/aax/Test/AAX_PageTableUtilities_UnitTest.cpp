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
 *	\file AAX_PageTableUtilities_UnitTest.cpp
 *
 *	\brief Unit tests for AAX_PageTableUtilities template functions
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX_PageTableUtilities.h"
#include "AAX_CString.h"
#include "AAX_Errors.h"
#include <vector>
#include <string>

//--------------------------------------------------------------------
#pragma mark - Mock Page Table Classes

// Mock page table class for testing
class MockPageTable
{
public:
    MockPageTable() = default;
    
    // Add page with parameters
    void AddPage(const std::vector<std::string>& paramIds) {
        mPages.push_back(paramIds);
    }
    
    // Add parameter name variations
    void AddNameVariations(const std::string& paramId, const std::vector<std::string>& variations) {
        mNameVariations[paramId] = variations;
    }
    
    // Set error conditions
    void SetGetNumPagesError(AAX_Result error) { mGetNumPagesError = error; }
    void SetGetNumMappedParameterIDsError(AAX_Result error) { mGetNumMappedParameterIDsError = error; }
    void SetGetMappedParameterIDError(AAX_Result error) { mGetMappedParameterIDError = error; }
    void SetGetNumParametersWithNameVariationsError(AAX_Result error) { mGetNumParametersWithNameVariationsError = error; }
    
    // Mock interface methods
    AAX_Result GetNumPages(int32_t& outNumPages) const {
        if (mGetNumPagesError != AAX_SUCCESS) {
            return mGetNumPagesError;
        }
        outNumPages = static_cast<int32_t>(mPages.size());
        return AAX_SUCCESS;
    }
    
    AAX_Result GetNumMappedParameterIDs(int32_t inPageIndex, int32_t& outNumParameterIDs) const {
        if (mGetNumMappedParameterIDsError != AAX_SUCCESS) {
            return mGetNumMappedParameterIDsError;
        }
        if (inPageIndex < 0 || inPageIndex >= static_cast<int32_t>(mPages.size())) {
            return AAX_ERROR_INVALID_PARAMETER_INDEX;
        }
        outNumParameterIDs = static_cast<int32_t>(mPages[inPageIndex].size());
        return AAX_SUCCESS;
    }
    
    AAX_Result GetMappedParameterID(int32_t inPageIndex, int32_t inParameterIndex, AAX_CString& outParameterID) const {
        if (mGetMappedParameterIDError != AAX_SUCCESS) {
            return mGetMappedParameterIDError;
        }
        if (inPageIndex < 0 || inPageIndex >= static_cast<int32_t>(mPages.size())) {
            return AAX_ERROR_INVALID_PARAMETER_INDEX;
        }
        if (inParameterIndex < 0 || inParameterIndex >= static_cast<int32_t>(mPages[inPageIndex].size())) {
            return AAX_ERROR_INVALID_PARAMETER_INDEX;
        }
        outParameterID = mPages[inPageIndex][inParameterIndex].c_str();
        return AAX_SUCCESS;
    }
    
    AAX_Result GetNumParametersWithNameVariations(int32_t& outNumParameters) const {
        if (mGetNumParametersWithNameVariationsError != AAX_SUCCESS) {
            return mGetNumParametersWithNameVariationsError;
        }
        outNumParameters = static_cast<int32_t>(mNameVariations.size());
        return AAX_SUCCESS;
    }
    
    AAX_Result GetNameVariationParameterIDAtIndex(int32_t inIndex, AAX_CString& outParameterID) const {
        if (inIndex < 0 || inIndex >= static_cast<int32_t>(mNameVariations.size())) {
            return AAX_ERROR_INVALID_PARAMETER_INDEX;
        }
        auto it = mNameVariations.begin();
        std::advance(it, inIndex);
        outParameterID = it->first.c_str();
        return AAX_SUCCESS;
    }
    
    AAX_Result GetNumNameVariationsForParameter(const char* inParameterID, int32_t& outNumVariations) const {
        if (!inParameterID) {
            return AAX_ERROR_NULL_ARGUMENT;
        }
        auto it = mNameVariations.find(inParameterID);
        if (it == mNameVariations.end()) {
            return AAX_ERROR_INVALID_PARAMETER_ID;
        }
        outNumVariations = static_cast<int32_t>(it->second.size());
        return AAX_SUCCESS;
    }
    
    AAX_Result GetNameVariationOfParameterAtIndex(const char* inParameterID, int32_t inVariationIndex, AAX_CString& outVariation) const {
        if (!inParameterID) {
            return AAX_ERROR_NULL_ARGUMENT;
        }
        auto it = mNameVariations.find(inParameterID);
        if (it == mNameVariations.end()) {
            return AAX_ERROR_INVALID_PARAMETER_ID;
        }
        if (inVariationIndex < 0 || inVariationIndex >= static_cast<int32_t>(it->second.size())) {
            return AAX_ERROR_INVALID_PARAMETER_INDEX;
        }
        outVariation = it->second[inVariationIndex].c_str();
        return AAX_SUCCESS;
    }

    AAX_Result GetParameterNameVariationAtIndex(const char* inParameterID, int32_t inVariationIndex, AAX_CString& outVariation, int32_t& outLength) const {
        AAX_Result result = GetNameVariationOfParameterAtIndex(inParameterID, inVariationIndex, outVariation);
        if (result == AAX_SUCCESS) {
            outLength = static_cast<int32_t>(outVariation.Length());
        }
        return result;
    }
    
private:
    std::vector<std::vector<std::string>> mPages;
    std::map<std::string, std::vector<std::string>> mNameVariations;
    
    AAX_Result mGetNumPagesError = AAX_SUCCESS;
    AAX_Result mGetNumMappedParameterIDsError = AAX_SUCCESS;
    AAX_Result mGetMappedParameterIDError = AAX_SUCCESS;
    AAX_Result mGetNumParametersWithNameVariationsError = AAX_SUCCESS;
};

//--------------------------------------------------------------------
#pragma mark - Test fixture

class AAX_PageTableUtilities_Test : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
    
    // Helper to create identical page tables
    void CreateIdenticalPageTables(MockPageTable& table1, MockPageTable& table2) {
        // Add same pages to both tables
        table1.AddPage({"param1", "param2", "param3"});
        table1.AddPage({"param4", "param5"});
        table1.AddPage({"param6"});
        
        table2.AddPage({"param1", "param2", "param3"});
        table2.AddPage({"param4", "param5"});
        table2.AddPage({"param6"});
        
        // Add same name variations
        table1.AddNameVariations("param1", {"variation1a", "variation1b"});
        table1.AddNameVariations("param2", {"variation2a"});
        
        table2.AddNameVariations("param1", {"variation1a", "variation1b"});
        table2.AddNameVariations("param2", {"variation2a"});
    }
    
    void CreateDifferentPageTables(MockPageTable& table1, MockPageTable& table2) {
        // Different page structures
        table1.AddPage({"param1", "param2"});
        table1.AddPage({"param3"});
        
        table2.AddPage({"param1", "param2", "param3"});
        table2.AddPage({"param4"});
        
        // Different name variations
        table1.AddNameVariations("param1", {"variation1a"});
        table2.AddNameVariations("param1", {"variation1b"});
    }
};

//--------------------------------------------------------------------
#pragma mark - Parameter Mapping Comparison Tests

TEST_F(AAX_PageTableUtilities_Test, PageTableParameterMappingsAreEqual_Identical)
{
    MockPageTable table1, table2;
    CreateIdenticalPageTables(table1, table2);
    
    bool result = AAX::PageTableParameterMappingsAreEqual(table1, table2);
    EXPECT_TRUE(result);
}

TEST_F(AAX_PageTableUtilities_Test, PageTableParameterMappingsAreEqual_Different)
{
    MockPageTable table1, table2;
    CreateDifferentPageTables(table1, table2);
    
    bool result = AAX::PageTableParameterMappingsAreEqual(table1, table2);
    EXPECT_FALSE(result);
}

TEST_F(AAX_PageTableUtilities_Test, PageTableParameterMappingsAreEqual_EmptyTables)
{
    MockPageTable table1, table2;
    
    bool result = AAX::PageTableParameterMappingsAreEqual(table1, table2);
    EXPECT_TRUE(result); // Both empty should be equal
}

TEST_F(AAX_PageTableUtilities_Test, PageTableParameterMappingsAreEqual_OneEmpty)
{
    MockPageTable table1, table2;
    table1.AddPage({"param1"});
    
    bool result = AAX::PageTableParameterMappingsAreEqual(table1, table2);
    EXPECT_FALSE(result);
}

TEST_F(AAX_PageTableUtilities_Test, PageTableParameterMappingsAreEqual_DifferentPageCounts)
{
    MockPageTable table1, table2;
    table1.AddPage({"param1"});
    table1.AddPage({"param2"});
    
    table2.AddPage({"param1"});
    
    bool result = AAX::PageTableParameterMappingsAreEqual(table1, table2);
    EXPECT_FALSE(result);
}

TEST_F(AAX_PageTableUtilities_Test, PageTableParameterMappingsAreEqual_DifferentParameterCounts)
{
    MockPageTable table1, table2;
    table1.AddPage({"param1", "param2"});
    table2.AddPage({"param1"});
    
    bool result = AAX::PageTableParameterMappingsAreEqual(table1, table2);
    EXPECT_FALSE(result);
}

TEST_F(AAX_PageTableUtilities_Test, PageTableParameterMappingsAreEqual_DifferentParameterNames)
{
    MockPageTable table1, table2;
    table1.AddPage({"param1", "param2"});
    table2.AddPage({"param1", "param3"});
    
    bool result = AAX::PageTableParameterMappingsAreEqual(table1, table2);
    EXPECT_FALSE(result);
}

TEST_F(AAX_PageTableUtilities_Test, PageTableParameterMappingsAreEqual_DifferentParameterOrder)
{
    MockPageTable table1, table2;
    table1.AddPage({"param1", "param2"});
    table2.AddPage({"param2", "param1"});
    
    bool result = AAX::PageTableParameterMappingsAreEqual(table1, table2);
    EXPECT_FALSE(result); // Order matters
}

//--------------------------------------------------------------------
#pragma mark - Error Handling Tests for Parameter Mappings

TEST_F(AAX_PageTableUtilities_Test, PageTableParameterMappingsAreEqual_GetNumPagesError)
{
    MockPageTable table1, table2;
    table1.SetGetNumPagesError(AAX_ERROR_UNIMPLEMENTED);
    table2.SetGetNumPagesError(AAX_ERROR_UNIMPLEMENTED);
    
    bool result = AAX::PageTableParameterMappingsAreEqual(table1, table2);
    EXPECT_TRUE(result); // Equal errors should return true
}

TEST_F(AAX_PageTableUtilities_Test, PageTableParameterMappingsAreEqual_DifferentGetNumPagesErrors)
{
    MockPageTable table1, table2;
    table1.SetGetNumPagesError(AAX_ERROR_UNIMPLEMENTED);
    table2.SetGetNumPagesError(AAX_SUCCESS);
    table2.AddPage({"param1"});
    
    bool result = AAX::PageTableParameterMappingsAreEqual(table1, table2);
    EXPECT_FALSE(result); // Different errors should return false
}

TEST_F(AAX_PageTableUtilities_Test, PageTableParameterMappingsAreEqual_GetNumMappedParameterIDsError)
{
    MockPageTable table1, table2;
    table1.AddPage({"param1"});
    table2.AddPage({"param1"});
    
    table1.SetGetNumMappedParameterIDsError(AAX_ERROR_UNIMPLEMENTED);
    table2.SetGetNumMappedParameterIDsError(AAX_ERROR_UNIMPLEMENTED);
    
    bool result = AAX::PageTableParameterMappingsAreEqual(table1, table2);
    EXPECT_TRUE(result); // Should skip pages with equal errors
}

TEST_F(AAX_PageTableUtilities_Test, PageTableParameterMappingsAreEqual_GetMappedParameterIDError)
{
    MockPageTable table1, table2;
    table1.AddPage({"param1"});
    table2.AddPage({"param1"});

    table1.SetGetMappedParameterIDError(AAX_ERROR_UNIMPLEMENTED);
    table2.SetGetMappedParameterIDError(AAX_ERROR_UNIMPLEMENTED);

    bool result = AAX::PageTableParameterMappingsAreEqual(table1, table2);
    // NOTE: Implementation treats equal errors as equivalent
    EXPECT_TRUE(result);
}

//--------------------------------------------------------------------
#pragma mark - Name Variation Comparison Tests

TEST_F(AAX_PageTableUtilities_Test, PageTableParameterNameVariationsAreEqual_Identical)
{
    MockPageTable table1, table2;
    CreateIdenticalPageTables(table1, table2);
    
    bool result = AAX::PageTableParameterNameVariationsAreEqual(table1, table2);
    EXPECT_TRUE(result);
}

TEST_F(AAX_PageTableUtilities_Test, PageTableParameterNameVariationsAreEqual_Different)
{
    MockPageTable table1, table2;
    CreateDifferentPageTables(table1, table2);
    
    bool result = AAX::PageTableParameterNameVariationsAreEqual(table1, table2);
    EXPECT_FALSE(result);
}

TEST_F(AAX_PageTableUtilities_Test, PageTableParameterNameVariationsAreEqual_EmptyVariations)
{
    MockPageTable table1, table2;
    
    bool result = AAX::PageTableParameterNameVariationsAreEqual(table1, table2);
    EXPECT_TRUE(result); // Both empty should be equal
}

TEST_F(AAX_PageTableUtilities_Test, PageTableParameterNameVariationsAreEqual_DifferentParameterCounts)
{
    MockPageTable table1, table2;
    table1.AddNameVariations("param1", {"variation1"});
    table1.AddNameVariations("param2", {"variation2"});
    
    table2.AddNameVariations("param1", {"variation1"});
    
    bool result = AAX::PageTableParameterNameVariationsAreEqual(table1, table2);
    EXPECT_FALSE(result);
}

TEST_F(AAX_PageTableUtilities_Test, PageTableParameterNameVariationsAreEqual_DifferentParameterIDs)
{
    MockPageTable table1, table2;
    table1.AddNameVariations("param1", {"variation1"});
    table2.AddNameVariations("param2", {"variation1"});
    
    bool result = AAX::PageTableParameterNameVariationsAreEqual(table1, table2);
    EXPECT_FALSE(result);
}

TEST_F(AAX_PageTableUtilities_Test, PageTableParameterNameVariationsAreEqual_DifferentVariationCounts)
{
    MockPageTable table1, table2;
    table1.AddNameVariations("param1", {"variation1", "variation2"});
    table2.AddNameVariations("param1", {"variation1"});
    
    bool result = AAX::PageTableParameterNameVariationsAreEqual(table1, table2);
    EXPECT_FALSE(result);
}

TEST_F(AAX_PageTableUtilities_Test, PageTableParameterNameVariationsAreEqual_DifferentVariations)
{
    MockPageTable table1, table2;
    table1.AddNameVariations("param1", {"variation1", "variation2"});
    table2.AddNameVariations("param1", {"variation1", "variation3"});
    
    bool result = AAX::PageTableParameterNameVariationsAreEqual(table1, table2);
    EXPECT_FALSE(result);
}

//--------------------------------------------------------------------
#pragma mark - Error Handling Tests for Name Variations

TEST_F(AAX_PageTableUtilities_Test, PageTableParameterNameVariationsAreEqual_GetNumParametersError)
{
    MockPageTable table1, table2;
    table1.SetGetNumParametersWithNameVariationsError(AAX_ERROR_UNIMPLEMENTED);
    table2.SetGetNumParametersWithNameVariationsError(AAX_ERROR_UNIMPLEMENTED);
    
    bool result = AAX::PageTableParameterNameVariationsAreEqual(table1, table2);
    EXPECT_TRUE(result); // Equal errors should return true
}

TEST_F(AAX_PageTableUtilities_Test, PageTableParameterNameVariationsAreEqual_DifferentGetNumParametersErrors)
{
    MockPageTable table1, table2;
    table1.SetGetNumParametersWithNameVariationsError(AAX_ERROR_UNIMPLEMENTED);
    table2.SetGetNumParametersWithNameVariationsError(AAX_SUCCESS);
    
    bool result = AAX::PageTableParameterNameVariationsAreEqual(table1, table2);
    EXPECT_FALSE(result); // Different errors should return false
}

//--------------------------------------------------------------------
#pragma mark - Template Compatibility Tests

TEST_F(AAX_PageTableUtilities_Test, TemplateCompatibility)
{
    // Test that the template functions work with different types
    MockPageTable table1, table2;
    CreateIdenticalPageTables(table1, table2);
    
    // Should work with same types
    bool result1 = AAX::PageTableParameterMappingsAreEqual(table1, table2);
    EXPECT_TRUE(result1);
    
    bool result2 = AAX::PageTableParameterNameVariationsAreEqual(table1, table2);
    EXPECT_TRUE(result2);
    
    // Should work with const references
    const MockPageTable& constTable1 = table1;
    const MockPageTable& constTable2 = table2;
    
    bool result3 = AAX::PageTableParameterMappingsAreEqual(constTable1, constTable2);
    EXPECT_TRUE(result3);
    
    bool result4 = AAX::PageTableParameterNameVariationsAreEqual(constTable1, constTable2);
    EXPECT_TRUE(result4);
}
