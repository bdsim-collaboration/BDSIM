/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 19.1.2003
   Copyright (c) 2003 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
/* BDSComponentOffset. Version 1.0
   Author: John C.Carter, Royal Holloway, Univ. of London
   Date: 10 Mar 2005
*/
#include "BDSGlobalConstants.hh" 
#include "BDSComponentOffset.hh"

//============================================================

BDSComponentOffset::BDSComponentOffset (G4String& aName,
					G4double aXoffset,
					G4double aYoffset):
  BDSAcceleratorComponent(aName,
			  0,0,0,0,
			  NULL,"","",0,
			  aXoffset,
			  aYoffset)
{}

BDSComponentOffset::~BDSComponentOffset()
{}
