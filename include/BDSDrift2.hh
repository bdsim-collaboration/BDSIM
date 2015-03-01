#ifndef BDSDrift2_h
#define BDSDrift2_h 1

#include "globals.hh"
#include "BDSMultipole.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSBeamPipeInfo.hh"

#include "G4Material.hh"

class BDSDrift2 :public BDSAcceleratorComponent
{
public:
  BDSDrift2(G4String     nameIn, 
	    G4double     lengthIn,
	    beamPipeInfo beamPipeInfoIn);
  ~BDSDrift2(){;};

protected:
  void Build();

private:
  void BuildMarkerLogicalVolume(){;}; // to fulfill bdsacceleratorcomponent requirements

  beamPipeInfo itsBeamPipeInfo;

};

#endif
