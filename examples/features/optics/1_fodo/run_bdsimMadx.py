import pybdsim

#Note: currently the Testing folder in pybdsim is not included in pybdsim  __init__ by default.
#This is because in Testing root_numpy package is used, which may not be installed all users.

"""
This is an example run of the bdsimMadx tester. Constructor of the tester takes the name of the madx file containing the lattice description and the number of particles to be simulated. 
"""

l = pybdsim.Testing.LatticeTest("FODO.madx",nparticles=1000) #creates a test instance

l.Clean()                                                    #cleans directory from previous run files

l.Run()                                                      #Performs conversions, builds BDSIM and
                                                             # PTC lattices and runs the simulation

l.Compare(addPrimaries=False)                                #Performs analysis and comparison of the
                                                             #simulation data


#Alternatively the CleanRunCompare() function performes the above operations
#The Clean() function removes all files produced in the directory except for original madx file
