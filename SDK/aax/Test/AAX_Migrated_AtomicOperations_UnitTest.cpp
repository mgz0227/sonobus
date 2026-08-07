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
 *	\file AAX_Migrated_AtomicOperations_UnitTest.cpp
 *
 *	\brief Migrated unit tests for Atomic Operations (originally from UnitTest++ framework)
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"

// AAX Includes
#include "AAX_Atomic.h"
#include "MigratedAAX_CTestThread.h"

// Standard Includes
#include <iostream>
#include <iomanip>
#include <ctime>
#include <cstdint>
#include <set>

//==============================================================================
// Test Timer Utility
//==============================================================================

struct StTestTimer
{
	explicit StTestTimer(const char* inTestName) : mStart(std::clock()), mTestName(inTestName) {}
	
	~StTestTimer()
	{
		const std::clock_t end = std::clock();
		double duration = (1000.0 * (end - mStart)) / CLOCKS_PER_SEC; // Doesn't handle wraparound
		
		std::ios prevState(nullptr);
		prevState.copyfmt(std::cout);
		std::cout << mTestName << " - " << std::fixed << std::setprecision(2) << "CPU time used: " << duration  << " ms" << std::endl;
		std::cout.copyfmt(prevState);
	}
	
private:
	std::clock_t mStart;
	const char* mTestName;
};

//==============================================================================
// Test Fixture
//==============================================================================

class AAX_Migrated_AtomicOperations_Test : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

//==============================================================================
// Utility Classes and Type Definitions
//==============================================================================

#if (AAX_PointerSize == AAXPointer_64bit)
typedef uint64_t TUIntPtr;
#elif (AAX_PointerSize == AAXPointer_32bit)
typedef uint32_t TUIntPtr;
#else
#error Unsupported pointer size
#endif

template <int NUM_ITER_PER_THREAD>
class IncFunc_32 : public MigratedAAX_CTestThread::Functor
{
public:
	virtual ~IncFunc_32() {}
	explicit IncFunc_32(uint32_t& inTestInt)
	: mTestInt(inTestInt)
	, mNumIncrements(0)
	{
	}

	virtual void Run(void)
	{
		for (int i = 0; i < NUM_ITER_PER_THREAD; ++i)
		{
			AAX_Atomic_IncThenGet_32(mTestInt);
			AAX_Atomic_IncThenGet_32(mNumIncrements);
		}
	}

	uint32_t GetNumIncrements() const
	{
		return mNumIncrements;
	}

private:
	uint32_t& mTestInt;
	uint32_t mNumIncrements;
};

template <int NUM_ITER_PER_THREAD>
class DecFunc_32 : public MigratedAAX_CTestThread::Functor
{
public:
	virtual ~DecFunc_32() {}
	explicit DecFunc_32(uint32_t& inTestInt)
	: mTestInt(inTestInt)
	, mNumDecrements(0)
	{
	}

	virtual void Run(void)
	{
		for (int i = 0; i < NUM_ITER_PER_THREAD; ++i)
		{
			AAX_Atomic_DecThenGet_32(mTestInt);
			AAX_Atomic_IncThenGet_32(mNumDecrements);
		}
	}

	uint32_t GetNumDecrements() const
	{
		return mNumDecrements;
	}

private:
	uint32_t& mTestInt;
	uint32_t mNumDecrements;
};

// Test AAX_Atomic_Exchange_32
// Create one functor per thread
// All functors must use the same inTestValues set
class ExcFunc_32 : public MigratedAAX_CTestThread::Functor
{
public:
	typedef uint32_t TInt;

public:
	virtual ~ExcFunc_32() {}
	explicit ExcFunc_32(TInt& inTestInt, const std::set<TInt>& inTestValues, int inNumIterations)
	: mTestInt(inTestInt)
	, mTestValues(inTestValues)
	, mNumIterations(inNumIterations)
	{
		// use current time as seed for random generator
		static bool sRandSeedDone = false;
		if (false == sRandSeedDone)
		{
			sRandSeedDone = true;
			std::srand((unsigned)std::time(0));
		}
	}

	virtual void Run(void)
	{
		std::set<TInt>::const_iterator valIter = mTestValues.begin();

		for (int i = 0; i < mNumIterations; ++i)
		{
			// Go to a random element in mTestValues
			for (int incCount = std::rand() % (int)mTestValues.size();
				 incCount > 0;
				 --incCount)
			{
				if (mTestValues.end() == ++valIter)
				{
					valIter = mTestValues.begin();
				}
			}

			const TInt val = AAX_Atomic_Exchange_32(mTestInt, *valIter);
			EXPECT_TRUE(0 < mTestValues.count(val));
		}
	}

private:
	TInt& mTestInt;
	const std::set<TInt>& mTestValues;
	int mNumIterations;
};

// Test AAX_Atomic_Exchange_64
// Create one functor per thread
// All functors must use the same inTestValues set
class ExcFunc_64 : public MigratedAAX_CTestThread::Functor
{
public:
	typedef uint64_t TInt;

public:
	virtual ~ExcFunc_64() {}
	explicit ExcFunc_64(TInt& inTestInt, const std::set<TInt>& inTestValues, int inNumIterations)
	: mTestInt(inTestInt)
	, mTestValues(inTestValues)
	, mNumIterations(inNumIterations)
	{
		// use current time as seed for random generator
		static bool sRandSeedDone = false;
		if (false == sRandSeedDone)
		{
			sRandSeedDone = true;
			std::srand((unsigned)std::time(0));
		}
	}

	virtual void Run(void)
	{
		std::set<TInt>::const_iterator valIter = mTestValues.begin();

		for (int i = 0; i < mNumIterations; ++i)
		{
			// Go to a random element in mTestValues
			for (int incCount = std::rand() % (int)mTestValues.size();
				 incCount > 0;
				 --incCount)
			{
				if (mTestValues.end() == ++valIter)
				{
					valIter = mTestValues.begin();
				}
			}

			const TInt val = AAX_Atomic_Exchange_64(mTestInt, *valIter);
			EXPECT_TRUE(0 < mTestValues.count(val));
		}
	}

private:
	TInt& mTestInt;
	const std::set<TInt>& mTestValues;
	int mNumIterations;
};

// Test AAX_Atomic_CompareAndExchange_32
// Create one functor per thread
// All functors must use the same inTestValues set
class CNXFunc_32 : public MigratedAAX_CTestThread::Functor
{
public:
	typedef uint32_t TInt;

public:
	virtual ~CNXFunc_32() {}
	explicit CNXFunc_32(TInt& inTestInt, const std::set<TInt>& inTestValues, int inNumIterations)
	: mTestInt(inTestInt)
	, mLastVal(0)
	, mTestValues(inTestValues)
	, mNumExchanged(0)
	, mNumNotExchanged(0)
	, mNumIterations(inNumIterations)
	{
		// use current time as seed for random generator
		static bool sRandSeedDone = false;
		if (false == sRandSeedDone)
		{
			sRandSeedDone = true;
			std::srand((unsigned)std::time(0));
		}
	}

	virtual void Run(void)
	{
		std::set<TInt>::const_iterator valIter = mTestValues.begin();

		for (int i = 0; i < mNumIterations; ++i)
		{
			// Go to a random element in mTestValues
			for (int incCount = std::rand() % (int)mTestValues.size();
				 incCount > 0;
				 --incCount)
			{
				if (mTestValues.end() == ++valIter)
				{
					valIter = mTestValues.begin();
				}
			}

			const bool exchanged = AAX_Atomic_CompareAndExchange_32(mTestInt, mLastVal, *valIter);
			mLastVal = *valIter;

			if (true == exchanged)
			{
				++mNumExchanged;
			}
			else
			{
				++mNumNotExchanged;
			}

			// Sleep between 0 and 100 us, or the minimum
			// supported by this platform
			AAXThread::SleepMS((double)(std::rand() % 1000) / 1000.);
		}
	}

	void CheckResults(void) const
	{
		EXPECT_EQ(mNumIterations, mNumExchanged + mNumNotExchanged); // Sanity check
//		EXPECT_TRUE(0 < mNumExchanged); // If we want to validate that exchanges are happening with the current test parameterization
//		EXPECT_TRUE(0 < mNumNotExchanged); // If we want to validate that non-exchanges are happening with the current test parameterization
	}

private:
	TInt& mTestInt;
	TInt mLastVal;
	const std::set<TInt>& mTestValues;
	int mNumExchanged;
	int mNumNotExchanged;
	int mNumIterations;
};

// Test AAX_Atomic_CompareAndExchange_64
// Create one functor per thread
// All functors must use the same inTestValues set
class CNXFunc_64 : public MigratedAAX_CTestThread::Functor
{
public:
	typedef uint64_t TInt;

public:
	virtual ~CNXFunc_64() {}
	explicit CNXFunc_64(TInt& inTestInt, const std::set<TInt>& inTestValues, int inNumIterations)
	: mTestInt(inTestInt)
	, mLastVal(0)
	, mTestValues(inTestValues)
	, mNumExchanged(0)
	, mNumNotExchanged(0)
	, mNumIterations(inNumIterations)
	{
		// use current time as seed for random generator
		static bool sRandSeedDone = false;
		if (false == sRandSeedDone)
		{
			sRandSeedDone = true;
			std::srand((unsigned)std::time(0));
		}
	}

	virtual void Run(void)
	{
		std::set<TInt>::const_iterator valIter = mTestValues.begin();

		for (int i = 0; i < mNumIterations; ++i)
		{
			// Go to a random element in mTestValues
			for (int incCount = std::rand() % (int)mTestValues.size();
				 incCount > 0;
				 --incCount)
			{
				if (mTestValues.end() == ++valIter)
				{
					valIter = mTestValues.begin();
				}
			}

			const bool exchanged = AAX_Atomic_CompareAndExchange_64(mTestInt, mLastVal, *valIter);
			mLastVal = *valIter;

			if (true == exchanged)
			{
				++mNumExchanged;
			}
			else
			{
				++mNumNotExchanged;
			}

			// Sleep between 0 and 1000 us, or the minimum
			// supported by this platform
			AAXThread::SleepMS((double)(std::rand() % 1000)/1000.);
		}
	}

	void CheckResults(void) const
	{
		EXPECT_EQ(mNumIterations, mNumExchanged + mNumNotExchanged); // Sanity check
//		EXPECT_TRUE(0 < mNumExchanged); // If we want to validate that exchanges are happening with the current test parameterization
//		EXPECT_TRUE(0 < mNumNotExchanged); // If we want to validate that non-exchanges are happening with the current test parameterization
	}

private:
	TInt& mTestInt;
	TInt mLastVal;
	const std::set<TInt>& mTestValues;
	int mNumExchanged;
	int mNumNotExchanged;
	int mNumIterations;
};

// Test AAX_Atomic_Load_Pointer
// Create one functor per thread
//
// In order for the test to be relevant, this functor checks that the pointer value has changed
// at least once between two consecutive load calls made from the same thread.
// * If the value has changed at least once then that is a good indication that there was a
//   concurrent call to set the pointer value handled at the same time as the first load call.
class LoadFunc : public MigratedAAX_CTestThread::Functor
{
public:
	virtual ~LoadFunc() {}
	explicit LoadFunc(TUIntPtr& inTestPtr, const std::set<TUIntPtr>& inTestValues, int inNumIterations)
	: mTestPtr(inTestPtr)
	, mTestValues(inTestValues)
	, mNumDifferent(0)
	, mNumIterations(inNumIterations)
    , MAX_TICK_COUNT(100000)
	{
		// use current time as seed for random generator
		static bool sRandSeedDone = false;
		if (false == sRandSeedDone)
		{
			sRandSeedDone = true;
			std::srand((unsigned)std::time(0));
		}
	}

	virtual void Run(void)
	{
        // Load and wait until someone change value
        int tickCount = 0;
		for (int i = 0; i < mNumIterations; ++i)
		{
            tickCount = 0;
			// Load the current value
			void* const val1 = AAX_Atomic_Load_Pointer((void* const *)&mTestPtr);
			// Spin while someone changes value from other thread
            while (val1 == AAX_Atomic_Load_Pointer((void* const *)&mTestPtr) && ++tickCount < MAX_TICK_COUNT)
                ;

            // Integrity check - loaded value is in the set of test values
            EXPECT_TRUE(0UL < mTestValues.count((TUIntPtr)val1));
            //
            if (tickCount <= MAX_TICK_COUNT)
                ++mNumDifferent;

			AAXThread::SurrenderCPU();
		}
	}

	void CheckResults(void) const
	{
		EXPECT_TRUE(0 < mNumDifferent);
	}

private:
	TUIntPtr& mTestPtr;
	const std::set<TUIntPtr>& mTestValues;
	int mNumDifferent;
	int mNumIterations;
    const int MAX_TICK_COUNT;
};

//==============================================================================
// Atomic Operations Tests
//==============================================================================

TEST_F(AAX_Migrated_AtomicOperations_Test, SingleThreaded)
{
	const size_t ptrSize = sizeof(void*);
#if AAX_PointerSize == AAXPointer_32bit
	EXPECT_EQ(4U, ptrSize);
#elif AAX_PointerSize == AAXPointer_64bit
	EXPECT_EQ(8U, ptrSize);
#else
#error Unsupported pointer size
#endif

	// AAX_Atomic_IncThenGet_32
	{
		uint32_t dataU32 = 0;

		EXPECT_EQ(1U, AAX_Atomic_IncThenGet_32(dataU32));
		EXPECT_EQ(1U, dataU32);
		EXPECT_EQ(2U, AAX_Atomic_IncThenGet_32(dataU32));
		EXPECT_EQ(2U, dataU32);
		dataU32 = UINT32_MAX-1;
		EXPECT_EQ(UINT32_MAX, AAX_Atomic_IncThenGet_32(dataU32));
		EXPECT_EQ(UINT32_MAX, dataU32);
		EXPECT_EQ(0U, AAX_Atomic_IncThenGet_32(dataU32));
		EXPECT_EQ(0U, dataU32);
	}

	// AAX_Atomic_DecThenGet_32
	{
		uint32_t dataU32 = 2;

		EXPECT_EQ(1U, AAX_Atomic_DecThenGet_32(dataU32));
		EXPECT_EQ(1U, dataU32);
		EXPECT_EQ(0U, AAX_Atomic_DecThenGet_32(dataU32));
		EXPECT_EQ(0U, dataU32);
		EXPECT_EQ(UINT32_MAX, AAX_Atomic_DecThenGet_32(dataU32));
		EXPECT_EQ(UINT32_MAX, dataU32);
		EXPECT_EQ(UINT32_MAX-1, AAX_Atomic_DecThenGet_32(dataU32));
		EXPECT_EQ(UINT32_MAX-1, dataU32);
	}

	// AAX_Atomic_Exchange_32
	{
		uint32_t dataU32_1 = 0;
		uint32_t dataU32_2 = 0;

		EXPECT_EQ(0U, AAX_Atomic_Exchange_32(dataU32_1, 1));
		EXPECT_EQ(1U, dataU32_1);
		EXPECT_EQ(0U, AAX_Atomic_Exchange_32(dataU32_2, UINT32_MAX));
		EXPECT_EQ(UINT32_MAX, dataU32_2);
		EXPECT_EQ(1U, AAX_Atomic_Exchange_32(dataU32_1, dataU32_2));
		EXPECT_EQ(UINT32_MAX, dataU32_1);
		EXPECT_EQ(UINT32_MAX, dataU32_2);
	}

	// AAX_Atomic_Exchange_64
	{
		uint64_t dataU64_1 = 0;
		uint64_t dataU64_2 = 0;

		EXPECT_EQ(0U, AAX_Atomic_Exchange_64(dataU64_1, 1U));
		EXPECT_EQ(1U, dataU64_1);
		EXPECT_EQ(0U, AAX_Atomic_Exchange_64(dataU64_2, UINT32_MAX));
		EXPECT_EQ(UINT32_MAX, dataU64_2);
		EXPECT_EQ(1U, AAX_Atomic_Exchange_64(dataU64_1, dataU64_2));
		EXPECT_EQ(UINT32_MAX, dataU64_1);
		EXPECT_EQ(UINT32_MAX, dataU64_2);

		dataU64_1 = 0;
		dataU64_2 = 0;

		EXPECT_EQ(0U, AAX_Atomic_Exchange_64(dataU64_2, UINT64_MAX));
		EXPECT_EQ(UINT64_MAX, dataU64_2);
		EXPECT_EQ(0U, AAX_Atomic_Exchange_64(dataU64_1, dataU64_2));
		EXPECT_EQ(UINT64_MAX, dataU64_1);
		EXPECT_EQ(UINT64_MAX, dataU64_2);
	}

	// AAX_Atomic_Exchange_Pointer
	{
		void* dataPtr_1 = (void*)0x0;
		void* dataPtr_2 = (void*)0x0;

		EXPECT_EQ((void*)0U, AAX_Atomic_Exchange_Pointer(dataPtr_1, (void*)(uintptr_t)1U));
		EXPECT_EQ((void*)(uintptr_t)1U, dataPtr_1);
		EXPECT_EQ((void*)0U, AAX_Atomic_Exchange_Pointer(dataPtr_2, (void*)(uintptr_t)UINT32_MAX));
		EXPECT_EQ((void*)(uintptr_t)UINT32_MAX, dataPtr_2);
		EXPECT_EQ((void*)(uintptr_t)1U, AAX_Atomic_Exchange_Pointer(dataPtr_1, dataPtr_2));
		EXPECT_EQ((void*)(uintptr_t)UINT32_MAX, dataPtr_1);
		EXPECT_EQ((void*)(uintptr_t)UINT32_MAX, dataPtr_2);
#if AAX_PointerSize == AAXPointer_64bit
		dataPtr_1 = 0x0;
		dataPtr_2 = 0x0;
		EXPECT_EQ((void*)0U, AAX_Atomic_Exchange_Pointer(dataPtr_2, (void*)UINT64_MAX));
		EXPECT_EQ((void*)UINT64_MAX, dataPtr_2);
		EXPECT_EQ((void*)0U, AAX_Atomic_Exchange_Pointer(dataPtr_1, dataPtr_2));
		EXPECT_EQ((void*)UINT64_MAX, dataPtr_1);
		EXPECT_EQ((void*)UINT64_MAX, dataPtr_2);
#endif
	}

	// AAX_Atomic_CompareAndExchange_32
	{
		struct Ints
		{
			uint32_t a;
			uint32_t b;
			uint32_t c;
			uint32_t d;
		} ints;

		ints.a = 0x00000000UL;
		ints.b = 0x00000001UL;
		ints.c = 0x80000000UL;
		ints.d = 0xFFFFFFFFUL;

		EXPECT_FALSE(AAX_Atomic_CompareAndExchange_32(ints.a, ints.b, ints.b));
		EXPECT_FALSE(AAX_Atomic_CompareAndExchange_32(ints.a, ints.b, 0x00000000UL));
		EXPECT_FALSE(AAX_Atomic_CompareAndExchange_32(ints.d, ints.b, ints.b));
		EXPECT_FALSE(AAX_Atomic_CompareAndExchange_32(ints.d, ints.b, 0x00000000UL));
		EXPECT_FALSE(AAX_Atomic_CompareAndExchange_32(ints.d, ints.b, 0xFFFFFFFFUL));

		EXPECT_TRUE(AAX_Atomic_CompareAndExchange_32(ints.a, ints.a, ints.a));
		EXPECT_EQ(0x00000000UL, ints.a);
		EXPECT_TRUE(AAX_Atomic_CompareAndExchange_32(ints.d, ints.d, ints.d));
		EXPECT_EQ(0xFFFFFFFFUL, ints.d);

		EXPECT_TRUE(AAX_Atomic_CompareAndExchange_32(ints.a, ints.a, 0xFFFFFFFFUL));
		EXPECT_EQ(0xFFFFFFFFUL, ints.a);
		EXPECT_TRUE(AAX_Atomic_CompareAndExchange_32(ints.a, ints.a, 0x0000FFFFUL));
		EXPECT_EQ(0x0000FFFFUL, ints.a);
		EXPECT_TRUE(AAX_Atomic_CompareAndExchange_32(ints.a, ints.a, 0x00000000UL));
		EXPECT_EQ(0x00000000UL, ints.a);

		EXPECT_TRUE(AAX_Atomic_CompareAndExchange_32(ints.a, ints.a, ints.b));
		EXPECT_EQ(ints.a, ints.b);
		EXPECT_TRUE(AAX_Atomic_CompareAndExchange_32(ints.a, ints.b, ints.c));
		EXPECT_EQ(ints.a, ints.c);
		EXPECT_TRUE(AAX_Atomic_CompareAndExchange_32(ints.a, ints.c, ints.d));
		EXPECT_EQ(ints.a, ints.d);
		EXPECT_EQ(0x00000001UL, ints.b);
		EXPECT_EQ(0x80000000UL, ints.c);
		EXPECT_EQ(0xFFFFFFFFUL, ints.d);
	}

	// AAX_Atomic_CompareAndExchange_64
	{
		struct Ints
		{
			uint64_t a;
			uint64_t b;
			uint64_t c;
			uint64_t d;
		} ints;

		ints.a = 0x0000000000000000ULL;
		ints.b = 0x0000000000000001ULL;
		ints.c = 0x8000000000000000ULL;
		ints.d = 0xFFFFFFFFFFFFFFFFULL;

		EXPECT_FALSE(AAX_Atomic_CompareAndExchange_64(ints.a, ints.b, ints.b));
		EXPECT_FALSE(AAX_Atomic_CompareAndExchange_64(ints.a, ints.b, 0x0000000000000000ULL));
		EXPECT_FALSE(AAX_Atomic_CompareAndExchange_64(ints.d, ints.b, ints.b));
		EXPECT_FALSE(AAX_Atomic_CompareAndExchange_64(ints.d, ints.b, 0x0000000000000000ULL));
		EXPECT_FALSE(AAX_Atomic_CompareAndExchange_64(ints.d, ints.b, 0xFFFFFFFFFFFFFFFFULL));

		EXPECT_TRUE(AAX_Atomic_CompareAndExchange_64(ints.a, ints.a, ints.a));
		EXPECT_EQ(0x0000000000000000ULL, ints.a);
		EXPECT_TRUE(AAX_Atomic_CompareAndExchange_64(ints.d, ints.d, ints.d));
		EXPECT_EQ(0xFFFFFFFFFFFFFFFFULL, ints.d);

		EXPECT_TRUE(AAX_Atomic_CompareAndExchange_64(ints.a, ints.a, 0xFFFFFFFFFFFFFFFFULL));
		EXPECT_EQ(0xFFFFFFFFFFFFFFFFULL, ints.a);
		EXPECT_TRUE(AAX_Atomic_CompareAndExchange_64(ints.a, ints.a, 0x00000000FFFFFFFFULL));
		EXPECT_EQ(0x00000000FFFFFFFFULL, ints.a);
		EXPECT_TRUE(AAX_Atomic_CompareAndExchange_64(ints.a, ints.a, 0x0000000000000000ULL));
		EXPECT_EQ(0x0000000000000000ULL, ints.a);

		EXPECT_TRUE(AAX_Atomic_CompareAndExchange_64(ints.a, ints.a, ints.b));
		EXPECT_EQ(ints.a, ints.b);
		EXPECT_TRUE(AAX_Atomic_CompareAndExchange_64(ints.a, ints.b, ints.c));
		EXPECT_EQ(ints.a, ints.c);
		EXPECT_TRUE(AAX_Atomic_CompareAndExchange_64(ints.a, ints.c, ints.d));
		EXPECT_EQ(ints.a, ints.d);
		EXPECT_EQ(0x0000000000000001ULL, ints.b);
		EXPECT_EQ(0x8000000000000000ULL, ints.c);
		EXPECT_EQ(0xFFFFFFFFFFFFFFFFULL, ints.d);
	}

	// AAX_Atomic_CompareAndExchange_Pointer
	// (not tested - just redirects to 32-bit or 64-bit cnx methods)

	// AAX_Atomic_Load_Pointer
	{
		void* dataPtr_1 = (void*)0x0;
		void* dataPtr_2 = (void*)0x1;

		EXPECT_EQ((void*)0x0, AAX_Atomic_Load_Pointer(&dataPtr_1));
		EXPECT_EQ((void*)0x0, dataPtr_1);
		EXPECT_EQ((void*)0x1, AAX_Atomic_Load_Pointer(&dataPtr_2));
		EXPECT_EQ((void*)0x1, dataPtr_2);

#if AAX_PointerSize == AAXPointer_64bit
		void* dataPtr_3 = (void*)0xFFFFFFFFFFFFFFFF;
		EXPECT_EQ((void*)0xFFFFFFFFFFFFFFFF, AAX_Atomic_Load_Pointer(&dataPtr_3));
		EXPECT_EQ((void*)0xFFFFFFFFFFFFFFFF, dataPtr_3);

		void* dataPtr_4 = (void*)0x0000000080000000;
		EXPECT_EQ((void*)0x0000000080000000, AAX_Atomic_Load_Pointer(&dataPtr_4));
		EXPECT_EQ((void*)0x0000000080000000, dataPtr_4);

		void* dataPtr_5 = (void*)0x0000000100000000;
		EXPECT_EQ((void*)0x0000000100000000, AAX_Atomic_Load_Pointer(&dataPtr_5));
		EXPECT_EQ((void*)0x0000000100000000, dataPtr_5);
#elif AAX_PointerSize == AAXPointer_32bit
		void* dataPtr_3 = (void*)(uintptr_t)0xFFFFFFFF;
		EXPECT_EQ((void*)(uintptr_t)0xFFFFFFFF, AAX_Atomic_Load_Pointer(&dataPtr_3));
		EXPECT_EQ((void*)(uintptr_t)0xFFFFFFFF, dataPtr_3);

		void* dataPtr_4 = (void*)(uintptr_t)0x00008000;
		EXPECT_EQ((void*)(uintptr_t)0x00008000, AAX_Atomic_Load_Pointer(&dataPtr_4));
		EXPECT_EQ((void*)(uintptr_t)0x00008000, dataPtr_4);

		void* dataPtr_5 = (void*)(uintptr_t)0x00010000;
		EXPECT_EQ((void*)(uintptr_t)0x00010000, AAX_Atomic_Load_Pointer(&dataPtr_5));
		EXPECT_EQ((void*)(uintptr_t)0x00010000, dataPtr_5);
#endif
	}
}

TEST_F(AAX_Migrated_AtomicOperations_Test, IncThenGet_32)
{
	static const int sNumThreads = 32;
	static const int sNumIterationsPerThread = 1 << 12;
	static uint32_t sTestInt = 0;

	IncFunc_32<sNumIterationsPerThread> func(sTestInt);


	//
	// Actual test operation
	//

	{
		StTestTimer timer("AtomicOperationsTest::IncThenGet_32");

		MigratedAAX_CTestThread* thread[sNumThreads];
		for (int i = 0; i < sNumThreads; ++i)
		{
			thread[i] = new MigratedAAX_CTestThread(&func);
		}

		for (int i = 0; i < sNumThreads; ++i)
		{
			thread[i]->Finish();
		}

		EXPECT_EQ(static_cast<uint32_t>(sNumThreads * sNumIterationsPerThread), func.GetNumIncrements());
		EXPECT_EQ(static_cast<uint32_t>(sNumThreads * sNumIterationsPerThread), sTestInt);

		for (int i = 0; i < sNumThreads; ++i)
		{
			delete thread[i];
		}
	}
}

TEST_F(AAX_Migrated_AtomicOperations_Test, IncThenGet_And_DecThenGet_32)
{
	static const int sNumThreads = 32;
	static const int sNumIterationsPerThread = 1 << 12;
	static uint32_t sTestInt = 0;

	IncFunc_32<sNumIterationsPerThread> incFunc(sTestInt);
	DecFunc_32<sNumIterationsPerThread> decFunc(sTestInt);


	//
	// Actual test operation
	//

	{
		StTestTimer timer("AtomicOperationsTest::IncThenGet_And_DecThenGet_32");

		MigratedAAX_CTestThread* incThread[sNumThreads];
		MigratedAAX_CTestThread* decThread[sNumThreads];
		for (int i = 0; i < sNumThreads; ++i)
		{
			incThread[i] = new MigratedAAX_CTestThread(&incFunc);
			decThread[i] = new MigratedAAX_CTestThread(&decFunc);
		}

		for (int i = 0; i < sNumThreads; ++i)
		{
			incThread[i]->Finish();
			decThread[i]->Finish();
		}

		EXPECT_EQ(static_cast<uint32_t>(sNumThreads * sNumIterationsPerThread), incFunc.GetNumIncrements());
		EXPECT_EQ(static_cast<uint32_t>(sNumThreads * sNumIterationsPerThread), decFunc.GetNumDecrements());
		EXPECT_EQ(0UL, sTestInt);

		for (int i = 0; i < sNumThreads; ++i)
		{
			delete incThread[i];
			delete decThread[i];
		}
	}
}

TEST_F(AAX_Migrated_AtomicOperations_Test, Exchange_32)
{
	typedef uint32_t TInt;

	static const int sNumThreads = 32;
	static const int sNumIterationsPerThread = 1 << 10;
	static TInt sTestInt = 0;

	static std::set<TInt> sTestValues;
	{
		sTestValues.insert(0x00000001);
		sTestValues.insert(0x000000FF);
		sTestValues.insert(0x0000FFFF);
		sTestValues.insert(0x80000000);
		sTestValues.insert(0xFF000000);
		sTestValues.insert(0xFFFF0000);
		sTestValues.insert(0x00000000);
		sTestValues.insert(0xFFFFFFFF);
		sTestValues.insert(0x00FF00FF);
		sTestValues.insert(0xFF00FF00);
		sTestValues.insert(0xAAAAAAAA);
		sTestValues.insert(0x55555555);
	}


	//
	// Actual test operation
	//

	{
		StTestTimer timer("AtomicOperationsTest::Exchange_32");

		ExcFunc_32* excFunc[sNumThreads];
		MigratedAAX_CTestThread* excThread[sNumThreads];
		for (int i = 0; i < sNumThreads; ++i)
		{
			excFunc[i] = new ExcFunc_32(sTestInt, sTestValues, sNumIterationsPerThread);
		}
		for (int i = 0; i < sNumThreads; ++i)
		{
			excThread[i] = new MigratedAAX_CTestThread(excFunc[i]);
		}

		for (int i = 0; i < sNumThreads; ++i)
		{
			excThread[i]->Finish();
		}

		for (int i = 0; i < sNumThreads; ++i)
		{
			delete excThread[i];
			delete excFunc[i];
		}
	}
}

TEST_F(AAX_Migrated_AtomicOperations_Test, Exchange_64)
{
	typedef uint64_t TInt;

	static const int sNumThreads = 32;
	static const int sNumIterationsPerThread = 1 << 10;
	static TInt sTestInt = 0;

	static std::set<TInt> sTestValues;
	{
		sTestValues.insert(0x0000000000000001);
		sTestValues.insert(0x00000000000000FF);
		sTestValues.insert(0x000000000000FFFF);
		sTestValues.insert(0x0000000080000000);
		sTestValues.insert(0x00000000FF000000);
		sTestValues.insert(0x00000000FFFF0000);
		sTestValues.insert(0x0000000000000000);
		sTestValues.insert(0x00000000FFFFFFFF);
		sTestValues.insert(0x0000000000FF00FF);
		sTestValues.insert(0x00000000FF00FF00);
		sTestValues.insert(0x00000000AAAAAAAA);
		sTestValues.insert(0x0000000055555555);
		sTestValues.insert(0x0000000100000000);
		sTestValues.insert(0x000000FF00000000);
		sTestValues.insert(0x0000FFFF00000000);
		sTestValues.insert(0x8000000000000000);
		sTestValues.insert(0xFF00000000000000);
		sTestValues.insert(0xFFFF000000000000);
		sTestValues.insert(0xFFFFFFFF00000000);
		sTestValues.insert(0x00FF00FF00000000);
		sTestValues.insert(0xFF00FF0000000000);
		sTestValues.insert(0xAAAAAAAA00000000);
		sTestValues.insert(0x5555555500000000);
		sTestValues.insert(0xFFFFFFFF00000001);
		sTestValues.insert(0xFFFFFFFF000000FF);
		sTestValues.insert(0xFFFFFFFF0000FFFF);
		sTestValues.insert(0xFFFFFFFF80000000);
		sTestValues.insert(0xFFFFFFFFFF000000);
		sTestValues.insert(0xFFFFFFFFFFFF0000);
		sTestValues.insert(0xFFFFFFFF00000000);
		sTestValues.insert(0xFFFFFFFFFFFFFFFF);
		sTestValues.insert(0xFFFFFFFF00FF00FF);
		sTestValues.insert(0xFFFFFFFFFF00FF00);
		sTestValues.insert(0xFFFFFFFFAAAAAAAA);
		sTestValues.insert(0xFFFFFFFF55555555);
		sTestValues.insert(0x00000001FFFFFFFF);
		sTestValues.insert(0x000000FFFFFFFFFF);
		sTestValues.insert(0x0000FFFFFFFFFFFF);
		sTestValues.insert(0x80000000FFFFFFFF);
		sTestValues.insert(0xFF000000FFFFFFFF);
		sTestValues.insert(0xFFFF0000FFFFFFFF);
		sTestValues.insert(0x00000000FFFFFFFF);
		sTestValues.insert(0x00FF00FFFFFFFFFF);
		sTestValues.insert(0xFF00FF00FFFFFFFF);
		sTestValues.insert(0xAAAAAAAAFFFFFFFF);
		sTestValues.insert(0x55555555FFFFFFFF);
		sTestValues.insert(0x00FF00FF00FF00FF);
		sTestValues.insert(0xFF00FF00FF00FF00);
		sTestValues.insert(0xAAAAAAAAAAAAAAAA);
		sTestValues.insert(0x5555555555555555);
	}


	//
	// Actual test operation
	//

	{
		StTestTimer timer("AtomicOperationsTest::Exchange_64");

		ExcFunc_64* excFunc[sNumThreads];
		MigratedAAX_CTestThread* excThread[sNumThreads];
		for (int i = 0; i < sNumThreads; ++i)
		{
			excFunc[i] = new ExcFunc_64(sTestInt, sTestValues, sNumIterationsPerThread);
		}
		for (int i = 0; i < sNumThreads; ++i)
		{
			excThread[i] = new MigratedAAX_CTestThread(excFunc[i]);
		}

		for (int i = 0; i < sNumThreads; ++i)
		{
			excThread[i]->Finish();
		}

		for (int i = 0; i < sNumThreads; ++i)
		{
			delete excThread[i];
			delete excFunc[i];
		}
	}
}

TEST_F(AAX_Migrated_AtomicOperations_Test, CompareAndExchange_32)
{
	typedef uint32_t TInt;

	static const int sNumThreads = 20;
	static const int sNumIterationsPerThread = 1 << 8;
	static TInt sTestInt = 0;

	static std::set<TInt> sTestValues;
	{
		sTestValues.insert(0x00000001);
		sTestValues.insert(0x80000000);
		sTestValues.insert(0xAAAAAAAA);
		sTestValues.insert(0x55555555);
		sTestValues.insert(0x00000002);
		sTestValues.insert(0x70000000);
		sTestValues.insert(0xBBBBBBBB);
		sTestValues.insert(0x44444444);
		sTestValues.insert(0x00000003);
		sTestValues.insert(0x60000000);
		sTestValues.insert(0xCCCCCCCC);
		sTestValues.insert(0x33333333);
		sTestValues.insert(0x00000004);
		sTestValues.insert(0x50000000);
		sTestValues.insert(0xDDDDDDDD);
		sTestValues.insert(0x22222222);
	}


	//
	// Actual test operation
	//

	{
		StTestTimer timer("AtomicOperationsTest::CompareAndExchange_32");

		CNXFunc_32* cnxFunc[sNumThreads];
		MigratedAAX_CTestThread* cnxThread[sNumThreads];
		for (int i = 0; i < sNumThreads; ++i)
		{
			cnxFunc[i] = new CNXFunc_32(sTestInt, sTestValues, sNumIterationsPerThread);
		}
		for (int i = 0; i < sNumThreads; ++i)
		{
			cnxThread[i] = new MigratedAAX_CTestThread(cnxFunc[i]);
		}

		for (int i = 0; i < sNumThreads; ++i)
		{
			cnxThread[i]->Finish();
		}

		// Check results
		for (int i = 0; i < sNumThreads; ++i)
		{
			cnxFunc[i]->CheckResults();
		}

		for (int i = 0; i < sNumThreads; ++i)
		{
			delete cnxThread[i];
			delete cnxFunc[i];
		}
	}

	EXPECT_TRUE(0 < sTestValues.count(sTestInt));
}

TEST_F(AAX_Migrated_AtomicOperations_Test, CompareAndExchange_64)
{
	typedef uint64_t TInt;

	static const int sNumThreads = 20;
	static const int sNumIterationsPerThread = 1 << 8;
	static TInt sTestInt = 0;

	static std::set<TInt> sTestValues;
	{
		sTestValues.insert(0x0000000000000001);
		sTestValues.insert(0x8000000000000000);
		sTestValues.insert(0xAAAAAAAAAAAAAAAA);
		sTestValues.insert(0x5555555555555555);
		sTestValues.insert(0x0000000000000002);
		sTestValues.insert(0x7000000000000000);
		sTestValues.insert(0xBBBBBBBBBBBBBBBB);
		sTestValues.insert(0x4444444444444444);
		sTestValues.insert(0x0000000000000003);
		sTestValues.insert(0x6000000000000000);
		sTestValues.insert(0xCCCCCCCCCCCCCCCC);
		sTestValues.insert(0x3333333333333333);
		sTestValues.insert(0x0000000000000004);
		sTestValues.insert(0x5000000000000000);
		sTestValues.insert(0xDDDDDDDDDDDDDDDD);
		sTestValues.insert(0x2222222222222222);
	}


	//
	// Actual test operation
	//

	{
		StTestTimer timer("AtomicOperationsTest::CompareAndExchange_64");

		CNXFunc_64* cnxFunc[sNumThreads];
		MigratedAAX_CTestThread* cnxThread[sNumThreads];
		for (int i = 0; i < sNumThreads; ++i)
		{
			cnxFunc[i] = new CNXFunc_64(sTestInt, sTestValues, sNumIterationsPerThread);
		}
		for (int i = 0; i < sNumThreads; ++i)
		{
			cnxThread[i] = new MigratedAAX_CTestThread(cnxFunc[i]);
		}

		for (int i = 0; i < sNumThreads; ++i)
		{
			cnxThread[i]->Finish();
		}

		// Check results
		for (int i = 0; i < sNumThreads; ++i)
		{
			cnxFunc[i]->CheckResults();
		}

		for (int i = 0; i < sNumThreads; ++i)
		{
			delete cnxThread[i];
			delete cnxFunc[i];
		}
	}

	EXPECT_TRUE(0 < sTestValues.count(sTestInt));
}

TEST_F(AAX_Migrated_AtomicOperations_Test, Exchange_And_Load_Ptr)
{
#if (AAX_PointerSize == AAXPointer_64bit)
	typedef ExcFunc_64 TExcFunc;
#elif (AAX_PointerSize == AAXPointer_32bit)
	typedef ExcFunc_32 TExcFunc;
#else
#error Unsupported pointer size
#endif

	static const int sNumExchangeThreads = 4;
	static const int sNumLoadThreads = 4;
	static const int sNumIterationsPerThread = 1 << 8;
	static TUIntPtr sTestPtr = 0x0;

	static std::set<TUIntPtr> sTestValues;
	{
#if (AAX_PointerSize == AAXPointer_64bit)
		sTestValues.insert(0x0000000000000001);
		sTestValues.insert(0x00000000000000FF);
		sTestValues.insert(0x000000000000FFFF);
		sTestValues.insert(0x0000000080000000);
		sTestValues.insert(0x00000000FF000000);
		sTestValues.insert(0x00000000FFFF0000);
		sTestValues.insert(0x0000000000000000);
		sTestValues.insert(0x00000000FFFFFFFF);
		sTestValues.insert(0x0000000000FF00FF);
		sTestValues.insert(0x00000000FF00FF00);
		sTestValues.insert(0x00000000AAAAAAAA);
		sTestValues.insert(0x0000000055555555);
		sTestValues.insert(0x0000000100000000);
		sTestValues.insert(0x000000FF00000000);
		sTestValues.insert(0x0000FFFF00000000);
		sTestValues.insert(0x8000000000000000);
		sTestValues.insert(0xFF00000000000000);
		sTestValues.insert(0xFFFF000000000000);
		sTestValues.insert(0xFFFFFFFF00000000);
		sTestValues.insert(0x00FF00FF00000000);
		sTestValues.insert(0xFF00FF0000000000);
		sTestValues.insert(0xAAAAAAAA00000000);
		sTestValues.insert(0x5555555500000000);
		sTestValues.insert(0xFFFFFFFF00000001);
		sTestValues.insert(0xFFFFFFFF000000FF);
		sTestValues.insert(0xFFFFFFFF0000FFFF);
		sTestValues.insert(0xFFFFFFFF80000000);
		sTestValues.insert(0xFFFFFFFFFF000000);
		sTestValues.insert(0xFFFFFFFFFFFF0000);
		sTestValues.insert(0xFFFFFFFF00000000);
		sTestValues.insert(0xFFFFFFFFFFFFFFFF);
		sTestValues.insert(0xFFFFFFFF00FF00FF);
		sTestValues.insert(0xFFFFFFFFFF00FF00);
		sTestValues.insert(0xFFFFFFFFAAAAAAAA);
		sTestValues.insert(0xFFFFFFFF55555555);
		sTestValues.insert(0x00000001FFFFFFFF);
		sTestValues.insert(0x000000FFFFFFFFFF);
		sTestValues.insert(0x0000FFFFFFFFFFFF);
		sTestValues.insert(0x80000000FFFFFFFF);
		sTestValues.insert(0xFF000000FFFFFFFF);
		sTestValues.insert(0xFFFF0000FFFFFFFF);
		sTestValues.insert(0x00000000FFFFFFFF);
		sTestValues.insert(0x00FF00FFFFFFFFFF);
		sTestValues.insert(0xFF00FF00FFFFFFFF);
		sTestValues.insert(0xAAAAAAAAFFFFFFFF);
		sTestValues.insert(0x55555555FFFFFFFF);
		sTestValues.insert(0x00FF00FF00FF00FF);
		sTestValues.insert(0xFF00FF00FF00FF00);
		sTestValues.insert(0xAAAAAAAAAAAAAAAA);
		sTestValues.insert(0x5555555555555555);
#elif (AAX_PointerSize == AAXPointer_32bit)
		sTestValues.insert(0x00000001);
		sTestValues.insert(0x000000FF);
		sTestValues.insert(0x0000FFFF);
		sTestValues.insert(0x80000000);
		sTestValues.insert(0xFF000000);
		sTestValues.insert(0xFFFF0000);
		sTestValues.insert(0x00000000);
		sTestValues.insert(0xFFFFFFFF);
		sTestValues.insert(0x00FF00FF);
		sTestValues.insert(0xFF00FF00);
		sTestValues.insert(0xAAAAAAAA);
		sTestValues.insert(0x55555555);
#else
#error Unsupported pointer size
#endif
	}

	//
	// Actual test operation
	//

	{
		StTestTimer timer("AtomicOperationsTest::Exchange_And_Load_Ptr");

		std::vector<TExcFunc*> excFunc(sNumExchangeThreads, nullptr);
		std::vector<MigratedAAX_CTestThread*> excThread(sNumExchangeThreads, nullptr);
		std::vector<LoadFunc*> loadFunc(sNumLoadThreads, nullptr);
		std::vector<MigratedAAX_CTestThread*> loadThread(sNumLoadThreads, nullptr);

		for (size_t i = 0; i < std::max<size_t>(sNumExchangeThreads, sNumLoadThreads); ++i)
		{
			if (i < sNumExchangeThreads)
			{
				excFunc[i] = new TExcFunc(sTestPtr, sTestValues, sNumIterationsPerThread);
			}

			if (i < sNumLoadThreads)
			{
				loadFunc[i] = new LoadFunc(sTestPtr, sTestValues, sNumIterationsPerThread);
			}
		}

		for (size_t i = 0; i < std::max<size_t>(sNumExchangeThreads, sNumLoadThreads); ++i)
		{
			if (i < sNumExchangeThreads)
			{
				excThread[i] = new MigratedAAX_CTestThread(excFunc[i]);
			}

			if (i < sNumLoadThreads)
			{
				loadThread[i] = new MigratedAAX_CTestThread(loadFunc[i]);
			}
		}

		for (size_t i = 0; i < sNumExchangeThreads; ++i)
		{
			excThread[i]->Finish();
		}
		for (size_t i = 0; i < sNumLoadThreads; ++i)
		{
			loadThread[i]->Finish();
		}

		for (size_t i = 0; i < sNumLoadThreads; ++i)
		{
			loadFunc[i]->CheckResults();
		}

		for (size_t i = 0; i < sNumExchangeThreads; ++i)
		{
			delete excThread[i];
			delete excFunc[i];
		}
		for (size_t i = 0; i < sNumLoadThreads; ++i)
		{
			delete loadThread[i];
			delete loadFunc[i];
		}
	}

	EXPECT_TRUE(0 < sTestValues.count(sTestPtr));
}
