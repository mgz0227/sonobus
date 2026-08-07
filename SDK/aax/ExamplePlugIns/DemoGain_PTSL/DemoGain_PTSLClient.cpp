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
/*================================================================================================*/ 

// Self Include
#include "DemoGain_PTSLClient.h"

// Standard Includes
#include <optional>
#include <string>
#include <mutex>
#include <vector>
#include <unordered_map>
#include <unordered_set>

DemoGain_PTSLClient::DemoGain_PTSLClient()
{
	this->StartPTSLClient();
}

DemoGain_PTSLClient::~DemoGain_PTSLClient()
{
	this->StopPTSLClient();
}

// PTSL client methods
namespace DemoGain_PTSL_Client
{
	// This example uses global data to avoid redundant menu items or other
	// conflicts when multiple plugin instances are active.
	class GlobalData
	{
	public:
		std::string GetNextMenuItemTagForObjectAt(void const * addr)
		{
			auto lock = this->Lock();

			// Get the lowest available index
			int i = 1;
			while (mUsedMenuItemIndices.count(i) > 0)
			{
				++i;
			}

			mUsedMenuItemIndices.insert(i);
			mMenuItemIndicesPerObject[addr].push(i);
			return std::to_string(i);
		}

		void RemoveNextMenuItemTagForObjectAt(void const * addr)
		{
			auto lock = this->Lock();

			// Remove the next item index for the object in FIFO order
			auto findIter = mMenuItemIndicesPerObject.find(addr);
			if (findIter != mMenuItemIndicesPerObject.end())
			{
				auto& indices = findIter->second;
				if (!indices.empty())
				{
					mUsedMenuItemIndices.erase(indices.front());
					indices.pop();
				}

				if (indices.empty())
				{
					mMenuItemIndicesPerObject.erase(findIter);
				}
			}
			else
			{
				AAX_TRACE_RELEASE(
					kAAX_Trace_Priority_High,
					"DemoGain_PTSLClient - WARNING RemoveNextMenuItemTagForObjectAt: Key object not found at %p", addr);
			}
		}

	private:
		std::lock_guard<std::mutex> Lock() { return std::lock_guard<std::mutex>(mMutex); }

		std::mutex mMutex{};
		std::unordered_set<int> mUsedMenuItemIndices{};
		std::unordered_map<void const *, std::queue<int>> mMenuItemIndicesPerObject{};
	};

	GlobalData gData{};

	void OnPTSLCallback(const PTSLC_CPP::CppPTSLResponse& response)
	{
		AAX_TRACE_RELEASE(
			kAAX_Trace_Priority_Normal,
			"DemoGain_PTSLClient OnPTSLCallback:\n\ttask id: %s,\n\tcommand id: %d\n\tstatus: %d\n\tprogress: %d\n\tbody: %s,\n\terror: %s",
			response.GetTaskId().c_str(),
			response.GetCommandId(),
			response.GetStatus(),
			response.GetProgress(),
			response.GetResponseBodyJson().c_str(),
			response.GetResponseErrorJson().c_str());
	}

	std::string GetValueFromResponse(const std::string& key, const std::string& response)
	{
		const std::string keyString = R"(")" + key + R"(":")";
		std::string outputString = "";
		std::string::size_type pos = response.find(keyString);
		if (pos != std::string::npos)
		{
			std::string tempString = response.substr(pos + keyString.size());

			std::string::size_type endPos = tempString.find(R"(")");
			if (endPos != std::string::npos)
			{
				const size_t lengthToEnd = tempString.length() - endPos;
				tempString.erase(endPos, lengthToEnd);
				outputString = tempString;
			}
			else
			{
				AAX_TRACE_RELEASE(
					kAAX_Trace_Priority_High,
					"DemoGain_PTSLClient - GetValueFromResponse: '%s' key parsing error", key.c_str());
			}
		}
		else
		{
			AAX_TRACE_RELEASE(
				kAAX_Trace_Priority_High,
				"DemoGain_PTSLClient - GetValueFromResponse: '%s' key not found in response", key.c_str());
		}

		return outputString;
	}

	std::optional<PTSLC_CPP::CppPTSLRequest> DoPollEvents(AAX_PTSLClient_DefaultThreadData * threadData)
	{
		if (!threadData->mClientInitialized) {
			AAX_TRACE_RELEASE (
				kAAX_Trace_Priority_High,
				"DemoGain_PTSLClient - DoPollEvents: PTSL Client has not been initialized");
			return std::nullopt;
		}

		// Poll for events
		constexpr auto cid = PTSLC_CPP::CommandId::CId_PollEvents;
		return std::make_optional<PTSLC_CPP::CppPTSLRequest>(cid);
	}

	std::optional<PTSLC_CPP::CppPTSLRequest> DoAddHostMenuItem(AAX_PTSLClient_DefaultThreadData * threadData)
	{
		if (!threadData->mClientInitialized) {
			AAX_TRACE_RELEASE (
				kAAX_Trace_Priority_High,
				"DemoGain_PTSLClient - DoAddHostMenuItem: PTSL Client has not been initialized");
			return std::nullopt;
		}

		std::string tag = gData.GetNextMenuItemTagForObjectAt(threadData->mClientPtr.get());

		// Add a menu item to the Export menu
		constexpr auto cid = PTSLC_CPP::CommandId::CId_InstallMenuHandler;
		PTSLC_CPP::CppPTSLRequest request{ cid, R"({
			"menu_info": {
				"menu_area": "MArea_Export",
				"menu_label": [
					{
						"locale": "en",
						"ui_string": "DemoGain PTSL Export )" + tag + R"("
					},
					{
						"locale": "zh-cn",
						"ui_string": "从 DemoGain PTSL )" + tag + R"( 导出"
					}
				]
			}
			})"
		};

		return std::make_optional<PTSLC_CPP::CppPTSLRequest>(std::move(request));
	}

	std::optional<PTSLC_CPP::CppPTSLRequest> DoRemoveHostMenuItem(AAX_PTSLClient_DefaultThreadData * threadData)
	{
		if (!threadData->mClientInitialized) {
			AAX_TRACE_RELEASE (
				kAAX_Trace_Priority_High,
				"DemoGain_PTSLClient - DoRemoveHostMenuItem: PTSL Client has not been initialized");
			return std::nullopt;
		}

		std::string menuItemID{};

		// Get the menu item ID of the next InstallMenuHandler command in FIFO order
		{
			std::lock_guard<std::mutex> lock(threadData->mResponseMutex);
			auto responseFuturesFindIter = threadData->mResponseMap.find(PTSLC_CPP::CommandId::CId_InstallMenuHandler);
			if (responseFuturesFindIter == threadData->mResponseMap.end())
			{
				AAX_TRACE_RELEASE (
					kAAX_Trace_Priority_Normal,
					"DemoGain_PTSLClient - DoRemoveHostMenuItem: InstallMenuHandler command not found in response map");
				return std::nullopt;
			}
			auto& responseFutures = responseFuturesFindIter->second;
			if (responseFutures.empty())
			{
				AAX_TRACE_RELEASE (
					kAAX_Trace_Priority_Normal,
					"DemoGain_PTSLClient - DoRemoveHostMenuItem: No response futures found for InstallMenuHandler command");
				return std::nullopt;
			}
			auto const & response = responseFutures.back().get();
			std::string const responseStr = AAX_PTSLClient_Helper::ParseResponse(response);
			menuItemID = GetValueFromResponse("menu_item_id", responseStr);
			responseFutures.pop_back();
		}

		if (menuItemID.empty())
		{
			AAX_TRACE_RELEASE (
				kAAX_Trace_Priority_High,
				"DemoGain_PTSLClient - DoRemoveHostMenuItem: menu_item_id is empty");
			return std::nullopt;
		}

		// Remove the menu item from the Export menu
		constexpr auto cid = PTSLC_CPP::CommandId::CId_UninstallMenuHandler;
		PTSLC_CPP::CppPTSLRequest request{ cid, R"({
			"menu_item_id": ")" + menuItemID + R"("
			})"
		};

		// This cleanup task must complete, so wait for it to finish synchronously
		PTSLC_CPP::CppPTSLResponse response = (threadData->mClientPtr)->SendRequest(request).get();
		AAX_PTSLClient_Helper::ParseResponse(response); // just for logging

		// The menu item has been removed, so another instance can now add its own menu item with the same name
		gData.RemoveNextMenuItemTagForObjectAt(threadData->mClientPtr.get());

		return std::nullopt; // The request has already been processed so do not pass it to the caller
	}
}

void DemoGain_PTSLClient::StartPTSLClient()
{
	if (!mPTSLClient)
	{
		mPTSLClient = std::make_unique<AAX_PTSLClient<>>("Avid", "DemoGain PTSL Example");
		mPTSLClient->ConnectPTSLClient();
		mPTSLClient->AddTask(
			DemoGain_PTSL_Client::DoPollEvents,
			DemoGain_PTSL_Client::OnPTSLCallback);

		// This example adds a menu item in the Export menu
		mPTSLClient->AddTask(DemoGain_PTSL_Client::DoAddHostMenuItem);
	}
}

void DemoGain_PTSLClient::StopPTSLClient() noexcept
{
	try
	{
		mPTSLClient->AddTask(DemoGain_PTSL_Client::DoRemoveHostMenuItem);
		mPTSLClient.reset();
	}
	catch (...)
	{
		// nothing to do here
	}
}

