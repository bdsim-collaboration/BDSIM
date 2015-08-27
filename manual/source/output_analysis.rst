.. _output-analysis-section:

===============
Output Analysis
===============

ROOT Output (robdsim)
---------------------
To use the ROOT analysis (and the Python interface)::

   export ROBDSIM=:<bdsim-build-dir>/utils/robdsim
   export PATH=$PATH:$ROBDSIM
   export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ROBDSIM (linux only)
   export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$ROBDSIM (mac only)
   export PYTHON_PATH=$PYTHON_PATH:$ROBDSIM
   
Note that several ROOT analysis output (output of robdsimAnal) files can be combined into one file with the command::

   robdsimComb combined.root output_0.root output_1.root output_2.root output_3.root

Using the robdsim library from within ROOT::

   > root 
   root [0] gSystem->Load("<bdsim-build-dir>/utils/robdsim/librobdsim.so")
   root [1] r = new RobdsimAnalysis("analysisConfig.txt")

Using the robdsim library from within Python::

   > pylab 
   In [1]: import robdsim
   In [2]: r = robdsim.RobdsimAnalysis("analysisConfig.txt")

From the command line with the executable (containing the ROOT output files and analysisConfig.txt)::

   > robdsimAnal analysisConfig.txt 

Typically analysis is performed with a simple configuration file (analysisConfig.txt, in the two examples above) that defined the histograms that should be created from trees etc. An example structure follows::  

   Debug                                   0
   InputFilePath                           ./output.root
   OutputFileName                          ./analysis.root
   CalculateOpticalFunctions               1
   CalculateOpticalFunctionsFileName       ./output.dat
   Histogram                               Sampler_sampler4        elec_x          x               partID==11
   Histogram                               Sampler_sampler4        elec_y          y               partID==11
   Histogram                               Sampler_sampler4        elec_xy         y:x             partID==11
   Histogram                               Sampler_sampler4        elec_E          E               partID==11
   Histogram                               Sampler_sampler4        photon_x        x               partID==22
   Histogram                               Sampler_sampler4        photon_y        y               partID==22
   Histogram                               Sampler_sampler4        photon_xy       y:x             partID==22
   Histogram                               Sampler_sampler4        photon_E        E               partID==22
   Histogram                               Sampler_sampler4        photon_Spec     log10(E)        partID==22

Converting ROOT trees as numpy arrays
-------------------------------------
First root_numpy needs to be installed (from scratch)::

   wget https://pypi.python.org/packages/source/r/root_numpy/root_numpy-4.3.0.tar.gz
   tar zxf root_numpy-4.3.0.tar.gz
   cd root_numpy-4.3.0
   python2.7 setup.py build 
   sudo python2.7 setup.py install

Or using PIP (mac):: 

   sudo port install py27-pip
   sudo pip-2.7 install root_numpy 

Extracting data from ROOT file::

   > pylab
   In [1]: import ROOT 
   In [2]: import root_numpy 
   In [3]: f = ROOT.TFile("analysis.root")
   In [4]: t = f.Get("Sampler1")
   In [5]: a = root_numpy.tree2rec(t)   

ROOT classes and structure 
--------------------------
* RobdsimAnalysis
* RobdsimOutput 
* AnalysisConfig

Then for each tree type (Eloss, PrecisionRegionElossPhits, Sampler)
* Tree
* TreeAnalysis

Extending ROOT analysis 
-----------------------

Examples
--------

Example: Plot Optical Functions with pybdsim
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
::

   import pybdsim
   import pymadx
   import robdsim

   # load ROOT output file into robdsim
   r=robdsim.RobdsimOutput("output.root")
   # calculate optics and write to output file "optics.dat"
   r.CalculateOpticalFunctions("optics.dat")
   # load optics file into pybdsim
   a=pybdsim.Data.Load("optics.dat")

   f = figure()
   # Plot sqrt Beta_x
   plot(a.S(),sqrt(a.Beta_x()),'.-',label='BDSIM')

   # compare with MadX file:
   b = pymadx.Tfs("madx.tfs")
   s = b.GetColumn("S")
   betx = b.GetColumn("BETX")
   plot(s,sqrt(betx),'.-',label='MadX')

   # labels and legend:
   xlabel('$s$ [m]')
   ylabel('$\sqrt{\\beta_x}$ [m]')
   legend(loc=0)

   # add machine lattice to figure (optional):
   pybdsim.Plot.AddMachineLatticeToFigure(f,b)
   
   # save figure
   f.savefig("sqrtbetax.png")

ASCII Output
------------
