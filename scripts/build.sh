#!/bin/sh -e
# scripts/build.sh

BASEDIR=$(realpath "$(dirname "$0")")
ROOTDIR=$(realpath "$BASEDIR/..")

echo "Configuring project with CMake..."
cmake -S "$ROOTDIR" -B "$ROOTDIR/build"

echo "Building project..."
cmake --build "$ROOTDIR/build" --parallel

echo "Build completed successfully."
