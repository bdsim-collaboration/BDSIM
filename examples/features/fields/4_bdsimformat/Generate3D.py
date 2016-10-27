#!/usr/bin/env python2.7

from mpl_toolkits.mplot3d import axes3d
import matplotlib.pyplot as _plt
import numpy as _np
import pybdsim
import tarfile

def main():
    # generate x and y points along their own axes
    x = _np.arange(-30, 30, 6.1)
    y = _np.arange(-25, 25, 7.2)
    z = _np.arange(-35, 35, 8)

    # create a mesh of coordinates using these 1d x and y coords
    xu,yu,zu = _np.meshgrid(y,x,z)

    # calculate some example field value at each coordinate
    xv = 4*_np.cos(0.08*xu)*_np.sin(0.1*yu)
    yv = 2*_np.sin(0.08*xu)*_np.cos(0.1*yu)
    zv = 3*_np.cos(0.09*zu+1.4)

    # restructure into correct shape of array
    magf = xv[:,:,:,_np.newaxis]

    # stack in the coordinates in front of the field x component
    magfyx  = _np.insert(magf,0,(xu,yu,zu),axis=3)

    # stack in the field y and z components after the field x component
    magfyxb = _np.insert(magfyx, 4, (yv,zv),axis=3)

    # construct a BDSIM format field object and write it out
    f = pybdsim.Field.Field3D(magfyxb)
    f.Write('3dexample.dat')

    # compress the result
    tar = tarfile.open("3dexample.tar.gz", "w:gz")
    tar.add('3dexample.dat')
    tar.close()

    Plot(magfyxb)

    return magfyxb

def Plot(array):

    a = array #shortcut
    #mag = _np.sqrt(a[:,:,:,3]**2 + a[:,:,:,4]**2 + a[:,:,:,5]**2)
    
    fig = _plt.figure()
    ax = fig.gca(projection='3d')
    ax.quiver(a[:,:,:,0], a[:,:,:,1], a[:,:,:,2], a[:,:,:,3], a[:,:,:,4], a[:,:,:,5], length=2, cmap=_plt.cm.magma)
    _plt.xlabel('X Index')
    _plt.ylabel('Y Index')
    ax.set_zlabel('Z Index')
    _plt.suptitle('3D Example Data')
    _plt.savefig('3dexample.png',dpi=400)

    _plt.show()
    
if __name__ == "__main__":
    main()
