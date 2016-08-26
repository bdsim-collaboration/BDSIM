/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
// BDSParticle.hh
// Bunch format interfacing to PLACET output file
// G.A.Blair (RHUL) 10.12.01
// adapted from code supplied by D.Schulte (CERN)

#ifndef BDSParticle_h
#define BDSParticle_h 1

#include "globals.hh"

class BDSParticle
{
public:
  BDSParticle();
  G4double energy,wgt,y,yp;
  G4double x,xp;
};


#endif
