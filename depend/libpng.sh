#!/bin/sh

source ./setup.sh

##################################################
# cmake
##################################################
cd $BASEDIR
wget http://prdownloads.sourceforge.net/libpng/libpng-1.6.21.tar.gz?download
mv libpng-1.6.21.tar.gz?download ./src/libpng-1.6.21.tar.gz
mkdir $BASEDIR/build/libpng
tar zxf $BASEDIR/src/libpng-1.6.21.tar.gz -C ./build/libpng --strip-components=1
mkdir $BASEDIR/build/libpng-build
cd $BASEDIR/build/libpng-build
../libpng/configure --prefix=$INSTALLDIR 
make -j$NCPU
make install
