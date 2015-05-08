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

* single particle Monte-Carlo simulations of particle accelerators
* simulating beam loss in a particle accelerator
* simulating detector backgrounds from halo and machine background sources

What BDSIM is not intended for
------------------------------

* Long term tracking studies
* Simulating collective effects
* Lattice optical design and optimisation
* A replacment for SixTrack

Example Applications
--------------------

* LHC beam loss simulation
* CLIC muon backgrounds studies
* Laserwire signal to background ratio
* ILC collimator efficieny study and detector backgrounds
  

General Simulation Steps
========================

1) Create an text input **.gmad** lattice for BDSIM by converting a **MADX** or **MAD8** twiss file.
2) Run BDSIM with core beam distribution for validation.
3) Analyse data from 2) to reproduce optical functions of lattice as validation of a correct model
4) Run BDSIM with desired input distribution either as a single instance or on a farm.
5) Analyse output data as desired.

A Little More Detail
====================

BDSIM uses **ASCII** text input with a syntax designed to be very similar to
**MAD8** / **MADX**.  The user prepares a representation of the
accelerator lattice they wish
to simulate by defining magnets or various accelerator components and the sequence
they should appear in.  Additionally, the user may set options describing for
example, energy tracking cuts, which physics processes of importance and at which
locations to record output.

BDSIM can then use the input file to simulate the passage of the desired number of
particles and how they interact with the accelerator components themselves.
Should a particle hit the beampipe, the physics processes of Geant4 will be used
to claculate the interaction with the beampipe and and secondary particles that may
be produced.  Particles are recorded at user specified 'sampling' planes and energy
deposition through the accelerator is recorded in both discrete 'hits' and a
histogram as a function of distance along the accelerator.

One may generally write their own C++ program to simulate the setup of geometry
and magnetic fields they like, however, in the case of an accelerator the
typical geometry is highly repetitive and usually consists of either a beampipe
or a beampipe with a magnet surrounding it. BDSIM generate Geant4 geometry
automatically and uses custom 'steppers' for specific magnetic fields in place
of normal Runge-Kutta integrators used by Geant4. Equations of motion descirbing
particle motion in magnetic fields such as that of a quadruople or sector-bend have 
analytical solutions that can be used in place of numerical integration. BDSIM
uses these to provide fast and accurate thick lens tracking in vacuum.

Although BDSIM uses fairly generic geometry, the user may specify the style of
geometry to be used, which will cover most cases. Should a more detailed geometry
be required, the user may supply this in various formats as well as magnetic field
maps.
