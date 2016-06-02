BASEDIR=`pwd`
echo $BASEDIR
INSTALLDIR=$BASEDIR/install
SRCDIR=$BASEDIR/src
BUILDDIR=$BASEDIR/build
NCPU=20
echo $INSTALLDIR
echo $SRCDIR
echo $BUILDDIR

##################################################
# GCC
##################################################
cd $BASEDIR
wget http://nl.mirror.babylon.network/gcc/releases/gcc-4.9.3/gcc-4.9.3.tar.gz
mv gcc-4.9.3.tar.gz ./src/
mkdir $BASEDIR/build/gcc
tar zfx $BASEDIR/src/gcc-4.9.3.tar.gz -C ./build/gcc --strip-components=1
cd $BASEDIR/build/gcc
./contrib/download_prerequisites

mkdir $BASEDIR/build/gcc-build
cd $BASEDIR/build/gcc-build/
../gcc/configure --prefix=$INSTALLDIR --enable-languages=c,c++,fortran,go
make -j$NCPU 
make -j$NCPU install

