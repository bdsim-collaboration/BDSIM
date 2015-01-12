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


def makeTest(type_='drift', foldername=None, particle='e-', energy=1.0,**kwargs) : 
    print 'makeTest.type>   ',type_
    print 'makeTest.folder> ',foldername
    for k in kwargs : 
        print 'makeTest.kwargs> ',k+'='+str(kwargs[k])

    ptcFileName = 'inrays.madx'
    if foldername != None:
        ptcPathName = foldername+'/'+ptcFileName
    else:
        ptcPathName = ptcFileName

    ptc = pymadx.Ptc.Generator()
    ptc.Generate(1000,ptcPathName)

    bb  = pybdsim.Beam(particle,energy,'ptc')
    xb  = pymadx.Beam(particle,energy,'ptc')

    bb.SetDistribFileName(ptcFileName) 
    xb.SetDistribFileName(ptcFileName) 

    bm  = pybdsim.Builder.Machine()
    xm  = pymadx.Builder.Machine()

    bm.AddBeam(bb)
    xm.AddBeam(xb)

    if type_ == 'drift' :
        name = 'd1'
        bm.AddDrift(name,**kwargs)
        xm.AddDrift(name,**kwargs)
    elif type_ == 'quadrupole' :
        name = 'q1'
        bm.AddQuadrupole(name,**kwargs)
        xm.AddQuadrupole(name,**kwargs)
    elif type == 'sextupole' :
        name = 's1'
        bm.AddSextupole(name,**kwargs)
        xm.AddSextupole(name,**kwargs)        

    bm.AddMarker("theend") # Need a post element marker to sample at, only for bdsim

    bm.AddSampler('all')
    xm.AddSampler('all')

    if foldername != None:
        outputname = foldername+'/'+type_
    else:
        outputname = type_
    bm.WriteLattice(outputname)
    xm.WriteLattice(outputname)

def executeTest(type_) : 
    _os.system("madx < "+type_+".madx > madx.log")
    _os.system("bdsim --file="+type_+".gmad --batch --output=root > bdsim.log")


