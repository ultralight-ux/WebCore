#!/bin/bash
# Build script
mdkir -p build
cd build
cmake .. -G "Ninja"
ninja
cd ..