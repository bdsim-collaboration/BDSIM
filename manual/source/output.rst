.. _output-section:

======
Output
======

Output from BDSIM can be in various formats.

+-------------------+-----------------------+----------------------------------------------------------------------------+
| Format            | Syntax                | Description                                                                |
+===================+=======================+============================================================================+
| None              | --output=none         | No output is written                                                       |
+-------------------+-----------------------+----------------------------------------------------------------------------+
| ASCII             | --output=ascii        | A series of text files in a folder are written                             |
+-------------------+-----------------------+----------------------------------------------------------------------------+
| ROOT              | --output=root         | A root file with a tree for each sampler is written                        |
+-------------------+-----------------------+----------------------------------------------------------------------------+
| ROOT (detailed)   | --output=rootdetailed | Similar to the above ROOT format but with extra variables for more detail  |
+-------------------+-----------------------+----------------------------------------------------------------------------+
| ROOT (per event)  | --output=rootevent    | A root file with a per event structure format                              |
+-------------------+-----------------------+----------------------------------------------------------------------------+
| Multiple          | --output=combined     | All formats written at once                                                |
+-------------------+-----------------------+----------------------------------------------------------------------------+

Details about each one are listed below in `output format differences`_.
As a general guideline, the following naming conventions are used:

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

With the ROOT format, everything is recorded in one single file. If the
number of events simulated exceeds :code:`nperfile` a new file will be
started. The chosen filename will be suffixed with :code:`_N.root` where
:code:`N` is an integer.

* Histograms are stored as TH1F objects within the file
* Each sampler has its own Tree

ASCII Output
^^^^^^^^^^^^

With ASCII output, a folder is created with the given output name. Inside this
histograms and sampler output are produced in different text files.

* Histograms are suffixed with :code:`.hist.txt.`
* The file with only :code:`.txt` is the main output from all samplers
* The sampler output is recorded in simulation order, not spatial order

The ASCII output is relatively limited compared to the root output.

* The main :code:`filename.txt` file contains hits on samplers in the order they
  happened and are not grouped by sampler.
* In the energy loss, primary hits and primary loss files, `x'` and `y'` are always
  0 as they are undefined in these cases.
* In all files, local `x` and `y` are used whereas global `Z` is used.

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
variables are recorded:

=============== ============= ===================================
Variable Name   Units         Meaning
=============== ============= ===================================
E0              GeV           Initial primary energy
x0              m             Initial primary global x position
y0              m             Initial primary global y position
z0              m             Initial primary global z position
xp0             rad           Initial primary global x' position
yp0             rad           Initial primary global y' position
zp0             rad           Initial primary global z' position
t0              ns            Initial primary global time
E               GeV           Total current energy
x               m             Local x position
y               m             Local y position
z               m             Local z position
xp              rad           Local x' position
yp              rad           Local y' position
zp              rad           Local z' position
t               ns            Time of flight
X               m             Global x position
Y               m             Global y position
Z               m             Global z position
Xp              rad           Global x' position
Yp              rad           Global y' position
Zp              rad           Global z' position
s               m             Curvilinear S
weight          NA            Weight
partID          NA            PDG ID number
nEvent          NA            Event number
parentID        NA            Parent ID (0 means primary)
trackID         NA            Track ID
turnnumber      NA            Turns completed
=============== ============= ===================================

.. note:: `rad` is not strictly correct for the prime units but is used in the small angle approximation.
	  The prime is the differential of that position


Primary Coordinates
-------------------

The primary coordinates for each event are recorded in a similar fashion to the samplers
in their own file / tree.

.. warning:: A common issue is apparently half of the particles missing in the first sampler in
	     the beam line. If a sampler is placed at the beginning of the beam line and a bunch
	     distribution with a finite z width is used, approximately half of the particles will
	     start in front of the sampler, never pass through it and never be registered. For this
	     reason, putting a sampler at the beginning of a beam line should be avoided to avoid
	     confusion. The primary output (either separate file in ASCII or as a tree in root) records
	     all primary coordinates before they enter the tracking in the geometry, so it always
	     contains all primary particles.
	     
