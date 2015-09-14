.. _appendix2_geometry:

===========================
Appendix 2 - Geometry Input
===========================

The element with user-defined physical geometry (see also :ref:`element`) is defined by::
  
  <element_name> : element, geometry=format:filename, attributes

for example::
  
  colli : element, geometry="gmad:colli.geo"

gmad
----

ggmad is a simple format used as G4geometry wrapper. It can be used for specifying more or less simple geometries like collimators. Example::

  Cons {x0=0,y0=0,z0=375,rmin1=100rmax1=500rmin2=5rmax2=500z=125material=Graphite,phi0=0,dphi=360,temperature=1}

A file can contain several objects which will be placed sequentially into the volume.

.. note:: The user has to make sure that there is no overlap between them.
  
Available shapes are:

Box
^^^

========== ========================
parameter  description
x0         x origin
y0         y origin
z0         z origin
x          x size
y          y size
z          z size
phi        Euler angle for rotation
theta      Euler angle for rotation
psi        Euler angle for rotation
material   Material name
========== ========================

Cons
^^^^

========== =========================
parameter  description
x0         x origin
y0         y origin
z0         z origin
rmin1      inner radius at start
rmax1      outer radius at start
rmin2      inner radius at end
rmax2      outer radius at end
z          z size
phi        Euler angle for rotation
theta      Euler angle for rotation
psi        Euler angle for rotation
phi0       angle for start of sector
dphi       angle swept by sector
material   Material name
========== =========================

Tubs
^^^^

========== =========================
parameter  description
x0         x origin
y0         y origin
z0         z origin
rmin       inner radius
rmax       outer radius
z          z size
phi        Euler angle for rotation
theta      Euler angle for rotation
psi        Euler angle for rotation
material   Material name
========== =========================

Trd
^^^

========== ============================
parameter  description
x0         x origin
y0         y origin
z0         z origin
x1         half length at wider side
x2         half length at narrower side
y1         half length at wider side
y2         half length at narrower side
z          z size
phi        Euler angle for rotation
theta      Euler angle for rotation
psi        Euler angle for rotation
material   Material name
========== ============================



Mokka
-----

GDML
----
