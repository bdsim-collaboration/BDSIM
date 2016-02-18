#ifndef BDSDRIFT_H
#define BDSDRIFT_H

#include "globals.hh"

#include "BDSAcceleratorComponent.hh"

class BDSBeamPipeInfo;

/**
 * @brief A section of beam pipe.
 * 
 * @author Laurie Nevay
 */

class BDSDrift: public BDSAcceleratorComponent
{
public:
  BDSDrift(G4String         name, 
	   G4double         length,
	   BDSBeamPipeInfo* beamPipeInfo,
	   G4int            precisionRegion = 0);
  virtual ~BDSDrift();

protected:
  void Build();

private:
  void BuildContainerLogicalVolume(){;}; // to fulfill bdsacceleratorcomponent requirements
};

#endif
