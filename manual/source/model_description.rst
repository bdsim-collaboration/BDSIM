================================
Model Description - Input Syntax
================================

GMAD Syntax
-----------

GMAD is a language specifically for BDSIM but is made to be human readable
and very similar to MADX.

* arithmetic expressions can be defined
* binary operators +, -, \*, /, ^ are valid
* unary operators +, -, are valid
* boolean operators <, >, <=, >=, <>, == are valid
* every expression **must** end with a semi-colon;
* no name can begin with a number

The following functions are provided

* sqrt
* cos
* sin
* tan
* exp, e
* log
* acos
* asin
* abs

Examples
^^^^^^^^
::

   x = 1;
   y = 2.5-x;
   z = sin(x) + log(y) -8e5;


Coordinates & Units
-------------------

In Geant4, global euclidean coordinates are used for tracking purposes, however,
in describing a lattice with BDISM, curvlinear coordinates are used as is common with
accelerators (X,Y,S).

**GMAD uses SI units**

==============================  =========================
Name                            Units
==============================  =========================
length                          [m] (metres)
time                            [s] (seconds)
angle                           [rad] (radians) 
quadrupole coefficient          [m :math:`^{-2}` ]
multipole coefficient 2n poles  [m :math:`^{-n}` ]
electric voltage                [MV] (Megavolts)
electric field strength         [MV/m]
particle energy                 [GeV]
particle mass                   [GeV/c :math:`^2` ]
particle momentum               [GeV/c :math:`^2` ]
beam current                    [A] (Amperes)
particle charge                 [e] (elementary charges)
emittances                      [pi m mrad]
density                         [g/cm :math:`^{3}` ] 
temperature                     [K] (Kelvin)
pressure                        [atm] (atmosphere)
mass number                     [g/mol]
==============================  =========================

Some useful predefined values / units are:

==========  =================================
Name        Value
==========  =================================
pi          3.14159265358979
GeV         1
eV          :math:`10^{-9}`
KeV         :math:`10^{-6}`
MeV         :math:`10^{-3}`
TeV         :math:`10^{3}`
MV          1
Tesla       1
rad         1
mrad        :math:`10^{-3}`
clight      :math:`2.99792458 \times 10^{8}`
m           1
cm          :math:`10^{-2}`
mm          :math:`10^{-3}`
um          :math:`10^{-6}`
nm          :math:`10^{-9}`
s           1
ms          :math:`10^{-3}`
us          :math:`10^{-6}`
ns          :math:`10^{-9}`
==========  =================================

For example, one can write either 100*eV or 0.1*KeV to specify an energy in GMAD
and both are equivalent.

Lattice Description
-------------------

A model of the accelerator is given to BDSIM via input text files in the GMAD language.
The overall program strucutre should follow:

1) Component definition
2) Sequence defention (of the already defined components)
3) Which sequence to use
4) Where to record output (samplers)
5) A beam distribution
6) Options, including which physics lists, number to simulate etc.

These are described in the following sections

Lattice Elements
----------------

Any element in BDSIM is descrbied with the following pattern::

  type: name, paramter=value, parameter="string";

.. note:: Notice the ':', the inverted commas for a string parameter and that each
	  functional line must end with a semi-colon. Spaces will be ignored

The following elements may be defined

* `drift`_
* `rbend`_
* `sbend`_
* `quadrupole`_
* `sextupole`_
* `octupole`_
* `decapole`_
* `multipole`_
* `vkick`_
* `hkick`_
* `rf`_
* `rcol`_
* `ecol`_
* `muspoiler`_
* `solenoid`_
* `laser`_
* `transform3d`_
* `element`_
* `marker`_

These are detailed in the following sections.

drift
^^^^^

.. figure:: figures/drift.png
	    :width: 30%
	    :align: right

:code:`drift` defines a straight beam pipe with no field.

================  ===================  ==========  =========
parameter         description          default     required
`l`               length [m]           0.1         yes
`vacuumMaterial`  the vacuum material  vacuum      no
                  to use, can be user
		  defined
================  ===================  ==========  =========

* The `aperture parameters`_ may also be specified.

Examples
""""""""
::

   l203b: drift, l=1*m;
   l204c: drift, l=3*cm, beampipeRadius=10*cm;

rbend
^^^^^
.. figure:: figures/rbend.png
	    :width: 30%
	    :align: right
	    :figclass: align-right

`rbend` defines a rectangular bend magnet. Either the total bending angle, `angle`
for the nominal beam energy can be specified or the magnetic field, `B` in Tesla.
`B` overrides angle. The faces of the magnet are normal to the chord of the
input and output point. Furthermore, an additional very small drift section is
added on either side and the magnetic field up-scaled for the shorter field
length to ensure that the magnet body fits inside the start and end faces
of the element volume and doesn't protrude into the previous and next elements.

================  =====================  ==========  ===========
parameter         description            default     required
`l`               length [m]             0.1         yes
`angle`           angle [rad]            0           yes, or `B`
`B`               magnetic field [T]     0           yes
`material`        magnet outer material  Iron        no
================  =====================  ==========  ===========

* The `aperture parameters`_ may also be specified.
* The `magnet geometry parameters`_ may also be specified.

.. note:: For large angles (> 100 mrad) particles may hit the aperture as the beam pipe is
	  is represented by a straight (chord) section and even nominal energy particles
	  may hit the aperture depending on the degree of tracking accuracy specified. In this
	  case, consider splitting the `rbend` into multiple ones.

.. note:: As of v0.64 a combined quadrupole component is not possible, but is under
	  development

Examples
""""""""
::

   MRB20: rbend, l=3*m, angle=0.003;
   r1: rbend, l=5.43m, beampipeRadius=10*cm, B=2*Tesla;

sbend
^^^^^

.. figure:: figures/sbend.png
	    :width: 30%
	    :align: right

`sbend` defines a sector bend magnet. Either the total bending angle, `angle`
for the nominal beam energy can be specified or the magnetic field, `B` in Tesla.
`B` overrides angle. The faces of the magnet are normal to the curvlinear coordinate
system. `sbend` magnets are made of a series of straight segments. If the specified
(or calculated from `B` field) bending angle is large, the `sbend` is automatically
split such that the maximum tangential error in the aperture is 1 mm. For an LHC for
example with a bending angle of ~0.005rad and l = 14m, the magnet is typically split
into 5 cojoined `sbend` magnets.

================  =====================  ==========  ===========
parameter         description            default     required
`l`               length [m]             0.1         yes
`angle`           angle [rad]            0           yes, or `B`
`B`               magnetic field [T]     0           yes
`material`        magnet outer material  Iron        no
================  =====================  ==========  ===========

* The `aperture parameters`_ may also be specified.
* The `magnet geometry parameters`_ may also be specified.

.. note:: As of v0.64 a combined quadrupole component is not possible, but is under
	  development

Examples
""""""""
::

   s1: sbend, l=14.5*m, angle=0.005, magnetGeometryType="LHCRight";
   mb201x: sbend, l=304.2*cm, b=1.5*Tesla;

quadrupole
^^^^^^^^^^

.. figure:: figures/quadrupole.png
	    :width: 30%
	    :align: right

`quadrupole` defines a quadrupole magnet. The strength parameter `k1` is defined as
:math:`k1 = 1/(B \rho)~dB_{y}~/~dx~[m^{-2}]`. `ks1` specifies a skew quadrupole
component as with `k1` but rotated by 45 degrees.

================  ===========================  ==========  ===========
parameter         description                  default     required
`l`               length [m]                   0.1         yes
`k1`              quadrupole coefficient       0           yes
`ks1`             skew quadrupole coefficient  0           no
`material`        magnet outer material        Iron        no
================  ===========================  ==========  ===========

* The `aperture parameters`_ may also be specified.
* The `magnet geometry parameters`_ may also be specified.

Examples
""""""""
::

   q1: quadrupole, l=0.3*m, k1=45.23;
   qm15ff: quadrupole, l=20*cm, k1=95.2;

sextupole
^^^^^^^^^

.. figure:: figures/sextupole.png
	    :width: 30%
	    :align: right

`sextupole` defines a sextupole magnet. The strength parameter `k2` is defined as
:math:`k2 = 1/(B \rho)~dB^{2}_{y}~/~dx^{2}~[m^{-3}]`. `ks2` specifies a skew sextupole
component as with `k2` but rotated by 30 degrees.

================  ===========================  ==========  ===========
parameter         description                  default     required
`l`               length [m]                   0.1         yes
`k2`              sextupole coefficient        0           yes
`ks2`             skew sextupole coefficient   0           no
`material`        magnet outer material        Iron        no
================  ===========================  ==========  ===========

* The `aperture parameters`_ may also be specified.
* The `magnet geometry parameters`_ may also be specified.
		    
octupole
^^^^^^^^

.. figure:: figures/octupole.png
	    :width: 30%
	    :align: right

`octupole` defines an octupole magnet. The strength parameter `k3` is defined as
:math:`k3 = 1/(B \rho)~dB^{3}_{y}~/~dx^{3}~[m^{-4}]`. `ks3` specifies a skew octupole
component as with `k3` but rotated by 15 degrees.

================  ===========================  ==========  ===========
parameter         description                  default     required
`l`               length [m]                   0.1         yes
`k3`              octupole coefficient         0           yes
`ks3`             skew octupole coefficient    0           no
`material`        magnet outer material        Iron        no
================  ===========================  ==========  ===========

* The `aperture parameters`_ may also be specified.
* The `magnet geometry parameters`_ may also be specified.
		    
decapole
^^^^^^^^
.. warning:: To be completed - not yet implemented

`decapole` defines a decapole magnet. The strength parameter `k4` is defined as
:math:`k4 = 1/(B \rho)~dB^{4}_{y}~/~dx^{4}~[m^{-5}]`. `k43` specifies a skew decapole
component as with `k4` but rotated by 7.5 degrees.

================  ===========================  ==========  ===========
parameter         description                  default     required
`l`               length [m]                   0.1         yes
`k4`              decapole coefficient         0           yes
`ks4`             skew decapole coefficient    0           no
`material`        magnet outer material        Iron        no
================  ===========================  ==========  ===========

* The `aperture parameters`_ may also be specified.
* The `magnet geometry parameters`_ may also be specified.

multipole
^^^^^^^^^

`multipole` defines a general multipole magnet.

* The `aperture parameters`_ may also be specified.
* The `magnet geometry parameters`_ may also be specified.

vkick
^^^^^

`vkick` defines a vertical dipole magnet and has the same parameters as `sbend`.

* The `aperture parameters`_ may also be specified.
* The `magnet geometry parameters`_ may also be specified.

hkick
^^^^^

`hkick` defines a horizontal dipole magnet and has the same parameters as `sbend`.

* The `aperture parameters`_ may also be specified.
* The `magnet geometry parameters`_ may also be specified.

rf
^^^^

`rf` defines an rf cavity

================  ===========================  ==========  ===========
parameter         description                  default     required
`l`               length [m]                   0.1         yes
`gradient`        field gradien [MV/m]         0           yes
`material`        outer material               Iron        no
================  ===========================  ==========  ===========

* The `aperture parameters`_ may also be specified.

.. note:: Be careful with the sign of the gradient with respect to the sign of
	  the primary particle

rcol
^^^^

`rcol` defines a rectangular collimator. The aperture is rectangular and the eternal
volume is square.

================  ============================  ==========  ===========
parameter         description                   default     required
`l`               length [m]                    0.1         yes
`xsize`           horizontal half aperture [m]  0           yes
`ysize`           veritcal half aperture [m]    0           yes
`material`        outer material                Iron        no
`outerDiameter`   outer full width [m]          global      no
================  ============================  ==========  ===========

ecol
^^^^

`ecol` defines an elliptical collimator. This is exactly the same as `rcol` except that
the aperture is elliptical and the `xsize` and `ysize` define the horizontal and vertical
half axes repsectively.

muspoiler
^^^^^^^^^

`muspoiler` defines a muon spoiler, which is a rotationally magnetised iron cylinder with
a beam pipe in the middle. There is no magnetic field in the beam pipe.

================  ============================  ==========  ===========
parameter         description                   default     required
`l`               length [m]                    0.1         yes
`B`               magnetic field [T]            1           yes
`material`        outer material                Iron        no
`outerDiameter`   outer full width [m]          global      no
================  ============================  ==========  ===========

solenoid
^^^^^^^^

laser
^^^^^

transform3d
^^^^^^^^^^^

element
^^^^^^^

marker
^^^^^^

Aperture Parameters
^^^^^^^^^^^^^^^^^^^

For elements that contain a beam pipe, several aperture models can be used. These aperture
parameters can be set as the default for every element using the :code:`option` command and
can be overridden for each element by specifing them with the element definition.  The aperture
is controlled throught the following parameters:

* `apertureType`
* `beampipeRadius` or `aper1`
* `aper2`
* `aper3`
* `aper4`
* `vacuumMaterial`
* `beampipeThickness`
* `beampipeMaterial`


For each aperture model, a different number of parameters are required. Here, we follow the MADX
convention and have four parameters and the user must specify them as required for that model.
BDSIM will check to see if the combination of parameters is valid. `beampipeRadius` and `aper1`
are degenerate.
  
+-------------------+--------------+-------------------+-----------------+---------------+---------------+
| Aperture Model    | # of         | `aper1`           | `aper2`         | `aper3`       | `aper4`       |
|                   | parameters   |                   |                 |               |               |
+===================+==============+===================+=================+===============+===============+
| `circular`        | 1            | radius            | NA              | NA            | NA            |
+-------------------+--------------+-------------------+-----------------+---------------+---------------+
| `rectangular`     | 2            | x half width      | y half width    | NA            | NA            |
+-------------------+--------------+-------------------+-----------------+---------------+---------------+
| `elliptical`      | 2            | x semi-axis       | y semi-axis     | NA            | NA            |
+-------------------+--------------+-------------------+-----------------+---------------+---------------+
| `lhcscreensimple` | 3            | x half width of   | y half width of | radius of     | NA            |
|                   |              | rectangle         | rectangle       | circle        |               |
+-------------------+--------------+-------------------+-----------------+---------------+---------------+
| `lhcscreen`       | 3            | x half width of   | y half width of | radius of     | NA            |
|                   |              | rectangle         | rectangle       | circle        |               |
+-------------------+--------------+-------------------+-----------------+---------------+---------------+
| `rectellipse`     | 4            | x half width of   | y half width of | x semi-axis   | y semi-axis   |
|                   |              | rectangle         | rectangle       | of ellipse    | of ellipse    |
+-------------------+--------------+-------------------+-----------------+---------------+---------------+
| `racetrack`       | 3            | horizontal offset | vertical offset | radius of     | NA            |
|                   |              | of circle         | of circle       | circular part |               |
+-------------------+--------------+-------------------+-----------------+---------------+---------------+
| `octagon`         | 4            | x half width      | y half width    | angle 1 [rad] | angle 2 [rad] |
+-------------------+--------------+-------------------+-----------------+---------------+---------------+

Magnet Geometry Parameters
^^^^^^^^^^^^^^^^^^^^^^^^^^

Details here.

* `boxSize`
* `outerDiameter`


Lattice Sequence
----------------

Samplers - Output
-----------------

Physics Lists
-------------

Options
-------

Beam Parameters
---------------

.. _distributions-section:

Beam Distributions
------------------


- gauss
- gaussTwiss
- reference


after reading this, go back to the top in :ref:`distributions-section`

You can also have a look in another document at :ref:`output-analysis-section`


These parameters can be set with the *option* command as the default parameters
and also on a per element basis, that overrides the defaults for that specific element.
Up to four parameters
can be used to specify the aperture shape (*aper1*, *aper2*, *aper3*, *aper4*).
These are used differently for each aperture model and match the MADX aperture definitions.
The required parameters and their meaning are given in the following table.

Currently, only circular and rectangular are implemented.  More models will be completed shortly.

The outer volume is represented (with the exception of the *drift* 
element) by a cylinder with inner radius equal to the beampipe outer radius and
with outer radius given by default by the global *boxSize* option, which
can usually be overridden with the *outR* option.

In Geant4 it is possible to drive different *regions* each with their own production cuts and user limits.
In BDSIM three different regions exist, each with their own user defined production cuts (see *Physics*). 
These are the default region, the precision region and the approximation region. Beamline elements 
can be set to the precision region by setting the attribute *precisionRegion* equal to 1. For example:
