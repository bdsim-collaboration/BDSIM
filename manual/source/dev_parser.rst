.. _dev-parser:

Parser
******

The main parser interface can be found in :code:`parser/parser.h`.  The parser is currently a
singleton (only one instance with global scope).

Options
=======

All options for a BDSIM run are contained in an instance of :code:`parser/options.h:GMAD::Options` class.
This is passed to BDSIM.
