import pylab as _pl
import numpy as _np

class BDSBunchTester :
    def __init__(self) :
        bunchDistributions = ['1_Reference']
        bunchParticles     = ['e-','e+','proton']
        
    def runTests(self) : 
        for distrib in bunchDistribtions : 
            print distrib
    

class BDSBunchOutputPlotter : 
    def __init__(self) : 
        self.d  = _np.loadtxt("BDSBunchTestOutput.txt") 
        self.x  = self.d[:,1]
        self.y  = self.d[:,2]
        self.z  = self.d[:,3]
        self.xp = self.d[:,4]
        self.yp = self.d[:,5]
        self.zp = self.d[:,6]
        self.t  = self.d[:,7]
        self.e  = self.d[:,8]
        self.w  = self.d[:,9]
        

    def statsPhaseSpace(self) : 
        print 'rms x ',self.x.std()
        print 'rms y ',self.y.std()
        print 'rms z ',self.z.std()
        print 'rms xp',self.xp.std()
        print 'rms yp',self.yp.std()
        print 'rms zp',self.zp.std()
        print 'rms t ',self.t.std()
        print 'rms e ',self.e.std()
        print 'rms w ',self.w.std()

    def plotPhaseSpace(self) : 
        f = _pl.figure(1)

        _pl.subplot(3,3,1) 
        _pl.hist(self.x)

        _pl.subplot(3,3,2) 
        _pl.hist(self.y)

        _pl.subplot(3,3,3) 
        _pl.hist(self.z)

        _pl.subplot(3,3,4) 
        _pl.hist(self.xp)

        _pl.subplot(3,3,5) 
        _pl.hist(self.yp)

        _pl.subplot(3,3,6) 
        _pl.hist(self.zp)

        _pl.subplot(3,3,7) 
        _pl.hist(self.t)

        _pl.subplot(3,3,8) 
        _pl.hist(self.e)

        _pl.subplot(3,3,9) 
        _pl.hist(self.w)
        
