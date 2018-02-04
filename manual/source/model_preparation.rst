=================
Model Preparation
=================

A BDSIM model can be prepared either manually in a hand-written fashion,
or using a provided suite of python tools to automatically convert
the description of an accelerator lattice from other formats, such as
MADX, MAD8 or Transport, to that of BDSIM - gmad.

The automatic conversion is typically achieved by preparing a 'rendered'
or 'flat' output description of each element in the accelerator from
whatever optics program you use to design the accelerator, and then
converting this using our Python utility pybdsim - see :ref:`python-utilities`.

Additionally, the python tools can be used to programatically create
an accelerator lattice of your own design, which is described in
`Python Builder`_.

Manual Preparation
------------------

An input gmad (text) file can be prepared manually in your favourite
text editor easily by defining (in order):

1. Individual elements
2. Define the order they appear in a :code:`line`
3. Which :code:`period` to use - the above line
4. Options such as the physics list and tracking cuts
5. Input beam distribution

Please see :ref:`model-description` for a description of the
input syntax.

MADX Conversion
---------------

A MADX lattice can be easily converted to a BDSIM gmad input file using the supplied
python utilities. This is achieved by

1. preparing a tfs file with madx containing all twiss table information
2. converting the tfs file to gmad using pybdsim

The twiss file can be prepared by appending the following MADX syntax to the
end of your MADX script::

  select,flag=twiss, clear; 
  twiss,sequence=SEQUENCENAME, file=twiss.tfs;

where `SEQUENCENAME` is the name of the sequence in madx. By not specifying the output
columns, a very large file is produced containing all possible columns.  This is required
to successfully convert the lattice.  If the tfs file contains insufficient information,
pybdsim will not be able to convert the model.

This is described in more detail in the dedicated pybdsim documentation
`<http://www.pp.rhul.ac.uk/bdsim/pybdsim/convert.html>`_.

.. note:: The python utilities require "`.tfs`" suffix as the file type to work properly.

To convert the tfs file, pybdsim should be used.  pybdsim along with other utilities can
be found in the utils directory in the bdsim source directory.

.. note:: If these folders are empty, please update the submodules as described in
	  :ref:`from-git-repository`.


MAD8 Conversion
---------------

This can be prepared in a similar fashion to a MADX model. The user must have our
pymad8 and pybdsim packages (see :ref:`python-utilities`).

This is described in more detail in the dedicated pybdsim documentation
`<http://www.pp.rhul.ac.uk/bdsim/pybdsim/convert.html>`_.

Python Builder
--------------

Using the classes in the pybdsim package that are used in the conversion process,
the user may create a BDSIM model programmatically, i.e. the user can write a Python
script to create the lattice they want.

This is described in more detail in the dedicated pybdsim documentation
`<http://www.pp.rhul.ac.uk/bdsim/pybdsim/builder.html>`_

The pymadx package also has a similar functionality and is documented in its
dedicated documentation `<http://www.pp.rhul.ac.uk/bdsim/pymadx/moduledocs.html#module-pymadx.Builder>`_.
