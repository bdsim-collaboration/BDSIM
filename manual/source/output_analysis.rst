.. _output-analysis-section:

===============
Output Analysis
===============

This describes how to load and view data from the recommend output **rootevent** format. Other legacy formats
are detailed below (see `ROOTOutputAnalysis`_).

Setup
-----

.. note:: BDSIM must be installed after compilation for the analysis tools to function properly.

Once BDSIM has been installed the following environmental variables must be updated to allow the analysis
tool, 'rebdsim' (root event BDSIM), to function.  These can be set manually or added to your :code:`.profile` or
:code:`.bashrc` file::

   export BDSIM=<bdsim-INSTALL-dir>
   export PATH=$PATH:$BDSIM/bin
   export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$BDSIM/lib (Linux only)
   export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$BDSIM/lib (mac only)

* Re-source your profile (restart terminal).
* You should be able to execute 'rebdsim'

.. figure:: figures/rebdsim_execution.png
	    :width: 100%
	    :align: center

If the analysis will be regularly used interactively, it is worth automating the library loading
in root by finding and editing the :code:`rootlogon.C` in your :code:`<root-install-dir>/macros/`
directory.  Example text would be::

  cout << "Loading rebdsim libraries" << endl;
  gSystem->Load("<bdsim-install-dir>/lib/librebdsimLib");

.. note:: This fails to specify the file extension on purpose.

Usage
-----

rebdsim can be used either as a `standalone executable`_ or `interactively in ROOT`_, with
Cling.

Standalone Executable
=====================

rebdsim is executed with one argument which is the path to an analysis configuration text
file.  This is a simple text file that describes what histrograms to make from the data.
Optionally, a second argument of a data file to operate on will override the one specified
in the analysis configuration file. This allows the same analysis configuration to be used
to analyse many different data files. A third optional argument (must have second argument
specified) is the output file name that the resultant analysis will be written to.

Examples::

  rebdsim analysisConfig.txt
  rebdsim analysisConfig.txt output_event.root
  rebdsim analysisConfig.txt output_event.root results.root

See `Preparing an Analysis Configuration File`_ for details on the analysis configuration.

  
Interactively in ROOT
=====================

When using ROOT's interpreter, you can use the functionality of the BDSIM classes
dynamically. First you must load the shared library to provide the classes::

  root> gSystem->Load("<bdsim-install-dir>/lib/librebdsimLib");

.. note:: Unfortunately, ROOT does not show the loaded classes with tab completion in the interpreter.

Loading this library exposes all classes that are found in :code:`<bdsim>/analysis`. If you
are familiar with ROOT, you may use the ROOT file as you would any other given the
classes provided by the library::

  root> TFile* f = new TFile("output_event.root", "READ");
  root> TTree* eventTree = (TTree*)f->Get("Event");
  root> BDSOutputROOTEventLoss* elosslocal = new BDSOutputROOTEventLoss();
  root> eventTree->SetBranchAddress("Eloss.", &elosslocal);
  root> eventTree->GetEntry(0);
  root> cout << elosslocal->n << endl;
        345
  root>
  

Preparing an Analysis Configuration File
========================================

The analysis configuration file is a simple text file. This can be prepared by copying
and editing an example. The text file acts as a thin interface to an analysis in ROOT
that would commonly use the :code:`TTree->Draw()` method.

An example can be found in :code:`<bdsim>/examples/features/io/3_rootevent/analysisConfig.txt`::

  Debug                               1
  InputFilePath                       ./output_event.root
  OutputFileName                      ./ana_1.root
  CalculateOpticalFunctions           1
  CalculateOpticalFunctionsFileName   ./ana_1.dat
  # Object  Tree Name Histogram Name  # of Bins Binning             Variable            Selection
  Histogra  Event.    Primaryx        {100}     {-0.1:0.1}          Primary.x           1
  Histogram Event.    Primaryy        {100}     {-0.1:0.1}          Primary.y           1
  Histogram Options.  seedState       {200}     {0:200}             Options.GMAD::OptionsBase.seed 1
  Histogram Model.    componentLength {100}     {0.0:100}           Model.length        1
  Histogram Run.      runDuration     {1000}    {0,1000}            Info.duration       1
  Histogram Event.    XvsY            {100:100} {-0.1:0.1,-0.1:0.1} Primary.x:Primary.y 1

* Arguments in the histogram rows must not contain any white space!
* Columns in the histogram rows must be separated by any amount of white space (at least one space).
* A line beginning with :code:`#` is ignored as a comment line.
* 2D histograms (last line in example) have arguments separated by :code:`:`.

Converting ROOT trees as numpy arrays
-------------------------------------

A useful interface is root_numpy that allows root data to be loaded as a numpy array.

Installing root_numpy from source
=================================

To install from source::

   wget https://pypi.python.org/packages/source/r/root_numpy/root_numpy-4.3.0.tar.gz
   tar -zxf root_numpy-4.3.0.tar.gz
   cd root_numpy-4.3.0
   python2.7 setup.py build 
   sudo python2.7 setup.py install


Installing root_numpy from PIP
==============================

To install using the python package manager PIP::

   sudo port install py27-pip
   sudo pip-2.7 install root_numpy 

Extracting data from ROOT file ::

   > pylab
   In [1]: import ROOT 
   In [2]: import root_numpy 
   In [3]: f = ROOT.TFile("analysis.root")
   In [4]: t = f.Get("Sampler1")
   In [5]: a = root_numpy.tree2rec(t)  

.. _ROOTOutputAnalysis:

Old ROOT Output (robdsim)
-------------------------
To use the ROOT analysis (and the Python interface)::

   export ROBDSIM=<bdsim-build-dir>/utils/robdsim
   export PATH=$PATH:$ROBDSIM
   export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ROBDSIM (Linux only)
   export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$ROBDSIM (mac only)
   export PYTHONPATH=$PYTHONPATH:$ROBDSIM
   
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

ROOT classes and structure
==========================

* RobdsimAnalysis
* RobdsimOutput 
* AnalysisConfig

Then for each tree type (Eloss, PrecisionRegionElossPhits, Sampler)
* Tree
* TreeAnalysis



Examples
--------

Example: Plot Optical Functions with pybdsim
============================================
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
