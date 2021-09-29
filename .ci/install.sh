#!/usr/bin/env bash
set -eo pipefail

if [[ "$DEBUG_CI" == "true" ]]; then
  set -x
fi

if (! command -v conan >/dev/null 2>&1 ); then
  # install conan if not already installed
  pip3 install wheel setuptools
  pip3 install conan
fi
