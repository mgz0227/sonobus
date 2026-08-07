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
 *	\file AAX_MockController.h
 *
 *	\brief Mock controller implementation for unit testing
 *
 */ 
/*================================================================================================*/


#ifndef AAX_MOCKCONTROLLER_H
#define AAX_MOCKCONTROLLER_H

#include "AAX_IController.h"
#include <cstdint>

/** \brief A mock controller implementation for unit testing
 *
 *  Specific unit tests may override this class to provide test-specific behavior
 */
class AAX_MockController : public AAX_IController
{
public:
    AAX_MockController() = default;
    ~AAX_MockController() override = default;
    
    AAX_Result GetEffectID(AAX_IString* outEffectID) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetSampleRate(AAX_CSampleRate* outSampleRate) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetInputStemFormat(AAX_EStemFormat* outStemFormat) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetOutputStemFormat(AAX_EStemFormat* outStemFormat) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetSignalLatency(int32_t* outSamples) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetCycleCount(AAX_EProperty inWhichCycleCount, AAX_CPropertyValue* outNumCycles) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetTODLocation(AAX_CTimeOfDay* outTODLocation) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result SetSignalLatency(int32_t inNumSamples) override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result SetCycleCount(AAX_EProperty* inWhichCycleCounts, AAX_CPropertyValue* iValues, int32_t numValues) override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result PostPacket(AAX_CFieldIndex inFieldIndex, const void* inPayloadP, uint32_t inPayloadSize) override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result SendNotification(AAX_CTypeID inNotificationType, const void* inNotificationData, uint32_t inNotificationDataSize) override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result SendNotification(AAX_CTypeID inNotificationType) override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetCurrentMeterValue(AAX_CTypeID inMeterID, float* outMeterValue) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetMeterPeakValue(AAX_CTypeID inMeterID, float* outMeterPeakValue) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result ClearMeterPeakValue(AAX_CTypeID inMeterID) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetMeterCount(uint32_t* outMeterCount) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetMeterClipped(AAX_CTypeID inMeterID, AAX_CBoolean* outClipped) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result ClearMeterClipped(AAX_CTypeID inMeterID) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetNextMIDIPacket(AAX_CFieldIndex* outPort, AAX_CMidiPacket* outPacket) override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetHybridSignalLatency(int32_t* outSamples) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetCurrentAutomationTimestamp(AAX_CTransportCounter* outTimestamp) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetHostName(AAX_IString* outHostNameString) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetPlugInTargetPlatform(AAX_CTargetPlatform* outTargetPlatform) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetIsAudioSuite(AAX_CBoolean* outIsAudioSuite) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetInstanceGroupID(AAX_CInstanceGroupID* outInstanceGroupID) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result GetInstanceID(AAX_CInstanceID* outInstanceID) const override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_Result RegisterForNotification(AAX_CTypeID inNotificationType, IACFUnknown const* inSubscriberObject) override { return AAX_ERROR_UNIMPLEMENTED; }
    AAX_IPageTable* CreateTableCopyForEffect(AAX_CPropertyValue inManufacturerID, AAX_CPropertyValue inProductID, AAX_CPropertyValue inPlugInID, uint32_t inTableType, int32_t inTablePageSize) const override { return nullptr; }
    AAX_IPageTable* CreateTableCopyForLayout(const char* inEffectID, const char* inLayoutName, uint32_t inTableType, int32_t inTablePageSize) const override { return nullptr; }
    AAX_IPageTable* CreateTableCopyForEffectFromFile(const char* inPageTableFilePath, AAX_ETextEncoding inFilePathEncoding, AAX_CPropertyValue inManufacturerID, AAX_CPropertyValue inProductID, AAX_CPropertyValue inPlugInID, uint32_t inTableType, int32_t inTablePageSize) const override { return nullptr; }
    AAX_IPageTable* CreateTableCopyForLayoutFromFile(const char* inPageTableFilePath, AAX_ETextEncoding inFilePathEncoding, const char* inLayoutName, uint32_t inTableType, int32_t inTablePageSize) const override { return nullptr; }
};





#endif // AAX_MOCKCONTROLLER_H
