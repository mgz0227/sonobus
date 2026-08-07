#!/usr/bin/env python3

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


"""
Test verification script for build_unit_tests.py

This script verifies that our unit test build script works correctly by checking
preconditions, running the script, and validating postconditions. It tests:

1. Command-line option functionality:
   - --help: Help text display
   - --no-setup-env: Skip environment setup, use existing virtual environment
   - --list-targets: List available build targets
   - --config Debug/Release: Build configuration selection
   - --no-build, --no-run: Skip build/run phases
   - Specific target building by name

2. Conan package manager integration:
   - Cache isolation to prevent affecting user's global Conan cache
   - Conan 2.x compatibility and proper environment variable handling
   - Dependency installation and build environment setup

3. Virtual environment management:
   - Environment creation and validation
   - Tool availability (CMake, Conan) in virtual environment
   - Proper cleanup functionality

4. Build system integration:
   - Interface library implementation for test mocks
   - CMake configuration and target building
   - AAX SDK integration and compilation
"""

import argparse
import os
import sys
import subprocess
from pathlib import Path


# Timeout constants (in seconds)
TIMEOUT_HELP = 60              # Help command
TIMEOUT_ENV_SETUP = 600        # Environment setup
TIMEOUT_CONAN_ISOLATION = 600  # Conan isolation test
TIMEOUT_CONAN_ENV_VARS = 600   # Environment variables test
TIMEOUT_CLEAN = 120            # Clean operation
TIMEOUT_FULL_BUILD = 1200      # Full build with package download
TIMEOUT_INTERFACE_BUILD = 600  # Interface library build test
TIMEOUT_NO_SETUP_ENV = 300     # --no-setup-env test
TIMEOUT_LIST_TARGETS = 300     # --list-targets test
TIMEOUT_SPECIFIC_TARGET = 600  # Specific target build test
TIMEOUT_CONFIG_TEST = 600      # Configuration option test


class TestVerifier:
    def __init__(self, test_config=None):
        self.test_dir = Path(__file__).parent.parent
        self.script_path = self.test_dir / "Scripts" / "build_unit_tests.py"
        self.failures = []
        self.test_config = test_config or {}  # Dictionary controlling which tests to run
        
    def log(self, message):
        print(f"[VERIFY] {message}")
        
    def error(self, message):
        self.failures.append(message)
        print(f"[ERROR] {message}")
        
    def run_subprocess_with_timeout(self, cmd, operation_name, timeout_seconds, **kwargs):
        """Run subprocess with enhanced timeout error reporting."""
        try:
            self.log(f"Running {operation_name} (timeout: {timeout_seconds}s)")
            result = subprocess.run(cmd, timeout=timeout_seconds, **kwargs)
            return result
        except subprocess.TimeoutExpired as e:
            timeout_msg = (f"{operation_name} timed out after {timeout_seconds} seconds. "
                          f"Command: {' '.join(str(arg) for arg in cmd)}")
            self.error(timeout_msg)
            raise subprocess.TimeoutExpired(cmd, timeout_seconds) from e
        
    def check_preconditions(self):
        """Check that all required files exist before running the script."""
        self.log("Checking preconditions...")
        
        required_files = [
            self.script_path,
            self.test_dir / "Scripts" / "_subprocess_wrapper.py",
            self.test_dir / "Scripts" / "_get_python_command.py",
            self.test_dir / "Scripts" / "requirements.txt",
            self.test_dir / "Scripts" / "conanfile.py",
            self.test_dir / "CMakeLists.txt",
            self.test_dir / "Scripts" / "excluded_tests.json"
        ]
        
        for file_path in required_files:
            if not file_path.exists():
                self.error(f"Required file missing: {file_path}")
            else:
                self.log(f"[OK] Found: {file_path}")
                
        # Check that Python version is adequate
        if sys.version_info < (3, 6):
            self.error(f"Python version {sys.version} is too old. Need 3.6+")
        else:
            self.log(f"[OK] Python version {sys.version} is adequate")
            
        return len(self.failures) == 0
        
    def run_help_test(self):
        """Test that the help functionality works."""
        self.log("Testing help functionality...")
        
        try:
            result = self.run_subprocess_with_timeout([
                sys.executable, str(self.script_path), "--help"
            ], "Help command", TIMEOUT_HELP, capture_output=True, text=True)
            
            if result.returncode != 0:
                self.error(f"Help command failed with exit code {result.returncode}")
                return False
                
            if "AAX SDK Unit Test Builder" not in result.stdout:
                self.error("Help output doesn't contain expected title")
                return False
                
            self.log("[OK] Help functionality works")
            return True
            
        except subprocess.TimeoutExpired:
            return False  # Error already logged by helper method
        except Exception as e:
            self.error(f"Help test failed with exception: {e}")
            return False
            
    def run_env_setup_test(self):
        """Test that environment setup works without building."""
        self.log("Testing environment setup...")
        
        try:
            result = self.run_subprocess_with_timeout([
                sys.executable, str(self.script_path), 
                "--no-build", "--no-run"
            ], "Environment setup", TIMEOUT_ENV_SETUP, capture_output=True, text=True)
            
            # Print output for debugging
            if result.stdout:
                print("STDOUT:", result.stdout[-1000:])  # Last 1000 chars
            if result.stderr:
                print("STDERR:", result.stderr[-1000:])
                
            venv_dir = self.test_dir / ".venv"
            build_dir = self.test_dir / ".build"
            
            if not venv_dir.exists():
                self.error("Virtual environment was not created")
                return False
                
            # Check that venv has the right structure
            if sys.platform == "win32":
                python_exe = venv_dir / "Scripts" / "python.exe"
            else:
                python_exe = venv_dir / "bin" / "python"
                
            if not python_exe.exists():
                self.error(f"Python executable not found in venv: {python_exe}")
                return False
                
            self.log("[OK] Virtual environment created successfully")
            
            # Check that build directory was created
            if not build_dir.exists():
                self.error("Build directory was not created")
                return False
                
            self.log("[OK] Build directory created")
            
            # Check that conanfile.py was copied to build directory
            build_conanfile = build_dir / "conanfile.py"
            if build_conanfile.exists():
                self.log("[OK] conanfile.py copied to build directory")
            else:
                self.log("Note: conanfile.py not copied (--no-build was used)")
            
            # Check that the isolated Conan cache was set up
            conan_cache_dir = build_dir / ".conan"
            if conan_cache_dir.exists():
                self.log("[OK] Isolated Conan cache directory created")
            else:
                self.log("Note: Conan cache directory created but may have been cleaned up")
            
            return True
            
        except subprocess.TimeoutExpired:
            return False  # Error already logged by helper method
        except Exception as e:
            self.error(f"Environment setup test failed: {e}")
            return False
            
    def run_conan_isolation_test(self):
        """Test that Conan cache isolation is working properly."""
        self.log("Testing Conan cache isolation...")
        
        # Get paths to potential global Conan caches
        home_dir = Path.home()
        global_conan_paths = [
            home_dir / ".conan",
            home_dir / ".conan2", 
            Path("/Users") / os.getenv("USER", "unknown") / "git.ws" / ".conan"
        ]
        
        # Record initial state of global caches - get directory modification times
        initial_cache_states = {}
        for cache_path in global_conan_paths:
            if cache_path.exists():
                try:
                    # Get modification time of the cache directory itself
                    initial_cache_states[cache_path] = cache_path.stat().st_mtime
                    self.log(f"Recording initial state of {cache_path}")
                except OSError:
                    pass
        
        try:
            # Run the script with environment setup only (no build to avoid long test)
            result = self.run_subprocess_with_timeout([
                sys.executable, str(self.script_path),
                "--no-build", "--no-run"
            ], "Conan cache isolation test", TIMEOUT_CONAN_ISOLATION, capture_output=True, text=True)
            
            # Check if the script succeeded
            if result.returncode != 0:
                self.error(f"Script failed during isolation test: {result.stderr}")
                return False
            
            # Verify isolated cache was created
            isolated_cache = self.test_dir / ".build" / ".conan"
            if not isolated_cache.exists():
                self.error("Isolated Conan cache directory was not created")
                return False
            
            self.log(f"[OK] Isolated Conan cache created at {isolated_cache}")
            
            # Check that global caches were not modified
            cache_isolation_verified = True
            for cache_path, initial_mtime in initial_cache_states.items():
                if cache_path.exists():
                    try:
                        current_mtime = cache_path.stat().st_mtime
                        if current_mtime > initial_mtime:
                            self.error(f"Global Conan cache was modified: {cache_path}")
                            cache_isolation_verified = False
                        else:
                            self.log(f"[OK] Global cache unchanged: {cache_path}")
                    except OSError:
                        pass
            
            # Check the script output for isolation messages
            if "Isolating conan cache: CONAN_HOME=" not in result.stdout:
                self.error("Script output doesn't show Conan cache isolation message")
                cache_isolation_verified = False
            else:
                self.log("[OK] Script output shows cache isolation is active")
            
            # Verify the isolated cache contains expected structure
            expected_dirs = ["profiles", "p"]  # "p" is the package cache directory
            for dir_name in expected_dirs:
                expected_dir = isolated_cache / dir_name
                if expected_dir.exists():
                    self.log(f"[OK] Found expected cache directory: {dir_name}")
                else:
                    self.log(f"Note: Cache directory {dir_name} not found (may be created on first use)")
            
            return cache_isolation_verified
            
        except subprocess.TimeoutExpired:
            return False  # Error already logged by helper method
        except Exception as e:
            self.error(f"Interface library test failed: {e}")
            return False

    def run_no_setup_env_test(self):
        """Test the --no-setup-env functionality.
        
        This test verifies that:
        1. --no-setup-env uses existing virtual environment without creating a new one
        2. --no-setup-env skips dependency installation (conan install)
        3. --no-setup-env fails gracefully when virtual environment doesn't exist
        4. --no-setup-env can be combined with build operations using existing environment
        5. Error messages are clear and helpful when virtual environment is missing
        """
        self.log("Testing --no-setup-env functionality...")
        
        try:
            # First ensure we have a virtual environment by running full setup
            self.log("Setting up virtual environment for --no-setup-env test...")
            setup_result = self.run_subprocess_with_timeout([
                sys.executable, str(self.script_path), 
                "--no-build", "--no-run"
            ], "Setup for --no-setup-env test", TIMEOUT_ENV_SETUP, capture_output=True, text=True)
            
            if setup_result.returncode != 0:
                self.error(f"Failed to set up environment for --no-setup-env test: {setup_result.stderr}")
                return False
            
            venv_dir = self.test_dir / ".venv"
            if not venv_dir.exists():
                self.error("Virtual environment was not created for --no-setup-env test")
                return False
            
            self.log("[OK] Virtual environment prepared for --no-setup-env test")
            
            # Test --no-setup-env with valid virtual environment
            result = self.run_subprocess_with_timeout([
                sys.executable, str(self.script_path),
                "--no-setup-env", "--no-build", "--no-run"
            ], "--no-setup-env with existing venv", TIMEOUT_NO_SETUP_ENV, capture_output=True, text=True)
            
            if result.returncode != 0:
                self.error(f"--no-setup-env failed with existing venv: {result.stderr}")
                return False
            
            # Check that it uses existing virtual environment
            if "Using existing virtual environment:" not in result.stdout:
                self.error("--no-setup-env doesn't show 'Using existing virtual environment' message")
                return False
            
            # Check that dependency installation was skipped
            if "Installing conan dependencies" in result.stdout or "Running conan install" in result.stdout:
                self.error("--no-setup-env still runs dependency installation")
                return False
            
            self.log("[OK] --no-setup-env uses existing environment and skips dependency installation")
            
            # Test --no-setup-env without virtual environment
            venv_backup_dir = self.test_dir / ".venv_backup_for_test"
            if venv_backup_dir.exists():
                import shutil
                shutil.rmtree(venv_backup_dir)
            
            # Move venv temporarily
            venv_dir.rename(venv_backup_dir)
            
            try:
                no_venv_result = self.run_subprocess_with_timeout([
                    sys.executable, str(self.script_path),
                    "--no-setup-env", "--no-build", "--no-run"
                ], "--no-setup-env without venv", TIMEOUT_NO_SETUP_ENV, capture_output=True, text=True)
                
                # Should fail with proper error message
                if no_venv_result.returncode == 0:
                    self.error("--no-setup-env should fail when virtual environment doesn't exist")
                    return False
                
                if "Virtual environment not found" not in no_venv_result.stdout:
                    self.error("--no-setup-env doesn't show proper error when venv missing")
                    return False
                
                self.log("[OK] --no-setup-env properly fails when virtual environment is missing")
                
            finally:
                # Restore venv
                if venv_backup_dir.exists():
                    venv_backup_dir.rename(venv_dir)
            
            # Test --no-setup-env with build (should use existing venv without dependency installation)
            build_result = self.run_subprocess_with_timeout([
                sys.executable, str(self.script_path),
                "--no-setup-env", "--no-run", "AAX_DataBuffer_UnitTest"
            ], "--no-setup-env with build", TIMEOUT_NO_SETUP_ENV, capture_output=True, text=True)
            
            if build_result.returncode != 0:
                self.error(f"--no-setup-env with build failed: {build_result.stderr}")
                return False
            
            # Check that dependency installation was still skipped but build proceeded
            if "Installing conan dependencies" in build_result.stdout:
                self.error("--no-setup-env with build still runs dependency installation")
                return False
            
            if "Using existing virtual environment:" not in build_result.stdout:
                self.error("--no-setup-env with build doesn't show existing venv usage")
                return False
            
            # Should show build activity
            if "Building unit tests" not in build_result.stdout:
                self.error("--no-setup-env with build doesn't proceed to build phase")
                return False
            
            self.log("[OK] --no-setup-env with build uses existing environment and skips dependencies")
            
            return True
            
        except subprocess.TimeoutExpired:
            return False  # Error already logged by helper method
        except Exception as e:
            self.error(f"--no-setup-env test failed: {e}")
            return False

    def run_list_targets_test(self):
        """Test the --list-targets functionality."""
        self.log("Testing --list-targets functionality...")
        
        try:
            # First ensure we have environment set up
            setup_result = self.run_subprocess_with_timeout([
                sys.executable, str(self.script_path), 
                "--no-build", "--no-run"
            ], "Setup for --list-targets test", TIMEOUT_ENV_SETUP, capture_output=True, text=True)
            
            if setup_result.returncode != 0:
                self.log("Warning: Failed to set up environment for --list-targets test, but continuing")
            
            # Test --list-targets
            result = self.run_subprocess_with_timeout([
                sys.executable, str(self.script_path), "--list-targets"
            ], "--list-targets command", TIMEOUT_LIST_TARGETS, capture_output=True, text=True)
            
            if result.returncode != 0:
                self.error(f"--list-targets failed: {result.stderr}")
                return False
            
            # Check that it shows available targets
            if "AVAILABLE TARGETS" not in result.stdout:
                self.error("--list-targets doesn't show 'AVAILABLE TARGETS' header")
                return False
            
            if "Unit Test Targets" not in result.stdout:
                self.error("--list-targets doesn't show 'Unit Test Targets' section")
                return False
            
            # Check for some expected unit test targets
            expected_tests = ["AAX_DataBuffer_UnitTest", "AAX_CPacketDispatcher_UnitTest"]
            found_tests = 0
            for test in expected_tests:
                if test in result.stdout:
                    found_tests += 1
            
            if found_tests == 0:
                self.error("--list-targets doesn't show any expected unit test targets")
                return False
            
            self.log(f"[OK] --list-targets shows {found_tests} expected unit test targets")
            
            # Check for usage examples
            if "Usage examples:" not in result.stdout:
                self.error("--list-targets doesn't show usage examples")
                return False
            
            self.log("[OK] --list-targets functionality works correctly")
            return True
            
        except subprocess.TimeoutExpired:
            return False  # Error already logged by helper method
        except Exception as e:
            self.error(f"--list-targets test failed: {e}")
            return False

    def run_specific_target_test(self):
        """Test building specific targets."""
        self.log("Testing specific target building...")
        
        try:
            # Test building a specific unit test target
            result = self.run_subprocess_with_timeout([
                sys.executable, str(self.script_path),
                "--no-run", "AAX_DataBuffer_UnitTest"
            ], "Specific target build test", TIMEOUT_SPECIFIC_TARGET, capture_output=True, text=True)
            
            if result.returncode != 0:
                self.error(f"Specific target build failed: {result.stderr}")
                return False
            
            # Check that it mentions the specific target
            if "Will build/run only specified targets: AAX_DataBuffer_UnitTest" not in result.stdout:
                self.error("Specific target build doesn't show target selection message")
                return False
            
            # Check that build succeeded
            if "Building 1 specified targets: AAX_DataBuffer_UnitTest" not in result.stdout:
                self.error("Specific target build doesn't show target building message")
                return False
            
            self.log("[OK] Specific target building works correctly")
            
            # Test building multiple specific targets
            multi_result = self.run_subprocess_with_timeout([
                sys.executable, str(self.script_path),
                "--no-run", "AAX_DataBuffer_UnitTest", "AAX_CPacketDispatcher_UnitTest"
            ], "Multiple specific targets build test", TIMEOUT_SPECIFIC_TARGET, capture_output=True, text=True)
            
            if multi_result.returncode != 0:
                self.error(f"Multiple target build failed: {multi_result.stderr}")
                return False
            
            # Check that it mentions both targets
            if "Will build/run only specified targets: AAX_DataBuffer_UnitTest, AAX_CPacketDispatcher_UnitTest" not in multi_result.stdout:
                self.error("Multiple target build doesn't show both targets in selection message")
                return False
            
            self.log("[OK] Multiple specific target building works correctly")
            return True
            
        except subprocess.TimeoutExpired:
            return False  # Error already logged by helper method
        except Exception as e:
            self.error(f"Specific target test failed: {e}")
            return False

    def run_config_option_test(self):
        """Test the --config option with Debug/Release configurations."""
        self.log("Testing --config option...")
        
        try:
            # Test Debug configuration (default)
            debug_result = self.run_subprocess_with_timeout([
                sys.executable, str(self.script_path),
                "--config", "Debug", "--no-build", "--no-run"
            ], "Debug configuration test", TIMEOUT_CONFIG_TEST, capture_output=True, text=True)
            
            if debug_result.returncode != 0:
                self.error(f"Debug configuration test failed: {debug_result.stderr}")
                return False
            
            if "Configuration: Debug" not in debug_result.stdout:
                self.error("Debug configuration not shown in summary")
                return False
            
            self.log("[OK] Debug configuration works correctly")
            
            # Test Release configuration
            release_result = self.run_subprocess_with_timeout([
                sys.executable, str(self.script_path),
                "--config", "Release", "--no-build", "--no-run"
            ], "Release configuration test", TIMEOUT_CONFIG_TEST, capture_output=True, text=True)
            
            if release_result.returncode != 0:
                self.error(f"Release configuration test failed: {release_result.stderr}")
                return False
            
            if "Configuration: Release" not in release_result.stdout:
                self.error("Release configuration not shown in summary")
                return False
            
            self.log("[OK] Release configuration works correctly")
            
            # Test invalid configuration
            invalid_result = self.run_subprocess_with_timeout([
                sys.executable, str(self.script_path),
                "--config", "InvalidConfig", "--no-build", "--no-run"
            ], "Invalid configuration test", TIMEOUT_CONFIG_TEST, capture_output=True, text=True)
            
            if invalid_result.returncode == 0:
                self.error("Invalid configuration should fail but didn't")
                return False
            
            # Should show argument parser error
            if "invalid choice" not in invalid_result.stderr and "InvalidConfig" not in invalid_result.stderr:
                self.error("Invalid configuration doesn't show proper error message")
                return False
            
            self.log("[OK] Invalid configuration properly rejected")
            return True
            
        except subprocess.TimeoutExpired:
            return False  # Error already logged by helper method
        except Exception as e:
            self.error(f"Config option test failed: {e}")
            return False

    def run_option_combinations_test(self):
        """Test various combinations of command-line options."""
        self.log("Testing option combinations...")
        
        try:
            # Test --no-build --no-run combination (setup only)
            setup_only_result = self.run_subprocess_with_timeout([
                sys.executable, str(self.script_path),
                "--no-build", "--no-run"
            ], "Setup-only test (--no-build --no-run)", TIMEOUT_ENV_SETUP, capture_output=True, text=True)
            
            if setup_only_result.returncode != 0:
                self.error(f"Setup-only test failed: {setup_only_result.stderr}")
                return False
            
            # Should show virtual environment setup (one of two possible messages depending on state)
            # - "Creating virtual environment:" if creating new venv
            # - "Virtual environment already exists:" if venv exists (setup_env=True is default)
            venv_messages = [
                "Creating virtual environment:",
                "Virtual environment already exists:"
            ]
            if not any(msg in setup_only_result.stdout for msg in venv_messages):
                self.error("Setup-only test doesn't show virtual environment setup")
                return False
            
            if "Building unit tests" in setup_only_result.stdout:
                self.error("Setup-only test should not build but shows building")
                return False
            
            if "Running unit tests" in setup_only_result.stdout:
                self.error("Setup-only test should not run tests but shows running")
                return False
            
            self.log("[OK] --no-build --no-run combination works (setup only)")
            
            # Test --no-run combination (build but don't run)
            build_only_result = self.run_subprocess_with_timeout([
                sys.executable, str(self.script_path),
                "--no-run", "AAX_DataBuffer_UnitTest"
            ], "Build-only test (--no-run)", TIMEOUT_SPECIFIC_TARGET, capture_output=True, text=True)
            
            if build_only_result.returncode != 0:
                self.error(f"Build-only test failed: {build_only_result.stderr}")
                return False
            
            # Should show building but not running
            if "Building unit tests" not in build_only_result.stdout:
                self.error("Build-only test should build but doesn't show building")
                return False
            
            if "Running unit tests" in build_only_result.stdout:
                self.error("Build-only test should not run tests but shows running")
                return False
            
            self.log("[OK] --no-run combination works (build only)")
            
            return True
            
        except subprocess.TimeoutExpired:
            return False  # Error already logged by helper method
        except Exception as e:
            self.error(f"Option combinations test failed: {e}")
            return False
    
    def run_conan_version_test(self):
        """Test that the script uses Conan 2 and not Conan 1."""
        self.log("Testing Conan version requirements...")
        
        try:
            # Check requirements.txt for Conan version constraint
            requirements_file = self.test_dir / "Scripts" / "requirements.txt"
            if not requirements_file.exists():
                self.error("requirements.txt not found")
                return False
            
            with open(requirements_file, 'r') as f:
                requirements_content = f.read()
            
            # Check for Conan 2.x requirement
            if "conan>=2.0.0" in requirements_content or "conan<3.0.0" in requirements_content:
                self.log("[OK] requirements.txt specifies Conan 2.x")
            else:
                self.error("requirements.txt doesn't specify Conan 2.x requirement")
                return False
            
            # Check conanfile.py for Conan 2 syntax
            conanfile = self.test_dir / "Scripts" / "conanfile.py"
            if not conanfile.exists():
                self.error("conanfile.py not found")
                return False
            
            with open(conanfile, 'r') as f:
                conanfile_content = f.read()
            
            # Check for Conan 2 import syntax
            if "from conan import ConanFile" in conanfile_content:
                self.log("[OK] conanfile.py uses Conan 2 import syntax")
            elif "from conans import ConanFile" in conanfile_content:
                self.error("conanfile.py still uses old Conan 1 import syntax")
                return False
            else:
                self.error("conanfile.py doesn't contain expected ConanFile import")
                return False
            
            return True
            
        except Exception as e:
            self.error(f"Conan version test failed: {e}")
            return False

    def run_conan_environment_variables_test(self):
        """Test that Conan environment variables are properly set and cleared."""
        self.log("Testing Conan environment variable handling...")
        
        # Set a potentially interfering Conan environment variable
        original_conan_user_home = os.environ.get("CONAN_USER_HOME")
        test_conan_var = "CONAN_TEST_VAR"
        os.environ[test_conan_var] = "should_be_cleared"
        
        try:
            # Run a quick environment setup
            result = self.run_subprocess_with_timeout([
                sys.executable, str(self.script_path),
                "--no-build", "--no-run"
            ], "Conan environment variables test", TIMEOUT_CONAN_ENV_VARS, capture_output=True, text=True)
            
            if result.returncode != 0:
                self.error(f"Script failed during environment variable test: {result.stderr}")
                return False
            
            # Check that the script reports clearing environment variables
            if "Cleared environment variable:" in result.stdout:
                self.log("[OK] Script reports clearing Conan environment variables")
            else:
                self.log("Note: No Conan environment variables were cleared (this is OK)")
            
            # Check that CONAN_HOME isolation message is present
            if "CONAN_HOME=" not in result.stdout:
                self.error("Script doesn't show CONAN_HOME setting for isolation")
                return False
            
            self.log("[OK] CONAN_HOME environment variable is being set for isolation")
            return True
            
        except subprocess.TimeoutExpired:
            return False  # Error already logged by helper method
        except Exception as e:
            self.error(f"Environment variable test failed: {e}")
            return False
        finally:
            # Clean up test environment variable
            if test_conan_var in os.environ:
                del os.environ[test_conan_var]
            
            # Restore original CONAN_USER_HOME if it existed
            if original_conan_user_home is not None:
                os.environ["CONAN_USER_HOME"] = original_conan_user_home
            elif "CONAN_USER_HOME" in os.environ:
                del os.environ["CONAN_USER_HOME"]

    def run_clean_test(self):
        """Test that clean functionality works."""
        self.log("Testing clean functionality...")
        
        venv_dir = self.test_dir / ".venv"
        build_dir = self.test_dir / ".build"
        pycache_dir = self.test_dir / "Scripts" / "__pycache__"
        
        # First run a script operation to ensure __pycache__ gets created
        try:
            setup_result = self.run_subprocess_with_timeout([
                sys.executable, str(self.script_path), "--no-build", "--no-run"
            ], "Setup for clean test", TIMEOUT_ENV_SETUP, capture_output=True, text=True)
        except subprocess.TimeoutExpired:
            self.log("Setup for clean test timed out, but continuing with clean test")
        
        # Check that directories existed before clean
        dirs_existed_before = venv_dir.exists() or build_dir.exists() or pycache_dir.exists()
        
        if pycache_dir.exists():
            self.log(f"[OK] Found __pycache__ directory to test cleanup: {pycache_dir}")
        else:
            self.log("Note: __pycache__ directory not found - may not have been created")
        
        try:
            result = self.run_subprocess_with_timeout([
                sys.executable, str(self.script_path), "--clean"
            ], "Clean operation", TIMEOUT_CLEAN, capture_output=True, text=True)
            
            if result.returncode != 0:
                self.error(f"Clean command failed with exit code {result.returncode}")
                return False
                
            if venv_dir.exists():
                self.error("Virtual environment directory still exists after clean")
                return False
                
            if build_dir.exists():
                self.error("Build directory still exists after clean")
                return False
                
            if pycache_dir.exists():
                self.error("Python cache directory still exists after clean")
                return False
                
            if dirs_existed_before:
                self.log("[OK] Clean functionality removed all directories including __pycache__")
            else:
                self.log("[OK] Clean functionality ran (no directories to remove)")
            return True
            
        except subprocess.TimeoutExpired:
            return False  # Error already logged by helper method
        except Exception as e:
            self.error(f"Clean test failed: {e}")
            return False

    def run_full_build_isolation_test(self):
        """Test cache isolation during a full build that downloads Conan packages."""
        self.log("Testing cache isolation during full build with package download...")
        
        # Get paths to potential global Conan caches
        home_dir = Path.home()
        global_conan_paths = [
            home_dir / ".conan",
            home_dir / ".conan2", 
            Path("/Users") / os.getenv("USER", "unknown") / "git.ws" / ".conan"
        ]
        
        # Record initial state of global caches - get file lists
        initial_file_sets = {}
        for cache_path in global_conan_paths:
            if cache_path.exists():
                try:
                    # Get set of all file paths in the cache directory
                    file_set = set(str(f.relative_to(cache_path)) for f in cache_path.rglob("*") if f.is_file())
                    initial_file_sets[cache_path] = file_set
                    self.log(f"Recording {len(file_set)} files in {cache_path}")
                except OSError:
                    pass
        
        try:
            # First clean to ensure fresh start
            try:
                clean_result = self.run_subprocess_with_timeout([
                    sys.executable, str(self.script_path), "--clean"
                ], "Clean for full build test", TIMEOUT_CLEAN, capture_output=True, text=True)
            except subprocess.TimeoutExpired:
                self.log("Clean operation timed out, but continuing with full build test")
            
            # Run the script with build that will download gtest via Conan
            result = self.run_subprocess_with_timeout([
                sys.executable, str(self.script_path),
                "--no-run", "AAX_CPacketDispatcher_UnitTest"  # Build one specific test
            ], "Full build with package download", TIMEOUT_FULL_BUILD, capture_output=True, text=True)
            
            # Check if the script succeeded
            if result.returncode != 0:
                self.error(f"Full build failed during isolation test: {result.stderr}")
                return False
            
            # Verify isolated cache contains downloaded packages
            isolated_cache = self.test_dir / ".build" / ".conan"
            if not isolated_cache.exists():
                self.error("Isolated Conan cache directory was not created")
                return False
            
            # Check that gtest was downloaded to isolated cache
            gtest_found = False
            try:
                for item in isolated_cache.rglob("*gtest*"):
                    if item.is_file() or item.is_dir():
                        gtest_found = True
                        break
            except OSError:
                pass
            
            if not gtest_found:
                self.error("gtest package not found in isolated cache - download may have failed")
                return False
            
            self.log("[OK] gtest package found in isolated cache")
            
            # Check that global caches gained no new files
            cache_isolation_verified = True
            for cache_path, initial_files in initial_file_sets.items():
                if cache_path.exists():
                    try:
                        current_files = set(str(f.relative_to(cache_path)) for f in cache_path.rglob("*") if f.is_file())
                        new_files = current_files - initial_files
                        if new_files:
                            self.error(f"Global Conan cache gained {len(new_files)} new files: {cache_path}")
                            # Log first few new files for debugging
                            for new_file in sorted(new_files)[:5]:
                                self.log(f"  New file: {new_file}")
                            if len(new_files) > 5:
                                self.log(f"  ... and {len(new_files) - 5} more")
                            cache_isolation_verified = False
                        else:
                            self.log(f"[OK] Global cache unchanged during full build: {cache_path}")
                    except OSError:
                        pass
            
            return cache_isolation_verified
            
        except subprocess.TimeoutExpired:
            return False  # Error already logged by helper method
        except Exception as e:
            self.error(f"Full build isolation test failed: {e}")
            return False
            
    def run_interface_library_test(self):
        """Test that the Test/CMakeLists.txt contains the AAX_SDK_Tests_Mocks interface library."""
        self.log("Testing interface library implementation...")
        
        try:
            # Check that the Test/CMakeLists.txt contains the interface library
            original_cmake = self.test_dir / "CMakeLists.txt"
            if not original_cmake.exists():
                self.error("Test/CMakeLists.txt not found")
                return False
            
            with open(original_cmake, 'r') as f:
                cmake_content = f.read()
            
            # Check for interface library definition
            if "add_library(AAX_SDK_Tests_Mocks INTERFACE)" not in cmake_content:
                self.error("Test/CMakeLists.txt doesn't contain AAX_SDK_Tests_Mocks interface library")
                return False
            
            self.log("[OK] Found AAX_SDK_Tests_Mocks interface library definition")
            
            # Check for interface library usage in avid_add_gtest_unittest function
            if "AAX_SDK_Tests_Mocks" not in cmake_content:
                self.error("Test/CMakeLists.txt doesn't reference AAX_SDK_Tests_Mocks interface library")
                return False
            
            self.log("[OK] Interface library is referenced in the CMakeLists.txt")
            
            # Run the script to verify it can build successfully with the interface library
            result = self.run_subprocess_with_timeout([
                sys.executable, str(self.script_path), 
                "--no-run", "AAX_CPacketDispatcher_UnitTest"
            ], "Interface library build test", TIMEOUT_INTERFACE_BUILD, capture_output=True, text=True)
            
            if result.returncode != 0:
                self.error(f"Build failed with interface library: {result.stderr}")
                return False
            
            self.log("[OK] Build succeeds with interface library implementation")
            return True
            
        except subprocess.TimeoutExpired:
            return False  # Error already logged by helper method
        except Exception as e:
            self.error(f"Interface library test failed: {e}")
            return False
            
    def check_postconditions(self):
        """Check expected side effects after running the script."""
        self.log("Checking postconditions...")
        
        # After our tests, the directories should not exist (due to clean)
        venv_dir = self.test_dir / ".venv"
        build_dir = self.test_dir / ".build"
        pycache_dir = self.test_dir / "Scripts" / "__pycache__"
        
        if venv_dir.exists():
            self.log(f"Note: Virtual environment still exists at {venv_dir}")
        else:
            self.log("[OK] No virtual environment directory after clean")
            
        if build_dir.exists():
            self.log(f"Note: Build directory still exists at {build_dir}")
        else:
            self.log("[OK] No build directory after clean")
            
        if pycache_dir.exists():
            self.log(f"Note: Python cache directory still exists at {pycache_dir}")
        else:
            self.log("[OK] No Python cache directory after clean")
            
        # Verify that the Test/CMakeLists.txt contains the expected interface library
        original_cmake = self.test_dir / "CMakeLists.txt"
        if original_cmake.exists():
            with open(original_cmake, 'r') as f:
                original_content = f.read()
            if "AAX_SDK_Tests_Mocks" in original_content:
                self.log("[OK] Test/CMakeLists.txt contains expected interface library")
            else:
                self.error("Test/CMakeLists.txt doesn't contain expected AAX_SDK_Tests_Mocks interface library")
                return False
        
        return True
        
    def run_all_tests(self):
        """Run all verification tests."""
        self.log("Starting verification of build_unit_tests.py")
        self.log("=" * 60)
        
        if not self.check_preconditions():
            self.error("Preconditions not met, aborting tests")
            return False
            
        # Test individual functions - each with a key for enabling/disabling
        tests = [
            ("help", "Help Test", self.run_help_test),
            ("conan_version", "Conan Version Test", self.run_conan_version_test),
            ("env_setup", "Environment Setup Test", self.run_env_setup_test),
            ("conan_isolation", "Conan Cache Isolation Test", self.run_conan_isolation_test),
            ("conan_env_vars", "Conan Environment Variables Test", self.run_conan_environment_variables_test),
            ("no_setup_env", "No Setup Environment Test", self.run_no_setup_env_test),
            ("list_targets", "List Targets Test", self.run_list_targets_test),
            ("specific_target", "Specific Target Test", self.run_specific_target_test),
            ("config_option", "Config Option Test", self.run_config_option_test),
            ("option_combinations", "Option Combinations Test", self.run_option_combinations_test),
            ("full_build_isolation", "Full Build Cache Isolation Test", self.run_full_build_isolation_test),
            ("interface_library", "Interface Library Test", self.run_interface_library_test),
            ("clean", "Clean Test", self.run_clean_test),
        ]
        
        for test_key, test_name, test_func in tests:
            # Check if this test is enabled in the configuration
            if not self.test_config.get(test_key, True):
                self.log(f"\n[SKIP] {test_name} (disabled via command-line option)")
                continue
                
            self.log(f"\nRunning {test_name}...")
            try:
                success = test_func()
                if success:
                    self.log(f"[PASS] {test_name} PASSED")
                else:
                    self.error(f"[FAIL] {test_name} FAILED")
            except Exception as e:
                self.error(f"[ERROR] {test_name} ERROR: {e}")
                
        self.check_postconditions()
        
        # Print summary
        self.log("\n" + "=" * 60)
        self.log("VERIFICATION SUMMARY")
        self.log("=" * 60)
        
        if self.failures:
            self.log(f"[FAIL] VERIFICATION FAILED - {len(self.failures)} issues found:")
            for failure in self.failures:
                self.log(f"  - {failure}")
            return False
        else:
            self.log("[PASS] ALL VERIFICATION TESTS PASSED")
            self.log("The build_unit_tests.py script appears to be working correctly!")
            return True


def parse_arguments():
    """Parse command-line arguments for test selection."""
    parser = argparse.ArgumentParser(
        description='Verification script for build_unit_tests.py',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog='''
Test Selection:
  Each test can be enabled or disabled using --test-<name> or --no-test-<name> options.
  All tests are enabled by default when no options are specified.
  
  If any --test-<name> option is specified, only those tests will run (equivalent to
  --no-test-all). Use --test-all explicitly to override this behavior.

Available Tests:
  help                  - Help functionality test
  conan-version         - Conan version requirements test
  env-setup             - Environment setup test
  conan-isolation       - Conan cache isolation test
  conan-env-vars        - Conan environment variables test
  no-setup-env          - --no-setup-env functionality test
  list-targets          - --list-targets functionality test
  specific-target       - Specific target building test
  config-option         - --config option test
  option-combinations   - Option combinations test
  full-build-isolation  - Full build cache isolation test
  interface-library     - Interface library implementation test
  clean                 - Clean functionality test

Examples:
  # Run all tests (default)
  python3 test_verification.py
  
  # Run only the help and environment setup tests
  python3 test_verification.py --test-help --test-env-setup
  
  # Run all tests except the full build test (faster)
  python3 test_verification.py --no-test-full-build-isolation
  
  # Run only quick tests (skip slow build/isolation tests)
  python3 test_verification.py --no-test-full-build-isolation --no-test-interface-library
'''
    )
    
    # Add individual test control options
    # Define test names with underscores (for internal use) and dashes (for CLI)
    test_names_internal = [
        'help', 'conan_version', 'env_setup', 'conan_isolation', 'conan_env_vars',
        'no_setup_env', 'list_targets', 'specific_target', 'config_option',
        'option_combinations', 'full_build_isolation', 'interface_library', 'clean'
    ]
    test_names_cli = [
        'help', 'conan-version', 'env-setup', 'conan-isolation', 'conan-env-vars',
        'no-setup-env', 'list-targets', 'specific-target', 'config-option',
        'option-combinations', 'full-build-isolation', 'interface-library', 'clean'
    ]
    
    # Add --test-all and --no-test-all for convenience
    parser.add_argument('--test-all', action='store_true', dest='enable_all',
                       help='Enable all tests (default behavior)')
    parser.add_argument('--no-test-all', action='store_false', dest='enable_all',
                       help='Disable all tests (use with specific --test-<name> to run only selected tests)')
    parser.set_defaults(enable_all=True)
    
    # Add individual test enable/disable options
    for internal_name, cli_name in zip(test_names_internal, test_names_cli):
        parser.add_argument(f'--test-{cli_name}', action='store_true', dest=f'test_{internal_name}',
                           help=f'Enable {cli_name.replace("-", " ")} test')
        parser.add_argument(f'--no-test-{cli_name}', action='store_false', dest=f'test_{internal_name}',
                           help=f'Disable {cli_name.replace("-", " ")} test')
        parser.set_defaults(**{f'test_{internal_name}': None})  # None means use default (enable_all)
    
    args = parser.parse_args()
    
    # Check if any --test-<name> options were explicitly enabled
    any_test_enabled = any(getattr(args, f'test_{name}') is True for name in test_names_internal)
    
    # If any --test-<name> was specified, automatically disable --test-all
    effective_enable_all = args.enable_all
    if any_test_enabled:
        effective_enable_all = False
    
    # Build test configuration dictionary
    test_config = {}
    for test_name in test_names_internal:
        arg_value = getattr(args, f'test_{test_name}')
        if arg_value is None:
            # Not explicitly set, use effective enable_all default
            test_config[test_name] = effective_enable_all
        else:
            # Explicitly set via --test-<name> or --no-test-<name>
            test_config[test_name] = arg_value
    
    return test_config


if __name__ == "__main__":
    test_config = parse_arguments()
    verifier = TestVerifier(test_config)
    success = verifier.run_all_tests()
    sys.exit(0 if success else 1)
