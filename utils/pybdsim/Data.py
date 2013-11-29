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
    def __init__(self):
        dict.__init__(self)
        self.units = {}

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
    data.units    = dict(zip(keys,units))
    data.keyslist = keys
    data.array    = dataarray
    return data
    
def _LoadRoot(filepath):
    data = RootData()
    f = open(filepath,'r')
    #stuff here
    f.close()
    return data
