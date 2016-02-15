/* * BDSIM code.    Version 1.0
   * Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   * Last modified 24.7.2002
   * Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 


   Author of this code: John C. Carter, Royal Holloway, Univ. of London.
   Last modified 13.04.2005
*/

#ifdef USE_GDML

#ifndef BDSGEOMETRYGDML_H
#define BDSGEOMETRYGDML_H

#include "G4LogicalVolume.hh"

class BDSGeometryGDML
{
public:
  BDSGeometryGDML(G4String GMDLfile);
  ~BDSGeometryGDML();
  void Construct(G4LogicalVolume *marker);

  /// Access all logical volumes belonging to this component
  std::vector<G4LogicalVolume*> GetAllLogicalVolumes() const;  
  /// Access all sensitive volumes belonging to this component
  std::vector<G4LogicalVolume*> GetAllSensitiveVolumes() const;

private:
  G4String                      GDMLfile;
  G4LogicalVolume              *markerVol;
  G4LogicalVolume              *gdmlWorld;
  std::vector<G4LogicalVolume*> logicalVols; 
  std::vector<G4LogicalVolume*> sensitiveVols;
};
#endif

#endif
