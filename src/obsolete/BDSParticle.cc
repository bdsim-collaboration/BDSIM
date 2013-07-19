/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
// BDSParticle.hh
// Bunch format interfacing to PLACET output file
// G.A.Blair (RHUL) 10.12.01
// adapted from code supplied by D.Schulte (CERN)

#include "BDSParticle.hh"

BDSParticle::BDSParticle():energy(0.0),wgt(0.0),y(0.0),yp(0.0),x(0.0),xp(0.0)
{;}
