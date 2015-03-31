=============
Running BDSIM
=============

BDSIM can be executed in a terminal with extra arguments to specify various inputs.

+----------------------------+----------------------------------------------+
| --file=<file>              | specify the input gmad file                  |
+----------------------------+----------------------------------------------+
| --output=<fmt>             | output format "root" or "ascii" (default)    |
+----------------------------+----------------------------------------------+
| --outfile=<file>           | output file name. Will be appended with _N   |
|                            | where N = 0, 1, 2, 3...                      |
+----------------------------+----------------------------------------------+
| --vis\_mac=<file>          | visualization macro script, default vis.mac  |
+----------------------------+----------------------------------------------+
| --gflash=<N>               | whether or not to turn on gFlash fast shower |
|                            | parameterisation.                            |
+----------------------------+----------------------------------------------+
| --gflashemax=<N>           | maximum energy for gflash shower             |
|                            | parameterisation in GeV.                     |
+----------------------------+----------------------------------------------+
| --gflashemin=<N>           | minimum energy for gflash shower             |
|                            | parameterisation in GeV.                     |
+----------------------------+----------------------------------------------+
| --help                     | display this message.                        |
+----------------------------+----------------------------------------------+
| --verbose                  | display general parameters before run        |
+----------------------------+----------------------------------------------+
| --verbose\_event           | display information for every event          |
+----------------------------+----------------------------------------------+
| --verbose\_step            | display tracking information after each step |
+----------------------------+----------------------------------------------+
| --verbose\_event\_num=<N>  | display tracking information for event N     |
+----------------------------+----------------------------------------------+
| --batch                    | batch mode - no graphics                     |
+----------------------------+----------------------------------------------+
| --outline=<file>           | print geometry / optics info to <file>       |
+----------------------------+----------------------------------------------+
| --outline_type=<fmt>       | type of outline format where fmt is one of   |
|                            | "optics" or "survey"                         |
+----------------------------+----------------------------------------------+
| --materials                | list materials included in BDSIM by default  |
+----------------------------+----------------------------------------------+
| --circular                 | assume circular machine - turn control       |
+----------------------------+----------------------------------------------+
| --seed=<N>                 | seed for the random number generator         |
+----------------------------+----------------------------------------------+
| --seedstate=<file>         | file containing CLHEP::Random seed state     |
|                            | NB \- this overrides other seed value        |
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

* interactive visualisation of accelerator model
* ability to run individual events
* typically slower than batch mode

To execute BDSIM in 


In Batch Mode
=============
