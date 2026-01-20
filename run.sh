#!/bin/bash

# Create build directory and configure cmake if `build` does not exist.
if [[ ! -d build ]]; then
    cmake -B build
fi

# Build project.
cmake --build build

# Run application.
./build/bin/assignment1

# Status code, info dump, and exit.
#echo "Program exited with code $?.  Output written to 'output' directory."

