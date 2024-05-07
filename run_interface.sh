#!/bin/bash

# Directory for the compiled binary
if [ ! -d "./bin/" ]; then
  mkdir bin
fi

# Compile the C++ program
g++ -o bin/chatted main.cpp -std=c++17

# Execute the binary with the provided arguments
./bin/chatted "$@"
