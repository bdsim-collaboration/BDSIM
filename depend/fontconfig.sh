#!/bin/sh

source ./setup.sh
source ./setup_gcc.sh

##################################################
# freetype
##################################################
cd $BASEDIR
wget https://www.freedesktop.org/software/fontconfig/release/fontconfig-2.11.95.tar.gz
mv fontconfig-2.11.95.tar.gz ./src/
mkdir $BASEDIR/build/fontconfig
tar zxf $BASEDIR/src/fontconfig-2.11.95.tar.gz -C ./build/fontconfig/ --strip-components=1
mkdir $BASEDIR/build/fontconfig-build
cd $BASEDIR/build/fontconfig-build
../fontconfig/configure --prefix=$INSTALLDIR --disable-docs
make -j$NCPU
make install
