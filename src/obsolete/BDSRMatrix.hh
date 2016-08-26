/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
// G.A.Blair (RHUL) 10.12.01
// adapted from code supplied by D.Schulte (CERN)

#ifndef BDSRMatrix_h
#define BDSRMatrix_h 1

#include "globals.hh"

class BDSRMatrix
{
public:
  BDSRMatrix();
  G4double r11,r12,r21,r22;
};

#endif
