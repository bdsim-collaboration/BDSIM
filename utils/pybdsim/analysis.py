import numpy as _np
import outputLoader as _ol
import matplotlib.pyplot as _plt

def analysis(fileName, plot = False) : 
    l  = _ol.ascii() 
    d  = l.load(fileName)
    da = d.getSamplerDataArray()
    a  = asciiDataAnalysis(da)
    a.run()

    if plot : 
        z = a._analOut[:,3]
        
        _plt.figure()
        _plt.subplot(5,1,1)
        _plt.plot(z,a._analOut[:,0],'-')    
        _plt.ylabel('$N_{particle}$')
        
        _plt.subplot(5,1,2)
        _plt.plot(z,a._analOut[:,1],'-')    
        _plt.plot(z,a._analOut[:,2],'-')    
        
        _plt.subplot(5,1,3)
        _plt.plot(z,a._analOut[:,4],'-')    
        _plt.plot(z,a._analOut[:,5],'-')    
        
        _plt.subplot(5,1,4)
        _plt.plot(z,a._analOut[:,6],'-')
        _plt.plot(z,a._analOut[:,7],'-')
        _plt.ylabel('$\sigma_{x,y}$')
        
        _plt.subplot(5,1,5)
        _plt.plot(z,a._analOut[:,8],'-')
        _plt.plot(z,a._analOut[:,9],'-')
        _plt.ylabel('$\sigma_{xp,yp}$')
        
    return a  
    
def basicSamplerAnalysis(sd) :
    npart = len(sd[:,sd._keys['x']])
    zmean = sd[:,sd._keys['z']].mean()
    xmean = sd[:,sd._keys['x']].mean()
    ymean = sd[:,sd._keys['y']].mean()
    xpmean= sd[:,sd._keys['xp']].mean()
    ypmean= sd[:,sd._keys['yp']].mean()    
    xrms  = sd[:,sd._keys['x']].std()
    yrms  = sd[:,sd._keys['y']].std()
    xprms = sd[:,sd._keys['xp']].std()
    yprms = sd[:,sd._keys['yp']].std()

    return _np.array([npart,xmean,ymean,zmean,xpmean,ypmean,xrms,yrms,xprms,yprms])
    
class asciiDataAnalysis : 
    def __init__(self, samplerDataList) : 
        '''Sampler data analysis '''
        self._dataList = samplerDataList 

    def run(self) : 
        # List of analysis output 
        self._analOut = [] 
        for d in self._dataList : 
            # Perform basic analysis on a plane 
            self._analOut.append(basicSamplerAnalysis(d))
            
        self._analOut = _np.array(self._analOut)

        
class asciiAnalysis : 
    def __init__(self, data) : 
        print 'analysis.asciiAnalysis.__init__()>'
        self._data = data
            
    def plotXY(self, hist=True) : 
        if hist : 
            _plt.hist2d(self._data[:,self._data._keys['x']],self._data[:,self._data._keys['y']],30)
        else :
            _plt.plot(self._data[:,self._data._keys['x']],self._data[:,self._data._keys['y']],"+")        
        _plt.xlabel("$x\;\mu m$")
        _plt.ylabel("$y\;\mu m$")
            
    def plotXXp(self) : 
        _plt.plot(self._data[:,self._data._keys['x']],self._data[:,self._data._keys['xp']],"+")
        _plt.xlabel("$x\;\mu m$")
        _plt.ylabel("$x^{\prime}$")

    def plotYYp(self) : 
        _plt.plot(self._data[:,self._data._keys['y']],self._data[:,self._data._keys['yp']],"+")        
        _plt.xlabel("$y\;\mu m$")
        _plt.ylabel("$y^{\prime}$")

    def plotTE(self) : 
        _plt.xlabel("$T$ s")
        _plt.ylabel("$E$ MeV")        

    def plotAll(self) :
        _plt.figure(1)
        _plt.clf()

        _plt.subplot(2,2,1)
        self.plotXY()

        _plt.subplot(2,2,2)
        self.plotXXp()

        _plt.subplot(2,2,3)
        self.plotYYp()

        _plt.subplot(2,2,4)
        self.plotTE()
        
        
