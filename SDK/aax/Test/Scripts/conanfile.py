# Copyright 2025 Avid Technology, Inc.
# 
# This file is part of the Avid AAX SDK.
# 
# The AAX SDK is subject to commercial or open-source licensing.
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


from conan import ConanFile
from conan.tools.cmake import CMakeDeps, CMakeToolchain


class AAXSDKUnitTestConan(ConanFile):
    name = "AAXSDKUnitTest"
    settings = "os", "arch", "compiler", "build_type"

    def requirements(self):
        self.requires("gtest/1.14.0")

    def configure(self):
        # Configure gtest options
        self.options["gtest"].shared = False

    def generate(self):
        tc = CMakeToolchain(self)
        tc.user_presets_path = False
        tc.generate()

        cmake = CMakeDeps(self)
        cmake.generate()
