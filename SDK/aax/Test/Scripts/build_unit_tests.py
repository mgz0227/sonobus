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


# NOTE: This script requires python3

import argparse
import json
import os
import platform
import shutil
import subprocess
import sys
import venv
from dataclasses import dataclass
from pathlib import Path
import glob

from _get_python_command import get_python_command
from _subprocess_wrapper import _wrap_subprocess

# ANSI color codes
COLOR_LIGHT_BLUE = '\033[94m'
COLOR_RESET = '\033[0m'

# Build configuration constants
DEFAULT_CONFIG = 'Debug'
VALID_CONFIGS = ['Debug', 'Release']

# Platform-specific constants
WINDOWS_SCRIPTS_DIR = 'Scripts'
UNIX_BIN_DIR = 'bin'
WINDOWS_EXECUTABLE_EXT = '.exe'

# AAX SDK build options
AAX_SDK_BUILD_OPTIONS = [
    '-DAAX_BUILD_EXAMPLES=OFF',
    '-DAAX_BUILD_PTSL_EXAMPLES=OFF', 
    '-DAAX_BUILD_JUCE_GUI_EXTENSION=OFF',
    '-DAAX_BUILD_LIBRARY=ON',  # We need this for the tests
    '-DAAX_BUILD_TI_INTERFACE=ON'  # Keep basic interfaces
]

# File and directory names
VENV_DIR_NAME = '.venv'
BUILD_DIR_NAME = '.build'
CONAN_CACHE_DIR_NAME = '.conan'
REQUIREMENTS_FILE = 'requirements.txt'
CONANFILE_NAME = 'conanfile.py'
EXCLUDED_TESTS_CONFIG = 'excluded_tests.json'

# CMake targets and files
CONAN_TOOLCHAIN_FILE = 'conan_toolchain.cmake'
AAX_LIBRARY_TARGET = 'AAXLibrary'
TEST_TARGET_SUFFIX = '_UnitTest'

def log_command(cmd, cwd=None):
    """Log a command before execution with light blue coloring."""
    if isinstance(cmd, list):
        cmd_str = ' '.join(str(arg) for arg in cmd)
    else:
        cmd_str = str(cmd)
    
    print("Executing command:")
    print(f"{COLOR_LIGHT_BLUE}{cmd_str}{COLOR_RESET}")
    if cwd:
        print(f"Working directory: {cwd}")


def execute_command(cmd, cwd=None, description=None, env=None):
    """Execute a command with logging and error handling."""
    if description:
        print(f"\n{description}")
    log_command(cmd, cwd)
    _wrap_subprocess(cmd, cwd=cwd, env=env)


def get_platform_executable_path(base_path, executable_name):
    """Get platform-specific path to an executable in a virtual environment."""
    if platform.system() == "Windows":
        return str(base_path / WINDOWS_SCRIPTS_DIR / f"{executable_name}{WINDOWS_EXECUTABLE_EXT}")
    else:
        return str(base_path / UNIX_BIN_DIR / executable_name)


def get_cmake_command(venv_path):
    """Get the CMake command path for the virtual environment."""
    return get_platform_executable_path(venv_path, "cmake")


def get_test_executable_patterns(build_dir, config, include_utils=False):
    """Get platform-specific patterns for finding test executables and utility executables.
    
    Args:
        build_dir: Build directory path
        config: Build configuration (Debug/Release)
        include_utils: If True, include utils/ directory in search
    """
    if platform.system() == "Windows":
        # On Windows, executables are consistently in .build\tests\<config>\ and .build\utils\<config>\
        patterns = [
            str(build_dir / "tests" / config / f"*{TEST_TARGET_SUFFIX}{WINDOWS_EXECUTABLE_EXT}"),
            str(build_dir / "tests" / config / f"*{WINDOWS_EXECUTABLE_EXT}")
        ]
        if include_utils:
            patterns.append(str(build_dir / "utils" / config / f"*{WINDOWS_EXECUTABLE_EXT}"))
        return patterns
    else:
        patterns = [
            str(build_dir / "tests" / "*")
        ]
        if include_utils:
            patterns.append(str(build_dir / "utils" / "*"))
        patterns.append(str(build_dir / "*"))  # Fallback
        return patterns


def discover_test_executables(build_dir, config, specific_targets=None):
    """Discover all test and utility executables in the build directory.
    
    Args:
        build_dir: Build directory path
        config: Build configuration (Debug/Release)
        specific_targets: If provided, also search utils/ directory for these targets
    """
    # Only search utils/ if specific targets are requested
    include_utils = specific_targets is not None and len(specific_targets) > 0
    test_patterns = get_test_executable_patterns(build_dir, config, include_utils)
    test_executables = []
    
    search_desc = "test and utility executables" if include_utils else "test executables"
    print(f"Searching for {search_desc} with patterns:")
    for pattern in test_patterns:
        print(f"  {pattern}")
        found = glob.glob(pattern, recursive=True)
        print(f"    Found {len(found)} files: {found}")
        test_executables.extend(found)
    
    # On Windows, also check if executables exist but aren't executable due to permission issues
    if platform.system() == "Windows":
        # Filter to only actual files (executable check is less reliable on Windows)
        test_executables = [
            exe for exe in test_executables 
            if os.path.isfile(exe) and exe.lower().endswith('.exe')
        ]
    else:
        # Filter to only actual executable files
        test_executables = [
            exe for exe in test_executables 
            if os.path.isfile(exe) and os.access(exe, os.X_OK)
        ]
    
    print(f"Final executable list ({len(test_executables)} files):")
    for exe in test_executables:
        print(f"  {exe}")
    
    return test_executables


def filter_executables_by_targets(test_executables, target_names):
    """Filter test executables to match specific target names."""
    target_set = set(target_names)
    return [
        exe for exe in test_executables 
        if (os.path.basename(exe) in target_set or 
            os.path.splitext(os.path.basename(exe))[0] in target_set)
    ]


def run_single_test(test_exe, build_dir):
    """Run a single test executable and return the result."""
    test_name = os.path.basename(test_exe)
    print(f"\n{'='*60}")
    print(f"Running test: {test_name}")
    print(f"{'='*60}")
    
    try:
        # Run the test executable directly
        result = subprocess.run([test_exe], 
                              capture_output=True, 
                              text=True, 
                              cwd=build_dir)
        
        print(result.stdout)
        if result.stderr:
            print("STDERR:", result.stderr)
        
        if result.returncode == 0:
            print(f"[PASS] {test_name} PASSED")
            return (test_name, "PASSED", "")
        else:
            print(f"[FAIL] {test_name} FAILED (exit code: {result.returncode})")
            return (test_name, "FAILED", result.stderr)
            
    except Exception as e:
        print(f"[ERROR] {test_name} ERROR: {e}")
        return (test_name, "ERROR", str(e))


def run_ctest_fallback(build_dir, venv_path):
    """Try running tests using CTest as a fallback."""
    print("No test executables found. Trying CTest...")
    try:
        # Get ctest command path (same directory as cmake)
        if platform.system() == "Windows":
            ctest_command = get_platform_executable_path(venv_path, "ctest")
        else:
            ctest_command = str(venv_path / UNIX_BIN_DIR / "ctest")
        
        cmd = [ctest_command, "--verbose", "--config", "Debug"]
        execute_command(cmd, cwd=build_dir)
        return [("CTest", "PASSED", "")]
    except Exception as e:
        print(f"CTest failed: {e}")
        return [("CTest", "FAILED", str(e))]


def load_excluded_targets_config():
    """Load excluded targets configuration from JSON file"""
    config_path = Path(__file__).parent / EXCLUDED_TESTS_CONFIG
    try:
        if config_path.exists():
            with open(config_path, 'r') as f:
                config = json.load(f)
                return config.get('excluded_targets', {})
        else:
            print("Warning: Configuration file not found, using default exclusions")
            return {}
    except (json.JSONDecodeError, IOError) as e:
        print("Warning: Failed to load excluded targets config")
        return {}


def create_venv(path):
    """Create a virtual environment at the specified path."""
    print(f"\nCreating virtual environment: {path.resolve()}")
    try:
        venv.EnvBuilder(clear=True, with_pip=True).create(path)
    except (subprocess.CalledProcessError, ImportError, FileExistsError, RuntimeError) as e:
        print(f"\nAn error occurred: {str(e)}. Trying to clean up and create venv without using the EnvBuilder.")
        shutil.rmtree(path, ignore_errors=True)
        try:
            cmd = [get_python_command(), "-m", "venv", str(path)]
            execute_command(cmd)
        except Exception as e:
            print(f"\nAn error occurred while creating venv: {e}")
            sys.exit(1)


@dataclass
class ScriptParams:
    test_dir: Path
    build_dir: Path
    venv_dir: Path
    setup_env: bool
    build_tests: bool
    run_tests: bool
    config: str
    excluded_targets: dict = None
    specific_targets: list = None


def setup_conan_env(build_dir):
    """Setup isolated conan environment."""
    conan_cache_dir = build_dir / CONAN_CACHE_DIR_NAME
    conan_cache_dir.mkdir(exist_ok=True)
    
    # For Conan 2, we use CONAN_HOME environment variable to isolate the cache
    os.environ["CONAN_HOME"] = str(conan_cache_dir)
    print(f"Isolating conan cache: CONAN_HOME={os.environ['CONAN_HOME']}")
    
    # Clear any existing Conan-related environment variables that might interfere
    conan_env_vars = [key for key in os.environ.keys() if key.startswith('CONAN_') and key != 'CONAN_HOME']
    for var in conan_env_vars:
        del os.environ[var]
        print(f"Cleared environment variable: {var}")


def get_venv_python_command(venv_path):
    """Get the python command for the virtual environment."""
    return get_platform_executable_path(venv_path, "python")


def get_venv_environment(venv_path):
    """Get environment variables with virtual environment's bin directory in PATH."""
    env = os.environ.copy()
    
    # Add the virtual environment's bin directory to PATH
    if platform.system() == "Windows":
        venv_bin_dir = str(venv_path / WINDOWS_SCRIPTS_DIR)
    else:
        venv_bin_dir = str(venv_path / UNIX_BIN_DIR)
    
    # Prepend the venv bin directory to PATH so it takes precedence
    if "PATH" in env:
        env["PATH"] = f"{venv_bin_dir}{os.pathsep}{env['PATH']}"
    else:
        env["PATH"] = venv_bin_dir
    
    return env


def setup_virtual_environment(params):
    """Setup the virtual environment and install dependencies."""
    if not params.venv_dir.exists():
        create_venv(params.venv_dir)
    else:
        print(f"\nVirtual environment already exists: {params.venv_dir.resolve()}")
    
    venv_python = get_venv_python_command(params.venv_dir)
    
    # Get virtual environment for consistent PATH
    env = get_venv_environment(params.venv_dir)
    
    # Install requirements
    requirements_file = params.test_dir / WINDOWS_SCRIPTS_DIR / REQUIREMENTS_FILE
    cmd = [venv_python, "-m", "pip", "install", "-r", str(requirements_file)]
    execute_command(cmd, description=f"Installing requirements from {requirements_file}", env=env)
    
    # Verify installations
    cmd = [venv_python, "-m", "pip", "list"]
    execute_command(cmd, description="Verifying installations:", env=env)


def prepare_cmake_environment(params):
    """Prepare CMake environment using the existing Test/CMakeLists.txt in place."""
    
    # Load excluded targets from configuration file (these will be counted as skipped)
    excluded_targets = load_excluded_targets_config()
    if not excluded_targets:
        excluded_targets = {}  # Fallback to empty dict if config loading fails
    
    # Store excluded targets for later use in build step
    params.excluded_targets = excluded_targets
    
    # Discover all test files for statistics
    all_test_files = []
    for file_path in params.test_dir.glob("*_UnitTest.cpp"):
        all_test_files.append(file_path.name)
    
    print(f"Discovered {len(all_test_files)} total test files")
    
    if params.specific_targets:
        print(f"Will build/run only specified targets: {', '.join(params.specific_targets)}")
    else:
        print(f"Will exclude {len(excluded_targets)} targets via cmake arguments")
        if excluded_targets:
            print("Excluded targets:")
            for target, reason in excluded_targets.items():
                print(f"  - {target}: {reason}")
    
    print("Using Test/CMakeLists.txt in place with separate build directory")
    
    # Return the count of excluded targets for reporting
    return len(excluded_targets)


def setup_conan_dependencies(params):
    """Install conan dependencies."""
    venv_python = get_venv_python_command(params.venv_dir)
    venv_conan = get_platform_executable_path(params.venv_dir, "conan")
    conanfile = params.test_dir / WINDOWS_SCRIPTS_DIR / CONANFILE_NAME
    
    print(f"\nInstalling conan dependencies from {conanfile}")
    
    # Copy conanfile to build directory
    shutil.copy2(conanfile, params.build_dir / CONANFILE_NAME)
    
    # Create default profile if it doesn't exist (for Conan 2)
    conan_cache_dir = params.build_dir / CONAN_CACHE_DIR_NAME
    profile_path = conan_cache_dir / "profiles" / "default"
    if not profile_path.exists():
        print("Creating default Conan profile...")
        profile_cmd = [venv_conan, "profile", "detect", "--force"]
        # Execute with CONAN_HOME environment variable set and venv PATH
        env = get_venv_environment(params.venv_dir)
        env["CONAN_HOME"] = str(conan_cache_dir)
        subprocess.run(profile_cmd, env=env, cwd=params.build_dir, check=True)
    
    # Run conan install with isolated cache (via CONAN_HOME environment variable)
    cmd = [
        venv_conan, "install", ".",
        "--output-folder=.",
        "--build=missing",
        "-s", f"build_type={params.config}"
    ]
    
    # Add compiler.libcxx setting only for Linux
    if platform.system() == "Linux":
        cmd.extend(["-s", "compiler.libcxx=libstdc++11"])
    
    # Execute with CONAN_HOME environment variable set and venv PATH
    env = get_venv_environment(params.venv_dir)
    env["CONAN_HOME"] = str(conan_cache_dir)
    
    print(f"Running conan install with isolated cache: CONAN_HOME={env['CONAN_HOME']}")
    print(f"Virtual environment PATH: {env['PATH'].split(os.pathsep)[0]} (first entry)")
    subprocess.run(cmd, env=env, cwd=params.build_dir, check=True)


def get_cmake_targets_from_files(build_dir, test_dir, include_all=False):
    """Extract target names directly from CMakeLists.txt files.
    
    Args:
        build_dir: Build directory path
        test_dir: Test directory path containing CMakeLists.txt
        include_all: If True, include all executable targets. If False, only include *_UnitTest targets.
    """
    targets = []
    
    # Read the main Test/CMakeLists.txt
    cmake_file = test_dir / "CMakeLists.txt"
    if cmake_file.exists():
        print(f"Parsing CMakeLists.txt from: {cmake_file}")
        try:
            with open(cmake_file, 'r', encoding='utf-8') as f:
                content = f.read()
            
            print(f"  CMakeLists.txt file size: {len(content)} bytes")
                
            # Look for add_executable() calls and avid_add_gtest_unittest() calls
            # Pattern: add_executable(target_name ...) or avid_add_gtest_unittest(target_name)
            import re
            
            # Match both add_executable and avid_add_gtest_unittest
            patterns = [
                r'add_executable\s*\(\s*([A-Za-z0-9_]+)',
                r'avid_add_gtest_unittest\s*\(\s*([A-Za-z0-9_]+)',
                # Also match add_test calls which might indicate test targets
                r'add_test\s*\(\s*NAME\s+([A-Za-z0-9_]+)',
            ]
            
            for pattern in patterns:
                matches = re.findall(pattern, content)
                print(f"  Pattern '{pattern[:50]}...' found {len(matches)} matches")
                for target_name in matches:
                    if include_all:
                        targets.append(target_name)
                    elif 'UnitTest' in target_name or target_name.endswith('_UnitTest'):
                        targets.append(target_name)
            
            # Remove duplicates
            targets = sorted(list(set(targets)))
            print(f"Found {len(targets)} unique targets by parsing CMakeLists.txt")
            if len(targets) > 0:
                print(f"  First 5 targets: {targets[:5]}")
            
        except Exception as e:
            print(f"Warning: Failed to parse CMakeLists.txt: {e}")
    else:
        print(f"Warning: CMakeLists.txt not found at: {cmake_file}")
    
    return sorted(list(set(targets)))


def get_cmake_targets(cmake_command, build_dir, env=None, include_all=False, test_dir=None):
    """Get list of available CMake targets by running cmake --build --target help.
    
    Args:
        cmake_command: Path to cmake executable
        build_dir: Build directory path
        env: Environment variables
        include_all: If True, include all executable targets. If False, only include *_UnitTest targets.
        test_dir: Test directory path (for fallback parsing of CMakeLists.txt)
    """
    try:
        # Run cmake --build with help target to get list of available targets
        result = subprocess.run([
            cmake_command,
            "--build", str(build_dir),
            "--target", "help"
        ], capture_output=True, text=True, cwd=build_dir, env=env)
        
        # Debug output to see what CMake actually returned
        #print(f"\nDEBUG: CMake command returncode: {result.returncode}")
        #print(f"DEBUG: CMake stdout length: {len(result.stdout)} chars")
        #print(f"DEBUG: CMake stderr length: {len(result.stderr)} chars")
        
        if result.returncode != 0:
            print(f"Warning: CMake --build --target help failed (this is expected on Windows with Visual Studio generator)")
            if result.stderr:
                print(f"  STDERR: {result.stderr[:500]}")
            if result.stdout:
                print(f"  STDOUT: {result.stdout[:500]}")
            # Try fallback immediately
            if test_dir:
                print("Attempting fallback: parsing CMakeLists.txt...")
                return get_cmake_targets_from_files(build_dir, test_dir, include_all)
            return []
        
        # Show a sample of the output for debugging
        #if result.stdout:
        #    output_lines = result.stdout.split('\n')[:20]  # First 20 lines
        #    print("DEBUG: First lines of CMake output:")
        #    for i, line in enumerate(output_lines, 1):
        #        print(f"  {i}: {repr(line)[:100]}")
        
        # Parse the output to extract target names
        # Handle both Makefile format ("... target_name") and Visual Studio format
        targets = []
        lines = result.stdout.split('\n')
        
        # First try Visual Studio format - look for lines that are just target names
        # after build engine information
        vs_mode = False
        for line in lines:
            stripped = line.strip()
            
            # Detect Visual Studio output format
            if 'Microsoft (R) Build Engine' in line or 'MSBuild version' in line:
                vs_mode = True
                continue
            
            # Skip empty lines and build engine info
            if not stripped or stripped.startswith('Microsoft') or stripped.startswith('Build started'):
                continue
            
            # For Makefile format: lines start with "... "
            if stripped.startswith('... '):
                target_name = stripped[4:]  # Remove "... " prefix
                
                # Only include executable targets, not object files
                if (not target_name.endswith('.o') and 
                    not target_name.endswith('.i') and 
                    not target_name.endswith('.s') and
                    not '/' in target_name and
                    not '\\' in target_name and  # Also exclude Windows paths
                    not target_name.startswith('/')):
                    
                    if include_all:
                        targets.append(target_name)
                    elif 'UnitTest' in target_name or target_name.endswith('_UnitTest'):
                        targets.append(target_name)
            
            # For Visual Studio format: target names are typically listed one per line
            # after the build starts, but before any file paths or detailed output
            elif vs_mode and not any(char in stripped for char in [':', '\\', '/', '.vcxproj', '.sln']):
                # This looks like a plain target name
                target_name = stripped
                
                # Skip common build output that isn't a target
                skip_patterns = ['Build', 'Project', 'Time Elapsed', 'Build succeeded', 
                                'Build failed', 'Warning', 'Error', 'Copyright', 'Version']
                if any(pattern in target_name for pattern in skip_patterns):
                    continue
                
                if target_name:  # Non-empty
                    if include_all:
                        targets.append(target_name)
                    elif 'UnitTest' in target_name or target_name.endswith('_UnitTest'):
                        targets.append(target_name)
        
        # Remove duplicates and sort
        targets = sorted(list(set(targets)))
        target_type = "targets" if include_all else "test targets"
        
        if targets:
            print(f"Found {len(targets)} {target_type} via CMake")
        else:
            print(f"Found 0 {target_type} via CMake output parsing")
            # If no targets found via cmake command, try parsing CMakeLists.txt as fallback
            if test_dir:
                print("Attempting fallback: parsing CMakeLists.txt...")
                targets = get_cmake_targets_from_files(build_dir, test_dir, include_all)
        
        return targets
        
    except Exception as e:
        print(f"Error getting CMake targets: {e}")
        import traceback
        traceback.print_exc()
        
        # Try fallback method
        if test_dir:
            print("Attempting fallback: parsing CMakeLists.txt...")
            return get_cmake_targets_from_files(build_dir, test_dir, include_all)
        return []


def build_aax_sdk(params, cmake_command):
    """Build the AAX SDK with disabled optional components."""
    aax_sdk_build_dir = params.test_dir.parent / 'build'
    
    # Get virtual environment with PATH updated
    env = get_venv_environment(params.venv_dir)
    
    # Configure AAX SDK with disabled options
    aax_sdk_args = [
        cmake_command,
        f"-S", str(params.test_dir.parent),
        f"-B", str(aax_sdk_build_dir),
        f"-DCMAKE_BUILD_TYPE={params.config}"
    ] + AAX_SDK_BUILD_OPTIONS
    
    execute_command(aax_sdk_args, description=f"Configuring AAX SDK (with disabled optional modules) in {aax_sdk_build_dir}", env=env)
    
    # Build AAX SDK
    aax_build_args = [
        cmake_command,
        "--build", str(aax_sdk_build_dir),
        "--config", params.config,
        "--target", AAX_LIBRARY_TARGET
    ]
    
    execute_command(aax_build_args, description="Building AAX SDK", env=env)


def configure_unit_tests(params, cmake_command):
    """Configure the unit tests using CMake."""
    # Get virtual environment with PATH updated
    env = get_venv_environment(params.venv_dir)
    
    cmake_args = [
        cmake_command,
        f"-S", str(params.test_dir),  # Use Test/CMakeLists.txt in place
        f"-B", str(params.build_dir),  # Build artifacts go to sandbox directory
        f"-DCMAKE_BUILD_TYPE={params.config}",
        f"-DCMAKE_TOOLCHAIN_FILE={params.build_dir / CONAN_TOOLCHAIN_FILE}",
        # Point to AAX SDK package location so find_package(AAX_SDK) works
        f"-DCMAKE_PREFIX_PATH={params.test_dir.parent}/build",
    ]
    
    execute_command(cmake_args, description=f"Configuring unit tests in {params.build_dir}", env=env)


def build_unit_tests_targets(params, cmake_command):
    """Build the unit test targets based on parameters."""
    print(f"\nBuilding unit tests")
    
    if params.specific_targets:
        _build_specific_targets(params, cmake_command)
    else:
        _build_all_or_excluded_targets(params, cmake_command)


def _build_specific_targets(params, cmake_command):
    """Build only the specified targets."""
    print(f"\nBuilding {len(params.specific_targets)} specified targets: {', '.join(params.specific_targets)}")
    
    # Get virtual environment with PATH updated
    env = get_venv_environment(params.venv_dir)
    
    # Validate that specified targets exist (include all types, not just unit tests)
    all_targets = get_cmake_targets(cmake_command, params.build_dir, env, include_all=True, test_dir=params.test_dir)
    invalid_targets = [t for t in params.specific_targets if t not in all_targets]
    
    if invalid_targets:
        print(f"Warning: The following targets were not found: {', '.join(invalid_targets)}")
        print(f"Available targets: {', '.join(sorted(all_targets))}")
    
    valid_targets = [t for t in params.specific_targets if t in all_targets]
    
    if valid_targets:
        build_args = [
            cmake_command,
            "--build", str(params.build_dir),
            "--config", params.config
        ]
        # Add all specified targets to the command
        for target in valid_targets:
            build_args.extend(["--target", target])
        
        execute_command(build_args, env=env)
    else:
        print("No valid targets to build")


def _build_all_or_excluded_targets(params, cmake_command):
    """Build all targets or exclude the problematic ones."""
    excluded_targets = getattr(params, 'excluded_targets', {})
    
    # Get virtual environment with PATH updated
    env = get_venv_environment(params.venv_dir)
    
    if excluded_targets:
        print(f"\nExcluding {len(excluded_targets)} targets from build:")
        for target, reason in excluded_targets.items():
            print(f"  - {target}: {reason}")
        
        # Get list of all targets using CMake (unit tests only for exclusion logic)
        all_targets = get_cmake_targets(cmake_command, params.build_dir, env, include_all=False, test_dir=params.test_dir)
        
        # Build only the targets that are not excluded
        included_targets = [t for t in all_targets if t not in excluded_targets]
        
        if included_targets:
            # Build all targets in a single cmake command
            build_args = [
                cmake_command,
                "--build", str(params.build_dir),
                "--config", params.config
            ]
            # Add all targets to the command
            for target in included_targets:
                build_args.extend(["--target", target])
            
            execute_command(build_args, description=f"Building {len(included_targets)} included targets...", env=env)
        else:
            print("No targets to build after exclusions")
    else:
        # Build all targets if no exclusions
        build_args = [
            cmake_command,
            "--build", str(params.build_dir),
            "--config", params.config
        ]
        execute_command(build_args, env=env)


def build_tests(params):
    """Build the unit tests using CMake."""
    cmake_command = get_cmake_command(params.venv_dir)
    
    # Build the AAX SDK first
    build_aax_sdk(params, cmake_command)
    
    # Configure the unit tests
    configure_unit_tests(params, cmake_command)
    
    # Build the unit test targets
    build_unit_tests_targets(params, cmake_command)


def run_tests(params):
    """Run all built unit tests."""
    print(f"\nRunning unit tests from {params.build_dir}")
    
    # Discover test executables (include utils/ only if specific targets requested)
    test_executables = discover_test_executables(params.build_dir, params.config, params.specific_targets)
    
    # Filter by specific targets if requested
    if params.specific_targets:
        test_executables = filter_executables_by_targets(test_executables, params.specific_targets)
        if not test_executables:
            print(f"No test executables found for specified targets: {', '.join(params.specific_targets)}")
            return []
        print(f"Running {len(test_executables)} specified test executables: {', '.join(os.path.basename(exe) for exe in test_executables)}")
    else:
        print(f"Running {len(test_executables)} test executables")
    
    # If no executables found, try CTest fallback
    if not test_executables:
        return run_ctest_fallback(params.build_dir, params.venv_dir)
    
    # Run each test executable
    test_results = []
    for test_exe in test_executables:
        result = run_single_test(test_exe, params.build_dir)
        test_results.append(result)
    
    return test_results


def print_summary(params, test_results=None, skipped_count=0):
    """Print a summary of all actions performed."""
    # Print failures summary first if there are any failures
    if test_results:
        failed = [name for name, status, _ in test_results if status == "FAILED"]
        errors = [name for name, status, _ in test_results if status == "ERROR"]
        
        if failed or errors:
            print(f"\n{'='*80}")
            print("FAILED TESTS SUMMARY")
            print(f"{'='*80}")
            if failed:
                print("FAILED:")
                for name in failed:
                    print(f"  - {name}")
            if errors:
                print("ERROR:")
                for name in errors:
                    print(f"  - {name}")
            print()
    
    print(f"\n{'='*80}")
    print("SUMMARY OF ACTIONS PERFORMED")
    print(f"{'='*80}")
    
    print(f"Virtual environment: {params.venv_dir.resolve()}")
    print(f"Build directory: {params.build_dir.resolve()}")
    print(f"Configuration: {params.config}")
    
    if params.venv_dir.exists():
        print(f"[OK] Virtual environment created/verified")
    
    cmake_file = params.build_dir / "CMakeLists.txt"
    if cmake_file.exists():
        print(f"[OK] Standalone CMakeLists.txt generated: {cmake_file}")
    
    conanfile = params.build_dir / "conanfile.py"
    if conanfile.exists():
        print(f"[OK] Conan dependencies configured: {conanfile}")
    
    if test_results:
        print(f"\nTEST RESULTS:")
        passed = sum(1 for _, status, _ in test_results if status == "PASSED")
        failed = sum(1 for _, status, _ in test_results if status == "FAILED")
        errors = sum(1 for _, status, _ in test_results if status == "ERROR")
        
        print(f"  Total tests: {len(test_results)}")
        print(f"  [PASS] Passed: {passed}")
        print(f"  [FAIL] Failed: {failed}")
        print(f"  [ERROR] Errors: {errors}")
        print(f"  [SKIP] Skipped: {skipped_count}")
        
        if failed > 0 or errors > 0:
            print(f"\nFAILED/ERROR TESTS:")
            for name, status, error in test_results:
                if status != "PASSED":
                    print(f"  {status}: {name}")
                    if error:
                        print(f"    {error[:200]}...")
    
    # Check for log files
    log_files = list(params.build_dir.glob("*.log")) + list(params.build_dir.glob("**/*.log"))
    if log_files:
        print(f"\nLOG FILES:")
        for log_file in log_files:
            print(f"  {log_file}")


def list_available_targets(params):
    """List all available targets that can be built."""
    print(f"\n{'='*80}")
    print("AVAILABLE TARGETS")
    print(f"{'='*80}")
    
    # Check if build directory exists with CMake configuration
    if not params.build_dir.exists() or not (params.build_dir / "CMakeCache.txt").exists():
        print("\nBuild directory not configured yet.")
        print("Reading target list directly from CMakeLists.txt...\n")
        
        # Use the CMakeLists.txt parsing fallback directly - no need to configure yet
        all_targets = get_cmake_targets_from_files(params.build_dir, params.test_dir, include_all=True)
    
    else:
        # Build is already configured, try to use cmake command
        # Get cmake command
        cmake_command = get_cmake_command(params.venv_dir)
        env = get_venv_environment(params.venv_dir)
        
        # Get all targets (tests and utilities)
        all_targets = get_cmake_targets(cmake_command, params.build_dir, env, include_all=True, test_dir=params.test_dir)

    # Fallback: If CMake target list is empty, scan build output directories for executables
    if not all_targets:
        print("\nCMake did not return any targets. Scanning build output directories for executables...")
        config = params.config if params.config in VALID_CONFIGS else DEFAULT_CONFIG
        test_dirs = [params.build_dir / "tests" / config, params.build_dir / "utils" / config]
        found_targets = set()
        for d in test_dirs:
            if d.exists():
                for f in d.iterdir():
                    if platform.system() == "Windows":
                        if f.is_file() and f.suffix.lower() == ".exe":
                            found_targets.add(f.stem)
                    else:
                        if f.is_file() and os.access(f, os.X_OK):
                            found_targets.add(f.name)
        all_targets = sorted(found_targets)
        if not all_targets:
            print("No executables found in build output directories.")
        else:
            print(f"Found {len(all_targets)} executables in build output directories.")

    # Separate into categories
    unit_tests = [t for t in all_targets if 'UnitTest' in t or t.endswith('_UnitTest')]
    utilities = [t for t in all_targets if t not in unit_tests]

    # Print unit tests
    print(f"\nUnit Test Targets ({len(unit_tests)}):")
    print("-" * 80)
    for test in sorted(unit_tests):
        print(f"  {test}")

    # Print utilities if any
    if utilities:
        print(f"\nUtility Targets ({len(utilities)}):")
        print("-" * 80)
        for util in sorted(utilities):
            print(f"  {util}")

    print(f"\n{'='*80}")
    print(f"Total: {len(all_targets)} targets ({len(unit_tests)} tests, {len(utilities)} utilities)")
    print(f"{'='*80}")
    print("\nUsage examples:")
    print("  # Build and run specific test:")
    print("  python3 build_unit_tests.py ACF_ABI_Verification_UnitTest")
    print("\n  # Build utility without running:")
    print("  python3 build_unit_tests.py --no-run abi_extractor")
    print("\n  # Build multiple targets:")
    print("  python3 build_unit_tests.py abi_extractor ACF_ABI_Verification_UnitTest")
    print()


def clean_environment(params):
    """Clean up all generated files and directories."""
    print(f"Cleaning up environment...")
    
    if params.venv_dir.exists():
        print(f"Removing virtual environment: {params.venv_dir}")
        shutil.rmtree(params.venv_dir, ignore_errors=True)
    
    if params.build_dir.exists():
        print(f"Removing build directory: {params.build_dir}")
        shutil.rmtree(params.build_dir, ignore_errors=True)
    
    # Clean up Python cache directories
    scripts_pycache = params.test_dir / "Scripts" / "__pycache__"
    if scripts_pycache.exists():
        print(f"Removing Python cache: {scripts_pycache}")
        shutil.rmtree(scripts_pycache, ignore_errors=True)
    
    print("Clean completed.")


def print_help():
    """Print comprehensive help information."""
    help_text = """
AAX SDK Unit Test Builder

This script creates an isolated virtual environment and builds/runs the AAX SDK unit tests
using Google Test/Google Mock framework via Conan package manager.

USAGE:
    python3 build_unit_tests.py [OPTIONS] [TARGETS...]

ARGUMENTS:
    TARGETS              Specific targets to build/run (optional)
                        Can be unit test targets or utility executables
                        Example: AAX_DataBuffer_UnitTest abi_extractor
                        If not specified, all unit tests are processed

OPTIONS:
    --setup-env, -s      Set up the virtual environment (default: True)
    --no-setup-env       Skip virtual environment setup
    --build, -b          Build the unit tests (default: True)  
    --no-build           Skip building tests
    --run, -r            Run the unit tests (default: True)
    --no-run             Skip running tests
    --config CONFIG      Build configuration: Debug or Release (default: Debug)
    --clean, -c          Clean up all generated files and exit
    --list-targets, -l   List all available targets and exit
    --help, -h           Show this help message and exit

EXAMPLES:
    # Full build and test (default behavior)
    python3 build_unit_tests.py
    
    # Build and run specific tests only
    python3 build_unit_tests.py AAX_DataBuffer_UnitTest AAX_PanelSupport_UnitTest
    
    # Run specific tests without building (assumes already built)
    python3 build_unit_tests.py --no-build AAX_DataBuffer_UnitTest AAX_PanelSupport_UnitTest
    
    # Build specific tests without running them
    python3 build_unit_tests.py --no-run AAX_DataBuffer_UnitTest
    
    # Only setup environment, don't build or run
    python3 build_unit_tests.py --no-build --no-run
    
    # Build and run all tests with Release configuration
    python3 build_unit_tests.py --config Release
    
    # Clean up all generated files
    python3 build_unit_tests.py --clean
    
    # List all available targets
    python3 build_unit_tests.py --list-targets
    
    # Only run all tests (assumes already built)
    python3 build_unit_tests.py --no-setup-env --no-build

SIDE EFFECTS:
    This script creates the following directories and files:
    - .venv/                 Virtual environment (in Test directory)
    - .build/                Build artifacts and CMake files
    - .build/.conan/         Isolated Conan cache
    - .build/CMakeLists.txt  Generated standalone CMake configuration
    - .build/conanfile.py    Conan dependencies configuration
    - Scripts/__pycache__/   Python bytecode cache (in Test/Scripts directory)

    The script does NOT modify the system outside of these directories.
    Use --clean to remove all generated files.

DEPENDENCIES:
    The script will automatically install these dependencies in the virtual environment:
    - cmake (3.28.1)
    - conan (2.21.0)  
    - gtest (1.14.0) - via Conan

REQUIREMENTS:
    - Python 3.6 or later
    - CMake-compatible C++ compiler (GCC, Clang, MSVC)
    - Internet connection (for downloading dependencies)
"""
    print(help_text)


def prepare_parser():
    """Prepare command line argument parser."""
    parser = argparse.ArgumentParser(
        prog='AAX SDK Unit Test Builder',
        description='Build and run AAX SDK unit tests in isolated environment',
        add_help=False  # We'll handle help ourselves
    )
    
    parser.add_argument('targets', nargs='*',
                       help='Specific targets to build/run (e.g., AAX_DataBuffer_UnitTest, abi_extractor). If not specified, all unit tests are processed.')
    
    parser.add_argument('--setup-env', '-s', action='store_true', default=True,
                       help='Set up virtual environment')
    parser.add_argument('--no-setup-env', action='store_false', dest='setup_env',
                       help='Skip virtual environment setup')
    
    parser.add_argument('--build', '-b', action='store_true', default=True,
                       help='Build unit tests')
    parser.add_argument('--no-build', action='store_false', dest='build',
                       help='Skip building tests')
    
    parser.add_argument('--run', '-r', action='store_true', default=True,
                       help='Run unit tests')
    parser.add_argument('--no-run', action='store_false', dest='run',
                       help='Skip running tests')
    
    parser.add_argument('--config', default=DEFAULT_CONFIG, choices=VALID_CONFIGS,
                       help=f'Build configuration (default: {DEFAULT_CONFIG})')
    
    parser.add_argument('--clean', '-c', action='store_true',
                       help='Clean up generated files and exit')
    
    parser.add_argument('--list-targets', '-l', action='store_true',
                       help='List all available targets and exit')
    
    parser.add_argument('--help', '-h', action='store_true',
                       help='Show help message and exit')
    
    return parser


if __name__ == "__main__":
    print(f"Platform: {platform.system()}")
    print(f"Python Version: {sys.version}")
    print(f"Python Path: {sys.executable}")
    
    if sys.version_info.major < 3 or (sys.version_info.major == 3 and sys.version_info.minor < 6):
        print(f"Python version {sys.version} is not supported. Python 3.6+ is required.")
        sys.exit(1)
    
    parser = prepare_parser()
    args = parser.parse_args()
    
    if args.help:
        print_help()
        sys.exit(0)
    
    # Setup paths
    test_dir = Path(__file__).parent.parent  # Scripts/../ = Test/
    venv_dir = test_dir / VENV_DIR_NAME
    build_dir = test_dir / BUILD_DIR_NAME
    
    params = ScriptParams(
        test_dir=test_dir,
        build_dir=build_dir,
        venv_dir=venv_dir,
        setup_env=args.setup_env,
        build_tests=args.build,
        run_tests=args.run,
        config=args.config,
        specific_targets=args.targets if args.targets else None
    )
    
    if args.clean:
        clean_environment(params)
        sys.exit(0)
    
    if args.list_targets:
        list_available_targets(params)
        sys.exit(0)
    
    test_results = None
    skipped_count = 0
    
    try:
        if params.setup_env:
            setup_virtual_environment(params)
            params.build_dir.mkdir(exist_ok=True)
            setup_conan_env(params.build_dir)
            setup_conan_dependencies(params)
        else:
            # When --no-setup-env is used, validate that virtual environment exists
            if not params.venv_dir.exists():
                print(f"\nERROR: Virtual environment not found at {params.venv_dir}")
                print("Please run the script without --no-setup-env first to create the environment,")
                print("or remove --no-setup-env to allow automatic environment setup.")
                sys.exit(1)
                
            # Validate that required tools are available in the virtual environment
            cmake_command = get_cmake_command(params.venv_dir)
            if not os.path.exists(cmake_command):
                print(f"\nERROR: CMake not found in virtual environment at {cmake_command}")
                print("Please run the script without --no-setup-env to reinstall dependencies,")
                print("or remove --no-setup-env to allow automatic environment setup.")
                sys.exit(1)
            
            venv_conan = get_platform_executable_path(params.venv_dir, "conan")
            if not os.path.exists(venv_conan):
                print(f"\nERROR: Conan not found in virtual environment at {venv_conan}")
                print("Please run the script without --no-setup-env to reinstall dependencies,")
                print("or remove --no-setup-env to allow automatic environment setup.")
                sys.exit(1)
                
            print(f"\nUsing existing virtual environment: {params.venv_dir.resolve()}")
            
        if params.build_tests:
            params.build_dir.mkdir(exist_ok=True)
            if params.setup_env:
                # Conan dependencies were already set up above in setup_env block
                pass
            else:
                # For --no-setup-env, we still need to ensure build dir has conan environment set up
                # but we skip the actual dependency installation
                setup_conan_env(params.build_dir)
            skipped_count = prepare_cmake_environment(params)
            build_tests(params)
            
        if params.run_tests:
            test_results = run_tests(params)
            
        print_summary(params, test_results, skipped_count)
        
        # Exit with error code if any tests failed
        if test_results:
            failed_count = sum(1 for _, status, _ in test_results if status in ["FAILED", "ERROR"])
            if failed_count > 0:
                sys.exit(1)
        
    except KeyboardInterrupt:
        print("\n\nOperation cancelled by user.")
        sys.exit(1)
    except Exception as e:
        print(f"\n\nFATAL ERROR: {e}")
        print_summary(params, test_results)
        sys.exit(1)
