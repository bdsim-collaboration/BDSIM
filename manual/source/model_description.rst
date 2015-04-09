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

Examples
""""""""
::

   sx1: sextupole, l=0.5*m, k2=4.678;
   sx2: sextupole, l=20*cm, k2=45.32, magnetGeometry="normalconducting";
		    
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

Examples
""""""""
::

   oct4b: octupole, l=0.3*m, k3=32.9;
		    
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

Examples
""""""""
::

   MXDEC3: decapole, l=0.3*m, k3=32.9;
  

multipole
^^^^^^^^^

`multipole` defines a general multipole magnet.

* The `aperture parameters`_ may also be specified.
* The `magnet geometry parameters`_ may also be specified.

Examples
""""""""
::

   **To be copmleted**

vkick
^^^^^

`vkick` defines a vertical dipole magnet and has the same parameters as `sbend`.

* The `aperture parameters`_ may also be specified.
* The `magnet geometry parameters`_ may also be specified.

Examples
""""""""
::

   KX15v: vkick, angle=0.01*mrad;

hkick
^^^^^

`hkick` defines a horizontal dipole magnet and has the same parameters as `sbend`.

* The `aperture parameters`_ may also be specified.
* The `magnet geometry parameters`_ may also be specified.

Examples
""""""""
::

   KX17h: hkick, angle=0.01;

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

Examples
""""""""
::

   RF4f: rf, l=3*m, gradient=10*MV;

rcol
^^^^

.. figure:: figures/rcol.png
	    :width: 30%
	    :align: right

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

.. note:: `rcol` and `ecol` do not currently implement tilt, so if an angled collimator
	  is required, a `transform3d` should before and afterwards in the sequence to
	  rotate the coordinate frame before and afterwards. See `transform3d`_ for further
	  details and examples.

Examples
""""""""
::

   TCP15: rcol, l=1.22*m, material="graphite", xsize=104*um, ysize=5*cm;


ecol
^^^^

.. figure:: figures/ecol.png
	    :width: 30%
	    :align: right

`ecol` defines an elliptical collimator. This is exactly the same as `rcol` except that
the aperture is elliptical and the `xsize` and `ysize` define the horizontal and vertical
half axes repsectively.

muspoiler
^^^^^^^^^

.. figure:: figures/muspoiler.png
	    :width: 30%
	    :align: right

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

.. figure:: figures/solenoid.png
	    :width: 30%
	    :align: right

`solenoid` defines a solenoid magnet. This utilises a thick lens transfer map with a
hard edge field profile so it is not equivalent to split a single solenoid into multiple
smaller ones. **This is currently under development**. The strength parameter `ks` is
defined as :math:`ks =`.

================  ============================  ==========  ===========
parameter         description                   default     required
`l`               length [m]                    0.1         yes
`ks`              solenoid strength [ ]         0           yes
`material`        outer material                Iron        no
`outerDiameter`   outer full width [m]          global      no
================  ============================  ==========  ===========

Examples
""""""""
::

   atlassol: solenoid, l=20*m, ks=0.004;


laser
^^^^^

`laser` defines a drift section with a laser beam inside. The laser acts as a static target
of photons.

================  =================================================  ==========  ===========
parameter         description                                        default     required
`l`               length of drift section [m]                        0.1         yes
`x`, `y`, `z`     components of laser direction vector (normalised)  (1,0,0)     yes
`waveLength`      laser wavelength [m]                               532*nm      yes
================  =================================================  ==========  ===========

Examples
""""""""
::

   laserwire: laser, l=1*um, x=1, y=0, z=0, wavelength=532*nm;


transform3d
^^^^^^^^^^^

`transform3d` defines an arbitrary 3-dimensional transformation of the the curvlinear coordinate
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
	  if it intedded for only one component.

Examples
""""""""
::

   rcolrot: transform3d, psi=pi/2;

element
^^^^^^^

`element` defines an arbitrary element that's defined by external geometry and magnetic field
maps. Several geometry formats are supported. The user must also supply the outer radius of the
object for tunnel geometry compatability.

================  ===============================  ==========  ===========
parameter         description                      default     required
`geometry`        filename of geometry             NA          yes
`outR`            outermost radius [m]             NA          yes
`bmap`            filename of magnetif field map   NA          no
================  ===============================  ==========  ===========

`geometry` and `bmap` require the input string to be of the format `format:filename`, where
`format` is the geometry format being used (`gdml` | `mokka`) and filename is the filename of
the geometry file.

Examples
""""""""
::

   detector: element, geometry="gdml:atlasreduced.gmdl", outR=20*m;
   detec: element, geometry="mokka:qq.sql", bmap ="mokka:qq.bmap";


marker
^^^^^^
`marker` defines a point in the lattice. This element has no physical length and is only
used as a reference. For example, a `sampler` (see `samplers - output`_ )
is used to record particle passage at the
front of a component but how would you record particles exiting a particular component?
The intended method is to use a `marker` and place it in the sequence after that element
then attach a sampler to the marker.

Examples
""""""""
::

   m1: marker;
   

Aperture Parameters
^^^^^^^^^^^^^^^^^^^

For elements that contain a beam pipe, several aperture models can be used. These aperture
parameters can be set as the default for every element using the :code:`option` command
(see `options`_ ) and
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

Magnet Geometry Parameters
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. warning:: To be completed

* `boxSize`
* `outerDiameter`


Lattice Sequence
----------------

Once all the necessary components have been defined, they must be placed in a sequence to make
a lattice. Elements can be repeated [#doublesamplernote]_. A sequence of elements is defined by
a `line`_. Lines of lines can be made to describe the accelerator sequence programatically i.e.
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

use - Defining which Line to Use
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Once all elements and at least one `line` is defined, the main sequence of the
beam line can be defined. This must be defined using the following syntax::

  use, period=<line_name>

Examples
""""""""
::
   
   d1: drift, l=3.2*m;
   q1: quadrupole, l=20*cm, k1=4.5;
   q2: quadrupole, l=20*cm, k1=-4.5;
   fodo: line=(d1,q1,d1,q2,d1);
   use, period=fodo;


Samplers - Output
-----------------

Normally, the only output BDSIM would produce is the various particle loss histograms,
as well as the coordinates of energy deposition hits. To observe the particles at a
point in the beam lattice a `sampler` can be used. Samplers are attached to an already
defined element and record all the particles passing through a plane at the entrance
to that element. They are defined using the following syntax::

  sample, range=<element_name>

where `element_name` is the name of the element you wish to sample. Depending on the
output format chosen, the element name may be recorded in the output (ROOT output only).

To place a sampler after an item, attach it to the next item. If however, you wish
to record the coordinates at the end of the line or with another name, you must define
a marker, place it in the sequence and then define a sampler that uses that marker::

  d1: drfit, l=2.4*m;
  endoftheline: marker;
  l1: line=(d1,d1,d1,d1,endoftheline);
  use,period=l1;

  sample, range=endoftheline;

.. note:: Samplers **can only** be defined **after** the main sequences has been defined
	  using the `use` command (see `use - Defining which Line to Use`_). Failure to do
	  so will result in an error and BDSIM will exit.
	  

Physics Lists
-------------

BDSIM can exploit all the physics processes that come with Geant4. As with any Geant4 program
and simulation it is very useful to define the physical processes that should be simulated so
that the simulation is both relevant and efficient. Rather than specify each individual process
for every individual particle, a series of "physics lists" are provided that are a predetermined
set of physics process suitable for a certain application. BDSIM follows the Geant4 ethos in this
regard.

The physics list can be selected with the following syntax::

  option, physicsList="physicslistname";

.. note:: Some physics lists allow biasing and re-weighting for some processes to further improve
	  simulation efficiency. (See `options`_ for more details).

Physics Lists In BDSIM
^^^^^^^^^^^^^^^^^^^^^^
============================  ======================================================================
standard                      transportation of primary particles only - no scattering in material
em_standard                   transportation of primary particles, ionization, bremsstrahlung,
                              Cerenkov, multiple scattering/
em_low                        the same as `em_standard` but using low energy electromagnetic models.
em_single_scatter             **TBC**.
em_muon                       `em_standard` plus muon production processes with biased muon
                              cross-sections.
lw                            list for laser wire simulation - `em_standard` and "laserwire"
                              physics, which is Compton Scattering with total cross-section
			      renormalized to 1.
merlin                        transportation of primary particles, and the following processes
                              for electrons: multiple scattering, ionisation, and bremsstrahlung.
hadronic_standard             `em_standard` plus fission, neutron capture, neutron and proton
                              elastic and inelastic scattering.
hadronic_muon                 `hadronic_standard` plus muon production processes with biased muon
                              cross-sections.
hadronic_QGSP_BERT            `em_standard` plus hadronic physics using the quark gluon string
                              plasma (QGSP) model and the Bertini cascade model (BERT).
hadronic_QGSP_BERT_muon       `hadron_QGSP_BERT` plus muon production processes with biased muon
                              cross-sections.
hadronic_FTFP_BERT            `em_standard` plus hadronic physics using the Fritiof model followed
                              by Reggion cascade and Precompound and evaporation models for the
			      nucleus de-excitation (FTFP) model and the Bertini cascade model
			      (BERT).
hadronic_FTFP_BERT_muon       `hadronic_FTFP_BERT` plus muon production processes with biased muon
                              cross-sections.
hadronic_QGSP_BERT_HP_muon    `hadronic_QGSP_BERT_muon` plus high precision low energy neutron
                              scattering models
============================  ======================================================================


Options
-------

Various simulation details can be controlled throught the `option` command. Options are defined
using the following syntax::

  option, <option_name>=<value>;

If the value is a string and not a number, it should be enclosed in "double inverted commas".
Multiple options can be defined at once using the following syntax::

  option, <option1> = <value>,
          <option2> = <value>;

options in BDSIM
^^^^^^^^^^^^^^^^ 

Below is a full list of all options in BDSIM. If the option is boolean, 1 or 0 can be used
as their value.

+----------------------------------+-------------------------------------------------------+
| Option                           | Function                                              |
+==================================+=======================================================+
| **Common Paramters**             |                                                       |
+----------------------------------+-------------------------------------------------------+
| beampipeRadius                   | default beam pipe inner radius [m]                    |
+----------------------------------+-------------------------------------------------------+
| beampipeThickness                | default beam pipe thickness [m]                       |
+----------------------------------+-------------------------------------------------------+
| beampipeMaterial                 | default beam pipe material                            |
+----------------------------------+-------------------------------------------------------+
| boxSize                          | default accelerator component full width [m]          |
+----------------------------------+-------------------------------------------------------+
| randomSeed                       | the integer seed value for the random number          |
|                                  | generator                                             |
+----------------------------------+-------------------------------------------------------+
| ngenerate                        | number of primary particles to simulate               |
+----------------------------------+-------------------------------------------------------+
| elossHistoBinWidth               | the width of the histogram bins [m]                   |
+----------------------------------+-------------------------------------------------------+
| physicsList                      | the physics list to use                               |
+----------------------------------+-------------------------------------------------------+
| thresholdCutCharged              | the minimum energy above which to simulate electron   |
|                                  | and positrons - any below this energy will be killed  |
+----------------------------------+-------------------------------------------------------+
| thresholdCutPhotons              | the minimum energy above which to simulate photons -  |
|                                  | any below this energy will be killed                  |
+----------------------------------+-------------------------------------------------------+
| stopTracks                       | whether to track secondaires or not (defalt = 1)      |
+----------------------------------+-------------------------------------------------------+
| circular                         | whether the accelerator is circular or not            |
+----------------------------------+-------------------------------------------------------+
| **Geometry Parameters**          |                                                       |
+----------------------------------+-------------------------------------------------------+
| samplerDiamater                  | diameter of samplers (default 8 m) [m]                |
+----------------------------------+-------------------------------------------------------+
| includeIronMagFields             | whether to include magnetic fields in the magnet      |
|                                  | poles                                                 |
+----------------------------------+-------------------------------------------------------+
| sensitiveBeamlineComponents      | whether all beam line componetns record energy loss   |
+----------------------------------+-------------------------------------------------------+
| sensitiveBeamPipe                | wheterh the beam pipe records energy loss             |
+----------------------------------+-------------------------------------------------------+
| vacuumMaterial                   | the material to use for the beam pipe vacuum          |
+----------------------------------+-------------------------------------------------------+
| vacuumPressure                   | the pressure of the vacuum gas                        |
+----------------------------------+-------------------------------------------------------+
| **Tracking Parameters**          |                                                       |
+----------------------------------+-------------------------------------------------------+
| deltaChord                       | chord finder precision                                |
+----------------------------------+-------------------------------------------------------+
| deltaIntersection                | boundary intersection precision                       |
+----------------------------------+-------------------------------------------------------+
| chordStepMinimum                 | minimum step size                                     |
+----------------------------------+-------------------------------------------------------+
| lengthSafety                     | element overlap safety (caution!)                     |
+----------------------------------+-------------------------------------------------------+
| minimumEpsilonStep               | minimum relative error acceptable in stepping         |
+----------------------------------+-------------------------------------------------------+
| maximumEpsilonStep               | maximum relative error acceptable in stepping         |
+----------------------------------+-------------------------------------------------------+
| deltaOneStep                     | set position error acceptable in an integration step  |
+----------------------------------+-------------------------------------------------------+
| **Physics Processes Parameters** |                                                       |
+----------------------------------+-------------------------------------------------------+
| synchRadOn                       | whether to use synchrotron radiation processes        |
+----------------------------------+-------------------------------------------------------+
| srTrackPhotons                   | whether to track synchrotron radiation photons        |
+----------------------------------+-------------------------------------------------------+
| srLowX                           | minimum synchrotron radiation energy as a fraction of |
|                                  | `E critical` ( 0 > `srLowX` > 1 )                     |
+----------------------------------+-------------------------------------------------------+
| srLowGamE                        | lowest synchrotron photon energy to track             |
+----------------------------------+-------------------------------------------------------+
| srMultiplicity                   | a factor multiplying the number of synchrotron        |
|                                  | photons                                               |
+----------------------------------+-------------------------------------------------------+
| prodCutPhotons                   | standard overall production cuts for photons          |
+----------------------------------+-------------------------------------------------------+
| prodCutPhotonsP                  | precision production cuts for photons                 |
+----------------------------------+-------------------------------------------------------+
| prodCutElectrons                 | standard overall production cuts for electrons        |
+----------------------------------+-------------------------------------------------------+
| prodCutElectronsP                | precision production cuts for electons                |
+----------------------------------+-------------------------------------------------------+
| prodCutPositrons                 | standard overall production cuts for positrons        |
+----------------------------------+-------------------------------------------------------+
| prodCutPositronsP                | precision production cuts for positrons               |
+----------------------------------+-------------------------------------------------------+
| turnOnCerenkov                   | whether to produce cerenkov radiation                 |
+----------------------------------+-------------------------------------------------------+
| defaultRangeCut                  | the default predicted range at which a particle is    |
|                                  | cut (default 0.7 mm) [m]                              |
+----------------------------------+-------------------------------------------------------+
| gammaToMuFe                      | the cross-section enhancement factor for the gamma to |
|                                  | muon process                                          |
+----------------------------------+-------------------------------------------------------+
| annihiToMuFe                     | the cross-section enhancement factor for the          |
|                                  | electron-positron annihilation to muon process        |
+----------------------------------+-------------------------------------------------------+
| eetoHadronsFe                    | the cross-section enhancement factor for the          |
|                                  | electron-positron annihilation to hadrons process     |
+----------------------------------+-------------------------------------------------------+
| useEMLPB                         | wheter to use electromagnetic lead particle biasing   |
|                                  | (default = 0)                                         |
+----------------------------------+-------------------------------------------------------+
| LPBFraction                      | the fraction of electromagnetic process in which      |
|                                  | lead particle biasing is used ( 0 < LPBFraaction < 1) |
+----------------------------------+-------------------------------------------------------+
| trajCutGTZ                       | global z position cut (minimum) for storing           |
|                                  | trajectories                                          |
+----------------------------------+-------------------------------------------------------+
| trajCutLTR                       | radius cut for storing trajectories (maximum)         |
+----------------------------------+-------------------------------------------------------+
| Output Parameters                | Function                                              |
+----------------------------------+-------------------------------------------------------+
| storeTrajectory                  | whether to store trajectories in the output           |
+----------------------------------+-------------------------------------------------------+
| storeMuonTrajectories            | whether to store muon trajectories in the output      |
+----------------------------------+-------------------------------------------------------+
| storeNeutronTrajectories         | whether to store neutron trajectories in the output   |
+----------------------------------+-------------------------------------------------------+
| nperfile                         | number of evens to record per output file             |
+----------------------------------+-------------------------------------------------------+
| nlinesIgnore                     | number of lines to ignore when reading user bunch     |
|                                  | input files                                           |
+----------------------------------+-------------------------------------------------------+
| **Tunnel Parameters**            | **Currently Not Working**                             |
+----------------------------------+-------------------------------------------------------+
| buildTunnel                      | whether to build a tunnel (default = 0)               |
+----------------------------------+-------------------------------------------------------+
| builTunnelFloor                  | whether to add a floor to the tunnel                  |
+----------------------------------+-------------------------------------------------------+
| tunnelRadius                     | tunnel inner radius [m]                               |
+----------------------------------+-------------------------------------------------------+
| tunnelThickness                  | thickness of tunnel wall [m]                          |
+----------------------------------+-------------------------------------------------------+
| tunnelSoilThickness              | soil thickness outside tunnel wall [m]                |
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
| tunnelFLoorOffset                | the offset of the tunnel floor from the centre of the |
|                                  | tunnel                                                |
+----------------------------------+-------------------------------------------------------+

Beam Parameters
---------------

To specify the input particle distribution to the accelerator model, the `beam` command is
used. This also specifies the particle species and **reference energy**, which is the
design energy of the machine. This is used along with the particle species to calculate
the momentum of the reference particle and therefore the magnetic field of dipole magnets
if only the angle has been specified.

.. note:: A design energy can be specified and in addtion, the central energy, of say
	  a bunch with a Gaussian distribution, can be specified.

The user must specify at least `energy`, `particle` and `distrType` (the distribution type).
Additional parameters can be specified to detail in the input distribution. The beam is
defined using the following syntax::

  beam, particle="proton",
        energy=4.0*TeV,
	distrType="reference";

Enegy is in `GeV` by default. The partilce may be one of the following:

* `e-`
* `e+`
* `proton`
* `gamma`
* `mu-`
* `mu+`

Many particles can be used and are taken from the Geant4 particle table directly.

Available input distributions and their associated parameters are described in the following
section.

Beam Distributions
^^^^^^^^^^^^^^^^^^


- gauss
- gaussTwiss
- reference


Regions
-------

In Geant4 it is possible to drive different *regions* each with their own production cuts and user limits.
In BDSIM three different regions exist, each with their own user defined production cuts (see *Physics*). 
These are the default region, the precision region and the approximation region. Beamline elements 
can be set to the precision region by setting the attribute *precisionRegion* equal to 1. For example:





.. rubric:: Footnotes

.. [#doublesamplernote] Note, if a sampler is attached to a beam line element and that element is
			use more than once in a *line*, then output will only be from the first
			occurence of that element in the sequence. This will be addressed in future
			releases.
