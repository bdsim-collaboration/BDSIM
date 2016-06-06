#!/bin/sh

source ./setup.sh
source ./setup_gcc.sh

##################################################
# freetype
##################################################
cd $BASEDIR
wget ftp://xmlsoft.org/libxml2//libxml2-2.9.4.tar.gz
mv libxml2-2.9.4.tar.gz ./src/
mkdir $BASEDIR/build/libxml2
tar zxf $BASEDIR/src/libxml2-2.9.4.tar.gz -C ./build/libxml2/ --strip-components=1
mkdir $BASEDIR/build/libxml2-build
cd $BASEDIR/build/libxml2-build
../libxml2/configure --prefix=$INSTALLDIR --disable-docs
make -j$NCPU
make install
