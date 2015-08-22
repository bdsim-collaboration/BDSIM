#! /usr/bin/env python

import pybdsim
import optparse

#Note: currently the Testing folder in pybdsim is not included in pybdsim  __init__ by default.
#This is because in Testing root_numpy package is used, which may not be installed all users.

"""
This script executes the bdsimMadx tester.

chmod a+x run_bdsimMadx.py                                     to allow execturion

./run_bdsimMadx.py                                             to run default FODO example here

./run_bdsimMadx.py -c                                          to clean produced files

./run_bdsimMadx.py --file="foo.madx"                           to run another madx file

./run_bdsimMadx.py --file="foo.madx" --folder="foofolder"      eg. to run tester in a different folder

./run_bdsimMadx.py --foldername=" " -c                         to clean files if folder is used

./run_bdsimMadx.py --option=value                              to set parameters and run

options: filename, foldername, nparticles, verbose 
"""

def Run(filename,foldername,nparticles,verbose):

    l = pybdsim.Testing.LatticeTest(filename,nparticles,foldername,verbose=verbose) #creates a test instance
    
    l.Clean()                                                    #cleans directory from previous run files
    
    l.Run()                                                      #Performs conversions, builds BDSIM and
                                                                 # PTC lattices and runs the simulation

    l.Compare(addPrimaries=False)                                #Performs analysis and comparison of the
                                                                 #simulation data
def Clean(filename,foldername):
    l = pybdsim.Testing.LatticeTest(filename,foldername=foldername)
    l.Clean()

#The Clean() function removes all files produced in the directory except for original madx file


def main():

    usage = ''
    parser = optparse.OptionParser(usage)
    parser.add_option('-c','--clean',action='store_true',default=False)
    parser.add_option('-f','--file=',action='store',dest="filename",type="string", default="FODO.madx")
    parser.add_option('-d','--folder=',action='store',dest="foldername",type="string", default=None)
    parser.add_option('-n','--nparticles=',action='store',dest="nparticles",type="int", default=1000)
    parser.add_option('-v','--verbose=',action='store_true',dest="verbose",default=False)
    
    options,args = parser.parse_args()

    if options.clean:
        Clean(options.filename, options.foldername)
    else:
        Run(options.filename, options.foldername, options.nparticles, options.verbose)

if __name__ == "__main__":
    main()

