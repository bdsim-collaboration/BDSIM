#!/bin/sh

source ./setup.sh

##################################################
# freetype
##################################################
cd $BASEDIR
wget http://download.savannah.gnu.org/releases/freetype/freetype-2.6.3.tar.gz
mv freetype-2.6.3.tar.gz ./src/
mkdir $BASEDIR/build/freetype
tar zxf $BASEDIR/src/freetype-2.6.3.tar.gz -C ./build/freetype --strip-components=1
mkdir $BASEDIR/build/freetype-build
cd $BASEDIR/build/freetype-build
../freetype/configure --prefix=$INSTALLDIR
make -j$NCPU
make install
