#!/usr/bin/python2.7

#import pypoisson
import matplotlib.pyplot as _plt
import numpy as _np
import itertools as _it


def Compare():

    PlotQueried("nearest")
    PlotQueried("linear")
    PlotQueried("cubic")
    PlotQueried("nearest_zoom")
    PlotQueried("linear_zoom")
    PlotQueried("cubic_zoom")
    
    d2,dr = LoadRaw('raw.dat')
    PlotRaw(d2, 'array')
    PlotRaw(dr, 'reflected')

def PlotQueried(name):
    print 'Loading: ' + name + '.dat'
    d = LoadTestData(name + '.dat')

    print 'Plotting: ' + name
    _plt.figure()
    mag = _np.sqrt(d[:,:,2]**2 + d[:,:,3]**2)
    #_plt.quiver(d[:,0],d[:,1],d[:,2],d[:,3],mag,cmap=_plt.cm.magma, scale=0.05, scale_units='xy')
    _plt.quiver(d[:,:,0],d[:,:,1],d[:,:,2],d[:,:,3],mag,cmap=_plt.cm.magma, scale=None, pivot='mid')
    _plt.xlabel('X (mm)')
    _plt.ylabel('Y (mm)')
    _plt.title('Queried Field Object - ' + name)
    _plt.colorbar()
    _plt.tight_layout()
    _plt.axes().set_aspect('equal', 'datalim')

    _plt.savefig(name + '.pdf')
    _plt.savefig(name + '.png')
    
def PlotRaw(d2, title='array'):
    s  = _np.shape(d2)
    nx = s[0]
    ny = s[1]
    #d2 = d2.reshape(nx*ny, 3)
    mag = _np.sqrt(d2[:,:,0]**2 + d2[:,:,1]**2)
    
    x = _np.arange(nx)
    y = _np.arange(ny)
    xv,yv = _np.meshgrid(x,y)
    xv = xv.reshape(nx*ny)
    yv = yv.reshape(nx*ny)

    d = d2.reshape(nx*ny,3)
    
    _plt.figure()
    _plt.quiver(xv, yv, d[:,0], d[:,1], mag, cmap=_plt.cm.magma, scale=1e-5, scale_units='xy')
    _plt.xlabel('X Index')
    _plt.ylabel('Y Index')
    _plt.title(title)
    _plt.colorbar()
    _plt.tight_layout()
    _plt.axes().set_aspect('equal', 'datalim')

    _plt.savefig(title+'.pdf')
    _plt.savefig(title+'.png')

def LoadTestData(filename):
    f = open(filename,'r')

    data = []
    for line in f:
        data.append(line.strip().split())
    f.close()
    data = _np.array(data, dtype=float)
    return data

def LoadRaw(filename):

    data  = []
    dataR = []
    intoR = False
    with open(filename) as f:
        for line in _it.islice(f, 5, None):
            if not line.strip():
                pass
                # only do stuff with lines that aren't only whitespace
            else:
                if 'REFLECTED' in line:
                    intoR = True
                elif intoR:
                    dataR.append(line.strip().strip('(').strip(')').split(','))
                else:
                    data.append(line.strip().strip('(').strip(')').split(','))

    n = int(_np.sqrt(len(data)))
    #print n
    data = _np.array(data, dtype=float).reshape(n,n,3)
    if (intoR):
        dataR = _np.array(dataR, dtype=float).reshape(2*n,2*n,3)
    return data, dataR

if __name__ == "__main__":
    Compare()
