#!/bin/sh

source ./setup.sh
# check if these directories exist
mkdir $INSTALLDIR
mkdir $SRCDIR
mkdir $BUILDDIR

#./gcc.sh
#./cmake.sh
#./llvm.sh
#./clang.sh
#./python.sh
./libpng.sh
./freetype.sh
./fontconfig.sh
./xml2.sh
./xerces3.sh
# ./zlib.sh ! Not sure about this
./root.sh
./clhep.sh
./geant4.sh
./bdsim.sh
