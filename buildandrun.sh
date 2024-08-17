#!/bin/bash

set -e

cmake -B build/rel/ -S . -DCMAKE_BUILD_TYPE=Release && cmake --build build/rel/ && ./build/rel/src/photoViewer
cp build/rel/compile_commands.json .
