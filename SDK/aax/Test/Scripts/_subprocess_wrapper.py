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


import subprocess
import os
import locale
import logging

class WarningFormatter(logging.Formatter):
    def format(self, record):
        if record.levelno in (logging.WARNING, logging.ERROR):
            return f"{record.msg}"
        return record.msg

logging.basicConfig(level=logging.INFO)

handler = logging.StreamHandler()
handler.setFormatter(WarningFormatter())

logger = logging.getLogger()

for existing_handler in logger.handlers:
    existing_handler.setFormatter(handler.formatter)

def _wrap_subprocess(cmd, cwd=os.getcwd(), env=None):
    if isinstance(cmd, list):
        cmd = " ".join(cmd)
        
    enc = locale.getpreferredencoding(False) or "utf-8"
    
    p = subprocess.Popen(
        cmd,
        shell=True,
        cwd=cwd,
        env=env,  # Use the provided environment or inherit from parent
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        encoding=enc,
        errors="replace"
    )
    try:
        for line in p.stdout:   
            logger.info(line.rstrip())
        p.communicate()
        exit_code = p.returncode
        if exit_code != 0:
            raise RuntimeError(f"Subprocess returned non-zero exit code: {exit_code}")
    except Exception as e:
        p.terminate()
        logger.error(f"Process has been terminated due to an exception: {e}")
        raise e
    return exit_code
