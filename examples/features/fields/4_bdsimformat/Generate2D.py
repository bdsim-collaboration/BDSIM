#!/usr/bin/env python2.7

import matplotlib.pyplot as _plt
import numpy as _np
import pybdsim
import tarfile

def main():
    # generate x and y points along their own axes
    x = _np.arange(-30, 30, 5.6)
    y = _np.arange(-25, 25, 6.8)
    
    # define functions for each field component
    def fx(x,y):
        return 4*_np.cos(0.08*x)*_np.sin(0.1*y)
    def fy(x,y):
        return 2*_np.sin(0.08*x)*_np.cos(0.1*y)
    def fz(x,y):
        return 3*_np.cos(0.05*x)
    
    data = []
    # loop over and build up 3d lists of lists of lists
    for xi in x:
        v = []
        for yi in y:
            v.append([xi,yi,fx(xi,yi),fy(xi,yi),fz(xi,yi)])
        data.append(v)

    # convert to numpy array
    data = _np.array(data)
    
    # construct a BDSIM format field object and write it out
    f = pybdsim.Field.Field2D(data)
    f.Write('2dexample.dat')

    fd = pybdsim.Field.Field2D(data,doublePrecision=True)
    fd.Write('2dexample_dp.dat')

    # compress the result
    tar = tarfile.open("2dexample.tar.gz", "w:gz")
    tar.add('2dexample.dat')
    tar.close()

    #Plot(data)
    return data

def Plot(array):

    a = array #shortcut
    mag = _np.sqrt(a[:,:,2]**2 + a[:,:,3]**2)

    _plt.figure()
    _plt.quiver(a[:,:,0], a[:,:,1], a[:,:,2], a[:,:,3], mag, cmap=_plt.cm.magma, pivot='mid', scale=None)
    _plt.xlabel('X (cm)')
    _plt.ylabel('Y (cm)')
    _plt.title('2D Example Data')
    _plt.colorbar(label='Tesla')
    _plt.tight_layout()
    _plt.axes().set_aspect('equal', 'datalim')
    _plt.savefig('2dexample.png',dpi=400)
    
if __name__ == "__main__":
    main()
