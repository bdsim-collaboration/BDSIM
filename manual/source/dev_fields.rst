.. _dev-fields:

Fields
******

BDSIM provides the magnetic fields typical to an accelerator as well as the ability
to import field maps and overlay them on geometry. Practically, this accomplished
through several objects, but the main two the user or developer must select are the
field and the integrator.

Coordinate System
=================

The accelerator is modelled, following convention, in curvilinear coordinates
that follow the beam line. However, in practice, Geant4 requires that the fields
and coordinates be calculated in global carteasian coordinates. The simplest solution
in Geant4 is to get the transformation from the global coordinates to the local
coordinates of the volume being queried for the field and tracking.  However, if
the field is 'attached' to not just a simple single shape, but a nested set of volumes,
the local coordinates of that volume are not the same as the accelerator curvilinear
coordinates. To get round this, the read out world is used for transformations.

To query a point (it's not simple / available to query a volume directly) in the
geometry, one should use a G4Navigator instance.  There is the singleton main
G4Navigator available to the developer, but this must never be used. Querying a point
in the geometry with this navigator changes the state of the navigator and therefore
the perceived location in the geometry hierarchy of the particle in question from that
point on. To avoid this, an extra navigator is created and used. Whilst these are not
large objects in memory, a single static extra navigator (member of BDSAuxiliaryNavigator)
is used. This decreases the look up time of a point signifcantly by performing a relative
search from the last point queried.

Utility methods for conversion are provided in BDSAuxiliaryNavigator. The developer should
design their class to inherit this one if they wish to convert to curvilinear coordinates.

Pure magnetic fields are provided that don't inherit BDSAuxiliaryNavigator to avoid the
requirement of 'closed' Geant4 geometry and a read out world. This greatly simplifies things
if the developer wants to simply make use of (or test alone for that matter) a single field.

Important Points
----------------

* A field object that uses curvlinear coordinates should only be attached to one volume,
  as it can only have one transform. Therefore, if attached to multiple volumes, only
  one will provide correct results.
* The fields use BDSAuxiliary navigator which uses the read out geometry - it's not only
  for sensitivity - it's really for curvilinear coordinates.
* Using BDSAuxiliaryNavigator requires an accelerator to be built - ie, it requires a world
  volume, and read out world, contents in both, the geometry to be 'closed' by Geant4 and
  a valid run manager instantiated.


Pure Magnetic Fields From Equations
===================================

Described here are a list of typical magnetic fields that are described by equations rather
that an interpolated field map. These are used for the majority of the accelerator components.
Described here are the pure version without global to curvilinear transformations. These classes
are wrapped when used with general BDSAcceleratorComponent instances.

Dipole
------

The dipole field is constructed with a magnitude :math:`|B|` and a unit vector
:math:`\hat{\mathbf{b}}`. It is constant with position and the default unit vector
is :math:`(0,1,0)` - unit y.

.. math::

   \mathbf{B} = \hat{\mathbf{b}} \cdot |B|
   

.. figure:: dev_figures/sectorbend_radial.pdf
	    :width: 70%
	    :align: center

	    Example field map of a dipole with :math:`\mathbf{B} = 1.3~\mathrm{T}`, and :math:`B\rho = 0.3456`.


Quadrupole
----------

.. figure:: dev_figures/quadrupole_radial.pdf
	    :width: 70%
	    :align: center

	    Example field map of a quadrupole with :math:`k_1 = 0.12`, and :math:`B\rho = 0.3456`.

Sextupole
---------

.. figure:: dev_figures/sextupole_radial.pdf
	    :width: 70%
	    :align: center

	    Example field map of a sextupole with :math:`k_2 = 0.02`, and :math:`B\rho = 0.3456`.


Octupole
--------

.. figure:: dev_figures/octupole_radial.pdf
	    :width: 70%
	    :align: center

	    Example field map of a octupole with :math:`k_3 = 0.003`, and :math:`B\rho = 0.3456`.


Decapole
--------

.. figure:: dev_figures/decapole_radial.pdf
	    :width: 70%
	    :align: center

	    Example field map of a decapole with :math:`k_4 = 0.004`, and :math:`B\rho = 0.3456`.


Skewed Versions
---------------

All of the above magnets (dipole, quadrupole, sextupole, octupole & decapole) are also
available as their skew counterparts. With BDSIM it is trivial to create a skew component
by simply creating a normal component and applying the appropriate tilt to it. However,
should one want the field skewed but not the component - say the correct upright square
aperture - these fields can be used.

A wrapper class is provided that is instantiated with an angle (hard coded in BDSFieldFactory)
. When the field is queried, the coordinates being queried are rotated by the angle. The
returned field vector is then anti-rotated to give the correct skew field at the original
location.

.. math::

   \mathbf{B}_{skew}(x,y) = R(-\theta) \mathbf{B}(x',y')
   
   \begin{bmatrix}
   x'\\
   y'\\
   \end{bmatrix}
   =
   R(\theta)
   \begin{bmatrix}
   x \\
   y \\
   \end{bmatrix}
   =
   \begin{bmatrix}
   \cos \theta & - \sin \theta \\
   \sin \theta & \cos \theta \\
   \end{bmatrix}
   \begin{bmatrix}
   x \\
   y \\
   \end{bmatrix}

Example field maps are shown below.

Skew Quadrupole
---------------

.. figure:: dev_figures/skew_quadrupole_radial.pdf
	    :width: 70%
	    :align: center

	    Example field map of a skew quadrupole with :math:`k_1 = 0.12`, and :math:`B\rho = 0.3456`.

Skew Sextupole
--------------

.. figure:: dev_figures/skew_sextupole_radial.pdf
	    :width: 70%
	    :align: center

	    Example field map of a skew sextupole with :math:`k_2 = 0.02`, and :math:`B\rho = 0.3456`.


Skew Octupole
-------------

.. figure:: dev_figures/skew_octupole_radial.pdf
	    :width: 70%
	    :align: center

	    Example field map of a skew octupole with :math:`k_3 = 0.003`, and :math:`B\rho = 0.3456`.


Skew Decapole
-------------

.. figure:: dev_figures/skew_decapole_radial.pdf
	    :width: 70%
	    :align: center

	    Example field map of a skew decapole with :math:`k_4 = 0.004`, and :math:`B\rho = 0.3456`.




Multipole
---------
