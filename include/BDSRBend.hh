#ifndef BDSRBEND_H
#define BDSRBEND_H 

#include "globals.hh"

#include "BDSMagnet.hh"

struct BDSBeamPipeInfo;
struct BDSMagnetOuterInfo;

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
  ~BDSRBend();

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

  /// orientation of shifts - depends on angle - calculations use absolute value of angle for safety
  G4int orientation;

  /// radius of magnet body
  G4double outerRadius;

  /// Extra start and finish piece of beam pipe
  BDSBeamPipe* bpFirstBit;
  BDSBeamPipe* bpLastBit;
  
  virtual void Build();

  /// Override so we can change the magnetic field length to just the length of the
  /// straight section in the rbend
  virtual void BuildBPFieldAndStepper();

  /// Override method so we can build several bits of beam pipe
  virtual void BuildBeampipe();

  /// Override BDSMagnet::BuildOuter() so we can get a different length of central section
  /// and magnet container.
  virtual void BuildOuter();

  /// Place the beam pipe and outer geometry in the overall container. If there's no outer
  /// geometry, then we don't need to place either as the beam pipe becomes the container.
  virtual void PlaceComponents();

  /// temporary function while old constructor still exists - used to avoid duplicating
  /// code in the mean time
  void CalculateLengths(G4double aLength);
};

#endif
