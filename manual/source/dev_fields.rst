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

The quadrupole field is constructed with strength parameter :math:`k_1` and with respect to
a nominal rigidity :math:`B\rho`. Although the rigidity is included in :math:`k_1`, it is
required to calculate the field gradient internally.

.. math::

   k_1 = \frac{1}{B\rho} \frac{\partial B_y}{\partial x}

The field is described by

.. math::
   \begin{eqnarray}
   B_x & = & \frac{\partial B_y}{\partial x} y \\
   B_y & = & \frac{\partial B_y}{\partial x} x \\
   B_z & = & 0
   \end{eqnarray}
   
   
.. figure:: dev_figures/quadrupole_radial.pdf
	    :width: 70%
	    :align: center

	    Example field map of a quadrupole with :math:`k_1 = 0.12`, and :math:`B\rho = 0.3456`.

Sextupole
---------

The sextupole field is constructed with strength parameter :math:`k_2` and with respect
to a nominal rigidity :math:`B\rho`.

.. math::

   k_2 = \frac{1}{B\rho} \frac{\partial^2 B_y}{\partial x^2}

The field is described by

.. math::
   \begin{eqnarray}
   B_x & = & \frac{1}{2!} \frac{\partial^2 B_y}{\partial x^2} \,2xy \\
   B_y & = & \frac{1}{2!} \frac{\partial^2 B_y}{\partial x^2} \, (x^2 - y^2) \\
   B_z & = & 0 \\
   \end{eqnarray}
   

.. figure:: dev_figures/sextupole_radial.pdf
	    :width: 70%
	    :align: center

	    Example field map of a sextupole with :math:`k_2 = 0.02`, and :math:`B\rho = 0.3456`.


Octupole
--------

The octupole field is constructed with strength parameter :math:`k_3` and with respect to
a nominal rigidity :math:`B\rho`.

.. math::

   k_3 = \frac{1}{B\rho} \frac{\partial^3 B_y}{\partial x^3}

The field is described by

.. math::
   \begin{eqnarray}
   B_x & = & \frac{1}{3!} \frac{\partial^3 B_y}{\partial x^3} \,(3x^2 y - y^3) \\
   B_y & = & \frac{1}{3!} \frac{\partial^3 B_y}{\partial x^3} \, (x^3 - 3xy^2) \\
   B_z & = & 0 \\
   \end{eqnarray}


.. figure:: dev_figures/octupole_radial.pdf
	    :width: 70%
	    :align: center

	    Example field map of a octupole with :math:`k_3 = 0.003`, and :math:`B\rho = 0.3456`.


Decapole
--------

The decapole field is constructed with strength parameter :math:`k_4` and with respect to
a nominal rigidity :math:`B\rho`.

.. math::

   k_4 = \frac{1}{B\rho} \frac{\partial^4 B_y}{\partial x^4}

The field is described by

.. math::
   \begin{eqnarray}
   B_x & = & \frac{1}{4!} \frac{\partial^4 B_y}{\partial x^4} \, 4xy(x^2 - y^2) \\
   B_y & = & \frac{1}{4!} \frac{\partial^4 B_y}{\partial x^4} \, (x^4 - 6x^2y^2 + y^4) \\ 
   B_z & = & 0 \\
   \end{eqnarray}

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
   x' \\
   y' \\
   z' \\
   \end{bmatrix}
   =
   R(\theta)
   \begin{bmatrix}
   x \\
   y \\
   z \\
   \end{bmatrix}
   =
   \begin{bmatrix}
   \cos \theta & - \sin \theta & 0\\
   \sin \theta & \cos \theta   & 0\\
   0 & 0 & 0 \\
   \end{bmatrix}
   \begin{bmatrix}
   x \\
   y \\
   z \\
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

A general multipole field is also provided. The field is calculated in cylindrical coordinates, then converted
to carteasian. The field is calculated using an array of strength parameters :math:`k_1,k_2,\dotsc k_{12}` and
the skewed strength parmeters :math:`ks_1,ks_2,\dotsc ks_{12}` with respect to a nominal rigidity :math:`B\rho`.

.. note:: Currently the dipole component is not implemented. :math:`k_1` is the quadrupole strength,
	  :math:`k_2` is the sextupole strength, :math:`etc.`.

.. math::

   \begin{eqnarray}
   r                          & = & \sqrt{x^2 + y^2} \\
   B_r      (\mathrm{normal}) & = & \frac{1}{B\rho} \displaystyle\sum_{i=1}^{12} \frac{k_i}{i!} \,r^i \sin(i \phi) \\
   B_{\phi} (\mathrm{normal}) & = & \frac{1}{B\rho} \displaystyle\sum_{i=1}^{12} \frac{k_i}{i!} \, r^i \cos(i \phi) \\
   B_r      (\mathrm{skewed}) & = & \frac{1}{B\rho} \displaystyle\sum_{i=1}^{12} \frac{ks_i}{i!} \, r^i \cos(i \phi) \\
   B_{\phi} (\mathrm{skewed}) & = & \frac{1}{B\rho} \displaystyle\sum_{i=1}^{12} -\frac{ks_i}{i!} \, r^i \sin(i \phi) \\
   \end{eqnarray}
   
   \begin{eqnarray}
   B_x & = & B_r \cos \phi - B_{\phi} \sin \phi \\
   B_y & = & B_r \sin \phi + B_{\phi} \cos \phi \\
   \end{eqnarray}

      
.. figure:: dev_figures/multipole_radial.pdf
	    :width: 70%
	    :align: center

	    Example field map of a multipole with :math:`\{k_1, k_2, k_3, k_4, k_5\} = \{0.12,0.02,-0.003,0.0004,-0.00005\}`,
	    and :math:`B\rho = 0.3456`.


Muon Spoiler
------------

A muon spoiler field is provided that gives a constant toroidal field. It is constructed with field strength
:math:`B~(\mathrm{T})`. The field is calculated
according to

.. math::

   \begin{eqnarray}
   r & = & \sqrt{x^2 + y^2} \\
   B_x & = & \frac{y}{r} B \\
   B_y & = & \frac{-x}{r} B \\
   B_z & = & 0
   \end{eqnarray}

.. figure:: dev_figures/muon_spoiler_radial2.pdf
	    :width: 70%
	    :align: center

	    Example field map of a muon spoiler with field :math:`B = 1.3~(\mathrm{T})`. Note, the
	    variation shown in the graph is only numerical differences. The field is constant and this
	    is purely due to the plotting vector field algorithm.
