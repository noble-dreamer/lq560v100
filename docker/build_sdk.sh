#!/bin/bash
# SD3589CV100 SDK Full Build Script (Docker-based)
# Usage: ./build_sdk.sh [options]
#
# Options:
#   -h, --help           Show help message
#   -q, --quickboot      Enable quickboot build
#   -s, --secureboot     Enable secureboot build
#   -c, --clean          Run 'make clean' before building
#   --log FILE           Build log file (default: SDK/.build.log)
#
# Note: make -j16 is hardcoded in SDK build.sh
#
# Examples:
#   ./build_sdk.sh                  # default build
#   ./build_sdk.sh -q               # quickboot build
#   ./build_sdk.sh -c -q            # clean then quickboot build

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
SDK_HOST="$(cd "$SCRIPT_DIR/.." && pwd)"
SDK_MOUNT="/workspace/sdk"
IMAGE="sd3589cv100-dev:latest"
CONTAINER_NAME="sd3589cv100-build"

# Default options
BUILD_ARGS=""      # passed through to SDK build.sh (-q, -s)
DO_CLEAN="false"
LOG_FILE="${SDK_HOST}/.build.log"

usage() {
    cat <<EOF
SD3589CV100 SDK Full Build Script (Docker-based)

Usage: $(basename "$0") [options]

Options:
  -h, --help           Show this help message
  -q, --quickboot      Enable quickboot build
  -s, --secureboot     Enable secureboot build
  -c, --clean          Run 'make clean' before building
  --log FILE           Build log file (default: SDK/.build.log)

Note: make -j16 is hardcoded in the SDK build.sh. The build output
      is saved to the log file and also printed to stdout.

Environment:
  SDK_HOST             SDK source path on host (override default)
  IMAGE                Docker image name (override default)
EOF
    exit 0
}

# Parse arguments
while [[ $# -gt 0 ]]; do
    case "$1" in
        -h|--help)
            usage
            ;;
        -q|--quickboot)
            BUILD_ARGS="$BUILD_ARGS -q"
            shift
            ;;
        -s|--secureboot)
            BUILD_ARGS="$BUILD_ARGS -s"
            shift
            ;;
        -c|--clean)
            DO_CLEAN="true"
            shift
            ;;
        --log)
            LOG_FILE="$2"
            shift 2
            ;;
        *)
            echo "Unknown option: $1"
            usage
            ;;
    esac
done

# Pre-flight checks
echo "=============================================="
echo " SD3589CV100 SDK Full Build"
echo "=============================================="
echo " SDK:        $SDK_HOST"
echo " Image:      $IMAGE"
echo " Log:        $LOG_FILE"
echo " Clean:      $DO_CLEAN"
echo " Build args: ${BUILD_ARGS:-(none)}"
echo "=============================================="

if [[ ! -d "$SDK_HOST" ]]; then
    echo "ERROR: SDK directory not found: $SDK_HOST"
    exit 1
fi

if ! docker image inspect "$IMAGE" &>/dev/null; then
    echo "ERROR: Docker image '$IMAGE' not found. Build it first:"
    echo "  cd $SDK_HOST/.."
    echo "  docker build -f $SDK_HOST/docker/Dockerfile -t $IMAGE ."
    exit 1
fi

# Remove any stale container
docker rm -f "$CONTAINER_NAME" 2>/dev/null || true

# Clear old log
> "$LOG_FILE"

echo "Starting build container..."
echo "Log: tail -f $LOG_FILE"
echo ""

# Run the build inside Docker
docker run --rm \
    --name "$CONTAINER_NAME" \
    -v "${SDK_HOST}:${SDK_MOUNT}" \
    "$IMAGE" \
    /bin/bash -c "
        set -e

        echo '=== Toolchain ==='
        aarch64-otv02-linux-gnu-gcc --version | head -1

        cd ${SDK_MOUNT}

        if [[ '$DO_CLEAN' == 'true' ]]; then
            echo '=== make clean ==='
            make clean 2>&1 || true
        fi

        echo '=== Starting SDK build ==='
        bash ./build.sh ${BUILD_ARGS}
    " 2>&1 | tee "$LOG_FILE"

BUILD_EXIT=${PIPESTATUS[0]}
echo ""
echo "=============================================="
if [[ $BUILD_EXIT -eq 0 ]]; then
    echo " BUILD SUCCESS"
else
    echo " BUILD FAILED (exit code: $BUILD_EXIT)"
fi
echo " Log: $LOG_FILE"
echo "=============================================="

exit $BUILD_EXIT
