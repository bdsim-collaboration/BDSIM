#!/bin/sh
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/agapov/BDSIM/parser
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/nfs/exp_software/local/flc/products/root/lib
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/nfs/exp_software/local/flc/products/geant4.0.8/lib
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/nfs/exp_software/local/flc/products/clhep/1.9.2.2/lib
/home/agapov/geant4/bin/Linux-g++/bdsim --file=/home/agapov/work/ilc-collimation/@INF --output=root --batch --outfile=@OUTF
