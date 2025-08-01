#
# Copyright 2025 Avid Technology, Inc.
#
# This file is part of the Avid AAX SDK.
# 
# The AAX SDK is subject to commercial or open-source
# licensing.
# 
# By using the AAX SDK, you agree to the terms of both the Avid AAX SDK License
# Agreement and Avid Privacy Policy.
# 
# AAX SDK License: https://developer.avid.com/aax
# Privacy Policy: https://www.avid.com/legal/privacy-policy-statement
# 
# Or: You may also use this code under the terms of the GPL v3 (see
# www.gnu.org/licenses).
# 
# THE AAX SDK IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
# EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
# DISCLAIMED.
#

# Function: aax_plugin
# 
# Define a target representing an AAX plugin project.
#
# Arguments:
#   TARGET_NAME       - The name of the generated plugin target.
#   OUTPUT_NAME       - The output filename of the .aaxplugin bundle.
#   COPYRIGHT_STRING  - The copyright string for the plugin.
#   BUNDLE_IDENTIFIER - The bundle identifier for the plugin.
#   VERSION           - (Optional) The version of the plugin.
#   RESOURCE_FILES    - (Optional) A list of resource files to be copied.
#   FACTORY_PRESETS   - (Optional) A directory containing factory presets to be copied.
#   ICON_FILE         - (Optional) A path to a Windows icon file that will be copied.
#
# Example usage:
#   aax_plugin(
#     TARGET_NAME MyPluginProject
#     OUTPUT_NAME MyPlugin
#     VERSION 1.0.0
#     COPYRIGHT_STRING "© 2023 My Company"
#     BUNDLE_IDENTIFIER com.mycompany.myplugin
#   )
#
# Example usage with some project macro substitution:
#   aax_plugin(TARGET_NAME "${PROJECT_NAME}_Project"
#     OUTPUT_NAME ${PROJECT_NAME}
#     VERSION ${PROJECT_VERSION}
#     COPYRIGHT_STRING "Copyright 2024-${CURRENT_YEAR} My Company"
#     BUNDLE_IDENTIFIER "com.mycompany.aax.${PROJECT_NAME}")
#   )
function(aax_plugin)
	set(options)
	set(oneValueArgs
		TARGET_NAME
		OUTPUT_NAME
		COPYRIGHT_STRING
		BUNDLE_IDENTIFIER
		VERSION
		FACTORY_PRESETS
		ICON_FILE)
	set(multiValueArgs
		RESOURCE_FILES)
	cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	if(NOT ARG_TARGET_NAME)
		message(FATAL_ERROR "Missing argument TARGET_NAME")
	endif()

	if(NOT ARG_OUTPUT_NAME)
		message(FATAL_ERROR "Missing argument OUTPUT_NAME")
	endif()

	if(NOT ARG_COPYRIGHT_STRING)
		message(FATAL_ERROR "Missing argument COPYRIGHT_STRING")
	endif()

	if(NOT ARG_BUNDLE_IDENTIFIER)
		message(FATAL_ERROR "Missing argument BUNDLE_IDENTIFIER")
	endif()

	# Set up the basic target
	_aax_plugin_base(${ARG_TARGET_NAME} ${ARG_OUTPUT_NAME})

	# Configure the plugin as a bundle
	_aax_plugin_setup_bundle(TARGET_NAME ${ARG_TARGET_NAME}
		OUTPUT_NAME ${ARG_OUTPUT_NAME}
		VERSION ${ARG_VERSION}
		COPYRIGHT_STRING ${ARG_COPYRIGHT_STRING}
		BUNDLE_IDENTIFIER ${ARG_BUNDLE_IDENTIFIER})

	# Copy additional bundle contents
	if(ARG_RESOURCE_FILES)
		aax_plugin_resources(TARGET_NAME ${ARG_TARGET_NAME}
			RESOURCE_FILES ${ARG_RESOURCE_FILES})
	endif()
	if(ARG_FACTORY_PRESETS)
		aax_plugin_factory_presets(TARGET_NAME ${ARG_TARGET_NAME}
			FACTORY_PRESETS ${ARG_FACTORY_PRESETS})
	endif()
	if(ARG_ICON_FILE)
		aax_plugin_icon(TARGET_NAME ${ARG_TARGET_NAME}
			ICON_FILE ${ARG_ICON_FILE})
	endif()

	# Add the AAX Library as a dependency
	target_link_libraries(${ARG_TARGET_NAME} PRIVATE
		AAX_SDK::AAX_Export
		AAX_SDK::AAXLibrary)

	# Support a custom output directory if -DAAX_PLUGIN_OUTPUT_DIRECTORY is set
	if(AAX_SDK_PLUGIN_OUTPUT_DIRECTORY)
		if(WIN32)
			# TODO: Detect target architecture: x64, etc.
			set(AAX_SDK_PLUGIN_BUNDLE_BINARY_SUBPATH "$<TARGET_PROPERTY:OUTPUT_NAME>$<TARGET_PROPERTY:SUFFIX>/Contents/x64")
			set_target_properties(${ARG_TARGET_NAME} PROPERTIES
				LIBRARY_OUTPUT_DIRECTORY "${AAX_SDK_PLUGIN_OUTPUT_DIRECTORY}/${AAX_SDK_PLUGIN_BUNDLE_BINARY_SUBPATH}"
				RUNTIME_OUTPUT_DIRECTORY "${AAX_SDK_PLUGIN_OUTPUT_DIRECTORY}/${AAX_SDK_PLUGIN_BUNDLE_BINARY_SUBPATH}"
			)
		else()
			set_target_properties(${ARG_TARGET_NAME} PROPERTIES
				LIBRARY_OUTPUT_DIRECTORY ${AAX_SDK_PLUGIN_OUTPUT_DIRECTORY}
				RUNTIME_OUTPUT_DIRECTORY ${AAX_SDK_PLUGIN_OUTPUT_DIRECTORY}
			)
		endif()
	endif()

	# Define common compiler options
	target_compile_options(${ARG_TARGET_NAME} PRIVATE $<$<C_COMPILER_ID:GNU,Clang,AppleClang>:-Wno-multichar>)

endfunction()

# Function: aax_plugin_resources
# 
# Copy resources to an AAX plugin bundle.
#
# The resource files should be provided as paths relative to the source directory.
#
# Parameters:
#   TARGET_NAME    - The name of the generated plugin target.
#   RESOURCE_FILES - The list of resource files to be copied
#
# Example usage:
#   aax_plugin_resources(MyPlugin
#     MyPlugin.xml
#   )
function(aax_plugin_resources)
	set(options)
	set(oneValueArgs TARGET_NAME)
	set(multiValueArgs RESOURCE_FILES)
	cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	if(NOT ARG_TARGET_NAME)
		message(FATAL_ERROR "Missing argument TARGET_NAME")
	endif()

	_get_aax_plugin_target_output_path(${ARG_TARGET_NAME} TARGET_OUTPUT_PATH)

	foreach(RESOURCE ${ARG_RESOURCE_FILES})
		set(FROM_RESOURCE_FILE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${RESOURCE}")
		get_filename_component(RESOURCE_FILE_NAME ${FROM_RESOURCE_FILE_PATH} NAME)
		set(TO_RESOURCE_FILE_PATH "${TARGET_OUTPUT_PATH}/Contents/Resources/${RESOURCE_FILE_NAME}")
		message(STATUS "aax_plugin_resources: POST_BUILD command will copy ${FROM_RESOURCE_FILE_PATH} to ${TO_RESOURCE_FILE_PATH}")
		add_custom_command(TARGET ${ARG_TARGET_NAME} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy "${FROM_RESOURCE_FILE_PATH}" "${TO_RESOURCE_FILE_PATH}"
		)
	endforeach()

endfunction()

# Function: aax_plugin_factory_presets
# 
# Copy factory presets to an AAX plugin bundle. The entire contents of the specified
# directory will be copied, but not the directory itself.
#
# Parameters:
#   TARGET_NAME     - The name of the generated plugin target.
#   FACTORY_PRESETS - The directory containing factory presets to be copied
#
# Example usage:
#   aax_plugin_factory_presets(MyPlugin
#     "Factory Presets/MyPlugin"
#   )
function(aax_plugin_factory_presets)
	set(options)
	set(oneValueArgs TARGET_NAME FACTORY_PRESETS)
	set(multiValueArgs)
	cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	if(NOT ARG_TARGET_NAME)
		message(FATAL_ERROR "Missing argument TARGET_NAME")
	endif()

	_get_aax_plugin_target_output_path(${ARG_TARGET_NAME} TARGET_OUTPUT_PATH)

	message(STATUS "aax_plugin_factory_presets: POST_BUILD command will copy all contents of ${CMAKE_CURRENT_SOURCE_DIR}/${ARG_FACTORY_PRESETS} to ${TARGET_OUTPUT_PATH}/Contents/Factory Presets")

	add_custom_command(TARGET ${ARG_TARGET_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_CURRENT_SOURCE_DIR}/${ARG_FACTORY_PRESETS} "${TARGET_OUTPUT_PATH}/Contents/Factory Presets"
	)

endfunction()

# Function: aax_plugin_icon
# 
# Copy an icon file to the target's output directory.
#
# Implemented for Windows only (but always safe to call).
# 
# Arguments:
#   TARGET_NAME  - The name of the target for which the icon file will be copied.
#   ICON_FILE    - The path to the icon file that will be copied.
# 
# Example Usage:
#   aax_plugin_icon(
#     TARGET_NAME MyPlugin
#     ICON_FILE path/to/icon.ico
#   )
function(aax_plugin_icon)
	set(options)
	set(oneValueArgs TARGET_NAME ICON_FILE)
	set(multiValueArgs)
	cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	if(NOT ARG_TARGET_NAME)
		message(FATAL_ERROR "Missing argument TARGET_NAME")
	endif()

	if(NOT ARG_ICON_FILE)
		message(FATAL_ERROR "Missing argument ICON_FILE")
	endif()
	
	if(WIN32)
		_get_aax_plugin_target_output_path(${ARG_TARGET_NAME} TARGET_OUTPUT_PATH)

		message(STATUS "aax_plugin_icon: POST_BUILD command will copy ${CMAKE_CURRENT_SOURCE_DIR}/${ARG_ICON_FILE} to ${TARGET_OUTPUT_PATH}/Contents/PlugIn.ico")

		set(FROM_RESOURCE_FILE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${ARG_ICON_FILE}")
		get_filename_component(RESOURCE_FILE_NAME ${FROM_RESOURCE_FILE_PATH} NAME)
		set(TO_RESOURCE_FILE_PATH "${TARGET_OUTPUT_PATH}/Contents/${RESOURCE_FILE_NAME}")
		message(STATUS "aax_plugin_icon: POST_BUILD command will copy ${FROM_RESOURCE_FILE_PATH} to ${TO_RESOURCE_FILE_PATH}")
		add_custom_command(TARGET ${ARG_TARGET_NAME} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy "${FROM_RESOURCE_FILE_PATH}" "${TO_RESOURCE_FILE_PATH}"
		)
		
	endif()

endfunction()

##########################################################################################
##########################################################################################
##########################################################################################


# Function: get_aax_plugin_target_output_path
# 
# Provide the output path for an AAX plugin target.
# 
# Parameters:
# - TARGET_NAME: The name of the target for which the output path is being generated.
# 
# Returns:
# - OUTPUT_PATH_VAR The constructed output path for the AAX plugin target.
#
# Example usage:
# get_target_output_path(MyPlugInTarget MY_OUTPUT_PATH)
# message(STATUS "The target output path is: ${MY_OUTPUT_PATH}")
function(_get_aax_plugin_target_output_path TARGET_NAME OUTPUT_PATH_VAR)
	if(APPLE)
		set(TARGET_OUTPUT_PATH $<TARGET_BUNDLE_DIR:${ARG_TARGET_NAME}>)
		set(${OUTPUT_PATH_VAR} "${TARGET_OUTPUT_PATH}" PARENT_SCOPE)
	else()
		set(TARGET_OUTPUT_BINARY_DIR $<TARGET_FILE_DIR:${ARG_TARGET_NAME}>)
		set(${OUTPUT_PATH_VAR} "${TARGET_OUTPUT_BINARY_DIR}/../" PARENT_SCOPE)
	endif()
	
endfunction()

# Function: _aax_plugin_base
# 
# This helper function creates a new library target for an AAX plugin with the
# specified project name and output name.
#
# You should not use this function directly. Instead, use aax_plugin() to define an
# AAX plugin target.
function(_aax_plugin_base TARGET_NAME OUTPUT_NAME)
	add_library(${TARGET_NAME} MODULE)

	# The output file will be OUTPUT_NAME.aaxplugin
	set_target_properties(${TARGET_NAME} PROPERTIES
		OUTPUT_NAME "${OUTPUT_NAME}"
		PREFIX ""
		SUFFIX ".aaxplugin"
	)

	set(CXX_STANDARD 17)
	set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD ${CXX_STANDARD})
	set_property(TARGET ${TARGET_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

	# GNU compiler options
	target_compile_options(${TARGET_NAME}
		PRIVATE $<$<C_COMPILER_ID:GNU>: -fextended-identifiers>
	)

	# Define preprocessor macro based on the platform:
	# LINUX_VERSION, MAC_VERSION, WIN_VERSION, or UNKNOWN_VERSION
	#
	# TODO: Remove this; sources that require it can include AAX_EnvironmentUtilities.h
	target_compile_definitions(${TARGET_NAME}
		PUBLIC
		$<IF:$<PLATFORM_ID:Linux>,LINUX,
		$<IF:$<PLATFORM_ID:Darwin>,MAC,
		$<IF:$<PLATFORM_ID:Windows>,WIN,
			UNKNOWN>>>_VERSION=1
		PLATFORM_ID=$<PLATFORM_ID>
	)

endfunction()

# function: _aax_plugin_setup_bundle
#
# Set up bundle structure for an AAX plugin.
#
# This function configures various properties for the specified target to
# ensure it is a valid AAX plugin bundle.
#
# Arguments:
#   TARGET_NAME       - The name of the target to configure.
#   OUTPUT_NAME       - The output filename of the .aaxplugin bundle.
#   COPYRIGHT_STRING  - The copyright string for the target.
#   BUNDLE_IDENTIFIER - The bundle identifier for the target.
#   VERSION           - (Optional) The version of the target.
#   SHORT_VERSION     - (Optional) The short version string of the target.
function(_aax_plugin_setup_bundle)
	set(options)
	set(oneValueArgs TARGET_NAME OUTPUT_NAME COPYRIGHT_STRING BUNDLE_IDENTIFIER VERSION SHORT_VERSION)
	set(multiValueArgs)
	cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	if(NOT ARG_TARGET_NAME)
		message(FATAL_ERROR "Missing argument TARGET_NAME")
	endif()

	if(NOT ARG_OUTPUT_NAME)
		message(FATAL_ERROR "Missing argument OUTPUT_NAME")
	endif()

	if(NOT ARG_COPYRIGHT_STRING)
		message(FATAL_ERROR "Missing argument COPYRIGHT_STRING")
	endif()

	if(NOT ARG_BUNDLE_IDENTIFIER)
		message(FATAL_ERROR "Missing argument BUNDLE_IDENTIFIER")
	endif()

	if(NOT ARG_VERSION)
		set(ARG_VERSION "1.0.0")
	endif()

	if(NOT ARG_SHORT_VERSION)
		set(ARG_SHORT_VERSION ${ARG_VERSION})
	endif()

	if(APPLE)
		set_target_properties(${ARG_TARGET_NAME} PROPERTIES
			BUNDLE TRUE
			XCODE_ATTRIBUTE_OTHER_CODE_SIGN_FLAGS "-o linker-signed" #https://gitlab.kitware.com/cmake/cmake/-/issues/21854
			XCODE_EMBED_FRAMEWORKS_CODE_SIGN_ON_COPY 1
			XCODE_ATTRIBUTE_GENERATE_PKGINFO_FILE YES
			XCODE_SKIP_INSTALL NO
			MACOSX_BUNDLE TRUE
			MACOSX_BUNDLE_BUNDLE_NAME "${ARG_OUTPUT_NAME}"
			MACOSX_BUNDLE_GUI_IDENTIFIER ${ARG_BUNDLE_IDENTIFIER}
			MACOSX_BUNDLE_INFO_STRING "${ARG_SHORT_VERSION}, ${ARG_COPYRIGHT_STRING}"
			MACOSX_BUNDLE_BUNDLE_VERSION "${ARG_VERSION}"
			MACOSX_BUNDLE_SHORT_VERSION_STRING "${ARG_SHORT_VERSION}"
			# Custom Info.plist.in is used to modify: CFBundlePackageType, CFBundleSignature
			MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Info.plist.in"
			BUNDLE_EXTENSION aaxplugin
		)

		_get_aax_plugin_target_output_path(${ARG_TARGET_NAME} TARGET_OUTPUT_PATH)

		# Create the PkgInfo file and copy it to the bundle
		add_custom_command(TARGET ${ARG_TARGET_NAME} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E echo "TDMwPTul" > ${TARGET_OUTPUT_PATH}/Contents/PkgInfo
		)
	endif()

endfunction()


