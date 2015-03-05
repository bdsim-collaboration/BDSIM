================================
Model Description - Input Syntax
================================

Lattice Elements
----------------

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

Distributions
-------------


- gauss
- gaussTwiss
- reference



+-----------------+--------------+-------------------+-----------------+---------------+---------------+
| Aperture Model  | # of         | aper1             | aper2           | aper3         | aper4         |
|                 | parameters   |                   |                 |               |               |
+=================+==============+===================+=================+===============+===============+
| circular        | 1            | radius            | NA              | NA            | NA            |
+-----------------+--------------+-------------------+-----------------+---------------+---------------+
| rectangular     | 2            | x half width      | y half width    | NA            | NA            |
+-----------------+--------------+-------------------+-----------------+---------------+---------------+
| elliptical      | 2            | x semi-axis       | y semi-axis     | NA            | NA            |
+-----------------+--------------+-------------------+-----------------+---------------+---------------+
| lhcscreensimple | 3            | x half width of   | y half width of | radius of     | NA            |
|                 |              | rectangle         | rectangle       | circle        |               |
+-----------------+--------------+-------------------+-----------------+---------------+---------------+
| lhcscreen       | 3            | x half width of   | y half width of | radius of     | NA            |
|                 |              | rectangle         | rectangle       | circle        |               |
+-----------------+--------------+-------------------+-----------------+---------------+---------------+
| rectellipse     | 4            | x half width of   | y half width of | x semi-axis   | y semi-axis   |
|                 |              | rectangle         | rectangle       | of ellipse    | of ellipse    |
+-----------------+--------------+-------------------+-----------------+---------------+---------------+
| racetrack       | 3            | horizontal offset | vertical offset | radius of     | NA            |
|                 |              | of circle         | of circle       | circular part |               |
+-----------------+--------------+-------------------+-----------------+---------------+---------------+
| octagon         | 4            | x half width      | y half width    | angle 1 [rad] | angle 2 [rad] |
+-----------------+--------------+-------------------+-----------------+---------------+---------------+


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
