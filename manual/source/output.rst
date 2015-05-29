.. _output-section:

======
Output
======

Output from BDSIM can be in various formats. Details about each one are listed
below in `output format differences`_.  As a general guideline, the following
naming conventions are used:

========== ================
Short Name Meaning
========== ================
phits      primary hits
ploss      primary losses
eloss      energy loss
PE         per element
========== ================

.. note:: A "hit" is the point of first contact, whereas a "loss" is the
	  last point that particle existed - in the case of a primary it
	  is where it stopped being a primary.

.. note:: Energy loss is the energy deposited by particles along their step

Output Format Differences
-------------------------

ROOT
^^^^

With the root format, everything is recorded in one single file. If the
number of events simulated exceeds :code:`nperfile` a new file will be
started. The chosen filename will be suffixed with :code:`_N.root` where
:code:`N` is an integer.

* Histograms are stored as TH1F objects within the file
* Each sampler has its own Tree

ASCII Output
^^^^^^^^^^^^

With ASCII output, a folder is created with the given output name. Inside this
histograms and sampler output is produced in different text files.

* Histograms are suffixed with :code:`.hist.txt.`
* The file with only :code:`.txt` is the main output from all samplers
* The sampler output is recorded in simulation order, not spatial order

Histograms
----------

BDSIM produces six histograms by default during the simulation. These are: primary
hits per bin width; primary losses per bin width; energy loss per metre (GeV);
primary hits per element; primary losses per element; and Energy loss per element.

The per element histograms are integrated across the length of each element so they
will have a different bin width. The other histograms are evenly binned according
to the option :code:`elossHistoBinWidth` (in metres).

.. note:: Histograms are only written to disk once all events have been simulated.

Samplers
--------

Samplers record the particle position at the start of each element.  The following
coordinates are recorded:

=============== ============= ==================
Coordinate Name Units         Meaning
=============== ============= ==================
E0              GeV           Initial energy
x0              :math:`\mu m` Initial x position
y0              :math:`\mu m` Initial y position
z0              :math:`\mu m` Initial z position
xp0             rad           Initial x' position
yp0             rad           Initial y' position
zp0             rad           Initial z' position
t0              s             Initial time
E               GeV           Energy
x               :math:`\mu m` x position
y               :math:`\mu m` y position
z               :math:`\mu m` z position
xp              rad           x' position
yp              rad           y' position
zp              rad           z' position
t               s             Time
X               :math:`\mu m` Global x position
Y               :math:`\mu m` Global y position
Z               :math:`\mu m` Global z position
Xp              rad           Global x' position
Yp              rad           Global y' position
Zp              rad           Global z' position
s               m             Curvilinear S
weight          NA            weight
partID          NA            PDG ID number
nEvent          NA            event number
parentID        NA            parent ID
trackID         NA            track ID
turnnumber      NA            turns completed
=============== ============= ==================

.. note:: `rad` is not strictly correct for the prime units but is used in the small angle approximation.
	  The prime is the differential of that position


Primary Coordinates
-------------------

The primary coordinates for each event are recorded in a similar fashion to the samplers
in their own file / tree.

	       
