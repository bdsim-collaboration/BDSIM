V0.8 - 2015 / ?? / ??
^^^^^^^^^^^^^^^^^^^^^

* C++11 adopted
* Remove dependency on boost (issue #57)
* Return error if superfluous arguments are present (issue #56)
* Make parser more robust against duplicate element names (issue #43)

V0.7 - 2015 / 06 / 30
^^^^^^^^^^^^^^^^^^^^^

* Ability to write no output
* New magnet geometry factories introduced with 7 possible magnet types.
* Fixes to overlapping volumes and tracking errors in beam pipes.
* Introduction of --vis_debug flag to see container volumes without debug build.
* Revised magnet colours (same base colour, just prettier variant).
* Fix for wrong transverse coordinates for geometry other than cylindrical magnets (issue #30).
* New manual using sphinx documentation system.
* Support for all Geant4 visualisers introduced (issue #11).
* Default visualiser provided - no requirement for a vis.mac by the user.
* Nicer visualisation GUI by default.
* Improved visualisation for GDML geometry.
* Histograms now written to disk in case of crash or kill signal (issue #38).
* Fix for uncontrolled memory consumption for synchrotron radiation (issue #36).
* Fix syntax error in parser on windows end of line character (issue #40).
* Follow user paths properly (issue #24).
* Parser can end on commented line (issue #41).
* Introduction of more flexible and weigthed halo bunch distribution.
* Significant tidy of BDSAcceleratorComponent base class and derived classes.
* Fix LHC magnet geometry overlaps and improve efficiency as well as more flexible with
  different beam pipes.
* Updated python utilities.
* New BDSBeamline class used for component placement consistently in code.

V0.65 - 2015 / 04 / 10
^^^^^^^^^^^^^^^^^^^^^^

* New base class for any geometrical object BDSGeometryComponent.
* New interchangeable beam pipes with 6 possible beam pipe shapes.
* New sensitive detector manager to hold single instance of sd classes.
* Introduction of G4Galatic material for 'empty' volumes rather than beam pipe vacuum.
* Possibility to write to multiple output formats at once.
* Extensive removal of unnecessary headers throughout.
* Updated python utilities.
* Fix for muon spoiler magnetic field (thanks to B. Pilicer).
* Fix for invisible cylinder of iron surrounding drifts previously.


V0.64 - 2015 / 02 / 16
^^^^^^^^^^^^^^^^^^^^^^

* New histogram manager and factorisation of histograms from outputs.
* Extra per element histograms.
* Basic implementation of valid solenoid.

V0.63 - 2015 / 02 / 06
^^^^^^^^^^^^^^^^^^^^^^

* Large angle sbends split into multiple sbends - based on aperture error tolerance - currently 1mm.
* New geometry construction and placement for sbends and rbends - no overlapping volumes and
  simpler / increased performance.
* Proper building under c++11 if available.
* Introduction of composite bunch distribution.

V0.62 - 2014 / 08 / 07
^^^^^^^^^^^^^^^^^^^^^^

V0.61 - 2014 / 08 / 05
^^^^^^^^^^^^^^^^^^^^^^
