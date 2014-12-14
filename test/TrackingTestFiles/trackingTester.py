import pymadx.Builder
import pybdsim.Builder 

def makeTest(type, name, **kwargs) : 
    print 'makeTest.type> ',type
    print 'makeTest.name> ',name
    for k in kwargs : 
        print 'makeTest.kwargs>',k+'='+str(kwargs[k])

    bb = pybdsim.Builder.Machine()
    xb = pymadx.Builder.Machine()

    if type == 'drift' : 
        bb.AddDrift(name,kwargs['l'])
        xb.AddDrift(name,kwargs['l'])

    bb.AddSampler('all')
    xb.AddSampler('all')
        
    bb.WriteLattice(type)
    xb.WriteLattice(type)
