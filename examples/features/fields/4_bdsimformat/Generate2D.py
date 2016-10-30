#!/usr/bin/env python2.7

import matplotlib.pyplot as _plt
import numpy as _np
import pybdsim
import tarfile

def main():
    # generate x and y points along their own axes
    x = _np.arange(-30, 30, 5.6)
    y = _np.arange(-25, 25, 6.8)

    # create a mesh of coordinates using these 1d x and y coords
    xu,yu = _np.meshgrid(y,x)

    # calculate some example field value at each coordinate
    xv = 4*_np.cos(0.08*xu)*_np.sin(0.1*yu)
    yv = 2*_np.sin(0.08*xu)*_np.cos(0.1*yu)

    # restructure into correct shape of array
    magf = xv[:,:,_np.newaxis]

    # stack in the coordinates in front of the field x component
    magfyx  = _np.insert(magf,0,(xu,yu),axis=2)

    # stack in the field y and z components after the field x component
    magfyxb = _np.insert(magfyx, 3, (yv,_np.zeros_like(xu)),axis=2)

    # construct a BDSIM format field object and write it out
    f = pybdsim.Field.Field2D(magfyxb)
    f.Write('2dexample.dat')

    # compress the result
    tar = tarfile.open("2dexample.tar.gz", "w:gz")
    tar.add('2dexample.dat')
    tar.close()

    Plot(magfyxb)

def Plot(array):

    a = array #shortcut
    mag = _np.sqrt(a[:,:,2]**2 + a[:,:,3]**2)

    _plt.figure()
    _plt.quiver(a[:,:,0], a[:,:,1], a[:,:,2], a[:,:,3], mag, cmap=_plt.cm.magma, pivot='mid', scale=None)
    _plt.xlabel('X Index')
    _plt.ylabel('Y Index')
    _plt.title('2D Example Data')
    _plt.colorbar()
    _plt.tight_layout()
    _plt.axes().set_aspect('equal', 'datalim')
    _plt.savefig('2dexample.png',dpi=400)
    
if __name__ == "__main__":
    main()
