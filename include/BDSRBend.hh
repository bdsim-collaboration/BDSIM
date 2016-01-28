#ifndef BDSRBEND_H
#define BDSRBEND_H 

#include "globals.hh"

#include "BDSMagnet.hh"

class  BDSBeamPipeInfo;
struct BDSMagnetOuterInfo;

class BDSBeamPipe;

class BDSRBend: public BDSMagnet
{
public:
  BDSRBend(G4String            name,
	   G4double            length,
	   G4double            bField,
	   G4double            bGrad,
	   G4double            angle,
	   BDSBeamPipeInfo*    beamPipeInfo,
	   BDSMagnetOuterInfo* magnetOuterInfo);

private:
  G4double bField;
  G4double bGrad;
  G4double magFieldLength;

  /// radius of magnet body
  G4double outerRadius;

  /// Override method from BDSAcceleratorComponent to detail construction process.
  virtual void Build();

  /// Required by BDSMagnet. We change the magnetic field length to just the length of the
  /// straight section in the rbend here.
  virtual void BuildBPFieldAndStepper();

};

#endif
