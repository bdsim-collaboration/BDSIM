#ifndef __BDSPCLTUBE_H
#define __BDSPCLTUBE_H

#include "G4VSolid.hh"

/** 
 * @brief PostCollisionLine Tube
 * 
 * @author Lawrence Deacon <l.deacon@cern.ch>>
 */

class BDSPCLTube
{
public:
  BDSPCLTube(G4double aperX, G4double aperYUp, G4double aperYDown, G4double aperDy, G4double thickness, G4double length, G4String name);
  ~BDSPCLTube();
  G4VSolid* GetSolid();
  void Build();
  void BuildHollow();
private:
  G4double itsSizeX;
  G4double itsSizeYUp;
  G4double itsSizeYDown;
  G4double itsSizeDy;
  G4double itsThickness;
  G4double itsLength;
  G4String itsName;

  G4VSolid* upper_solid;
  G4VSolid* middle_solid;
  G4VSolid* lower_solid;
  G4VSolid* temp_solid;
  G4VSolid* solid;


};

#endif
