.. _example-collimation-advanced:

Collimation
===========

Preparation
-----------

* BDSIM has been compiled and installed.
* The (DY)LD_LIBRARY_PATH and ROOT_INCLUDE_PATH environmental variables are set as
  described in :ref:`output-analysis-setup`.
* ROOT can be imported in Python
* `pybdsim` has been installed.

Description
-----------

This is an example to show how to turn on extra collimation output and
how to analyse the data. This example shows information most relevant
to a collimation system. A basic version of this example exists that
shows general data exploration and analysis (see
:ref:`example-collimation-simple`). This example focusses on more
detailed collimation specific information.

The model consists of two collimators, followed by a triplet set of
quadrupoles, a shielding wall and a third collimator. The colliamtors
are made of successively denser material (carbon, copper and tungsten).
This looks like:

.. figure:: collimation-10events.png
	    :width: 90%
	    :align: center

Model Preparation
-----------------

The model is fictional and designed to show the relevant features of
BDSIM. As the beam line was relatively short, the model was written by
hand without any automatic conversion.

The files can be found in :code:`bdsim/examples/collimation`:

* collimation.gmad - model for beam losses and collimation
* collimationOptics.gmad - model for Gaussian beam for optics


Optics
------

To understand how this machine transports particles, it is useful to
simulate a Gaussian beam that would nominally represent some 'core'
beam that would be expected in the machine. Here we expect the losses
to be very low. A specific bdsim input file is included that chooses
a Gaussian distribution according to chosen Twiss parameters. Secondary
partilces are stopped and the distribution is recorded after every
beam line element with the :code:`sample, all` command. Running this
optics model takes around 10s to run 5000 particles on the developer's
computer.::

  bdsim --file=collimationOptics.gmad --outfile=o1 --batch --ngenerate=5000

.. note:: It is recommend to run at least 1000 particles for optical funciton
	  evaluation and 10000 to 50000 if high accuracy is desired or
	  a large energy spread is defined.
  
This produces an output file called "o1.root". We can then calculate
the optical functions and sizes of the beam after each element using
the included rebdsimOptics tool.::

  rebdsimOptics o1.root o1-optics.root

This creates another file called "o1-optics.root" that contains only the
optical function information. The Python utility `pybdsim` can be used
to viusalise the data.::

  ipython
  >>> import pybdsim
  >>> pybdsim.Plot.BDSIMOptics("o1-optics.root")

This produces a series of graphs showing, for example, the mean, sigma, divergence
and dispersion of the beam. The sigma and dispersion are shown below.

.. figure:: collimation-optics-sigma.pdf
	    :width: 90%
	    :align: center

.. figure:: collimation-optics-dispersion.pdf
	    :width: 90%
	    :align: center


Losses
------

Of course, more interesting than the optical functions is the possibility of
beam losses. To illustrate this, we choose a beam distribution that is a circular
ring of particles close to the edge of the collimator. Most will hit the first collimator
but around 1/3 will make it through at the edges as the first collimator is square.
We should generate some events to investigate the beam losses. The below command
generates 2000 events (2000 primary particles), which takes approximately 30s on the
developer's computer.::

  bdsim --file=collimation.gmad --outfile=data1 --batch --ngenerate=2000

This produces an output file called "data1.root", which is approximately 20Mb.
Firstly, we might like to quickly see if there were any losses at all and if
there was any energy deposition. This can be done by browsing the output data
file as described in :ref:`basic-data-inspection`, however, we'd like to look
at the average energy loss and impacts quickly. Histograms of the primary
particle impact and loss points as well as energy deposition are included by
default per event in the output. A tool for conveninence (`rebdsimHistoMerge`)
allows averaging of these quickly as opposed to running `rebdsim` with an
analysis configuration text file. This is run as follows::

  rebdsimHistoMerge data1.root data1-histos.root

This produces an output file called "data1-histos.root" that contains the
merged histograms. This too can also be viewed with a TBrowser in ROOT
as described in :ref:`basic-data-inspection`, however, here we will make
more standardised plots using `pybdsim` in Python.::

  ipython
  >>> import pybdsim
  >>> pybdsim.Plot.LossAndEnergyDeposition("data1-histos.root", hitslegendloc='upper center')
  >>> pybdsim.Plot.EnergyDeposition("data1-histos.root")

In the case of the first command, the legend overlaps with an expected data point, so
we move it - this is optional (limitation of plotting library). These produce the following
plots of primary hits, losses and energy deposition and secondly just energy deposition.

.. figure:: collimation-losses-2k.pdf
	    :width: 90%
	    :align: center

.. figure:: collimation-energy-deposition-2k.pdf
	    :width: 90%
	    :align: center

As expected we see a large fraction of particle impact the first collimator and we see
some energy deposition throughout. Now, we can perform a more advanced analysis to
learn about these impacts and losses in the collimation system.

Questions Answered
------------------

1) Where are particles absorbed that impact the first collimator?
2) Where do particles impact that don't impact the first collimator?
3) What secondaries make it through the shielding wall created from
   impacts on the first collimator?

Question 1
**********

* Where are particles absorbed that impact the first collimator?

We want to histogram the absorption point of the primary particle in each event but
only for the events where the primary impact was in the first collimator. We always
record the primary first hit point and the loss point, but here we make use of the
collimator specific information. The first collimator is called "c1" and the collimator
hits are stored under the "COLL_c1_0" branch of the Event tree.

.. note:: The name of the collimator is prefixed with "COLL_" to distinguish it
	  from a sampler which would have the name "c1". The suffix "_0" is because
	  it's the 0th placement of that collimator in the beam line.

In this collimator structure in the output there is a variable "primaryInteracted". This
is a Boolean which is true if the primary particle interacted with the material of the
collimator on that event. We use this as a 'selection' in the histogram. We prepare
an analysis configuration text file for rebdsim (see :ref:`analysis-preparing-analysis-config`).
We can start from an example in BDSIM and edit that one. An example can be found in
:code:`bdsim/examples/features/analysis/perEntryHistograms/analysisConfig.txt`.

The variables in the histogram specification must be exactly as you see in the output
file so it's useful to use a TBrowser in ROOT to browse the file while preparing the
analysis configuation file. The following is the desired histogram specification.::
  
  # Object     treeName Histogram Name       #Bins Binning  Variable          Selection
  Histogram1D  Event.   C1ImpactLossLocation {96}  {0:12}   PrimaryLastHit.S  COLL_c1_0.primaryInteracted

.. note:: Take not of the "." in the variable names.

An example analysis configuration file is included in
:code:`bdsim/examples/collimation/analysisConfig.txt` that contains the histograms
for this and subsequent questions.

Question 2
**********

* Where do particles impact that don't impact the first collimator?

Similarly, we want to histogram the impact location, so `PrimaryFirstHit.S`, but for
only the events where the primary particle didn't impact the first collimator. Again,
we use a selection in the histogram specification.::

  # Object     treeName Histogram Name       #Bins Binning  Variable          Selection
  Histogram1D  Event.   NoC1ImpactLossLocation {96}  {0:12}  PrimaryLastHit.S  COLL_c1_0.primaryInteracted==0


 # Object     treeName Histogram Name       #Bins Binning  Variable          Selection
  Histo
