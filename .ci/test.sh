#!/usr/bin/env bash
set -eo pipefail

if [[ "$DEBUG_CI" == "true" ]]; then
  CTEST_FLAGS=--verbose
  set -x
else
  CTEST_FLAGS=--output-on-failure
fi

cd build

ctest -C "${MODE}" ${CTEST_FLAGS}
