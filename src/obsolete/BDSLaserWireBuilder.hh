/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 9.3.2003
   Copyright (c) 2003 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
#ifndef BDSLaserWireBuilder_h
#define BDSLaserWireBuilder_h 

#include "globals.hh"
#include "G4ios.hh"

#include "BDSLaserCompton.hh"
#include "G4VProcess.hh"

class BDSLaserWireBuilder 
{
public: 
  BDSLaserWireBuilder();
  virtual ~BDSLaserWireBuilder();
  
public: 
  void Build();
  
protected:
  BDSLaserCompton theLaserWireProcess;
};
// 2003 by G.A Blair (after J.P. Wellisch)
#endif





