#! /bin/sh

mkdir -p build && cd build && cmake .. && make splitspace -j4 && make -j4 && cd ..
./build/test/splitspace-test

