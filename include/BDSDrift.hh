#ifndef BDSDRIFT_H
#define BDSDRIFT_H

#include "globals.hh"
#include "BDSMultipole.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSBeamPipeInfo.hh"

#include "G4Material.hh"

class BDSDrift: public BDSAcceleratorComponent
{
public:
  BDSDrift(G4String         nameIn, 
	   G4double         lengthIn,
	   BDSBeamPipeInfo* beamPipeInfoIn);
  ~BDSDrift(){;};

protected:
  void Build();

private:
  void BuildContainerLogicalVolume(){;}; // to fulfill bdsacceleratorcomponent requirements
};

#endif
