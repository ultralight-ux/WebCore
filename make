#!/bin/bash
# Build script
mkdir -p build_deps
cd build_deps
cmake ../Source/GetDeps -G "Ninja"
ninja
cd ..
mkdir -p build
cd build
cmake .. -G "Ninja"
ninja
cd ..
