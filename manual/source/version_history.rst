V0.8.develop - 2015 / ?? / ??
=============================

New Features
------------

Bug fixes
---------

Geometry
^^^^^^^^

* Magnets now have tightly fitting invisible container volumes as opposed to
  large boxes before.
* Changed return type of magnet outer geometry factories to new BDSMagnetOuter
  class. This is because the container contruction is now delegated to the
  magnet outer factory for tight fitting container volumes.
* Extended examples and tests.
* Fixed bug where the read out coordinates would also be offset by the offset
  of the element.
* Reduced dupliation in magnet outer factories.
* Fixed overlaps in rbend geometry.

Physics
^^^^^^^

Parser
^^^^^^

* Occasional material parser segfault fixed (issue #25)

Tracking
^^^^^^^^

* Introducted a separate auxilliary G4Navigator to avoid accidentally moving
  the particle during tracking when querying global to local transforms.

  
General
^^^^^^^


V0.8 - 2015 / 08 / 10
=====================

New Features
------------

* Tunnel geometry and flexible tunnel factories for different styles
* Tunnel read out geometry introduced for coordinates along tunnel axis
* C++11 adopted (required)
* `stopSecondaries` option
* Remove dependency on boost (issue #57)
* Restructured examples directory - top level contains only full machines
  and subdirectories contain features.
* Example documentation in manual and in place beside each example with
  example screenshots
* Updated python utilities *pybdsim v0.1*, *pymadx v0.1*, *pymad8 v0.1* and *robdsim v0.2*
* Repeated components are not duplicated in memory - previously, they would
  be repeatedly constructed. Reduced memory footprint
* Component information comes from Physical Volumes instead of Logical Volumes
* Improved manual documentation
* Improved Doxygen documentation
* Rubbish collection for all objects rather than relying on only one run and
  Geant4 (partial) rubbish collection.
* String representation of enum types leading to more readable output
* Introduce ability to switch to new modular physics lists with flexible
  construction and addition of physics lists without hard-coded names for each
  combination - the user must turn this on explicitly

Bug fixes
---------

Geometry
^^^^^^^^
* Geometry overlaps (issues #55 and #58)
* Transform3d fix (issue #54)
* Fixed placement of objects outside x,z global plane - rotation bug, similarly
  for read out geometry placement
* Fix broken circular control - bug was introduced in v0.7 - (issue #51)
* Strict checking of read out geometry construction to avoid invalid solids that
  would cause Geant4 to exit and BDSIM to crash
* Strict checking on teleporter volume construction for circular machines that
  would cause Geant4 to exit and BDSIM to crash
* Fix for calculation of length of sector bend magnet that would cause it to be
  slightly short - introduced in v0.7
* Removed stored axes of rotation due to better implementation in BDSBeamline
  avoiding duplication of information
* Fixed issue of zero angle rbends causing a crash (issue #44)
* Event number print out is now dynamic and based on the number of events to be
  generated and is also controllable with `printModuloFraction` option
* Protect against bad user specified values of `lengthSafety` to avoid
  geometry overlaps
* Improved parser speed

Physics
^^^^^^^
* SR radiation fix in dipole (issue #53)
* Removed continuous synchrotron radiation as traps particles in low step size
  infinite loop
* Removal of poorly set deltaIntersection, chordStepMinimum and lengthSafety
  variables from examples - these should be left unset unless the user knows
  their purpose.

Output
^^^^^^
* Change all transverse output units to **metres** - manual updated accordingly
* Change `z` in ASCII output to **global Z** instead of local z.
* Recorded energy in output is now unweighted but energy recorded in convenience
  energy loss histogram is. Could have lead to double weighting previously
* Fix for global coordinates being written out as local coordinates in ROOT
  output
* Random number generator seed state not written out when no output is specified

Parser
^^^^^^
* Return error if superfluous arguments are present (issue #56)
* Make parser more robust against duplicate element names (issue #43)
* Fixed warnings about compiling c as c++ being deprecated behaviour
  
General
^^^^^^^
* Fix for wrong print out warning due to logic error (issue #51)
* Fix for boundary effects of energy deposition (issue #52)
* Fix large memory leak for events with large number of particles - was due to
  accumulation of BDSTrajectory objects


V0.702 2015 / 07 / 28 - Hotfix
==============================

* Fix for physics production range cuts were not obeyed in simulation
  
V0.701 2015 / 07 / 02 - Hotfix
==============================

* Fix for global X coordinate not written to output for energy deposition

V0.7 - 2015 / 06 / 30
=====================

New Features
------------

* Ability to write no output
* New magnet geometry factories introduced with 7 possible magnet types.
* Introduction of --vis_debug flag to see container volumes without debug build.
* Revised magnet colours (same base colour, just prettier variant).
* New manual using sphinx documentation system.
* Default visualiser provided - no requirement for a vis.mac by the user.
* Nicer visualisation GUI by default.
* Improved visualisation for GDML geometry.
* Support for all Geant4 visualisers introduced (issue #11).

Bug fixes
---------

* Fixes to overlapping volumes and tracking errors in beam pipes.
* Fix for wrong transverse coordinates for geometry other than cylindrical magnets (issue #30).
* Histograms now written to disk in case of crash or kill signal (issue #38).
* Fix for uncontrolled memory consumption for synchrotron radiation (issue #36).
* Fix syntax error in parser on windows end of line character (issue #40).
* Follow user paths properly (issue #24).
* Parser can end on commented line (issue #41).
* Introduction of more flexible and weighted halo bunch distribution.
* Significant tidy of BDSAcceleratorComponent base class and derived classes.
* Fix LHC magnet geometry overlaps and improve efficiency as well as more flexible with different beam pipes.
* New BDSBeamline class used for component placement consistently in code.

V0.65 - 2015 / 04 / 10
======================

* New base class for any geometrical object BDSGeometryComponent.
* New interchangeable beam pipes with 6 possible beam pipe shapes.
* New sensitive detector manager to hold single instance of sd classes.
* Introduction of G4Galactic material for 'empty' volumes rather than beam pipe vacuum.
* Possibility to write to multiple output formats at once.
* Extensive removal of unnecessary headers throughout.
* Updated python utilities.
* Fix for muon spoiler magnetic field (thanks to B. Pilicer).
* Fix for invisible cylinder of iron surrounding drifts previously.


V0.64 - 2015 / 02 / 16
======================

* New histogram manager and factorisation of histograms from outputs.
* Extra per element histograms.
* Basic implementation of valid solenoid.

V0.63 - 2015 / 02 / 06
======================

* Large angle sbends split into multiple sbends - based on aperture error tolerance - currently 1mm.
* New geometry construction and placement for sbends and rbends - no overlapping volumes and
  simpler / increased performance.
* Proper building under c++11 if available.
* Introduction of composite bunch distribution.
* Drop support for Geant4 versions 9.5 and older
  
V0.62 - 2014 / 08 / 07
======================

V0.61 - 2014 / 08 / 05
======================

* Geant4 version 10 support

v0.6 - 2013 / 12 / 02
=====================


v0.5 - 2008 / 11 / 08
=====================


v0.4 - 2008 / 02 / 26
=====================


v0.3 - 2007 / 01 / 26
=====================


v0.2 - 2006 / 05 / 18
=====================


v0.1 - 2006 / 02 / 22
=====================


beta - 2005 / 05 / 01
=====================
