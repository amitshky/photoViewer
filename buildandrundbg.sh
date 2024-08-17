#!/bin/bash

set -e

cmake -B build/dbg/ -S . -DCMAKE_BUILD_TYPE=Debug && cmake --build build/dbg/ && ./build/dbg/src/photoViewer
cp build/dbg/compile_commands.json .
