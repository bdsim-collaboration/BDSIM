#! /usr/bin/env python2.7

"""
This script executes the bdsimMadx tester.

chmod a+x run_bdsimMadx.py                                     to allow execution

./run_bdsimMadx.py                                             to run default FODO example here
./run_bdsimMadx.py -c                                          to clean produced files
./run_bdsimMadx.py --file="fodo.madx"                          to run another madx file
./run_bdsimMadx.py --file="fodo.madx" --folder="fodofolder"    eg. to run tester in a different folder
./run_bdsimMadx.py --foldername=" " -c                         to clean files if folder is used
./run_bdsimMadx.py --option=value                              to set parameters and run

options: filename, foldername, nparticles, verbose 
"""

import optparse
import pybdsim
import pybdsim.Testing # requires explicit import

def Run(filename,foldername,nparticles,verbose):
    """
    Converts madx lattice to gmad and ptc lattice (also madx fresh) and 
    runs nparticles in each and then compares them. Previous files are 
    removed (clean function) before the simulation and the comparision.
    """
    # creates a test instance
    l = pybdsim.Testing.LatticeTest(filename,nparticles,foldername,verbose=verbose) 

    # clean directory from previous run files
    l.Clean()

    # convert madx tfs into gmad and ptc lattice, run the simulation for each
    l.Run()

    # load and compare the tracking output from both simualtions
    l.Compare(addPrimaries=False)                                

    
def Clean(filename,foldername):
    """
    Clean previous files based on filename and foldername. Removes everything
    apart from the original .madx file.
    """
    l = pybdsim.Testing.LatticeTest(filename,foldername=foldername)
    l.Clean()

def Main():
    usage = ''
    parser = optparse.OptionParser(usage)
    parser.add_option('-c','--clean',     action='store_true',default=False,help=Clean.__doc__)
    parser.add_option('-f','--file',      action='store',     dest="filename",  type="string", default="*.madx")
    parser.add_option('-d','--folder',    action='store',     dest="foldername",type="string", default="./")
    parser.add_option('-n','--nparticles',action='store',     dest="nparticles",type="int",    default=1000)
    parser.add_option('-v','--verbose',   action='store_true',dest="verbose",                  default=False)

    options,args = parser.parse_args()
    
    if options.clean:    
        Clean(options.filename, options.foldername)
    else:
        Run(options.filename, options.foldername, options.nparticles, options.verbose)

if __name__ == "__main__":
    Main()

