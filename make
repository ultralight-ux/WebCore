#!/bin/bash
# Build script
mkdir -p build
cd build
cmake .. -G "Ninja"
ninja
cd ..