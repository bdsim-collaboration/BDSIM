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

  /// chord length of straight section (along main chord) [m]
  G4double straightSectionChord;

  /// length of little straight sections on either side of dipole [m]
  G4double straightSectionLength;

  /// x shift for magnet and beampipe from chord
  G4double magnetXShift;

  /// radius of magnet body
  G4double outerRadius;

  /// Override method from BDSAcceleratorComponent to detail construction process.
  virtual void Build();

  /// Required by BDSMagnet. We change the magnetic field length to just the length of the
  /// straight section in the rbend here.
  virtual void BuildBPFieldAndStepper();

  /// temporary function while old constructor still exists - used to avoid duplicating
  /// code in the mean time
  void CalculateLengths(G4double aLength);
};

#endif
