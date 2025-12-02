#!/bin/bash
set -e

# Initialize variables
MICROPYTHON_REPO="https://github.com/micropython/micropython.git"
MICROPYTHON_DIR="micropython_repo"
BOARD="XIAO_STM32G431"
BOARD_DIR=$(pwd)/micropython/boards/$BOARD

# Install dependencies if needed (assuming Ubuntu/Debian environment for CI)
# sudo apt-get install -y build-essential libffi-dev git pkg-config gcc-arm-none-eabi

# Clone MicroPython if not exists
if [ ! -d "$MICROPYTHON_DIR" ]; then
    echo "Cloning MicroPython..."
    git clone $MICROPYTHON_REPO $MICROPYTHON_DIR
    cd $MICROPYTHON_DIR
    git submodule update --init
else
    cd $MICROPYTHON_DIR
    echo "Updating MicroPython..."
    git pull
    git submodule update --init
fi

# Build mpy-cross
echo "Building mpy-cross..."
make -C mpy-cross

# Setup board directory
# We symlink our board directory into the MicroPython boards directory
TARGET_BOARD_DIR="ports/stm32/boards/$BOARD"
if [ ! -L "$TARGET_BOARD_DIR" ]; then
    echo "Symlinking board directory..."
    ln -s "$BOARD_DIR" "$TARGET_BOARD_DIR"
fi

# Build Firmware
echo "Building Firmware for $BOARD..."
cd ports/stm32
make BOARD=$BOARD

# Copy output to artifact dir
mkdir -p ../../../build_artifacts
cp build-$BOARD/firmware.hex ../../../build_artifacts/
cp build-$BOARD/firmware.dfu ../../../build_artifacts/
cp build-$BOARD/firmware.bin ../../../build_artifacts/

echo "Build complete. Artifacts in build_artifacts/"
