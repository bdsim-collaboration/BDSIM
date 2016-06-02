#!/bin/sh

source ./setup.sh

##################################################
# llvm
##################################################
cd $BASEDIR
wget http://llvm.org/releases/3.7.0/llvm-3.7.0.src.tar.xz
unxz llvm-3.7.0.src.tar.xz
mv  llvm-3.7.0.src.tar ./src/llvm-3.7.0.src.tar
mkdir $BASEDIR/build/llvm
tar xf $BASEDIR/src/llvm-3.7.0.src.tar -C ./build/llvm --strip-components=1
mkdir $BASEDIR/build/llvm-build
cd $BASEDIR/build/llvm-build
cmake ../llvm -DCMAKE_INSTALL_PREFIX=$INSTALLDIR/ -DCMAKE_CXX_LINK_FLAGS="-L$INSTALLDIR/lib64 -L$INSTALLDIR/lib -Wl,-rpath,$INSTALLDIR/lib64,-rpath,$INSTALLDIR/lib"
make -j$NCPU
make install
