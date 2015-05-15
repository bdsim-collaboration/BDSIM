#ifndef BDSDrift_h
#define BDSDrift_h 1

#include "globals.hh"
#include "BDSMultipole.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSBeamPipeInfo.hh"

#include "G4Material.hh"

class BDSDrift :public BDSAcceleratorComponent
{
public:
  BDSDrift(G4String        nameIn, 
	   G4double        lengthIn,
	   BDSBeamPipeInfo beamPipeInfoIn);
  ~BDSDrift(){;};

protected:
  void Build();

private:
  void BuildMarkerLogicalVolume(){;}; // to fulfill bdsacceleratorcomponent requirements

  BDSBeamPipeInfo itsBeamPipeInfo;
};

#endif
