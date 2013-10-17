# Data Loader for pybdsim
import numpy as _np


class Data:
    """
    class to read bdsim data format

    example usage
    a = Data()
    a.Read(filepath)
    a.data
    a.units
    a.keys
    """
    
    def __init__(self):
        self.data  = {}
        self.keys  = []
        self.units = []

    def __call__(self):
        print 'pybdsim.Data.Data instance'

    def __repr__(self):
        print 'pybdsim.Data.Data instance'
        if len(self.keys) > 0:
            print 'containting the following data'
            print 'Name\tUnits'
            for i,key in enumerate(self.keys):
                if self.units[i] != 'NA':
                    print str(key)+'\t['+str(self.units[i])+']'
                else:
                    print str(key)
            return ' '
        else:
            return ' '
    
    def __getitem__(self,datakey=None):
        if (datakey != None) & (self.data.has_key(datakey) == True):
            return self.data[datakey]
        elif (datakey != None) & (self.data.has_key(datakey) == False):
            print 'Invalid datakey'
        else:
            pass
        
    def _UpdateKeys(self):
        self.keys = self.data.keys()

    def Read(self,filepath):
        """
        Load bdsim output data
        
        Load(filepath)
        
        returns data dictionary
        
        format is determined automatically from 
        file extension
        
        .txt  - bdsim ASCII output format
        .root - root file format
        
        """
        self.filepath = filepath
        self.filename = filepath.split('/')[-1]
        
        knownfiletypes = ['txt','root']
        format = filepath.split('.')[-1]
        if format not in knownfiletypes:
            print 'pybdsim>Data>Load> unknown format'
        elif format == 'txt':
            a = AsciiFormat()
            a.Read(filepath)
            self.data      = a.data
            self.units     = a.units
            self.dataarray = a.dataarray
            self.keys      = a.keys
        elif format == 'root':
            a = RootFormat()
            a.Read(filepath)
            self.data  = a.data
            self.units = a.units
        else:
            print 'pybdsim>Data>Load> unknown format'

class AsciiFormat:
    def __init__(self):
        self.data = {}
        self.dataarray = _np.array([])

    def Clear(self):
        self.data = {}
        self.dataarray = _np.array([])

    def Read(self,filepath):
        #with open(filepath,'r') as fileobject:
        fileobject = open(filepath,'r')
        datalist = []
        for i, line in enumerate(fileobject):
            if i < 2:
                pass
            elif i == 2:
                self._ParseKeys(line)
            else:
                datalist.append(map(float,line.split()))
        self.dataarray = _np.array(datalist)
        for i,key in enumerate(self.keys):
            self.data[key] = list(self.dataarray[:,i])

    def _ParseKeys(self,keyline):
        rawkeys    = keyline.split()[1:]
        self.keys  = []
        self.units = []
        
        #remove units from keys
        for i,key in enumerate(rawkeys):
            if key.count('[') > 0:
                unit   = key.split('[')[1].split(']')[0]
                newkey = key.split('[')[0] 
                self.keys.append(newkey)
                self.units.append(unit)
            else:
                self.keys.append(key)
                self.units.append('NA')
                

class RootFormat:
    def __init__(self):
        self.data  = {}
        self.units = {}
    
    def Clear(self):
        self.data  = {}
        self.units = {}

    def Load(self,filepath):
        pass

