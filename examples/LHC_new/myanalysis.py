import numpy as _np

def Load(filename):
    f = open(filename,'r')
    lines = f.readlines()
    f.close()

    keys = lines[2].strip('\n').strip('#').strip().split()
    keys_new = []
    for key in keys:
        keys_new.append(key.split('[')[0])

    for i in range(len(
    
    masterdata = []
    for line in lines[3:]:
        masterdata.append(map(_np.double,line.strip('\n').split()))

    masterdata = _np.array(masterdata)

    data = {}

    for i,k in enumerate(keys):
        data[k] = masterdata[:,i]

    return data
