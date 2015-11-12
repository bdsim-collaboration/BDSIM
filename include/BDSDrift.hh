#ifndef BDSDRIFT_H
#define BDSDRIFT_H

#include "globals.hh"

#include "BDSAcceleratorComponent.hh"

class BDSBeamPipeInfo;

class BDSDrift: public BDSAcceleratorComponent
{
public:
  BDSDrift(G4String         name, 
	   G4double         length,
	   G4double         e1,
	   G4double         e2,
	   BDSBeamPipeInfo* beamPipeInfo,
	   G4int            precisionRegion = 0);
  ~BDSDrift();

protected:
  void Build();

private:
  void BuildContainerLogicalVolume(){;}; // to fulfill bdsacceleratorcomponent requirements
  G4double e1;
  G4double e2;
};

#endif
