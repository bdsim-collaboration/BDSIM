#! /usr/bin/env python2.7

"""
This script executes the bdsimMadx tester.

./run_bdsimMadx.py                                             to run default to run a .madx file in current folder
./run_bdsimMadx.py -c                                          to clean produced files
./run_bdsimMadx.py --filepath=file.madx                        to run another madx file
./run_bdsimMadx.py --filepath=folderpath/file.madx             eg. to run tester in a different folder
./run_bdsimMadx.py --filepath=folderpath/file.madx             to clean files if folder is used
./run_bdsimMadx.py --option1=value1, option2=value2            to set parameters and run

options: filepath, nparticles, verbose 

Notes: 
1. No quotation marks needed for option strings (eg. --filepath="/path/file" -> --filepath=/path/file)
2. In the filepath option ./ is not needed to specify current folder (eg. --filepath=./folder/file -> --filepath=folder/file)
3. Due to MadX convention the output of the simulations will always be produced in the same folder as the input .madx file
"""

import optparse
import pybdsim
import pybdsim.Testing # requires explicit import

def Run(filepath,nparticles,verbose):
    """
    Converts madx lattice to gmad and ptc lattice (also madx fresh) and 
    runs nparticles in each and then compares them. Previous files are 
    removed (clean function) before the simulation and the comparision.
    """
    # creates a test instance
    l = pybdsim.Testing.LatticeTest(filepath,nparticles,verbose=verbose) 

    # clean directory from previous run files
    l.Clean()

    # convert madx tfs into gmad and ptc lattice, run the simulation for each
    l.Run()

    # load and compare the tracking output from both simualtions
    l.Compare(addPrimaries=False)                                

    
def Clean(filepath):
    """
    Clean previous files based on filename and foldername. Removes everything
    apart from the original .madx file.
    """
    l = pybdsim.Testing.LatticeTest(filepath)
    l.Clean()

def Main():
    usage = ''
    parser = optparse.OptionParser(usage)
    parser.add_option('-c','--clean',     action='store_true',default=False,help=Clean.__doc__)
    parser.add_option('-f','--filepath',  action='store',     dest="filepath",  type="string", default="*.madx")
    parser.add_option('-n','--nparticles',action='store',     dest="nparticles",type="int",    default=1000)
    parser.add_option('-v','--verbose',   action='store_true',dest="verbose",                  default=False)

    options,args = parser.parse_args()
    
    if options.clean:    
        Clean(options.filepath)
    else:
        Run(options.filepath, options.nparticles, options.verbose)

if __name__ == "__main__":
    Main()

