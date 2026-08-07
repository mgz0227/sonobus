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
 *	\file AAX_Migrated_InterfaceSize_UnitTest.cpp
 *
 *	\brief Migrated unit tests for Interface Size validation (originally from UnitTest++ framework)
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include <stdint.h>
#include "AAX.h"
#include "AAX_IACFPropertyMap.h"
#include "AAX_IACFEffectDescriptor.h"
#include "AAX_IACFHostServices.h"
#include "AAX_IACFComponentDescriptor.h"
#include "AAX_IACFCollection.h"
#include "AAX_IACFHostProcessorDelegate.h"
#include "AAX_IACFHostProcessor.h"
#include "AAX_IACFEffectDirectData.h"
#include "AAX_IACFPrivateDataAccess.h"
#include "AAX_IACFViewContainer.h"
#include "AAX_IACFTransport.h"
#include "AAX_IACFEffectParameters.h"
#include "AAX_IACFEffectGUI.h"
#include "AAX_IACFController.h"
#include "AAX_IACFAutomationDelegate.h"
#include "AAX_IString.h"

//<DMT>  These tests don't run on Windows right now, but are still useful on Mac.  Actually, Mac testing alone should cover the area I want,
// which is to know that the interfaces didn't change.  For now, let's just test this on Mac.

#if defined(__GNUC__)

//<DMT> Excerpted this code from the Avid Reflection library.  Excerption done to limit dependencies since this is a
// 3P SDK.
template<typename T>
union PMFCaster_t
{
	T		pFn;
	size_t	ptr;
};

template<typename T>
size_t pmf_cast(T pmf)
{
	PMFCaster_t<T> c;
	c.pFn = pmf;
	return c.ptr;
}

//extract_vt_offset - returns offset of pointer to class member function in virtual table of class using pointer to the function
//returned offset is in pointers (i.e. return number of function in the table)
template <typename T>
size_t extract_vt_offset(T pmf)
{
	size_t offs = pmf_cast(pmf);

// I don't know why, but for x86_64 architecture, offset of pointer to class member function
// in virtual table of class for the first function, equals 1, but for arm64 architecture it equals 0.
#if defined(__arm64__)
    offs += size_t(1);
#endif
    
	if(offs & 1)
	{
		return (offs - 1)/sizeof(void*);		//offset in vtable is obtained. see below for MS implementation and think how much gcc is better!!
	}
	else
	{
		assert(false);		//non-virtual function is used ?
	}
}

template <typename T>
int VTableSize()
{
    class VTableCounter
    {  //Note, we could do this with Boost, but I don't want to introduce Boost dependencies into SDK, even for Unit Tests.
    public:
        virtual int Get1() { return 0; }
        virtual int Get2() { return 1; }
        virtual int Get3() { return 2; }
        virtual int Get4() { return 3; }
        virtual int Get5() { return 4; }
        virtual int Get6() { return 5; }
        virtual int Get7() { return 6; }
        virtual int Get8() { return 7; }
        virtual int Get9() { return 8; }
        virtual int Get10() { return 9; }
        virtual int Get11() { return 10; }
        virtual int Get12() { return 11; }
        virtual int Get13() { return 12; }
        virtual int Get14() { return 13; }
        virtual int Get15() { return 14; }
        virtual int Get16() { return 15; }
        virtual int Get17() { return 16; }
        virtual int Get18() { return 17; }
        virtual int Get19() { return 18; }
        virtual int Get20() { return 19; }
        virtual int Get21() { return 20; }
        virtual int Get22() { return 21; }
        virtual int Get23() { return 22; }
        virtual int Get24() { return 23; }
        virtual int Get25() { return 24; }
        virtual int Get26() { return 25; }
        virtual int Get27() { return 26; }
        virtual int Get28() { return 27; }
        virtual int Get29() { return 28; }
        virtual int Get30() { return 29; }
        virtual int Get31() { return 30; }
        virtual int Get32() { return 31; }
        virtual int Get33() { return 32; }
        virtual int Get34() { return 33; }
        virtual int Get35() { return 34; }
        virtual int Get36() { return 35; }
        virtual int Get37() { return 36; }
        virtual int Get38() { return 37; }
        virtual int Get39() { return 38; }
        virtual int Get40() { return 39; }
        virtual int Get41() { return 40; }
        virtual int Get42() { return 41; }
        virtual int Get43() { return 42; }
        virtual int Get44() { return 43; }
        virtual int Get45() { return 44; }
        virtual int Get46() { return 45; }
        virtual int Get47() { return 46; }
        virtual int Get48() { return 47; }
        virtual int Get49() { return 48; }
        virtual int Get50() { return 49; }
        virtual int Get51() { return 50; }
        virtual int Get52() { return 51; }
        virtual int Get53() { return 52; }
        virtual int Get54() { return 53; }
        virtual int Get55() { return 54; }
        virtual int Get56() { return 55; }
        virtual int Get57() { return 56; }
        virtual int Get58() { return 57; }
        virtual int Get59() { return 58; }
        //Might have to add more at some point, but works for now.
    };

    class SubClass : public T
    {
    public:
        virtual int LastVirtual() {return -1;}
    };

    VTableCounter vt;
    return reinterpret_cast<SubClass*>(&vt)->LastVirtual();
}

//==============================================================================
// AAX V1 Interface Test Fixture
//==============================================================================

class AAX_Migrated_AAX_V1_Interface_Test : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

//==============================================================================
// AAX V1 Interface Tests
//==============================================================================

TEST_F(AAX_Migrated_AAX_V1_Interface_Test, IACFUnknown_Size)
{
    EXPECT_EQ(3, VTableSize<IACFUnknown>());
	EXPECT_EQ(size_t(0), extract_vt_offset(&IACFUnknown::QueryInterface));
	EXPECT_EQ(size_t(1), extract_vt_offset(&IACFUnknown::AddRef));
	EXPECT_EQ(size_t(2), extract_vt_offset(&IACFUnknown::Release));
}

TEST_F(AAX_Migrated_AAX_V1_Interface_Test, AAX_IACFPropertyMap_Size)
{
	EXPECT_EQ(6, VTableSize<AAX_IACFPropertyMap>());  //3 from IUnknown
	EXPECT_EQ(size_t(3), extract_vt_offset(&AAX_IACFPropertyMap::GetProperty));
	EXPECT_EQ(size_t(4), extract_vt_offset(&AAX_IACFPropertyMap::AddProperty));
	EXPECT_EQ(size_t(5), extract_vt_offset(&AAX_IACFPropertyMap::RemoveProperty));
}

TEST_F(AAX_Migrated_AAX_V1_Interface_Test, AAX_IACFEffectDescriptor_Size)
{
	EXPECT_EQ(11, VTableSize<AAX_IACFEffectDescriptor>());  //3 from IUnknown
}

TEST_F(AAX_Migrated_AAX_V1_Interface_Test, AAX_IACFHostServices_Size)
{
	EXPECT_EQ(5, VTableSize<AAX_IACFHostServices>());  //3 from IUnknown
}

TEST_F(AAX_Migrated_AAX_V1_Interface_Test, AAX_IACFComponentDescriptor_Size)
{
	EXPECT_EQ(19, VTableSize<AAX_IACFComponentDescriptor>());  //3 from IUnknown
}

TEST_F(AAX_Migrated_AAX_V1_Interface_Test, AAX_IACFCollection_Size)
{
    EXPECT_EQ(6, VTableSize<IACFDefinition>());  //3 from IUnknown
    EXPECT_EQ(7, VTableSize<IACFPluginDefinition>());
	EXPECT_EQ(12, VTableSize<AAX_IACFCollection>());
}

TEST_F(AAX_Migrated_AAX_V1_Interface_Test, AAX_IACFHostProcessorDelegate_Size)
{
    EXPECT_EQ(5, VTableSize<AAX_IACFHostProcessorDelegate>());
}

TEST_F(AAX_Migrated_AAX_V1_Interface_Test, AAX_IACFHostProcessor_Size)
{
    EXPECT_EQ(13, VTableSize<AAX_IACFHostProcessor>());  //3 from IUnknown
}

TEST_F(AAX_Migrated_AAX_V1_Interface_Test, AAX_IACFEffectDirectData_Size)
{
    EXPECT_EQ(6, VTableSize<AAX_IACFEffectDirectData>());
}

TEST_F(AAX_Migrated_AAX_V1_Interface_Test, AAX_IACFPrivateDataAccess_Size)
{
    EXPECT_EQ(5, VTableSize<AAX_IACFPrivateDataAccess>());
}

TEST_F(AAX_Migrated_AAX_V1_Interface_Test, AAX_IACFViewContainer_Size)
{
    EXPECT_EQ(10, VTableSize<AAX_IACFViewContainer>());
}

TEST_F(AAX_Migrated_AAX_V1_Interface_Test, AAX_IACFTransport_Size)
{
    EXPECT_EQ(13, VTableSize<AAX_IACFTransport>());
}

TEST_F(AAX_Migrated_AAX_V1_Interface_Test, AAX_IACFEffectParameters_Size)
{
    EXPECT_EQ(45, VTableSize<AAX_IACFEffectParameters>());
}

TEST_F(AAX_Migrated_AAX_V1_Interface_Test, AAX_IACFEffectGUI_Size)
{
    EXPECT_EQ(13, VTableSize<AAX_IACFEffectGUI>());
}

TEST_F(AAX_Migrated_AAX_V1_Interface_Test, AAX_IACFController_Size)
{
    EXPECT_EQ(20, VTableSize<AAX_IACFController>());
}

TEST_F(AAX_Migrated_AAX_V1_Interface_Test, AAX_IACFAutomationDelegate_Size)
{
    EXPECT_EQ(10, VTableSize<AAX_IACFAutomationDelegate>());
}

TEST_F(AAX_Migrated_AAX_V1_Interface_Test, AAX_IString_Size)
{
    EXPECT_EQ(8, VTableSize<AAX_IString>());      //<DMT>  This is not an ACF interface.  Works a little different.
}

//==============================================================================
// AAX V2 Interface Test Fixture
//==============================================================================

class AAX_Migrated_AAX_V2_Interface_Test : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

//==============================================================================
// AAX V2 Interface Tests
//==============================================================================

TEST_F(AAX_Migrated_AAX_V2_Interface_Test, AAX_IACFEffectDescriptor_V2_Size)
{
	EXPECT_EQ(12, VTableSize<AAX_IACFEffectDescriptor_V2>());
}

TEST_F(AAX_Migrated_AAX_V2_Interface_Test, AAX_IACFTransport_V2_Size)
{
    //<DMT> This was bumped from 16 to 17 after the AAX 2.0.1 SDK after we determined it was safe to do so.
    // We specifically added a host function to query whether the metronome was engaged to fix issues with Click II.
    EXPECT_EQ(17, VTableSize<AAX_IACFTransport_V2>());
}

TEST_F(AAX_Migrated_AAX_V2_Interface_Test, AAX_IACFEffectParameters_V2_Size)
{
    EXPECT_EQ(48, VTableSize<AAX_IACFEffectParameters_V2>());
}

TEST_F(AAX_Migrated_AAX_V2_Interface_Test, AAX_IACFController_V2_Size)
{
    EXPECT_EQ(24, VTableSize<AAX_IACFController_V2>());
}

TEST_F(AAX_Migrated_AAX_V2_Interface_Test, AAX_IACFPropertyMap_V2_Size)
{
	EXPECT_EQ(8, VTableSize<AAX_IACFPropertyMap_V2>());
}

TEST_F(AAX_Migrated_AAX_V2_Interface_Test, AAX_IACFComponentDescriptor_V2_Size)
{
	EXPECT_EQ(20, VTableSize<AAX_IACFComponentDescriptor_V2>());
}

#endif
