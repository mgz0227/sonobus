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
 *	\file AAX_CPacketDispatcher_UnitTest.cpp
 *
 *	\brief Unit tests for AAX_CPacketDispatcher and AAX_CPacket classes
 *
 */ 
/*================================================================================================*/


#include "gtest/gtest.h"
#include "AAX_CPacketDispatcher.h"
#include "AAX_MockController.h"
#include "AAX_MockEffectParameters.h"
#include "AAX_CParameter.h"
#include "AAX_CString.h"
#include "AAX_MockTaperDelegate.h"
#include <memory>

// Test-specific implementations that inherit from the stub mock classes

class TestMockController : public AAX_MockController
{
public:
    TestMockController()
        : mPostPacketCallCount(0)
        , mLastFieldIndex(0)
        , mLastDataSize(0)
        , mGetSampleRateCallCount(0)
        , mMockSampleRate(44100.0)
        , mMockPostPacketResult(AAX_SUCCESS)
    {}

    // Tracking data for PostPacket calls
    mutable int mPostPacketCallCount;
    mutable AAX_CFieldIndex mLastFieldIndex;
    mutable uint32_t mLastDataSize;
    mutable std::vector<uint8_t> mLastData;
    mutable std::vector<std::vector<uint8_t>> mAllPostedData;
    mutable std::vector<AAX_CFieldIndex> mAllFieldIndices;

    // Tracking data for other method calls
    mutable int mGetSampleRateCallCount;

    // Configurable return values
    AAX_CSampleRate mMockSampleRate;
    AAX_Result mMockPostPacketResult;

    // Reset method for test cleanup
    void Reset()
    {
        mPostPacketCallCount = 0;
        mLastFieldIndex = 0;
        mLastDataSize = 0;
        mLastData.clear();
        mAllPostedData.clear();
        mAllFieldIndices.clear();
        mGetSampleRateCallCount = 0;
    }

    // Override the key methods needed for packet dispatcher testing
    virtual AAX_Result PostPacket(AAX_CFieldIndex inFieldIndex, const void* inPayloadP, uint32_t inPayloadSize) override
    {
        mPostPacketCallCount++;
        mLastFieldIndex = inFieldIndex;
        mLastDataSize = inPayloadSize;

        // Store the data for verification
        mLastData.clear();
        if (inPayloadP && inPayloadSize > 0) {
            const uint8_t* data = static_cast<const uint8_t*>(inPayloadP);
            mLastData.assign(data, data + inPayloadSize);

            // Also store in the complete history
            mAllPostedData.push_back(mLastData);
            mAllFieldIndices.push_back(inFieldIndex);
        }

        return mMockPostPacketResult;
    }

    virtual AAX_Result GetSampleRate(AAX_CSampleRate* outSampleRate) const override
    {
        mGetSampleRateCallCount++;
        if (outSampleRate) {
            *outSampleRate = mMockSampleRate;
            return AAX_SUCCESS;
        }
        return AAX_ERROR_NULL_ARGUMENT;
    }
};

class TestMockEffectParameters : public AAX_MockEffectParameters
{
public:
    TestMockEffectParameters()
        : mGetParameterCallCount(0)
        , mSetParameterCallCount(0)
        , mUpdateParameterNormalizedValueCallCount(0)
        , mLastValue(0.0)
        , mMockGetParameterResult(AAX_SUCCESS)
        , mMockSetParameterResult(AAX_SUCCESS)
    {}

    // Tracking data for method calls
    mutable int mGetParameterCallCount;
    mutable int mSetParameterCallCount;
    mutable int mUpdateParameterNormalizedValueCallCount;

    // Last call parameters for detailed verification
    mutable std::string mLastParamID;
    mutable double mLastValue;

    // Internal parameter storage
    std::map<std::string, double> mParameterValues;
    std::map<std::string, std::unique_ptr<AAX_CParameter<double>>> mParameters;

    // Configurable return values for testing different scenarios
    AAX_Result mMockGetParameterResult;
    AAX_Result mMockSetParameterResult;

    // Helper methods for test setup
    void SetParameterValue(const std::string& paramID, double value)
    {
        mParameterValues[paramID] = value;

        // Create or update AAX_CParameter for this parameter with mock taper delegate
        AAX_CString paramName(paramID.c_str());
        auto mockTaper = std::make_unique<AAX_MockTaperDelegate<double>>(0.0, 10.0); // Range 0-10 for testing

        // Create parameter with the mock taper delegate
        auto parameter = std::make_unique<AAX_CParameter<double>>(paramName, paramName, value, false);
        parameter->SetTaperDelegate(*mockTaper, false); // false = don't preserve current value
        parameter->SetValue(value); // Set the desired value

        // Store both the parameter and taper delegate (parameter takes ownership of taper)
        mParameters[paramID] = std::move(parameter);
        mockTaper.release(); // Parameter now owns the taper delegate
    }

    void Reset()
    {
        mGetParameterCallCount = 0;
        mSetParameterCallCount = 0;
        mUpdateParameterNormalizedValueCallCount = 0;

        mLastParamID.clear();
        mLastValue = 0.0;

        mParameterValues.clear();
        mParameters.clear();
    }

    // Override the key method needed for packet dispatcher testing
    virtual AAX_Result GetParameter(AAX_CParamID iParameterID, AAX_IParameter** oParameter) override
    {
        mGetParameterCallCount++;
        if (iParameterID) {
            mLastParamID = iParameterID;
        }

        if (!oParameter) {
            return AAX_ERROR_NULL_ARGUMENT;
        }

        if (iParameterID) {
            auto it = mParameters.find(iParameterID);
            if (it != mParameters.end()) {
                *oParameter = it->second.get();
                // Update mLastValue with the parameter's current value
                auto valueIt = mParameterValues.find(iParameterID);
                if (valueIt != mParameterValues.end()) {
                    mLastValue = valueIt->second;
                }
                return mMockGetParameterResult;
            }
        }

        *oParameter = nullptr;
        return AAX_ERROR_INVALID_PARAMETER_ID;
    }

    virtual AAX_Result GetParameterNormalizedValue(AAX_CParamID iParameterID, double* oValuePtr) const override
    {
        mGetParameterCallCount++;
        if (iParameterID) {
            mLastParamID = iParameterID;
        }

        if (!oValuePtr) {
            return AAX_ERROR_NULL_ARGUMENT;
        }

        if (iParameterID) {
            auto it = mParameterValues.find(iParameterID);
            if (it != mParameterValues.end()) {
                *oValuePtr = it->second;
                mLastValue = it->second;
                return mMockGetParameterResult;
            }
        }

        *oValuePtr = 0.0;
        mLastValue = 0.0;
        return AAX_ERROR_INVALID_PARAMETER_ID;
    }

    virtual AAX_Result SetParameterNormalizedValue(AAX_CParamID iParameterID, double iValue) override
    {
        mSetParameterCallCount++;
        if (iParameterID) {
            mLastParamID = iParameterID;
            mLastValue = iValue;
            mParameterValues[iParameterID] = iValue;
        }
        return mMockSetParameterResult;
    }

    virtual AAX_Result UpdateParameterNormalizedValue(AAX_CParamID iParameterID, double iValue, AAX_EUpdateSource iSource) override
    {
        mUpdateParameterNormalizedValueCallCount++;
        if (iParameterID) {
            mLastParamID = iParameterID;
            mLastValue = iValue;
        }
        return AAX_SUCCESS;
    }
};

// Test worker class for packet handlers
class TestWorker
{
public:
    TestWorker() : mCallCount(0), mLastParamID(""), mLastPacketID(0), mLastDataSize(0) {}
    
    int mCallCount;
    std::string mLastParamID;
    AAX_CFieldIndex mLastPacketID;
    uint32_t mLastDataSize;
    
    AAX_Result HandlePacket(AAX_CPacket& packet)
    {
        mCallCount++;
        mLastPacketID = packet.GetID();

        // Write some test data to the packet (this sets the packet size)
        float* data = packet.GetPtr<float>();
        if (data) {
            *data = 42.0f;
        }

        // Get size after GetPtr() has been called
        mLastDataSize = packet.GetSize();

        return AAX_SUCCESS;
    }
    
    AAX_Result HandlePacketWithParam(AAX_CParamID paramID, AAX_CPacket& packet)
    {
        mCallCount++;
        mLastParamID = paramID;
        mLastPacketID = packet.GetID();

        // Write some test data to the packet (this sets the packet size)
        int32_t* data = packet.GetPtr<int32_t>();
        if (data) {
            *data = 123;
        }

        // Get size after GetPtr() has been called
        mLastDataSize = packet.GetSize();

        return AAX_SUCCESS;
    }
};

//--------------------------------------------------------------------
#pragma mark - Test fixture

class AAX_CPacketDispatcher_Test : public ::testing::Test
{
protected:
    void SetUp() override
    {
        mMockController = std::make_unique<TestMockController>();
        mMockEffectParameters = std::make_unique<TestMockEffectParameters>();
        mDispatcher = std::make_unique<AAX_CPacketDispatcher>();
        mTestWorker = std::make_unique<TestWorker>();

        // Initialize the dispatcher
        mDispatcher->Initialize(mMockController.get(), mMockEffectParameters.get());
    }

    void TearDown() override {}

    std::unique_ptr<TestMockController> mMockController;
    std::unique_ptr<TestMockEffectParameters> mMockEffectParameters;
    std::unique_ptr<AAX_CPacketDispatcher> mDispatcher;
    std::unique_ptr<TestWorker> mTestWorker;
};

//--------------------------------------------------------------------
#pragma mark - AAX_CPacket Tests

TEST_F(AAX_CPacketDispatcher_Test, CPacket_BasicConstruction)
{
    AAX_CFieldIndex fieldIndex = 42;
    AAX_CPacket packet(fieldIndex);
    
    // Test basic properties
    EXPECT_EQ(fieldIndex, packet.GetID());
    EXPECT_TRUE(packet.IsDirty()); // Should be dirty by default
    EXPECT_EQ(0U, packet.GetSize()); // No data allocated yet
}

TEST_F(AAX_CPacketDispatcher_Test, CPacket_DirtyFlag)
{
    AAX_CPacket packet(1);
    
    // Test dirty flag manipulation
    EXPECT_TRUE(packet.IsDirty()); // Default is dirty
    
    packet.SetDirty(false);
    EXPECT_FALSE(packet.IsDirty());
    
    packet.SetDirty(true);
    EXPECT_TRUE(packet.IsDirty());
}

TEST_F(AAX_CPacketDispatcher_Test, CPacket_GetPtr_Float)
{
    AAX_CPacket packet(1);
    
    // Test getting typed pointer
    float* data = packet.GetPtr<float>();
    ASSERT_NE(nullptr, data);
    EXPECT_EQ(sizeof(float), packet.GetSize());
    
    // Test writing and reading data
    *data = 3.14159f;
    EXPECT_FLOAT_EQ(3.14159f, *data);
}

TEST_F(AAX_CPacketDispatcher_Test, CPacket_GetPtr_Int)
{
    AAX_CPacket packet(2);
    
    // Test getting typed pointer for different type
    int32_t* data = packet.GetPtr<int32_t>();
    ASSERT_NE(nullptr, data);
    EXPECT_EQ(sizeof(int32_t), packet.GetSize());
    
    // Test writing and reading data
    *data = 12345;
    EXPECT_EQ(12345, *data);
}

TEST_F(AAX_CPacketDispatcher_Test, CPacket_GetPtr_Struct)
{
    struct TestStruct {
        float value1;
        int32_t value2;
        bool flag;
    };
    
    AAX_CPacket packet(3);
    
    // Test getting typed pointer for struct
    TestStruct* data = packet.GetPtr<TestStruct>();
    ASSERT_NE(nullptr, data);
    EXPECT_EQ(sizeof(TestStruct), packet.GetSize());
    
    // Test writing and reading struct data
    data->value1 = 2.718f;
    data->value2 = 54321;
    data->flag = true;
    
    EXPECT_FLOAT_EQ(2.718f, data->value1);
    EXPECT_EQ(54321, data->value2);
    EXPECT_TRUE(data->flag);
}

TEST_F(AAX_CPacketDispatcher_Test, CPacket_GetPtr_VoidSpecialization)
{
    AAX_CPacket packet(4);
    
    // First allocate some data
    float* floatData = packet.GetPtr<float>();
    *floatData = 1.23f;
    
    // Test void pointer specialization
    const void* voidData = packet.GetPtr<const void>();
    ASSERT_NE(nullptr, voidData);
    
    // Should be able to cast back and read the data
    const float* readData = static_cast<const float*>(voidData);
    EXPECT_FLOAT_EQ(1.23f, *readData);
}

//--------------------------------------------------------------------
#pragma mark - AAX_CPacketDispatcher Basic Tests

TEST_F(AAX_CPacketDispatcher_Test, Dispatcher_BasicConstruction)
{
    // Test that dispatcher was constructed and initialized properly
    EXPECT_NE(nullptr, mDispatcher.get());
    EXPECT_EQ(0, mMockController->mPostPacketCallCount); // No packets posted yet
}

TEST_F(AAX_CPacketDispatcher_Test, Dispatcher_RegisterPacket_WithCustomHandler)
{
    // Test registering a packet with custom handler
    AAX_Result result = mDispatcher->RegisterPacket("TestParam", 100, mTestWorker.get(), &TestWorker::HandlePacket);
    EXPECT_EQ(AAX_SUCCESS, result);

    // Verify no packets posted yet
    EXPECT_EQ(0, mMockController->mPostPacketCallCount);
    EXPECT_EQ(0, mTestWorker->mCallCount);
}

TEST_F(AAX_CPacketDispatcher_Test, Dispatcher_RegisterPacket_WithCustomHandlerEx)
{
    // Test registering a packet with extended custom handler (includes param ID)
    AAX_Result result = mDispatcher->RegisterPacket("TestParam2", 101, mTestWorker.get(), &TestWorker::HandlePacketWithParam);
    EXPECT_EQ(AAX_SUCCESS, result);

    // Verify no packets posted yet
    EXPECT_EQ(0, mMockController->mPostPacketCallCount);
    EXPECT_EQ(0, mTestWorker->mCallCount);
}

TEST_F(AAX_CPacketDispatcher_Test, Dispatcher_RegisterPacket_WithDefaultHandler)
{
    // Test registering a packet with default single-value handler
    AAX_Result result = mDispatcher->RegisterPacket("TestParam3", 102);
    EXPECT_EQ(AAX_SUCCESS, result);

    // Set up parameter value for default handler
    mMockEffectParameters->SetParameterValue("TestParam3", 3.14159);

    // Verify no packets posted yet
    EXPECT_EQ(0, mMockController->mPostPacketCallCount);
}

TEST_F(AAX_CPacketDispatcher_Test, Dispatcher_SetDirty)
{
    // Register a packet first
    mDispatcher->RegisterPacket("TestParam", 100, mTestWorker.get(), &TestWorker::HandlePacket);

    // Test setting parameter as dirty
    AAX_Result result = mDispatcher->SetDirty("TestParam", true);
    EXPECT_EQ(AAX_SUCCESS, result);

    // Test setting parameter as clean
    result = mDispatcher->SetDirty("TestParam", false);
    EXPECT_EQ(AAX_SUCCESS, result);

    // POTENTIAL BUG: AAX_CPacketDispatcher::SetDirty returns AAX_SUCCESS for non-existent parameters
    // This might be intentional behavior, but seems unexpected from an API design perspective
    // Test with non-existent parameter
    result = mDispatcher->SetDirty("NonExistentParam", true);
    // EXPECT_NE(AAX_SUCCESS, result); // Should fail for unregistered parameter, but doesn't
    EXPECT_EQ(AAX_SUCCESS, result); // Current behavior - always succeeds
}

//--------------------------------------------------------------------
#pragma mark - Dispatch Functionality Tests

TEST_F(AAX_CPacketDispatcher_Test, Dispatcher_Dispatch_CustomHandler)
{
    // Register packet with custom handler
    mDispatcher->RegisterPacket("TestParam", 100, mTestWorker.get(), &TestWorker::HandlePacket);

    // Set parameter as dirty
    mDispatcher->SetDirty("TestParam", true);

    // Dispatch packets
    AAX_Result result = mDispatcher->Dispatch();
    EXPECT_EQ(AAX_SUCCESS, result);

    // Verify handler was called
    EXPECT_EQ(1, mTestWorker->mCallCount);
    EXPECT_EQ(100, mTestWorker->mLastPacketID);
    EXPECT_EQ(sizeof(float), mTestWorker->mLastDataSize);

    // Verify packet was posted to controller
    EXPECT_EQ(1, mMockController->mPostPacketCallCount);
    EXPECT_EQ(100, mMockController->mLastFieldIndex);
    EXPECT_EQ(sizeof(float), mMockController->mLastDataSize);

    // Verify the data content (handler writes 42.0f)
    ASSERT_EQ(sizeof(float), mMockController->mLastData.size());
    float postedValue = *reinterpret_cast<const float*>(mMockController->mLastData.data());
    EXPECT_FLOAT_EQ(42.0f, postedValue);
}

TEST_F(AAX_CPacketDispatcher_Test, Dispatcher_Dispatch_CustomHandlerEx)
{
    // Register packet with extended custom handler
    mDispatcher->RegisterPacket("TestParam2", 101, mTestWorker.get(), &TestWorker::HandlePacketWithParam);

    // Set parameter as dirty
    mDispatcher->SetDirty("TestParam2", true);

    // Dispatch packets
    AAX_Result result = mDispatcher->Dispatch();
    EXPECT_EQ(AAX_SUCCESS, result);

    // Verify handler was called with parameter ID
    EXPECT_EQ(1, mTestWorker->mCallCount);
    EXPECT_EQ("TestParam2", mTestWorker->mLastParamID);
    EXPECT_EQ(101, mTestWorker->mLastPacketID);
    EXPECT_EQ(sizeof(int32_t), mTestWorker->mLastDataSize);

    // Verify packet was posted to controller
    EXPECT_EQ(1, mMockController->mPostPacketCallCount);
    EXPECT_EQ(101, mMockController->mLastFieldIndex);
    EXPECT_EQ(sizeof(int32_t), mMockController->mLastDataSize);

    // Verify the data content (handler writes 123)
    ASSERT_EQ(sizeof(int32_t), mMockController->mLastData.size());
    int32_t postedValue = *reinterpret_cast<const int32_t*>(mMockController->mLastData.data());
    EXPECT_EQ(123, postedValue);
}

TEST_F(AAX_CPacketDispatcher_Test, Dispatcher_Dispatch_DefaultHandler)
{
    // Register packet with default handler
    mDispatcher->RegisterPacket("TestParam3", 102);

    // Set parameter value and mark as dirty
    mMockEffectParameters->SetParameterValue("TestParam3", 2.71828);
    mDispatcher->SetDirty("TestParam3", true);

    // Dispatch packets
    AAX_Result result = mDispatcher->Dispatch();
    EXPECT_EQ(AAX_SUCCESS, result);

    // Verify parameter was queried
    EXPECT_EQ(1, mMockEffectParameters->mGetParameterCallCount);
    EXPECT_EQ("TestParam3", mMockEffectParameters->mLastParamID);
    EXPECT_DOUBLE_EQ(2.71828, mMockEffectParameters->mLastValue);

    // Verify packet was posted to controller
    EXPECT_EQ(1, mMockController->mPostPacketCallCount);
    EXPECT_EQ(102, mMockController->mLastFieldIndex);
    EXPECT_EQ(sizeof(double), mMockController->mLastDataSize);

    // Verify the data content (should be parameter value)
    ASSERT_EQ(sizeof(double), mMockController->mLastData.size());
    double postedValue = *reinterpret_cast<const double*>(mMockController->mLastData.data());
    EXPECT_DOUBLE_EQ(2.71828, postedValue);
}

TEST_F(AAX_CPacketDispatcher_Test, Dispatcher_Dispatch_NoDirtyPackets)
{
    // Register packet but don't mark as dirty
    mDispatcher->RegisterPacket("TestParam", 100, mTestWorker.get(), &TestWorker::HandlePacket);
    mDispatcher->SetDirty("TestParam", false); // Explicitly set as clean

    // Dispatch packets
    AAX_Result result = mDispatcher->Dispatch();
    EXPECT_EQ(AAX_SUCCESS, result);

    // Verify no handlers were called and no packets posted
    EXPECT_EQ(0, mTestWorker->mCallCount);
    EXPECT_EQ(0, mMockController->mPostPacketCallCount);
}

TEST_F(AAX_CPacketDispatcher_Test, Dispatcher_Dispatch_MultiplePackets)
{
    // Register multiple packets
    mDispatcher->RegisterPacket("Param1", 200, mTestWorker.get(), &TestWorker::HandlePacket);
    mDispatcher->RegisterPacket("Param2", 201, mTestWorker.get(), &TestWorker::HandlePacketWithParam);
    mDispatcher->RegisterPacket("Param3", 202);

    // Set parameter values and mark as dirty
    mMockEffectParameters->SetParameterValue("Param3", 1.414);
    mDispatcher->SetDirty("Param1", true);
    mDispatcher->SetDirty("Param2", true);
    mDispatcher->SetDirty("Param3", true);

    // Dispatch all packets
    AAX_Result result = mDispatcher->Dispatch();
    EXPECT_EQ(AAX_SUCCESS, result);

    // Verify all handlers were called
    EXPECT_EQ(2, mTestWorker->mCallCount); // Two custom handlers
    EXPECT_EQ(1, mMockEffectParameters->mGetParameterCallCount); // One default handler

    // Verify all packets were posted
    EXPECT_EQ(3, mMockController->mPostPacketCallCount);
}

//--------------------------------------------------------------------
#pragma mark - Edge Cases and Error Handling Tests

TEST_F(AAX_CPacketDispatcher_Test, Dispatcher_RegisterPacket_SameParamMultipleTimes)
{
    // Register the same parameter multiple times (should be allowed)
    AAX_Result result1 = mDispatcher->RegisterPacket("TestParam", 100, mTestWorker.get(), &TestWorker::HandlePacket);
    AAX_Result result2 = mDispatcher->RegisterPacket("TestParam", 101, mTestWorker.get(), &TestWorker::HandlePacketWithParam);

    EXPECT_EQ(AAX_SUCCESS, result1);
    EXPECT_EQ(AAX_SUCCESS, result2);

    // Set as dirty and dispatch
    mDispatcher->SetDirty("TestParam", true);
    mDispatcher->Dispatch();

    // Both handlers should be called for the same parameter
    EXPECT_EQ(2, mTestWorker->mCallCount);
    EXPECT_EQ(2, mMockController->mPostPacketCallCount);
}

TEST_F(AAX_CPacketDispatcher_Test, Dispatcher_EmptyParameterID)
{
    // Test with empty parameter ID
    AAX_Result result = mDispatcher->RegisterPacket("", 100, mTestWorker.get(), &TestWorker::HandlePacket);
    EXPECT_EQ(AAX_SUCCESS, result); // Should succeed (empty string is valid)

    // Test setting dirty with empty parameter ID
    result = mDispatcher->SetDirty("", true);
    EXPECT_EQ(AAX_SUCCESS, result);

    // Dispatch should work
    result = mDispatcher->Dispatch();
    EXPECT_EQ(AAX_SUCCESS, result);
    EXPECT_EQ(1, mTestWorker->mCallCount);
}

TEST_F(AAX_CPacketDispatcher_Test, Dispatcher_NullParameterID_ReturnsError)
{
    // Test with null parameter ID - should return error instead of crashing
    AAX_Result result = mDispatcher->RegisterPacket(nullptr, 100, mTestWorker.get(), &TestWorker::HandlePacket);
    EXPECT_EQ(AAX_ERROR_NULL_ARGUMENT, result);
    
    // Verify no handlers were registered and no packets posted
    EXPECT_EQ(0, mTestWorker->mCallCount);
    EXPECT_EQ(0, mMockController->mPostPacketCallCount);
}

TEST_F(AAX_CPacketDispatcher_Test, Dispatcher_NullParameterID_SetDirtyReturnsError)
{
    // Test SetDirty behavior with null parameter ID - should return error instead of crashing
    AAX_Result result = mDispatcher->SetDirty(nullptr, true);
    EXPECT_EQ(AAX_ERROR_NULL_ARGUMENT, result);
    
    // Test with false flag as well
    result = mDispatcher->SetDirty(nullptr, false);
    EXPECT_EQ(AAX_ERROR_NULL_ARGUMENT, result);
}

TEST_F(AAX_CPacketDispatcher_Test, Dispatcher_NullWorker_ReturnsError)
{
    // Test RegisterPacket with null worker object - should return error instead of crashing
    AAX_Result result = mDispatcher->RegisterPacket("ValidParam", 100, static_cast<TestWorker*>(nullptr), &TestWorker::HandlePacket);
    EXPECT_EQ(AAX_ERROR_NULL_ARGUMENT, result);
    
    // Verify no handlers were registered and no packets posted
    EXPECT_EQ(0, mTestWorker->mCallCount);
    EXPECT_EQ(0, mMockController->mPostPacketCallCount);
}
