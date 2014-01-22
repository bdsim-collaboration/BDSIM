# pybdsim.Data - output data loader for pybdsim
# Version 1.0
# L. Nevay, S.T.Boogert
# laurie.nevay@rhul.ac.uk

"""
Output

Read bdsim output

Classes:
Data - read various output files


"""

import numpy as _np

class AsciiData(dict):
    def __init__(self, *args, **kwargs):
        dict.__init__(self)
        self._Update(*args, **kwargs)
        self._UpdateKeys()
        self.units = {}
        
    def _Update(self, *args, **kwargs):
        for k,v in dict(*args,**kwargs).iteritems():
            self[k] = v
    
    def SetUnits(self,unitsdict):
        self.unitsdict = unitsdict
        
    def _UpdateKeys(self):
        self.keyslist = self.keys()

    def __setitem__(self,key,value):
        dict.__setitem__(self,key,value)
        self._UpdateKeys()

def Load(filepath):
    knownfiletypes = ['txt','root']
    format = filepath.split('.')[-1]
    if format not in knownfiletypes:
        print 'pybdsim.Data.Load - unknown format'
    elif format == 'txt':
       data = _LoadAscii(filepath)
    elif format == 'root':
        data = _LoadRoot(filepath)
    return data

def ParseKeys(keyline):
    rawkeys    = keyline.split()[1:]
    keys  = []
    units = []
    #remove units from keys
    for i,key in enumerate(rawkeys):
        if key.count('[') > 0:
            unit   = key.split('[')[1].split(']')[0].replace('mu','$\mu$')
            newkey = key.split('[')[0] 
            keys.append(newkey)
            units.append(unit)
        else:
            keys.append(key)
            units.append('NA')
    return keys,units

def _LoadAscii(filepath):
    data = AsciiData()
    f = open(filepath,'r')
    datalist = []
    for i, line in enumerate(f):
        if i < 2:
            pass
        elif i == 2:
            keys,units = ParseKeys(line)
        else:
            datalist.append(map(float,line.split()))
    f.close()
    dataarray = _np.array(datalist)
    del datalist
    for i,key in enumerate(keys):
        data[key] = list(dataarray[:,i])
    data.SetUnits(dict(zip(keys,units)))
    return data,dataarray
    
def _LoadRoot(filepath):
    data = RootData()
    f = open(filepath,'r')
    #stuff here
    f.close()
    return data


class Event
