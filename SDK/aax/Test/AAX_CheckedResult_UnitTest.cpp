//  Copyright 2025 Avid Technology, Inc.
//
// This file is part of the Avid AAX SDK.
//
// The AAX SDK is subject to commercial or open-source
// licensing.
//
// By using the AAX SDK, you agree to the terms of both the Avid AAX SDK License
// Agreement and Avid Privacy Policy.
//
// AAX SDK License: https://developer.avid.com/aax
// Privacy Policy: https://www.avid.com/legal/privacy-policy-statement
//
// Or: You may also use this code under the terms of the GPL v3 (see
// www.gnu.org/licenses).
//
// THE AAX SDK IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
// EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
// DISCLAIMED.

#include "gmock/gmock.h"

#include "AAX_Exception.h"
#include "AAX_Errors.h"

#include <stdexcept>


TEST(AAX_CheckedResult, DefaultConstructor) {
	AAX_CheckedResult cr;
	EXPECT_EQ(cr, AAX_SUCCESS);
	EXPECT_EQ(cr.LastError(), AAX_SUCCESS);
}

TEST(AAX_CheckedResult, ImplicitConversionFromResult) {
	AAX_Result result = AAX_SUCCESS;
	AAX_CheckedResult cr = result;
	EXPECT_EQ(cr, result);
	EXPECT_EQ(cr.LastError(), result);
}

TEST(AAX_CheckedResult, ImplicitConversionToResult) {
	AAX_CheckedResult cr;
	AAX_Result result = cr;
	EXPECT_EQ(result, AAX_SUCCESS);
}

TEST(AAX_CheckedResult, ConstructorWithSuccess) {
	AAX_CheckedResult cr(AAX_SUCCESS);
	EXPECT_EQ(cr, AAX_SUCCESS);
	EXPECT_EQ(cr.LastError(), AAX_SUCCESS);
}

TEST(AAX_CheckedResult, ConstructorWithErrorThrowsException) {
	try {
		AAX_CheckedResult cr(AAX_ERROR_NULL_OBJECT);
		FAIL() << "Expected AAX_CheckedResult::Exception";
	} catch (const AAX_CheckedResult::Exception& e) {
		EXPECT_EQ(e.Result(), AAX_ERROR_NULL_OBJECT);
	} catch (...) {
		FAIL() << "Expected AAX_CheckedResult::Exception";
	}
}

TEST(AAX_CheckedResult, AssignmentOperatorThrowsException) {
	AAX_CheckedResult cr;
	try {
		cr = AAX_ERROR_NULL_OBJECT;
		FAIL() << "Expected AAX_CheckedResult::Exception";
	} catch (const AAX_CheckedResult::Exception& e) {
		EXPECT_EQ(e.Result(), AAX_ERROR_NULL_OBJECT);
	} catch (...) {
		FAIL() << "Expected AAX_CheckedResult::Exception";
	}
	EXPECT_EQ(cr, AAX_SUCCESS);
	EXPECT_EQ(cr.LastError(), AAX_ERROR_NULL_OBJECT);
}

TEST(AAX_CheckedResult, BitwiseOrOperatoreThrowsException) {
	AAX_CheckedResult cr;
	try {
		cr |= AAX_ERROR_NULL_OBJECT;
		FAIL() << "Expected AAX_CheckedResult::Exception";
	} catch (const AAX_CheckedResult::Exception& e) {
		EXPECT_EQ(e.Result(), AAX_ERROR_NULL_OBJECT);
	} catch (...) {
		FAIL() << "Expected AAX_CheckedResult::Exception";
	}
	EXPECT_EQ(cr, AAX_SUCCESS);
	EXPECT_EQ(cr.LastError(), AAX_ERROR_NULL_OBJECT);
}

TEST(AAX_CheckedResult, AcceptedResultDoesNotThrowException) {
	AAX_CheckedResult cr;
	cr.AddAcceptedResult(AAX_ERROR_NULL_OBJECT);
	cr = AAX_ERROR_NULL_OBJECT; // no throw
	EXPECT_EQ(cr, AAX_ERROR_NULL_OBJECT);
	EXPECT_EQ(cr.LastError(), AAX_SUCCESS);

	cr.AddAcceptedResult(AAX_ERROR_NOT_INITIALIZED);
	cr = AAX_ERROR_NOT_INITIALIZED; // no throw
	EXPECT_EQ(cr, AAX_ERROR_NOT_INITIALIZED);
	EXPECT_EQ(cr.LastError(), AAX_SUCCESS);
	cr = AAX_SUCCESS; // no throw
	EXPECT_EQ(cr, AAX_SUCCESS);
	EXPECT_EQ(cr.LastError(), AAX_SUCCESS);
	cr = AAX_ERROR_NULL_OBJECT; // no throw
	EXPECT_EQ(cr, AAX_ERROR_NULL_OBJECT);
	EXPECT_EQ(cr.LastError(), AAX_SUCCESS);

	cr.ResetAcceptedResults();
	EXPECT_EQ(cr, AAX_ERROR_NULL_OBJECT);
	EXPECT_EQ(cr.LastError(), AAX_SUCCESS);
	
	try {
		cr = AAX_ERROR_NULL_OBJECT;
		FAIL() << "Expected AAX_CheckedResult::Exception";
	} catch (const AAX_CheckedResult::Exception& e) {
		EXPECT_EQ(e.Result(), AAX_ERROR_NULL_OBJECT);
	} catch (...) {
		FAIL() << "Expected AAX_CheckedResult::Exception";
	}

	EXPECT_EQ(cr, AAX_SUCCESS);
	EXPECT_EQ(cr.LastError(), AAX_ERROR_NULL_OBJECT);
}

TEST(AAX_CheckedResult, DefaultStateAfterClearFromSuccess) {
	AAX_CheckedResult cr;
	cr.Clear();
	EXPECT_EQ(cr, AAX_SUCCESS);
	EXPECT_EQ(cr.LastError(), AAX_SUCCESS);
}

TEST(AAX_CheckedResult, DefaultStateAfterClearFromError) {
	AAX_CheckedResult cr;
	try {
		cr = AAX_ERROR_NULL_OBJECT;
		FAIL() << "Expected AAX_CheckedResult::Exception";
	} catch (const AAX_CheckedResult::Exception& e) {
		EXPECT_EQ(e.Result(), AAX_ERROR_NULL_OBJECT);
	} catch (...) {
		FAIL() << "Expected AAX_CheckedResult::Exception";
	}
	cr.Clear();
	EXPECT_EQ(cr, AAX_SUCCESS);
	EXPECT_EQ(cr.LastError(), AAX_SUCCESS);
}

TEST(AAX_CheckedResult, SwallowMacroDoesNotThrowException) {
	AAX_CheckedResult cr;
	AAX_SWALLOW(cr = AAX_ERROR_NULL_OBJECT);
	EXPECT_EQ(cr, AAX_SUCCESS);
	EXPECT_EQ(cr.LastError(), AAX_ERROR_NULL_OBJECT);
}

TEST(AAX_CheckedResult, CaptureMacroDoesNotThrowException) {
	auto nonThrowingFunc = []() -> void { return; };
	auto throwingFunc = []() -> void { throw AAX::Exception::ResultError(AAX_ERROR_NULL_OBJECT); };
	auto failingFunc = []() -> AAX_Result { return AAX_ERROR_NULL_OBJECT; };
	
	AAX_Result result = AAX_SUCCESS;
	AAX_CAPTURE(result, nonThrowingFunc());
	EXPECT_EQ(result, AAX_SUCCESS);

	AAX_CAPTURE(result, throwingFunc());
	EXPECT_EQ(result, AAX_ERROR_NULL_OBJECT);

	AAX_CheckedResult cr;
	AAX_CAPTURE(result, cr = failingFunc());
	EXPECT_EQ(result, AAX_ERROR_NULL_OBJECT);
	EXPECT_EQ(cr, AAX_SUCCESS);
	EXPECT_EQ(cr.LastError(), AAX_ERROR_NULL_OBJECT);
}

TEST(AAX_CheckedResult, CaptureMacroDoesNotCatchOtherExceptions) {
	AAX_Result result = AAX_SUCCESS;
	try {
		AAX_CAPTURE(result, throw std::runtime_error("test exception"));
		FAIL() << "Expected std::runtime_error";
	} catch (std::runtime_error&) {
		// success
	} catch (...) {
		FAIL() << "Expected std::runtime_error";
	}

	try {
		AAX_CAPTURE(result, throw 1);
		FAIL() << "Expected integer exception";
	} catch (int&) {
		// success
	} catch (...) {
		FAIL() << "Expected integer exception";
	}
}
