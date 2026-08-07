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
 *	\file AAX_CAtomicQueue_UnitTest.cpp
 *
 *	\brief Unit tests for AAX_CAtomicQueue template class
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX_CAtomicQueue.h"
#include "AAX_IContainer.h"

//--------------------------------------------------------------------
#pragma mark - Test fixture and helper classes

// Test object for queue operations
class TestObject
{
public:
    TestObject(int value = 0) : mValue(value) {}
    int GetValue() const { return mValue; }
    void SetValue(int value) { mValue = value; }
    
private:
    int mValue;
};

class AAX_CAtomicQueue_Test : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
    
    // Helper to create test objects
    std::unique_ptr<TestObject> CreateTestObject(int value) {
        return std::make_unique<TestObject>(value);
    }
};

//--------------------------------------------------------------------
#pragma mark - Basic Functionality Tests

TEST_F(AAX_CAtomicQueue_Test, TemplateInstantiation)
{
    // Test that we can instantiate queues with different sizes
    AAX_CAtomicQueue<TestObject, 4> smallQueue;
    AAX_CAtomicQueue<TestObject, 16> mediumQueue;
    AAX_CAtomicQueue<TestObject, 256> largeQueue;

    // Test that the queues can be instantiated and work with basic operations
    EXPECT_EQ(nullptr, smallQueue.Pop());   // Empty queue should return null
    EXPECT_EQ(nullptr, mediumQueue.Pop());  // Empty queue should return null
    EXPECT_EQ(nullptr, largeQueue.Pop());   // Empty queue should return null
    
    // Test that they can accept pushes
    auto testObj = CreateTestObject(42);
    EXPECT_EQ(AAX_IContainer::eStatus_Success, smallQueue.Push(testObj.get()));
    EXPECT_EQ(AAX_IContainer::eStatus_Success, mediumQueue.Push(testObj.get()));
    EXPECT_EQ(AAX_IContainer::eStatus_Success, largeQueue.Push(testObj.get()));
}

TEST_F(AAX_CAtomicQueue_Test, EmptyQueueOperations)
{
    AAX_CAtomicQueue<TestObject, 8> queue;
    
    // Empty queue should return null on pop and peek
    EXPECT_EQ(nullptr, queue.Pop());
    EXPECT_EQ(nullptr, queue.Peek());
}

TEST_F(AAX_CAtomicQueue_Test, BasicPushPop)
{
    AAX_CAtomicQueue<TestObject, 8> queue;
    auto testObj = CreateTestObject(42);
    TestObject* objPtr = testObj.get();
    
    // Push object
    AAX_IContainer::EStatus status = queue.Push(objPtr);
    EXPECT_EQ(AAX_IContainer::eStatus_Success, status);
    
    // Peek should return the object without removing it
    TestObject* peeked = queue.Peek();
    EXPECT_EQ(objPtr, peeked);
    EXPECT_EQ(42, peeked->GetValue());
    
    // Pop should return and remove the object
    TestObject* popped = queue.Pop();
    EXPECT_EQ(objPtr, popped);
    EXPECT_EQ(42, popped->GetValue());
    
    // Queue should now be empty
    EXPECT_EQ(nullptr, queue.Pop());
    EXPECT_EQ(nullptr, queue.Peek());
}

TEST_F(AAX_CAtomicQueue_Test, NullPointerHandling)
{
    AAX_CAtomicQueue<TestObject, 8> queue;

    // Pushing null should return unsupported
    AAX_IContainer::EStatus status = queue.Push(nullptr);
    EXPECT_EQ(AAX_IContainer::eStatus_Unsupported, status);

    // Queue should remain empty
    EXPECT_EQ(nullptr, queue.Pop());
}

TEST_F(AAX_CAtomicQueue_Test, MultipleObjects)
{
    AAX_CAtomicQueue<TestObject, 8> queue;
    std::vector<std::unique_ptr<TestObject>> objects;

    // Create and push multiple objects
    for (int i = 0; i < 5; ++i) {
        objects.push_back(CreateTestObject(i * 10));
        AAX_IContainer::EStatus status = queue.Push(objects.back().get());
        EXPECT_EQ(AAX_IContainer::eStatus_Success, status);
    }

    // Pop objects in FIFO order
    for (int i = 0; i < 5; ++i) {
        TestObject* popped = queue.Pop();
        EXPECT_NE(nullptr, popped);
        EXPECT_EQ(i * 10, popped->GetValue());
        EXPECT_EQ(objects[i].get(), popped);
    }

    // Queue should be empty
    EXPECT_EQ(nullptr, queue.Pop());
}

TEST_F(AAX_CAtomicQueue_Test, QueueCapacity)
{
    const size_t capacity = 4;
    AAX_CAtomicQueue<TestObject, capacity> queue;
    std::vector<std::unique_ptr<TestObject>> objects;

    // Fill queue to capacity
    for (size_t i = 0; i < capacity; ++i) {
        objects.push_back(CreateTestObject(static_cast<int>(i)));
        AAX_IContainer::EStatus status = queue.Push(objects.back().get());
        EXPECT_EQ(AAX_IContainer::eStatus_Success, status);
    }

    // Next push should fail (queue full)
    auto extraObj = CreateTestObject(999);
    AAX_IContainer::EStatus status = queue.Push(extraObj.get());
    EXPECT_EQ(AAX_IContainer::eStatus_Overflow, status);

    // Pop one item to make space
    TestObject* popped = queue.Pop();
    EXPECT_NE(nullptr, popped);
    EXPECT_EQ(0, popped->GetValue());

    // Now we should be able to push again
    status = queue.Push(extraObj.get());
    EXPECT_EQ(AAX_IContainer::eStatus_Success, status);
}

TEST_F(AAX_CAtomicQueue_Test, ClearOperation)
{
    AAX_CAtomicQueue<TestObject, 8> queue;
    std::vector<std::unique_ptr<TestObject>> objects;

    // Add some objects
    for (int i = 0; i < 3; ++i) {
        objects.push_back(CreateTestObject(i));
        queue.Push(objects.back().get());
    }

    // Verify objects are there
    EXPECT_NE(nullptr, queue.Peek());

    // Clear the queue
    queue.Clear();

    // Queue should be empty after clear
    EXPECT_EQ(nullptr, queue.Pop());
    EXPECT_EQ(nullptr, queue.Peek());
}

TEST_F(AAX_CAtomicQueue_Test, PeekConsistency)
{
    AAX_CAtomicQueue<TestObject, 8> queue;
    auto obj1 = CreateTestObject(100);
    auto obj2 = CreateTestObject(200);

    // Empty queue peek
    EXPECT_EQ(nullptr, queue.Peek());

    // Push first object
    queue.Push(obj1.get());
    EXPECT_EQ(obj1.get(), queue.Peek());
    EXPECT_EQ(obj1.get(), queue.Peek()); // Multiple peeks should return same object

    // Push second object - peek should still return first
    queue.Push(obj2.get());
    EXPECT_EQ(obj1.get(), queue.Peek());

    // Pop first object
    TestObject* popped = queue.Pop();
    EXPECT_EQ(obj1.get(), popped);

    // Now peek should return second object
    EXPECT_EQ(obj2.get(), queue.Peek());
}

TEST_F(AAX_CAtomicQueue_Test, SingleElementQueue)
{
    AAX_CAtomicQueue<TestObject, 1> queue;
    auto obj1 = CreateTestObject(42);
    auto obj2 = CreateTestObject(84);

    // Push to single-element queue
    AAX_IContainer::EStatus status = queue.Push(obj1.get());
    EXPECT_EQ(AAX_IContainer::eStatus_Success, status);

    // Second push should overflow
    status = queue.Push(obj2.get());
    EXPECT_EQ(AAX_IContainer::eStatus_Overflow, status);

    // Pop the element
    TestObject* popped = queue.Pop();
    EXPECT_EQ(obj1.get(), popped);

    // Now we can push the second object
    status = queue.Push(obj2.get());
    EXPECT_EQ(AAX_IContainer::eStatus_Success, status);
}

TEST_F(AAX_CAtomicQueue_Test, DifferentDataTypes)
{
    // Test with int pointers
    AAX_CAtomicQueue<int, 4> intQueue;
    int values[] = {10, 20, 30};

    // Push int pointers
    for (int i = 0; i < 3; ++i) {
        AAX_IContainer::EStatus status = intQueue.Push(&values[i]);
        EXPECT_EQ(AAX_IContainer::eStatus_Success, status);
    }

    // Pop and verify
    for (int i = 0; i < 3; ++i) {
        int* popped = intQueue.Pop();
        EXPECT_NE(nullptr, popped);
        EXPECT_EQ(&values[i], popped);
        EXPECT_EQ(values[i], *popped);
    }
}

TEST_F(AAX_CAtomicQueue_Test, LargeQueue)
{
    const size_t largeSize = 1024;
    AAX_CAtomicQueue<TestObject, largeSize> queue;
    std::vector<std::unique_ptr<TestObject>> objects;

    // Fill a significant portion of the queue
    const size_t numObjects = 100;
    for (size_t i = 0; i < numObjects; ++i) {
        objects.push_back(CreateTestObject(static_cast<int>(i)));
        AAX_IContainer::EStatus status = queue.Push(objects.back().get());
        EXPECT_EQ(AAX_IContainer::eStatus_Success, status);
    }

    // Verify all objects can be retrieved in order
    for (size_t i = 0; i < numObjects; ++i) {
        TestObject* popped = queue.Pop();
        EXPECT_NE(nullptr, popped);
        EXPECT_EQ(static_cast<int>(i), popped->GetValue());
    }

    // Queue should be empty
    EXPECT_EQ(nullptr, queue.Pop());
}

TEST_F(AAX_CAtomicQueue_Test, AlternatingPushPop)
{
    AAX_CAtomicQueue<TestObject, 8> queue;
    std::vector<std::unique_ptr<TestObject>> objects;

    // Alternate between push and pop operations
    for (int i = 0; i < 10; ++i) {
        // Push an object
        objects.push_back(CreateTestObject(i));
        AAX_IContainer::EStatus status = queue.Push(objects.back().get());
        EXPECT_EQ(AAX_IContainer::eStatus_Success, status);

        // Immediately pop it
        TestObject* popped = queue.Pop();
        EXPECT_EQ(objects.back().get(), popped);
        EXPECT_EQ(i, popped->GetValue());
    }

    // Queue should be empty
    EXPECT_EQ(nullptr, queue.Pop());
}
