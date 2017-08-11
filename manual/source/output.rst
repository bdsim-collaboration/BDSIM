.. _output-section:

======
Output
======

This section describes the output from BDSIM. This
section only describes the structure. Loading and analysis instructions can be found
in :ref:`output-analysis-section`.

The output format 'rootevent' is written to a ROOT file. This format
is preferred as it lends itself nicely to particle physics information; is stored as compressed
binary internally; and can store and load complex custom structures.

Units, unless specified, are SI (i.e. m, rad), plus energy in GeV and time in nanoseconds.
Small letters denote local (to that object) coordinates whereas capitals represent
global coordinates.

.. tabularcolumns:: |p{0.2\textwidth}|p{0.2\textwidth}|p{0.5\textwidth}|
	     
+----------------------+-----------------------------+----------------------------------------------------------------------------+
| Format               | Syntax                      | Description                                                                |
+======================+=============================+============================================================================+
| None                 | --output=none               | No output is written                                                       |
+----------------------+-----------------------------+----------------------------------------------------------------------------+
| ROOT Event (Default) | --output=rootevent          | A ROOT file with details of the model built, options used, seed states,    |
|                      |                             | and event by event information (default and recommended).                  |
+----------------------+-----------------------------+----------------------------------------------------------------------------+

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

.. note:: Energy loss is the energy deposited by particles along their step.

Structure Of Output
-------------------

BDSIM uses a series of classes to accumulate information about a Geant4 Run and Event.
These are stored directly in the file so that the same classes can be used by the output
analysis tool (rebdsim) to read and process the data. A BDSIM ROOT event file has the
following structure.

.. figure:: figures/rootevent_contents.png
	    :width: 40%
	    :align: center

The file consists of four ROOT 'trees' each with 'branches' that represent instances
of the BDSIM classes.  The trees are:

.. tabularcolumns:: |p{0.2\textwidth}|p{0.8\textwidth}|

+-------------+---------------------------------------------------------------------+
| Tree Name   | Description                                                         |
+=============+=====================================================================+
| Options     | A record of all options used by BDSIM.                              |
+-------------+---------------------------------------------------------------------+
| Model       | A record of the lengths and placement transforms of every element   |
|             | built by BDSIM in the accelerator beam line suitable for recreating |
|             | global coordinates or visualising trajectories.                     |
+-------------+---------------------------------------------------------------------+
| Run         | Information collected per Run.                                      |
+-------------+---------------------------------------------------------------------+
| Event       | Information collected per Event                                     |
+-------------+---------------------------------------------------------------------+

Options Tree
^^^^^^^^^^^^

.. figure:: figures/rootevent_options_tree.png
	    :width: 50%
	    :align: center

The options tree contains a single branch called "Options." (note the "."). This branch
represents an instance of :code:`parser/OptionsBase.hh`. The Tree typically contains one
entry as only one set of options were used per execution of BDSIM.

Model Tree
^^^^^^^^^^

.. figure:: figures/rootevent_model_tree.png
	    :width: 40%
	    :align: center

This tree contains a single branch called "Model.".  This branch represents and instance
of :code:`include/BDSOutputROOTEventModel.hh`. There is also typically one entry as there
is one model.  Note some variables here appear as 'leaf' icons and some as 'branch icons.
This is because some of the variables are vectors.


Run Tree
^^^^^^^^

.. figure:: figures/rootevent_run_tree.png
	    :width: 40%
	    :align: center

This tree contains two branches called "Histos." and "Info." which represent instances of
:code:`include/BDSOutputROOTEventHistograms.hh` and :code:`include/BSOutputROOTEventInfo`
respectively. Histos contains two vectors of 1D and 2D histograms that are produced per run.

Event Tree
^^^^^^^^^^

.. figure:: figures/rootevent_event_tree.png
	    :width: 35%
	    :align: center

This tree contains information on a per event basis.  Everything shown in the above tree has a
different value per event run in BDSIM.

.. tabularcolumns:: |p{0.15\textwidth}|p{0.35\textwidth}|p{0.4\textwidth}|

+-----------------+----------------------------------+--------------------------------------------------+
| Branch Name     | Type                             | Description                                      |
+=================+==================================+==================================================+
| Info            | BDSOutputROOTEventInfo           | Per event information.                           |
+-----------------+----------------------------------+--------------------------------------------------+
| Primary         | BDSOutputROOTEventSampler<float> | A record of the coordinates at the start of the  |
|                 |                                  | simulation, before tracking.                     |
+-----------------+----------------------------------+--------------------------------------------------+
| Eloss           | BDSOutputROOTEventLoss           | Coordinates of energy deposition in the          |
|                 |                                  | accelerator material.                            |
+-----------------+----------------------------------+--------------------------------------------------+
| PrimaryFirstHit | BDSOutputROOTEventLoss           | Energy deposit 'hit' representing the first      |
|                 |                                  | the process associated with the primary step is  |
|                 |                                  | not tracking - ie the first interaction.         |
+-----------------+----------------------------------+--------------------------------------------------+
| PrimaryLastHit  | BDSOutputROOTEventLoss           | Similar to PrimaryFirstHit, but the last point   |
|                 |                                  | of this type on the primary trajectory.          |
+-----------------+----------------------------------+--------------------------------------------------+
| TunnelHit       | BDSOutputROOTEventLoss           | Coordinates of energy deposition in the tunnel   |
|                 |                                  | material.                                        |
+-----------------+----------------------------------+--------------------------------------------------+
| Trajectory      | BDSOutputROOTEventTrajectory     | A record of all the steps the primary particle   |
|                 |                                  | took and the associated physics processes.       |
+-----------------+----------------------------------+--------------------------------------------------+
| Histos          | BDSOutputROOTEventHistograms     | Per event histograms in vectors.                 |
+-----------------+----------------------------------+--------------------------------------------------+
| xxxxx           | BDSOutputROOTEventSampler<float> | A dynamically generated branch created per       |
|                 |                                  | sampler (here named 'xxxxx') that contains a     |
|                 |                                  | record of all particles that passed through the  |
|                 |                                  | sampler during the event. Note this includes     |
|                 |                                  | both primary and secondary particles. More       |
|                 |                                  | in `Histograms`_.                                |
+-----------------+----------------------------------+--------------------------------------------------+

The types and names of the contents of each class can be found in the header files in
:code:`bdsim/include/BDSOutputROOTEvent*.hh`. The sampler data is the most commonly used
and so a more detailed description is provided here.

Note, the sampler structure, like everything else in the :code:`Event.` Tree is stored
per event.  However, for a given event, there may be multiple hits on a sampler, i.e.
many secondary particles may have passed through a sampler. For this purpose most variables
are vectors of numbers, where the vector is all the hits in that event.

As the sampler is considered infinitely thin and always in the same place, there is no
point in storing the z location or the S location for every particle hit. Therefore,
these variables are only stored once as a single number per event.  

+-----------------+--------------------------------------------------------------------------+
|  **Variable**   |  **Description**                                                         |
+=================+==========================================================================+
| n               | The number of this in this event in this sampler.                        |
+-----------------+--------------------------------------------------------------------------+
| energy          | Vector of the total energy (GeV) of each hit in this sampler.            |
+-----------------+--------------------------------------------------------------------------+
| x               | Vector of the x coordinate of each hit (m).                              |
+-----------------+--------------------------------------------------------------------------+
| y               | Vector of the y coordinate of each hit (m).                              |
+-----------------+--------------------------------------------------------------------------+
| z               | Single entry of z for this sampler (m).                                  |
+-----------------+--------------------------------------------------------------------------+
| xp              | Vector of the fractional x transverse momentum.                          |
+-----------------+--------------------------------------------------------------------------+
| yp              | Vector of the fractional y transverse momentum.                          |
+-----------------+--------------------------------------------------------------------------+
| zp              | Vector of the fractional forward momentum.                               |
+-----------------+--------------------------------------------------------------------------+
| t               | Vector of the time of flight of the particle (ns).                       |
+-----------------+--------------------------------------------------------------------------+
| weight          | Vector of the associated weights of the hits.                            |
+-----------------+--------------------------------------------------------------------------+
| partID          | Vector of the PDG ID for the particle of each hit.                       |
+-----------------+--------------------------------------------------------------------------+
| parentID        | Vector of the trackID of the progenitor of the particle that hit.        |
+-----------------+--------------------------------------------------------------------------+
| trackID         | Vector of the trackID of the particle that hit.                          |
+-----------------+--------------------------------------------------------------------------+
| modelID         | The index to the BDSIM model of which element the sampler belonged to.   |
+-----------------+--------------------------------------------------------------------------+
| turnNumber      | Vector of the turn number of the particle that hit.                      |
+-----------------+--------------------------------------------------------------------------+
| S               | S position of the sampler (m).                                           |
+-----------------+--------------------------------------------------------------------------+


.. warning:: A common issue is apparently half of the particles missing in the first sampler in
	     the beam line. If a sampler is placed at the beginning of the beam line and a bunch
	     distribution with a finite z width is used, approximately half of the particles will
	     start in front of the sampler, never pass through it and never be registered. For this
	     reason, putting a sampler at the beginning of a beam line should be avoided to avoid
	     confusion. The primary output records
	     all primary coordinates before they enter the tracking in the geometry, so it always
	     contains all primary particles.

Histograms
----------

BDSIM produces six histograms by default during the simulation. These are: primary
hits per bin width; primary losses per bin width; energy loss per metre (GeV);
primary hits per element; primary losses per element; and Energy loss per element.

The per element histograms are integrated across the length of each element so they
will have a different bin width. The other histograms are evenly binned according
to the option :code:`elossHistoBinWidth` (in metres).
