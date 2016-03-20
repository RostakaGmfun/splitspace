#! /bin/sh

mkdir -p lib
mkdir -p include/SOIL
wget http://www.lonesock.net/files/soil.zip
unzip soil.zip > /dev/null
cd Simple*Library/
cp projects/makefile/alternate\ Makefile.txt src/makefile
cd src
mkdir -p obj
make > /dev/null
cp libSOIL.a ../../lib/
cp SOIL.h ../../include/SOIL
cd ../..
rm -rf Simple*Library
rm soil.zip
