/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

// BDSExtract
// Generates individual particles from PLACET output file
// G.A.Blair (RHUL) 10.12.01
// adapted from code supplied by D.Schulte (CERN)

#ifndef BDSSpline_tab_entry_h
#define BDSSpline_tab_entry_h 1

//---------------
class BDSSpline_tab_entry
{
public:
  BDSSpline_tab_entry();

  G4double x,y,y2;
};

#endif
