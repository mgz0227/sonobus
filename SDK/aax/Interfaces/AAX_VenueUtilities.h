/*================================================================================================*/
/*

	AAX_VenueUtilities.h

	Copyright 2025 Avid Technology, Inc.
	All rights reserved.
	
	This file is part of the Avid AAX SDK.
	
	The AAX SDK is subject to commercial or open-source licensing.
	
	By using the AAX SDK, you agree to the terms of both the Avid AAX SDK License
	Agreement and Avid Privacy Policy.
	
	AAX SDK License: https://developer.avid.com/aax
	Privacy Policy: https://www.avid.com/legal/privacy-policy-statement
	
	Or: You may also use this code under the terms of the GPL v3 (see
	www.gnu.org/licenses).
	
	THE AAX SDK IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
	EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
	DISCLAIMED.
*/

/**
 *	\file AAX_VenueUtilities.h
 *
 *	\brief Useful environment definitions for Venue systems
 *
 */
/*================================================================================================*/

/// @cond ignore
#ifndef AAX_VENUEUTILITIES_H
#define AAX_VENUEUTILITIES_H
/// @endcond

#include "AAX_EnvironmentUtilities.h"

#if WINDOWS_VERSION
#include <string.h>
#include <cstdlib>
#endif

namespace AAX
{
    inline bool IsVenueSystem(void)
    {
#if WINDOWS_VERSION
    	static const char * const environmentVariableName = "JEX_HOST_TYPE";
    	static const char * const venueEnvironment = "venue";
    	static const char * const environment = std::getenv ( environmentVariableName );
    	static const bool isVenue = ( NULL != environment) && (0 == strcmp ( environment, venueEnvironment ) );
    	return isVenue;
#else
        return false;
#endif
    }
}


/// @cond ignore
#endif // AAX_VENUEUTILITIES_H
/// @endcond
