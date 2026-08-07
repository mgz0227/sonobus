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
 *	\file AAX_Migrated_RandomGen_UnitTest.cpp
 *
 *	\brief Migrated unit tests for Random Number Generation (originally from UnitTest++ framework)
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "../TI/SignalProcessing/AAX_RandomGen.h"
#include <array>
#include <limits>

namespace UTReference
{
    /* Original function. Expected to be used as reference
       and take input seeds which lead to ub cases */
    inline int32_t GetInt32RPDF(int32_t* iSeed)
    {
        // Requirement: iSeed param must be a static in the calling function.
    
        int32_t k = int32_t((*iSeed)*AAX::cSeedDivisor);
        *iSeed = 16807 * (*iSeed - k * 127773) - 2836 * k + 7395;
        if (*iSeed < 0) 
	        *iSeed += 2147483647;
        return (*iSeed - 1073741824) * 2;		// -2147483647..+2147483647
    }
}

//==============================================================================
// Test Fixture
//==============================================================================

class AAX_Migrated_RandomGen_Test : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

//==============================================================================
// Random Generation Tests
//==============================================================================

TEST_F(AAX_Migrated_RandomGen_Test, GetInt32RPDFTest)
{
    // Test vectors from the whole range of input values and don't
    // include ub cases
    const std::array<int32_t, 8> testSeeds { (std::numeric_limits<int32_t>::min)(),
                                    -12345667,
                                    -127776,
                                    -12,
                                    0,
                                    4568912,
                                    127776,
                                    (std::numeric_limits<int32_t>::max)()};
    for (const auto& testSeed : testSeeds)
    {
        // Seeds handed over by ptr and modified inside function under test
        // So keeping them in separate vars
        auto testSeed1 = testSeed, testSeed2 = testSeed;
	    EXPECT_EQ(AAX::GetInt32RPDF(&testSeed1), UTReference::GetInt32RPDF(&testSeed2));
    }
}
