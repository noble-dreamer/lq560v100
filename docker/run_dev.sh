#!/bin/bash
# SD3589CV100 SDK Development Environment Launcher

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
SDK_HOST="$(cd "$SCRIPT_DIR/.." && pwd)"

# Container name
CONTAINER_NAME="sd3589cv100-dev"

# Image name
IMAGE_NAME="sd3589cv100-dev:latest"

# Remove existing container if it exists
if docker ps -a --format '{{.Names}}' | grep -w "^${CONTAINER_NAME}$" > /dev/null; then
    echo "Removing existing container..."
    docker rm -f $CONTAINER_NAME
fi

# Run the development container with SDK mounted
echo "Starting SD3589CV100 development environment..."
echo "SDK: $SDK_HOST"
echo ""

# Detect if running interactively
if [ -t 0 ]; then
    DOCKER_OPTS="-it"
else
    DOCKER_OPTS="-i"
fi

docker run $DOCKER_OPTS --rm \
    --name $CONTAINER_NAME \
    --hostname sd3589cv100-dev \
    -v "$SDK_HOST:/workspace/sdk" \
    $IMAGE_NAME /bin/bash -c "
        echo 'Toolchain installed at /opt/toolchain'
        echo 'SDK mounted at /workspace/sdk'
        echo ''
        echo 'SDK structure:'
        ls -la /workspace/sdk/
        echo ''
        echo 'Cross compiler version:'
        aarch64-linux-gnu-gcc --version
        echo ''
        echo 'Environment variables:'
        echo \"ARCH=\$ARCH\"
        echo \"CROSS_COMPILE=\$CROSS_COMPILE\"
        echo \"PATH=\$PATH\"
    "
