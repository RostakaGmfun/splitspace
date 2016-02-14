#! /bin/sh

wget https://github.com/g-truc/glm/releases/download/0.9.5.4/glm-0.9.5.4.zip
unzip glm*.zip > /dev/null
cd glm*
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=../../ ..
make install
cd ../../
rm -rf glm*/
rm glm*.zip

