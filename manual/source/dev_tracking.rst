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
competitive computationally. The routine provided is a 2nd order Euler integration
algorithm.  More will be added in future.


Integrator Sets
===============

When using BDSIM, the user can select a *set* of integrators (ie tracking routines)
with the following syntax::

  option, integratorSet="geant4";

This choice affects the computation time and accuracy of the simulation but each set
may be suited to different scenarios.  As more integration algorithms are added to BDSIM,
more sets can be added that mix and match routines as required.

.. note:: "bdsim" is the default.

.. note:: Prior to v0.96, the set described by "bdsimold" was the default, although
	  in v0.95 it was labelled "bdsimone".

The specific details are described in _`Integrator Algorithms`.

The integrator set may be one of the following (case-insensitive):

+------------+-------------------------+--------------------------------+
| **Set**    | **Magnetic Field Type** | **Integrator**                 |
+============+=========================+================================+
| bdsimtwo   | Solenoid                | BDSIM Dipole2                  |
|            +-------------------------+--------------------------------+
|            | Dipole                  | BDSIM Dipole2                  |
|            +-------------------------+--------------------------------+
|            | Quadrupole              | BDSIM Quadrupole               |
|            +-------------------------+--------------------------------+
|            | Sextupole               | BDSIM Euler                    |
|            +-------------------------+--------------------------------+
|            | Octupole                | BDSIM Euler                    |
|            +-------------------------+--------------------------------+
|            | Decapole                | BDSIM Euler                    |
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
| geant4dp*  | Solenoid                | G4DormandPrince745             |
|            +-------------------------+--------------------------------+
|            | Dipole                  | G4DormandPrince745             |
|            +-------------------------+--------------------------------+
|            | Quadrupole              | G4DormandPrince745             |
|            +-------------------------+--------------------------------+
|            | Sextupole               | G4DormandPrince745             |
|            +-------------------------+--------------------------------+
|            | Octupole                | G4DormandPrince745             |
|            +-------------------------+--------------------------------+
|            | Decapole                | G4DormandPrince745             |
|            +-------------------------+--------------------------------+
|            | Thick Multipole         | G4DormandPrince745             |
|            +-------------------------+--------------------------------+
|            | Muon Spoiler            | G4DormandPrince745             |
|            +-------------------------+--------------------------------+
|            | RF Cavity (EM)          | G4DormandPrince745             |
|            +-------------------------+--------------------------------+
|            | RF (E only)             | G4DormandPrince745             |
|            +-------------------------+--------------------------------+
|            | General Default         | G4DormandPrince745             |
|            +-------------------------+--------------------------------+
|            | Skew Quadrupole         | G4DormandPrince745             |
|            +-------------------------+--------------------------------+
|            | Skew Sextupole          | G4DormandPrince745             |
|            +-------------------------+--------------------------------+
|            | Skew Octupole           | G4DormandPrince745             |
|            +-------------------------+--------------------------------+
|            | Skew Decapole           | G4DormandPrince745             |
|            +-------------------------+--------------------------------+
|            | Dipole Fringe           | BDSIM Dipole Fringe            |
|            +-------------------------+--------------------------------+
|            | Thin Multipole          | BDSIM Thin Multipole           |
+------------+-------------------------+--------------------------------+

.. Note:: `*` "geant4dp" is only available when BDSIM is compiled against
	  Geant 4.10.3 or higher.
 
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
provides the partial differential of the motion at a given location - i.e. the field
is found at that location and the vector potential calculated.

An integrator derived from :code:`G4MagIntegratorStepper` must implement a method:

.. code-block:: c++

		virtual void Stepper( const G4double y[],
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

   \mathbf{A} = ~ \frac{charge \cdot c}{ \|\mathbf{p}\| } (\mathbf{p} \times \mathbf{B})
   

.. math::

   \mathrm{dydx}[0] &= ~ \frac{p_x}{\|\mathbf{p}\|}\\
   \mathrm{dydx}[1] &= ~ \frac{p_y}{\|\mathbf{p}\|}\\
   \mathrm{dydx}[2] &= ~ \frac{p_z}{\|\mathbf{p}\|}\\
   \mathrm{dydx}[3] &= ~ \mathbf{A}[0]\\
   \mathrm{dydx}[4] &= ~ \mathbf{A}[1]\\
   \mathrm{dydx}[5] &= ~ \mathbf{A}[2]


There are other factors in the code for units that aren't shown here.

.. note:: **Field calls**: Geant4 will sample the field to give to the equation of
	  motion to calculate
	  :math:`\mathbf{A}`. Getting the field value is generally conidered an *expensive*
	  operation as may often involve geometry lookup for transforms, applying transforms
	  or indexing a large array along with interpolation.  In the case of BDSIM, the
	  majority of fields requie a geometry lookup and transform but are simple equations.

.. note:: **Time**: Geant4 magnetic integrators do not integrate time and
	  therefore copy the initial
	  value of time to the output coordinates.  BDSIM integrators follow this behaviour.
	  The time is handled by Geant4 at a higher level as the magnetic integrators are
	  specified to be only integrating over 6 variables.


Coordinate Convetion
--------------------

* Units are not explicitly mentioned here. In code there are factors to convert to Geant4 units.
* :math:`\mathbf{q}` is used to represent a 3-vector for spatial coordinates (:math:`x,y,z`).
* :math:`\mathbf{p}` is used to represent a 3-vector for the momentum (:math:`p_x, p_y, p_z`).
* The subscript ":math:`_{in}`" is used to denote input coordinates.
* The subscript ":math:`_{out}`" is used to denote what will be output coordinates after the step.
* :math:`h` is used to describe the spatial step length requested. This would be along the curved
  trajectory through a field the particle would take.

BDSIM Drift
-----------

This algorithm tranports a particle through free space with no external force acting on it.
This is provided here although provided generally by Geant4 as it is required by other
BDSIM integrators under various circumstances. It exists in the
:code:`BDSIntegratorBase::AdvanceDrift`
base class for the majority of BDSIM integrators.

.. math::

   \mathbf{q}_{out} ~ &= ~ \mathbf{q}_{in} + h~\mathbf{\hat{p}_{in}} \\
   \mathbf{p}_{out} ~ &= ~ \mathbf{p}_{in}

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

   \rho~=~ \frac{\|\mathbf{p}_{in}\|} {\mathbf{B} \cdot charge}

* Convert coorindates from global to local (curvilinear) frame of reference.
* Calculate local change of coordinates.

.. math::

   \theta           ~ &= ~ \frac{h}{\rho} \\
   \mathbf{\hat{f}} ~ &= ~ \mathbf{\hat{p}} \times \hat{\mathbf{y}} \\
   \mathrm{CT}      ~ &= ~ \cos^2(\theta/2) - sin^2(\theta/2) \\
   \mathrm{ST}      ~ &= ~ 2~\cos(\theta/2)\,\sin(\theta/2)

.. math::

   \mathbf{q}_{out} ~ &= ~ \mathbf{q}_{in} + \rho \left[ \, \mathrm{ST}\,\mathbf{\hat{p}_{in}} +
   (1- \mathrm{CT})\, \mathbf{\hat{f}} \,  \right]\\
   \mathbf{p}_{out} ~ &= ~ \mathbf{\hat{p}_{in}}\,\mathrm{CT} + \mathbf{\hat{f}}\,\mathrm{ST}

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

This routine makes use of the tracking routine provided in Geant4 for a pure magnetic field.
This is provided in the :code:`G4MagHelicalStepper` class, which provides the tracking routine
for a single step through a pure magnetic field, but not the other functionality required
for a suitable integrator. This BDSIM class that inherits it provides the rest of the require
functionality as well as special treatment for particles that may spiral indefinitely.

* The field :math:`\mathbf{B}` is queried at :math:`\mathbf{q}_{in}`.
* A full step along the trajectory is calculated.
* If the radius of curvature is less than the minimum radius of curvature (5 cm by default),
  use the explicit spiralling algorithm.

Otherwise:

* Calculate the motion through two half steps (includes sampling the field at the half step
  point).
* Calculate the error on the output coordinates as the difference between two half steps and
  one full step.

The spiralling algorithm artificially advances the helix of the particle along the field
direction more quickly than it would naturally by step length :math:`h`, even if it had
no momentum component along the field direction. This ensures that a particle that spirals
in a strong magnetic field without ever hitting a boundary will terminate in timely manner
and not dominate tracking time. The minimum radius of curvature is chosen to be approximately
the radius of the typical aperture throughout the model (specified in the options). As the
magnetic field does no work, a spiralling particle could spiral for a very long time and cause
an event to run almost indefinitely. Given most dipoles in accelerators induce only a few
miliradians of deflection, such a particle must be of a much lower momentum than the
design momentum of the dipole and would in reality not progress far from the magnet.

This artifical behaviour terminates particles in the approximate location by moving them
more quickly to a boundary.

The routine provided by Geant4 in G4MagHelicalStepper is as follows:

.. math::

   \mathbf{p}_{\|} ~ &= ~ \|\mathbf{B}\| ~ (\mathbf{\hat{B}} \cdot  \mathbf{\hat{p}}_{in}) \\
   \mathbf{p}_{\perp} ~ &= ~ \mathbf{\hat{p}}_{in} - \mathbf{p}_{\|}\\

.. math::
   
   R ~ &= ~ \frac{-\|B\| ~ charge} {\mathbf{p}_{in}}\\
   \theta ~ &= ~ \frac{h}{R}

* If :math:`\|\theta\| < 0.005`:

.. math::

   \mathrm{ST} ~ &= ~ \sin\theta\\
   \mathrm{CT} ~ &= ~ \cos\theta
   
* Else:

.. math::
   
   \mathrm{ST} ~ &= ~ \theta - \frac{1}{6}~\theta^{3}\\
   \mathrm{CT} ~ &= ~ 1 - \frac{1}{2}~\theta^{2} + \frac{1}{24}~\theta^{4}

The final coordinates are calculated as:
   
.. math::

   \mathbf{q}_{out} ~ = ~ \mathbf{q}_{in} + R ~ \Big[ \mathrm{ST}~\mathbf{p}_{\perp} + (1-\mathrm{CT})~(\mathbf{\hat{B}} \times \mathbf{\hat{p}}_{in}) \Big] + h~\mathbf{p}_{\|}

.. math::
   
   \mathbf{p}_{out} ~ = ~ \mathbf{\hat{p}}_{in} ~ \Big[ \mathrm{CT}~\mathbf{p}_{\perp} + \mathrm{ST}\,(\mathbf{\hat{B}} \times \mathbf{\hat{p}}_{in}) \Big] + \mathbf{p}_{\|}

The distance from the chord and arc of the true path are also calculated by Geant4 and
the algorithm is as follows.

* If the angle of the curve is in the range :math:`0 \leq \theta \leq ~ \pi`:

.. math::

   \Delta_{chord} ~ = ~ R ~\Bigg[1-\cos\Big(\frac{\theta}{2}\Big) \Bigg]

* Else if :math:`\pi < \theta < 2\pi`:

.. math::

   \Delta_{chord} ~ = ~ R ~\Bigg[1+\cos\Big(\frac{2\pi-\theta}{2}\Big) \Bigg]
   
* Else:

.. math::

    \Delta_{chord} ~ = 2~R


BDSIM Quadrupole
----------------

* Class name: :code:`BDSIntegratorQuadrupole`

The field gradient is calculated upon construction of the integrator as:

.. math::

   B' ~ = ~ \frac{\mathrm{d}B_{y}}{\mathrm{d}x} ~ = ~ B\rho~ \Big( \frac{1}{B\rho}~\frac{\mathrm{d}B_{y}}{\mathrm{d}x} \Big)~ = ~ B\rho~k_{1}

For each usage:

* Calculate strength parameter :math:`\kappa` *w.r.t.* a given particle rigidity:

.. math::

   \kappa ~=~ \frac{charge \cdot c}{\|\mathbf{p}_{in}\|} ~ \frac{\mathrm{d}B_{y}}{\mathrm{d}x}

If :math:`\|\kappa\| < 10^{-2}` use the drift integrator, else continue as:

* Convert to local curvilinear coordinates.

If :math:`\hat{p}_{z,local} < 0.9`, the particle is considered non-paraxial and the backup
integrator from :code:`BDSIntegratorMag` is used.  Else, proceed with thick matrix
transportation.  In this case, the following factors are calculated:

.. math::

   rk  = ~\sqrt{\|\kappa\|~p_{z}} \\
   rkh = h~p_{z}~rk

For :math:`\kappa > 0`, the focussing thick matrix is used (in the local curvilinear frame):

.. math::
   \mathbf{M_{quad, +\kappa}}~=~
   \begin{pmatrix}
   \cos(rkh)                         & \frac{1}{rk}\sin(rkh)  & 0 & 0    \\
   -\|\kappa\|~\frac{1}{rk}\sin(rkh) & \cos(rkh)              & 0 & 0    \\
   0 & 0 & \cosh(rkh)                          &  \frac{1}{rk}\sinh(rkh) \\
   0 & 0 & -\|\kappa\|~\frac{1}{rk}\sinh(rkh)  & \cosh(rkh)              \\
   \end{pmatrix}

and for :math:`\kappa < 0`, the defocussing thick matrix is used (again, in the local
curvilinear frame):

.. math::
   \mathbf{M_{quad, -\kappa}}~=~
   \begin{pmatrix}
   \cosh(rkh)                          &  \frac{1}{rk}\sinh(rkh) & 0 & 0 \\
   -\|\kappa\|~\frac{1}{rk}\sinh(rkh)  & \cosh(rkh)              & 0 & 0 \\
   0 & 0 & \cos(rkh)                         & \frac{1}{rk}\sin(rkh)     \\
   0 & 0 & -\|\kappa\|~\frac{1}{rk}\sin(rkh) & \cos(rkh)                 \\
   \end{pmatrix}

These are used as follows (again in the local curvilinear frame):

.. math::

   \begin{pmatrix}
   q_{x,out} \\
   p_{x,out} \\
   q_{y,out} \\
   p_{y,out} \\
   \end{pmatrix} ~ = ~
   \mathbf{M_{quad,\pm}} ~
   \begin{pmatrix}
   q_{x,in} \\
   p_{x,in} \\
   q_{y,in} \\
   p_{y,in} \\
   \end{pmatrix}

:math:`p_{z,out}` is calculated by conserving momentum.

.. math::

   p_{z,out} ~ = ~ \sqrt{1 - p_{x,out}^2 - p_{y,out}^2}

:math:`q_{z,out}` is calculated as:

.. math::

   q_{z,out} ~ = ~ \sqrt{\Big[ h^2\,(1 - \frac{h^2}{12\,R^2}) - (\mathrm{d}q_{x}^2 + \mathrm{d}q_{y}^2) \Big]} 

where :math:`\mathrm{d}q_{x,y}` are the changes in local :math:`x` and :math:`y` respectively.
:math:`R` is:

.. math::

   R ~ = ~ \frac{1}{\|R''\|}

.. math::

   R'' ~ = ~
   \begin{pmatrix}
   -p_{z,in}~q_{x,in} \\
   p_{z,in}~q_{y,in}  \\
   q_{x,in}~p_{x,in} - q_{y,in}~p_{y,in}\\
   \end{pmatrix}

The distance from the chord and arc of the true path are estimated as:

.. math::

   \Delta_{chord} ~ = ~ \frac{h^2}{8\,R}


BDSIM Euler
-----------

* Class name: :code:`BDSIntegratorEuler`

* Calculate the half way position along step length :math:`h` if the particle were to drift:

.. math::

   \mathbf{q}_{half} ~ = ~ \mathbf{q}_{in} + \mathbf{\hat{p}_{in}} ~ \frac{h}{2}

* Calculate the vector potential :math:`\mathbf{A}` *w.r.t.* :math:`\mathbf{q}_{half}`
  but with :math:`\mathbf{p}_{in}` (the original momentum - so as if the particle truly
  drifted to that point). Uses the equation of motion method :code:`RightHandSide`.
  This invokes 1 query of the field.
* Calculate the new coordinates:

.. math::

   \mathbf{q}_{out} ~ &= ~ \mathbf{q}_{in} + \mathbf{\hat{p}_{in}} ~ h + \mathbf{A}~\frac{h^{2}}{2~\|\mathbf{p}_{in}\|} \\
   \mathbf{p}_{out} ~ &= ~ \mathbf{p}_{in} + \mathbf{A}~h
  

BDSIM Sextupole
---------------

* Class name: :code:`BDSIntegratorSextupole`

This integrator is constructed with :math:`k_2` (originally calculated *w.r.t.* the nominal
beam rigidity higher up in BDSIM). It uses this to give a notion of a sextupolar field
whilst calcualting the magnetic vector potential in the local curvilinear coordinate frame.
The input coordinates must therefore be converted to local curvilinear ones.

In comparison to the _`BDSIM Euler` integrator, this has one extra transform for the coordinates
but one fewer for the field and so has roughly the same performance. The algorithm is as follows:

* If :math:`\|k_{2}\| < 10^{-12}`, track as a drift.

* Convert coordinates from global to local curvilinear frame.
* A point half way along the step length :math:`h` is calculated using a drift algorithm (":math:`_{mid}`").
* This position is used to calculate the vector potential as:

  
.. math::

   \mathbf{A} = \frac{k_2}{2!} ~
   \begin{pmatrix}
   \hat{p}_{z,in}~(q_{x,mid}^2 - q_{y,mid}^2 ) \\
   -2~\hat{p}_{z,in}~q_{x,mid}~q_{y,mid}       \\
   \hat{p}_{x,in}~(q_{x,mid}^2 - q_{y,mid}^2 ) - 2~\hat{p}_{y,in}~q_{x,mid}~q_{y,mid}
   \end{pmatrix}

.. note:: This can viewed as the cross product between the unit momentum vector and the
	  sextupolar field, whilst assuming that the :math:`B_z` component is always zero
	  and so some terms of the cross product can be ommitted.

* The output coordinates are calculated with the communal :ref:`communal-euler` algorithm.

BDSIM Octupole
--------------

* Class name: :code:`BDSIntegratorOctupole`

This integrator is constructed with :math:`k_3` (originally calculated *w.r.t.* the nominal
beam rigidity higher up in BDSIM).

* If :math:`\|k_{3}\| < 10^{-20}`, track as a drift.

* Convert coordinates from global to local curvilinear frame.
* A point half way along the step length :math:`h` is calculated using a drift algorithm (":math:`_{mid}`").
* This position is used to calculate the vector potential as:

  
.. math::

   \mathbf{A} = \frac{k_3}{3!} ~
   \begin{pmatrix}
   -\hat{p}_{z,in}~(q_{x,mid}^3 - 3~q_{y,mid}^2~q_{x,mid} ) \\
   -\hat{p}_{z,in}~(q_{x,mid}^3 - 3~q_{x,mid}^2~q_{y,mid} ) \\
   \hat{p}_{x,in}~(q_{x,mid}^3 - 3~q_{y,mid}^2~q_{x,mid} ) - \hat{p}_{y,in}~(q_{x,mid}^3 - 3~q_{x,mid}^2~q_{y,mid} )
   \end{pmatrix}

.. note:: This can viewed as the cross product between the unit momentum vector and the
	  octupolar field, whilst assuming that the :math:`B_z` component is always zero
	  and so some terms of the cross product can be ommitted.

* The output coordinates are calculated with the communal :ref:`communal-euler` algorithm.


BDSIM Decapole
--------------

* Class name: :code:`BDSIntegratorDecapole`

This integrator is constructed with :math:`k_4` (originally calculated *w.r.t.* the nominal
beam rigidity higher up in BDSIM).

* If :math:`\|k_{4}\| < 10^{-20}`, track as a drift.

* Convert coordinates from global to local curvilinear frame.
* A point half way along the step length :math:`h` is calculated using a drift algorithm (":math:`_{mid}`").
* This position is used to calculate the vector potential as:

  
.. math::

   \mathbf{A} = \frac{k_4}{4!} ~
   \begin{pmatrix}
   \hat{p}_{z,in}~(q_{x,mid}^4 - 6~q_{x,mid}^2~q_{y,mid}^2 + q_{y,mid}^4 )     \\
   -\hat{p}_{z,in}~\big[4~q_{x,mid}~q_{y,mid}~(q_{x,mid}^2-q_{y,mid}^2) \big] \\
   \hat{p}_{x,in}~\big[q_{x,mid}^4 - 6~q_{x,mid}^2~q_{y,mid}^2 + q_{y,mid}^4 \big] -
   \hat{p}_{y,in}~\big[4~q_{x,mid}~q_{y,mid}~(q_{x,mid}^2-q_{y,mid}^2) \big]
   \end{pmatrix}

.. note:: This can viewed as the cross product between the unit momentum vector and the
	  decapolar field, whilst assuming that the :math:`B_z` component is always zero
	  and so some terms of the cross product can be ommitted.

* The output coordinates are calculated with the communal :ref:`communal-euler` algorithm.

  
.. _communal-euler:
  
BDSIM Old Euler Common
----------------------

* Class name: :code:`BDSIntegratorMag`

The euler integration part of the original BDSIM integrators for higher order fields
exists in one place in :code:`BDSIntegratorMag::AdvanceChord()`. This takes the step
length :math:`h`, the local position, momentum and vector potential.  The algorithm
is as follows:

* If :math:`\|\mathbf{A}\| = 0`, advance as a drift.
* Else, proceed as:

.. math::

   q_{x,out} ~ = ~ q_{x,in} + p_{x,in}~h + \frac{A_{x}~h^2}{2}\\
   q_{y,out} ~ = ~ q_{y,in} + p_{y,in}~h + \frac{A_{y}~h^2}{2}\\

The output z coordinate is calculated as:
   
.. math::

   q_{z,out} ~ = ~ q_{z,in} + \sqrt{\Bigg[ h^2 ~ \big(1 - \frac{h^2~\|\mathbf{A}\|^2}{12} \big) - \delta_{x}^2 - \delta_{y}^2 \Bigg]} \\

The momentum is calcualted as:

.. math::

   \mathbf{p}_{out} = \mathbf{p}_{in} + h~\mathbf{A} \\

The delta chord intersection is calculated as:

.. math::
   
   \Delta_{chord} ~ = ~ \frac{h^2 \|\mathbf{A}\|}{8}

The error is not calculated here.


BDSIM Dipole Fringe
-------------------

* Class name: :code:`BDSIntegratorDipoleFringe`

This integrator provides a change in momentum only that represents the edge effect of a dipole
with a pole face rotation. This class inherits :code:`BDSIntegratorDipole2` as it uses it
for the dipole component of the motion. After that, the small change in momentum is applied.

* If the step length is longer than 1 mm, the kick is not applied (i.e. not a thin dipole edge element).

* The input coordinates are converted to the local curvilinear frame. This is required only for
  this algorithm and not that in :code:`BDSIntegratorDipole2`.

* If :math:`\hat{p}_{z,local} < 0.9`, the particle is considered non-paraxial and no change in momentum
  is applied.
  
The momentum change in the local curvilinear frame is calculated as:

.. math::

   dp_{y} ~ = ~ \frac{q_{y,in}}{\rho}~\tan(\theta - corr.)

Where :math:`\theta` is the angle of the pole face and ":math:`corr.`" is the fringe
field correction term, which is calculated in
:code:`BDSBendBuilder::CalculateFringeFieldCorrection()`:

.. math::

   corr. ~ = ~ \frac{f_{int}}{\rho}~ \frac{(1 + \sin^2\theta)}{\cos\theta}

Where  :math:`f_{int}` is an input parameter but described by:

.. math::

   f_{int} ~ = ~ \int_{-\infty}^{\infty} \frac{B_y(s)~\big(B_0 - B_y(s)\big)}{2~h_{gap}~B_0^2} \mathrm{d}s

Here, :math:`h_{gap}` is also an input parameter that specifies the half distance between the dipole
poles.

* The correction term .

The change in momentum:

.. math::

   d\mathbf{p} ~ = ~
   \begin{pmatrix}
   0        \\
   - dp_{y} \\
   0
   \end{pmatrix}

* This :math:`\mathrm{d}p` vector is converted to the global frame.

The output momentum (from the dipole integrator) is adjusted as:

.. math::

   \mathbf{p}_{out} ~ = ~ \mathbf{p}_{in} + d\mathbf{p}
  
   

BDSIM Thin Multipole
--------------------

* Class name: :code:`BDSIntegratorMultipoleThin`

This integrator applies a thin multipole kick to forward going paraxial particles. This is
normally attached to a box or disc that is very thin (Geant4 requires finite dimensions)
but sufficiently small that only one step is taken through it. Typically, a length of 1 pm
is used along :math:`S`. It is not possible to control how many steps a particle takes
through a given volume in Geant4 tracking as many physics processes can propose different
step lengths. However, by choosing such a short length of volume and by filling it with
vacuum, no other process will force a step in the middle of the volume. If more than one
step were taken, the integrator would be used multiple times resulting in stronger
kicks than are correct.

* Convert coordinates from global to local curvilinear frame.

If :math:`\hat{p}_{z,local} < 0.9`, the particle is considered non-paraxial and the backup
integrator from :code:`BDSIntegratorMag` is used. Else proceed with thin kick.

The output position remains the same.

.. math::

   \mathbf{q}_{out} ~ = ~ \mathbf{q}_{in}

The momentum is modified as:
   

.. math::
   
   qc = q_{x,in} + i~q_{y,in}

.. math::
   
   dp_{n} ~ = ~ \sum_{j=1}^{12} ~ \frac{k_{n,j}}{j!}~qc^j \\
   dp_{s} ~ = ~ \sum_{j=1}^{12} ~ \frac{k_{s,j}}{j!}~qc^j

Where :math:`qc` is the complex number formed from the horizontal and vertical positions
in the local curvilinear frame and the subscripts ":math:`_{n}`" and ":math:`_{s}`" represent
normal and skew multipole components respectively.  The output momentum is therefore:

.. math::

   p_{x,out} ~ = ~ p_{x,in} - \mathrm{Re}(dp_{n}) - \mathrm{Im}(dp_{s}) \\
   p_{y,out} ~ = ~ p_{y,in} + \mathrm{Im}(dp_{n}) + \mathrm{Im}(dp_{s}) \\
   p_{z,out} ~ = ~ \sqrt{\big[1 - p_{x,out}^2 - p_{x,out}^2 \big] }


.. math::

   \mathbf{p}_{out} ~ = ~
   \begin{pmatrix}
   p_{x,out} \\
   p_{y,out} \\
   p_{z,out}
   \end{pmatrix}

   

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
