import Data
import numpy as _np

class Analysis:
    """
    Analysis class for bdsim output

    one instance of the class per output file

    Analysis('../../path/to/my/output.txt')
    
    It will also look for '../../path/to/my/output.eloss.txt' 
    beside the output file

    """
    def __init__(self,filepath):
        a = Data.Data()
        a.Read(filepath)
        self.filepath  = a.filepath
        self.filename  = a.filename
        self.data      = a.data
        self.dataarray = a.dataarray
        self.keys      = a.keys
        self.units     = a.units
        
    def GroupBy(self,variable='Z'):
        """
        GroupBy(variable='Z')
        
        create instance.datagrouped dictionary
        finds unique values of variable and groups all the 
        data that has that value of variable

        e.g.
        GroupBy()   # default is 'Z'
        instance.datagrouped is dict with:
        0.000: array of (nparticles x allother variables)
        1.202: similar but different number of particles maybe
        ... etc
        """
        self.datagrouped = {}
        
        #find unique values of variable
        uniquevalues = sorted(list(set(_np.round(self.data[variable],2))))

        #remove the variable from the subset
        #find it's index in keys list
        indexofvariabletoremove = self.keys.index(variable)
        for value in uniquevalues:
            mask      = _np.round(self.data[variable],2) == value
            dcopy     = _np.delete(self.dataarray,indexofvariabletoremove,axis=1)[mask]
            variables = list(_np.delete(self.keys,indexofvariabletoremove))
            dcopydict = dict(zip(variables,[dcopy[:,i] for i in range(_np.shape(dcopy)[1])]))
            dcopydict['nparticles'] = _np.shape(dcopy)[0]
            self.datagrouped[value] = dcopydict
        
        self.keysgrouped = list(_np.sort(self.datagrouped.keys()))

#    def GenerateSigmas(self):
#        if hasattr(self,'datagrouped') == False:
#            self.GroupBy()
#        
#        z = self.keysgrouped
#        sx,sy = [],[]
#
#        for i in range(len(self.keysgrouped)):
#            sx.append(_np.std(self.datagrouped[self.keysgrouped[i]]['X']))
#            sy.append(_np.std(self.datagrouped[self.keysgrouped[i]]['Y']))
#        self.simpledata = {'sx':sx,'sy':sy,'z':z}

    def SortBy(self,variable='Z'):
        pass
        

    
