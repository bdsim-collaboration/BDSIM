************
Introduction
************


Purpose of BDSIM
================

Beam Delivery Simulation (BDSIM) is a C++ program that utilises the Geant4
toolkit to simulate both the transport of particles in an accelerator and
their interaction with the accelerator material. BDSIM is capable of
simulating a wide variety of accelerator components and magnets with Geant4
geometry dynamically built based on a text input file.

What BDSIM is suitable for
--------------------------

* Single particle Monte-Carlo simulations of particle accelerators
* Simulating beam loss in a particle accelerator
* Simulating detector backgrounds from halo and machine background sources

What BDSIM is not intended for
------------------------------

* Long term tracking studies
* Simulating collective effects
* Lattice optical design and optimisation
* A replacement for tracking codes like SixTrack or PTC

Example Applications
--------------------

* LHC beam loss simulation
* CLIC muon backgrounds studies
* Laserwire signal to background ratio
* ILC collimator efficiency study and detector backgrounds
  

General Simulation Steps
========================

1) Create an text input **.gmad** lattice for BDSIM by converting a **MADX** or **MAD8** twiss file.
2) Run BDSIM with core beam distribution for validation.
3) Analyse data from 2) to reproduce optical functions of lattice as validation of a correct model
4) Run BDSIM with desired input distribution and physics processes either as a single instance or on a farm.
5) Analyse output data as desired.

A Little More Detail
====================

BDSIM uses **ASCII** text input with a syntax designed to be very similar to
**MAD8** / **MADX**. The user prepares a representation of the
accelerator lattice they wish
to simulate by defining magnets or various accelerator components and the sequence
they should appear in. Additionally, the user may set options describing, for
example, energy tracking cuts, which physics processes are of importance and at which
locations to record output.

BDSIM can then use the input file to simulate the passage of the desired number of
particles and how they interact with the accelerator components themselves.
Should a particle hit the beampipe, the physics processes of Geant4 will be used
to calculate the interaction with the beampipe and and secondary particles that may
be produced. Particles are recorded at user specified 'sampling' planes and energy
deposition through the accelerator is recorded in both discrete 'hits' and a
histogram as a function of distance along the accelerator.

One may generally write their own C++ program to simulate the setup of geometry
and magnetic fields they like, however, in the case of an accelerator the
typical geometry is highly repetitive and usually consists of either a beampipe
or a beampipe with a magnet surrounding it. BDSIM generates Geant4 geometry
automatically and provides integrators for linear magnetic fields in place
of the normal Runge-Kutta integrators used by Geant4. Equations of motion describing
particle motion in magnetic fields such as that of a quadrupole or sector-bend have 
analytical solutions that can be used in place of numerical integration. BDSIM
uses these to provide fast and accurate thick lens tracking in vacuum.

BDSIM provides a library of fairly generic magnet gemoetry styles that will cover
most cases. Should a more detailed geometry be required, the user may supply
this in various formats as well as magnetic field maps. Alternatively, if the user
wishes to write their own Geant4 geometry for a particular component this can be
integrated into BDSIM relatively easily.
