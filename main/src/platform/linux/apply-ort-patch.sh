#!/bin/bash

# Get all patch files matching the pattern and sort them numerically
patches=$(ls *_CP[0-9]*.patch 2>/dev/null | sort -V)

# Check if any patches were found
if [ -z "$patches" ]; then
    echo "No patches found matching pattern *_CP*.patch"
    exit 1
fi

# Apply each patch in order
for patch in $patches; do
    echo "Applying $patch..."
    pushd $1 && \
        patch -p1 < ../"${patch}"; \
    popd
done

