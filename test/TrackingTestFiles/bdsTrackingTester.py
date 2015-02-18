
import pymadx.Ptc
import pymadx.Beam
import pymadx.Builder
import pymadx.Tfs
import pybdsim.Beam
import pybdsim.Builder
import pybdsim.Data
import os as _os
import matplotlib.pyplot as _plt
import robdsim
import numpy as _np

class Test:
    def __init__(self,type_,foldername=None,particle="e-",energy=1.0,distribution='flat',nparticles=10,length=1.0,**kwargs):
        """
        Tracking test class

        type = 'drift' | 'quadrupole' | 'sextupole'
        distribution = 'flat' | 'gaussian'
        """
        self.type_        = type_
        self.filename     = self.type_
        self.foldername   = foldername
        self.ptcfilename  = 'inrays.madx'
        if self.foldername != None:
            self.usingfolder = True
            self.filepath = self.foldername+'/'+self.filename
            self.ptcfilepath =  self.foldername+'/'+self.ptcfilename
            _os.system("mkdir -p " + self.foldername)
        else:
            self.usingfolder = False
            self.filepath = self.filename
            self.ptcfilepath = self.ptcfilename
        self.particle     = particle
        self.energy       = energy
        self.distribution = distribution
        self.distrkwargs  = {}
        self.nparticles   = nparticles
        self.length       = length
        self.kwargs       = kwargs
        self.figureNr     = 1 # arbitrary number where figure start from

    def CleanMakeRun(self):
        self.Clean()
        self.Make()
        self.Execute()
        self.Compare()

    def Clean(self):
        _os.system("rm -rf "+self.filepath+"*")
        _os.system("rm -rf "+self.foldername+"/output*")
        _os.system("rm -rf "+self.foldername+"/Maxwellian_bend_for_ptc.txt trackone inrays.madx")
        _os.system("rm -rf "+self.foldername+"/test*")
        _os.system("rm -rf "+self.foldername+"/*.log")
        _os.system("rm -rf "+self.foldername+"/*.dat")

        # clean and close figures (8 figures in total)
        for i in range(9):
            _plt.close(self.figureNr+i)

    def ChangeDistribution(self,distribution='flat',nparticles=10,**kwargs):
        """
        'flat'
        kwargs: mux=0.0, widthx=1e-05, mupx=0.0, widthpx=1e-05, muy=0.0,
                widthy=0.001, mupy=0.0, widthpy=0.001
        'gauss'
        kwargs: geBx_t=1e-10, betax=0.1, alfx=0.0, geBy_t=1e-10, betay=0.1,
                alfy=0.0, sigmat=1e-12, sigmapt=1e-12
        """
        self.distribution = distribution
        self.distrkwargs  = kwargs
        self.nparticles   = nparticles

    def Make(self):
        #type_='drift', foldername=None, particle='e-', energy=1.0,**kwargs) :
        print 'Test> Element type:         ',self.type_
        print 'Test> Destination filepath: ',self.filepath
        print 'Test> kwargs: ',
        for k in self.kwargs :
            print k+'='+str(self.kwargs[k]),

        if self.distribution == 'flat':
            ptc = pymadx.Ptc.FlatGenerator(**self.distrkwargs)
        elif self.distribution == 'gauss':
            ptc = pymadx.Ptc.GaussGenerator(**self.distrkwargs)

        ptc.Generate(self.nparticles,self.ptcfilepath)

        bb  = pybdsim.Beam(self.particle,self.energy,'ptc')

        bb.SetDistribFileName(self.ptcfilename)

        bm  = pybdsim.Builder.Machine()

        bm.AddBeam(bb)

        if self.type_ == 'drift' :
            name = 'd1'
            bm.AddDrift(name,length=self.length,**self.kwargs)
        elif self.type_ == 'sbend':
            name = 'sb1'
            bm.AddDipole(name,length=self.length,**self.kwargs)
        elif self.type_ == 'quadrupole' :
            name = 'q1'
            bm.AddQuadrupole(name,length=self.length,**self.kwargs)
        elif self.type_ == 'sextupole' :
            name = 's1'
            bm.AddSextupole(name,length=self.length,**self.kwargs)

        bm.AddMarker("theend") # Need a post element marker to sample at, only for bdsim

        bm.AddSampler('theend')

        bm.WriteLattice(self.filepath)

    def Execute(self):
        if self.usingfolder:
            _os.chdir(self.foldername)
        
        _os.system("bdsim-tracking --file="+self.filename+".gmad --batch --outfile='testTRK' --output=combined > bdsimTRK.log")
        _os.system("bdsim --file="+self.filename+".gmad --batch --outfile='test' --output=combined > bdsim.log")

        if self.usingfolder:
            _os.chdir("../")

    def Compare(self, addPrimaries=False):

        if self.usingfolder:
            _os.chdir(self.foldername)

        bdsimprim = pybdsim.Data.Load("test.primaries.txt")
        Bx0 = bdsimprim.X()
        By0 = bdsimprim.Y()
        Bxp0 = bdsimprim.Xp()
        Byp0 = bdsimprim.Yp()
        self.bdsimprimaries = {'x':Bx0,'y':By0,'xp':Bxp0,'yp':Byp0}

        bdsim = pybdsim.Data.Load("test.txt")
        Bx = bdsim.X()
        By = bdsim.Y()
        Bxp = bdsim.Xp()
        Byp = bdsim.Yp()
        self.bdsimoutput = {'x':Bx,'y':By,'xp':Bxp,'yp':Byp}


        bdsimprim_trk = pybdsim.Data.Load("testTRK.primaries.txt")
        Bx0_t = bdsimprim_trk.X()
        By0_t = bdsimprim_trk.Y()
        Bxp0_t = bdsimprim_trk.Xp()
        Byp0_t = bdsimprim_trk.Yp()
        self.bdsimprimaries_trk = {'x':Bx0_t,'y':By0_t,'xp':Bxp0_t,'yp':Byp0_t}

        bdsim_trk = pybdsim.Data.Load("testTRK.txt")
        Bx_t = bdsim_trk.X()
        By_t = bdsim_trk.Y()
        Bxp_t = bdsim_trk.Xp()
        Byp_t = bdsim_trk.Yp()
        self.bdsimoutput_trk = {'x':Bx_t,'y':By_t,'xp':Bxp_t,'yp':Byp_t}


        fresx  = _np.nan_to_num(Bx_t - Bx)
        fresy  = _np.nan_to_num(By_t - By)
        fresx  = _np.nan_to_num(fresx / Bx_t) #protect against nans for 0 diff
        fresy  = _np.nan_to_num(fresy / By_t)
        fresxp = _np.nan_to_num(Bxp_t - Bxp)
        fresyp = _np.nan_to_num(Byp_t - Byp)
        fresxp = _np.nan_to_num(fresxp / Bxp_t)
        fresyp = _np.nan_to_num(fresyp / Byp_t)
        self.residuals = {'x':fresx,'y':fresy,'xp':fresxp,'yp':fresyp}

        # 2d plots
        #X vs Y
        _plt.figure(self.figureNr)
        _plt.clf()
        _plt.plot(Bx_t,By_t,'b.',label='BDSTRK')
        _plt.plot(Bx,By,'g.',label='BDSIM')
        if addPrimaries:
            _plt.plot(Bx0,By0,'r.',label='BDSIM prim')
        _plt.legend()
        _plt.xlabel(r"x ($\mu$m)")
        _plt.ylabel(r"y ($\mu$m)")
        _plt.title(self.type_)
        #_plt.savefig(self.type_+'_xy.pdf')
        #_plt.savefig(self.type_+'_xy.png')

        #XP vs YP
        _plt.figure(self.figureNr+1)
        _plt.clf()
        _plt.plot(Bxp_t,Byp_t,'b.',label='BDSTRK')
        _plt.plot(Bxp,Byp,'g.',label='BDSIM')
        if addPrimaries:
            _plt.plot(Bxp0,Byp0,'r.',label='BDSIM prim')
        _plt.legend()
        _plt.xlabel(r"x' ($\mu$m)")
        _plt.ylabel(r"y' ($\mu$m)")
        _plt.title(self.type_)
        #_plt.savefig(self.type_+'_xpyp.pdf')
        #_plt.savefig(self.type_+'_xpyp.png')

        #X vs XP
        _plt.figure(self.figureNr+2)
        _plt.clf()
        _plt.plot(Bx_t,Bxp_t,'b.',label='BDSTRK')
        _plt.plot(Bx,Bxp,'g.',label='BDSIM')
        if addPrimaries:
            _plt.plot(Bx0,Bxp0,'r.',label='BDSIM prim')
        _plt.legend()
        _plt.xlabel(r"x ($\mu$m)")
        _plt.ylabel(r"x' (rad)")
        _plt.title(self.type_)
        #_plt.savefig(self.type_+'_xxp.pdf')
        #_plt.savefig(self.type_+'_xxp.png')

        #Y vs YP
        _plt.figure(self.figureNr+3)
        _plt.clf()
        _plt.plot(By_t,Byp_t,'b.',label='BDSTRK')
        _plt.plot(By,Byp,'g.',label='BDSIM')
        if addPrimaries:
            _plt.plot(By0,Byp,'r.',label='BDSIM prim')
        _plt.legend()
        _plt.xlabel(r"y ($\mu$m)")
        _plt.ylabel(r"y' (rad)")
        _plt.title(self.type_)
        #_plt.savefig(self.type_+'_yyp.pdf')
        #_plt.savefig(self.type_+'_yyp.png')

        # 1d plots
        # x comparison
        _plt.figure(self.figureNr+4)
        _plt.clf()
        _plt.hist(Bx_t,color='b',label='BDSTRK',histtype='step')
        _plt.hist(Bx,color='g',label='BDSIM',histtype='step')
        if addPrimaries:
            _plt.hist(Bx0,color='r',label='BDSIM prim',histtype='step')
        _plt.legend()
        _plt.xlabel(r"x ($\mu$m)")
        _plt.title(self.type_)
        #_plt.savefig(self.type_+'_x.pdf')
        #_plt.savefig(self.type_+'_x.png')

        # y comparison
        _plt.figure(self.figureNr+5)
        _plt.clf()
        _plt.hist(By_t,color='b',label='BDSTRK',histtype='step')
        _plt.hist(By,color='g',label='BDSIM',histtype='step')
        if addPrimaries:
            _plt.hist(By0,color='r',label='BDSIM prim',histtype='step')
        _plt.legend()
        _plt.xlabel(r"y ($\mu$m)")
        _plt.title(self.type_)
        #_plt.savefig(self.type_+'_y.pdf')
        #_plt.savefig(self.type_+'_y.png')

        # xp comparison
        _plt.figure(self.figureNr+6)
        _plt.clf()
        _plt.hist(Bxp_t,color='b',label='BDSTRK',histtype='step')
        _plt.hist(Bxp,color='g',label='BDSIM',histtype='step')
        if addPrimaries:
            _plt.hist(Bxp0,color='r',label='BDSIM prim',histtype='step')
        _plt.legend()
        _plt.xlabel(r"x' (rad)")
        _plt.title(self.type_)
        #_plt.savefig(self.type_+'_xp.pdf')
        #_plt.savefig(self.type_+'_xp.png')

        # yp comparison
        _plt.figure(self.figureNr+7)
        _plt.clf()
        _plt.hist(Byp_t,color='b',label='BDSTRK',histtype='step')
        _plt.hist(Byp,color='g',label='BDSIM',histtype='step')
        if addPrimaries:
            _plt.hist(Byp0,color='r',label='BDSIM prim',histtype='step')
        _plt.legend()
        _plt.xlabel(r"y' (rad)")
        _plt.title(self.type_)
        #_plt.savefig(self.type_+'_yp.pdf')
        #_plt.savefig(self.type_+'_yp.png')

        # residuals in one plot
        f = _plt.figure(self.figureNr+8)
        _plt.clf()

        axX = f.add_subplot(221)
        axX.hist(Bx_t,weights=fresx,bins=100,ec='b')
        axX.set_xlabel(r'X ($\mu$m)')
        axX.set_ylabel('Fractional Residual')

        axY = f.add_subplot(222)
        axY.hist(By_t,weights=fresy,bins=100,ec='b')
        axY.set_xlabel(r'Y ($\mu$m)')
        axY.set_ylabel('Fractional Residual')

        axXp = f.add_subplot(223)
        axXp.hist(Bxp_t*1e3,weights=fresxp,bins=100,ec='b')
        axXp.set_xlabel('Xp (mrad)')
        axXp.set_ylabel('Fractional Residual')

        axYp = f.add_subplot(224)
        axYp.hist(Byp_t*1e3,weights=fresyp,bins=100,ec='b')
        axYp.set_xlabel('Yp (mrad)')
        axYp.set_ylabel('Fractional Residual')

        _plt.subplots_adjust(left=0.15,right=0.95,top=0.95,wspace=0.39,hspace=0.25)
        #_plt.savefig(self.type_+'_residuals.pdf')
        #_plt.savefig(self.type_+'_residuals.png')

        #emittance
        #r = robdsim.robdsimOutput("test_0.root")
        #r.CalculateOpticalFunctions("optics.dat")
        #d = pybdsim.Data.Load("optics.dat")
        #print 'Horizontal emittance bdsim (before,after) ',d.Emitt_x()
        #print 'Vertical emittance bdsim (before,after) ',d.Emitt_y()
        
        _plt.show()

        if self.usingfolder:
            _os.chdir("../")
