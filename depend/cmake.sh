#!/bin/sh

source ./setup.sh

##################################################
# cmake
##################################################
cd $BASEDIR
wget https://cmake.org/files/v3.5/cmake-3.5.2.tar.gz
mv cmake-3.5.2.tar.gz ./src/
mkdir $BASEDIR/build/cmake
tar zxf $BASEDIR/src/cmake-3.5.2.tar.gz -C ./build/cmake --strip-components=1
mkdir $BASEDIR/build/cmake-build
cd $BASEDIR/build/cmake-build
cmake ../cmake -DCMAKE_INSTALL_PREFIX=$INSTALLDIR/ -DCMAKE_CXX_LINK_FLAGS="-L$INSTALLDIR/lib64 -L$INSTALLDIR/lib -Wl,-rpath,$INSTALLDIR/lib64,-rpath,$INSTALLDIR/lib"
make -j$NCPU
make install
