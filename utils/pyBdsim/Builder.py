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

Classes:
Element - beam line element that always has name,type and length
Machine - a list of elements

"""

import _General
from   decimal import Decimal
import math
import time

class Element(dict):
    """
    Element - a beam element class - inherits dict

    Element(name,type,length,**kwargs)
    
    A beam line element must ALWAYs have a name, type and length
    the keyword arguments are specific to the type and are up to
    the user to specify.

    Length uses python Decimal type to provide higher accuracy in
    the representation of numbers - 15 decimal places are used.
    This is suggested for other numbers too - such as angle. 

    """
    def __init__(self, name, category, length, **kwargs):
        dict.__init__(self)
        self.name        = str(name)
        self.category    = str(category)
        self.length      = Decimal(str(length))
        self['name']     = self.name
        self['category'] = self.category
        self['length']   = self.length
        self._keysextra = []
        for key,value in kwargs.iteritems():
            if type(value) != str:
                self[key] = Decimal(str(value))
                setattr(self,key,Decimal(str(value)))
            else:
                self[key] = value
                setattr(self,key,value)
            self._keysextra.append(str(key))
    
    def keysextra(self):
        #so behaviour is similar to dict.keys()
        return self._keysextra

class Machine(list):
    """
    Machine()

    Generic machine class. Allows you to create your own 
    lattice easily either by adding several components or
    choosing to use a premade builder.

    instance.line is a list of beam line elements

    each element is a list of [name,type,length...other parameters]

    """
    def __init__(self):
        list.__init__(self)
        self.nelements     = int(0)
        self.samplers      = []
        self.totallength   = Decimal(str(0.0))
        self._elementindex = int(0)
        self._maxindexpow  = 5 

    def append(self, object):
        if type(object) == Element:
            object.name = object.name + '_' + str(self._elementindex).zfill(self._maxindexpow)
            object['name'] = object['name'] + '_' + str(self._elementindex).zfill(self._maxindexpow)
        list.append(self,object)
        self.nelements     += 1
        self.totallength   += object.length
        self._elementindex += 1
       
    def WriteLattice(self, filename):
        #check filename
        if filename[-5:] != '.gmad':
            filename += '.gmad'
        #check if file already exists
        filename = _General.CheckFileExists(filename)
        f = open(filename,"w")

        # write some comments
        f.write('! ' + time.strftime("%a, %d %b %Y %H:%M:%S +0000", time.gmtime()) + '\n')
        f.write('! pybdsim.Builder Lattice \n')
        f.write('! number of elements = ' + str(self.nelements) + '\n')
        f.write('! total length       = ' + str(self.totallength) + ' m\n\n')
        f.write('! COMPONENT DEFINITION\n\n')

        # write components
        elementnames = []
        for e in self:
            if e.category == 'marker':
                linetowrite = e.name + ' : ' + e.category
            else:
                linetowrite = e.name + ' : ' + e.category+', l=' + format(e.length,'.15g') + '*m'
            for parameter in  e.keysextra():
                linetowrite = linetowrite + ', ' + str(parameter) + '=' + format(e[parameter],'.15g')
            linetowrite = linetowrite + ';\n'
            f.write(linetowrite)
            elementnames.append(e.name)

        # write lattice lines
        linelist = []
        ti = 0
        for ministring in _General.Chunks(elementnames,100):
            stw2 = 'l'+str(ti)+': line = ('+', '.join(ministring)+');\n'
            f.write(stw2)
            linelist.append('l'+str(ti))
            ti += 1
        
        f.write('\n! LATTICE DEFINITION\n\n')
        # need to define the period before making sampler planes
        f.write('lattice: line = ('+', '.join(linelist)+');\n')
        f.write('use, period=lattice;\n') 
        # now do the sampler planes

        f.write('\n! SAMPLER DEFINITION\n\n')
        for s in self.samplers:
            f.write('sample, range=' + str(s) + ';\n')
        
        f.close()
        print 'Machine lattice written to',filename
 
    def AddMarker(self,name='mk'):
        self.append(Element(name,'marker',0.0))
    
    def AddDrift(self,name='dr',length=0.1,**kwargs):
        self.append(Element(name,'drift',length,**kwargs))
          
    def AddDipole(self,name='dp',category='sbend',length=0.1,angle=None,b=None,**kwargs):
        """
        AddDipole(category='sbend')

        category - 'sbend' or 'rbend' - sector or rectangular bend

        """
        if (angle==None) and (b==None):
            raise TypeError('angle or b must be specified for an sbend')
        elif angle != None:
            self.append(Element(name,category,length,angle=angle,**kwargs))
        else:
            self.append(Element(name,category,length,B=b,**kwargs))

    def AddQuadrupole(self,name='qd',length=0.1,k1=0.0,**kwargs):
        self.append(Element(name,'quadrupole',length,k1=k1,**kwargs))
        
    def AddSextupole(self,name='sx',length=0.1,k2=0.0,**kwargs):
        self.append(Element(name,'sextupole',length,k2=k2,**kwargs))

    def AddOctupole(self,name='oc',length=0.1,k3=0.0,**kwargs):
        self.append(Element(name,'octupole',length,k3=k3,**kwargs))

    def AddMultipole(self,name='mp',length=0.1,knl=(0),ksl=(0),**kwargs):
        pass

    def AddRF(self,name='arreff',length=0.1,gradient=10,**kwargs):
        """
        AddRF(name,length,graident,**kwargs)
        
        length in metres
        gradient in MV / m
        """
        self.append(Element(name,'rf',length,gradient=gradient,**kwargs))
        
    def AddRCol(self,name='rc',length=0.1,xsize=0.1,ysize=0.1,**kwargs):
        self.append(Element(name,'rcol',length,xsize=xsize,ysize=ysize,**kwargs))

    def AddECol(self,name='ec',length=0.1,xsize=0.1,ysize=0.1,**kwargs):
        self.append(Element(name,'ecol',length,xsize=xsize,ysize=ysize,**kwargs))

    def AddFodoCell(self,basename='fodo',magnetlength=1.0,driftlength=4.0,kabs=1.0,**kwargs):
        """
        AddFodoCell(basename,magnetlength,driftlength,kabs,**kwargs)
        basename     - the basename for the fodo cell beam line elements
        magnetlength - length of magnets in metres
        driftlength  - length of drift segment in metres
        kabs         - the absolute value of the quadrupole strength - alternates between magnets

        **kwargs are other parameters for bdsim - ie material='W'
        """
        self.append(Element(basename+'_qfa','quadrupole',magnetlength/2.0,k1=kabs,**kwargs))
        self.append(Element(basename,'drift',driftlength))
        self.append(Element(basename+'_qd','quadrupole',magnetlength,k1=-1.0*kabs,**kwargs))
        self.append(Element(basename,'drift',driftlength))
        self.append(Element(basename+'_qfb','quadrupole',magnetlength/2.0,k1=kabs,**kwargs))

    def AddFodoCellSplitDrift(self,basename='fodo',magnetlength=1.0,driftlength=4.0,kabs=1.0,nsplits=10,**kwargs):
        """
        AddFodoCellSplitDrift(basename,magnetlength,driftlength,kabs,nsplits,**kwargs)
        basename - the basename for the fodo cell beam line elements
        magnetlength - length of magnets in metres
        driftlength  - length of drift segment in metres
        kabs         - the absolute value of the quadrupole strength - alternates between magnets
        nsplits      - number of segments drift length is split into 

        Will add qf quadrupole of strength +kabs, then drift of l=driftlength split into 
        nsplit segments followed by a qd quadrupole of strength -kabs and the same pattern
        of drift segments.
        
        nsplits will be cast to an even integer for symmetry purposes.

        **kwargs are other parameters for bdsim - ie aper=0.2
        """
        nsplits = _General.NearestEvenInteger(nsplits)
        splitdriftlength = driftlength / float(nsplits)
        maxn    = int(len(str(nsplits)))
        self.append(Element(basename+'_qfa','quadrupole',magnetlength/2.0,k1=kabs,**kwargs))
        for i in range(nsplits):
            self.append(Element(basename+'_d'+str(i).zfill(maxn),'drift',splitdriftlength))
        self.append(Element(basename+'_qd','quadrupole',magnetlength,k1=-1.0*kabs,**kwargs))
        for i in range(nsplits):
            self.append(Element(basename+'_d'+str(i).zfill(maxn),'drift',splitdriftlength))
        self.append(Element(basename+'_qfb','quadrupole',magnetlength/2.0,k1=kabs,**kwargs))

    def AddFodoCellMultiple(self,basename='fodo',magnetlength=1.0,driftlength=4.0,kabs=1.0,ncells=2,**kwargs):
        ncells = int(ncells)
        maxn   = int(len(str(ncells)))
        for i in range(ncells):
            cellname = basename+'_'+str(i).zfill(maxn)
            self.AddFodoCell(cellname,magnetlength,driftlength,kabs,**kwargs)

    def AddFodoCellSplitDriftMultiple(self,basename='fodo',magnetlength=1.0,driftlength=4.0,kabs=1.0,nsplits=10,ncells=2,**kwargs):
        ncells = int(ncells)
        maxn   = int(len(str(ncells)))
        for i in range(ncells):
            cellname = basename+'_'+str(i).zfill(maxn)
            self.AddFodoCellSplitDrift(cellname,magnetlength,driftlength,kabs,nsplits=10,**kwargs)
            
    def SetSamplers(self,command='first'):
        """
        SetSamplers(command)
        command is a string and one of:
        first - only the first element in the lattice
        last  - only the last element in the lattice
        all   - all elementss
        category - only elements of that category
        """
        if command == 'first':
            self.samplers.append(self[0].name)
        elif command == 'last':
            self.samplers.append(self[-1].name)
        elif command == 'all':
            for e in self:
                self.samplers.append(e.name)
        else:
            #assume it's a category
            #if it's not, it won't match so tolerant of faulty commands
            for e in self:
                if e.category == command:
                    self.samplers.append(e.name)

def CreateDipoleRing(filename, ndipole=60, dlength=1.0, clength=10.0, samplers='first'):
    ndipole = int(ndipole)
    if dlength > (0.9*clength):
        raise Warning("Dipole length > 90% of cell length - geometry errors may occur")
    a = Machine()
    dangle = Decimal(str(2.0*math.pi / float(ndipole)))
    driftlength = clength - dlength
    a.AddDipole(length=dlength/2.0, angle=dangle/Decimal(2))
    a.AddDrift(length=driftlength)
    for i in range(1,ndipole,1):
        a.AddDipole(length=dlength, angle=dangle)
        a.AddDrift(length=driftlength)
    a.AddDipole(length=dlength/2.0, angle=dangle/Decimal(2))
    a.SetSamplers(samplers)
    a.WriteLattice(filename)

def CreateDipoleFodoRing(filename, ncells=60, circumference=200.0, samplers='first'):
    a       = Machine()
    cangle  = Decimal(str(2.0*math.pi / ncells))
    clength = Decimal(str(float(circumference) / ncells))
    #dipole = 0.7 of cell, quads=0.2, drift=0.1, two dipoles
    #dipole:
    dl  = clength * Decimal(str(0.7)) * Decimal(str(0.5))
    da  = cangle/Decimal(2.0)
    #quadrupole:
    ql  = clength * Decimal('0.2') * Decimal('0.5')
    k1  = SuggestFodoK(ql,dl)
    #drift:
    drl = clength * Decimal('0.1') * Decimal('0.25')
    #naming
    nplaces  = len(str(ncells))
    basename = 'dfodo_'
    for i in range(ncells):
        cellname = basename + str(i).zfill(nplaces)
        a.AddQuadrupole(cellname+'_qd_a',ql/Decimal('2.0'),k1)
        a.AddDrift(cellname+'_dr_a',drl)
        a.AddDipole(cellname+'_dp_a','sbend',dl,da)
        a.AddDrift(cellname+'_dr_b',drl)
        a.AddQuadrupole(cellname+'_qf_b',ql,k1*Decimal('-1.0'))
        a.AddDrift(cellname+'_dr_c',drl)
        a.AddDipole(cellname+'_dp_b','sbend',dl,da)
        a.AddDrift(cellname+'_dr_d',drl)
        a.AddQuadrupole(cellname+'_qd_c',ql/Decimal('2.0'),k1)
    a.SetSamplers(samplers)
    a.WriteLattice(filename)
    
def CreateFodoLine(filename, ncells=10, driftlength=4.0, magnetlength=1.0, samplers='all',**kwargs):
    ncells = int(ncells)
    a      = Machine()
    k1     = SuggestFodoK(magnetlength,driftlength)
    a.AddFodoCellSplitDriftMultiple(magnetlength=magnetlength,driftlength=driftlength,kabs=k1,nsplits=10,ncells=ncells,**kwargs)
    a.SetSamplers(samplers)
    a.WriteLattice(filename)

def SuggestFodoK(magnetlength,driftlength):
                return Decimal(1) / (Decimal(str(magnetlength))*(Decimal(str(magnetlength)) + Decimal(str(driftlength))))
