.. _output-analysis-section:

===============
Output Analysis
===============

This describes how to load and view data from the recommend output **rootevent**
format.

BDSIM is accompanied by an analysis tool called REBDSIM (root event BDSIM) that provides the ability
to use simple text input files to specify histograms and process data. It also
provides the ability to calculate optical functions from the sampler data.

REBDSIM is based on a set of analysis classes that are compiled into a library. These
may be used through REBDSIM, but also through the ROOT interpreter and in users'
ROOT macros or compiled code. They may also be used through Python if the user has
ROOT available through Python.

Setup
-----

1) BDSIM must be installed after compilation for the analysis tools to function properly.
2) Environmental variables should be set.
3) A ROOT logon macro may be written for convenience.

Once BDSIM has been installed the following environmental variables must be updated to allow the analysis
tool, 'rebdsim' (root event BDSIM), to function.  These can be set manually or added to your :code:`.profile` or
:code:`.bashrc` file::

   export BDSIM=<bdsim-INSTALL-dir>
   export PATH=$PATH:$BDSIM/bin
   export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$BDSIM/lib (Linux only)
   export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$BDSIM/lib (mac only)
   export ROOT_INCLUDE_PATH=$BDSIM/include/bdsim/:$BDSIM/include/bdsim/analysis/

* Re-source your profile (or restart the terminal).
* You should be able to execute 'rebdsim'

.. figure:: figures/rebdsim_execution.png
	    :width: 100%
	    :align: center

If the analysis will be regularly used interactively, it is worth automating the library loading
in root by finding and editing the :code:`rootlogon.C` in your :code:`<root-install-dir>/macros/`
directory.  Example text would be::

  cout << "Loading rebdsim libraries" << endl;
  gSystem->Load("librebdsimLib");

.. note:: The file extension is omitted on purpose.

The absolute path is not necessary as the above environmental variables are used by ROOT
to find the library.

Usage
-----

rebdsim can be used either as a `standalone executable`_ or `interactively in ROOT`_
or `interactively in Python`_.

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

Interactively in Python
=======================

This is the preferred method. ROOT must have been installed or compiled with Python support.
You can test this by starting Python and trying to import ROOT - there should be no errors:

   >>> import ROOT

The library containing the analysis classes may be then loaded:

   >>> import ROOT
   >>> ROOT.gSystem.Load("librebdsimLib")

The classes in :code:`bdsim/analysis` will now be available inside ROOT in Python.

  
Interactively in ROOT
=====================

When using ROOT's interpreter, you can use the functionality of the BDSIM classes
dynamically. First you must load the shared library (if not done so in your root logon
macro) to provide the classes::

  root> gSystem->Load("<bdsim-install-dir>/lib/librebdsimLib");

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

The header (".hh") files in :code:`<bdsim>/analysis` provide the contents and abilities
of each class.
  

Preparing an Analysis Configuration File
========================================

The analysis configuration file is a simple text file. This can be prepared by copying
and editing an example. The text file acts as a thin interface to an analysis in ROOT
that would commonly use the :code:`TTree->Draw()` method.

An example can be found in :code:`<bdsim>/examples/features/io/3_rootevent/analysisConfig.txt` ::

  Debug                           True
  InputFilePath                   ./output_event.root
  OutputFileName                  ./ana_1.root
  CalculateOpticalFunctions       True
  OpticalFunctionsFileName       ./ana_1.dat
  # Object  Tree Name Histogram Name  # of Bins  Binning             Variable            Selection
  Histogram1D  Event.    Primaryx        {100}      {-0.1:0.1}          Primary.x           1
  Histogram1D  Event.    Primaryy        {100}      {-0.1:0.1}          Primary.y           1
  Histogram1D  Options.  seedState       {200}      {0:200}             Options.GMAD::OptionsBase.seed 1
  Histogram1D  Model.    componentLength {100}      {0.0:100}           Model.length        1
  Histogram1D  Run.      runDuration     {1000}     {0:1000}            Info.duration       1
  Histogram2D  Event.    XvsY            {100,100}  {-0.1:0.1,-0.1:0.1} Primary.x:Primary.y 1
  Histogram3D  Event.    PhaseSpace3D    {50,50,50} {-5e-6:5e-6,-5e-6:5e-6,-5e-6:5e-6} Primary.x:Primary.y:Primary.z 1

* Arguments in the histogram rows must not contain any white space!
* Columns in the histogram rows must be separated by any amount of white space (at least one space).
* A line beginning with :code:`#` is ignored as a comment line.
* Empty lines are also ignored.
* For bins and binning, the dimensions are separated by :code:`,`.
* For bins and binning, the range from low to high is specified by :code:`low:high`.
* For a 2D or 3D histogram, x vs. y variables are specified by :code:`samplername.y:samplername.x`. See warning below.
* Variables must contain the full 'address' of a variable inside a Tree.
* A 3D histogram is shown on the last line.
* True or False as well as 1 or 0 may be used for Boolean options.

.. warning:: The variable for plotting is really a simple interface to CERN ROOT's TTree Draw
	     method.  This has some inconsistency.  If 1D, there is just `x`.  If 2D, it's
	     `y` : `x`. If 3D, it's `x` : `y` : `z`.  This only applies to the variable and
	     not to the bin specification.
  
The following (case-insensitive) options may be specified in the top part.


+----------------------------+------------------------------------------------------+
| **Option**                 | **Description**                                      |
+============================+======================================================+
| Debug                      | Whether to print out debug information.              |
+----------------------------+------------------------------------------------------+
| InputFilePath              | The root event file to analyse (or regex for         |
|                            | multiple).                                           |
+----------------------------+------------------------------------------------------+
| OutputFileName             | The name of the result file to written to.           |
+----------------------------+------------------------------------------------------+
| CalculateOpticalFunctions  | Whether to calculate optical functions or not.       |
+----------------------------+------------------------------------------------------+
| OpticalFunctionsFileName   | The name of a separate text file copy of the opcial  |
|                            | functions output.                                    |
+----------------------------+------------------------------------------------------+
| PrintModuloFraction        | The fraction of events to print out (default 0.01).  |
|                            | If you require print out for every event, set this   |
|                            | to be very small.                                    |
+----------------------------+------------------------------------------------------+
| ProcessSamplers            | Whether to load the sampler data or not.             |
+----------------------------+------------------------------------------------------+
| ProcessLosses              | TBC.                                                 |
+----------------------------+------------------------------------------------------+
| ProcessAllTrees            | TBC.                                                 |
+----------------------------+------------------------------------------------------+
| MergeHistograms            | Whether to merge the event level default histograms  |
|                            | provided by BDSIM. Turning this off will             |
|                            | significantly improve the speed of analysis if only  |
|                            | separate user-defined histograms are desired.        |
+----------------------------+------------------------------------------------------+
| GDMLFileName               | TBC.                                                 |
+----------------------------+------------------------------------------------------+


Variables In Data
-----------------

The variables for histograms are described in :ref:`output-section`. However, the
user can also quickly determine what they want by using a ROOT TBrowser to inspect
a file::

  root output_event.root
  root> TBrowser tb;

At which point, a browser window will appear with the specified file open. The variable
used in the histogram should be the full 'address' of the variable inside the Tree. Here,
the tree is :code:`Event.` and the variable is :code:`Info.duration`.

.. figure:: figures/root-tbrowser.png
	    :width: 90%
	    :align: center


Speed & Efficiency
------------------

Whilst the ROOT file IO is very efficient, the sheer volume of data to process can
easily result in slow running analysis. To combat this, only the minimal variables
should be loaded that need to be. REBDSIM automatically activates only the 'ROOT
branches' it needs for the analysis. A few possible ways to improve performance are:

* Turn off optical function calculation if it's not needed or doesn't make sense. I.e.
  if you're analysing the spray from a collimator in a sampler, it makes no sense to
  calculate the optical functions of that distribution.
* Turn off the MergeHistograms. If you're only making your own histograms this should
  speed up the analysis considerably for a large number of events.


Example Analysis
================

TBC


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
