# SD3589CV100 SDK Docker Development Environment
# Reference: SD3589CV100 开发环境用户指南 v1.0.12
# Base image with Ubuntu 22.04 (compatible with Ubuntu 20.04 SDK requirements)
FROM ubuntu:22.04

# Avoid interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Configure an Alibaba Cloud Ubuntu mirror appropriate for the container architecture.
RUN set -eux; \
    arch="$(dpkg --print-architecture)"; \
    if [ "$arch" = "amd64" ] || [ "$arch" = "i386" ]; then \
        repo="http://mirrors.aliyun.com/ubuntu"; \
    else \
        repo="http://mirrors.aliyun.com/ubuntu-ports"; \
    fi; \
    printf '%s\n' \
        "deb $repo jammy main restricted universe multiverse" \
        "deb $repo jammy-updates main restricted universe multiverse" \
        "deb $repo jammy-security main restricted universe multiverse" \
        "deb $repo jammy-backports main restricted universe multiverse" \
        > /etc/apt/sources.list; \
    echo "===== sources.list ====="; \
    cat /etc/apt/sources.list; \
    echo 'Acquire::Retries "10";' > /etc/apt/apt.conf.d/80retries; \
    apt-get update

# ======================================================================
# Step 1: Install ALL system packages required by the SDK (ref: 1.2.3)
# ======================================================================
RUN apt-get update && apt-get install -y \
    # --- Build essentials ---
    make \
    g++ \
    build-essential \
    cmake \
    # --- Cross-compilation libs (32-bit compat) ---
    libc6-i386 \
    lib32z1 \
    lib32stdc++6 \
    # --- Compression & archive ---
    zlib1g-dev \
    liblzo2-dev \
    p7zip-full \
    # --- Terminal / ncurses ---
    libncurses5-dev \
    libncursesw5-dev \
    ncurses-term \
    # --- U-Boot / kernel tools ---
    u-boot-tools \
    # --- Text processing ---
    texinfo \
    gawk \
    # --- SSL / crypto ---
    libssl-dev \
    openssl \
    zip \
    # --- Misc utilities ---
    bc \
    gperf \
    bison \
    flex \
    diffutils \
    git \
    unzip \
    rsync \
    pkg-config \
    uuid-dev \
    # --- autotools / gettext ---
    libffi-dev \
    libtool \
    libfreetype6 \
    automake \
    autoconf \
    autopoint \
    po4a \
    gettext \
    # --- XML ---
    libxml2-dev \
    # --- Python (Step 4) ---
    python3 \
    python-is-python3 \
    python3-pip \
    # --- Convenience tools ---
    wget \
    curl \
    vim \
    tar \
    gzip \
    bzip2 \
    xz-utils \
    && rm -rf /var/lib/apt/lists/*

# ======================================================================
# Step 2: Configure pkgconfig path (ref: 1.2.3 step 3)
# ======================================================================
ENV PKG_CONFIG_PATH="/usr/lib/x86_64-linux-gnu/pkgconfig:${PKG_CONFIG_PATH}"

# ======================================================================
# Step 3: Install Python3 dependencies (ref: 1.2.3 step 4)
# ======================================================================
RUN pip3 install --no-cache-dir \
    wheel==0.36.2 \
    pycryptodome==3.9.8 \
    pyelftools==0.27 \
    cryptography

# ======================================================================
# Step 4: Set working directory
# ======================================================================
WORKDIR /workspace

# ======================================================================
# Step 5: Install cross-compilation toolchain (ref: 1.2.4)
#
# Build context must be the parent directory of lq560v100_sdk
# (where gcc-aarch64-otv02-linux-gnu.tgz lives alongside the SDK).
#
# Example:
#   cd lq560v100_sdk/..
#   docker build -f lq560v100_sdk/docker/Dockerfile -t sd3589cv100-dev:latest .
# ======================================================================
RUN mkdir -p /opt/toolchain
COPY gcc-aarch64-otv02-linux-gnu.tgz /toolchain.tgz
RUN tar -xzf /toolchain.tgz -C /opt/toolchain && \
    rm /toolchain.tgz && \
    touch /opt/toolchain/.installed

# ======================================================================
# Step 6: Use bash as /bin/sh (ref: 1.2.3 step 1)
# SDK Makefiles use bash builtins (pushd/popd). dash is the default /bin/sh on Ubuntu.
# ======================================================================
RUN ln -sf /bin/bash /bin/sh

# ======================================================================
# Step 7: Environment variables for cross compilation
# Note: SDK Makefiles reference aarch64-otv02-linux-gnu- for CROSS_COMPILE.
# The toolchain provides both aarch64-linux-gnu-* (real binaries) and
# aarch64-otv02-linux-gnu-* (symlinks).
# ======================================================================
ENV ARCH=arm64
ENV CROSS_COMPILE=aarch64-linux-gnu-
ENV PATH=/opt/toolchain/gcc-aarch64-otv02-linux-gnu/aarch64-otv02-linux-gnu-gcc/bin:$PATH

# Default command
CMD ["/bin/bash"]
