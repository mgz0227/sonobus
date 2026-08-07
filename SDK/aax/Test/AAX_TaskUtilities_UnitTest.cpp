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
 *
 */

/**  
 *	\file AAX_TaskUtilities_UnitTest.cpp
 *
 *	\brief Unit tests for AAX_TaskUtilities.h template functions
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX_TaskUtilities.h"
#include "AAX_ITask.h"
#include "AAX_IACFDataBuffer.h"
#include "AAX.h"
#include <vector>
#include <string>
#include <cstring>

//--------------------------------------------------------------------
#pragma mark - Test fixture

class AAX_TaskUtilities : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
    
    // Mock implementation of AAX_IACFDataBuffer for testing
    class MockDataBuffer : public AAX_IACFDataBuffer
    {
    public:
        MockDataBuffer(AAX_CTypeID type, const void* data, int32_t size)
            : mType(type), mData(data), mSize(size) {}
        
        // IACFUnknown methods (minimal implementation for testing)
        ACFRESULT ACFMETHODCALLTYPE QueryInterface(const acfIID& riid, void** ppvObjOut) override
        {
            if (ppvObjOut) *ppvObjOut = this;
            return ACF_OK;
        }

        acfUInt32 ACFMETHODCALLTYPE AddRef() override { return ++mRefCount; }
        acfUInt32 ACFMETHODCALLTYPE Release() override
        {
            if (--mRefCount == 0) {
                delete this;
                return 0;
            }
            return mRefCount;
        }
        
        // AAX_IACFDataBuffer methods
        AAX_Result Type(AAX_CTypeID* oType) const override
        {
            if (oType) {
                *oType = mType;
                return AAX_SUCCESS;
            }
            return AAX_ERROR_NULL_ARGUMENT;
        }
        
        AAX_Result Size(int32_t* oSize) const override
        {
            if (oSize) {
                *oSize = mSize;
                return AAX_SUCCESS;
            }
            return AAX_ERROR_NULL_ARGUMENT;
        }
        
        AAX_Result Data(void const** oBuffer) const override
        {
            if (oBuffer) {
                *oBuffer = mData;
                return AAX_SUCCESS;
            }
            return AAX_ERROR_NULL_ARGUMENT;
        }
        
    private:
        AAX_CTypeID mType;
        const void* mData;
        int32_t mSize;
        mutable acfUInt32 mRefCount = 1;
    };
    
    // Mock implementation of AAX_ITask for testing
    class MockTask : public AAX_ITask
    {
    public:
        MockTask(AAX_CTypeID taskType) : mTaskType(taskType), mProgress(0.0f), mTaskID(12345) {}
        
        void AddMockArgument(AAX_CTypeID argType, const void* data, int32_t size)
        {
            mArguments[argType] = std::make_unique<MockDataBuffer>(argType, data, size);
        }
        
        // AAX_ITask methods
        AAX_Result GetType(AAX_CTypeID* oType) const override
        {
            if (oType) {
                *oType = mTaskType;
                return AAX_SUCCESS;
            }
            return AAX_ERROR_NULL_ARGUMENT;
        }
        
        AAX_IACFDataBuffer const* GetArgumentOfType(AAX_CTypeID iType) const override
        {
            auto it = mArguments.find(iType);
            if (it != mArguments.end()) {
                it->second->AddRef(); // Caller takes ownership
                return it->second.get();
            }
            return nullptr;
        }
        
        AAX_Result SetProgress(float iProgress) override
        {
            mProgress = iProgress;
            return AAX_SUCCESS;
        }
        
        float GetProgress() const override
        {
            return mProgress;
        }
        
        AAX_Result AddResult(AAX_IACFDataBuffer const* iResult) override
        {
            // Store result for testing
            return AAX_SUCCESS;
        }
        
        AAX_ITask* SetDone(AAX_TaskCompletionStatus iStatus) override
        {
            mStatus = iStatus;
            return nullptr; // Success case
        }
        
        AAX_Result GetID(AAX_CTaskID* outID) const override
        {
            if (outID) {
                *outID = mTaskID;
                return AAX_SUCCESS;
            }
            return AAX_ERROR_NULL_ARGUMENT;
        }
        
        AAX_Result SetProgressLabel(const char* iLabel) override
        {
            if (iLabel) {
                mProgressLabel = iLabel;
            }
            return AAX_SUCCESS;
        }
        
    private:
        AAX_CTypeID mTaskType;
        float mProgress;
        AAX_CTaskID mTaskID;
        std::string mProgressLabel;
        AAX_TaskCompletionStatus mStatus = AAX_TaskCompletionStatus::None;
        std::map<AAX_CTypeID, std::unique_ptr<MockDataBuffer>> mArguments;
    };
    
    // Test type IDs
    static constexpr AAX_CTypeID kTestTaskType = 'TEST';
    static constexpr AAX_CTypeID kIntArgType = 'INT ';
    static constexpr AAX_CTypeID kFloatArgType = 'FLT ';
    static constexpr AAX_CTypeID kStringArgType = 'STR ';
    static constexpr AAX_CTypeID kArrayArgType = 'ARR ';
    static constexpr AAX_CTypeID kMismatchType = 'MISM';
};

//--------------------------------------------------------------------
#pragma mark - GetArgument Basic Type Tests

TEST_F(AAX_TaskUtilities, GetArgumentInt32)
{
    using namespace AAX::Task;
    
    // Test successful int32_t extraction
    MockTask task(kTestTaskType);
    int32_t testValue = 42;
    task.AddMockArgument(kIntArgType, &testValue, sizeof(testValue));
    
    int32_t result = 0;
    AAX_Result err = GetArgument(task, kIntArgType, result);
    
    EXPECT_EQ(AAX_SUCCESS, err);
    EXPECT_EQ(testValue, result);
}

TEST_F(AAX_TaskUtilities, GetArgumentFloat)
{
    using namespace AAX::Task;
    
    // Test successful float extraction
    MockTask task(kTestTaskType);
    float testValue = 3.14159f;
    task.AddMockArgument(kFloatArgType, &testValue, sizeof(testValue));
    
    float result = 0.0f;
    AAX_Result err = GetArgument(task, kFloatArgType, result);
    
    EXPECT_EQ(AAX_SUCCESS, err);
    EXPECT_FLOAT_EQ(testValue, result);
}

TEST_F(AAX_TaskUtilities, GetArgumentDouble)
{
    using namespace AAX::Task;
    
    // Test successful double extraction
    MockTask task(kTestTaskType);
    double testValue = 2.718281828;
    task.AddMockArgument(kFloatArgType, &testValue, sizeof(testValue));
    
    double result = 0.0;
    AAX_Result err = GetArgument(task, kFloatArgType, result);
    
    EXPECT_EQ(AAX_SUCCESS, err);
    EXPECT_DOUBLE_EQ(testValue, result);
}

//--------------------------------------------------------------------
#pragma mark - GetArgument Type Mismatch Tests

TEST_F(AAX_TaskUtilities, GetArgumentTypeMismatch)
{
    using namespace AAX::Task;

    // Test type mismatch - argument type doesn't match data buffer type
    MockTask task(kTestTaskType);
    int32_t testValue = 42;
    task.AddMockArgument(kMismatchType, &testValue, sizeof(testValue)); // Different type than requested

    int32_t result = 999; // Initialize with non-zero value
    AAX_Result err = GetArgument(task, kIntArgType, result);
    EXPECT_EQ(AAX_ERROR_INVALID_ARGUMENT, err);

    EXPECT_EQ(999, result); // Result should be unchanged when error occurs
}

TEST_F(AAX_TaskUtilities, GetArgumentNonExistentArgument)
{
    using namespace AAX::Task;

    // Test requesting an argument that doesn't exist
    MockTask task(kTestTaskType);
    // Don't add any arguments

    int32_t result = 999;
    AAX_Result err = GetArgument(task, kIntArgType, result);
    EXPECT_EQ(AAX_ERROR_INVALID_ARGUMENT, err);

    EXPECT_EQ(999, result); // Result should be unchanged when error occurs
}

//--------------------------------------------------------------------
#pragma mark - GetArgument Vector Tests

TEST_F(AAX_TaskUtilities, GetArgumentVector)
{
    using namespace AAX::Task;

    // Test successful vector extraction
    MockTask task(kTestTaskType);
    std::vector<int32_t> testData = {1, 2, 3, 4, 5};
    task.AddMockArgument(kArrayArgType, testData.data(), static_cast<int32_t>(testData.size() * sizeof(int32_t)));

    std::vector<int32_t> result;
    AAX_Result err = GetArgument(task, kArrayArgType, result);

    EXPECT_EQ(AAX_SUCCESS, err);
    EXPECT_EQ(testData.size(), result.size());
    EXPECT_EQ(testData, result);
}

TEST_F(AAX_TaskUtilities, GetArgumentVectorFloat)
{
    using namespace AAX::Task;

    // Test vector of floats
    MockTask task(kTestTaskType);
    std::vector<float> testData = {1.1f, 2.2f, 3.3f, 4.4f};
    task.AddMockArgument(kArrayArgType, testData.data(), static_cast<int32_t>(testData.size() * sizeof(float)));

    std::vector<float> result;
    AAX_Result err = GetArgument(task, kArrayArgType, result);

    EXPECT_EQ(AAX_SUCCESS, err);
    EXPECT_EQ(testData.size(), result.size());
    for (size_t i = 0; i < testData.size(); ++i) {
        EXPECT_FLOAT_EQ(testData[i], result[i]);
    }
}

TEST_F(AAX_TaskUtilities, GetArgumentVectorEmpty)
{
    using namespace AAX::Task;

    // Test empty vector
    MockTask task(kTestTaskType);
    task.AddMockArgument(kArrayArgType, nullptr, 0);

    std::vector<int32_t> result;
    AAX_Result err = GetArgument(task, kArrayArgType, result);

    EXPECT_EQ(AAX_SUCCESS, err);
    EXPECT_TRUE(result.empty());
}

TEST_F(AAX_TaskUtilities, GetArgumentVectorTypeMismatch)
{
    using namespace AAX::Task;

    // Test vector with type mismatch
    MockTask task(kTestTaskType);
    std::vector<int32_t> testData = {1, 2, 3};
    task.AddMockArgument(kMismatchType, testData.data(), static_cast<int32_t>(testData.size() * sizeof(int32_t)));

    std::vector<int32_t> result = {99, 98, 97}; // Initialize with different values
    AAX_Result err = GetArgument(task, kArrayArgType, result);
    EXPECT_EQ(AAX_ERROR_INVALID_ARGUMENT, err);

    EXPECT_EQ(3, result.size());
    EXPECT_EQ(99, result[0]); // Result should be unchanged when error occurs
}

//--------------------------------------------------------------------
#pragma mark - GetArgument C-String Tests

TEST_F(AAX_TaskUtilities, GetArgumentCString)
{
    using namespace AAX::Task;

    // Test successful C-string extraction
    MockTask task(kTestTaskType);
    const char* testString = "Hello, World!";
    task.AddMockArgument(kStringArgType, testString, static_cast<int32_t>(strlen(testString) + 1));

    const char* result = nullptr;
    AAX_Result err = GetArgument(task, kStringArgType, result);

    EXPECT_EQ(AAX_SUCCESS, err);
    EXPECT_NE(nullptr, result);
    EXPECT_STREQ(testString, result);
}

TEST_F(AAX_TaskUtilities, GetArgumentCStringEmpty)
{
    using namespace AAX::Task;

    // Test empty C-string
    MockTask task(kTestTaskType);
    const char* testString = "";
    task.AddMockArgument(kStringArgType, testString, 1);

    const char* result = nullptr;
    AAX_Result err = GetArgument(task, kStringArgType, result);

    EXPECT_EQ(AAX_SUCCESS, err);
    EXPECT_NE(nullptr, result);
    EXPECT_STREQ("", result);
}

TEST_F(AAX_TaskUtilities, GetArgumentCStringNull)
{
    using namespace AAX::Task;

    // Test null data pointer
    MockTask task(kTestTaskType);
    task.AddMockArgument(kStringArgType, nullptr, 0);

    const char* result = reinterpret_cast<const char*>(0x12345); // Initialize with non-null value
    AAX_Result err = GetArgument(task, kStringArgType, result);

    EXPECT_EQ(AAX_SUCCESS, err);
    EXPECT_EQ(reinterpret_cast<const char*>(0x12345), result); // Should be unchanged
}

TEST_F(AAX_TaskUtilities, GetArgumentCStringTypeMismatch)
{
    using namespace AAX::Task;

    // Test C-string with type mismatch
    MockTask task(kTestTaskType);
    const char* testString = "Test String";
    task.AddMockArgument(kMismatchType, testString, static_cast<int32_t>(strlen(testString) + 1));

    const char* result = reinterpret_cast<const char*>(0x12345);
    AAX_Result err = GetArgument(task, kStringArgType, result);
    EXPECT_EQ(AAX_ERROR_INVALID_ARGUMENT, err);

    EXPECT_EQ(reinterpret_cast<const char*>(0x12345), result); // Should be unchanged when error occurs
}

//--------------------------------------------------------------------
#pragma mark - GetArgument std::string Tests

TEST_F(AAX_TaskUtilities, GetArgumentStdString)
{
    using namespace AAX::Task;

    // Test successful std::string extraction
    MockTask task(kTestTaskType);
    const char* testString = "Hello, AAX!";
    task.AddMockArgument(kStringArgType, testString, static_cast<int32_t>(strlen(testString) + 1));

    std::string result;
    AAX_Result err = GetArgument(task, kStringArgType, result);

    EXPECT_EQ(AAX_SUCCESS, err);
    EXPECT_EQ(testString, result);
}

TEST_F(AAX_TaskUtilities, GetArgumentStdStringEmpty)
{
    using namespace AAX::Task;

    // Test empty std::string
    MockTask task(kTestTaskType);
    const char* testString = "";
    task.AddMockArgument(kStringArgType, testString, 1);

    std::string result = "initial_value";
    AAX_Result err = GetArgument(task, kStringArgType, result);

    EXPECT_EQ(AAX_SUCCESS, err);
    EXPECT_EQ("", result);
}

TEST_F(AAX_TaskUtilities, GetArgumentStdStringNull)
{
    using namespace AAX::Task;

    // Test null data pointer for std::string
    MockTask task(kTestTaskType);
    task.AddMockArgument(kStringArgType, nullptr, 0);

    std::string result = "initial_value";
    AAX_Result err = GetArgument(task, kStringArgType, result);

    EXPECT_EQ(AAX_SUCCESS, err);
    EXPECT_EQ("initial_value", result); // Should be unchanged when data is null
}

TEST_F(AAX_TaskUtilities, GetArgumentStdStringTypeMismatch)
{
    using namespace AAX::Task;

    // Test std::string with type mismatch
    MockTask task(kTestTaskType);
    const char* testString = "Test String";
    task.AddMockArgument(kMismatchType, testString, static_cast<int32_t>(strlen(testString) + 1));

    std::string result = "initial_value";
    AAX_Result err = GetArgument(task, kStringArgType, result);
    EXPECT_EQ(AAX_ERROR_INVALID_ARGUMENT, err);

    EXPECT_EQ("initial_value", result); // Should be unchanged when error occurs
}

//--------------------------------------------------------------------
#pragma mark - Edge Cases and Error Conditions

TEST_F(AAX_TaskUtilities, GetArgumentLargeData)
{
    using namespace AAX::Task;

    // Test with large data arrays
    MockTask task(kTestTaskType);
    std::vector<int32_t> largeData(1000);
    for (size_t i = 0; i < largeData.size(); ++i) {
        largeData[i] = static_cast<int32_t>(i);
    }
    task.AddMockArgument(kArrayArgType, largeData.data(), static_cast<int32_t>(largeData.size() * sizeof(int32_t)));

    std::vector<int32_t> result;
    AAX_Result err = GetArgument(task, kArrayArgType, result);

    EXPECT_EQ(AAX_SUCCESS, err);
    EXPECT_EQ(largeData.size(), result.size());
    EXPECT_EQ(largeData, result);
}

TEST_F(AAX_TaskUtilities, GetArgumentLongString)
{
    using namespace AAX::Task;

    // Test with very long string
    MockTask task(kTestTaskType);
    std::string longString(10000, 'A'); // 10,000 'A' characters
    longString += '\0'; // Null terminate
    task.AddMockArgument(kStringArgType, longString.c_str(), static_cast<int32_t>(longString.length()));

    std::string result;
    AAX_Result err = GetArgument(task, kStringArgType, result);

    EXPECT_EQ(AAX_SUCCESS, err);
    EXPECT_EQ(10000, result.length());
    EXPECT_EQ(std::string(10000, 'A'), result);
}

TEST_F(AAX_TaskUtilities, GetArgumentStringWithNullChars)
{
    using namespace AAX::Task;

    // Test string containing null characters
    MockTask task(kTestTaskType);
    const char testData[] = {'H', 'e', 'l', 'l', 'o', '\0', 'W', 'o', 'r', 'l', 'd', '\0'};
    task.AddMockArgument(kStringArgType, testData, sizeof(testData));

    const char* result = nullptr;
    AAX_Result err = GetArgument(task, kStringArgType, result);

    EXPECT_EQ(AAX_SUCCESS, err);
    EXPECT_NE(nullptr, result);
    EXPECT_STREQ("Hello", result); // Should stop at first null character
}

TEST_F(AAX_TaskUtilities, GetArgumentVectorOddSize)
{
    using namespace AAX::Task;

    // Test vector with size not perfectly divisible by element size
    MockTask task(kTestTaskType);
    char testData[] = {1, 2, 3, 4, 5, 6, 7}; // 7 bytes, not divisible by sizeof(int32_t)
    task.AddMockArgument(kArrayArgType, testData, sizeof(testData));

    std::vector<int32_t> result;
    AAX_Result err = GetArgument(task, kArrayArgType, result);

    EXPECT_EQ(AAX_SUCCESS, err);
    EXPECT_EQ(1, result.size()); // Should only get 1 complete int32_t (7 / 4 = 1)
}

TEST_F(AAX_TaskUtilities, GetArgumentMultipleTypes)
{
    using namespace AAX::Task;

    // Test task with multiple argument types
    MockTask task(kTestTaskType);

    int32_t intValue = 42;
    float floatValue = 3.14f;
    const char* stringValue = "Test";
    std::vector<int32_t> arrayValue = {10, 20, 30};

    task.AddMockArgument(kIntArgType, &intValue, sizeof(intValue));
    task.AddMockArgument(kFloatArgType, &floatValue, sizeof(floatValue));
    task.AddMockArgument(kStringArgType, stringValue, static_cast<int32_t>(strlen(stringValue) + 1));
    task.AddMockArgument(kArrayArgType, arrayValue.data(), static_cast<int32_t>(arrayValue.size() * sizeof(int32_t)));

    // Test extracting each type
    int32_t intResult = 0;
    float floatResult = 0.0f;
    std::string stringResult;
    std::vector<int32_t> arrayResult;

    EXPECT_EQ(AAX_SUCCESS, GetArgument(task, kIntArgType, intResult));
    EXPECT_EQ(AAX_SUCCESS, GetArgument(task, kFloatArgType, floatResult));
    EXPECT_EQ(AAX_SUCCESS, GetArgument(task, kStringArgType, stringResult));
    EXPECT_EQ(AAX_SUCCESS, GetArgument(task, kArrayArgType, arrayResult));

    EXPECT_EQ(intValue, intResult);
    EXPECT_FLOAT_EQ(floatValue, floatResult);
    EXPECT_EQ(stringValue, stringResult);
    EXPECT_EQ(arrayValue, arrayResult);
}

//--------------------------------------------------------------------
#pragma mark - Custom Type Tests

TEST_F(AAX_TaskUtilities, GetArgumentCustomStruct)
{
    using namespace AAX::Task;

    // Test with custom struct
    struct TestStruct {
        int32_t id;
        float value;
        char name[16];
    };

    MockTask task(kTestTaskType);
    TestStruct testData = {123, 4.56f, "TestName"};
    task.AddMockArgument(kIntArgType, &testData, sizeof(testData));

    TestStruct result = {};
    AAX_Result err = GetArgument(task, kIntArgType, result);

    EXPECT_EQ(AAX_SUCCESS, err);
    EXPECT_EQ(testData.id, result.id);
    EXPECT_FLOAT_EQ(testData.value, result.value);
    EXPECT_STREQ(testData.name, result.name);
}

TEST_F(AAX_TaskUtilities, GetArgumentBooleanValues)
{
    using namespace AAX::Task;

    // Test with boolean values
    MockTask task(kTestTaskType);
    bool trueValue = true;
    bool falseValue = false;

    task.AddMockArgument('BOOL', &trueValue, sizeof(trueValue));

    bool result = false;
    AAX_Result err = GetArgument(task, 'BOOL', result);

    EXPECT_EQ(AAX_SUCCESS, err);
    EXPECT_TRUE(result);

    // Test false value
    task.AddMockArgument('BOO2', &falseValue, sizeof(falseValue));
    result = true; // Reset to opposite value
    err = GetArgument(task, 'BOO2', result);

    EXPECT_EQ(AAX_SUCCESS, err);
    EXPECT_FALSE(result);
}
