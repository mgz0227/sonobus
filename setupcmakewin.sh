#!/bin/bash

# 使用相对路径引用 SDK 目录
AAX_SDK_PATH="./SDK/aax"

DEPS=""

if [ -d "${AAX_SDK_PATH}" ] ; then
  echo "Will build AAX plugin using path: ${AAX_SDK_PATH}"
  DEPS="$DEPS -DAAX_SDK_PATH=${AAX_SDK_PATH}"
else
  echo "Warning: AAX SDK path not found at ${AAX_SDK_PATH}, skipping AAX build"
fi

# 可选：添加 VST2 SDK 路径
# VST2_SDK_PATH="./SDK/vst2"
# if [ -d "${VST2_SDK_PATH}" ] ; then
#   echo "Will build VST2 plugin using path: ${VST2_SDK_PATH}"
#   DEPS="$DEPS -DVST2_SDK_PATH=${VST2_SDK_PATH}"
# fi

# 调用 CMake 构建
cmake -G "Visual Studio 17 2022" -A "x64" $DEPS -B build

