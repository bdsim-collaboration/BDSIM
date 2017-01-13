import pylab as _pl
import numpy as _np
import pymadx
import robdsim

def compareAll(madxFile='trackone', bdsimFile='output_0.root') : 
    _pl.figure(1)
    
    _pl.subplot(2,2,1) 
    compare(0)

    _pl.subplot(2,2,2) 
    compare(1)
    
    _pl.subplot(2,2,3) 
    compare(2)

    _pl.subplot(2,2,4) 
    compare(3)

def compare(column = 1, madxFile='trackone', bdsimFile='output_0.root') : 
    m = pymadx.TfsArray(madxFile) 
    r = robdsim.robdsimOutput(bdsimFile) 
    
    md = ((m.GetSegment(5)).dataArray)[:,3:-2]
    print m.GetSegment(5).columns
    rd = robdsim.d2darrayToNumpy(r.GetSamplerData(4))[:-1,:] # have an extra particle in root !

    # fix root units back to metres from micron
    rd[:,0] = rd[:,0]*1e-6
    rd[:,2] = rd[:,2]*1e-6
    
    print _np.shape(md)
    print _np.shape(rd)

    
    _pl.plot(md[:,column],rd[:,column],"+")

    if column == 0 :
        pass
    elif column == 1 : 
        pass
    elif column == 2 :
        pass
    elif column == 3 :
        pass
    elif column == 4 :
        pass
    elif column == 5 :
        pass
    
    # straight line on top
    x = md[:,column]
    y = x 

    _pl.plot(x,y)
