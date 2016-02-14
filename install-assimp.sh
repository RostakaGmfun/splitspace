#! /bin/sh

wget https://github.com/assimp/assimp/archive/v3.2.tar.gz
tar zxf v3.2.tar.gz > /dev/null
cd assimp*/
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=../../ -DASSIMP_BUILD_TESTS=false ..
make install -j4
cd ../../
rm -rf assimp*/
rm v3.2.tar.gz

