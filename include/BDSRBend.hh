#ifndef BDSRBEND_H
#define BDSRBEND_H 

#include "globals.hh"

#include "BDSMagnet.hh"

struct BDSBeamPipeInfo;
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
  ~BDSRBend(){;};

  /// Access all sensitive volumes belonging to this component including those
  /// of the custom beam pipe
  virtual std::vector<G4LogicalVolume*> GetAllSensitiveVolumes() const;

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

  /// beam pipe sections for before and after the central magnet body
  BDSBeamPipe* bpFirstBit;
  BDSBeamPipe* bpLastBit;
  
  virtual void Build();
  virtual void BuildBPFieldAndStepper();
  virtual void BuildBeampipe();
  virtual void BuildOuterVolume(); // override this method to change length used

  /// temporary function while old constructor still exists - used to avoid duplicating
  /// code in the mean time
  void CommonConstructor(G4double aLength);
};

#endif
