#!/usr/bin/env bash
set -euo pipefail

# install-limine.sh
# Download Limine v12.3.3 binary release and extract limine-cd.bin and limine.sys into ./limine/

LIMINE_VER="v12.3.3"
ASSET_URL="https://github.com/Limine-Bootloader/Limine/releases/download/${LIMINE_VER}/limine-binary.zip"
TMP_ZIP="/tmp/limine-binary-${LIMINE_VER}.zip"

echo "Downloading Limine ${LIMINE_VER} binary bundle..."

if command -v curl >/dev/null 2>&1; then
  curl -L -o "${TMP_ZIP}" "${ASSET_URL}"
elif command -v wget >/dev/null 2>&1; then
  wget -O "${TMP_ZIP}" "${ASSET_URL}"
else
  echo "Error: neither curl nor wget available. Install one and retry." >&2
  exit 1
fi

mkdir -p limine

echo "Extracting limine-cd.bin and limine.sys into ./limine/ ..."
if command -v unzip >/dev/null 2>&1; then
  # Extract specific files if present
  unzip -j "${TMP_ZIP}" "*limine-cd.bin" -d limine 2>/dev/null || true
  unzip -j "${TMP_ZIP}" "*limine.sys" -d limine 2>/dev/null || true
else
  echo "Error: unzip not found. Install unzip and re-run this script." >&2
  exit 1
fi

if [ -f limine/limine-cd.bin ] && [ -f limine/limine.sys ]; then
  echo "Limine artifacts installed into ./limine/"
  ls -lh limine/limine-cd.bin limine/limine.sys
else
  echo "Warning: expected limine-cd.bin and limine.sys not found in the archive. Check the asset contents and extract manually." >&2
  exit 2
fi

# Clean up
rm -f "${TMP_ZIP}"
