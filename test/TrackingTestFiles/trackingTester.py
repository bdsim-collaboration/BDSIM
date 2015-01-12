import pymadx.Ptc
import pymadx.Beam
import pymadx.Builder
import pybdsim.Beam
import pybdsim.Builder 
import os as _os

def test(type_,name="test",particle="e-",energy=1.0,**kwargs) : 
    '''execute test for type
    calls : cleanTest, makeTest, executeTest, analysisTest
    '''
    cleanTest(type_)
    makeTest(type_,name,particle,energy,**kwargs)
    executeTest(type_)
    
def cleanTest(type_) : 
    '''Remove all files associated with test'''

    _os.system("rm -rf "+type_+"*")
    _os.system("rm -rf output*")
    _os.system("rm -rf Maxwellian_bend_for_ptc.txt trackone inrays.madx") 


def makeTest(type_, name, particle, energy,**kwargs) : 
    print 'makeTest.type> ',type_
    print 'makeTest.name> ',name
    for k in kwargs : 
        print 'makeTest.kwargs>',k+'='+str(kwargs[k])

    ptcFileName = 'inrays.madx'

    ptc = pymadx.Ptc.Generator()
    ptc.generate(1000,ptcFileName)

    bb  = pybdsim.Beam(particle,energy,'ptc')
    xb  = pymadx.Beam(particle,energy,'ptc')

    bb.SetDistribFileName(ptcFileName) 
    xb.SetDistribFileName(ptcFileName) 

    bm  = pybdsim.Builder.Machine()
    xm  = pymadx.Builder.Machine()

    bm.AddBeam(bb)
    xm.AddBeam(xb)

    if type_ == 'drift' : 
        bm.AddDrift(name,**kwargs)
        xm.AddDrift(name,**kwargs)
    elif type_ == 'quadrupole' : 
        bm.AddQuadrupole(name,**kwargs)
        xm.AddQuadrupole(name,**kwargs)
    elif type == 'sextupole' : 
        bm.AddSextupole(name,**kwargs)
        xm.AddSextupole(name,**kwargs)        

    bm.AddMarker("theend") # Need a post element marker to sample at, only for bdsim

    bm.AddSampler('all')
    xm.AddSampler('all')
        
    bm.WriteLattice(type_)
    xm.WriteLattice(type_)

def executeTest(type_) : 
    _os.system("madx < "+type_+".madx > madx.log")
    _os.system("bdsim --file="+type_+".gmad --batch --output=root > bdsim.log")


