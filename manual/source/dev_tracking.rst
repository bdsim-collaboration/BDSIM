.. _dev-tracking:

Tracking Algorithms
*******************

Background On How Geant4 Tracking Works
=======================================

Geant4 provides the ability track a variety of particles through space in the 3D
geometry model. Importantly, it provides the ability to track the motion of
particles in electro-magnetic fields.  As Geant4 provides a large library of
particle types, the tracking software must be capable of tracking particles
of different masses and charges.

This is achieved by factorising certain functionality into different sets of C++
classes.  The user must provide a function that will return the electic
and magnetic field vectors for a given set of :math:`(x,y,z,t)` coordinates.
Specifically, this is a class that inherits :code:`G4Field` and provides an implementation
of the pure virtual method :code:`GetFieldValue(position)` where position is :math:`x,y,z,t`.

As the user specifies the field, and it is essentially unknown, numerical integration
techniques must be used to solve the equation of motion to calculate the trajectory
of a given particle.  Geant4 provides a variety of different numerical integrators
that offer various capabilities and trade-offs in accuracy and computational speed.
The field and numerical integrator classes are combined with a few other necessary
Geant4 classes to create a *complete* "field" capable of calculating the trajectory of
a particle that would represent the physical motion in the given field. After this,
the complete field may be attached to a *G4LogicalVolume* instance. A logical volume
has not just a shape, but also material, colour, field, sensitivity etc. Even though
only one logical volume object may be created it may *placed* multiple times in the
3D geometry model.

As described in :ref:`dev-fields`, BDSIM provides a variety of C++ classes that
represent typical accelerator (pure) magnetic fields. These can be attached to
the relevant vacuum volumes in the geometry along with Geant4 numerical integrators
to achieve particle tracking in an accelerator.

Linear Fields
-------------

For both a uniform dipole field and quadrupolar field (linear fields), there exist
analytical solutions to the equations of
motion. These solutions provide a more accurate representation of the particle's
motion in the field and may offer significant computational advantage over numerical
integration techniques. Primarily for reasons of accuracy, these are provided in
BDSIM for the dipole and quadrupole.

Non-Linear Fields
-----------------

BDSIM provides an integrator for higher order fields that more accurately conserves
energy when calculating the particle trajectory ('symplecticity') as well as being
competitive computationally. The routine provided is a symplectic Euler integration
algorithm.  More will be added in future.


Integrator Sets
===============

When using BDSIM, the user can select a *set* of integrators (ie tracking routines)
with the following syntax::

  option, integratorSet="geant4";

This choice affects the computation time and accuracy of the simulation but each set
may be suited to different scenarios.  As more integration algorithms are added to BDSIM,
more sets can be added that mix and match routines as required.

The specific detauls are described in _`Integrator Algorithms`.

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
|            | Sextupole               | BDSIM Symplectic Euler         |
|            +-------------------------+--------------------------------+
|            | Octupole                | BDSIM Symplectic Euler         |
|            +-------------------------+--------------------------------+
|            | Decapole                | BDSIM Symplectic Euler         |
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
| bdsimold   | Solenoid                | BDSIM Solenoid                 |
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

BDSIM currently only provides integrators for magnetic fields, i.e. not electric
or electro-magnetic fields.  For these types of fields, Geant4 integrators are used.

Common Magnetic Field Interface From Geant4
-------------------------------------------

The magnetic field integrators provided by BDSIM inherit :code:`G4MagIntegratorStepper`.
This is constructed with respect to a :code:`G4EquationOfMotion` object, which is
a :code:`G4Mag_UsalEqRhs` instance for BDSIM integrators.  This *equation of motion*
provides the partial differential of the motion at a given location.

An integrator derived from :code:`G4MagIntegratorStepper` must implement a method:

.. code-block:: c++

		virtual  void  Stepper( const G4double y[],
                                        const G4double dydx[],
                                              G4double h,
                                              G4double yout[],
                                              G4double yerr[]  ) = 0;


This is reposnsible for calculating the coordinates of a trajectory given the input
point :code:`y[]` (which is [:math:`x,y,z,p_x,p_y,p_z,t`]) for a step length of :math:`h`.
The output coordinates are written to :code:`yout[]` (also [:math:`x,y,z,p_x,p_y,p_z,t`])
along with the associated absolute uncertainty for each parameter to :code:`yerr[]`.
The differentials at the initial location are given by :code:`dydx`.  These are calculated
in :code:`G4Mag_UsualEqRhs.cc` as follows:

.. math::

   \mathbf{A} = ~ \frac{charge \cdot c}{ |\mathbf{p}| } (\mathbf{p} \times \mathbf{B})
   

.. math::

   \mathrm{dydx}[0] &=& ~ \frac{p_x}{|\mathbf{p}|}\\
   \mathrm{dydx}[1] &=& ~ \frac{p_y}{|\mathbf{p}|}\\
   \mathrm{dydx}[2] &=& ~ \frac{p_z}{|\mathbf{p}|}\\
   \mathrm{dydx}[3] &=& ~ \mathbf{A}[0]\\
   \mathrm{dydx}[4] &=& ~ \mathbf{A}[1]\\
   \mathrm{dydx}[5] &=& ~ \mathbf{A}[2]


There are other factors in the code for units that aren't shown here.

.. note:: Geant4 will sample the field to give to the equation of motion to calculate
	  :math:`\mathbf{A}`. Getting the field value is generally conidered an *expensive*
	  operation as may often involve geometry lookup for transforms, applying transforms
	  or indexing a large array along with interpolation.  In the case of BDSIM, the
	  majority of fields requie a geometry lookup and transform but are simple equations.

.. note:: Geant4 magnetic integrators do not integrate time and therefore copy the initial
	  value of time to the output coordinates.  BDSIM integrators follow this behaviour.
	  The time is handled by Geant4 at a higher level as the magnetic integrators are
	  specified to be only integrating over 6 variables.

BDSIM Drift
------------------

This algorithm tranports a particle through free space with no external force acting on it.
This is provided here although provided generally by Geant4 as it is required by other
BDSIM integrators under various circumstances. It exists in the
:code:`BDSIntegratorBase::AdvanceDrift`
base class for the majority of BDSIM integrators. The new 3-vector coordinates ('out') are
*w.r.t.* the input coordinates ('in') for a step length of :math:`h` mm is given by:

.. math::

   \mathbf{x}_{out} ~ &=& ~ \mathbf{x}_{in} + h~\mathbf{|p|}\\
   \mathbf{p}_{out} ~ &=& ~ \mathbf{p}_{in}

.. note:: The drift element in BDSIM is not assigned a field or BDSIM provided tracking
	  algorithm. The tracking is handled by Geant4.

	  
BDSIM Dipole
------------

* Class name: :code:`BDSIntegratorDipole`

This integrator is constructed with it's own strength parameter and **ignores** the field
information provided by Geant4. The field value (already multiplied by :code:`CLHEP::telsa`) is
assumed to be entirely along local :math:`\hat{\mathbf{y}}`, i.e. the field vector is
:math:`\mathbf{B} = (0,B,0)`. The algorithm progresses as follows:

* If the field value is 0 or the particle is neutral, the coordinates are advanced as a drift.

Otherwise continue as follows:

* Calculate bending radius :math:`\rho` as:

.. math::

   \rho~=~ \frac{\mathbf{|p|}} {\mathbf{B}~charge}

* Convert coorindates from global to local (curvilinear) frame of reference.
* Calculate local change of coordinates.

.. math::

   \theta           ~ &=& ~ \frac{h}{\rho} \\
   \mathbf{\hat{f}} ~ &=& ~ \mathbf{\hat{p}} \times \hat{\mathbf{y}} \\
   \mathrm{CT}      ~ &=& ~ \cos^2(\theta/2) - sin^2(\theta/2) \\
   \mathrm{ST}      ~ &=& ~ 2~\cos(\theta/2)\,\sin(\theta/2)

.. math::

   \mathbf{x}_{out} ~ &=& ~ \mathbf{x}_{in} + \rho \left[ \, \mathrm{ST}\,\mathbf{\hat{p}} +
   (1- \mathrm{CT})\, \mathbf{\hat{f}} \,  \right]\\
   \mathbf{p}_{out} ~ &=& ~ \mathbf{\hat{p}}\,\mathrm{CT} + \mathbf{\hat{f}}\,\mathrm{ST}

* If :math:`\rho` is less than a minimum radius of curvature (5 cm by default) reduce the
  magnitude of the momentum by 2 % to induce artificial spiralling.
* Convert to global coordinates.

This was the original dipole algorithm included with BDSIM until v0.96, however this
is limited to dipole fields aligned with :math:`\hat{y}` only and often caused tracking
warnings with very low momenta particles in strong magnetic fields. A more flexible integrator
that works in 3D was written to improve upon this and is described in _`BDSIM Dipole2`.
   
BDSIM Dipole2
-------------

* Class name: :code:`BDSIntegratorDipole2`

BDSIM Quadrupole
----------------

* Class name: :code:`BDSIntegratorQuadrupole`

BDSIM Symplectic Euler
----------------------

* Class name: :code:`BDSIntegratorSymplecticEuler`

BDSIM Sextupole
---------------

* Class name: :code:`BDSIntegratorSextupole`

BDSIM Octupole
--------------

* Class name: :code:`BDSIntegratorOctupole`

BDSIM Decapole
--------------

* Class name: :code:`BDSIntegratorDecapole`

BDSIM Dipole Fringe
-------------------

* Class name: :code:`BDSIntegratorDipoleFringe`

BDSIM Thin Multipole
--------------------

* Class name: :code:`BDSIntegratorMultipoleThin`


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
