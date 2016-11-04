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

.. math::
   
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
	  :math:`k_2` is the sextupole strength, *etc*.

.. math::

   \begin{eqnarray}
   r                          & = & \sqrt{x^2 + y^2} \\
   B_r      (\mathrm{normal}) & = & \frac{1}{B\rho} \displaystyle\sum_{i=1}^{12} \frac{k_i}{i!} \,r^i \sin(i \phi) \\
   B_{\phi} (\mathrm{normal}) & = & \frac{1}{B\rho} \displaystyle\sum_{i=1}^{12} \frac{k_i}{i!} \, r^i \cos(i \phi) \\
   B_r      (\mathrm{skewed}) & = & \frac{1}{B\rho} \displaystyle\sum_{i=1}^{12} \frac{ks_i}{i!} \, r^i \cos(i \phi) \\
   B_{\phi} (\mathrm{skewed}) & = & \frac{1}{B\rho} \displaystyle\sum_{i=1}^{12} -\frac{ks_i}{i!} \, r^i \sin(i \phi) \\
   \end{eqnarray}


.. math::
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


.. _field-map-formats:

Field Map File Formats
======================


BDSIM Field Format
------------------

The field should be in an ASCII text file with the extension :code:`.dat`. A compressed
file using *tar* and *gzip* may be also used. The tar should contain only one file that
is the field. In this case, the file should have :code:`.tar.gz` extension.  Below is an
example of the required format in each 1D, 2D, 3D and 4D case.

The pybdsim utility may be used to prepare fields in the correct format in Python if a
Python numpy array is provided.  If the user has a custom field format, it would be
advisable to write a script to load this data into a Python numpy array and use the
provided file writers in pybdsim.

Generally:

 * A series of keys define the dimensions of the grid.
 * The keys at the beginning do not have to be in any order.
 * Empty lines will be skipped.
 * A line starting with :code:`!` denotes the column name definition row.
 * There can only be 1 column name definition row.
 * The order in the file must be keys, column name definition row, data.
 * A line starting with :code:`#` will be ignored as a comment line.
 * The order of the data must loop in the highest dimensions first and then lower,
   so the order should be :math:`t`, then :math:`z`, then :math:`y`, then :math:`x`.
 * Python classes are provided to write numpy arrays to this format.
 * Any lines beyond the amount of data specified by the dimensions will be ignored.
 * One cannot put a comment after the data in the line.

There are python scripts in :code:`bdsim/examples/features/fields/4_bdsimformat` called
:code:`Generate1D.py` etc., that were used to create the example data sets there that
have sinusoidally oscillating data.
   
.. Note:: The units are :math:`cm` for spatial coodinates and :math:`s` for temporal.

BDSIM Field Format 1D
---------------------

+--------------------+-------------------------------------------------------------------+
| **Parameter**      | **Description**                                                   |
+--------------------+-------------------------------------------------------------------+
| xmin               | The lower spatial coordinate in x associated with the field map.  |
+--------------------+-------------------------------------------------------------------+
| xmax               | The upper spatial coordinate in x associated with the field map.  |
+--------------------+-------------------------------------------------------------------+
| nx                 | Number of elements in x - 1 counting.                             |
+--------------------+-------------------------------------------------------------------+

Example syntax is shown below and there is an example in
:code:`bdsim/examples/features/fields/4_bdsimformat/1dexample.tar.gz`. Only part of the
field is specified here: ::

   xmin> -30.0
   nx> 47
   xmax> 29.8
   ! X	            Bx	            By	            Bz
   -3.0000000E+01	 1.3970775E+00	 0.0000000E+00	 0.0000000E+00
   -2.8700000E+01	 2.5843272E+00	 0.0000000E+00	 0.0000000E+00
   -2.7400000E+01	 3.5978584E+00	 0.0000000E+00	 0.0000000E+00
   -2.6100000E+01	 4.3695413E+00	 0.0000000E+00	 0.0000000E+00
   -2.4800000E+01	 4.8475035E+00	 0.0000000E+00	 0.0000000E+00
   -2.3500000E+01	 4.9996163E+00	 0.0000000E+00	 0.0000000E+00
   -2.2200000E+01	 4.8156547E+00	 0.0000000E+00	 0.0000000E+00
   -2.0900000E+01	 4.3079845E+00	 0.0000000E+00	 0.0000000E+00

   
BDSIM Field Format 2D
---------------------

All of the 1D parameters, plus:
   
+--------------------+---------------------------------------------------------------------------+
| **Parameter**      | **Description**                                                           |
+--------------------+---------------------------------------------------------------------------+
| ymin               | The lower spatial coordinate in :math:`y` associated with the field map.  |
+--------------------+---------------------------------------------------------------------------+
| ymax               | The upper spatial coordinate in :math:`y` associated with the field map.  |
+--------------------+---------------------------------------------------------------------------+
| ny                 | Number of elements in y - 1 counting.                                     |
+--------------------+---------------------------------------------------------------------------+

Example syntax is shown below and there is an example in
:code:`bdsim/examples/features/fields/4_bdsimformat/2dexample.tar.gz`.


BDSIM Field Format 3D
---------------------

All of the 1D & 2D parameters, plus:
   
+--------------------+---------------------------------------------------------------------------+
| **Parameter**      | **Description**                                                           |
+--------------------+---------------------------------------------------------------------------+
| zmin               | The lower spatial coordinate in :math:`z` associated with the field map.  |
+--------------------+---------------------------------------------------------------------------+
| zmax               | The upper spatial coordinate in :math:`z` associated with the field map.  |
+--------------------+---------------------------------------------------------------------------+
| nz                 | Number of elements in z - 1 counting.                                     |
+--------------------+---------------------------------------------------------------------------+

Example syntax is shown below and there is an example in
:code:`bdsim/examples/features/fields/4_bdsimformat/3dexample.tar.gz`.

BDSIM Field Format 4D
---------------------

All of the 1D, 2D & 3D parameters, plus:
   
+--------------------+---------------------------------------------------------------------------+
| **Parameter**      | **Description**                                                           |
+--------------------+---------------------------------------------------------------------------+
| tmin               | The lower spatial coordinate in :math:`t` associated with the field map.  |
+--------------------+---------------------------------------------------------------------------+
| tmax               | The upper spatial coordinate in :math:`t` associated with the field map.  |
+--------------------+---------------------------------------------------------------------------+
| nt                 | Number of elements in t - 1 counting.                                     |
+--------------------+---------------------------------------------------------------------------+

Example syntax is shown below and there is an example in
:code:`bdsim/examples/features/fields/4_bdsimformat/tdexample.tar.gz`.
      

.. _field-map-file-preparation:

BDSIM Field Map File Preparation
================================

The Python BDSIM utility *pybdsim* may be used to prepare a BDSIM format field map file
from a Python numpy array.

The pybdsim field classes are fully documented here :ref:`pybdsim-field-module`.


.. _field-interpolators:

Field Map Interpolators
=======================

A variety of interpolators are provided with BDSIM.  An example data set in 1D was generated
with simple :math:`x,y,z` field vector components that are different amplitude and phased
sinusoids shown below.

.. figure:: dev_figures/field_raw.pdf
	    :width: 80%
	    :align: center

	    Example 1D field value components.

Nearest Neightbour
------------------

The nearest neighbour algorithm returns the field value of the closest defined point in
the map and returns that value. Therefore, the interpolated map contains only the values
of the original map. This only serves the purpose of being able to query the map at any
set of coordinates and provides a 'pixelated' appearance and sharp discontinuities
half way between points in the map.  This is intended only for completeness and debugging.

.. figure:: dev_figures/field_nearest.pdf
	    :width: 80%
	    :align: center

	    Example 1D field value components with nearest neighbour interpolation.

Linear
------

In this case, the interpolated value lies on a straight line between two given points.
The field value :math:`f` at point :math:`x_i` lying between :math:`x_a` and :math:`x_b`
is given by

.. math::

   \begin{eqnarray}
   xd     &=& \frac{(x_i - x_a)}{(x_b - x_a)}\\
   f(x_i) &=& f(x_a)\,(1-xd) + f(x_b)\,xd
   \end{eqnarray}

Here, :math:`xd` will lie in the range :math:`[0,1]`. This is of course a 1D equation and
version of linear interpolation. See _`Linear & Cubic Higher Dimension Interpolation` for
further details for 2,3 & 4D interpolation.
   
   
.. figure:: dev_figures/field_linear.pdf
	    :width: 80%
	    :align: center

	    Example 1D field value components with linear interpolation.

Cubic
-----

In this case, the surrounding four map entries of any given point are used in combination
to give a small section of a cubic polynomial.  For a given point :math:`x_i`, the closest
point which is on the lower valued side is here called :math:`m_1` (m for map), and the
closest point which is on the higher valued side is called :math:`m_2`. Points further
outside these (in a 1D case) are called :math:`m_0` and :math:`m_3` respectively. (On a
linear number scale from low to hight they would be :math:`m_0, m_1, m_2, m_3`.) The
field value :math:`f(x_i)` is given by

.. math::
   xd = \frac{(x_i - x_a)}{(x_b - x_a)}

.. math::
   f(x_i) = m_1 + \frac{1}{2}\,xd\,(m_2 - m_0 + xd\,(\,2m_0 - 5 m_1 + 4 m_2 - m_3 + xd\,(\,3\,(m_1 - m_2) + m_3 - m_0)))


Here, :math:`xd` will lie in the range :math:`[0,1]`.

This is of course a 1D equation and version of cubic interpolation.
See :ref:`higher-dim-interpolation` for further details for 2,3 & 4D interpolation.
One could of course, cache the gradient at each point, but here it is calculated dynamically.
This allows the 1D interpolation case to be used in different dimensions for different gradients
and is not prohibitively slow.

.. figure:: dev_figures/field_cubic.pdf
	    :width: 80%
	    :align: center

	    Example 1D field value components with cubic interpolation.



.. Note:: Although the :math:`x,y,z` components are shown individually, they are in fact part of
	  a 3-vector class that is used for interpolation.  Ie, the components are not interpolated
	  individually.


.. _higher-dim-interpolation:

Linear & Cubic Higher Dimension Interpolation
---------------------------------------------

To interpolate both in a cubic polynomial and linear at greater than 1 dimension, the
1D interplator can be used iteratively. In the case of 2D interpolation this would be called
*bilinear* and *bicubic*, and in the case of 3D, *trilinear* and *tricubic* interpolation.
Below is a diagram of a cube representing a point :math:`C` at an arbitrary point inside the
8 corners that represent the closest values of the regular field map. The diagram shows this
approximately in the centre of the cube, but it could lie anywhere inside the 8 points.

.. figure:: dev_figures/interpolation_cube.svg
	    :width: 50%
	    :align: center

	    Field map value coordinates for 3D interpolation. This diagram was created by
	    "Marmelad" from Wikipedia [#f1]_.

.. [#f1] `Marmelad Cubic Diagram Wikipedia <https://commons.wikimedia.org/wiki/File:3D_interpolation2.svg>`_. 


:math:`C_{00}` can be found by interpolating between :math:`C_{000}` and :math:`C_{100}`.
:math:`C_{10}, C_{01}, C_{11}` can be found in a similar manner with each of their edges.
:math:`C_0` and :math:`C_1` can be found by then interpolating between :math:`C_{00}` and
:math:`C_{10}` for example (in the case of :math:`C_0`).  :math:`C` can then be found by
interpolating between :math:`C_0` and :math:`C_1` giving the desired value.

One may interpolate the dimensions in any order and arrive at the same result. By doing
it in such a way, the 2D interpolator can use the 1D interpolator; the 3D interpolator
can use the 2D interpolator etc. By ensuring the 1D case is correct, there is a much
lower likelihood of implementation faults occuring for higher dimensional interpolators.

Implementation Specifics
------------------------

To implement this iterative algorithm, *C* arrays were used as subsets can be easily
passed arround due to their underlying pointer nature in *C*. A small section of
code from :code:`bdsim/src/BDSInterpolatorRoutines.cc` is shown below:

.. figure:: dev_figures/interpolation_code_snippet.png
	    :width: 90%
	    :align: center
