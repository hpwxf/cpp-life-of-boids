#!/usr/bin/env bash
set -eo pipefail

if [[ "$DEBUG_CI" == "true" ]]; then
  set -x
fi

if [ -z "$3" ]; then
        echo "Usage: $0 <option_name> <option_value> <profile_path>"
        exit 1
fi

OPTION_NAME=$1
OPTION_VALUE=$2
PROFILE_PATH=$3

update_config() {
    local temp_file="${PROFILE_PATH}.tmp"
    grep -v "^${OPTION_NAME}=" "$PROFILE_PATH" > "$temp_file" || true
    echo "${OPTION_NAME}=${OPTION_VALUE}" >> "$temp_file"
    mv "$temp_file" "$PROFILE_PATH"
}

case $(uname -s) in
  Linux)
    sed -i "s/^${OPTION_NAME}=.*/${OPTION_NAME}=${OPTION_VALUE}/" "${PROFILE_PATH}"
    ;;
  Darwin)
    sed -i '' "s/^${OPTION_NAME}=.*/${OPTION_NAME}=${OPTION_VALUE}/" "${PROFILE_PATH}"
    ;;
  MSYS_NT*|MINGW64_NT*)
    # using sed on Windows with -i generate an error
    #   sed: cannot rename ./sedog7N2I: Invalid cross-device link
    sed "s/^${OPTION_NAME}=.*/${OPTION_NAME}=${OPTION_VALUE}/" "${PROFILE_PATH}" > "${PROFILE_PATH}.tmp"
    mv "${PROFILE_PATH}.tmp" "${PROFILE_PATH}" # this order to manage properly failures on sed
    ;;
  *)
    echo "Unknown OS [$ARCH]"
    exit 1
    ;;
esac