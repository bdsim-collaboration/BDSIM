#!/bin/sh

source ./setup.sh
source ./setup_gcc.sh

##################################################
# python
##################################################
cd $BASEDIR
wget https://www.python.org/ftp/python/2.7.11/Python-2.7.11.tgz
mv Python-2.7.11.tgz ./src/
mkdir $BASEDIR/build/python
tar zxf $BASEDIR/src/Python-2.7.11.tgz -C ./build/python --strip-components=1
mkdir $BASEDIR/build/python-build
cd $BASEDIR/build/python-build
../python/configure --prefix=$INSTALLDIR --enable-unicode=ucs4 --enable-shared LDFLAGS="-L$INSTALLDIR/lib64/ -L$INSTALLDIR/lib/ -Wl,-rpath,$INSTALLDIR/lib64,-rpath,$INSTALLDIR/lib"
make -j$NCPU
make install
