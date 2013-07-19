/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

// BDSExtract
// Generates individual particles from PLACET output file
// G.A.Blair (RHUL) 10.12.01
// adapted from code supplied by D.Schulte (CERN)

#include "globals.hh"
#include "BDSSpline_tab_entry.hh"

BDSSpline_tab_entry::BDSSpline_tab_entry():x(0.0),y(0.0),y2(0.0){;}
