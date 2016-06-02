#!/bin/sh

source ./setup.sh
source ./setup_gcc.sh

##################################################
# cmake
##################################################
cd $BASEDIR
wget http://zlib.net/zlib-1.2.8.tar.gz
mv zlib-1.2.8.tar.gz ./src/
mkdir $BASEDIR/build/zlib
tar zxf $BASEDIR/src/zlib-1.2.8.tar.gz -C ./build/zlib --strip-components=1
cd $BASEDIR/build/zlib
../zlib/configure --prefix=$INSTALLDIR 
make -j$NCPU
make install
