***************
Python Packages
***************

Several python packages are provided to aid preparation and conversion
of models to a from BDSIM's gmad format and are described the following
sections.

* `pybdsim`_ - conversion and building of input as well as ASCII data analysis
* `pymadx`_  - file loader for MADX Tfs file format
* `pymad8`_  - file loader for MAD8 file format

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


.. include:: pybdsim.rst
	     
.. include:: pymadx.rst
	     
.. include:: pymad8.rst

	     
