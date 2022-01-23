#!/usr/bin/env python

import numpy as _np
import pybdsim

def main():
    B = 2.0

    # LOOP METHOD 1
    data = []
    # loop over and build up 3d lists of lists of lists
    for x in [-1,0,1]:
        v = []
        for z in [3,4]:
            v.append([x, z, B*x, B*x*z, B*z])
        data.append(v)

    # convert to numpy array
    data = _np.array(data)

    # loop order is actually z, then x - ie z varies first, so tzyx, so flip=True
    f = pybdsim.Field.Field2D(data, flip=True, secondColumn='Z')
    # we do this so the order is always written out in the default bdsim way
    f.Write('2dexample_loopOrder_for_xz.dat')
    # but we can purposively write it out the other loop way for testing
    # note the header keys are still the same apart from loopOrder> tzyx
    f.Write('2dexample_loopOrder_for_xz_tzyx.dat', writeLoopOrderReversed=True)


    # LOOP METHOD 2
    data2 = []
    # loop over other way
    for z in [3,4]:
        v = []
        for x in [-1,0,1]:
            v.append([x, z, B*x, B*x*z, B*z]) # values must still be in xyzt order
        data2.append(v)

    # convert to numpy array
    data2 = _np.array(data2)

    # loop order is actually x, then z - ie x varies first, so xyzt, so flip=False
    g = pybdsim.Field.Field2D(data2, flip=False, secondColumn='Z')
    # this will write out a file identical to the first one
    g.Write('2dexample_loopOrder_for_zx.dat')
    # this will write out a file identical to the second one
    g.Write('2dexample_loopOrder_for_zx_tzyx.dat', writeLoopOrderReversed=True)
    
    return data,f,g
    
if __name__ == "__main__":
    main()
