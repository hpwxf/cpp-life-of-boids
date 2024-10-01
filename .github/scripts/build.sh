#!/usr/bin/env bash
set -eo pipefail

if [[ "$DEBUG_CI" == "true" ]]; then
  export VERBOSE=true
  set -x
fi

BASEDIR=$(dirname "$0")
BASEDIR=$(cd "$BASEDIR" && pwd -P)

# Default configuration when used out of travis-ci
MODE=${MODE:-Debug}
EXTRA_CMAKE_OPTIONS=${EXTRA_CMAKE_OPTIONS:-}

export ENABLE_COVERAGE=${ENABLE_COVERAGE:-off}
export ENABLE_MEMCHECK=${ENABLE_MEMCHECK:-off}
export ENABLE_STATIC_ANALYSIS=${ENABLE_STATIC_ANALYSIS:-off}

conan profile detect
CONAN_PROFILE_FILE=$(conan profile path default)

case "$COMPILER" in 
  gcc*)
    # FIXME
#    conan profile update settings.compiler=gcc default
#    conan profile update settings.compiler.version="${COMPILER#gcc-}" default
#    conan profile update settings.compiler.libcxx=libstdc++11 default
    export CXX=g++${COMPILER#gcc} 
    export CC=gcc${COMPILER#gcc}
    ;;
  clang*)
    # TODO
#    conan profile update settings.compiler=clang default
#    conan profile update settings.compiler.version="${COMPILER#clang-}" default
#    conan profile update settings.compiler.libcxx=libstdc++11 default
    export CXX=clang++${COMPILER#clang}
    export CC=clang${COMPILER#clang}
    # initially was only for clang ≥ 7
    # CXXFLAGS="-stdlib=libc++"
    ;;
  apple-clang)
    # conan profile update settings.compiler.libcxx=libstdc++11 default
    # initially was only for clang ≥ 7
    # CXXFLAGS="-stdlib=libc++"
    ;;
  msvc)
    # not defined: use default configuration
    ;;
  *)
    echo "${COMPILER} not supported compiler"
    exit 1
    ;;
esac

"${BASEDIR}"/update_conan_profile.sh build_type Debug "$CONAN_PROFILE_FILE"

mkdir -p build
cd build
# /!\ use profile defined above 
conan install --build=missing ..
cmake \
  -DCMAKE_TOOLCHAIN_FILE=conan_paths.cmake \
  -DCMAKE_BUILD_TYPE="${MODE}" \
  -DENABLE_COVERAGE="${ENABLE_COVERAGE}" \
  -DENABLE_MEMCHECK="${ENABLE_MEMCHECK}" \
  -DENABLE_STATIC_ANALYSIS=${ENABLE_STATIC_ANALYSIS} \
  $(eval echo ${EXTRA_CMAKE_OPTIONS}) \
  ..

cmake --build . --config "${MODE}"