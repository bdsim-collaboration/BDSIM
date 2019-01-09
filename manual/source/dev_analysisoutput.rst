.. _dev-analysisoutput:

Sensitivity, Output & Analysis
******************************

Information From Geant4
=======================

Geant4 allows the developer to gather information at every stage of the simulation, but is
by default a *quiet* simulation with no output generated.

BDSIM collects information broadly in two ways. Firstly, through sensitive detector ("SD")
classes that are attached to individual volumes and make a 'hit', which is a snapshot of
some desired information.  Secondly, through trajectory information. Trajectories are created
for the passage of a single particle throughout the whole geometry, whereas SDs only generate
information for particles taking steps in the volumes they are attached to.

Sensitive Detectors
-------------------

The role of a sensitive detector is to generate output information from steps taken in
a volume. It is registered to a 'logical volume' and generates hits when
particles pass through only the logical volume(s) it's attached to.

These 'hits' are then stored in a 'collection' (i.e. vector) that can be processed
at the end of an event.

BDSIM provides several SD classes to prepare different types of information. The
two main sensitive detector classes in BDSIM are `BDSEnergyCounterSD` and
`BDSSamplerSD`. These are responsible for generating energy deposition hits in
accelerator components and for recording information in sampler volumes respectively.
The hits are translated and put into output structures in the end of event action
in BDSEventAction.

There is usually only one instance of the sensitive detector class even though it
is attached to many logical volumes. These are held in the singleton class
BDSSDManager.

Attachment of Sensitive Detectors
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

All beam line objects in BDSIM generally attach the sensitive detector they want
at construction time. This is done through the BDSGeometryComponent base class
that keeps a map of G4LogicalVolume* to BDSSDType, a class enum(eration) for which
sensitive detector class to use.

During the placement of the beam lines in BDSDetectorConstruction, the SD classes
are retrieved from BDSSDManager and attached to the G4LogicalVolumes. At this stage
the user options are applied and if the hits are not required because some output
options are turned off, the SDs will not be attached saving CPU time and memory
during the simulation.

* Only SDs that are requried are attached based on the options.


Trajectory Creation \& Storage
------------------------------

Trajectory objects are created for each new track in the Geant4 user tracking action
hook (in `BDSTrackingAction`). If turned on for all particles, this would record
every piece of available information for the full simulation - an unmanageable amount
of data in large studies. Therefore, this is by default only turned on for primary
particles. The trajectory information is used by the Geant4 visualisers to view the
tracks, so it must be turned on in this case too.

If the user turns on trajectory storage, trajectories are created for every particle
in the event and then filtered to store only the ones of interest in the output
in the end of event action in `BDSEventAction`.
