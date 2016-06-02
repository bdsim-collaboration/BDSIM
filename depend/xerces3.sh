#!/bin/sh

source ./setup.sh

##################################################
# freetype
##################################################
cd $BASEDIR
wget http://mirror.catn.com/pub/apache//xerces/c/3/sources/xerces-c-3.1.3.tar.gz
mv xerces-c-3.1.3.tar.gz ./src/
mkdir $BASEDIR/build/xerces3
tar zxf $BASEDIR/src/xerces-c-3.1.3.tar.gz -C ./build/xerces3/ --strip-components=1
mkdir $BASEDIR/build/xerces3-build
cd $BASEDIR/build/xerces3-build
../xerces3/configure --prefix=$INSTALLDIR --disable-docs
make -j$NCPU
make install
