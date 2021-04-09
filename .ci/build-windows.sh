#!/usr/bin/env bash
set -eo pipefail

if [[ "$DEBUG_CI" == "true" ]]; then
  export VERBOSE=true
  set -x
fi

# Default configuration when used out of travis-ci
MODE=${MODE:-Debug}
EXTRA_CMAKE_OPTIONS=${EXTRA_CMAKE_OPTIONS:-}
BUILD_TEST=${BUILD_TEST:-true}

export ENABLE_COVERAGE=${ENABLE_COVERAGE:-off}
export ENABLE_MEMCHECK=${ENABLE_MEMCHECK:-off}
export ENABLE_STATIC_ANALYSIS=${ENABLE_STATIC_ANALYSIS:-off}

conan profile new default --detect
#conan profile update settings.build_type="${MODE}" default

mkdir -p build
cd build
# /!\ use profile defined above 
conan install ..
cmake \
  -DCMAKE_GENERATOR_PLATFORM=x64 \
  -DCMAKE_TOOLCHAIN_FILE=conan_paths.cmake \
  -DENABLE_COVERAGE="${ENABLE_COVERAGE}" \
  -DENABLE_MEMCHECK="${ENABLE_MEMCHECK}" \
  -DENABLE_STATIC_ANALYSIS=${ENABLE_STATIC_ANALYSIS} \
  $(eval echo ${EXTRA_CMAKE_OPTIONS}) \
  ..
  
cmake --build . --config "${MODE}"