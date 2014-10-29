/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#ifndef BDSSpoiler_h
#define BDSSpoiler_h 1

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"
#include "G4LogicalVolume.hh"

class G4Material;
class G4VPhysicalVolume;

class BDSSpoiler :public BDSAcceleratorComponent
{
public:
  BDSSpoiler(G4String& aName, G4double aLength,G4double bpRad, 
	     G4double xAper, G4double yAper,G4Material* SpoilerMaterial);
  ~BDSSpoiler();

protected:

private:

  virtual void Build();
  void BuildInnerSpoiler();
  virtual void SetVisAttributes();

  // Geometrical objects:

  G4VPhysicalVolume* itsPhysiComp;
  G4VPhysicalVolume* itsPhysiComp2;
  G4LogicalVolume* itsSolidLogVol;
  G4LogicalVolume* itsInnerLogVol;
                    
private:
  G4Material* itsSpoilerMaterial;
};

#endif
