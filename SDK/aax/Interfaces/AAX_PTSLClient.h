/*================================================================================================*/
/*
 *
 *  Copyright 2025 Avid Technology, Inc.
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

/** 
 *	\file   AAX_PTSLClient.h
 *
 *	\brief  Utility class to support PTSL task execution from an AAX plugin
 */
/*================================================================================================*/

#pragma once
#ifndef AAX_PTSLCLIENT_H
#define AAX_PTSLCLIENT_H

// PTSL Includes
#include "PTSLC_CPP/CppPTSLClient.h"
#include "PTSLC_CPP/CppPTSLRequest.h"
#include "PTSLC_CPP/CppPTSLResponse.h"

// AAX Includes
#include "AAX_Assert.h"

// Standard Includes
#include <functional>
#include <vector>
#include <queue>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <optional>
#include <atomic>

namespace AAX
{
	/** RAII helper class to run a function when a scope is exited
	 */
	template <typename F>
	struct DoWhenExitingScope{
		F mFunction; // must take no parameters; return value is ignored

		explicit DoWhenExitingScope(const F& function) : mFunction(function) {}
		explicit DoWhenExitingScope(F&& function) : mFunction(std::move(function)) {}
		~DoWhenExitingScope() { mFunction(); }
	};

	/** RAII helper class to set a value when a scope is exited
	 */
	template <typename T, typename V>
	struct SetWhenExitingScope {
		T& mElem;
		V mVal;

		explicit SetWhenExitingScope(T& elem, const V& val) : mElem(elem), mVal(val) {}
		explicit SetWhenExitingScope(T& elem, V&& val) : mElem(elem), mVal(std::move(val)) {}
		~SetWhenExitingScope() { mElem = mVal; }
	};
}

namespace AAX_PTSLClient_Helper
{
	static std::string ParseResponse(const PTSLC_CPP::CppPTSLResponse& rsp)
	{
		if (rsp.GetStatus() == PTSLC_CPP::CommandStatusType::Completed)
		{
			AAX_TRACE_RELEASE (kAAX_Trace_Priority_Low, "AAX_PTSLClient_Helper::ParseResponse: GetResponseBodyJson: %s", rsp.GetResponseBodyJson().c_str());
			return rsp.GetResponseBodyJson();
		}
		else if (rsp.GetStatus() == PTSLC_CPP::CommandStatusType::Failed)
		{
			AAX_TRACE_RELEASE (kAAX_Trace_Priority_Low, "AAX_PTSLClient_Helper::ParseResponse: GetResponseErrorJson: %s", rsp.GetResponseErrorJson().c_str());
			return rsp.GetResponseErrorJson();
		}
		else
		{
			AAX_TRACE_RELEASE (kAAX_Trace_Priority_Low, "AAX_PTSLClient_Helper::ParseResponse: Unknown status");
			return std::string{};
		}
	}
}

namespace AAX_PTSLClient_Function
{
	template <class ThreadData>
	std::optional<PTSLC_CPP::CppPTSLRequest> DoConnectPTSLClient(ThreadData* threadData)
	{
		AAX::SetWhenExitingScope resetter(threadData->mClientInitializing, false);

		// Called from Run() via function pointer in mTaskQueue - see ConnectPTSLClient
		if (threadData->mClientInitialized) {
			AAX_TRACE_RELEASE (kAAX_Trace_Priority_Low, "DemoPanel_PTSLClient.cpp - DoConnectPTSLClient: PTSL Client already initialized");
			return std::nullopt;
		}

		const PTSLC_CPP::ClientConfig config
		{
			"localhost:31416",							// server address -- note the server port!
			PTSLC_CPP::Mode::ProTools,					// turn off stand-alone mode -- look for a server instead of launching our own (see CppPTSLClient::LocateServer)
			PTSLC_CPP::SkipHostLaunch::Yes				// skip auto launch -- do not try to launch Pro Tools if it is not running already
		};

		if (!threadData->mClientPtr)
		{
			threadData->mClientPtr = std::make_unique<PTSLC_CPP::CppPTSLClient>(config);
			PTSLC_CPP::CppPTSLRequest requestRegister {
				PTSLC_CPP::CommandId::RegisterConnection,
				R"({ "company_name": ")" + threadData->mCompanyName + R"(", "application_name": ")" + threadData->mApplicationName + R"(" })" };
			PTSLC_CPP::CppPTSLResponse responseRegister = (threadData->mClientPtr)->SendRequest(requestRegister).get();

			AAX_PTSLClient_Helper::ParseResponse(responseRegister);

			if (responseRegister.GetStatus() == PTSLC_CPP::CommandStatusType::Failed)
			{
				AAX_TRACE_RELEASE (kAAX_Trace_Priority_High, "AAX_PTSLClient_Function::DoConnectPTSLClient: PTSL Client failed to initialize");
				threadData->mClientInitialized = false;
				threadData->mClientPtr.reset(nullptr);
			}
			else{
				threadData->mClientInitialized = true;
			}
		}
		else
		{
			AAX_TRACE_RELEASE (kAAX_Trace_Priority_Low, "AAX_PTSLClient_Function::DoConnectPTSLClient: PTSL Client object already exists");
		}

		return std::nullopt; // We ran the command synchronously
	}
}

/** Data structure to hold PTSL client thread data

	This structure may be extended by a client implementation.
 */
template <class Derived = void>
struct AAX_PTSLClient_ThreadData
{
	// Support inheritance
	virtual ~AAX_PTSLClient_ThreadData() = default;

	// Use CRTP to determine the correct data type for the task queue
	using type = typename std::conditional<
		std::is_same<Derived, void>::value,
		AAX_PTSLClient_ThreadData,
		Derived
	>::type;

	using task_function_type = std::function<std::optional<PTSLC_CPP::CppPTSLRequest>(type*)>;
	using callback_function_type = std::function<void(const PTSLC_CPP::CppPTSLResponse&)>;

	// Queue of thread tasks
	std::queue<std::pair<task_function_type, callback_function_type>> mTaskQueue{};

	// Final responses for commands
	std::unordered_map<PTSLC_CPP::CommandId, std::vector<std::shared_future<PTSLC_CPP::CppPTSLResponse>>> mResponseMap{};

	// The managed PTSL C++ client object
	std::unique_ptr<PTSLC_CPP::CppPTSLClient> mClientPtr{};

	// Thread synchronization
	std::mutex mTaskQueueMutex{}; // for mTaskQueue
	std::mutex mResponseMutex{}; // for mResponseMap
	std::condition_variable mTaskQueueCV{};
	bool mCancelFlag{false};

	// Session information
	std::atomic<bool> mClientInitializing{false};
	bool mClientInitialized{false};
	std::string mCompanyName{};
	std::string mApplicationName{};
};

using AAX_PTSLClient_DefaultThreadData = AAX_PTSLClient_ThreadData<>;

/** Helper class to support PTSL task execution

	Intended to be created and destroyed as part of the plugin's C++ object
	graph, e.g. with an object of this type as a member of the plugin's
	\ref AAX_IEffectParameters derived class.
 */
template <class ThreadData = AAX_PTSLClient_DefaultThreadData>
class AAX_PTSLClient
{
public:
	using thread_data_type = typename ThreadData::type;
	using task_function_type = typename ThreadData::task_function_type;
	using callback_function_type = typename ThreadData::callback_function_type;

	/** Set up and start the PTSL thread.
	 */
	AAX_PTSLClient(std::string const & companyName, std::string const & applicationName);

	// This class is not copyable. It is movable.
	AAX_PTSLClient(const AAX_PTSLClient &) = delete;
	AAX_PTSLClient & operator=(const AAX_PTSLClient &) = delete;
	AAX_PTSLClient(AAX_PTSLClient &&) = default;
	AAX_PTSLClient & operator=(AAX_PTSLClient &&) = default;

	/** Stop the PTSL thread and remove the PTSL client.

		Note: The thread is detached and will stop itself asynchronously
		once any in-flight task has returned. The thread data structure
		will be deleted once the thread has stopped.
	 */
	virtual ~AAX_PTSLClient();

	/** Add a task to connect this client to the PTSL server
	 */
	void ConnectPTSLClient();

	/** Add a task for execution on the PTSL thread

		The task's callback function argument will be empty.
	 */
	void AddTask(task_function_type && task);

	/** Add a task for execution on the PTSL thread with a callback function

		Note that both the PTSL thread and the callback function will execute
		asynchronously. The callback function must be safe to call even
		after the AAX_PTSLClient object has been destroyed.
	 */
	void AddTask(task_function_type && task, callback_function_type && callback);

	// If the generic AddTask() method is not sufficient then a client
	// implementation may extend this class with additional methods that
	// use custom information passed in the thread data structure. The
	// thread data structure itself may be extended using the ThreadData
	// template parameter.

private:
	/** Start the PTSL thread
	 */
	void StartThread(std::string const & companyName, std::string const & applicationName);

	/** Stop the PTSL thread
	
		Note: This method returns immediately. The thread will detach, stop,
		and destroy itself asynchronously once any in-flight task has returned.
	 */
	void StopThread();

	/** PTSL client thread function
	 */
	static void Run(ThreadData * dataPtr);

private:
	ThreadData * mThreadData{nullptr};
	std::thread mPTSLThread{};
};

template <typename ThreadData>
AAX_PTSLClient<ThreadData>::AAX_PTSLClient(std::string const & companyName, std::string const & applicationName)
{
	this->StartThread(companyName, applicationName);
	AAX_ASSERT(mThreadData);
}

template <typename ThreadData>
AAX_PTSLClient<ThreadData>::~AAX_PTSLClient()
{
	try {
		this->StopThread();
	}
	catch(...) {
		// nothing to be done here
	}
}

template <typename ThreadData>
void AAX_PTSLClient<ThreadData>::ConnectPTSLClient()
{
	{
		std::unique_lock<std::mutex> lock(mThreadData->mTaskQueueMutex);
		if (mThreadData->mCancelFlag || mThreadData->mClientInitializing) {
			AAX_TRACE_RELEASE (kAAX_Trace_Priority_High, "DemoPanel_PTSLClient::ConnectPTSLClient: DoConnectPTSLClient task not added to task queue");
			return;
		}
		mThreadData->mClientInitializing = true;
		mThreadData->mTaskQueue.push(std::make_pair(
			std::bind(&AAX_PTSLClient_Function::DoConnectPTSLClient<ThreadData>, std::placeholders::_1),
			callback_function_type{}));
	}
	AAX_TRACE_RELEASE (kAAX_Trace_Priority_Low, "DemoPanel_PTSLClient::ConnectPTSLClient: DoConnectPTSLClient task added to task queue");
	mThreadData->mTaskQueueCV.notify_one();
}

template <typename ThreadData>
void AAX_PTSLClient<ThreadData>::AddTask(task_function_type && task)
{
	{
		std::unique_lock<std::mutex> lock(mThreadData->mTaskQueueMutex);
		mThreadData->mTaskQueue.push(std::make_pair(std::move(task), callback_function_type{}));
	}
	mThreadData->mTaskQueueCV.notify_one();
}

template <typename ThreadData>
void AAX_PTSLClient<ThreadData>::AddTask(task_function_type && task, callback_function_type && callback)
{
	{
		std::unique_lock<std::mutex> lock(mThreadData->mTaskQueueMutex);
		mThreadData->mTaskQueue.push(std::make_pair(std::move(task), std::move(callback)));
	}
	mThreadData->mTaskQueueCV.notify_one();
}

template <typename ThreadData>
void AAX_PTSLClient<ThreadData>::StartThread(std::string const & companyName, std::string const & applicationName)
{
	mThreadData = new ThreadData{};
	mThreadData->mCompanyName = companyName;
	mThreadData->mApplicationName = applicationName;
	mPTSLThread = std::thread(&AAX_PTSLClient<ThreadData>::Run, mThreadData);
}

template <typename ThreadData>
void AAX_PTSLClient<ThreadData>::StopThread()
{
	if (mPTSLThread.joinable()) 
	{
		std::unique_lock<std::mutex> lock(mThreadData->mTaskQueueMutex);
		mThreadData->mCancelFlag = true;
		mThreadData->mTaskQueueCV.notify_one(); // notify while lock is held to avoid dangling thread data
		mPTSLThread.detach();
	}
	else
	{
		AAX_TRACE_RELEASE(
			kAAX_Trace_Priority_High,
			"AAX_PTSLClient::StopThread() - thread cannot be joined");
		if (mThreadData) {
			delete mThreadData;
			mThreadData = nullptr;
		}
	}
}

// static
template <typename ThreadData>
void AAX_PTSLClient<ThreadData>::Run(ThreadData * dataPtr)
{
	if (!dataPtr) { return; }
 
	// Ensure ThreadData is deleted
	AAX::DoWhenExitingScope dataDeleter([dataPtr](){ delete dataPtr; });
 
	ThreadData & data = *dataPtr;
 
	{
		std::unique_lock<std::mutex> taskQueueLock(data.mTaskQueueMutex);

		// This thread holds the mutex
		while (!data.mCancelFlag || !data.mTaskQueue.empty()) // TODO: Once mCancelFlag is set, only perform must-run commands like cleanup.
		{
			if(data.mTaskQueue.empty()) {   // Do not wait if there are more tasks in queue
				data.mTaskQueueCV.wait(taskQueueLock);   // Release the mutex and wait for a notify
			}
			// This thread holds the mutex
			else
			{
				auto & nextTaskPair = data.mTaskQueue.front(); // Grab the task
				auto & nextTask = nextTaskPair.first; // The function to run
				auto & responseCallback = nextTaskPair.second; // The callback function
				
				{
					// Allow other threads to add tasks while this task runs
					taskQueueLock.unlock();
					AAX::DoWhenExitingScope locker([&taskQueueLock](){ taskQueueLock.lock(); });

					std::optional<PTSLC_CPP::CppPTSLRequest> request = nextTask(dataPtr);
					if (request.has_value())
					{
						auto const cid = request->GetCommandId();
						auto responseFuture = data.mClientPtr->SendRequest(request.value(), responseCallback);
						{
							std::lock_guard<std::mutex> responseDataLock(data.mResponseMutex);
							auto & responseFutures = data.mResponseMap[cid];
							responseFutures.push_back(std::move(responseFuture));
						}
					}

					// locker re-locks the mutex here
				}

				data.mTaskQueue.pop();
			}
		}
	}

	data.mClientPtr.reset(nullptr);

	// dataDeleter deletes ThreadData here
}

#endif // AAX_PTSLCLIENT_H
