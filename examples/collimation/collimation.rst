.. _collimation-advanced:

Collimation
===========

This is an example to show how to turn on extra collimation output and
how to analyse the data. This example shows information most relevant
to a collimation system.

The model consists of two collimators, followed by a triplet set of
quadrupoles, a shielding wall and a third collimator. The colliamtors
are made of successively denser material (carbon, copper and tungsten).

Model Preparation
-----------------

The model is fictional and designed to show the relevant features of
BDSIM. As the beam line was relatively short, the model was written by
hand without any automatic conversion.

Files:

* collimation.gmad - model for beam losses and collimation
* collimationOptics.gmad - model for Gaussian beam for optics


Optics
------

To understand how this machine transports particles, it is useful to
simulate a Gaussian beam that would nominally represent some 'core'
beam that would be expected in the machine. Here we expect the losses
to be very low. A specific bdsim input file is included that chooses
a Gaussian distribution accordint to Twiss parameters. Secondary
partilces are stopped and the distribution is recorded after every
beam line element with the :code:`sample, all` command. Running this
optics model takes around 10s to run 5000 particles on the developer's
computer.::

  bdsim --file=collimationOptics.gmad --outfile=o1 --batch --ngenerate=5000

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

.. note:: The setup for the analysis tools should have been done


Losses
------

1000 events in around 20s.


How to run::

  bdsim --file=air.gmad --vis_debug

.. figure:: air.png
	    :width: 100%
