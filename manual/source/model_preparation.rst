=================
Model Preparation
=================

A BDSIM model can be prepared either manually in a hand-written fashion,
or using a provided suite of python tools to automatically convert
the description of an accelerator lattice from other formats to that
of BDSIM - gmad.

Additionally, the python tools can be used to programatically create
an accelerator lattice of your own design, which is described in
`Python Builder`_.

Manual Preparation
------------------

An input gmad (text) file can be prepared manually in your favourite
text editor easily by defining:

1 Individual elements
2 Define the order they appear in a :code:`line`
3 Which :code:`period` to use - the above line
4 Options such as the physics list and tracking cuts
5 Input beam distribution

Please see :ref:`model-description` for a description of the
input syntax.

MADX Conversion
---------------

A MADX lattice can be easily converted to a BDSIM gmad input file using the supplied
python utilities. This is achieved by

1 preparing a tfs file with madx containing all twiss table information
2 converting the tfs file to gmad using pybdsim

The twiss file can be prepared by appending the following MADX syntax to the
end of your MADX script::

  select,flag=twiss, clear; 
  twiss,sequence=SEQUENCENAME, file=twiss.tfs;

where `SEQUENCENAME` is the name of the sequence in madx. By not specifying the output
columns, a very large file is produced containing all possible columns.  This is required
to successfully convert the lattice.  If the tfs file contains insufficient information,
pybdsim will not be able to convert the model.

.. note:: The python utilities require "`.tfs`" suffix as the file type to work properly.

To convert the tfs file, pybdsim should be used.  pybdsim along with other utilities can
be found in the utils directory in the bdsim source directory.

.. note:: If these folders are empty, please update the submodules as described in
	  :ref:`from-git-repository`.



MAD8 Conversion
---------------

Python Builder
--------------
