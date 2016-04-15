import pylab as _pl
import numpy as _np

class BDSBunchTester :
    def __init__(self) :
        bunchDistributions = ['1_Reference']
        bunchParticles     = ['e-','e+','proton']
        
    def runTests(self) : 
        for distrib in bunchDistribtions : 
            print distrib
    

class BDSBunchOutput : 
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

        _pl.clf()
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
        
        
        f = _pl.figure(2)
        _pl.clf()

        _pl.subplot(6,6,1)
        _pl.plot(self.x,self.x,"+")
        _pl.subplot(6,6,2)
        _pl.plot(self.x,self.y,"+")
        _pl.subplot(6,6,3)
        _pl.plot(self.x,self.z,"+")
        _pl.subplot(6,6,4)
        _pl.plot(self.x,self.xp,"+")        
        _pl.subplot(6,6,5)
        _pl.plot(self.x,self.yp,"+")
        _pl.subplot(6,6,6)
        _pl.plot(self.x,self.zp,"+")

        _pl.subplot(6,6,7)
        _pl.plot(self.y,self.x,"+")
        _pl.subplot(6,6,8)
        _pl.plot(self.y,self.y,"+")
        _pl.subplot(6,6,9)
        _pl.plot(self.y,self.z,"+")
        _pl.subplot(6,6,10)
        _pl.plot(self.y,self.xp,"+")        
        _pl.subplot(6,6,11)
        _pl.plot(self.y,self.yp,"+")
        _pl.subplot(6,6,12)
        _pl.plot(self.y,self.zp,"+")

        _pl.subplot(6,6,13)
        _pl.plot(self.z,self.x,"+")
        _pl.subplot(6,6,14)
        _pl.plot(self.z,self.y,"+")
        _pl.subplot(6,6,15)
        _pl.plot(self.z,self.z,"+")
        _pl.subplot(6,6,16)
        _pl.plot(self.z,self.xp,"+")        
        _pl.subplot(6,6,17)
        _pl.plot(self.z,self.yp,"+")
        _pl.subplot(6,6,18)
        _pl.plot(self.z,self.zp,"+")


        

