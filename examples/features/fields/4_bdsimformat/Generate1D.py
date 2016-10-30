#!/usr/bin/env python2.7

import matplotlib.pyplot as _plt
import numpy as _np
import pybdsim
import tarfile

def main():
    # generate x and y points along their own axes
    x = _np.arange(-30, 30, 7.5)

    # calculate some example field value at each coordinate
    xv = 4*_np.cos(0.08*x)
    yv = 2*_np.sin(0.1*x)

    # construct a BDSIM format field object and write it out
    f = pybdsim.Field.Field1D(x,xv,yv,_np.zeros_like(xv))
    f.Write('1dexample.dat')

    # compress the result
    tar = tarfile.open("1dexample.tar.gz", "w:gz")
    tar.add('1dexample.dat')
    tar.close()

    Plot(x,xv,yv)

def Plot(x,xv,yv):
    
    mag = _np.sqrt(xv**2 + yv**2)

    _plt.figure()
    _plt.quiver(x, _np.zeros_like(x), xv, yv, mag, cmap=_plt.cm.magma, pivot='mid', scale=None)
    _plt.xlabel('X (cm)')
    _plt.ylabel('Y (cm)')
    _plt.title('1D Example Data')
    _plt.colorbar()
    _plt.tight_layout()
    _plt.axes().set_aspect('equal', 'datalim')
    _plt.savefig('1dexample.png',dpi=400)

    _plt.figure()
    _plt.plot(x,xv, '.', label='Fx')
    _plt.plot(x,yv, '.', label='Fy')
    _plt.xlabel('X (cm)')
    _plt.ylabel('Y (cm)')
    _plt.legend()
    _plt.tight_layout()
    _plt.savefig('1dexample_1d.png', dpi=400)
    
if __name__ == "__main__":
    main()
