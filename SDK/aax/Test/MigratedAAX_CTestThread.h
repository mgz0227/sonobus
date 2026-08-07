/*================================================================================================*/
/*
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
 */

/** \file Thread class used for AAX Library unit tests (Migrated version)
 */


#ifndef AAXLibrary_MigratedAAX_CTestThread_h
#define AAXLibrary_MigratedAAX_CTestThread_h

// Unit Test Includes
#include "gtest/gtest.h"


//------------------------------------------------------------
#pragma mark - Helper functions declaration

namespace AAXThread
{
	static void SleepMS(double inMilliseconds);
	static void SurrenderCPU();
}


//------------------------------------------------------------
#pragma mark - MigratedAAX_CTestThread declaration

class MigratedAAX_CTestThread
{
public:
	class Functor
	{
	public:
		virtual ~Functor() {}
		virtual void Run(void) = 0;
	};
	
public:
	explicit MigratedAAX_CTestThread(Functor* inFunc);
	~MigratedAAX_CTestThread();
	
public:
	void Finish();
	
private:
	class CImpl;
	CImpl* pImpl;
};



//------------------------------------------------------------
#pragma mark - MigratedAAX_CTestThread definition

// GCC/LLVM
#if defined(__GNUC__)

// Platform Includes
#include <unistd.h>
#include <pthread.h>
#include <sched.h>

/* static */
void AAXThread::SleepMS(double inMilliseconds)
{
	usleep((useconds_t)(inMilliseconds * 1000.));
}

/* static */
void AAXThread::SurrenderCPU()
{
	sched_yield();
}

class MigratedAAX_CTestThread::CImpl
{
public:
	explicit CImpl(Functor* inFunc)
	: mThreadID(0)
	, mFunc(inFunc)
	{
		pthread_attr_init(&mThreadAttr);
		pthread_attr_setdetachstate(&mThreadAttr, PTHREAD_CREATE_JOINABLE);
		
		const int rc = pthread_create(&mThreadID, &mThreadAttr, DoAction, mFunc);
		EXPECT_EQ(0, rc);
	}
	
	~CImpl()
	{
		pthread_cancel(mThreadID);
		pthread_attr_destroy(&mThreadAttr);
	}
	
public:
	void Finish()
	{
		void* status = (void*)0;
		const int res = pthread_join(mThreadID, &status);
		EXPECT_EQ(0, res);
		EXPECT_EQ((void*)0, status);
	}
	
private:
	static void* DoAction(void* inData)
	{
		Functor* const func = reinterpret_cast<Functor*>(inData);
		func->Run();
		pthread_exit(0);
	}
	
private:
	pthread_t mThreadID;
	pthread_attr_t mThreadAttr;
	Functor* mFunc;
};


// Visual C
#elif defined(_MSC_VER)

// Platform Includes
#include <windows.h>
#include <process.h>

// AAX Includes
#include "AAX_Atomic.h"


/* static */
void AAXThread::SleepMS(double inMilliseconds)
{
	::Sleep((1 > inMilliseconds) ? 1 : (DWORD)inMilliseconds);
}

/* static */
void AAXThread::SurrenderCPU(void)
{
	SwitchToThread();
}


class MigratedAAX_CTestThread::CImpl
{
public:
	explicit CImpl(Functor* inFunc)
	: mThreadHandle(0)
	, mFunc(inFunc)
	{
		mThreadHandle = ::CreateThread(NULL, 0, &DoAction, mFunc, 0, NULL);
		EXPECT_NE((HANDLE)NULL, mThreadHandle);
	}
	
	~CImpl()
	{
		// Try exiting for several seconds
		const bool finished = FinishWithTimeout(1000 * 10);
		EXPECT_TRUE(finished);
		
		if (false == finished)
		{
			::TerminateThread(mThreadHandle, -1);
		}

		::CloseHandle(mThreadHandle);
	}
	
public:
	void Finish()
	{
		// No timeout
		const bool finishSuccess = FinishWithTimeout(0);
		EXPECT_TRUE(finishSuccess);
	}
	
private:
	static DWORD WINAPI DoAction(LPVOID inData)
	{
		Functor* const func = reinterpret_cast<Functor*>(inData);
		func->Run();
		//AAX_Atomic_Exchange_32(mDone, 1);
		return 0;
	}
	
	bool FinishWithTimeout(DWORD inTimeoutMilliseconds)
	{
		const DWORD waitResult = ::WaitForSingleObject(mThreadHandle, (0 == inTimeoutMilliseconds) ? INFINITE : inTimeoutMilliseconds);
		return (WAIT_OBJECT_0 == waitResult);

		//const long int sleepMilliseconds = 10L;
		//const long int numIterations = inTimeoutMilliseconds / sleepMilliseconds;
		//
		//if (0 < inTimeoutMilliseconds)
		//{
		//	for (int i = 0; (true == AAX_Atomic_CompareAndExchange_32(mDone, 0, 0)) && (i < numIterations); ++i)
		//	{
		//		::Sleep(sleepMilliseconds);
		//	}
		//}
		//else
		//{
		//	// No timeout
		//	while (true == AAX_Atomic_CompareAndExchange_32(mDone, 0, 0))
		//	{
		//		::Sleep(sleepMilliseconds);
		//	}
		//}
		//
		//return AAX_Atomic_CompareAndExchange_32(mDone, 1, 1);
	}

private:
	HANDLE mThreadHandle;
	Functor* mFunc;
};

#else // Not Visual C or GCC/LLVM
#error Provide an atomic operation implementation for this compiler
#endif // Compiler version check


// Compiler-independent function definitions

MigratedAAX_CTestThread::MigratedAAX_CTestThread(Functor* inFunc)
: pImpl(new CImpl(inFunc))
{
}

MigratedAAX_CTestThread::~MigratedAAX_CTestThread()
{
	delete pImpl;
}

void MigratedAAX_CTestThread::Finish()
{
	pImpl->Finish();
}



#endif // AAXLibrary_MigratedAAX_CTestThread_h
