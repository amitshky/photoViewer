#!/bin/bash

cmake -B build/rel/ -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build/rel/
cp build/rel/compile_commands.json .
./build/rel/src/photoViewer -i "sandbox/"
