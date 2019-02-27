.. _example-worked-target:

Analysis of Thin Target Products
================================

* Based on :code:`bdsim/examples/target/target-p-cu.gmad`
  
.. figure:: target-p-cu-10events.png
	    :width: 80%
	    :align: center

  
Contents
--------

* `Preparation`_
* `Model Description`_
* `Generating Data`_
* `Questions Answered`_
  
Preparation
-----------

* BDSIM has been compiled and installed.
* The (DY)LD_LIBRARY_PATH and ROOT_INCLUDE_PATH environmental variables are set as
  described in :ref:`output-analysis-setup`.
* ROOT can be imported in Python
* `pybdsim` has been installed.

Model Description
-----------------

This example shows a high energy (6.5 TeV) proton beam hitting a
5cm copper target. This produces a large number of secondary particles.
This examples illustrates the analysis and understanding of the products
in an invisible 'sampler' plane after the target.

The model consists of a small 1 cm vacuum pipe followed by a target. The
solid box of target material is created by creating a collimator (`rcol`)
with no specified aperture (`xsize` and `ysize`) and so a closed collimator
is built (a solid box of material). The width of the target is the default
`horizontalWidth` for all elements, which is 0.5 m. See :ref:`options-common`.
The model looks like:

.. figure:: target-p-cu.png
	    :width: 90%
	    :align: center

The input files for BDSIM can be found in :code:`<bdsim>/examples/target/`.

* target-p-cu.gmad - model discussed here
* target-p-cu-sampler-extras.gmad - same model but with extra sampler output turned on.
* target-ion-w.gmad - tungsten target and 1 TeV carbon ion beam.
* target-ion-w-sampler-extras.gmad - similar but with extra sampler output.

The model was prepared by hand as it is quite simple. The input syntax is included
below: ::

 d1: drift, l=10*cm;
 c1: rcol, l=5*cm, material="copper";
 l1: line=(d1,c1);
 use, l1;

 sample, all;

 beam, particle="proton",
       energy=6.5*TeV;

 option, physicsList="g4FTFP_BERT", 
         defaultRangeCut=3*cm;

 option, ngenerate=5,
         seed=123,
         elossHistoBinWidth=1*cm;

First we define the beam line elements we want and then the line (sequence) of the them.
The :code:`use` command specifies which line to use. :code:`sample, all;` tells BDSIM
to put a `sampler` after each element. A sampler is an invisible 1 nm long plane that is
5 m wide by default that records the passage of any particle in any direction through it
(so both forwards and backwards). We then specify a beam, the physics processes to use
and some options.

Here we use a Geant4 reference physics list :code:`g4FTFP_BERT` that is a standard high
energy physics list suitable for LHC-like applications. See :ref:`physics-geant4-lists`.

We choose a default range cut to speed things up a little for the purposes of the example.
The range cut is roughly a distance a secondary particle would have to travel and survive
in that material. If it wouldn't travel this distance, it isn't produced and the energy
is recorded at that location. This therefore approximately corresponds to an energy cut
per particle species per material. This is much more accurate than a general energy cut,
but care should be taken not to choose a length scale longer than the typical element
length as you may see discontinuities in energy deposition close to boundaries. The default
in Geant4 is 1 mm that roughly corresponds to ~keV energies for most particles.

Specifying a seed allows the user to reproduce the simulation exactly that's demonstrated
here.

The :code:`elossHistoBinWidth` option specifies the binning along the beam line for the
default energy deposition ("eloss") histogram that's created as BDSIM runs. The default
is 1 m as BDSIM is typically used for large accelerators. In this case, our whole model
would fit in one bin, which wouldn't be so interesting. Therefore, we specify a smaller
bin width.  See :ref:`options-common`.

Generating Data
---------------

Here, we generate a sample of data to be analysed. Run BDSIM with the input model and
specify a number of events (i.e. number of primary beam particles) to simulate. We also
specify what we want the output data file to be called. ::

  bdsim --file=target-p-cu.gmad --outfile=data1 --batch --ngenerate=200

The :code:`--batch` option means that no visualiser is used and the events are simulated
and BDSIM finishes.

This creates an output ROOT file called `data1.root`. On the developer's laptop, this
took approximately 8 seconds to run in total.

The model can also be run interactively, but given the large number of secondary particles
it is advisable to run a low number of events e.g. 1 to 10. In this case you would run
BDSIM as follows: ::

  bdsim --file=target-p-cu.gmad --outfile=data1

Once the visualiser starts, enter the following command in the session terminal prompt
at the bottom. (see :ref:`running-interactively`). ::

  /run/beamOn 5

A visualisation of 10 events can be seen at the beginning of this example.

Some of the terminal print out (which is kept to be as minimal as possible in BDSIM) is
worth noting. The beam print out shows the kinematics of the beam particle chosen. ::

  Design particle properties: 
  Particle:       "proton"
  Mass:            0.938272 GeV
  Charge:          1 e
  Total Energy:    6500 GeV
  Kinetic Energy:  6499.06 GeV
  Momentum:        6500 GeV
  Gamma:           6927.63
  Beta:            1
  Rigidity (Brho): 21681.7 T*m

There is then a large print out of physics processes from Geant4. However, at the end
a "Table of Registered Couples" is of interest. This indicates the range cuts for basic
particle types and the corresponding energies calculated. These energies do not vary
linearly with range. We can use this as an idea of the approximate energy scale. ::

  ========= Table of registered couples ==============================

  Index : 0     used in the geometry : Yes
   Material : G4_AIR
   Range cuts        :  gamma  3 cm     e-  3 cm     e+  3 cm  proton 3 cm 
   Energy thresholds :  gamma  990 eV     e-  43.5254 keV    e+  43.2339 keV proton 3 MeV
   Region(s) which use this couple : 
      DefaultRegionForTheWorld
 
  Index : 1     used in the geometry : Yes
   Material : G4_Galactic
   Range cuts        :  gamma  3 cm     e-  3 cm     e+  3 cm  proton 3 cm 
   Energy thresholds :  gamma  990 eV     e-  990 eV     e+  990 eV  proton 3 MeV
   Region(s) which use this couple : 
      DefaultRegionForTheWorld 
 
  Index : 2     used in the geometry : Yes
   Material : vacuum
   Range cuts        :  gamma  3 cm     e-  3 cm     e+  3 cm  proton 3 cm 
   Energy thresholds :  gamma  990 eV     e-  990 eV     e+  990 eV  proton 3 MeV
   Region(s) which use this couple : 
      DefaultRegionForTheWorld
 
  Index : 3     used in the geometry : Yes
   Material : stainlesssteel
   Range cuts        :  gamma  3 cm     e-  3 cm     e+  3 cm  proton 3 cm 
   Energy thresholds :  gamma  109.289 keV    e-  46.8866 MeV    e+  43.5457 MeV proton 3 MeV
   Region(s) which use this couple : 
      DefaultRegionForTheWorld
 
  Index : 4     used in the geometry : Yes
   Material : G4_Cu
   Range cuts        :  gamma  3 cm     e-  3 cm     e+  3 cm  proton 3 cm 
   Energy thresholds :  gamma  128.416 keV    e-  52.2089 MeV    e+  48.4888 MeV proton 3 MeV
   Region(s) which use this couple : 
      DefaultRegionForTheWorld
 
  ====================================================================


Questions Answered
------------------

1) What fraction of the beam makes it through the target?
2) What spectrum of particles comes out after the target?
3) How much energy is produced or reflected backwards from the target?
