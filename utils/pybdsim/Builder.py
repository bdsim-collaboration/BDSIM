# pybdsim.Builder - tools to build bdsim lattices
# Version 1.0
# L. Nevay, S.T.Boogert
# laurie.nevay@rhul.ac.uk

"""
Builder

Build generic machines for bdsim. You can create a lattice
using one of the predefined simple lattices or by adding
many pieces together of your own design. Finally, output
the gmad files required.

"""

import pylab as pl 

class MachineRep(list):
    def __init__(self,type=None) : 
        list()
       
    def writeLattice() :
        pass

class Machine:
    """
    Machine()

    Generic machine class. Allows you to create your own 
    lattice easily either by adding several components or
    choosing to use a premade builder.


    """
    def __init__(self): 
        self.line = []
       
    def WriteLattice(self,filename):
        if filename[-4:] != '.gmad':
            filename += '.gmad'
        f = open(filename,"w")

        # write components
        for e in line : 
            wl = ''
            if e[1] == 'sbend' : 
                wl = '{0} : {1}, l={2}, angle={3};\n'.format(e[0],e[1],e[2],e[3])
            elif e[1] == 'drift' : 
                wl = '{0} : {1}, l={2};\n'.format(e[0],e[1],e[2])         
            elif e[1] == 'quadrupole' : 
                wl = '{0} : {1}, l={2}, k={3};\n'.format(e[0],e[1],e[2],e[3])
            f.write(wl)

        # write line  
        l = ''
        last = False 
        for e in line :
            if e == line[-1] : 
                last = True

        l += e[0]
        if not last : 
            l += ','

        l = 'lat : line = ('+l+');\n'
    
        f.write(l)
        
        f.close()
           
    def AddDipole(self):
        pass

    def AddQuadrupole(self):
        pass

    def AddFodoCell(self):
        pass

    def AddFodoCells(self):
        pass

    def CreateDipoleRing(self):
        pass

    def CreateDipoleFodoRing(self):
        pass

    def CreateFodoLine(self):
        pass


def DipoleRing(ndipole = 60, ldipole = 1.0, clength = 10.0, cell = []) : 
    """
    Build a ring composed of dipoles and drifts. Ideal for general
    testing purposes.
    
    Uses sector bend magnets - 'sbend'

    ndipole : number of dipole magnets
    ldipole : length of dipole 
    clength : length of other components
    cell    : components and specificiations of each dipole cell

    Note, a high number of elements is suggested. The default here
    is 60. With a low number, the vacuum dipole stepper will be
    perfectly correct, but the chord through the magnet may
    intercept the beam pipe in stepping through and the tracking
    will be affected or stop.

    Also, note that with very closely spaced elements, geometry
    errors may occur.

    Both of these faults are under revision Dec 2013.
    
    """

    line = []
    
    for i in range(0,ndipole,1) : 
        name = 'dipole.'+str(i)
        type = 'sbend' 
        angl = 2*pl.pi/ndipole
        leng = ldipole         
        line.append([name,type,leng,angl])

        name = 'drift.'+str(i)
        type = 'drift' 
        leng = clength-ldipole
        line.append([name,type,leng])
        
#        name = 'quadrupole.'+str(i)
#        line.append([name,'quadrupole',0.5,0.2])

    totalbl = 0.0
    totall  = 0.0 
    for e in line : 
        # total bending length
        if e[1] == 'sbend' : 
            totalbl += e[2]
            
        # sum all component lengths
        totall += e[2]

        print e

    print 'RingBuilder> total bength length',totalbl
    print 'RingBuilder> total length',totall


    f = open("RandomRing.gmad","w")

    # write components
    for e in line : 
        wl = ''
        if e[1] == 'sbend' : 
            wl = '{0} : {1}, l={2}, angle={3};\n'.format(e[0],e[1],e[2],e[3])
        elif e[1] == 'drift' : 
            wl = '{0} : {1}, l={2};\n'.format(e[0],e[1],e[2])         
        elif e[1] == 'quadrupole' : 
            wl = '{0} : {1}, l={2}, k={3};\n'.format(e[0],e[1],e[2],e[3])
        f.write(wl)

    # write line  
    l = ''
    last = False 
    for e in line :
        if e == line[-1] : 
            last = True

        l += e[0]
        if not last : 
            l += ','

    l = 'lat : line = ('+l+');\n'
    
    f.write(l)
        
    f.close()
