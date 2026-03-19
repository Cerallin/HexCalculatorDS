#!/bin/bash
set -euo pipefail

DEVKITPRO_PATH="${DEVKITPRO:-/opt/devkitpro}"

cmake -S . -B build-cppcheck \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DCMAKE_TOOLCHAIN_FILE=cmake/devkitarm-toolchain.cmake \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

cppcheck \
  --project=build-cppcheck/compile_commands.json \
  --enable=all \
  --std=c++17 \
  --suppress=missingIncludeSystem \
  --suppress=*:"${DEVKITPRO_PATH}/libnds/*" \
  --inline-suppr \
  --error-exitcode=1 \
  2>reports/cppcheck.txt
