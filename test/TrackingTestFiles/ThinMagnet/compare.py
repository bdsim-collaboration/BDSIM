import pylab as _pl
import numpy as _np
import pymadx
import robdsim

def compare(column = 1, madxFile='trackone', bdsimFile='output_0.root') : 
    m = pymadx.TfsArray(madxFile) 
    r = robdsim.robdsimOutput(bdsimFile) 
    
    md = ((m.GetSegment(5)).dataArray)[:,3:-2]
    print m.GetSegment(5).columns
    rd = robdsim.d2darrayToNumpy(r.GetSamplerData(4))[:-1,:] # have an extra particle in root !

    # fix root units back to metres 
    rd[:,0] = rd[:,0]/1000.0/1000.0
    rd[:,2] = rd[:,2]/1000.0/1000.0
    
    print _np.shape(md)
    print _np.shape(rd)

    # plot
    _pl.figure(1)
    _pl.clf()
    
    _pl.plot(md[:,column],rd[:,column],"+")

    # straight line on top
    x = md[:,column]
    y = x 

    _pl.plot(x,y)
