#!/bin/bash

cmake -B build/dbg/ -S . -DCMAKE_BUILD_TYPE=Debug &&
cmake --build build/dbg/ &&
cp build/dbg/compile_commands.json . &&
./build/dbg/src/photoViewer -i "sandbox/"
