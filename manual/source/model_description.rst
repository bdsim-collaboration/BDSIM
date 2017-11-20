.. _model-description:

================================
Model Description - Input Syntax
================================

Lattice Description
-------------------

A model of the accelerator is given to BDSIM via input text files in the :ref:`gmad-syntax`.
The overall program structure should follow:

1) Component definition (see :ref:`lattice-elements`).
2) Sequence definition using defined components (see :ref:`lattice-sequence`).
3) Which sequence to use (see :ref:`the-use-command`).
4) Where to record output (see :ref:`sampler-output`).
5) Options, including which physics lists, number to simulate etc. (see :ref:`bdsim-options`).
6) A beam definition (see :ref:`beam-parameters`).
   
These are described in the following sections. Aside from these standard parameters, more
detail may be added to the model through:

 * :ref:`magnet-geometry-parameters`.
 * Custom :ref:`field-maps`.
 * Adding :ref:`externally-provided-geometry`.
 * :ref:`offsets-and-tilts`.

.. _gmad-syntax:

GMAD Syntax
-----------

GMAD is a language specifically for BDSIM that is made to be human readable.
The name comes from the design intention of MADX syntax + extensions for Geant4.
While GMAD is very similar to MADX, not all MADX commands are supported.

* S.I. units are used except where explicitly specified
* variables can be defined using :code:`name = value;` syntax
* arithmetic expressions can be defined
* binary operators +, -, \*, /, ^ are valid
* unary operators +, -, are valid
* boolean operators <, >, <=, >=, <> (not equal), == are valid
* every expression **must** end with a semi-colon;
* no variable name can begin with a number
* !comments start with an exclamation mark "!"
* a variable may inherit values (via copy) from another variable using :code:`newvariable : existingvariable;`

Mathematical Functions
^^^^^^^^^^^^^^^^^^^^^^
  
The following mathematical functions are provided:

* sqrt
* cos
* sin
* tan
* exp
* log
* acos
* asin
* atan
* abs

Other Commands
^^^^^^^^^^^^^^

* :code:`print;` prints all elements
* :code:`print, line;` prints all elements that are in the beam line defined by :code:`use`, see also `use - Defining which Line to Use`_
* :code:`print, option;` prints the value of some options.
* :code:`print, variable;` prints the value of a numerical variable, which could be your own defined variable.
* :code:`length = d1["l"];` way to access properties of elements, in this case length of element d1.
* :code:`stop;` or :code:`return;` exists parser
* :code:`if () {};` if construct
* :code:`if () {} else {};` if-else construct

Examples
^^^^^^^^
  
Examples::

   x = 1;
   y = 2.5-x;
   z = sin(x) + log(y) - 8e5;
   mat = "copper";

Coordinates & Units
-------------------

In Geant4, global Euclidean coordinates are used for tracking purposes, however,
in describing a lattice with BDSIM, curvilinear coordinates are used as is common with
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
electric voltage                [V] (Volts)
electric field strength         [V/m]
particle energy                 [GeV]
particle mass                   [GeV/c :math:`^2` ]
particle momentum               [GeV/c :math:`^2` ]
beam current                    [A] (Amperes)
particle charge                 [e] (elementary charges)
emittances                      [pi m mrad]
density                         [g/cm :math:`^{3}` ] 
temperature                     [K] (Kelvin)
pressure                        [atm] (atmosphere)
frequency                       [Hz] (Hertz)
mass number                     [g/mol]
==============================  =========================

Some useful predefined values / units are:

==========  =================================
Name        Value
==========  =================================
pi          3.14159265358979
degrees     :math:`\pi` / 180
GeV         1
eV          :math:`10^{-9}`
keV         :math:`10^{-6}`
MeV         :math:`10^{-3}`
TeV         :math:`10^{3}`
V           1
kV          :math:`10^{3}`
MV          :math:`10^{6}`
Tesla       1
rad         1
mrad        :math:`10^{-3}`
urad        :math:`10^{-6}`
clight      :math:`2.99792458 \times 10^{8}`
km          :math:`10^{3}`
m           1
cm          :math:`10^{-2}`
mm          :math:`10^{-3}`
um          :math:`10^{-6}`
mum         :math:`10^{-6}`
nm          :math:`10^{-9}`
pm          :math:`10^{-12}`
s           1
ms          :math:`10^{-3}`
us          :math:`10^{-6}`
ns          :math:`10^{-9}`
ps          :math:`10^{-12}`
Hz          1
kHz         :math:`10^{3}`
MHz         :math:`10^{6}`
GHz         :math:`10^{9}`
==========  =================================

For example, one can write either :code:`100*eV` or :code:`0.1*keV` to specify an energy in GMAD
and both are equivalent.

.. _lattice-elements:

Lattice Elements
----------------

BDSIM provides a variety of different elements each with their own function, geometry and
potentially fields. Any element in BDSIM is described with the following pattern::

  name: type, parameter=value, parameter="string";

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
* `thinmultipole`_
* `vkicker`_
* `hkicker`_
* `kicker`_
* `rf`_
* `rcol`_
* `ecol`_
* `degrader`_
* `muspoiler`_
* `shield`_
* `solenoid`_
* `laser`_
* `transform3d`_
* `element`_
* `marker`_

.. TODO add screen, awakescreen

These are detailed in the following sections. 

Simple example, extend and copy
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Example::

  d1: drift, l=5*m;

This defines a drift element with name `d1` and a length of 5 metres. The definition can later be changed or extended with::

  d1: l=3*m, aper=0.1*m;

Note the omission of the type `drift`. This will change the length of `d1` to 3 metres and set the aperture to 10 centimetres. An element can also be defined by copying an existing element::

  d2: d1, l=2*m;

Element `d2` is a drift with the properties of `d1` and a length of 2 metres. Note that if `d1` is changed again, `d2` will **not** change.

Magnet Strength Polarity
^^^^^^^^^^^^^^^^^^^^^^^^

.. note:: BDSIM strictly follows the MADX definition of magnet strength parameter
	  `k` - "a **positive** `k` corresponds to **horizontal focussing** for a
	  **positively** charged particle. This therefore indicates a positive `k`
	  corresponds to horizontal defocussing for a negatively charged particle.
	  However, MADX treats all particles as positively charged for tracking purposes.

.. versionadded:: 0.7

		  
		  BDSIM currently treats k absolutely so to convert a MADX lattice for
		  negatively particles, the MADX k values must be multiplied by -1. The
		  pybdsim converter provides an option called `flipmagnets` for this
		  purpose.  This may be revised in future releases depending on changes
		  to MADX.
		  

drift
^^^^^

.. figure:: figures/drift.png
	    :width: 30%
	    :align: right

`drift` defines a straight beam pipe with no field.

================  ===================  ==========  =========
parameter         description          default     required
`l`               length [m]           0           yes
`vacuumMaterial`  the vacuum material  vacuum      no
                  to use, can be user
		  defined
================  ===================  ==========  =========


* The `aperture parameters`_ may also be specified.

Examples::

   l203b: drift, l=1*m;
   l204c: drift, l=3*cm, beampipeRadius=10*cm;

rbend
^^^^^

.. figure:: figures/rbend.png
	    :width: 30%
	    :align: right

.. |angleFieldComment| replace:: Either the total bending angle, `angle` for the nominal beam
				 energy can be specified or the magnetic field, `B` in Tesla.
				 If both are defined the magnet is under or over-powered.

`rbend` defines a rectangular bend magnet. |angleFieldComment| 
The faces of the magnet are normal to the chord of the 
input and output point. Pole face rotations can be applied to both the input
and output faces of the magnet, based upon the reference system shown in the above image.

================  ===========================  ==========  ===========
parameter         description                  default     required
`l`               length [m]                   0           yes
`angle`           angle [rad]                  0           yes, or `B`
`B`               magnetic field [T]           0           yes
`e1`              input poleface angle [rad]   0           no
`e2`              output poleface angle [rad]  0           no
`material`        magnet outer material        Iron        no
================  ===========================  ==========  ===========

* The `aperture parameters`_ may also be specified.
* The `magnet geometry parameters`_ may also be specified.
* `yokeOnInside` from the `magnet geometry parameters`_ may be specified.

.. note:: For large angles (> 100 mrad) particles may hit the aperture as the beam pipe is
	  is represented by a straight (chord) section and even nominal energy particles
	  may hit the aperture depending on the degree of tracking accuracy specified. In this
	  case, consider splitting the `rbend` into multiple ones.

.. note:: As of v0.64 a combined quadrupole component is not possible, but is under
	  development

.. figure:: figures/poleface_notation_rbend.pdf
	    :width: 75%
	    :align: center

.. note:: The poleface rotation angle is limited to :math:`\pm \pi /4` radians.

.. note:: If a non-zero poleface rotation angle is specified, the element preceding / succeeding
	  the rotated magnet face must either be a drift or an rbend with opposite rotation (e.g. an sbend with
	  :math:`e2 = 0.1` can be followed by an sbend with :math:`e1 = -0.1`). The preceding / succeeding
	  element must be longer than the projected length from the rotation, given by
	  :math:`2 \tan(\mathrm{eX})`.

.. note:: If an rbend has a poleface with non-zero rotation angle, and the option `includeFringeFields=1` is
      specified (see `options`_), then a thin fringefield magnet (1 micron thick by default) is included
      at the beginning (for non-zero e1) or at the end (for non-zero e2) of the rbend. The length of the
      fringefield element can be set by the option `thinElementLength` (see `options`_).

	  

Examples::

   MRB20: rbend, l=3*m, angle=0.003;
   r1: rbend, l=5.43m, beampipeRadius=10*cm, B=2*Tesla;
   RB04: rbend, l=1.8*m, angle=0.05, e1=0.1, e2=-0.1

sbend
^^^^^

.. figure:: figures/sbend.png
	    :width: 30%
	    :align: right
	    

`sbend` defines a sector bend magnet. |angleFieldComment| 
The faces of the magnet are normal to the curvilinear coordinate
system. `sbend` magnets are made of a series of straight segments. If the specified
(or calculated from `B` field) bending angle is large, the `sbend` is automatically
split such that the maximum tangential error in the aperture is 1 mm. Sbend magnets are
typically split into several co-joined `sbend` magnets, the number depending on the magnet
length and bending angle. Pole face rotations can be applied to both the input
and output faces of the magnet, based upon the reference system shown in the above image.

================  ====================================  ==========  ===========
parameter         description                           default     required
`l`               length [m]                            0           yes
`angle`           angle [rad]                           0           yes, or `B`
`B`               magnetic field [T]                    0           yes
`e1`              input poleface angle [rad]            0           no
`e2`              output poleface angle [rad]           0           no
`material`        magnet outer material                 Iron        no
`fint`            fringe field integral for exit face   0           no
`fintx`           fringe field integral for entrance    0           no
`hgap`            vertical gap for fringe field [m]     0           no
================  ====================================  ==========  ===========

* The `aperture parameters`_ may also be specified.
* The `magnet geometry parameters`_ may also be specified.
* `yokeOnInside` from the `magnet geometry parameters`_ may be specified.

.. note:: As of v0.64 a combined quadrupole component is not possible, but is under
	  development

.. figure:: figures/poleface_notation_sbend.pdf
	    :width: 75%
	    :align: center

.. note:: The poleface rotation angle is limited to :math:`\pm \pi /4` radians.

.. note:: If a non-zero poleface rotation angle is specified, the element preceding / succeeding
	  the rotated magnet face must either be a drift or an rbend with opposite rotation (e.g. an sbend with
	  :math:`e2 = 0.1` can be followed by an sbend with :math:`e1 = -0.1`). The preceding / succeeding
	  element must be longer than the projected length from the rotation, given by
	  :math:`2 \tan(\mathrm{eX})`.

.. note:: If an sbend has a poleface with non-zero rotation angle, and the option `includeFringeFields=1` is
	  specified (see `options`_), then a thin fringefield magnet (1 micron thick by default) is included
	  at the beginning (for non-zero e1) or at the end (for non-zero e2) of the sbend. The length of the
	  fringefield element can be set by the option `thinElementLength` (see `options`_).

.. note:: Unlike MADX, `fint` is used exclusively for the input face fringe field integral and `fintx` for
	  the exit face.  

Examples::

   s1: sbend, l=14.5*m, angle=0.005, magnetGeometryType="lhcright";
   mb201x: sbend, l=304.2*cm, b=1.5*Tesla;
   SB17A: sbend, l=0.61*m, angle=0.016, e1=-0.05, e2=0.09

quadrupole
^^^^^^^^^^

.. figure:: figures/quadrupole.png
	    :width: 30%
	    :align: right

`quadrupole` defines a quadrupole magnet. The strength parameter :math:`k1` is defined as
:math:`k1 = 1/(B \rho)~dB_{y}~/~dx~[m^{-2}]`.

================  ===========================  ==========  ===========
parameter         description                  default     required
`l`               length [m]                   0           yes
`k1`              quadrupole coefficient       0           yes
`material`        magnet outer material        Iron        no
================  ===========================  ==========  ===========

* The `aperture parameters`_ may also be specified.
* The `magnet geometry parameters`_ may also be specified.
* See `Magnet Strength Polarity`_ for polarity notes.

Examples::

   q1: quadrupole, l=0.3*m, k1=45.23;
   qm15ff: quadrupole, l=20*cm, k1=95.2;

sextupole
^^^^^^^^^

.. figure:: figures/sextupole.png
	    :width: 30%
	    :align: right

`sextupole` defines a sextupole magnet. The strength parameter :math:`k2` is defined as
:math:`k2 = 1/(B \rho)~dB^{2}_{y}~/~dx^{2}~[m^{-3}]`.

================  ===========================  ==========  ===========
parameter         description                  default     required
`l`               length [m]                   0           yes
`k2`              sextupole coefficient        0           yes
`material`        magnet outer material        Iron        no
================  ===========================  ==========  ===========

* The `aperture parameters`_ may also be specified.
* The `magnet geometry parameters`_ may also be specified.
* See `Magnet Strength Polarity`_ for polarity notes.

Examples::

   sx1: sextupole, l=0.5*m, k2=4.678;
   sx2: sextupole, l=20*cm, k2=45.32, magnetGeometry="normalconducting";
		    
octupole
^^^^^^^^

.. figure:: figures/octupole.png
	    :width: 30%
	    :align: right

`octupole` defines an octupole magnet. The strength parameter :math:`k3` is defined as
:math:`k3 = 1/(B \rho)~dB^{3}_{y}~/~dx^{3}~[m^{-4}]`.

================  ===========================  ==========  ===========
parameter         description                  default     required
`l`               length [m]                   0           yes
`k3`              octupole coefficient         0           yes
`material`        magnet outer material        Iron        no
================  ===========================  ==========  ===========

* The `aperture parameters`_ may also be specified.
* The `magnet geometry parameters`_ may also be specified.
* See `Magnet Strength Polarity`_ for polarity notes.

Examples::

   oct4b: octupole, l=0.3*m, k3=32.9;

decapole
^^^^^^^^

.. TODO: add picture

`decapole` defines a decapole magnet. The strength parameter :math:`k4` is defined as
:math:`k4 = 1/(B \rho)~dB^{4}_{y}~/~dx^{4}~[m^{-5}]`.

================  ===========================  ==========  ===========
parameter         description                  default     required
`l`               length [m]                   0           yes
`k4`              decapole coefficient         0           yes
`material`        magnet outer material        Iron        no
================  ===========================  ==========  ===========

* The `aperture parameters`_ may also be specified.
* The `magnet geometry parameters`_ may also be specified.
* See `Magnet Strength Polarity`_ for polarity notes.

Examples::

   MXDEC3: decapole, l=0.3*m, k4=32.9;

multipole
^^^^^^^^^

`multipole` defines a general multipole magnet. The strength parameter
:math:`knl` is a list defined as
:math:`knl[n] = 1/(B \rho)~dB^{n}_{y}~/~dx^{n}~[m^{-(n+1)}]`
starting with the quadrupole component.
The skew strength parameter :math:`ksl` is a list representing the skew coefficients.  
   
================  ===========================  ==========  ===========
parameter         description                  default     required
`l`               length [m]                   0           yes
`knl`             list of normal coefficients  0           no
`ksl`             list of skew coefficients    0           no
`material`        magnet outer material        Iron        no
================  ===========================  ==========  ===========

* The `aperture parameters`_ may also be specified.
* The `magnet geometry parameters`_ may also be specified.
* See `Magnet Strength Polarity`_ for polarity notes.
  
Examples::

   OCTUPOLE1 : multipole, l=0.5*m , knl={ 0,0,1 } , ksl={ 0,0,0 };

thinmultipole
^^^^^^^^^^^^^

`thinmultipole` is the same a multipole, but is set to have a default length of 1 micron.
For thin multipoles, the length parameter is not required. The element will appear as a thin length of drift
tube. A thinmultipole can be placed next to a bending magnet with finite poleface rotation angles.

Examples::

   THINOCTUPOLE1 : thinmultipole , knl={ 0,0,1 } , ksl={ 0,0,0 };

.. note:: The length of the thin multipole can be changed by setting `thinElementLength` (see `options`_).

vkicker
^^^^^^^

`vkicker` can either be a thin vertical kicker or a thick vertical dipole magnet. If specified
with a finite length :code:`l`, it will be constructed as a dipole. However, if no length or
a length of exactly 0 is specified, a thin kicker will be built. This it typically a 1um slice
with only the shape of the aperture and no surrounding geometry. It is also typically not
visible with the default visualisation settings.

The strength is specified by the parameter :code:`vkick`, which is the fractional momentum kick
in the vertical direction. A positive value corresponds to an increase in :math:`p_y`. In the
case of the thin kicker the position is not affect, whereas with the thick kicker, the position
will change.

In the case of a thick kicker, the resulting bending angle is calculated as:

.. math::

   \theta = \sin^{-1}(\,p_x)

The dipole field strength is then calculated with respect to the chord length:

.. math::

   \mathbf{B} = B\rho\, \frac{\theta}{\mathrm{chord\,length}}


* The `aperture parameters`_ may also be specified.
* For a vkicker with a finite length, the `magnet geometry parameters`_ may also be specified.

Examples::

   KX15v: vkicker, vkick=1.3e-5;
   KX17v: vkicker, vkick=-2.4e-2, l=0.5*m;

hkicker
^^^^^^^

`hkicker` can either be a thin horizontal kicker or a thick horizontal dipole magnet. If
specified
with a finite length :code:`l`, it will be constructed as a dipole. However, if no length or
a length of exactly 0 is specified, a thin kicker will be built. This it typically a 1um slice
with only the shape of the aperture and no surrounding geometry. It is also typically not
visible with the default visualisation settings.

The strength is specified by the parameter :code:`hkick`, which is the fractional momentum kick
in the vertical direction. A positive value corresponds to an increase in :math:`p_x`. In the
case of the thin kicker the position is not affect, whereas with the thick kicker, the position
will change.

.. note:: A positive value of `hkick` causes an increase in horizontal momentum so the particle
	  will bend to the left looking along the beam line, i.e. in positive `x`. This is
	  the opposite of a bend where a positive *angle* causes a deflection in negative
	  `x`.

* The `aperture parameters`_ may also be specified.
* For a hkicker with a finite length, the `magnet geometry parameters`_ may also be specified.

Examples::

   KX17h: hkicker, hkick=0.01;
   KX19h: hkicker, hkick=-1.3e-5, l=0.2*m;

kicker
^^^^^^

`kicker` defines a combined horizontal and vertical kicker.  Either both or one of the
parameters `hkick` and `vkick` may be specified. Like the `hkicker` and `vkicker`, this
may also be thin or thick. In the case of the thick kicker, the field is the linear
sum of two independently calculated fields.

Example::

  kick1: kicker, l=0.45*m, hkick=1.23e-4, vkick=0.3e-4;


tkicker
^^^^^^^

BDSIM, like MADX, provides a `tkicker` element. This is an alias in BDSIM for a `kicker`_,
however MADX differentiates the two on the basis of fitting parameters. BDSIM does
not make this distinction. See `kicker`_ for more details.


rf
^^^^

.. figure:: figures/rfcavity.png
	    :width: 50%
	    :align: right

`rf` or `rfcavity` defines an RF cavity with a time varying electric or electro-magnetic field.
There are several geometry and field options as well as ways to specify the strength.
The default field is a uniform (in space) electric-only field that is time varying
according to a simple sinusoid.  Optionally, the electro-magnetic field for a pill-box
cavity may be used. The `G4ClassicalRK4` numerical integrator is used to calculate
the motion of particles in both cases.


+----------------+-------------------------------+--------------+---------------------+
| **Parameter**  | **Description**               | **Default**  | **Required**        |
+================+===============================+==============+=====================+
| `l`            | length [m]                    | 0            | yes                 |
+----------------+-------------------------------+--------------+---------------------+
| `E`            | electric field strength       | 0            | yes (or `gradient`) |
+----------------+-------------------------------+--------------+---------------------+
| `gradient`     | field gradient [MV/m]         | 0            | yes                 |
+----------------+-------------------------------+--------------+---------------------+
| `frequency`    | frequency of oscillation (Hz) | 0            | yes                 |
+----------------+-------------------------------+--------------+---------------------+
| `phase`        | phase offset (rad)            | 0            | no                  |
+----------------+-------------------------------+--------------+---------------------+
| `tOffset`      | offset in time (ns)           | 0            | no                  |
+----------------+-------------------------------+--------------+---------------------+
| `material`     | outer material                | ""           | yes                 |
+----------------+-------------------------------+--------------+---------------------+
| `cavityModel`  | name of cavity model object   | ""           | no                  |
+----------------+-------------------------------+--------------+---------------------+

.. note:: The design energy of the machine is not affected, so the strength and fields
	  of components after an RF cavity in a lattice are calculated with respect to
	  the design energy and particle and therefore design rigidity. The user should
	  scale the strength values appropriately if they wish to match the increased
	  energy of the particle.

.. warning:: The elliptical cavity geometry may not render or appear in the Geant4
	     QT visualiser.  The geometry exists and is valid, but this is due to
	     deficiencies of the Geant4 visualisation system. The geometry exists
	     and is fully functional.

* The field is such that a positive E field results in acceleration of the primary particle.
* The phase is calculated automatically such that 0 phase results in the peak E field at
  the centre of the component for its position in the lattice.
* Either `tOffset` or `phase` may be used to specify the phase of the oscillator.
* The material must be specified in the `rf` gmad element or in the attached cavity model
  by name. The cavity model will override the element material.

If `tOffset` is specified, a phase offset is calculated from this time for the speed
of light in vacuum. Otherwise, the curvilinear S-coordinate of the centre of the rf
element is used to find the phase offset.

If `phase` is specified, this is added to calculated phase offset from either the lattice
position or `tOffset`.

Simple examples::

   rf1: rf, l=10*cm, E=10*MV, frequency=90*MHz, phase=0.02;
   rf2: rf, l=10*cm, gradient=14*MV / m, frequency=450*MHz;
   rf3: rf, l=10*cm, E=10*MV, frequency=90*MHz, tOffset=3.2*ns;

Rather than just a simple E field, an electro-magnetic field that is the solution to
a cylindrical pill-box cavity may be used. A cavity object (described in more detail
below) is used to specify the field type. All other cavity parameters may be safely ignored
and only the field type will be used. The field is described in :ref:`field-pill-box`.

Pill-Box field example::

  rffield: field, type="rfcavity";
  rf4: rf, l=10*cm, E=2*kV, frequency=1.2*GHz, fieldVacuum="rffield";
   
Elliptical SRF cavity geometry is also provided and may be specified by use of another
'cavity' object in the parser.  This cavity object can then be attached to an `rf`
object by name. Details can be found in :ref:`cavity-geometry-parameters`.



rcol
^^^^

.. figure:: figures/rcol.png
	    :width: 30%
	    :align: right

`rcol` defines a rectangular collimator. The aperture is rectangular and the eternal
volume is square.

================  =================================  ==========  ===========
parameter         description                        default     required
`l`               length [m]                         0           yes
`xsize`           horizontal half aperture [m]       0           yes
`ysize`           vertical half aperture [m]         0           yes
`xsizeOut`        horizontal exit half aperture [m]  0           no
`ysizeOut`        vertical exit half aperture [m]    0           no
`material`        outer material                     Iron        no
`outerDiameter`   outer full width [m]               global      no
================  =================================  ==========  ===========

.. note:: The collimator can be tapered by specifying an exit aperture size with `xsizeOut` and
	  `ysizeOut`, with the `xsize` and `ysize` parameters then defining the entrance aperture.


Examples::

   ! Standard
   TCP15: rcol, l=1.22*m, material="graphite", xsize=104*um, ysize=5*cm;

   ! Tapered
   TCP16: rcol, l=1.22*m, material="graphite", xsize=104*um, ysize=5*cm, xsizeOut=208*um, ysizeOut=10*cm;


ecol
^^^^

.. figure:: figures/ecol.png
	    :width: 30%
	    :align: right

`ecol` defines an elliptical collimator. This is exactly the same as `rcol` except that
the aperture is elliptical and the `xsize` and `ysize` define the horizontal and vertical
half axes respectively. When tapered, the ratio between the horizontal and vertical half
axes of the entrance aperture must be the same ratio for the exit aperture.


degrader
^^^^^^^^

.. figure:: figures/degrader.png
        :width: 70%
        :align: right

`degrader` defines an interleaved pyramidal degrader that decreases the beam's energy.

.. tabularcolumns:: |p{4cm}|p{4cm}|p{2cm}|p{2cm}|

===================    =======================================  ==========  ===========
parameter              description                              default     required
`l`                    length [m]                               0           yes
`numberWedges`         number of degrader wedges                1           yes
`wedgeLength`          degrader wedge length [m]                0           yes
`degraderHeight`       degrader height [m]                      0           yes
`materialThickness`    amount of material seen by the beam [m]  0           yes/no*
`degraderOffset`       horizontal offset of both wedge sets     0           yes/no*
`material`             degrader material                        Carbon      yes
`outerDiameter`        outer full width [m]                     global      no
===================    =======================================  ==========  ===========

.. note:: Either `materialThickness` or `degraderOffset` can be specified to adjust the horizontal lateral wedge
          position, and consequently the total material thickness the beam can propagate through. If both are
          specified, `degraderOffset` will be ignored.
	  
          When numberWedges is specified to be n, the degrader will consist of n-1 `full` wedges and two `half` wedges.
          When viewed from above, a `full` wedge appears as an isosceles triangle, and a `half` wedge appears as a right-angled
          triangle.

Examples::

    DEG1: degrader, l=0.25*m, material="carbon", numberWedges=5, wedgeLength=100*mm, degraderHeight=100*mm, materialThickness=200*mm;
    DEG2: degrader, l=0.25*m, material="carbon", numberWedges=5, wedgeLength=100*mm, degraderHeight=100*mm, degraderOffset=50*mm,


muspoiler
^^^^^^^^^

.. figure:: figures/muspoiler.png
	    :width: 30%
	    :align: right

`muspoiler` defines a muon spoiler, which is a rotationally magnetised iron cylinder with
a beam pipe in the middle. There is no magnetic field in the beam pipe.

================  ============================  ==========  ===========
parameter         description                   default     required
`l`               length [m]                    0           yes
`B`               magnetic field [T]            0           yes
`material`        outer material                Iron        no
`outerDiameter`   outer full width [m]          global      no
================  ============================  ==========  ===========

shield
^^^^^^

.. figure:: figures/shield.png
	    :width: 30%
	    :align: right

`shield` defines a square block of material with a square aperture. The user may choose
the outer width, and inner horizontal and vertical apertures of the block. A beam pipe
is also placed inside the aperture.  If the beam pipe dimensions (including thickness)
are greater than the aperture, the beam pipe will not be created.

================  ==============================  ==========  ===========
parameter         description                     default     required
`l`               length [m]                      0           yes
`material`        outer material                  Iron        no
`outerDiameter`   outer full width [m]            global      no
`xsize`           horizontal inner aperture [m]   0           no
`ysize`           vertical inner aperture [m]     0           no
================  ==============================  ==========  ===========

* The `aperture parameters`_ may also be specified.

solenoid
^^^^^^^^

.. figure:: figures/solenoid.png
	    :width: 30%
	    :align: right

`solenoid` defines a solenoid magnet. This utilises a thick lens transfer map with a
hard edge field profile so it is not equivalent to split a single solenoid into multiple
smaller ones. **This is currently under development**. The strength parameter `ks` is
defined as :math:`ks =`.

================  ============================  ==========  ===========
parameter         description                   default     required
`l`               length [m]                    0           yes
`ks`              solenoid strength [ ]         0           yes
`material`        outer material                Iron        no
`outerDiameter`   outer full width [m]          global      no
================  ============================  ==========  ===========

* See `Magnet Strength Polarity`_ for polarity notes.

Examples::

   atlassol: solenoid, l=20*m, ks=0.004;


laser
^^^^^

`laser` defines a drift section with a laser beam inside. The laser acts as a static target
of photons.

================  =================================================  ==========  ===========
parameter         description                                        default     required
`l`               length of drift section [m]                        0           yes
`x`, `y`, `z`     components of laser direction vector (normalised)  (1,0,0)     yes
`waveLength`      laser wavelength [m]                               532*nm      yes
================  =================================================  ==========  ===========

Examples::

   laserwire: laser, l=1*um, x=1, y=0, z=0, wavelength=532*nm;


transform3d
^^^^^^^^^^^

`transform3d` defines an arbitrary 3-dimensional transformation of the curvilinear coordinate
system at that point in the beam line sequence.  This is often used to rotate components by a large
angle.


================  ============================  ==========  ===========
parameter         description                   default     required
`x`               x offset                      0           no
`y`               y offset                      0           no
`z`               z offset                      0           no
`phi`             phi Euler angle               0           no
`theta`           theta Euler angle             0           no
`psi`             psi Euler angle               0           no
================  ============================  ==========  ===========

.. note:: this permanently changes the coordinate frame, so care must be taken to undo any rotation
	  if it intended for only one component.

Examples::

   rcolrot: transform3d, psi=pi/2;

.. _element:
   
element
^^^^^^^

`element` defines an arbitrary element that's defined by external geometry and magnetic field
maps. Several geometry formats are supported. The user must supply the length (accurately) as
well as a diameter such that the geometry will be contained in a box that has horizontal and
vertical size of diameter.

================  ===============================  ==========  ===========
parameter         description                      default     required
`geometry`        filename of geometry             NA          yes
`l`               length                           NA          yes
`outerDiameter`   diameter of component [m]        NA          yes
`fieldAll`        name of field object to use      NA          no
================  ===============================  ==========  ===========

`geometry` should be of the format `format:filename`, where `format` is the geometry
format being used (`gdml` | `gmad` | `mokka`) and filename is the path to the geometry
file. See :ref:`externally-provided-geometry` for more details.

`fieldAll` should refer to the name of a field object the user has defined in the input
gmad file. The syntax for this is described in :ref:`field-maps`.

.. note:: The length must be larger than the geometry so that it is contained within it and
	  no overlapping geometry will be produced. However, care must be taken as the length
	  will be the length of the component inserted in the beamline.  If this is much larger
	  than the size required for the geometry, the beam may be mismatched into the rest of
	  the accelerator. A common practice is to add a picometre to the length of the geometry.

Simple example::

  detector: element, geometry="gdml:atlasreduced.gdml", outerDiameter=10*m,l=44*m;

Example with field::

  somefield: field, type="ebmap2d",
		    eScaling = 3.1e3,
		    bScaling = 0.5,
		    integrator = "g4classicalrk4",
		    magneticFile = "poisson2d:/Path/To/File.TXT",
		    magneticInterpolator = "nearest2D",
		    electricFile = "poisson2d:/Another/File.TX",
		    electricInterpolator = "linear2D";
  
   detec: element, geometry="mokka:qq.sql", fieldAll="somefield", l=5*m, outerDiameter=0.76*m;



marker
^^^^^^
`marker` defines a point in the lattice. This element has no physical length and is only
used as a reference. For example, a `sampler` (see `samplers - output`_ )
is used to record particle passage at the
front of a component but how would you record particles exiting a particular component?
The intended method is to use a `marker` and place it in the sequence after that element
then attach a sampler to the marker.

Examples::

   m1: marker;


Colours
-------

A few items allow you to define a custom colour for them to aid in visualisation. Currently,
only `rcol`_ and `ecol`_ respond to this. The colour can be defined in with an RGB colour code
where the RGB values are space delimited and given from 0 to 255. Once the colour name has
been defined it may be used again without having to redefine the components. Once defined, a
colour may not be redefined.

Examples::

  col1: rcol, l=0.2*m, xsize=5*cm, ysize=4*cm, colour="crimson:220  20 60", material="copper";
  col2: rcol, l=0.2*m, xsize=10*cm, ysize=6*cm, colour="crimson", material="Iron";
   

Aperture Parameters
-------------------

For elements that contain a beam pipe, several aperture models can be used. These aperture
parameters can be set as the default for every element using the :code:`option` command
(see `options`_ ) and
can be overridden for each element by specifying them with the element definition.  The aperture
is controlled through the following parameters:

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
| `lhc`             | 3            | x half width of   | y half width of | radius of     | NA            |
|                   |              | rectangle         | rectangle       | circle        |               |
+-------------------+--------------+-------------------+-----------------+---------------+---------------+
| `lhcdetailed`     | 3            | x half width of   | y half width of | radius of     | NA            |
|                   |              | rectangle         | rectangle       | circle        |               |
+-------------------+--------------+-------------------+-----------------+---------------+---------------+
| `rectellipse`     | 4            | x half width of   | y half width of | x semi-axis   | y semi-axis   |
|                   |              | rectangle         | rectangle       | of ellipse    | of ellipse    |
+-------------------+--------------+-------------------+-----------------+---------------+---------------+
| `racetrack`       | 3            | horizontal offset | vertical offset | radius of     | NA            |
|                   |              | of circle         | of circle       | circular part |               |
+-------------------+--------------+-------------------+-----------------+---------------+---------------+
| `octagonal`       | 4            | x half width      | y half width    | x point of    | y point of    |
|                   |              |                   |                 | start of edge | start of edge |
+-------------------+--------------+-------------------+-----------------+---------------+---------------+

These parameters can be set with the *option* command as the default parameters
and also on a per element basis, that overrides the defaults for that specific element.
Up to four parameters
can be used to specify the aperture shape (*aper1*, *aper2*, *aper3*, *aper4*).
These are used differently for each aperture model and match the MADX aperture definitions.
The required parameters and their meaning are given in the following table.

.. _magnet-geometry-parameters:

Magnet Geometry Parameters
--------------------------

As well as the beam pipe, magnet beam line elements also have further outer geometry beyond the
beam pipe. This geometry typically represents the magnetic poles and yoke of the magnet but there
are several geometry types to choose from. The possible different styles are described below and
syntax **examples** can be found in *examples/features/geometry/4_magnets/*.

* Externally provided geometry can also be wrapped around the beam pipe (detailed below).

The magnet geometry is controlled by the following parameters.

.. note:: These can all be specified using the `option` command as well as on a per element
	  basis, but in this case they act as a default that will be used if none are
	  specified by the element.

+-----------------------+--------------------------------------------------------------+---------------+-----------+
| Parameter             | Description                                                  | Default       | Required  |
+=======================+==============================================================+===============+===========+
| `magnetGeometryType`  | | The style of magnet geometry to use. One of:               | `polessquare` | no        |
|                       | | `cylindrical`, `polescircular`, `polessquare`,             |               |           |
|                       | | `polesfacet`, `polesfacetcrop`, `lhcleft`, `lhcright`,     |               |           |
|                       | | `none` and `format:path`.                                  |               |           |
+-----------------------+--------------------------------------------------------------+---------------+-----------+
| `outerDiameter`       | **Full** horizontal width of the magnet (m)                  | 0.6 m         | no        |
+-----------------------+--------------------------------------------------------------+---------------+-----------+
| `outerMaterial`       | Material of the magnet                                       | "iron"        | no        |
+-----------------------+--------------------------------------------------------------+---------------+-----------+
| `yokeOnInside`        | | Whether the yoke of a dipole appears on the inside of the  | 1             | no        |
|                       | | bend and if false, it's on the outside. Applicable only    |               |           |
|                       | | to dipoles.                                                |               |           |
+-----------------------+--------------------------------------------------------------+---------------+-----------+

Example::

  option, magnetGeometryType = "polesfacetcrop",
          outerDiameter = 0.5*m;

::

   m1: quadrupole, l=0.3*m,
                   k1=0.03,
		   magnetGeometryType="gdml:geometryfiles/quad.gdml",
		   outerDiameter = 0.5*m;


.. deprecated:: 0.65
		`boxSize` - this is still accepted by the parser for backwards compatibility
		but users should use the `outerDiameter` keyword where possible.

.. warning:: The choice of magnet outer geometry will significantly affect the beam loss pattern in the
	     simulation as particles and radiation may propagate much further along the beam line when
	     a magnet geometry with poles is used.

.. note:: Should a custom selection of various magnet styles be required for your simulation, please
	  contact us (see :ref:`feature-request` and this can be added - it is a relatively simple processes.

No Magnet Outer Geometry - "`none`"
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

No geometry for the magnet outer part is built at all and nothing is place in the model. This results
in only a beam pipe with the correct fields being provided.

.. image:: figures/none_beamline.png
	   :width: 60%
	   :align: center

Cylindrical - "`cylindrical`"
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The beam pipe is surrounded by a cylinder of material (the default is iron) whose outer diameter
is controlled by the `outerDiameter` parameter. In the case of beam pipes that are not circular
in cross-section, the cylinder fits directly against the outside of the beam pipe.

This geometry is useful when a specific geometry is not known. The surrounding
magnet volume acts to produce secondary radiation as well as act as material for energy deposition,
therefore this geometry is best suited for the most general studies.

.. figure:: figures/cylindrical_quadrupole.png
	    :width: 40%

.. figure:: figures/cylindrical_sextupole.png
	    :width: 40%
    

Poles Circular - "`polescircular`"
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This magnet geometry has simple iron poles according to the order of the magnet and the yoke is
represented by an annulus. Currently no coils are implemented. If a non-symmetric beam pipe
geometry is used, the larger of the horizontal and vertical dimensions of the beam pipe will be
used to create the circular aperture at the pole tips.

.. figure:: figures/polescircular_quadrupole.png
	    :width: 40%

.. figure:: figures/polescircular_quadrupole_3d.png
	    :width: 40%

.. figure:: figures/polescircular_sextupole.png
	    :width: 40%

.. figure:: figures/polescircular_sextupole_3d.png
	    :width: 40%


Poles Square (Default) - "`polessquare`"
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This magnet geometry has again, individual poles according to the order of the magnet but the
yoke is an upright square section to which the poles are attached. This geometry behaves in the
same way as `polescircular` with regard to the beam pipe size.

`outerDiameter` is the full width of the magnet horizontally as shown in the figure below, 
**not** the diagonal width.

.. figure:: figures/polessquare_quadrupole.png
	    :width: 40%

.. figure:: figures/polessquare_quadrupole_3d.png
	    :width: 40%

.. figure:: figures/polessquare_sextupole.png
	    :width: 40%

.. figure:: figures/polessquare_sextupole_3d.png
	    :width: 40%


Poles Faceted - "`polesfacet`"
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This magnet geometry is much like `polessquare`, however the yoke is such that the pole always
joins at a flat piece of yoke and not in a corner. This geometry behaves in the
same way as `polescircular` with regard to the beam pipe size.

`outerDiameter` is the full width as shown in the figure.

.. figure:: figures/polesfacet_quadrupole.png
	    :width: 40%

.. figure:: figures/polesfacet_quadrupole_3d.png
	    :width: 40%

.. figure:: figures/polesfacet_sextupole.png
	    :width: 40%

.. figure:: figures/polesfacet_sextupole_3d.png
	    :width: 40%


Poles Faceted with Crop - "`polesfacetcrop`"
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This magnet geometry is quite similar to `polesfacet`, but the yoke in between each
pole is cropped to form another facet. This results in this magnet geometry having
double the number of poles as sides.

`outerDiameter` is the full width horizontally as shown in the figure.

.. figure:: figures/polesfacetcrop_quadrupole.png
	    :width: 40%

.. figure:: figures/polesfacetcrop_quadrupole_3d.png
	    :width: 40%

.. figure:: figures/polesfacetcrop_sextupole.png
	    :width: 40%

.. figure:: figures/polesfacetcrop_sextupole_3d.png
	    :width: 40%


LHC Left & Right - "`lhcleft`" | "`lhcright`"
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. versionadded:: 0.7

`lhcleft` and `lhcright` provide more detailed magnet geometry appropriate for the LHC. Here, the
left and right suffixes refer to the shift of the magnet body with respect to the reference beam line.
Therefore, `lhcleft` has the magnet body shifted to the left in the direction of beam travel and the
'active' beam pipe is the right one. Vice versa for the `lhcright` geometry.

For this geometry, only the `sbend` and `quadrupole` have been implemented.  All other magnet geometry
defaults to the cylindrical set.

This geometry is parameterised to a degree regarding the beam pipe chosen.  Of course, parameters similar
to the LHC make most sense as does use of the `lhcdetailed` aperture type. Examples are shown with various
beam pipes and both `sbend` and `quadrupole` geometries.


.. |lhcleft_sbend| image:: figures/lhcleft_sbend.png
			   :width: 60%

.. |lhcleft_quadrupole| image:: figures/lhcleft_quadrupole.png
				:width: 60%

.. |lhcleft_quadrupole_square| image:: figures/lhcleft_quadrupole_square.png
				       :width: 60%

.. |lhcleft_sextupole| image:: figures/lhcleft_sextupole.png
			       :width: 60%

+-----------------------------+-----------------------+
| |lhcleft_sbend|             | |lhcleft_quadrupole|  |
+-----------------------------+-----------------------+
| |lhcleft_quadrupole_square| | |lhcleft_sextupole|   |
+-----------------------------+-----------------------+

.. _cavity-geometry-parameters:

Cavity Geometry Parameters
--------------------------

A more detailed rf cavity geometry may be described by constructing a 'cavity' object
in gmad and attaching it by name to an element.  The following parameters may be added
to a cavity object:

.. tabularcolumns:: |p{3cm}|p{2cm}|p{5cm}|

+--------------------------+-----------------+-----------------------------------------------------------------+
| **Parameter**            | **Required**    | **Description**                                                 |
+==========================+=================+=================================================================+
| `name`                   | yes             | Name of the object                                              |
+--------------------------+-----------------+-----------------------------------------------------------------+
| `type`                   | yes             | (elliptical | rectangular | pillbox)                            |
+--------------------------+-----------------+-----------------------------------------------------------------+
| `material`               | yes             | The material for the cavity.                                    |
+--------------------------+-----------------+-----------------------------------------------------------------+
| `irisRadius`             | no              | The radius of the narrowest part.                               |
+--------------------------+-----------------+-----------------------------------------------------------------+
| `equatorRadius`          | no              | The radius of the widest part.                                  |
+--------------------------+-----------------+-----------------------------------------------------------------+
| `halfCellLength`         | no              | Half length along a cell.                                       |
+--------------------------+-----------------+-----------------------------------------------------------------+
| `equatorHorizontalAxis`  | Elliptical only | Horizontal semi-axis of the ellipse at the cavity equator.      |
+--------------------------+-----------------+-----------------------------------------------------------------+
| `equatorVerticalAxis`    | Elliptical only | Vertical semi-axis of the ellipse at the cavity equator.        |
+--------------------------+-----------------+-----------------------------------------------------------------+
| `irisHorizontalAxis`     | Elliptical only | Horizontal semi-axis of the ellipse at the iris.                |
+--------------------------+-----------------+-----------------------------------------------------------------+
| `irisVerticalAxis`       | Elliptical only | Vertical semi-axis of the ellipse at the iris                   |
+--------------------------+-----------------+-----------------------------------------------------------------+
| `tangentLineAngle`       | Elliptical only | Angle to the vertical line connecting two ellipses.             |
+--------------------------+-----------------+-----------------------------------------------------------------+
| `thickness`              | no              | Thickness of material.                                          |
+--------------------------+-----------------+-----------------------------------------------------------------+
| `numberOfPoints`         | no              | Number of points to generate around 2 :math:`\pi`.              |
+--------------------------+-----------------+-----------------------------------------------------------------+
| `numberOfCells`          | no              | Number of cells to construct.                                   |
+--------------------------+-----------------+-----------------------------------------------------------------+

Example::

  shinyCavity: cavity, type="elliptical",
                       irisRadius = 35*mm,
	               equatorRadius = 103.3*mm,
	               halfCellLength = 57.7*mm,
		       equatorHorizontalAxis = 40*mm,
		       equatorVerticalAxis = 42*mm,
	               irisHorizontalAxis = 12*mm,
	               irisVerticalAxis = 19*mm,
	               tangentLineAngle = 13.3*pi/180,
	               thickness = 1*mm,
	               numberOfPoints = 24,
	               numberOfCells = 1;

.. figure:: figures/elliptical-cavity.pdf
	   :width: 40%
	   :align: center

The parametrisation used to define elliptical cavities in BDSIM.
The symbols used in the figure map to the cavity options according to the table below.

+-----------------------+-----------------------------+
| **Symbol**            | **BDSIM Cavity Parameter**  |
+=======================+=============================+
| :math:`R`             | equatorRadius               |
+-----------------------+-----------------------------+
| :math:`r`             | irisRadius                  |
+-----------------------+-----------------------------+
| :math:`A`             | equatorHorizontalAxis       |
+-----------------------+-----------------------------+
| :math:`B`             | equatorVerticalAxis         |
+-----------------------+-----------------------------+
| :math:`a`             | irisHorizontalAxis          |
+-----------------------+-----------------------------+
| :math:`b`             | irisVerticalAxis            |
+-----------------------+-----------------------------+
| :math:`\alpha`        | tangentLineAngle            |
+-----------------------+-----------------------------+
| :math:`L`             | halfCellLength              |
+-----------------------+-----------------------------+


.. _field-maps:

Fields
------

BDSIM provides the facility to overlay a pure magnetic, pure electric or combined electro-magnetic fields
on an element as defined by an externally provided field map. This can be done for only the vacuum
volume; only the volume outside the vacuum (ie the yoke); each separately; or one full map for the whole
element.  BDSIM allows any Geant4 integrator to be used to calculate the motion of the particle, which
can be chosen given knowledge of the smoothness of the field or the application. BDSIM also provides
a selection of 1-4D interpolators that are used to provide the field value inbetween the data points
in the supplied field map.

To overlay a field, one must define a field 'object' in the parser and then 'attach' it to an element.

* Magnetic and electric field maps are specified in separate files and may have different interpolators.
* Fields may have up to 4 dimensions.

Currently, the dimensions are in order :math:`x,y,z,t`. For example, specifying a 3D field, will only be
:math:`x,y,z` and cannot currently be used for :math:`x,y,t` field maps for example. The functionality
for dimensional flexibility can be added if required (see :ref:`feature-request`).

.. Note:: Currently only **regular** (evenly spaced) grids are supported with field maps. It would
	  require significant development to extend this to irregular grids. It's strongly
	  recommended the user resample any existing field map into a regular grid.

Here is example syntax to define a field object named 'somefield' in the parser and overlay it onto
a drift pipe where it covers the full volume of the drift (not outside it though)::

  somefield: field, type="ebmap2d",
		    eScaling = 3.0,
		    bScaling = 0.4,
		    integrator = "g4classicalrk4",
		    magneticFile = "poisson2d:/Path/To/File.TXT",
		    magneticInterpolator = "nearest2D",
		    electricFile = "poisson2d:/Another/File.TX",
		    electricInterpolator = "linear2D";

  d1: drift, l=0.5*m, aper1=4*cm, fieldAll="somefield";

  
When defining a field, the following parameters can be specified.

+----------------------+-----------------------------------------------------------------+
| **Parameter**        | **Description**                                                 |
+======================+=================================================================+
| type                 | See type table below.                                           |
+----------------------+-----------------------------------------------------------------+
| eScaling             | A numerical scaling factor that all electric field vectors      |
|                      | amplitudes will be multiplied by.                               |
+----------------------+-----------------------------------------------------------------+
| bScaling             | A numerical scaling factor that all magnetic field vectors      |
|                      | amplitudes will be multiplied by.                               |
+----------------------+-----------------------------------------------------------------+
| integrator           | The integrator used to calculate the motion of the particle     |
|                      | in the field. See below for full list of supported integrators. |
+----------------------+-----------------------------------------------------------------+
| globalTransform      | boolean. Whether a transform from local curvilinear coordinates |
|                      | to global coordinates should be provided (default true).        |
+----------------------+-----------------------------------------------------------------+
| magneticFile         | "format:filePath" - see formats below .                         |
+----------------------+-----------------------------------------------------------------+
| magneticInterpolator | Which interpolator to use - see below for a full list.          |
+----------------------+-----------------------------------------------------------------+
| electricFile         | "format:filePath" - see formats below.                          |
+----------------------+-----------------------------------------------------------------+
| electricInterpolator | Which interpolator to use - see below for a full list.          |
+----------------------+-----------------------------------------------------------------+
| x                    | x offset from element it's attached to.                         |
+----------------------+-----------------------------------------------------------------+
| y                    | y offset from element it's attached to.                         |
+----------------------+-----------------------------------------------------------------+
| z                    | z offset from element it's attached to.                         |
+----------------------+-----------------------------------------------------------------+
| t                    | t offset from **Global** t in seconds.                          |
+----------------------+-----------------------------------------------------------------+
| phi                  | Euler phi rotation from the element the field is attached to.   |
+----------------------+-----------------------------------------------------------------+
| theta                | Euler theta rotation from the element the field is attached to. |
+----------------------+-----------------------------------------------------------------+
| psi                  | Euler psi rotation from the element the field is attached to.   |
+----------------------+-----------------------------------------------------------------+
| axisX                | x component of axis defining axis / angle rotation.             |
+----------------------+-----------------------------------------------------------------+
| axisY                | y component of axis defining axis / angle rotation.             |
+----------------------+-----------------------------------------------------------------+
| axisZ                | z component of axis defining axis / angle rotation.             |
+----------------------+-----------------------------------------------------------------+
| angle                | angle (rad) of defining axis / angle rotation.                  |
+----------------------+-----------------------------------------------------------------+

.. Note:: Either axis angle (with unit axis 3-vector) or Euler angles can be used to provide
	  the rotation between the element the field maps is attached to and the coordinates
	  of the field map.

.. Note:: A right handed coordinate system is used in Geant4, so +ve x is out of a ring.

Field Types
^^^^^^^^^^^

* These are not case sensitive.

+------------------+----------------------------------+
| **Type String**  | **Description**                  |
+==================+==================================+
| bmap1d           | 1D magnetic only field map.      |
+------------------+----------------------------------+
| bmap2d           | 2D magnetic only field map.      |
+------------------+----------------------------------+
| bmap3d           | 3D magnetic only field map.      |
+------------------+----------------------------------+
| bmap4d           | 4D magnetic only field map.      |
+------------------+----------------------------------+
| emap1d           | 1D electric only field map.      |
+------------------+----------------------------------+
| emap2d           | 2D electric only field map.      |
+------------------+----------------------------------+
| emap3d           | 3D electric only field map.      |
+------------------+----------------------------------+
| emap4d           | 4D electric only field map.      |
+------------------+----------------------------------+
| ebmap1d          | 1D electric-magnetic field map.  |
+------------------+----------------------------------+
| ebmap2d          | 2D electric-magnetic field map.  |
+------------------+----------------------------------+
| ebmap3d          | 3D electric-magnetic field map.  |
+------------------+----------------------------------+
| ebmap4d          | 4D electric-magnetic field map.  |
+------------------+----------------------------------+


Formats
^^^^^^^

+------------------+--------------------------------------------+
| **Format**       | **Description**                            |
+==================+============================================+
| bdsim1d          | 1D BDSIM format file. (Units :math:`cm,s`) |
+------------------+--------------------------------------------+
| bdsim2d          | 2D BDSIM format file. (Units :math:`cm,s`) |
+------------------+--------------------------------------------+
| bdsim3d          | 3D BDSIM format file. (Units :math:`cm,s`) |
+------------------+--------------------------------------------+
| bdsim4d          | 4D BDSIM format file. (Units :math:`cm,s`) |
+------------------+--------------------------------------------+
| poisson2d        | 2D Poisson Superfish SF7 file.             |
+------------------+--------------------------------------------+
| poisson2dquad    | 2D Poisson Superfish SF7 file              |
|                  | for 1/8th of quadrupole.                   |
+------------------+--------------------------------------------+
| poisson2ddipole  | 2D Poisson Superfish SF7 file for positive |
|                  | quadrant that's reflected to produce a     |
|                  | full windowed dipole field.                |
+------------------+--------------------------------------------+

Field maps in the following formats are accepted:

  * BDSIM's own format (both uncompressed :code:`.dat` and gzip compressed :code:`.tar.gz`)
  * Superfish Poisson 2D SF7

These are described in detail below. More field formats can be added
relatively easily - see :ref:`feature-request`. A detailed description
of the formats is given in :ref:`field-map-formats`. A preparation guide
for BDSIM format files is provided here :ref:`field-map-file-preparation`.


Integrators
^^^^^^^^^^^

The following integrators are provided.  The majority are interfaces to Geant4 ones.
*g4classicalrk4* is typically the recommended default and is very robust.
*g4cakskarprkf45* is similar but slightly less CPU-intensive.

+----------------------+----------+------------------+
|  **String**          | **B/EM** | **Time Varying** |
+======================+==========+==================+
| g4cashkarprkf45      | EM       | Y                |
+----------------------+----------+------------------+
| g4classicalrk4       | EM       | Y                |
+----------------------+----------+------------------+
| g4constrk4           | B        | N                |
+----------------------+----------+------------------+
| g4expliciteuler      | EM       | Y                |
+----------------------+----------+------------------+
| g4impliciteuler      | EM       | Y                |
+----------------------+----------+------------------+
| g4simpleheum         | EM       | Y                |
+----------------------+----------+------------------+
| g4simplerunge        | EM       | Y                |
+----------------------+----------+------------------+
| g4exacthelixstepper  | B        | N                |
+----------------------+----------+------------------+
| g4helixexpliciteuler | B        | N                |
+----------------------+----------+------------------+
| g4helixheum          | B        | N                |
+----------------------+----------+------------------+
| g4heliximpliciteuler | B        | N                |
+----------------------+----------+------------------+
| g4helixmixedstepper  | B        | N                |
+----------------------+----------+------------------+
| g4helixsimplerunge   | B        | N                |
+----------------------+----------+------------------+
| g4nystromrk4         | B        | N                |
+----------------------+----------+------------------+
| g4rkg3stepper        | B        | N                |
+----------------------+----------+------------------+

Interpolators
^^^^^^^^^^^^^

There are many algorithms which one can use to interpolate the field map data. The field
may be queried at any point inside the volume, so an interpolator is required. A
mathematical description as well as example plots are shown in :ref:`field-interpolators`.

* This string is case-insensitive.

+------------+-------------------------------+
| **String** | **Description**               |
+============+===============================+
| nearest1d  | Nearest neighbour in 1D.      |
+------------+-------------------------------+
| nearest2d  | Nearest neighbour in 2D.      |
+------------+-------------------------------+
| nearest3d  | Nearest neighbour in 3D.      |
+------------+-------------------------------+
| nearest4d  | Nearest neighbour in 4D.      |
+------------+-------------------------------+
| linear1d   | Linear interpolation in 1D.   |
+------------+-------------------------------+
| linear2d   | Linear interpolation in 2D.   |
+------------+-------------------------------+
| linear3d   | Linear interpolation in 3D.   |
+------------+-------------------------------+
| linear4d   | Linear interpolation in 4D.   |
+------------+-------------------------------+
| cubic1d    | Cubic interpolation in 1D.    |
+------------+-------------------------------+
| cubic2d    | Cubic interpolation in 2D.    |
+------------+-------------------------------+
| cubic3d    | Cubic interpolation in 3D.    |
+------------+-------------------------------+
| cubic4d    | Cubic interpolation in 4D.    |
+------------+-------------------------------+

.. _externally-provided-geometry:

Externally Provided Geometry
----------------------------

BDSIM provides the ability to use externally provided geometry in the Geant4 model constructed
by BDSIM. A variety of formats are supported (see :ref:`geometry-formats`). External
geometry can be used in three ways:

1) A placement of a piece of geometry unrelated to the beam line.
2) Wrapped around the beam pipe in a BDSIM magnet element.
3) As a general element in the beam line where the geometry constitutes the whole object.

These are discussed in order.

.. _placements:

Placements
^^^^^^^^^^

Geometry provided in an external file, may be placed in 3D geometry at any location with
any rotation, however the user is responsible for ensuring that the geometry does not
overlap with any other geometry.

.. Note:: If the geometry overlaps, tracking faults may occur from Geant4 as well as
	  incorrect results and there may not always be warnings provided. For this reason
	  BDSIM will **always** use the Geant4 overlap checker when placing external geometry
	  into the world volume. This only ensures the container doesn't overlap with BDSIM
	  geometry, not that the internal geometry is valid.

The following parameters may be specified.

+----------------+--------------------------------------------------------------------+
| **Parameter**  |  **Description**                                                   |
+----------------+--------------------------------------------------------------------+
| geometryFile   | :code:`format:file` - which geometry format and file to use.       |
+----------------+--------------------------------------------------------------------+
| x              | Offset in global x.                                                |
+----------------+--------------------------------------------------------------------+
| y              | Offset in global y.                                                |
+----------------+--------------------------------------------------------------------+
| z              | Offset in global z.                                                |
+----------------+--------------------------------------------------------------------+
| phi            | Euler angle phi for rotation.                                      |
+----------------+--------------------------------------------------------------------+
| theta          | Euler angle theta for rotation.                                    |
+----------------+--------------------------------------------------------------------+
| psi            | Euler angle psi for rotation.                                      |
+----------------+--------------------------------------------------------------------+
| axisX          | Axis angle rotation x component of unit vector.                    |
+----------------+--------------------------------------------------------------------+
| axisY          | Axis angle rotation y component of unit vector.                    |
+----------------+--------------------------------------------------------------------+
| axisZ          | Axis angle rotation z component of unit vector.                    |
+----------------+--------------------------------------------------------------------+
| angle          | Axis angle angle to rotate about unit vector.                      |
+----------------+--------------------------------------------------------------------+
| axisAngle      | Boolean whether to use axis angle rotation scheme (default false). |
+----------------+--------------------------------------------------------------------+
| sensitive      | Whether the geometry records energy deposition (default true).     |
+----------------+--------------------------------------------------------------------+

* The file path provided in :code:`geometryFile` should either be relative to where bdsim
  is executed from or an absolute path.
* The transform is relative to the world coordinate system and not the beginning of the
  beam line. The main beam line begins at (0,0,0) by default but may be offset.  See
  :ref:`beamline-offset` for more details.

  
Two styles of rotation can be used. Either a set of 3 Euler angles or the axis angle
rotation scheme where a **unit** vector is provided in :math:`x,y,z` and an angle to
rotate about that. These variables are used to construct a :code:`G4RotationMatrix`
directly, which is also the same as a :code:`CLHEP::HepRotation`.

.. Note:: Geant4 uses a right-handed coordinate system and :math:`m` and :math:`rad` are
	  the default units for offsets and angles in BDSIM.

The following is an example syntax is used to place a piece of geometry::

  leadblock: placement, x = 10*m,
                        y = 3*cm,
			z = 12*m,
			geometryFile="gdml:mygeometry/detector.gdml";



External Magnet Geometry
^^^^^^^^^^^^^^^^^^^^^^^^

A geometry file may be placed around a beam pipe inside a BDSIM magnet instance. The beam pipe
will be constructed as normal and will use the appropriate BDSIM tracking routines, but the
yoke geometry will be loaded from the file provided. The external geometry must have a cut out
in its container volume for the beam pipe to fit. Ie, both the beam pipe and the yoke exist
at the same level in the geometry hierarchy (both are placed in one container for the magnet).
The beam pipe is not placed 'inside' the yoke.

This will work for `solenoid`, `sbend`, `rbend`, `quadrupole`, `sextupole`, `octupole`,
`decapole`, `multipole`, `muonspoiler`, `vkicker`, `hkicker` element types in BDSIM.

Example::

  q1: quadrupole, l=20*cm, k1=0.0235, magnetGeometryType="gdml:mygeometry/atf2quad.gdml";


Element
^^^^^^^

A general piece of geometry may be placed in the beam line along with any externally provided
field map using the `element` beam line element.  See `element`_.

.. _geometry-formats:

Geometry Formats
^^^^^^^^^^^^^^^^

The following geometry formats are supported. More may be added in collaboration with the BDSIM
developers - please see :ref:`feature-request`. The syntax and preparation of these geometry
formats is described in more detail in :ref:`external-geometry-formats`.

+----------------------+---------------------------------------------------------------------+
| **Format String**    | **Description**                                                     |
+======================+=====================================================================+
| gdml                 | | Geometry Description Markup Language - Geant4's official geometry |
|                      | | persistency format - recommended.                                 |
+----------------------+---------------------------------------------------------------------+
| ggmad                | | Simple text interface provided by BDSIM to some simple Geant4     |
|                      | | geometry classes.                                                 |
+----------------------+---------------------------------------------------------------------+
| mokka                | | An SQL style description of geometry.                             |
+----------------------+---------------------------------------------------------------------+

.. Note:: BDSIM must be compiled with the GDML build option in CMake turned on for gdml loading to work.


.. _offsets-and-tilts:

Offsets & Tilts - Component Misalignment
----------------------------------------

To simulate a real accelerator it may be necessary to introduce measured placement offsets or misalignments
and rotations. Every component can be displaced transversely and rotated along the axis of the beam propagation.

.. note:: Components that have a finite angle (rbend and sbend) will only respond to tilt and not vertical or
	  horizontal offsets. This is because these would change the length of the bend about its central axis.
	  This is not currently handled but may be implemented in future releases.

.. note:: A tilt on a component with a finite angle causes the axis the angle is induced in (typically the y
	  axis) to be rotated without rotating the reference frame of the beam. Ie a dipole with a :math:`\pi/2`
	  will become a vertical bend without flipping x and y in the sampler or subsequent components. This
	  matches the behaviour of MAD8 and MADX.

.. note:: A right-handed coordinate system is used and the beamline built along the `z` direction.
	  
The misalignments can be controlled through the following parameters

+--------------+------------------------------------------------------------------------------------+
| Parameter    | Default value                                                                      | 
+==============+====================================================================================+
| `offsetX`    | Horizontal displacement of the component [m].                                      |
+--------------+------------------------------------------------------------------------------------+
| `offsetY`    | Vertical displacement of the component [m].                                        |
+--------------+------------------------------------------------------------------------------------+
| `tilt`       | Rotation of component clockwise facing in the direction of the beamline `z` [rad]. |
|              | In the case of an rbend or sbend, this rotates the axis about which the beam bends |
+--------------+------------------------------------------------------------------------------------+

Examples::

  d1: drift, l=1*m, offsetX=1*cm;
  d2: drift, l=0.5*m, offsetY = 0.3*cm, tilt=0.003;

.. _lattice-sequence:

Lattice Sequence
----------------

Once all the necessary components have been defined, they must be placed in a sequence to make
a lattice. Elements can be repeated. A sequence of elements is defined by
a `line`_. Lines of lines can be made to describe the accelerator sequence programmatically i.e.
::

   d1: drift, l=3*m;
   q1: quadrupole, l=0.1*m, k1=0.684;
   q2: quadrupole, l=0.1*m, k1=-0.684;
   fodo: line = (q1,d1,q2,d1);
   transportline: line(fodo, fodo, fodo, fodo);
   

line
^^^^

`line` defines a sequence of elements. ::

  name: line=(element1, element2, element3, ... );

where `element` can be any element or line. Lines can also be reversed using ::
  
  line_name : line=-(line_2)

or within another line by::

  line=(line_1,-line_2)

Reversing a line also reverses all nested lines within.

.. _the-use-command:

use - Defining which Line to Use
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Once all elements and at least one `line` is defined, the main sequence of the
beam line can be defined. This must be defined using the following syntax::

  use, period=<line_name>

Examples::
   
   d1: drift, l=3.2*m;
   q1: quadrupole, l=20*cm, k1=4.5;
   q2: quadrupole, l=20*cm, k1=-4.5;
   fodo: line=(d1,q1,d1,q2,d1);
   use, period=fodo;

The beam line is placed in the world volume (the outermost coordinate system) starting
at position (0,0,0) with direction (0,0,1) - i.e. pointing in positive `z`. The user
may specify an initial offset and rotation for the baem line with respect to the world
volume using the options described in :ref:`beamline-offset`.

Multiple beam lines may also be visualised - but only visualised (not suitable for
simulations currently).  Details are provided in :ref:`multiple-beamlines`.


.. _sampler-output:
   
Samplers - Output
-----------------

BDSIM provides a `sampler` as a means to observe the particle distribution at a
point in the lattice. A sampler is 'attached' to an already defined element
and records all the particles passing through a plane at the **exit** face of
that element. They are defined using the following syntax::

  sample, range=<element_name>;

where `element_name` is the name of the element you wish to sample. Depending on the
output format chosen, the element name may be recorded in the output ('rootevent' output only).

.. note:: Samplers **can only** be defined **after** the main sequence has been defined
	  using the `use` command (see `use - Defining which Line to Use`_). Failure to do
	  so will result in an error and BDSIM will exit.

.. note:: Samplers record **all** particles impinging on them - i.e. both forwards and
	  backwards. Even secondary particles that may originate from further along the
	  lattice. They have no material so they do not absorb or affect particles, only
	  witness them.

To place a sampler before an item, attach it to the previous item. If however,
you wish to record the coordinates with another name rather than the name of the
element before, you can define a marker; place it in the sequence; and then define
a sampler that uses that marker::

  d1: drift, l=2.4*m;
  d2: drift, l=1*m;
  interestingplane: marker;
  l1: line=(d1,d1,interestingplane,d2,d1);
  use,period=l1;

  sample, range = interestingplane;

When an element is defined multiple times in the line (such as "d1" in the above example),
samplers will be attached to all instances. If you wish to sample only one specific
instance, the following syntax can be used::

  sample, range=<element_name>[index];

To attach samplers after all elements::

  sample, all;

And to attach samplers after all elements of a specific type::

  sample, <type>;

e.g.::

  sample, quadrupole;
  
.. note:: If a sampler is placed at the very beginning of the lattice, it may appear
	  that approximately half of the primary particles seem to pass through it. This
	  is the correct behaviour as unlike an optics program such as MADX, the sampler
	  represents a thin plane in 3D space in BDSIM. If the beam distribution has some
	  finite extent in *z* or *t*, particles may start beyond this first sampler and
	  never pass through it.

Sampler Dimensions
^^^^^^^^^^^^^^^^^^

The sampler is represented by a cube solid that is 1 pm thin along z and 5m metres wide
transversely in x and y. If a smaller or larger capture area for the samplers is required,
the option *samplerDiameter* may be specified in the input gmad.::

  option, samplerDiameter=3*m;

This affects all samplers.

.. note:: For a very low energy lattice with large angle bends, the default samplerDiameter
	  may cause geometrical overlap warnings from Geant4. This situation is difficult to
	  avoid automatically, but easy to remedy by setting the samplerDiameter to a lower
	  value.

Sampler Visualisation
^^^^^^^^^^^^^^^^^^^^^

The samplers are normally invisible and are built in a parallel world geometry in Geant4. To
visualised them, the following command should be used in the visualiser::

  /vis/drawVolume worlds

The samplers will appear in semi-transparent green as well as the curvilinear geometry used
for coordinate transforms (cylinders).


Physics Processes
-----------------

BDSIM can exploit all the physics processes that come with Geant4. As with any Geant4 program
and simulation it is very useful to define the physical processes that should be simulated so
that the simulation is both relevant and efficient. By default, only tracking in magnetic fields
is provided and other processes must be specified to be used. Rather than specify each individual
particle physics process on a per-particle basis, a series of "physics lists" are provided that
are a predetermined set of physics processes suitable for a certain applications. BDSIM follows
the Geant4 ethos in this regard and the majority of those in BDSIM are simple shortcuts to the
Geant4 ones.

The physics list can be selected with the following syntax (delimited by a space)::

  option, physicsList = "physicslistname anotherphysicslistname";

  option, physicsList = "em optical";

For general high energy hadron physics we recommend::

  option, physicsList = "em ftfp_bert decay muon hadronic_elastic em_extra"

.. note:: Using extra physics processes that are not required will slow the simulation and produce
	  many orders of magnitude more particles, which in turn slow the simulation further. Therefore,
	  only use the minimal set of physics processes required.
  
.. note:: The strings for the physics list are case-insensitive.

.. versionadded:: 0.92
		  The physics lists changed from BDSIM produced physics lists to using the Geant4
		  modular physics lists in version 0.92. This also introduced the space-delimited syntax
		  slight changes to the physics list names.

  
A summary of the available physics lists in BDSIM is provided below (Others can be easily added
by contacting the developers - see :ref:`feature-request`).

BDSIM uses the Geant4 physics lists directly and more details can be found in the Geant4 documentation:

   * `Reference Physics Lists <http://geant4.cern.ch/support/proc_mod_catalog/physics_lists/referencePL.shtml>`_
   * `Physics Reference Manual <http://geant4.web.cern.ch/geant4/UserDocumentation/UsersGuides/PhysicsReferenceManual/fo/PhysicsReferenceManual.pdf>`_
   * `Use Cases <http://geant4.cern.ch/support/proc_mod_catalog/physics_lists/useCases.shtml>`_

Physics Lists In BDSIM
^^^^^^^^^^^^^^^^^^^^^^

.. tabularcolumns:: |p{5cm}|p{10cm}|

+---------------------------+------------------------------------------------------------------------+
| **String to use**         | **Description**                                                        |
+---------------------------+------------------------------------------------------------------------+
|                           | Transportation of primary particles only - no scattering in material.  |
+---------------------------+------------------------------------------------------------------------+
| cherenkov                 | Provides Cherenkov radiation for all charged particles. Provided by    |
|                           | BDSIM physics builder `BDSPhysicsCherenkov` that provides the process  |
|                           | `G4CherenkovProcess`.                                                  |
+---------------------------+------------------------------------------------------------------------+
| decay                     | Provides radioactive decay processes using `G4DecayPhysics`.           |
+---------------------------+------------------------------------------------------------------------+
| em                        | Transportation of primary particles, ionisation, bremsstrahlung,       |
|                           | Cerenkov, multiple scattering. Uses `G4EmStandardPhysics`.             |
+---------------------------+------------------------------------------------------------------------+
| em_extra                  | This provides extra electromagnetic models including, muon nuclear     |
|                           | processes, bertini electro-nuclear model and synchrotron radiation     |
|                           | (not in material). Provided by `G4EmPhysicsExtra`.                     |
+---------------------------+------------------------------------------------------------------------+
| em_low                    | The same as `em` but using low energy electromagnetic models. Uses     |
|                           | `G4EmPenelopePhysics`.                                                 |
+---------------------------+------------------------------------------------------------------------+
| em_penelope               | The same as `em` but using low energy electromagnetic models. Uses     |
|                           | `G4EmPenelopePhysics`.                                                 |
+---------------------------+------------------------------------------------------------------------+
| em_livermore              | `G4EmLivermorePhysics`.                                                |
+---------------------------+------------------------------------------------------------------------+
| em_livermore_polarised    | `G4EmLivermorePolarizedPhysics`.                                       |
+---------------------------+------------------------------------------------------------------------+
| em_low_ep                 | `G4EmLowEPPhysics`.                                                    |
+---------------------------+------------------------------------------------------------------------+
| em_gs                     | `G4EmStandardPhysicsGS`.                                               |
+---------------------------+------------------------------------------------------------------------+
| em_ss                     | `G4EmStandardPhysicsSS`.                                               |
+---------------------------+------------------------------------------------------------------------+
| em_wvi                    | `G4EmStandardPhysicsWVI`.                                              |
+---------------------------+------------------------------------------------------------------------+
| em_1                      | `G4EmStandardPhysics_option1`.                                         |
+---------------------------+------------------------------------------------------------------------+
| em_2                      | `G4EmStandardPhysics_option2`.                                         |
+---------------------------+------------------------------------------------------------------------+
| em_3                      | `G4EmStandardPhysics_option3`.                                         |
+---------------------------+------------------------------------------------------------------------+
| em_4                      | `G4EmStandardPhysics_option4`.                                         |
+---------------------------+------------------------------------------------------------------------+
| ftfp_bert                 | Fritiof Precompound Model with Bertini Cascade Model. The FTF model    |
|                           | is based on the FRITIOF description of string excitation and           |
|                           | fragmentation. This is provided by `G4HadronPhysicsFTFP_BERT`. All     |
|                           | FTF physics lists require `G4HadronElasticPhysics` to work correctly.  |
+---------------------------+------------------------------------------------------------------------+
| ftfp_bert_hp              | Similar to `FTFP_BERT` but with the high precision neutron package.    |
|                           | This is provided by `G4HadronPhysicsFTFP_BERT_HP`.                     |
+---------------------------+------------------------------------------------------------------------+
| hadronic_elastic          | Elastic hadronic processes. This is provided by                        |
|                           | `G4HadronElasticPhysics.`                                              |
+---------------------------+------------------------------------------------------------------------+
| hadronic                  | A shortcut for `QGSP_BERT`.                                            |
+---------------------------+------------------------------------------------------------------------+
| hadronic_hp               | A shortcut for `QGSP_BERT_HP`.                                         |
+---------------------------+------------------------------------------------------------------------+
| ion                       | A shortcut for `G4IonPhysics`.                                         |
+---------------------------+------------------------------------------------------------------------+
| ionphp (*)                | A shortcut for `G4IonPhysicsPHP`.                                      |
+---------------------------+------------------------------------------------------------------------+
| ioninclxx (*)             | A shortcut for `G4IonINCLXXPhysics`.                                   |
+---------------------------+------------------------------------------------------------------------+
| ionbinary (*)             | A shortcut for `G4IonBinaryCascadePhysics`.                            |
+---------------------------+------------------------------------------------------------------------+
| muon                      | Provides muon production and scattering processes. Gamma to muons,     |
|                           | annihilation to muon pair, 'ee' to hadrons, pion decay to muons,       |
|                           | multiple scattering for muons, muon bremsstrahlung, pair production    |
|                           | and Cherenkov light are all provided. Provided by BDSIM physics        |
|                           | builder (a la Geant4) `BDSPhysicsMuon`.                                |
+---------------------------+------------------------------------------------------------------------+
| optical                   | Optical physics processes including absorption, Rayleigh scattering,   |
|                           | Mie scattering, optical boundary processes, scintillation, cherenkov.  |
|                           | This uses `G4OpticalPhysics` class.                                    |
+---------------------------+------------------------------------------------------------------------+
| qgsp_bert                 | Quark-Gluon String Precompound Model with Bertini Cascade model.       |
|                           | This is based on `G4HadronPhysicsQGSP_BERT` class and includes         |
|                           | hadronic elastic and inelastic processes. Suitable for high energy     |
|                           | (>10 GeV).                                                             |
+---------------------------+------------------------------------------------------------------------+
| qgsp_bert_hp              | Similar to `QGSP_BERT` but with the addition of data driven high       |
|                           | precision neutron models to transport neutrons below 20 MeV down to    |
|                           | thermal energies.  This is provided by `G4HadronPhysicsQGSP_BERT_HP`.  |
+---------------------------+------------------------------------------------------------------------+
| qgsp_bic                  | Like `QGSP`, but using Geant4 Binary cascade for primary protons and   |
|                           | neutrons with energies below ~10GeV, thus replacing the use of the LEP |
|                           | model for protons and neutrons In comparison to the LEP model, Binary  |
|                           | cascade better describes production of secondary particles produced in |
|                           | interactions of protons and neutrons with nuclei. This is provided by  |
|                           | `G4HadronPhysicsQGSP_BIC`.                                             |
+---------------------------+------------------------------------------------------------------------+
| qgsp_bic_hp               | Similar to `QGSP_BIC` but with the high precision neutron package.     |
|                           | This is provided by `G4HadronPhysicsQGSP_BIC_HP`.                      |
+---------------------------+------------------------------------------------------------------------+
| spindecay                 | Decay physics but with spin correctly implemented. Note, only the      |
|                           | Geant4 tracking integrators track spin correctly.                      |
+---------------------------+------------------------------------------------------------------------+
| synchrad                  | Provides synchrotron radiation for all charged particles. Provided by  |
|                           | BDSIM physics builder `BDSPhysicsSynchRad` that provides the process   |
|                           | `G4SynchrotronRadiation`.                                              |
+---------------------------+------------------------------------------------------------------------+


.. warning:: (*) These physics lists require the optional low energy data from Geant4. The user should
	     download this data from the Geant4 website and install it (for example: extract to
	     <install-dir>/share/Geant4-10.3.3/data/ beside the other data) and export the environmental
	     variable `G4PARTICLEHPDATA` to point to this directory.


Physics Biasing
---------------

A physics biasing process can be defined with the keyword **xsecbias**.

.. note:: This only works with Geant4 version 10.1 or higher.

=================== ================================================
parameter           description               
name                biasing process name
particle            particle that will be biased
proc                process(es) to be biased
flag                flag which particles are biased for the process(es)
                    (1=all, 2=primaries, 3=secondaries)
xsecfact            biasing factor(s) for the process(es)
=================== ================================================

Example::

  biasDef1: xsecBias, particle="e-", proc="all", xsecfact=10, flag=3;
  biasDef2: xsecBias, particle="e+", proc="eBrem eIoni msc", xsecfact={10,1,5}, flag={1,1,2};

The process can also be attached to a specific element using the keywords `biasVacuum` or
`biasMaterial` for the biasing to be attached the vacuum volume or everything outside the
vacuum respectively::

  q1: quadrupole, l=1*m, material="Iron", biasVacuum="biasDef1 biasDef2"; ! uses the process biasDef1 and biasDef2
  q2: quadrupole, l=0.5*m, biasMaterial="biasDef2";

.. _bdsim-options:
  
Options
-------

Various simulation details can be controlled through the `option` command. Options are defined
using the following syntax::

  option, <option_name>=<value>;

If the value is a string and not a number, it should be enclosed in "double inverted commas".
Multiple options can be defined at once using the following syntax::

  option, <option1> = <value>,
          <option2> = <value>;

.. note:: No options are required to be specified to run a BDSIM model.  Defaults will be used in
	  all cases.  However, we do recommend you select an appropriate physics list and beam pipe
	  radius as these will have a large impact on the outcome of the simulation.

Options in BDSIM
^^^^^^^^^^^^^^^^ 

Below is a full list of all options in BDSIM. If the option is boolean, 1 (true) or 0 (false) can be used
as their value.

.. tabularcolumns:: |p{5cm}|p{10cm}|

+----------------------------------+-------------------------------------------------------+
| Option                           | Function                                              |
+==================================+=======================================================+
| **Common Parameters**            |                                                       |
+----------------------------------+-------------------------------------------------------+
| beampipeRadius                   | default beam pipe inner radius [m]                    |
+----------------------------------+-------------------------------------------------------+
| beampipeThickness                | default beam pipe thickness [m]                       |
+----------------------------------+-------------------------------------------------------+
| beampipeMaterial                 | default beam pipe material                            |
+----------------------------------+-------------------------------------------------------+
| circular                         | whether the accelerator is circular or not            |
+----------------------------------+-------------------------------------------------------+
| elossHistoBinWidth               | the width of the histogram bins [m]                   |
+----------------------------------+-------------------------------------------------------+
| eventNumberOffset                | event the recreation should start from                |
+----------------------------------+-------------------------------------------------------+
| killNeutrinos                    | whether to always stop tracking neutrinos for         |
|                                  | increased efficiency (default = true)                 |
+----------------------------------+-------------------------------------------------------+
| ngenerate                        | number of primary particles to simulate               |
+----------------------------------+-------------------------------------------------------+
| nturns                           | the number of revolutions particles are allowed to    |
|                                  | complete in a circular accelerator - requires         |
|                                  | --circular executable option to work                  |
+----------------------------------+-------------------------------------------------------+
| outerDiameter                    | default accelerator component full width [m]          |
+----------------------------------+-------------------------------------------------------+
| physicsList                      | the physics list to use                               |
+----------------------------------+-------------------------------------------------------+
| printModuloFraction              | the fraction of events to print out (default 0.1)     |
+----------------------------------+-------------------------------------------------------+
| recreate                         | whether to run in recreation mode (default 0)         |
+----------------------------------+-------------------------------------------------------+
| recreateFileName                 | which file to recreate events from                    |
+----------------------------------+-------------------------------------------------------+
| startFromEvent                   | event number offset to start from when recreating     |
+----------------------------------+-------------------------------------------------------+
| seed                             | the integer seed value for the random number          |
|                                  | generator                                             |
+----------------------------------+-------------------------------------------------------+
| seedStateFileName                | path to ASCII seed state to load - must be used with  |
|                                  | :code:`useASCIISeedState` to be effective             |
+----------------------------------+-------------------------------------------------------+
| stopSecondaries                  | whether to stop secondaries or not (default = false)  |
+----------------------------------+-------------------------------------------------------+
| stopTracks                       | whether to stop tracks after                          |
|                                  | interaction (default = false)                         |
+----------------------------------+-------------------------------------------------------+
| useASCIISeedState                | whether to load an ASCII seed state file using        |
|                                  | :code:`seedStateFileName`                             |
+----------------------------------+-------------------------------------------------------+
| writeSeedState                   | write the seed state of the last event start in ASCII |
+----------------------------------+-------------------------------------------------------+
| **Geometry Parameters**          |                                                       |
+----------------------------------+-------------------------------------------------------+
| aper1                            | default aper1 parameter                               |
+----------------------------------+-------------------------------------------------------+
| aper2                            | default aper2 parameter                               |
+----------------------------------+-------------------------------------------------------+
| aper3                            | default aper3 parameter                               |
+----------------------------------+-------------------------------------------------------+
| aper4                            | default aper4 parameter                               |
+----------------------------------+-------------------------------------------------------+
| dontSplitSBends                  | If true, do not split sbends into multiple segments   |
|                                  | (default = false)                                     |
+----------------------------------+-------------------------------------------------------+
| ignoreLocalAperture              | If this is true (1), any per-element aperture         |
|                                  | definitions will be ignored and the ones specified    |
|                                  | in options will be used.                              |
+----------------------------------+-------------------------------------------------------+
| checkOverlaps                    | Whether to run Geant4's geometry overlap checker      |
|                                  | during geometry construction (slower)                 |
+----------------------------------+-------------------------------------------------------+
| includeIronMagFields             | whether to include magnetic fields in the magnet      |
|                                  | poles                                                 |
+----------------------------------+-------------------------------------------------------+
| magnetGeometryType               | the default magnet geometry style to use              |
+----------------------------------+-------------------------------------------------------+
| outerDiameter                    | the default full width of a magnet                    |
+----------------------------------+-------------------------------------------------------+
| outerMaterial                    | the default material to use for the yoke of magnet    |
|                                  | geometry.                                             |
+----------------------------------+-------------------------------------------------------+
| samplerDiameter                  | diameter of samplers (default 5 m) [m]                |
+----------------------------------+-------------------------------------------------------+
| sensitiveBeamlineComponents      | whether all beam line components record energy loss   |
+----------------------------------+-------------------------------------------------------+
| sensitiveBeamPipe                | whether the beam pipe records energy loss             |
+----------------------------------+-------------------------------------------------------+
| vacuumMaterial                   | the material to use for the beam pipe vacuum          |
+----------------------------------+-------------------------------------------------------+
| vacuumPressure                   | the pressure of the vacuum gas [bar]                  |
+----------------------------------+-------------------------------------------------------+
| thinElementLength                | the length of all thinmultipoles and dipole           |
|                                  | fringefields in a lattice (default 1e-6) [m]          |
+----------------------------------+-------------------------------------------------------+
| **Tracking Parameters**          |                                                       |
+----------------------------------+-------------------------------------------------------+
| deltaChord                       | chord finder precision                                |
+----------------------------------+-------------------------------------------------------+
| deltaIntersection                | boundary intersection precision                       |
+----------------------------------+-------------------------------------------------------+
| chordStepMinimum                 | minimum step size                                     |
+----------------------------------+-------------------------------------------------------+
| includeFringeFields              | place thin fringefield elements on the end of bending |
|                                  | magnets with finite poleface angles. The length of the|
|                                  | total element is conserved. (default = false)         |
+----------------------------------+-------------------------------------------------------+
| integratorSet                    | set of tracking routines to use ("bdsim" or           |
|                                  | "bdsimtwo" or "geant4")                               |
+----------------------------------+-------------------------------------------------------+
| lengthSafety                     | element overlap safety (caution!)                     |
+----------------------------------+-------------------------------------------------------+
| maximumEpsilonStep               | maximum relative error acceptable in stepping         |
+----------------------------------+-------------------------------------------------------+
| maximumStepLength                | maximum step length (default = 20 m)                  |
+----------------------------------+-------------------------------------------------------+
| maximumTrackingTime              | the maximum time of flight allowed for any particle   |
|                                  | before it is killed                                   |
+----------------------------------+-------------------------------------------------------+
| maximumTrackLength               | the maximum length in metres of any track passing     |
|                                  | through any geometry in the model (not including the  |
|                                  | world volume).                                        |
+----------------------------------+-------------------------------------------------------+
| minimumEpsilonStep               | minimum relative error acceptable in stepping         |
+----------------------------------+-------------------------------------------------------+
| minimumRadiusOfCurvature         | minimum tolerable radius of curvature of a particle   |
|                                  | below which, the energy will be decreased by 2% on    |
|                                  | each use of the integrator to prevent infinite        |
|                                  | loops - should be just greater than width of beam     |
|                                  | pipe.                                                 |
+----------------------------------+-------------------------------------------------------+
| deltaOneStep                     | set position error acceptable in an integration step  |
+----------------------------------+-------------------------------------------------------+
| **Physics Processes Parameters** |                                                       |
+----------------------------------+-------------------------------------------------------+
| defaultBiasVacuum                | name of bias object to be attached to vacuum volumes  |
|                                  | by default                                            |
+----------------------------------+-------------------------------------------------------+
| defaultBiasMaterial              | name of bias object to be attached to general         |
|                                  | material of components outside the vacuum by default  |
+----------------------------------+-------------------------------------------------------+
| synchRadOn                       | whether to use synchrotron radiation processes        |
+----------------------------------+-------------------------------------------------------+
| turnOnCerenkov                   | whether to produce cerenkov radiation                 |
+----------------------------------+-------------------------------------------------------+
| defaultRangeCut                  | the default predicted range at which a particle is    |
|                                  | cut. Overwrites other production cuts unless these    |
|                                  | are explicitly set (default 1e-3) [m].                |
+----------------------------------+-------------------------------------------------------+
| prodCutPhotons                   | standard overall production cuts for photons          |
|                                  | (default 1e-3) [m].                                   |
+----------------------------------+-------------------------------------------------------+
| prodCutElectrons                 | standard overall production cuts for electrons        |
|                                  | (default 1e-3) [m].                                   |
+----------------------------------+-------------------------------------------------------+
| prodCutPositrons                 | standard overall production cuts for positrons        |
|                                  | (default 1e-3) [m].                                   |
+----------------------------------+-------------------------------------------------------+
| prodCutProtons                   | standard overall production cuts for protons          |
|                                  | (default 1e-3) [m].                                   |
+----------------------------------+-------------------------------------------------------+
| **Output Parameters**            |                                                       |
+----------------------------------+-------------------------------------------------------+
| storeTrajectories                | whether to store trajectories in the output           |
+----------------------------------+-------------------------------------------------------+
| storeTrajectoryDepth             | maximum depth (secondaries) of stored trajectories    |
+----------------------------------+-------------------------------------------------------+
| storeTrajectoryEnergyThreshold   | minimum energy of stored trajectories                 |
+----------------------------------+-------------------------------------------------------+
| storeTrajectoryParticle          | store trajectories of these particles                 |
+----------------------------------+-------------------------------------------------------+
| trajCutGTZ                       | global z position cut (minimum) for storing           |
|                                  | trajectories                                          |
+----------------------------------+-------------------------------------------------------+
| trajCutLTR                       | radius cut for storing trajectories (maximum)         |
+----------------------------------+-------------------------------------------------------+
| nperfile                         | number of events to record per output file            |
+----------------------------------+-------------------------------------------------------+
| **Visualisation Parameters**     |                                                       |
+----------------------------------+-------------------------------------------------------+
| nSegmentsPerCircle               | the number of facets per 2 :math:`\pi` in the         |
|                                  | visualiser. Note, this does not affect the accuracy   |
|                                  | of the geometry - only the visualisation (default =   |
|                                  | 50)                                                   |
+----------------------------------+-------------------------------------------------------+

* For **Tunnel** parameters, see, `Tunnel Geometry`_.

.. _beamline-offset:

Offset for Main Beam Line
^^^^^^^^^^^^^^^^^^^^^^^^^

The following options may be used to offset the main beam line with respect to the world
volume, which is the outermost coordinate system.

+----------------------+--------------------------------------------------------------------+
| Option               | Description                                                        |
+======================+====================================================================+
| beamlineX            | Offset in x.                                                       |
+----------------------+--------------------------------------------------------------------+
| beamlineY            | Offset in y.                                                       |
+----------------------+--------------------------------------------------------------------+
| beamlineZ            | Offset in z.                                                       |
+----------------------+--------------------------------------------------------------------+
| beamlinePhi          | Euler angle phi for rotation.                                      |
+----------------------+--------------------------------------------------------------------+
| beamlineTheta        | Euler angle theta for rotation.                                    |
+----------------------+--------------------------------------------------------------------+
| beamlinePsi          | Euler angle psi for rotation.                                      |
+----------------------+--------------------------------------------------------------------+
| beamlineAxisX        | Axis angle rotation x component of unit vector.                    |
+----------------------+--------------------------------------------------------------------+
| beamlineAxisY        | Axis angle rotation y component of unit vector.                    |
+----------------------+--------------------------------------------------------------------+
| beamlineAxisZ        | Axis angle rotation z component of unit vector.                    |
+----------------------+--------------------------------------------------------------------+
| beamlineAngle        | Axis angle angle to rotate about unit vector.                      |
+----------------------+--------------------------------------------------------------------+
| beamlineAxisAngle    | Boolean whether to use axis angle rotation scheme (default false). |
+----------------------+--------------------------------------------------------------------+

Two styles of rotation can be used. Either a set of 3 Euler angles or the axis angle
rotation scheme where a **unit** vector is provided in :math:`x,y,z` and an angle to
rotate about that. These variables are used to construct a :code:`G4RotationMatrix`
directly, which is also the same as a :code:`CLHEP::HepRotation`.

.. Note:: Geant4 uses a right-handed coordinate system and :math:`m` and :math:`rad` are
	  the default units for offsets and angles in BDSIM.

Example::

  option, beamlineX = 3*m,
          beamlineY = 20*cm,
	  beamlineZ = -30*m,
	  beamlineAxisAngle = 1,
	  beamlineAxisY = 1,
	  beamlineAngle = 0.2;

This offsets the beam line by (3,0.2,-30) m and rotated about the unit vector (0,1,0) (ie in the
horizontal plane - x,z) by 0.2 rad.


.. _beam-parameters:
  
Beam Parameters
---------------

To specify the input particle distribution to the accelerator model, the `beam` command is
used [#beamcommandnote]_. This also specifies the particle species and **reference energy**, which is the
design energy of the machine. This is used along with the particle species to calculate
the momentum of the reference particle and therefore the magnetic rigidity that magnetic
field strengths are calculated with respect to. For example, the field of dipole magnets
is calculated using this if only the `angle` parameter has been specified.

.. note:: The design energy is required to be specified, but the central energy, of say
	  a bunch with a Gaussian distribution, can be also be specified with `E0`.

.. note:: `energy` here is the total energy of the particle. This must be greater than
	  the rest mass of the particle.

The user **must** specify at least `energy` and the `particle` type. In this case the
`reference`_ distribution will be used as well as default parameters. The minimum
beam definitions is::

  beam, particle="proton",
        energy=34.2*GeV;

  
Other parameters, such
as the beam distribution type, `distrType`, are optional and can be specified as described
in the following sections. The beam is defined using the following syntax::

  beam, particle="proton",
        energy=4.0*TeV,
	distrType="reference";

Energy is the total energy in `GeV`. The beam particle may be one of the following:

* `e-` or `e+`
* `proton` or `antiproton`
* `gamma`
* `neutron`
* `mu-` or `mu+`
* `pi-` or `pi+`

The user may also specify any ion with the following syntax::

  beam, particle="ion A Z";

or::
  
  beam, particle="ion A Z Q";

where `A`, `Z` and `Q` should be replaced by the the atomic number, the number of protons
in the nucleus and the charge. The charge is optional and by default is Z (i.e. a fully
ionised ion). In this case, it is recommended to use the `ion` physicslist.

Available input distributions and their associated parameters are described in the following
section.

Beam Distributions
^^^^^^^^^^^^^^^^^^
The following beam distributions are available in BDSIM

- `reference`_
- `gaussmatrix`_
- `gauss`_
- `gausstwiss`_
- `circle`_
- `square`_
- `ring`_
- `eshell`_
- `halo`_
- `composite`_ 
- `userfile`_
- `ptc`_ 


reference
^^^^^^^^^
This is a single particle with the same position and angle defined by the following parameters. The
coordinates are the same for every particle fired using the reference distribution. It is therefore
not likely to be useful to generate a large number of repeated events with this distribution.

These parameters also act as **central** parameters for all other distributions. For example, a Gaussian
distribution may defined with the `gauss`_ parameters but `X0` set to offset the centroid of the
Gaussian with respect to the reference trajectory.

+----------------------------------+-------------------------------------------------------+----------+
| Option                           | Description                                           | Default  |
+==================================+=======================================================+==========+
| `X0`                             | Horizontal position [m]                               | 0        |
+----------------------------------+-------------------------------------------------------+----------+
| `Y0`                             | Vertical position [m]                                 | 0        |
+----------------------------------+-------------------------------------------------------+----------+
| `Z0`                             | Longitudinal position [m]                             | 0        |
+----------------------------------+-------------------------------------------------------+----------+
| `T0`                             | Longitudinal position [s]                             | 0        |
+----------------------------------+-------------------------------------------------------+----------+
| `Xp0`                            | Horizontal canonical momentum                         | 0        |
+----------------------------------+-------------------------------------------------------+----------+
| `Yp0`                            | Vertical canonical momentum                           | 0        |
+----------------------------------+-------------------------------------------------------+----------+
| `E0`                             | Central energy of bunch distribution (GeV)            | 'energy' |
+----------------------------------+-------------------------------------------------------+----------+

Examples::

  beam, particle = "e-",
        energy = 10*GeV,
	distrType = "reference";

Generates a beam with all coordinates 0 at the nominal energy.::

  beam, particle = "e-",
        energy = 10*GeV,
	distrType = "reference",
	X0 = 100*um,
	Y0 = 3.5*um;

Generate a particle with an offset of 100 :math:`\mu\mathrm{m}` horizontally and 3.5
:math:`\mu\mathrm{m}` vertically.

gaussmatrix
^^^^^^^^^^^

Uses the :math:`N` dimensional gaussian generator from `CLHEP`, `CLHEP::RandMultiGauss`. The generator
is initialised by a :math:`6\times1` means vector and :math:`6\times 6` sigma matrix.  

* All parameters from `reference`_ distribution as used as centroids.

+----------------------------------+-------------------------------------------------------+
| Option                           | Description                                           |
+==================================+=======================================================+
| `sigmaNM`                        | Sigma matrix element (N,M)                            |
+----------------------------------+-------------------------------------------------------+

Examples::

   beam, particle = "e-",
         energy = 10*GeV,
	 distrType = "gaussmatrix",
	 sigma11 = 100*um,
	 sigma22 = 3*um,
	 sigma33 = 50*um,
	 sigma44 = 1.4*um,
	 sigma55 = 1e-12,
	 sigma66 = 1e-4,
	 sigma12 = 1e-2,
	 sigma34 = 1.4e-3;


gauss
^^^^^

Uses the `gaussmatrix`_ beam generator but with simplified input parameters opposed to a complete 
beam sigma matrix. This beam distribution has a diagonal :math:`\sigma`-matrix and does not allow for 
correlations between phase space coordinates, so 

.. math:: 
   \sigma_{11} & =  \sigma_x^2   \\
   \sigma_{22} & =  \sigma_x^{\prime 2}  \\
   \sigma_{33} & =  \sigma_y^2   \\
   \sigma_{44} & =  \sigma_y^{\prime 2}  \\    
   \sigma_{55} & =  \sigma_{T}^2 \\  
   \sigma_{66} & =  \sigma_{E}^2.

* All parameters from `reference`_ distribution as used as centroids.

+----------------------------------+-------------------------------------------------------+
| Option                           | Description                                           |
+==================================+=======================================================+
| `sigmaX`                         | Horizontal gaussian sigma [m]                         |
+----------------------------------+-------------------------------------------------------+
| `sigmaY`                         | Vertical gaussian sigma [m]                           |
+----------------------------------+-------------------------------------------------------+
| `sigmaXp`                        | Sigma of the horizontal canonical momentum            |
+----------------------------------+-------------------------------------------------------+
| `sigmaYp`                        | Sigma of the vertical canonical momentum              |
+----------------------------------+-------------------------------------------------------+
| `sigmaE`                         | Relative energy spread                                |
+----------------------------------+-------------------------------------------------------+
| `sigmaT`                         | Sigma of the temporal distribution [s]                |
+----------------------------------+-------------------------------------------------------+


gausstwiss
^^^^^^^^^^

The beam parameters are defined by the usual :math:`\alpha`, :math:`\beta` and :math:`\gamma` from which
the usual beam :math:`\sigma`-matrix is calculated, using the following equations 

.. math:: 
   \sigma_{11} & =  \epsilon_x \beta_x  \\
   \sigma_{12} & = -\epsilon_x \alpha_x \\  
   \sigma_{21} & = -\epsilon_x \alpha_x \\
   \sigma_{22} & =  \epsilon_x \gamma_x \\
   \sigma_{33} & =  \epsilon_y \beta_y \\
   \sigma_{34} & = -\epsilon_y \alpha_y \\ 
   \sigma_{43} & = -\epsilon_y \alpha_y \\
   \sigma_{44} & =  \epsilon_y \gamma_y \\    
   \sigma_{55} & =  \sigma_{T}^2 \\  
   \sigma_{66} & =  \sigma_{E}^2  

* All parameters from `reference`_ distribution as used as centroids.
   
+----------------------------------+-------------------------------------------------------+
| Option                           | Description                                           |
+==================================+=======================================================+
| `emitx`                          | Horizontal beam core emittance [m]                    |
+----------------------------------+-------------------------------------------------------+
| `emity`                          | Vertical beam core emittance [m]                      |
+----------------------------------+-------------------------------------------------------+
| `betx`                           | Horizontal beta function [m]                          |
+----------------------------------+-------------------------------------------------------+
| `bety`                           | Vertical beta function [m]                            |
+----------------------------------+-------------------------------------------------------+
| `alfx`                           | Horizontal alpha function                             |
+----------------------------------+-------------------------------------------------------+
| `alfy`                           | Vertical alpha function                               |
+----------------------------------+-------------------------------------------------------+

circle
^^^^^^

Beam of randomly distributed particles with a uniform distribution within a circle in each
dimension dimension of phase space - `x` & `xp`; `y` & `yp`, `T` & `E` with each uncorrelated.
Each parameter defines the maximum absolute extent in that dimension. Ie, the possible values
range from `-envelopeX` to `envelopeX` for example.

* All parameters from `reference`_ distribution as used as centroids.

+----------------------------------+-------------------------------------------------------+
| Option                           | Description                                           |
+==================================+=======================================================+
| `envelopeR`                      | Maximum position                                      |
+----------------------------------+-------------------------------------------------------+
| `envelopeRp`                     | Maximum canonical momentum                            |
+----------------------------------+-------------------------------------------------------+
| `envelopeT`                      | Maximum time offset [s]                               |
+----------------------------------+-------------------------------------------------------+
| `envelopeE`                      | Maximum energy offset [GeV]                           |
+----------------------------------+-------------------------------------------------------+
  

square
^^^^^^

This distribution has similar properties to the `circle`_ distribution with the
exception that the particles are randomly uniformly distributed within a square.

* All parameters from `reference`_ distribution as used as centroids.

+----------------------------------+-------------------------------------------------------+
| Option                           | Description                                           |
+==================================+=======================================================+
| `envelopeX`                      | Maximum position in X [m]                             |
+----------------------------------+-------------------------------------------------------+
| `envelopeXp`                     | Maximum canonical momentum in X                       |
+----------------------------------+-------------------------------------------------------+
| `envelopeY`                      | Maximum position in Y [m]                             |
+----------------------------------+-------------------------------------------------------+
| `envelopeYp`                     | Maximum canonical momentum in Y                       |
+----------------------------------+-------------------------------------------------------+
| `envelopeT`                      | Maximum time offset [s]                               |
+----------------------------------+-------------------------------------------------------+
| `envelopeE`                      | Maximum energy offset [GeV]                           |
+----------------------------------+-------------------------------------------------------+


ring
^^^^

The ring distribution randomly and uniformly fills a ring in `x` and `y` between two radii. For
all other parameters, the `reference`_ coordinates are used - ie `xp`, `yp` etc.

* All parameters from `reference`_ distribution as used as centroids.

+----------------------------------+-------------------------------------------------------+
| Option                           | Description                                           |
+==================================+=======================================================+
| `Rmin`                           | Minimum radius in `x` and `y` [m]                     |
+----------------------------------+-------------------------------------------------------+
| `Rmax`                           | Maximum radius in `x` and `y` [m]                     |
+----------------------------------+-------------------------------------------------------+
  

eshell
^^^^^^

Defines an elliptical annulus in phase space in each dimension that's uncorrelated.

* All parameters from `reference`_ distribution as used as centroids.
  
+----------------------------------+--------------------------------------------------------------------+
| Option                           | Description                                                        |
+==================================+====================================================================+
| `shellX`                         | Ellipse semi-axis in phase space in horizontal position [m]        |
+----------------------------------+--------------------------------------------------------------------+
| `shellXp`                        | Ellipse semi-axis in phase space in horizontal canonical momentum  |
+----------------------------------+--------------------------------------------------------------------+
| `shellY`                         | Ellipse semi-axis in phase space in vertical position [m]          |
+----------------------------------+--------------------------------------------------------------------+
| `shellYp`                        | Ellipse semi-axis in phase space in vertical momentum              |
+----------------------------------+--------------------------------------------------------------------+
| `shellXWidth`                    | Spread of ellipse in phase space in horizontal position [m]        |
+----------------------------------+--------------------------------------------------------------------+
| `shellXpWidth`                   | Spread of ellipse in phase space in horizontal canonical momentum  |
+----------------------------------+--------------------------------------------------------------------+
| `shellYWidth`                    | Spread of ellipse in phase space in vertical position [m]          |
+----------------------------------+--------------------------------------------------------------------+
| `shellYpWidth`                   | Spread of ellipse in phase space in vertical momentum              |
+----------------------------------+--------------------------------------------------------------------+



halo
^^^^
The halo distribution is effectively a flat phase space with the central beam core removed at 
:math:`\epsilon_{\rm core}`. The beam core is defined using the standard twiss parameters described 
previously. The implicit general form of a rotated ellipse is  

.. math::

   \gamma x^2 + 2\alpha\;x\;x^{\prime} + \beta x^{\prime 2} = \epsilon

where the parameters have their usual meanings. A phase space point can be rejected or weighted 
depending on the single particle emittance, which is calculated as    

.. math::
   \epsilon_{\rm SP} = \gamma x^2 + 2\alpha\;x\;x^{\prime} + \beta x^{\prime 2}

if the single particle emittance is less than beam emittance so :math:`\epsilon_{\rm SP} \epsilon_{\rm core}` 
the particle is rejected. `haloPSWeightFunction` is a string that selects the function 
:math:`f_{\rm haloWeight}(\epsilon_{\rm SP})` which is 1 at the ellipse defined by :math:`\epsilon_{\rm core}`. The
weighting functions are either `flat`, one over emittance `oneoverr` or exponential `exp` so  

.. math:: 
   f_{\rm haloWeight}(\epsilon_{\rm SP}) & = 1 \\
   f_{\rm haloWeight}(\epsilon_{\rm SP}) & = \left(\frac{\epsilon_{\rm core}}{\epsilon_{\rm SP}}\right)^p \\
   f_{\rm haloWeight}(\epsilon_{\rm SP}) & = \exp\left(-\frac{\epsilon_{SP}-\epsilon_{\rm core}}{p \epsilon_{\rm core}}\right)

* All parameters from `reference`_ distribution as used as centroids.
  
+----------------------------------+-----------------------------------------------------------------------------+
| Option                           | Description                                                                 |
+==================================+=============================================================================+
| `emitx`                          | Horizontal beam core emittance [m] :math:`\epsilon_{{\rm core},x}`          |
+----------------------------------+-----------------------------------------------------------------------------+
| `emity`                          | Vertical beam core emittance [m] :math:`\epsilon_{{\rm core},y}`            |
+----------------------------------+-----------------------------------------------------------------------------+
| `betx`                           | Horizontal beta function [m]                                                |
+----------------------------------+-----------------------------------------------------------------------------+
| `bety`                           | Vertical beta function [m]                                                  |
+----------------------------------+-----------------------------------------------------------------------------+
| `alfx`                           | Horizontal alpha function                                                   |
+----------------------------------+-----------------------------------------------------------------------------+
| `alfy`                           | Vertical alpha function                                                     |
+----------------------------------+-----------------------------------------------------------------------------+
| `haloNSigmaXInner`               | Inner radius of halo in x (multiples of sigma)                              |
+----------------------------------+-----------------------------------------------------------------------------+
| `haloNSigmaXOuter`               | Outer radius of halo in x (multiples of sigma)                              |
+----------------------------------+-----------------------------------------------------------------------------+
| `haloNSigmaYInner`               | Inner radius of halo in y (multiples of sigma)                              |
+----------------------------------+-----------------------------------------------------------------------------+
| `haloeNSigmaYOuter`              | Outer radius of halo in y (multiples of sigma)                              |
+----------------------------------+-----------------------------------------------------------------------------+
| `haloPSWeightFunction`           | Phase space weight function [string]                                        |
+----------------------------------+-----------------------------------------------------------------------------+
| `haloPSWeightParameter`          | Phase space weight function parameters []                                   |
+----------------------------------+-----------------------------------------------------------------------------+
| `haloXCutInner`                  | X position cut in halo (multiples of sigma)                                 |
+----------------------------------+-----------------------------------------------------------------------------+
| `haloYCutInner`                  | Y position cut in halo (multiples of sigma)                                 |
+----------------------------------+-----------------------------------------------------------------------------+

composite
^^^^^^^^^

The horizontal, vertical and longitudinal phase spaces can be defined independently. The `xDistrType`, 
`yDistrType` and `zDistrType` can be selected from all the other beam distribution types. All of the 
appropriate parameters need to be defined for each individual distribution.

* All parameters from `reference`_ distribution as used as centroids.

+----------------------------------+-------------------------------------------------------+
| Option                           | Description                                           |
+==================================+=======================================================+
| `xDistrType`                     | Horizontal distribution type                          |
+----------------------------------+-------------------------------------------------------+
| `yDistrType`                     | Vertical distribution type                            |
+----------------------------------+-------------------------------------------------------+
| `zDistrType`                     | Longitudinal distribution type                        |
+----------------------------------+-------------------------------------------------------+

.. note:: It is currently not possible to use two differently specified versions of the same
	  distribution within the composite distribution - ie gaussTwiss (parameter set 1) for x
	  and gaussTwiss (parameter set 2) for y. They will have the same settings.

Examples::

  beam, particle="proton",
        energy=3500*GeV,
        distrType="composite",
        xDistrType="eshell",
        yDistrType="gausstwiss",
        zDistrType="gausstwiss",
        betx = 0.5*m,
        bety = 0.5*m,
        alfx = 0.00001234,
        alfy = -0.0005425,
        emitx = 1e-9, 
        emity = 1e-9,
        sigmaE = 0.00008836,
        sigmaT = 0.00000000001,
        shellX  = 150*um, 
        shellY  = 103*um, 
        shellXp = 1.456e-6,
        shellYp = 2.4e-5,
        shellXWidth = 10*um,
        shellYWidth = 15*um,
        shellXpWidth = 1e-9,
        shellYpWidth = 1d-9;


userFile
^^^^^^^^

The `userFile` distribution allows the user to supply an ASCII text file with particle
coordinates that are tab-delimited. The column names and the units are specified in an
input string.

The file may also be compressed using tar and gz. Any file with the extentsion `.tar.gz`
will be automatically decompressed during the run without any temporary files. This is
recommended as compressed ASCII is significantly smaller in size.

.. note:: BDSIM must be compiled with GZIP. This is normally sourced from Geant4 and is
	  by default on.

+----------------------------------+-------------------------------------------------------+
| Option                           | Description                                           |
+==================================+=======================================================+
| `distrFile`                      | File path to ASCII data file                          |
+----------------------------------+-------------------------------------------------------+
| `distrFileFormat`                | A string that details the column names and units      |
+----------------------------------+-------------------------------------------------------+
| `nlinesIgnore`                   | Number of lines to ignore when reading user bunch     |
|                                  | input files                                           |
+----------------------------------+-------------------------------------------------------+

Examples::

  beam, particle = "e-",
        energy = 1*GeV,
        distrType  = "userfile",
        distrFile  = "9_UserFile.dat",
        distrFileFormat = "x[mum]:xp[mrad]:y[mum]:yp[mrad]:z[cm]:E[MeV]";


The corresponding `9_UserFile.dat` file looks like::

  0 1 2 1 0 1000
  0 1 0 1 0 1002
  0 1 0 0 0 1003
  0 0 2 0 0 1010
  0 0 0 2 0 1100
  0 0 0 4 0 1010
  0 0 0 3 0 1010
  0 0 0 4 0 1020
  0 0 0 2 0 1000


	
ptc
^^^

Output from MAD-X PTC used as input for BDSIM. 

+----------------------------------+-------------------------------------------------------+
| Option                           | Description                                           |
+==================================+=======================================================+
| `distrFile`                      | PTC output file                                       |
+----------------------------------+-------------------------------------------------------+

Tunnel Geometry
---------------

BDSIM can build a tunnel around the beamline. Currently, there are two main ways to control this.

1) The tunnel follows the beamline, bending automatically (recommended)
2) The tunnel is just built in a straight line - this may be useful for linear colliders but
   may also cause geometry overlaps and the user is responsible for checking this!

.. warning:: With option 2, the user is entirely responsible to ensure no overlaps occur
	     (through good design). Also note that the samplers may overlap the tunnel
	     depending on the tunnel geometry (samplers are square with half width of
	     `samplerRadius`). In practice however, we haven't observed many ill effects
	     because of this. Problems would take the form of 'stuck particles' and
	     Geant4 would terminate that event.

Examples of tunnel geometry can be found with the BDSIM source code in */examples/features/geometry/tunnel*
and are described in :ref:`tunnel-examples`. 

+----------------------------------+-------------------------------------------------------+
| **Tunnel Parameters**            |                                                       |
+----------------------------------+-------------------------------------------------------+
| buildTunnel                      | whether to build a tunnel (default = 0)               |
+----------------------------------+-------------------------------------------------------+
| buildTunnelStraight              | whether to build a tunnel ignoring the beamline and   |
|                                  | just in a straight line (default = 0)                 |
+----------------------------------+-------------------------------------------------------+
| builTunnelFloor                  | whether to add a floor to the tunnel                  |
+----------------------------------+-------------------------------------------------------+
| tunnelType                       | which style of tunnel to use - one of:                |
|                                  | `circular`, `elliptical`, `square`, `rectangular`     |
|                                  | (more to come in v0.9)                                |
+----------------------------------+-------------------------------------------------------+
| tunnelAper1                      | tunnel aperture parameter #1 - typically              |
|                                  | horizontal (m)                                        |
+----------------------------------+-------------------------------------------------------+
| tunnelAper2                      | tunnel aperture parameter #2 - typically              |
|                                  | vertical (m)                                          |
+----------------------------------+-------------------------------------------------------+
| tunnelThickness                  | thickness of tunnel wall (m)                          |
+----------------------------------+-------------------------------------------------------+
| tunnelSoilThickness              | soil thickness outside tunnel wall (m)                |
+----------------------------------+-------------------------------------------------------+
| tunnelMaterial                   | material for tunnel wall                              |
+----------------------------------+-------------------------------------------------------+
| soilMaterial                     | material for soil outside tunnel wall                 |
+----------------------------------+-------------------------------------------------------+
| tunnelOffsetX                    | horizontal offset of the tunnel with respect to the   |
|                                  | beam line reference trajectory                        |
+----------------------------------+-------------------------------------------------------+
| tunnelOffsetY                    | vertical offset of the tunnel with respect to the     |
|                                  | beam line reference trajectory                        |
+----------------------------------+-------------------------------------------------------+
| tunnelFloorOffset                | the offset of the tunnel floor from the centre of the |
|                                  | tunnel (**not** the beam line).                       |
+----------------------------------+-------------------------------------------------------+

These parameters are shown schematically in the figure below. (gaps not to scale, elliptical
shown as an example).

.. figure:: figures/tunnel/tunnel_parameters.pdf
	    :width: 80%
	    :align: center
	    
The soil around the tunnel is typically symmetric with the `tunnelSoilThickness` being added to
the larger of the horizontal and vertical tunnel dimensions.
		    
.. note:: Construction of the tunnel geometry may fail in particular cases of different beam lines.
	  Beam lines with very strong bends ( > 0.5 rad) over a few metres may cause overlapping
	  geometry. In future, it will be possible to override the automatic algorithm between
	  certain elements in the beamline, but for now such situations must be avoided.


Materials and Atoms
-------------------

Materials and atoms can be added via the parser, just like lattice elements.

If the material is composed by a single element, it can be defined using the **matdef** command with the following syntax::

  materialname : matdef, Z=<int>, A=<double>, density=<double>, T=<double>, P=<double>, state=<char*>;

=========  ========================== =============
parameter  description                default
Z          atomic number
A          mass number [g/mol]
density    density in [g/cm3]
T          temperature in [K]         300
P          pressure [atm]             1
state      "solid", "liquid" or "gas" "solid"
=========  ========================== =============

Example::
  
  iron : matdef, Z=26, A=55.845, density=7.87;
  
If the material is made up by several components, first of all each of them must be specified with the **atom** keyword::
  
  elementname : atom, Z=<int>, A=<double>, symbol=<char*>;
       
=========  =====================
parameter  description               
Z          atomic number
A          mass number [g/mol]
symbol     atom symbol
=========  =====================

The compound material can be specified in two manners:

**1.** If the number of atoms of each component in material unit is known, the following syntax can be used::

   <material> : matdef, density=<double>, T=<double>, P=<double>,
                state=<char*>, components=<[list<char*>]>,
                componentsWeights=<{list<int>}>;

================= ===================================================
parameter         description               
density           density in [g/cm3]
components        list of symbols for material components
componentsWeights number of atoms for each component in material unit
================= ===================================================

Example::
  
  niobium : atom, symbol="Nb", Z=41, A=92.906;
  titanium : atom, symbol="Ti", Z=22, A=47.867;
  NbTi : matdef, density=5.6, T=4.0, components=["Nb","Ti"], componentsWeights={1,1};

**2.** On the other hand, if the mass fraction of each component is known, the following syntax can be used::
     
   <material> : matdef, density=<double>, T=<double>, P=<double>,
                state=<char*>, components=<[list<char*>]>,
                componentsFractions=<{list<double>}>;
		  
=================== ================================================
parameter           description               
components          list of symbols for material components
componentsFractions mass fraction of each component in material unit
=================== ================================================

Example::
  
  samarium : atom, symbol="Sm", Z=62, A=150.4;
  cobalt : atom, symbol="Co", Z=27, A=58.93;
  SmCo : matdef, density=8.4, T=300.0, components=["Sm","Co"], componentFractions = {0.338,0.662};

The second syntax can be used also to define materials which are composed by other materials (and not by atoms).
Nb: Square brackets are required for the list of element symbols, curly brackets for the list of weights or fractions.
  
Regions
-------

In Geant4 it is possible to drive different *regions* each with their own production cuts and user limits.
In BDSIM, there is one default region to which the options prodCutXXXX apply (see `Options`_) and then
the user may define additional regions and attach them to the objects desired.  For example::

  precisionRegion: cutsregion, prodCutProtons=1*m,
                               prodCutElectrons=10*m,
			       prodCutPositrons=10*m,
			       prodCutPhotons = 1*mm;

  d1: drift, l=10*m, region="precisionRegion";


.. rubric:: Footnotes

.. [#beamcommandnote] Note, the *beam* command is actually currently equivalent to the *option* command.
		      The distinction is kept for clarity, and this might be changed in the future.

.. _multiple-beamlines:

Multiple Beam Lines
-------------------

BDSIM has the ability to use multiple beam lines.  This feature is still in development and
is currently only for visualisation purposes. Secondary beam lines are placed either with
respect to the world coordinate system or with respect to a particular element in the main
beam line. A few caveats:

* Only for visualisation purposes.
* Beam lines cannot be placed with respect to an element in another secondary beam line.
* Secondary beam lines are not suitable for tracking.
* Secondary beam lines are not sensitive to energy deposition nor produce output.
* The user is entirely responsible for overlapping geometry. The visualiser will render
  the geometry but of course it will not be suitable for simulations as overlaps lead
  to volume navigation problems and incorrect tracking.

The user may use any sequence defined in the parser before the `use` command. The secondary
beam line is produced by declaring a placement. The placement definition (see
:ref:`placements`) is augmented with the following parameters:

     

+------------------------+---------------------------------------------------------------+
| **Parameter**          |  **Description**                                              |
+------------------------+---------------------------------------------------------------+
| sequence               | Name of the sequence (with `line`) to use for the secondary   |
|                        | beam line.                                                    |
+------------------------+---------------------------------------------------------------+
| referemeceElement      | The element in the sequence with respect to which the beam    |
|                        | line will be placed.                                          |
+------------------------+---------------------------------------------------------------+
| referenceElementNumber | The *i* th instance of the element in the sequence (zero      |
|                        | counting).  i.e. 2 -> the 3rd instance of `referenceElement`  |
|                        | in the `sequence`.                                            |
+------------------------+---------------------------------------------------------------+

Examples
^^^^^^^^

This example is shown in bdsim/examples/features/geometry/10_multiple_beamlines.  It defines
a simple beam line and two other sequences that are placed along side it. Further explanation
is given below the example.

::

   d1: drift, l=1*m;
   d2: drift, l=3*m;
   d3: drift, l=5*m;
   sb1: sbend, l=1*m, angle=0.5;
   sb2: sbend, l=1*m, angle=-0.5;
   q1: quadrupole, l=0.2*m, k1=4.166666;
   q2: quadrupole, l=0.2*m, k1=-4.166666;

   fodo: line=(d1,q1,d1,q2);
   mainLine: line=(d2,sb1,d2,sb2,d2,fodo,fodo);

   auxLine1: line=(d3,sb1,d1,sb2,d1,fodo,d1);
   auxLine2: line=(d1,sb1,d1,sb2,d1,fodo,d1);
   auxLine3: line=(fodo);

   use, mainLine;

   beam, particle="e-",
         energy=3*GeV;

   auxLine1Place: placement, sequence = "auxLine1",
                             referenceElement = "d2" ,
			     referenceElementNumber = 2,
			     x = -5*cm,
			     z = -1*m,
			     axisAngle = 1,
			     axisY = 1,
			     angle = -0.2;
   
   auxLine2Place: placement, sequence = "auxLine2",
                             referenceElement = "d2",
			     referenceElementNumber = 2,
			     x = -10*cm,
			     z = -1*m,
			     axisAngle = 1,
			     axisY = 1,
			     angle = -0.5;

   auxLine3Place: placement, sequence = "auxLine3",
                             x = 1*m,
			     axisAngle = 1,
			     axisY = 1,
			     angle = 0.2;

Firstly a series of simple elements are defined (drifts, quadrupoles and bends). A simple
sequence called `fodo` is defined and also the main beam line called `mainLine`. After this
extra sequences are defined that we will use for secondary beam lines.  The `use` command
selects which beam line the simulation will be based on.::

  use, mainLine;

After this, the beam is defined (required for any simualtion for rigidity calculations) and
then the placement of secondary beam lines.

The first placement `auxLine1Place` is a placement that will place the sequence named
`auxLine1` with respect to the 3rd instance of the element `d2` in the primary sequence
(`mainLine`).::

  auxLine1Place: placement, sequence="auxLine1",
                            referenceElement="d2" ,
			    referenceElementNumber=2,

The placement is generally with respect to the centre of the element described in the primary
beam line and along the direction it's pointing. Without any displacement, the geometry
would therefore overlap.  Here, an offset and rotation are specified for this placement.
An offset in `x` of -5 cm and -1 m in `z` is specified. The coordinate sytem is right-handed
with positive z pointing along the direction of travel in the beam line. A negative x
displacement is therefore to the right looking along the direction or travel and 1 m in
`z` is towards the beginning of the element from the centre.  Rotations are described
in :ref:`placements`. Here, an axis angle rotation is used. The beam line is rotated about
unit Y axis (local to that element) by -0.2 rad.

The second placement uses a different sequence, but in a similar fashion.

The third placement doesn't specify a `referenceElement`, so the placement is with respect
to the beginning of the beam line.

The model is shown below.

.. figure:: figures/multiple_beamlines.png
	    :width: 90%
	    :align: center

The drift segments do not of course connect but are merely placed close to each other.
In future, continuous vacuum points will be provided.

.. figure:: figures/multiple_beamlines_junction.png
	    :width: 90%
	    :align: center
