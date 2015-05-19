/* * BDSIM code.    Version 1.0
   * Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   * Last modified 24.7.2002
   * Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 


   Author of this code: John C. Carter, Royal Holloway, Univ. of London.
   Last modified 13.04.2005
*/

#ifdef USE_GDML

#ifndef BDSGeometryGDML_h
#define BDSGeometryGDML_h 1
#include "G4LogicalVolume.hh"
#include "BDSGeometry.hh"

class BDSGeometryGDML : public BDSGeometry{
public:
  BDSGeometryGDML(G4String GMDLfile);
  ~BDSGeometryGDML();
  virtual void Construct(G4LogicalVolume *marker);
private:
  G4String itsGDMLfile;
  G4LogicalVolume* itsMarkerVol;
};
#endif

#endif
