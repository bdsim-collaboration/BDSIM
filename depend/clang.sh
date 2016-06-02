#!/bin/sh

source ./setup.sh

##################################################
# cmake
##################################################
cd $BASEDIR
wget http://llvm.org/releases/3.7.0/cfe-3.7.0.src.tar.xz
unxz cfe-3.7.0.src.tar.xz
mv  cfe-3.7.0.src.tar ./src/cfe-3.7.0.src.tar
mkdir $BASEDIR/build/cfe
tar xf $BASEDIR/src/cfe-3.7.0.src.tar -C ./build/cfe --strip-components=1
mkdir $BASEDIR/build/cfe-build
cd $BASEDIR/build/cfe-build
cmake ../cfe -DCMAKE_INSTALL_PREFIX=$INSTALLDIR/ -DCMAKE_CXX_LINK_FLAGS="-L$INSTALLDIR/lib64 -L$INSTALLDIR/lib -Wl,-rpath,$INSTALLDIR/lib64,-rpath,$INSTALLDIR/lib"
make -j$NCPU
make install
