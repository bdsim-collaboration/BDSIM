import pymadx.Ptc
import pymadx.Beam
import pymadx.Builder
import pybdsim.Beam
import pybdsim.Builder 

def makeTest(type, name, particle, energy,**kwargs) : 
    print 'makeTest.type> ',type
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

    if type == 'drift' : 
        bm.AddDrift(name,kwargs['l'])
        xm.AddDrift(name,kwargs['l'])
    elif type == 'quadrupole' : 
        bm.AddQuadrupole(name,length=kwargs['length'],k1=kwargs['k1'])
        xm.AddQuadrupole(name,length=kwargs['length'],k1=kwargs['k1'])

    bm.AddSampler('all')
    xm.AddSampler('all')
        
    bm.WriteLattice(type)
    xm.WriteLattice(type)
