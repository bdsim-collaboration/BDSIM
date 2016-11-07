.. _dev-tracking:

Tracking Algorithms
*******************

Background
==========

Geant4 provides the ability track a variet of particles through space in the 3D
geometry model. Importantly, it also provides the ability to track the motion of
particles in electro-magnetic fields.  As Geant4 provides a large library of
particle types, the tracking software must be capable of tracking particles
of different masses and charges.

This is achieved by factorising certain functionality into different sets of C++
classes.  The user for example must provide a function that will return the electic
and magnetic field vectors for a given set of :math:`(x,y,z,t)` coordinates.
As the user specifies the field, and it is essentially unknown, numerical integration
techniques must be used to solve the equation of motion to calculate the trajectory
of a given particle.  Geant4 provides a variety of different numerical integrators
that offer various capabilities and trade-offs in accuracy and computational speed.
The field in and numerical integrator classes are combined with a few other necessary
Geant4 classes to create a *complete* "field" capable of calculating the trajectory of
a particle that would represent the physical motion in the given field. After this,
the complete field may be attached to a *G4LogicalVolume* instance. A logical volume
has not just a shape, but also material, colour, field, sensitivity etc. Even though
only one logical volume object may be created it may *placed* multiple times in the
3D geometry model.

As described in :ref:`dev-fields`, BDSIM provides a variety of C++ classes that
represent typical accelerator (pure) magnetic fields. These can be attached to
the relevant vacuum volumes in the geometry along with Geant4 numerical integrators
to achieve particle tracking in an accelerator. However, for both a uniform dipole
field and quadrupolar field, there exist analytical solutions to the equations of
motion. These solutions provide a more accurate representation of the particle's
motion in the field and may offer significant computational advantage over numerical
integration techniques. Primarily for reasons of accuracy, these are provided in
BDSIM.

BDSIM provides a set of what appear to Geant4 as integrators that calculate the
particle's motion in a given field. In the Geant4 scheme a field class must also
be provided and Geant4 will query this, and supply the field vector to the integrator.
The BDSIM accelerator integrators are constructed with a strength parameter that is
used with a magnetic rigidity to calculate an effictive field or graident that can
be used in the analytical solution. They therefore, ignore the field as provided by
Geant4.

However, these analytical solutions are typically only valid for forward travelling
paraxial particle motion and therefore would not be able to calculate the particle
motion for backwards partilces or even particles travelling perpendicular to the
intended direction. In this case, the BDSIM integrators fall back to a Geant4
provided integrator that uses the previuosly ignored field value. This combination
provides the benefit of accurate and fast tracking in vacuum, but the robust treatment
of likely secondary particles at a variety of energies, masses and charges in any
direction.

Integrator Sets
===============

When using BDSIM, the user can select a *set* of integrators (ie tracking routines)
with the following syntax::

  option, integratorSet="geant4";

The integrator set may be one of the following (case-insensitive):

+------------+-------------------------+--------------------------------+
| **Set**    | **Magnetic Field Type** | **Integrator**                 |
+============+=========================+================================+
| bdsim      | Solenoid                | BDSIM Solenoid                 |
|            +-------------------------+--------------------------------+
|            | Dipole                  | BDSIM Dipole                   |
|            +-------------------------+--------------------------------+
|            | Quadrupole              | BDSIM Quadrupole               |
|            +-------------------------+--------------------------------+
|            | Sextupole               | BDSIM Sextuple                 |
|            +-------------------------+--------------------------------+
|            | Octupole                | BDSIM Octupole                 |
|            +-------------------------+--------------------------------+
|            | Decapole                | BDSIM Decapole                 |
|            +-------------------------+--------------------------------+
|            | Thick Multipole         | G4ClassicalRK4                 |
|            +-------------------------+--------------------------------+
|            | Muon Spoiler            | G4ClassicalRK4                 |
|            +-------------------------+--------------------------------+
|            | RF Cavity (EM)          | G4ClassicalRK4                 |
|            +-------------------------+--------------------------------+
|            | RF (E only)             | G4ClassicalRK4                 |
|            +-------------------------+--------------------------------+
|            | General Default         | G4ClassicalRK4                 |
|            +-------------------------+--------------------------------+
|            | Skew Quadrupole         | G4ClassicalRK4                 |
|            +-------------------------+--------------------------------+
|            | Skew Sextupole          | G4ClassicalRK4                 |
|            +-------------------------+--------------------------------+
|            | Skew Octupole           | G4ClassicalRK4                 |
|            +-------------------------+--------------------------------+
|            | Skew Decapole           | G4ClassicalRK4                 |
|            +-------------------------+--------------------------------+
|            | Dipole Fringe           | BDSIM Dipole Fringe            |
|            +-------------------------+--------------------------------+
|            | Thin Multipole          | BDSIM Thin Multipole           |
+------------+-------------------------+--------------------------------+
| geant4     | Solenoid                | G4ClassicalRK4                 |
|            +-------------------------+--------------------------------+
|            | Dipole                  | G4ClassicalRK4                 |
|            +-------------------------+--------------------------------+
|            | Quadrupole              | G4ClassicalRK4                 |
|            +-------------------------+--------------------------------+
|            | Sextupole               | G4ClassicalRK4                 |
|            +-------------------------+--------------------------------+
|            | Octupole                | G4ClassicalRK4                 |
|            +-------------------------+--------------------------------+
|            | Decapole                | G4ClassicalRK4                 |
|            +-------------------------+--------------------------------+
|            | Thick Multipole         | G4ClassicalRK4                 |
|            +-------------------------+--------------------------------+
|            | Muon Spoiler            | G4ClassicalRK4                 |
|            +-------------------------+--------------------------------+
|            | RF Cavity (EM)          | G4ClassicalRK4                 |
|            +-------------------------+--------------------------------+
|            | RF (E only)             | G4ClassicalRK4                 |
|            +-------------------------+--------------------------------+
|            | General Default         | G4ClassicalRK4                 |
|            +-------------------------+--------------------------------+
|            | Skew Quadrupole         | G4ClassicalRK4                 |
|            +-------------------------+--------------------------------+
|            | Skew Sextupole          | G4ClassicalRK4                 |
|            +-------------------------+--------------------------------+
|            | Skew Octupole           | G4ClassicalRK4                 |
|            +-------------------------+--------------------------------+
|            | Skew Decapole           | G4ClassicalRK4                 |
|            +-------------------------+--------------------------------+
|            | Dipole Fringe           | BDSIM Dipole Fringe            |
|            +-------------------------+--------------------------------+
|            | Thin Multipole          | BDSIM Thin Multipole           |
+------------+-------------------------+--------------------------------+
 
.. Note:: Both dipole fringe and thin multipole fields are *thin* elements
	  and have no *thick* equivalent and therefore have no field that
	  Geant4 can use. Therefore, they only use the BDSIM integrators.

Integrator Algorithms
=====================

BDSIM Dipole
------------

BDSIM Quadrupole
----------------

BDSIM Sextupole
---------------

BDSIM Octupole
--------------

BDSIM Decapole
--------------

BDSIM Dipole Fringe
-------------------

BDSIM Thin Multipole
--------------------


Combined Dipole-Quadrupole
--------------------------

RMatrix - from Particle Accelerator Physics (3rd Edition) by Wiedemann, chapter 5.
The z terms are not calculated via the matrix method, rather the z position
is simply the addition of the step length, and the  z momentum is calculated
from the x and y momentum to ensure momentum conservation.
Note that this matrix is incomplete, there are terms for the calculation of the
l parameter which are not needed in this stepper.

.. math::

   \begin{pmatrix}
   x_1    \\
   x'_1   \\
   y_1    \\
   y'_1   \\
   l_1    \\
   \delta \\
   \end{pmatrix} =

    \begin{pmatrix}
    \cos{\Theta}            & \frac{\sin{\Theta}}{\sqrt{K}} & 0                     & 0                              & 0 & \frac{1 - \cos{\Theta}}{\sqrt{K}}  \\
    -\sqrt{K}\sin{\Theta}   & \cos{\Theta}                  & 0                     & 0                              & 0 & \sin{\Theta}                       \\
    0                       & 0                             & \cosh{\Theta}         & \frac{\sinh{\Theta}}{\sqrt{K}} & 0 & 0                                  \\
    0                       & 0                             & \sqrt{K}\sinh{\Theta} & \cosh{\Theta}                  & 0 & 0                                  \\
    0                       & 0                             & 0                     & 0                              & 1 & 0                                  \\
    0                       & 0                             & 0                     & 0                              & 0 & 1                                  \\
   \end{pmatrix}
   \begin{pmatrix}
   x_0    \\
   x'_0   \\
   y_0    \\
   y'_0   \\
   l_0    \\
   \delta \\
   \end{pmatrix}


Validation of BDSIM Integrators
===============================

* Comparison with PTC
* Comparison with RK4


BDSIM Integrator Response to Non-Paraxial Particles
===================================================

* Use Geant4 RK4
* Treat as drift for very low energy
