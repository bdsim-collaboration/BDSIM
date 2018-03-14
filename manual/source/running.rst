=============
Running BDSIM
=============

Run Time Requirements
=====================

If BDSIM is compiled with GDML support on and GDML is required, the user is required
to be connected to the internet to access the GDML schema. This is a requriement
of the Geant4 GDML parser. In future, this need will be eliminated.

Basic Operation
===============

The minimum required information to run BDSIM is an input GMAD file.::

  bdsim --file=lattice.gmad

The following section describes other 'executable' options that may specify
various options such as whether to run in 'batch' mode or interactively with a
visualiser; and where to write output.

.. _executable-options:

Executable Options
==================

BDSIM can be executed in a terminal with extra arguments to specify various inputs.
The angular brackets here are to indicate a user specified value and should not
be used literally.  The following command may be used to dipslay all options::

  bdsim --help

.. tabularcolumns:: |p{6cm}|p{9cm}|

+----------------------------+----------------------------------------------+
| --file=<file>              | specify the input gmad file                  |
+----------------------------+----------------------------------------------+
| --batch                    | batch mode - no graphics                     |
+----------------------------+----------------------------------------------+
| --circular                 | assume circular machine - turn control       |
+----------------------------+----------------------------------------------+
| --distrFile=<file>         | which file to use for the bunch distribution |
+----------------------------+----------------------------------------------+
| --exportGeometryTo=<file>  | export the geometry to a file                |
|                            | extension determines format                  |
|                            | where possible extensions are ("gdml")       |
+----------------------------+----------------------------------------------+
| --generatePrimariesOnly    | generate primary particle coordinates only   |
|                            | then exit without simulating anything        |
+----------------------------+----------------------------------------------+
| --help                     | display this message.                        |
+----------------------------+----------------------------------------------+
| --materials                | list materials included in BDSIM by default  |
+----------------------------+----------------------------------------------+
| --ngenerate=N              | the number of primary events to simulate     |
|                            | overrides the ngenerate option in input file |
+----------------------------+----------------------------------------------+
| --output=<fmt>             | output format "rootevent" (default) or "none"|
+----------------------------+----------------------------------------------+
| --outfile=<file>           | output file name. Will be appended with _N   |
|                            | where N = 0, 1, 2, 3...                      |
+----------------------------+----------------------------------------------+
| --recreate=<file>          | the rootevent output file to recreate events |
|                            | from                                         |
+----------------------------+----------------------------------------------+
| --seed=<N>                 | seed for the random number generator         |
+----------------------------+----------------------------------------------+
| --seedStateFileName=<file> | file containing CLHEP::Random seed state     |
|                            | NB \- this overrides other seed value        |
+----------------------------+----------------------------------------------+
| --startFromEvent=N         | event offset to start from when recreating   |
|                            | events when using --recreate                 |
+----------------------------+----------------------------------------------+
| --survey=<file>            | print survey info to <file>                  |
+----------------------------+----------------------------------------------+
| --printFractionEvents=N    | how often to print out events as a fraction  |
|                            | of the total number of events to simulate    |
|                            | (default 0.1 i.e. 10%). -1 for all.          |
+----------------------------+----------------------------------------------+
| --printFractionTurns=N     | how often to print out turns as a fraction   |
|                            | of the total number of turns to simulate     |
|                            | (default 0.2 i.e. 20%). -1 for all. Will     |
|                            | only print out on an event that will print   |
|                            | out as well.                                 |
+----------------------------+----------------------------------------------+
| --verbose                  | display general parameters before run        |
+----------------------------+----------------------------------------------+
| --verbose\_event           | display information for every event          |
+----------------------------+----------------------------------------------+
| --verbose\_step            | display tracking information after each step |
+----------------------------+----------------------------------------------+
| --verbose\_event\_num=<N>  | display tracking information for event N     |
+----------------------------+----------------------------------------------+
| --vis_debug                | display all volumes in visualiser            |
+----------------------------+----------------------------------------------+
| --vis_mac=<file>           | file with the visualisation macro script,    |
|                            | default provided by BDSIM: openGL (OGLSQt))  |
+----------------------------+----------------------------------------------+
| --writeSeedState           | write an ASCII file seed state for each      |
|                            | event                                        |
+----------------------------+----------------------------------------------+

BDSIM can be run in one of two ways, `interactively`_, or `in batch mode`_, which
are described in the following sections.

When run interactively, a Geant4 visualiser is invoked that produces a window with an image
of the BDSIM model as well as a terminal prompt to control it. No events are simulated
without user input. Alternatively, BDSIM can be run in batch mode, where no visualiser
is used and the specified number of primary events is simulated and feedback printed
to the terminal. Batch mode is typically much faster than the interactive mode, but
the interactive mode is very useful for understanding the model and a typical event
in the simulation - ie where a particle hits.

Interactively
=============

Features:

* default option
* interactive visualisation of accelerator model
* ability to view and rotate accelerator model
* ability to run individual events
* typically slower than batch mode
* no events run without user input

To execute BDSIM in interactive mode, the user must simply not use the :code:`--batch` command.
The user can also specify a macro file using the :code:`--vis_mac` option above otherwise, BDSIM
will look for "vis.mac" in the current working directory. If not found, BDSIM will use its own
default visualisation settings (typically: Qt visualiser, extra convenient buttons). Once
executed a window such as this will appear (depending on the visualiser you use).

.. figure:: figures/visualisation/qtvisualiser.png
   :width: 90%
   :align: center
   :figclass: align-center

   BDSIM running interactively with OpenGL Qt visualiser from Geant4.

Note, the visualiser is part of Geant4 so if the desired visualiser isn't available, you
must recompile Geant4 with the correct visualiser (and subsequently BDSIM afterwards). Geant4
also uses the CMake configuration system. The visualiser shown is the OpenGL Qt visualiser, which
we recommend for its ease of use and high level of interactivity.

More details can be found in :ref:`Visualisation`.

.. note:: BDSIM simulates one particle at a time from the primary distribution and all of the
	  associated secondaries. Each event is independent and different particles in the input
	  bunch cannot interact with each other or their secondaries. This is an underlying feature
	  of Geant4.

In Batch Mode
=============

Features:

* no interactive visualiser
* faster
* number of events run then program quits
* no user input
* typical use for a "job" on a farm

To execute BDSIM in batch mode, simply use the :code:`--batch` execution option.

Examples
========
::

   bdsim --file=atf2.gmad --outfile=run1 --batch --seed=123

This executes BDSIM for the ATF2 example with ROOT output to a file name "run1" in batch
mode with a seed value of 123. The simulation runs the number of events specified by the
:code:`ngenerate` options parameter in the input gmad file. ::

      bdsim --file=sm.gmad --outfile=run2

This executes BDSIM for the simpleMachine example with ROOT output to a file named
"run2". The program is run interactively and the window in Figure appears. From here, the
user types::
  
  /run/beamOn 1
  exit

In the visualiser terminal to run one event and visualise it.


