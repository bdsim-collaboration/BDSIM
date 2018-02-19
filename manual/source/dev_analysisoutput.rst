.. _dev-analysisoutput:

Sensitivity, Output & Analysis
******************************

Information From Geant4
=======================

Geant4 allows the developer to gather information at every stage of the simulation, but is
by default a *quiet* simulation with no output generated.

BDSIM collects information through sensitive detector ("SD") classes and through
trajectory information. There is the opportunity to collect information per step or
per track creation, but this is not used as the combination of the SDs and the trajectories
provide sufficient information.

Sensitive Detectors
-------------------

The role of a sensitive detector is generate output information from steps taken in
a volume. It is registered to a 'logical volume' and that SD generates hits when
particles pass through only the logical volume(s) it's attached to.

These 'hits' are then stored in a 'collection' (i.e. vector) that can be processed
at the end of an event.

The two main sensitive detector classes in BDSIM are `BDSEnergyCounterSD` and
`BDSSamplerSD`. These are responsible for generating energy deposition hits in
accelerator components and for recording information in sampler volumes respectively.
Each prepare 'hits collections' that are processed at the end of event action in
`BDSEventAction`. Here the hits are translated into the output structures.

Trajectory Creation \& Storage
------------------------------

Trajectory objects are created for each new track in the Geant4 user tracking action
hook (in `BDSTrackingAction`). If turned on for all particles this would record
every piece of available information for the full simulation - an unmanageable amount
of data in large studies. Therefore, this is by default only turned on for primary
particles. The trajectory information is used by the Geant4 visualisers to view the
tracks so it must be turned on in this case too.

If the user turns on trajectory storage, trajectories are created for every particle
in the event and then filtered to store only the ones of interest in the output
in the end of event action in `BDSEventAction`.
