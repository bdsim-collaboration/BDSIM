import ctypes as _ctypes
import numpy as _np
import matplotlib.pyplot as _plt

class Survey : 
    def __init__(self) : 
        self._x_current = 0 
        self._y_current = 0
        self._theta     = _np.pi
        self._names     = []
        self._beamline  = []
        self._x_coords  = [] 
        self._y_coords  = []
        self._lentotal  = 0

    def load(self, fileName) : 
        self._file = Loader() 
        self._file.load(fileName) 
        
        nelement = self._file._parserLib.get_nelements()
        
        for i in range(0,nelement,1) : 
            name   = self._file._parserLib.get_name(i) 
            length = self._file._parserLib.get_length(i)
            angle  = self._file._parserLib.get_angle(i)
            self._names.append(name)
            self.step(angle,length)
            i += 1    
               
    def step(self,angle,length):
        self._theta += angle
        dx    = length * _np.cos(self._theta)
        dy    = length * _np.sin(self._theta)
        self._lentotal += length
        
        x_new = self._x_current - dx
        y_new = self._y_current - dy
        self._beamline.append([[self._x_current,x_new],[self._y_current,y_new]])
        self._x_current = x_new
        self._y_current = y_new
        self._x_coords.append(x_new)
        self._y_coords.append(y_new)        

    def finalDiff(self):
        dx = self._beamline[-1][0][1] - self._x_current
        dy = self._beamline[-1][1][1] - self._y_current
        print 'Final dx ',dx
        print 'Final dy ',dy
        
    def plot(self) : 
        _plt.figure()
        _plt.plot(self._x_coords,self._y_coords,'b.')
        _plt.plot(self._x_coords,self._y_coords,'b-')
        _plt.xlabel('X (m)')
        _plt.ylabel('Y (m)')
        _plt.show()

    def compareMadX(self, fileName) : 
        import pymadx as _pymadx
        
        mxs = _pymadx.Survey()
        mxs.load(fileName) 

        self.plot()

        _plt.plot(mxs._x_coords,mxs._y_coords,'rx')
        _plt.plot(mxs._x_coords,mxs._y_coords,'r-')
        _plt.show()        
        
    def findClosestElement(self, coord) : 
        arr = _np.arange(0,len(self._x_coords),1) 
        xdiff = _np.array(self._x_coords)-coord[0]
        ydiff = _np.array(self._y_coords)-coord[1]
        d     = _np.sqrt(xdiff**2+ydiff**2)
        print arr[d == min(d)]
            

class Loader :
    def __init__(self) :
        #self._parserLibFileName = "/Users/sboogert/Physics/general/acc/bdsim/bdsim-mac/parser/libgmadShared.dylib"
        self._parserLibFileName = "/Users/nevay/physics/reps/bdsim-build/parser/libgmadShared.dylib"
        self._parserLib = _ctypes.cdll.LoadLibrary(self._parserLibFileName)
        
        self._parserLib.get_name.restype    = _ctypes.c_char_p
        self._parserLib.get_name.argtypes   = [_ctypes.c_int]
        self._parserLib.get_type.restype    = _ctypes.c_short
        self._parserLib.get_type.argtypes   = [_ctypes.c_int]    
        self._parserLib.get_length.restype  = _ctypes.c_double
        self._parserLib.get_length.argtypes = [_ctypes.c_int]
        self._parserLib.get_angle.restype   = _ctypes.c_double
        self._parserLib.get_angle.argtypes  = [_ctypes.c_int]
    
    def load(self, fileName) :
        self._parserLib.gmad_parser_c(fileName)
    
    def getElement(self, i) : 
        name   = self._parserLib.get_name(i) 
        type   = self._parserLib.get_type(i) 
        length = self._parserLib.get_length(i)
        angle  = self._parserLib.get_angle(i)
        
        return [name,type,length,angle]

    def getElementByName(self, name) :
        pass

    def parseLattice(self) : 
        return 

    
    def printToScreen(self) :
        nelement = self._parserLib.get_nelements()

        for i in range(0,nelement,1) : 
            name   = self._parserLib.get_name(i) 
            type   = self._parserLib.get_type(i) 
            length = self._parserLib.get_length(i)
            angle  = self._parserLib.get_angle(i)
            print i,name,type,length,angle        
    
