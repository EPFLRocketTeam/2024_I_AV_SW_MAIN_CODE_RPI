#!/bin/bash

# This script is intended to be run from the root of the project to copy
# the libraires installed on the rpi to this computer.

# Configuration
PI_USER=pi
PI_HOST=raspberrypi.local
DEST_DIR="external_rpi_sysroot"

# Rsync paths
declare -a PATHS_TO_COPY=(
  "/usr/include"
  "/usr/local"
  "/usr/lib/aarch64-linux-gnu"
  "/lib/aarch64-linux-gnu"
)

echo "🔄 Syncing Raspberry Pi sysroot from $PI_USER@$PI_HOST..."

for path in "${PATHS_TO_COPY[@]}"; do
    echo "📁 Copying $path..."
    mkdir -p "${DEST_DIR}${path}"  # Ensure target directory exists

    # Add trailing slash to SOURCE path to avoid duplication
    rsync -avz --delete --progress \
      -e ssh \
      "${PI_USER}@${PI_HOST}:${path}/" \
      "${DEST_DIR}${path}"
done

echo "✅ Raspberry Pi sysroot synced at $DEST_DIR"
