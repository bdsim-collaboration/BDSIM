#ifndef BDSDRIFT_H
#define BDSDRIFT_H

#include "globals.hh"

#include "BDSAcceleratorComponent.hh"

class BDSBeamPipe;

struct BDSBeamPipeInfo;

class BDSDrift: public BDSAcceleratorComponent
{
public:
  BDSDrift(G4String         name, 
	   G4double         length,
	   BDSBeamPipeInfo* beamPipeInfo,
	   G4int            precisionRegion = 0);
  ~BDSDrift();

  /// Access all sensitive volumes belonging to this component including those
  /// of the pipe object
  virtual std::vector<G4LogicalVolume*> GetAllSensitiveVolumes() const;

protected:
  void Build();


private:
  void BuildContainerLogicalVolume(){;}; // to fulfill bdsacceleratorcomponent requirements

  /// The piece of beam pipe for the drift.
  BDSBeamPipe* pipe;
};

#endif
