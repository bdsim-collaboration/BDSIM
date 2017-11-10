.. _python-packages:

***************
Python Packages
***************

Several python packages are provided to aid preparation and conversion
of models to a from BDSIM's gmad format and are described the following
sections.

* pybdsim - conversion and building of input as well as ASCII data analysis
* pymadx  - file loader for MADX Tfs file format
* pymad8  - file loader for MAD8 file format
* pytransport - file loader for Transport file format

Installing Python Packages
==========================

To access (import) the supplied packages from anywhere on your system,
the `utils` directory should be added to your `PYTHONPATH` environmental
variable::

  export PYTHONPATH=$PYTHONPATH:~/physics/reps/bdsim/utils

This should allow you to import the python utilities without error
from any location::

  cd ~/
  python
  
>>> import pybdsim
>>> # no errors


Documentation
=============

In the following sections is individual documentatino for each python package.
These are also available when using ipython by typing ? after any module or class.

   * `<http://www.pp.rhul.ac.uk/bdsim/pybdsim/>`_
   * `<http://www.pp.rhul.ac.uk/bdsim/pymadx/>`_
   * `<http://www.pp.rhul.ac.uk/bdsim/pytransport/>`_

.. toctree::
   :maxdepth: 2
   
   pymad8




   

