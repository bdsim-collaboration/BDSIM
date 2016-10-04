#ifndef BDSDRIFT_H
#define BDSDRIFT_H

#include "globals.hh"

#include "BDSAcceleratorComponent.hh"

class BDSBeamPipeInfo;

/**
 * @brief A piece of vacuum beam pipe.
 * 
 * A simple class that represents a section of beam pipe or drift
 * in an accelerator. This simply uses the BDSBeamPipeFactory to 
 * construct the piece of beam pipe based on the input parameters.
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
  /// Void function to fulfill BDSAcceleratorComponent requirements.
  void BuildContainerLogicalVolume(){;}; 
};

#endif
