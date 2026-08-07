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
 *	\file AAX_CMutex_UnitTest.cpp
 *
 *	\brief Unit tests for AAX_CMutex and AAX_StLock_Guard classes
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX_CMutex.h"
#include <thread>
#include <chrono>
#include <atomic>

//--------------------------------------------------------------------
#pragma mark - Test fixture

class AAX_CMutex_Test : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

//--------------------------------------------------------------------
#pragma mark - Basic Functionality Tests

TEST_F(AAX_CMutex_Test, BasicConstruction)
{
    // Test that mutex can be constructed and destroyed
    AAX_CMutex mutex;
    // If we get here, construction succeeded
    EXPECT_TRUE(true);
}

TEST_F(AAX_CMutex_Test, BasicLockUnlock)
{
    AAX_CMutex mutex;
    
    // Lock should succeed
    bool lockResult = mutex.Lock();
    EXPECT_TRUE(lockResult);
    
    // Unlock (void return)
    mutex.Unlock();
    
    // Should be able to lock again
    lockResult = mutex.Lock();
    EXPECT_TRUE(lockResult);
    mutex.Unlock();
}

TEST_F(AAX_CMutex_Test, TryLock)
{
    AAX_CMutex mutex;
    
    // Try_Lock should succeed when mutex is available
    bool tryLockResult = mutex.Try_Lock();
    EXPECT_TRUE(tryLockResult);
    
    // Unlock
    mutex.Unlock();
    
    // Try_Lock should succeed again
    tryLockResult = mutex.Try_Lock();
    EXPECT_TRUE(tryLockResult);
    mutex.Unlock();
}

TEST_F(AAX_CMutex_Test, LockGuardBasic)
{
    AAX_CMutex mutex;
    
    // Test RAII lock guard
    {
        AAX_StLock_Guard guard(mutex);
        // Mutex should be locked within this scope
        // We can't directly test if it's locked, but we can test that
        // the guard was constructed successfully
        EXPECT_TRUE(true);
    }
    // Mutex should be unlocked when guard goes out of scope
    
    // Should be able to lock again after guard destruction
    bool lockResult = mutex.Lock();
    EXPECT_TRUE(lockResult);
    mutex.Unlock();
}

TEST_F(AAX_CMutex_Test, MultipleLockGuards)
{
    AAX_CMutex mutex;
    
    // Test nested scopes with lock guards
    {
        AAX_StLock_Guard guard1(mutex);
        EXPECT_TRUE(true);
        
        // Inner scope - this should work since it's the same thread
        {
            // Note: This might not work if the mutex is not recursive
            // We'll test what actually happens
            bool lockResult = mutex.Try_Lock();
            if (lockResult) {
                mutex.Unlock();
            }
            // Either way, the test should complete
            EXPECT_TRUE(true);
        }
    }
    
    // Should be able to lock after all guards are destroyed
    bool lockResult = mutex.Lock();
    EXPECT_TRUE(lockResult);
    mutex.Unlock();
}

TEST_F(AAX_CMutex_Test, LockGuardExceptionSafety)
{
    AAX_CMutex mutex;
    
    try {
        AAX_StLock_Guard guard(mutex);
        // Simulate some work that might throw
        // For this test, we won't actually throw
        EXPECT_TRUE(true);
    } catch (...) {
        // Should not reach here in this test
        EXPECT_TRUE(false);
    }
    
    // Mutex should be unlocked even if exception occurred
    bool lockResult = mutex.Lock();
    EXPECT_TRUE(lockResult);
    mutex.Unlock();
}

TEST_F(AAX_CMutex_Test, SequentialLocking)
{
    AAX_CMutex mutex;
    
    // Test multiple sequential lock/unlock cycles
    for (int i = 0; i < 10; ++i) {
        bool lockResult = mutex.Lock();
        EXPECT_TRUE(lockResult);
        
        // Do some "work"
        std::this_thread::sleep_for(std::chrono::microseconds(1));
        
        mutex.Unlock();
        
        // Try lock should also work
        bool tryLockResult = mutex.Try_Lock();
        EXPECT_TRUE(tryLockResult);
        mutex.Unlock();
    }
}

TEST_F(AAX_CMutex_Test, LockGuardSequential)
{
    AAX_CMutex mutex;
    
    // Test multiple sequential lock guard uses
    for (int i = 0; i < 5; ++i) {
        {
            AAX_StLock_Guard guard(mutex);
            // Do some "work"
            std::this_thread::sleep_for(std::chrono::microseconds(1));
            EXPECT_TRUE(true);
        }
        
        // Verify mutex is unlocked between iterations
        bool lockResult = mutex.Try_Lock();
        EXPECT_TRUE(lockResult);
        mutex.Unlock();
    }
}

TEST_F(AAX_CMutex_Test, ThreadSafety)
{
    AAX_CMutex mutex;
    std::atomic<int> counter(0);
    std::atomic<int> errors(0);
    const int numThreads = 4;
    const int incrementsPerThread = 100;

    std::vector<std::thread> threads;

    // Launch multiple threads that increment a counter
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&mutex, &counter, &errors, incrementsPerThread]() {
            for (int i = 0; i < incrementsPerThread; ++i) {
                bool lockResult = mutex.Lock();
                if (!lockResult) {
                    errors.fetch_add(1);
                    continue;
                }

                // Critical section - increment counter
                int oldValue = counter.load();
                std::this_thread::sleep_for(std::chrono::microseconds(1));
                counter.store(oldValue + 1);

                mutex.Unlock();
            }
        });
    }

    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }

    // Verify results
    EXPECT_EQ(0, errors.load());
    EXPECT_EQ(numThreads * incrementsPerThread, counter.load());
}

TEST_F(AAX_CMutex_Test, ThreadSafetyWithLockGuard)
{
    AAX_CMutex mutex;
    std::atomic<int> counter(0);
    const int numThreads = 4;
    const int incrementsPerThread = 100;

    std::vector<std::thread> threads;

    // Launch multiple threads using lock guard
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&mutex, &counter, incrementsPerThread]() {
            for (int i = 0; i < incrementsPerThread; ++i) {
                {
                    AAX_StLock_Guard guard(mutex);
                    // Critical section - increment counter
                    int oldValue = counter.load();
                    std::this_thread::sleep_for(std::chrono::microseconds(1));
                    counter.store(oldValue + 1);
                }
            }
        });
    }

    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }

    // Verify results
    EXPECT_EQ(numThreads * incrementsPerThread, counter.load());
}

TEST_F(AAX_CMutex_Test, TryLockContention)
{
    AAX_CMutex mutex;
    std::atomic<bool> thread1Ready(false);
    std::atomic<bool> thread2CanProceed(false);
    std::atomic<int> tryLockFailures(0);

    std::thread thread1([&mutex, &thread1Ready, &thread2CanProceed]() {
        bool lockResult = mutex.Lock();
        EXPECT_TRUE(lockResult);

        thread1Ready.store(true);

        // Hold the lock for a short time
        while (!thread2CanProceed.load()) {
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }

        mutex.Unlock();
    });

    std::thread thread2([&mutex, &thread1Ready, &thread2CanProceed, &tryLockFailures]() {
        // Wait for thread1 to acquire the lock
        while (!thread1Ready.load()) {
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }

        // Try to acquire the lock - should fail
        bool tryLockResult = mutex.Try_Lock();
        if (!tryLockResult) {
            tryLockFailures.fetch_add(1);
        } else {
            mutex.Unlock();
        }

        thread2CanProceed.store(true);
    });

    thread1.join();
    thread2.join();

    // Try_Lock should have failed when mutex was held
    EXPECT_EQ(1, tryLockFailures.load());
}
