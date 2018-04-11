.. _output-analysis-section:

===============
Output Analysis
===============

This section describes how to load and view data from the recommend output **rootevent**
format.

BDSIM is accompanied by an analysis tool called REBDSIM (that stands for "root event BDSIM")
that provides
the ability to use simple text input files to specify histograms and process data. It also
provides the ability to calculate optical functions from the sampler data.

REBDSIM is based on a set of analysis classes that are compiled into a library. These
may be used through REBDSIM, but also through the ROOT interpreter and in a user's
ROOT macro or compiled code. They may also be used through Python if the user has
ROOT available through Python.

See :ref:`basic-data-inspection` for how to view the data and make the most basic
histograms.

.. _output-analysis-setup:

Setup
-----

1) BDSIM must be installed after compilation for the analysis tools to function properly.
2) Environmental variables should be set.
3) A ROOT logon macro may be written for convenience.

Once BDSIM has been installed the following environmental variables must be updated to
allow `rebdsim`, to function.  These can be set manually or added to your
:code:`.profile` or :code:`.bashrc` file::

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

If the analysis will be regularly used interactively, it is worth automating the library
loading in root by finding and editing the :code:`rootlogon.C` in your
:code:`<root-install-dir>/macros/` directory.  Example text would be::

  cout << "Loading rebdsim libraries" << endl;
  gSystem->Load("librebdsimLib");
  gSystem->Load("libbdsimRootEvent");

.. note:: The file extension is omitted on purpose.

The absolute path is not necessary as the above environmental variables are used by ROOT
to find the library.

REBDSIM Usage
-------------

rebdsim is executed with one argument which is the path to an analysis configuration text
file. This is a simple text file that describes what histograms to make from the data.
Optionally, a second argument of a data file to operate on will override the one specified
in the analysis configuration file. This allows the same analysis configuration to be used
to analyse many different data files. A third optional argument (must have second argument
specified) is the output file name that the resultant analysis will be written to.

Examples::

  rebdsim analysisConfig.txt
  rebdsim analysisConfig.txt output.root
  rebdsim analysisConfig.txt output.root results.root

.. _analysis-preparing-analysis-config:

Preparing an Analysis Configuration File
----------------------------------------

The analysis configuration file is a simple text file. This can be prepared by copying
and editing an example. The text file acts as a thin interface to an analysis in ROOT
that would commonly use the :code:`TTree->Draw()` method.

We strongly recommend browsing the data in a TBrowser beforehand and double clicking
the variables. This gives you idea of the range of the data. See :ref:`basic-data-inspection`
for more details.

There are three types of histograms that rebdsim can produce:

1. "Simple" histograms - these are sum over all entries in that tree.
2. "Per-Entry" histograms - here an individual histogram is made for each entry in the
   tree and these are averaged across all entries. In the case of the Event tree, each
   entry is a single event. A per-entry histogram is therefore a per-event histogram.
3. "Merged" histograms - these are the mean taken across all entries of a histogram
   already in the output file. For example, there is an energy deposition histogram
   stored with each event. This would be merged into a per-event average.

Per Entry and Simple Histograms
===============================

For the energy deposition histogram for example, the energy deposition hits are binned
as a function of curvilinear `S` position along the accelerator. In fact, the `S` position
is binned with the weight of the energy. In each event, a single primary particle can lead
to the creation of thousands of secondaries that can each create many energy deposition hits.
In the case of a simple histogram, all energy deposition hits across all events are binned.
This gives us a total for the simulation performed and the bin error (uncertainty associated
with a given histogram bin) is proportional to :math:`1/sqrt(N)` where :math:`N` is the
number of entries in that bin. This however, doesn't represent correctly the variation seen
from event to event. Using the per event histograms, a single simple 1D histogram of energy
deposition is created and these are averaged. The resultant histogram has the mean per event
(note the normalisation here versus the simple histograms) and the error on the bin is the
standard error on the beam, i.e.

.. math::
  \mathrm{bin~error} = \frac{\sigma}{\sqrt{n_{events}}}

where :math:`\sigma` is the standard deviation of the values in that bin for all events.

.. note:: Per-entry histograms will only be calculated where there exists 2 or more entries
	  in the tree. In the case of the Event tree, this corresponds to more than 2 events.

.. _output-analysis-configuration-file:
	  
Analysis Configuration File
===========================

The input text file has roughly two sections - options and histogram definitions.

Examples can be found in:

* `<bdsim>/examples/features/io/1_rootevent/analysisConfig.txt`
* `<bdsim>/examples/features/analysis/simpleHistograms/analysisConfig.txt`
* `<bdsim>/examples/features/analysis/perEntryHistograms/analysisConfig.txt`

::

  Debug                                   True
  InputFilePath                           ./output.root
  OutputFileName                          ./ana_1.root
  CalculateOpticalFunctions               True
  CalculateOpticalFunctionsFileName       ./ana_1.dat
  # Object  Tree Name Histogram Name  # of Bins  Binning             Variable            Selection
  Histogram1D  Event.    Primaryx        {100}      {-0.1:0.1}          Primary.x           1
  Histogram1D  Event.    Primaryy        {100}      {-0.1:0.1}          Primary.y           1
  Histogram1D  Options.  seedState       {200}      {0:200}             Options.GMAD::OptionsBase.seed 1
  Histogram1D  Model.    componentLength {100}      {0.0:100}           Model.length        1
  Histogram1D  Run.      runDuration     {1000}     {0:1000}            Info.duration       1
  Histogram2D  Event.    XvsY            {100,100}  {-0.1:0.1,-0.1:0.1} Primary.x:Primary.y 1
  Histogram3D  Event.    PhaseSpace3D    {50,50,50} {-5e-6:5e-6,-5e-6:5e-6,-5e-6:5e-6} Primary.x:Primary.y:Primary.z 1
  Histogram1DLog Event.  PrimaryXAbs     {20}       {-9:-3}      abs(Primary.x)                 1
  Histogram2DLinLog Event. PhaseSpaceAbs {20,20}    {-1e-6:1e-5,-9:-3} Primary.x:abs(Primary.y) 1
  Histogram2DLog    Event. PhaseSpaceAbs2 {20,20}   {-9:-3,-1e-6:1e-5} abs(Primary.x):Primary.y 1


* :code:`HistogramND` defines an N dimension per-entry histogram where `N` is 1,2 or 3.
* :code:`SimpleHistogramND` defines an N dimension simple histogram where `N` is 1,2 or 3.
* Arguments in the histogram rows must not contain any white space!
* Columns in the histogram rows must be separated by any amount of white space (at least one space).
* A line beginning with :code:`#` is ignored as a comment line.
* Empty lines are also ignored.
* For bins and binning, the dimensions are separated by :code:`,`.
* For bins and binning, the range from low to high is specified by :code:`low:high`.
* For a 2D or 3D histogram, x vs. y variables are specified by :code:`samplername.y:samplername.x`. See warning below for order of variables.
* Variables must contain the full 'address' of a variable inside a Tree.
* Variables can also contain a value manipulation, e.g. :code:`1000*(Primary.energy-0.938)` (to get the kinetic energy of proton primaries in MeV).
* The selection is a weight. In the case of the Boolean expression, it is a weight of 1 or 0.
* Selection can be a Boolean operation (e.g. :code:`Primary.x>0`) or simply :code:`1` for all events.
* Multiple Boolean operations can be used e.g. :code:`Primary.x>0&&samplername.ParentID!=0`.
* If a Boolean and a weight is desired, multiply both with the Boolean in brackets e.g.
  :code:`Eloss.energy*(Eloss.S>145.3)`.
* True or False as well as 1 or 0 may be used for Boolean options at the top.

.. note:: Per-entry histograms will only be calculated where there exists 2 or more entries
	  in the tree. In the case of the Event tree, this corresponds to more than 2 events.
	  Whilst the per-entry histograms will work for any tree in the output, they are primarily
	  useful for per-event analysis on the Event tree.

A full explanation on the combination of selection parameters is given in the ROOT TTree class:
`<https://root.cern.ch/doc/master/classTTree.html>`_.  See the "Draw" method and "selection".

Logarithmic Binning
===================

Logarithmic binning may be used by specifying 'Log' after 'HistogramND' for each dimension.
The dimensions specified in order are `x`, `y`, `z`. If a linearly spaced dimension is
required the user should write 'Lin'. If nothing is specified it is assumed to be linear.

Examples::

  Histogram1D       // linearly spaced
  Histogram1DLog    // logarithmically spaced
  Histogram2D       // X and Y are linearly spaced
  Histogram2DLog    // X is logarithmically spaced and Y linearly
  Histgoram2DLinLog // X is linearly spaced and Y logarithmically

The bin lower edges and upper edges should be an exponent of 10. For example to generate
a 1D histogram with 30 logarithmically spaced bins from 1e-3 to 1e3, the following syntax
would be used::

  Histogram1DLog Event. EnergySpectrum {30} {-3:3} Eloss.energy 1

.. warning:: The variable for plotting is really a simple interface to CERN ROOT's TTree Draw
	     method.  This has some inconsistency.  If 1D, there is just `x`.  If 2D, it's
	     `y` : `x`. If 3D, it's `x` : `y` : `z`.  This *only* applies to the variable and
	     not to the bin specification.


Analysis Configuration Options
==============================

The following (case-insensitive) options may be specified in the top part.

.. tabularcolumns:: |p{5cm}|p{10cm}|

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
| CalculateOptics            | Whether to calculate optical functions or not.       |
+----------------------------+------------------------------------------------------+
| OpticsFileName             | The name of a separate text file copy of the         |
|                            | optical functions output.                            |
+----------------------------+------------------------------------------------------+
| EmittanceOnTheFly          | Whether to calculate the emittance freshly at each   |
|                            | sampler or simply use the emittance calculated from  |
|                            | the first sampler (i.e. the primaries). The default  |
|                            | is false and therefore to calculate the emittance at |
|                            | each sampler.                                        |
+----------------------------+------------------------------------------------------+
| PrintModuloFraction        | The fraction of events to print out (default 0.01).  |
|                            | If you require print out for every event, set this   |
|                            | to 0.                                                |
+----------------------------+------------------------------------------------------+
| ProcessSamplers            | Whether to load the sampler data or not.             |
+----------------------------+------------------------------------------------------+
| MergeHistograms            | Whether to merge the event level default histograms  |
|                            | provided by BDSIM. Turning this off will             |
|                            | significantly improve the speed of analysis if only  |
|                            | separate user-defined histograms are desired.        |
+----------------------------+------------------------------------------------------+
| BackwardsCompatible        | ROOT event output files from BDSIM prior to v0.994   |
|                            | do not have the header structure that is used to     |
|                            | ensure the files are the right format and prevent    |
|                            | a segfault from ROOT. If this options is true, the   |
|                            | header will not be checked allowing old files to be  |
|                            | analysed.                                            |
+----------------------------+------------------------------------------------------+


Variables In Data
=================

The variables for histograms are described in :ref:`output-section`. However, the
user can also quickly determine what they want by using a ROOT TBrowser to inspect
a file::

  root output.root
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
* Turn off the MergeHistograms option. If you're only making your own histograms this should
  speed up the analysis considerably for a large number of events.

Simple histograms to not require loading each entry in the tree and an analysis with
only simple histograms will be quicker. Per-entry histograms of course, require loading
each entry.

REBDSIM 'turns off' the loading of all data and only loads what is necessary for the
given analysis.

Scaling Up - Parallelising Analysis
-----------------------------------

For high statistics studies, it's common to run multiple instances of BDSIM with different
seeds (different seeds ensures different results) on a high throughput computer cluster.
There are two possible strategies to efficiently scale the statistics and analysis. Both
produce numerically identical output but make different use of computing resources. The
more data stored per event in the output files, the longer it takes to load it from disk and
the longer the analysis. Similarly, the more events simulated, the longer the analysis will
take. Of course either strategy can be used.

Low Data Volume
===============

If the overall output data volume is relatively low, we recommend analysing all of the
output files at once with rebdsim. In the `Analysis Configuration File`_ file,
the `InputFilePath` should be specified as `"*.root"` to match all the root files
in the current directory.

.. note:: For `"*.root"` all files should be from the same simulation and only BDSIM
	  output files (i.e. not rebdsim output files).

rebdsim will 'chain' the files together to behave as one big file with all of the events.
This is shown schematically in the figure below.

.. figure:: figures/multiple_outputs_rebdsim.pdf
	    :width: 100%
	    :align: center

	    Schematic of strategy for a low volume of data produced from a computationally
	    intense simulation. Multiple instances of BDSIM are executed each producing their
	    own output file. These are analysed all at once with `rebdsim`.

This strategy works best for a relatively low number of events and data volume (example
numbers might be < 10000 events and < 10 GB of data).

High Data Volume
================

In this case, it is better to analyse each output file with rebdsim separately and then
combine the results. In the case of per-event histograms, rebdsim provides the mean
per event along with the error on the mean for the bin error. A separate tool, `rebdsimCombine`
is provided that can combine these rebdsim output files correctly (i.e. the mean of the
mean histograms) to provide the overall mean and error on the mean as if all events had
been analysed in one execution of rebdsim.

The combination of the histograms from the rebdsim output files is very quick in comparison
to the analysis. `rebdsimCombine` is used as follows: ::

  rebdsimCombine <result.root> <file1.root> <file2.root> ....

where `<result.root>` is the desired name of the merge output file and `<file.root>` etc.
are input files to be merged. This workflow is shown schematically in the figure below.

.. figure:: figures/multiple_analyses.pdf
	    :width: 100%
	    :align: center

	    Schematic of strategy for a high data volume analysis. Multiple instances of
	    BDSIM are executed in a script that then executes `rebdsim` with a suitable
	    analysis configuration. Only the output files from `rebdsim` are then combined
	    into a final output identical to what would have been produced from analysing
	    all data at once, but in vastly reduced time.


Further Analysis
----------------

The class used to store and load data in BDSIM are packaged into a library. This library
can be used interactively in Python and ROOT to load the data manually. This is useful
to prepare a more involved analysis.

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

  root> gSystem->Load("librebdsimLib");

Loading this library exposes all classes that are found in :code:`<bdsim>/analysis`. If you
are familiar with ROOT, you may use the ROOT file as you would any other given the
classes provided by the library::

  root> TFile* f = new TFile("output.root", "READ");
  root> TTree* eventTree = (TTree*)f->Get("Event");
  root> BDSOutputROOTEventLoss* elosslocal = new BDSOutputROOTEventLoss();
  root> eventTree->SetBranchAddress("Eloss.", &elosslocal);
  root> eventTree->GetEntry(0);
  root> cout << elosslocal->n << endl;
        345
  root>

The header (".hh") files in :code:`<bdsim>/analysis` provide the contents and abilities
of each class.

This would of course be fairly tedious to load all the structures in the output. Therefore,
a data loader class is provided that constructs local instances of all the objects and
sets the branch address on them (links them to the open file). For example::

  root> gSystem->Load("librebdsimLib");
  root> DataLoader* dl = new DataLoader("output.root");
  root> Event* evt = dl->GetEvent();
  root> TTree* evtTree = dl->GetEventTree();

Here, a file is loaded and by default all data is loaded in the file. We get access to
the local event object and the event tree (here, a chain of all files). We can then load
a particular entry in the tree, which for the Event tree is an individual event::

  root> evtTree->GetEntry(10);

The event object now contains the data loaded from the file. ::

  root> evt->Eloss.n
  (int_t) 430

For our example, the file has 430 entries of energy loss for event \#10. The analysis loading
classes are designed to have the same structure as the output file. Look at
`bdsim/analysis/Event.hh` to see what objects the class has.

One may manually loop over the events in a macro::

  void DoLoop()
  {
    gSystem->Load("librebdsimLib");
    DataLoader* dl = new DataLoader("output.root");
    Event* evt = dl->GetEvent();
    TTree* evtTree = dl->GetEventTree()
    int nentries = (int)evtTree->GetEntries();
    for (int i = 0; i < nentries; ++i)
      {
        evtTree->GetEntry(i)
	std::cout << evt->Eloss.n >> std::endl;
      }
  }

  root> .L myMacro.C
  root> DoLoop()


This would loop over all entries and print the number of energy deposition hits per
event.

Samplers are dynamically added to the output based on the names the user decides in
their input accelerator model. The names of the samplers can be accessed from the
DataLoader class::

  std::vector<std::string> samplerNames = dl->GetSamplerNames();


The following classes are used for data loading and can be found in `bdsim/analysis`:

* DataLoader.hh
* Beam.hh
* Event.hh
* Header.hh
* Model.hh
* Options.hh
* Run.hh


ROOT trees as Numpy Arrays
--------------------------

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

  pip install root_numpy

.. warning:: Your system may have multiple versions of Python with their respective PIP. Make
	     sure you use the same version you use for ROOT.

Extracting data from ROOT file ::

   > pylab
   In [1]: import ROOT
   In [2]: import root_numpy
   In [3]: f = ROOT.TFile("analysis.root")
   In [4]: t = f.Get("Sampler1")
   In [5]: a = root_numpy.tree2rec(t)


Numerically Stable Calculation of Mean \& Variance
--------------------------------------------------

To calculate the mean in the per-entry histograms as well as the associated error
(the standard error on the mean), the following formulae are used:

.. math::

   \bar{x} &= \sum_{i = 0}^{n} x_{i}\\
   \sigma_{\bar{x}} &= \frac{1}{\sqrt{n}}\sigma = \frac{1}{\sqrt{n}}\sqrt{\frac{1}{n}\sum_{i = 0}^{n}(x_{i} - \bar{x})^2 }

These equations are however problematic to implement computationally. The formula above
for the variance requires two passes through the data to first calculate the mean and
then the variance using that mean. The above equation can rearranged to provide the same
calculation with a single pass through the data, however such algorithms are typically
numerically unstable, i.e. they rely on a small difference between two very large numbers.
With the finite precision of a number represented in a C++ double type (~15 significant
digits), the instability may lead to unphysical results (negative variances) and generally
incorrect results.

The algorithm used in REBDSIM to calculate the means and variances is an online, single-pass
numerically stable one. This means that the variance is calculated as each data point
is accumulated, it requires only one pass of the data, and does not suffer numerical instability.
To calculate the mean, the following recurrence relation is used:

.. math::

   \bar{x}_{i = 0} &= 0\\
   \bar{x}_{i+1} &= \bar{x}_{i} + \frac{(x - \bar{x}_{i})}{i}\\

   \mathrm{for}~ i~ [1\, ...\, n_{event}]


The variance is calculated with the following recurrence relation that requires the above
online mean calculation:

.. math::

   Var\,(x)_{i = 0} &= 0 \\
   Var\,(x)_{i+1} &= Var\,(x)_{i} + (x - \bar{x}_{i})\,(x - \bar{x}_{i+1})\\

   \mathrm{for}~ i~ [1\, ... \,n_{event}]

After processing all entries, the variance is used to calculate the standard error on the mean
with:

.. math::

   \sigma_{\bar{x}} = \frac{1}{\sqrt{n}}\sqrt{\frac{1}{\sqrt{n-1}} Var\,(x)}


Merging Histograms
==================

`rebdsimCombine` merges histograms that already have the mean and the error on the
mean in each bin. These are combined with a separate algorithm that is also numerically
stable.

The mean is calculated as:

.. math::

   \bar{x}_{i = 0} &= 0\\
   \delta &= x_{i+1} - \bar{x}_{i}\\
   \bar{x}_{i+1} &= \bar{x}_{i} + n_{i+1}\frac{\delta}{n_{i} + n_{i+1}}


.. math::

   Var\,(x)_{i = 0} &= 0 \\
   Var\,(x)_{i+1} &= Var\,(x)_{i} + Var\,(x)_{i+1} + (n_{i}\,n_{i+1} \frac{\delta^{2}}{n_{i} + n_{i+1}})


.. math::

   \mathrm{for}~ i~ [1\, ... \,n_{rebdsim\, files}]
