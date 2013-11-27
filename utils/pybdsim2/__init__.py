# pybdsim - python tools for bdsim
# Version 1.0
# L. Nevay
# laurie.nevay@rhul.ac.uk

import Data
import Plot
import Build
import _General

from Analysis import Analysis

__all__ = ['Data','Plot','Build']

"""
pybdsim - python tools for bdsim

dependencies:
package     - minimum version required
numpy       - 1.7.1
matplotlib  - 1.3.0

Modules:
Build  - create generic accelerators for bdsim
Data   - read the bdsim output formats
Plot   - some nice plots for data

Classes:
Analysis - encapsulates functions & plots for a single file 

"""

#import BeamTester
#import Builder
#import ComponentTester
#import Tester
#import analysis
#import outputLoader
