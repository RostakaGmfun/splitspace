#! /bin/sh

mkdir -p build && cd build && cmake .. && make -j4 && cd ..
./build/test/splitspace-test

